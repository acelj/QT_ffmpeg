#pragma once

#include <QtWidgets/QWidget>
#include "ui_XPaly.h"

class XPaly : public QWidget
{
    Q_OBJECT

public:
    XPaly(QWidget *parent = Q_NULLPTR);
	~XPaly();

	// 定时器， 滑动条显示(重载)
	void timerEvent(QTimerEvent *e);
	
	// 窗口尺寸变化
	void  resizeEvent(QResizeEvent *e);

	// 双击全屏
	void mouseDoubleClickEvent(QMouseEvent *e);

	void SetPause(bool isPause);
private:
    Ui::XPalyClass ui;

	bool isSliderPress = false;
};
