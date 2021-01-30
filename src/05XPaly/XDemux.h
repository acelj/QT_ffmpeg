#pragma once
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	// 打开媒体文件， 或者流媒体文件 rtmp, http,rstp 
	virtual bool Open(const char* url);

	//空间需要调用者释放 ，释放AVPacket对象空间，和数据空间 av_packet_free
	virtual AVPacket *Read();
	// 只读视频，音频丢弃，空间释放
	virtual AVPacket *ReadVideo();

	virtual bool IsAudio(AVPacket *pkt);

	//获取视频参数  返回的空间需要清理  avcodec_parameters_free
	virtual AVCodecParameters *CopyVPara();

	//获取音频参数  返回的空间需要清理 avcodec_parameters_free
	virtual AVCodecParameters *CopyAPara();

	//seek 位置 pos 0.0 ~1.0
	virtual bool Seek(double pos);

	//清空读取缓存
	virtual void Clear();
	virtual void Close();

	// 媒体总时长（ms）
	int totalMs = 0;
	int width = 0;
	int height = 0;

	int sampleRate = 0;
	int channels = 0;


protected:
	std::mutex mux;
	//解封装上下文
	AVFormatContext* ic = nullptr;
	// 音视频索引，读取区别音视频
	int videoStream = 0;
	int audioStream = 1;

};

