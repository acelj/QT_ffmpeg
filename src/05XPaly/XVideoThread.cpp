#include "XVideoThread.h"
#include "XDecode.h"
#include <iostream>
using namespace std;

//打开，不管成功与否都清理
bool XVideoThread::Open(AVCodecParameters *para, IVideoCall *call, int width, int height)
{
	if (!para)return false;

	Clear();

	vmux.lock();
	synpts = 0; 
	//初始化显示窗口
	this->call = call;
	if (call)
	{
		call->Init(width, height);
	}
	vmux.unlock();
	//打开解码器

	int re = true;
	if (!decode->Open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		re = false;
	}
	cout << "XAudioThread::Open :" << re << endl;
	return re;
}

// void XVideoThread::Push(AVPacket *pkt)
// {
// 	if (!pkt)return;
// 	//阻塞
// 	while (!isExit)
// 	{
// 		mux.lock();
// 		if (packs.size() < maxList)
// 		{
// 			packs.push_back(pkt);
// 			mux.unlock();
// 			break;
// 		}
// 		mux.unlock();
// 		msleep(1);
// 	}
// }

void XVideoThread::SetPause(bool isPause)
{
	vmux.lock();
	this->isPause = isPause;

	vmux.unlock();

}

void XVideoThread::run()
{
	while (!isExit)
	{
		vmux.lock();

		if (this->isPause)
		{
			vmux.unlock();
			msleep(5);
			continue;
		}

// 		//没有数据
// 		if (packs.empty() || !decode)
// 		{
// 			mux.unlock();
// 			msleep(1);
// 			continue;
// 		}

		// 音视频同步
		if (synpts > 0 && synpts < decode->pts)   // 视频快了，就需要等  , 没有音频不需要等待
		{
			vmux.unlock();
			msleep(1);
			continue;
		}

// 		AVPacket *pkt = packs.front();
// 		packs.pop_front();
		AVPacket *pkt = Pop();
		bool re = decode->Send(pkt);
		if (!re)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
		while (!isExit)
		{
			AVFrame * frame = decode->Recv();
			if (!frame) break;
			//显示视频
			if (call)
			{
				call->Repaint(frame);
			}

		}
		vmux.unlock();
	}
}

// 解码pts， 如果接收到的解码数据pts >= seekpts    return true;    并且显示画面
bool XVideoThread::RepaintPts(AVPacket *pkt, long long seekpts)
{
	vmux.lock();
	bool re = decode->Send(pkt);
	// av_packet_free(&pkt);
	if (!re)
	{
		vmux.unlock();
		return true;  // 表示结束解码
	}
		AVFrame *frame = decode->Recv();
		if (!frame)
		{
			vmux.unlock();
			return false;
		}
	// 如果到达位置，就显示
	if (decode->pts >= seekpts)
	{
		// 等于关键帧， 需要显示出去
		if(call)
			call->Repaint(frame);
		vmux.unlock();
		return true;
	}
	XFreeFrame(&frame);
	vmux.unlock();

	return false;
}

XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
// 	//等待线程退出
// 	isExit = true;
// 	wait();
}
