
#include "XDemux.h"
#include <iostream>

using namespace std;
// 要引用c语言函数
extern "C" {
#include<libavformat/avformat.h>
}
#pragma comment(lib,"avformat.lib")   // 这样避免每次修改x86 x64,debug,release 都去修改配置属性
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")

static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

XDemux::XDemux()
{
	static bool isFirst = true;
	static std::mutex dumex;    // 添加一个互斥变量
	dumex.lock();
	if (isFirst)
	{
		// 初始化封装库
		av_register_all();

		// 初始化网络 （可以打开rtsp,rtmp，http 协议的流媒体视频）
		avformat_network_init();
		isFirst = false;
	}
	dumex.unlock();
	

}

XDemux::~XDemux()
{
}

//清空读取缓存
void XDemux::Clear()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	//清理读取缓冲
	avformat_flush(ic);
	mux.unlock();
}

void XDemux::Close()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	avformat_close_input(&ic);
	//媒体总时长（毫秒）
	totalMs = 0;
	mux.unlock();
}

// seek 位置 pos 0.0 ~1.0
bool XDemux::Seek(double pos)
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return false;
	}
	//清理读取缓冲
	avformat_flush(ic);

	long long seekPos = 0;
	seekPos = ic->streams[videoStream]->duration * pos;
	int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	mux.unlock();
	if (re < 0) return false;
	return true;
}

bool XDemux::Open(const char * url)
{
	Close();
	// 参数设置
	AVDictionary* opts = nullptr;
	// 设置rtsp 流以tcp协议打开
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	// 网络设置延时
	av_dict_set(&opts, "max_delay", "500", 0);

	// 解封装上下文
	// AVFormatContext* ic = nullptr;
	// const char* url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";   // 路径
	mux.lock();
	int re = avformat_open_input(
		&ic,
		url,
		0,   // 自动选择解封装器  
		&opts   //参数设置，比如rtsp的延时时间
	);

	if (re != 0)
	{
		mux.unlock();
		char buf[1024] = { 0 };
		cout << "failed! = " << endl;
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << "failed!: " << buf << endl;
		return false;
	}
	cout << "open " << url << " success! " << endl;

	// 获取流信息
	re = avformat_find_stream_info(ic, 0);

	// 总时长 ms
	this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;

	// 打印视频流详细信息
	av_dump_format(ic, 0, url, 0);

	// 获取视频流
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

	AVStream *as = ic->streams[videoStream];
	width = as->codecpar->width;
	height = as->codecpar->height;

	cout << "================================================================" << endl;
	cout << videoStream << "视频信息" << endl;  // 0
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "width = " << as->codecpar->width << endl;  // 可能没有需要注意
	cout << "height = " << as->codecpar->height << endl;
	// 帧率 fps
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;

	//////////////////////////////////////////////////////////////////////
	// 
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	as = ic->streams[audioStream];
	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->channels;

	cout << "================================================================" << endl;
	cout << audioStream << "音频信息" << endl;  // 1
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	// AVSampleFormat;
	cout << "channels = " << as->codecpar->channels << endl;
	// 一帧数据？？ 存放的是一定量的样本数 ， 音频中fps = 0；
	// 单通道的样本数  1024
	cout << "frame_size = " << as->codecpar->frame_size << endl;
	mux.unlock();

	return true;
}

// 只读视频，音频丢弃，空间释放
AVPacket * XDemux::ReadVideo()
{
	mux.lock();
	if (!ic)   // 容错
	{
		mux.unlock();
		return 0;
	}

	mux.unlock();

	AVPacket *pkt = nullptr;
	// 防止阻塞
	for (int i = 0; i < 20; i++)
	{
		pkt = Read();
		if (!pkt) break;
		if (pkt->stream_index == videoStream)
		{
			break;
		}
		av_packet_free(&pkt);
	}
	return pkt;
}

//空间需要调用者释放 ，释放AVPacket对象空间，和数据空间 av_packet_free
AVPacket * XDemux::Read()
{
	mux.lock();
	if (!ic) //容错
	{
		mux.unlock();
		return 0;
	}
	AVPacket *pkt = av_packet_alloc();
	//读取一帧，并分配空间
	int re = av_read_frame(ic, pkt);
	if (re != 0)
	{
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//pts转换为毫秒
	pkt->pts = pkt->pts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	pkt->dts = pkt->dts*(1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	mux.unlock();
	// cout << "pkt->pts = " << pkt->pts << " " << flush;
	return pkt;
}

bool XDemux::IsAudio(AVPacket * pkt)
{
	if (!pkt) return false;
	if (pkt->stream_index == videoStream) 
		return false;
	return true;
}

//获取视频参数  返回的空间需要清理  avcodec_parameters_free
AVCodecParameters * XDemux::CopyVPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return NULL;
	}
	AVCodecParameters *pa = avcodec_parameters_alloc();  // 申请空间
	avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
	mux.unlock();
	return pa;
}

//获取音频参数  返回的空间需要清理 avcodec_parameters_free
AVCodecParameters * XDemux::CopyAPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return NULL;
	}
	AVCodecParameters *pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
	mux.unlock();
	return pa;
}
