#pragma once

///解码和显示视频
struct AVPacket;
struct AVCodecParameters;
class XDecode;
#include <list>
#include <mutex>
#include <QThread>
#include "IVideoCall.h"
#include "XDecodeThread.h"
class XVideoThread :public XDecodeThread
{
public:
	// 解码pts， 如果接收到的解码数据pts >= seekpts    return true;    并且显示画面
	virtual bool RepaintPts(AVPacket *pkt, long long seekpts);
	//打开，不管成功与否都清理
	virtual bool Open(AVCodecParameters *para, IVideoCall *call, int width, int height);
/*	virtual void Push(AVPacket *pkt);*/
	void run();

	XVideoThread();
	virtual ~XVideoThread();
// 	//最大队列
// 	int maxList = 1000;
// 	bool isExit = false;
	// 同步时间由外部传入
	long long synpts = 0;    // 同步的pts

	void SetPause(bool isPause);
	bool isPause = false;
	
protected:
/*	std::list <AVPacket *> packs;*/
	std::mutex vmux;
/*	XDecode *decode = 0;*/
	IVideoCall *call = 0;


};

