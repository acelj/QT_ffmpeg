#include "XPaly.h"
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "XDemuxThread.h"
// static XDemuxThread *dt = 0;   // 指针的好处= 构造函数的调用不会影响初始化
static XDemuxThread dt;
XPaly::XPaly(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
	
	dt.Start();

	connect(ui.openFile, &QPushButton::clicked, this, [this] {
		// 选择文件
		QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频"));
		if (name.isEmpty()) return;
		this->setWindowTitle(name);
		if (!dt.Open(name.toLocal8Bit(), ui.video))   // 打开失败
		{
			QMessageBox::information(0, "error", "open file failed!");
			return;
		}
		
		SetPause(dt.isPause);
		// qDebug() << name;
	});

	connect(ui.isPlay, &QPushButton::clicked, this, [this] {
		bool isPause = !dt.isPause;
		SetPause(isPause);
		dt.SetPause(isPause);
	});

	connect(ui.playPos, &QSlider::sliderPressed, this, [this] {
		isSliderPress = true;

	});
	connect(ui.playPos, &QSlider::sliderReleased, this, [this] {    // 在松开的时候做seek , 这能保证计算小一点
		isSliderPress = false;
		double pos = 0.0;
		pos = (double)ui.playPos->value() / (double)ui.playPos->maximum();
		dt.Seek(pos);
	});

	startTimer(40);    // 40ms 的定时器， 一秒25帧

}
// 定时器， 滑动条显示
void XPaly::timerEvent(QTimerEvent *e)
{
	if (isSliderPress) return;    // 按住滑动条， 直接返回，不在刷新界面

	// 一秒25 次， 是很流畅的
	long long total = dt.totalMs;   // 把值存起来，以防别的线程更改
	if (total > 0)
	{
		double pos = (double)dt.pts / (double)total;
		int v = ui.playPos->maximum() * pos;
		ui.playPos->setValue(v);
	}
}

// 窗口尺寸变化
void XPaly::resizeEvent(QResizeEvent *e)
{
	ui.playPos->move(50, this->height() - 100);
	ui.playPos->resize(this->width() - 100, ui.playPos->height());
	ui.openFile->move(100, this->height() - 150);
	ui.video->resize(this->size());
	ui.isPlay->move(ui.openFile->x() + ui.openFile->width() + 10, ui.openFile->y());
}

void XPaly::mouseDoubleClickEvent(QMouseEvent * e)
{
	if (isFullScreen())
		this->showNormal();
	else
	{
		this->showFullScreen();
	}
}
void XPaly::SetPause(bool isPause)
{
	if (isPause)
		ui.isPlay->setText(QString::fromLocal8Bit("播 放"));
	else
		ui.isPlay->setText(QString::fromLocal8Bit("暂 停"));
}
XPaly::~XPaly()
{
	dt.Close();
}
