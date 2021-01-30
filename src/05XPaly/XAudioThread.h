#pragma once
#include <QThread>
#include <mutex>
#include <list>
struct AVCodecParameters;
class XDecode;
class XAudioPlay;
class XResample;
/*class AVPacket;*/
#include "XDecodeThread.h"
class XAudioThread :public XDecodeThread
{
public:
	// 当前音频播放的pts
	long long pts = 0;
	//打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters *para, int sampleRate, int channels);   //把音频相关的业务放在这边

	//停止线程，清理资源
	virtual void Close();
	virtual void Clear();

/*	virtual void Push(AVPacket *pkt);*/
	void run();
	XAudioThread();
	virtual ~XAudioThread();

	void SetPause(bool isPause);
	bool isPause = false;

// 	//最大队列
// 	int maxList = 100;          // 44100 就是每秒44帧左右，100帧大概缓冲2秒多一点
// 	bool isExit = false;
protected:

/*	std::list <AVPacket *> packs;   // 链表做队列*/
	std::mutex amux;
/*	XDecode *decode = 0;*/
	XAudioPlay *ap = 0;
	XResample *res = 0;

};

