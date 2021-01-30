#include "XPaly.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>
#include "XDemux.h"
#include "XDecode.h"
#include "XResample.h"
#include "XAudioPlay.h"
#include "XAudioThread.h"
#include "XVideoThread.h"

using namespace std;

class TestThread : public QThread
{
public:
	XAudioThread at;
	XVideoThread vt;
	void Init()
	{
		char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";   // 湖南卫视
		url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";
		cout << "demux.Open = " << demux.Open(url) << endl;
// 		demux.Read();
// 		demux.Clear();
// 		demux.Close();
		//cout << "demux.Open = " << demux.Open(url) << endl;
		//cout << "demux.CopyVPara = " << demux.CopyVPara() << endl;
		//cout << "demux.CopyAPara = " << demux.CopyAPara() << endl;
		//cout << "seek=" << demux.Seek(0.9) << endl;

		// cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
		//cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;

		//cout << "resample.Open = " << resample.Open(demux.CopyAPara()) << endl;

		//XAudioPlay::Get()->channels = demux.channels;
		//XAudioPlay::Get()->sampleRate = demux.sampleRate;
		//cout << "XAudioPlay::Get()->Open() = " << XAudioPlay::Get()->Open() << endl;
		cout << "at.Open = " << at.Open(demux.CopyAPara(), demux.sampleRate, demux.channels);
		vt.Open(demux.CopyVPara(), video, demux.width, demux.height);
		at.start();
		vt.start();
	}
	void run()
	{
		unsigned char *pcm = new unsigned char[1024 * 1024];
		for (;;)
		{
			AVPacket *pkt = demux.Read();
			if (demux.IsAudio(pkt))
			{
				at.Push(pkt);
				//adecode.Send(pkt);
				//AVFrame *frame = adecode.Recv();
				//// cout << "Resample:" << resample.Resample(frame, pcm) << " " << endl;
				//// 测试播放声音
				//int len = resample.Resample(frame, pcm);
				//cout << "Resample:" << len << " ";
				//while (len > 0)
				//{
				//	if (XAudioPlay::Get()->GetFree() >= len)
				//	{
				//		XAudioPlay::Get()->Write(pcm, len);
				//		break;
				//	}
				//	msleep(1);
				//}

				//cout << "Audio:" << frame << endl;
			}
			else
			{
				vt.Push(pkt);
				/*vdecode.Send(pkt);
				AVFrame *frame = vdecode.Recv();
				video->Repaint(frame);*/
				// msleep(40);
				//cout << "Video:" << frame << endl;
			}
			if (!pkt)
			{
				demux.Seek(0);
			//	break;
			}
		}
	}

	XDemux demux;
	//XDecode vdecode;
	//XDecode adecode;
	//XResample resample;
	XVideoWidget *video = 0;
};

// 新建项目时候不选择包含解决方案目录的前提下
/// 1. 属性->常规->输出目录					..\..\bin\win64
/// 2. 属性->调试->工作目录					..\..\bin\win64	
/// 3. 属性->C/C++->常规->附加包含目录		..\..\include
/// 4. 属性->链接器->常规->附加库目录		..\..\lib\win64
/// 5. qt程序调试成控制台：属性->链接器->系统->子系统->console
/// 6. 在属性->链接器->所有选项->映像具有安全异常处理程序  修改成 否SAFESEH:NO

#include "XDemuxThread.h"
int main(int argc, char *argv[])
{

	//XDemux demux;
	//char* url = "rtmp://58.200.131.2:1935/livetv/hunantv";   // 湖南卫视
	//url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";
	//cout << "demux.Open = " << demux.Open(url) << endl;
	//demux.Read();
	//demux.Clear();
	//demux.Close();
	//cout << "demux.Open = " << demux.Open(url) << endl;
	//cout << "demux.CopyVPara = " << demux.CopyVPara() << endl;
	//cout << "demux.CopyAPara = " << demux.CopyAPara() << endl;
	//cout << "seek=" << demux.Seek(0.9) << endl;

	////////////////////////////////////////////////////////////
	// 解码测试
	//XDecode vdecode;
	//cout << "vdecode.Open() = " << vdecode.Open(demux.CopyVPara()) << endl;
	////vdecode.Clear();
	////vdecode.Close();
	//XDecode adecode;
	//cout << "adecode.Open() = " << adecode.Open(demux.CopyAPara()) << endl;


	//for (;;)
	//{
	//	AVPacket *pkt = demux.Read();
	//	if (demux.IsAudio(pkt))
	//	{
	//		adecode.Send(pkt);
	//		AVFrame *frame = adecode.Recv();
	//		//cout << "Audio:" << frame << endl;
	//	}
	//	else
	//	{
	//		vdecode.Send(pkt);
	//		AVFrame *frame = vdecode.Recv();
	//		//cout << "Video:" << frame << endl;
	//	}
	//	if (!pkt)break;
	//}

	//TestThread tt;
	

    QApplication a(argc, argv);
    XPaly w;
    w.show();

	// 初始化gl窗口
	// w.ui.video->Init(tt.demux.width, tt.demux.height);
	//tt.video = w.ui.video;    // 窗口初始化完成后启动线程
	//tt.Init();
	//tt.start();
	//XDemuxThread dt;
	//char* url = "rtmp://116.213.200.53/tslsChannelLive/PCG0DuD/live";   // 湖南卫视
	//url = "D:\\02C++\\08FFMPEG\\src\\v720p.mp4";
	//dt.Open("D:\\02C++\\08FFMPEG\\src\\v720p.mp4", w.ui.video);
	//dt.Start();    // 这里是重写的Start函数

    return a.exec();
}
