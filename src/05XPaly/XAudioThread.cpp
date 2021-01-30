#include "XAudioThread.h"
#include "XDecode.h"
#include "XAudioPlay.h"
#include "XResample.h"
#include <iostream>
using namespace std;

/// 在继承中的基类完成了
// void XAudioThread::Push(AVPacket *pkt)
// {
// 	if (!pkt)return;
// 	//阻塞，因为这里会经常满的
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

void XAudioThread::Clear()
{
	XDecodeThread::Clear();
	mux.lock();
	if (ap) ap->Clear();
	mux.unlock();
}

//停止线程，清理资源
void XAudioThread::Close()
{
	XDecodeThread::Close();
	if (res)
	{
		res->Close();
		amux.lock();
		delete res;
		res = NULL;
		amux.unlock();
	}
	if (ap)
	{
		ap->Close();
		amux.lock();
		ap = NULL;
		amux.unlock();
	}
}

bool XAudioThread::Open(AVCodecParameters *para, int sampleRate, int channels)
{
	if (!para)return false;
	Clear();
	amux.lock();
	pts = 0;
	bool re = true;
	if (!res->Open(para, false))
	{
		cout << "XResample open failed!" << endl;
		re = false;
	}
	ap->sampleRate = sampleRate;
	ap->channels = channels;
	if (!ap->Open())
	{
		re = false;
		cout << "XAudioPlay open failed!" << endl;
	}
	if (!decode->Open(para))
	{
		cout << "audio XDecode open failed!" << endl;
		re = false;
	}
	amux.unlock();
	cout << "XAudioThread::Open :" << re << endl;
	return re;
}

void XAudioThread::SetPause(bool isPause)
{
	//amux.lock();    //在音频下面加上isPause 之后 ，需要将这个锁去掉
	this->isPause = isPause;
	// 另外需要和ap 保持一致
	if (ap)
		ap->SetPause(isPause);
	//amux.unlock();
}
void XAudioThread::run()  // 这里没有 运行
{
	unsigned char *pcm = new unsigned char[1024 * 1024 * 100];
	while (!isExit)
	{
		amux.lock();
		if (this->isPause)
		{
			amux.unlock();
			msleep(5);
			continue;
		}
// 		//没有数据
// 		if (packs.empty() || !decode || !res || !ap)
// 		{
// 			mux.unlock();
// 			msleep(1);
// 			continue;
// 		}

// 		AVPacket *pkt = packs.front();
// 		packs.pop_front();   // 尾部插入， 头部取出， 头部删除
		AVPacket *pkt = Pop();   // 代替上面
		bool re = decode->Send(pkt);
		if (!re)
		{
			amux.unlock();
			msleep(1);
			continue;
		}
		//一次send 多次recv
		while (!isExit)
		{
			AVFrame * frame = decode->Recv();
			if (!frame) break;
			
			// 减去缓冲中为播放的时间
			pts = decode->pts - ap->Get()->GetNoPlayMs();
			//cout << "audio pts = " << pts << endl;
			
			//重采样 
			int size = res->Resample(frame, pcm);
			//播放音频
			while (!isExit)
			{
				if (size <= 0)break;
				//缓冲未播完，空间不够
				if (ap->GetFree() < size || isPause)
				{
					msleep(1);
					continue;
				}
				ap->Write(pcm, size);
				break;
			}
		}
		amux.unlock();
	}
	delete pcm;
}

XAudioThread::XAudioThread()
{
	if (!res) res = new XResample();
	if (!ap) ap = XAudioPlay::Get();
}


XAudioThread::~XAudioThread()
{
	//等待线程退出
	isExit = true;
	wait();
}
