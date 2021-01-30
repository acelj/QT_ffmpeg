#include "XDemuxThread.h"
#include "XDemux.h"
#include "XVideoThread.h"
#include "XAudioThread.h"
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}
#include "XDecode.h"
using namespace std;
void XDemuxThread::SetPause(bool isPause)
{
	mux.lock();
	this->isPause = isPause;    // 它自己暂停 还要把at， vt 暂停
	if (at) at->SetPause(isPause);
	if (vt) vt->SetPause(isPause);
	mux.unlock();
}


void XDemuxThread::run()
{
	while (!isExit)
	{
		mux.lock();
		if (isPause)
		{
			mux.unlock();
			msleep(5);
			continue;

		}
		if (!demux)
		{
			mux.unlock();
			msleep(1);
			continue;
		}

		if (vt && at)    // 这里就是音视频同步
		{
			pts = at->pts;
			vt->synpts = at->pts;
		}

		AVPacket *pkt = demux->Read();
		if (!pkt)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//判断数据是音频
		if (demux->IsAudio(pkt))
		{
			if (at)at->Push(pkt);  // yuv rgb 视频格式中是没有音频的
		}
		else //视频
		{
			if (vt)vt->Push(pkt);
		}

		mux.unlock();
		msleep(1);    //读取速度太快，导致不成功   ===============================
	}
}


bool XDemuxThread::Open(const char *url, IVideoCall *call)
{
	if (url == 0 || url[0] == '\0')   //判断url是否为空
		return false;

	mux.lock();
	//if (!demux) demux = new XDemux();
	//if (!vt) vt = new XVideoThread();
	//if (!at) at = new XAudioThread();

	//打开解封装
	bool re = demux->Open(url);
	if (!re)
	{
		cout << "demux->Open(url) failed!" << endl;
		return false;
	}
	//打开视频解码器和处理线程
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		re = false;
		cout << "vt->Open failed!" << endl;
	}
	//打开音频解码器和处理线程
	if (!at->Open(demux->CopyAPara(), demux->sampleRate, demux->channels))
	{
		re = false;
		cout << "at->Open failed!" << endl;
	}
	totalMs = demux->totalMs;
	mux.unlock();
	cout << "XDemuxThread::Open " << re << endl;
	return re;
}
//关闭线程清理资源
void XDemuxThread::Close()
{
	isExit = true;
	wait();				// 等待当前线程的退出
	if (vt) vt->Close();
	if (at) at->Close();
	mux.lock();
	delete vt;
	delete at;
	vt = NULL;
	at = NULL;
	mux.unlock();
}

void XDemuxThread::Clear()
{
	mux.lock();
	if(demux) demux->Clear();
	if (vt) vt->Clear();
	if (at) at->Clear();
	mux.unlock();
}

//void XDemuxThread::Seek(double pos)
//{
//	Clear();
//	mux.lock();
//	if (demux)
//		demux->Seek(pos);
//	mux.unlock();
//}

void XDemuxThread::Seek(double pos)
{
	// 清理缓存
	Clear();
	
	mux.lock();
	bool status = this->isPause;
	mux.unlock();
	// 暂停
	SetPause(true);
	mux.lock();
	if(demux)
		demux->Seek(pos);    // 这里需要清空缓冲
	// 实际要显示的位置pos
	long long seekPos = pos * demux->totalMs;
	while (!isExit)
	{
// 		AVPacket *pkt = demux->Read();
// 		if (!pkt) break;
// 		if (pkt->stream_index == demux->audioStream)
// 		{
// 			// 是音频数据就丢弃
// 			av_packet_free(&pkt);
// 			continue;
// 		}
		AVPacket *pkt = demux->ReadVideo();
		if (!pkt) break;
		// 如果解码到seekPts
		if (vt->RepaintPts(pkt, seekPos))
		{

			break;
		}

		//bool re = vt->decode->Send(pkt);
		//// av_packet_free(&pkt);
		//if (!re) break;  // 失败
		//AVFrame *frame = vt->decode->Recv();
		//if (!frame) continue;
		//// 如果到达位置，就显示
		//if (frame->pts >= seekPos)
		//{
		//	this->pts = frame->pts;
		//	// 等于关键帧， 需要显示出去

		//	vt->call->Repaint(frame);
		//	break;
		//}
		//av_frame_free(&frame);
	}
	mux.unlock();

	// 播放 seek 是非暂停状态
	if (!status)
		SetPause(false);
}

//启动所有线程
void XDemuxThread::Start()
{
	mux.lock();
	if (!demux) demux = new XDemux();
	if (!vt) vt = new XVideoThread();
	if (!at) at = new XAudioThread();
	//启动当前线程
	QThread::start();
	if (vt)vt->start();
	if (at)at->start();
	mux.unlock();
}
XDemuxThread::XDemuxThread()
{
}


XDemuxThread::~XDemuxThread()
{
	isExit = true;
	wait();
}
