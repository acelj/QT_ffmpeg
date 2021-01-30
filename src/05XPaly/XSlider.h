#pragma once

#include <QObject>
#include <QSlider>
#include <QMouseEvent>

class XSlider : public QSlider
{
	Q_OBJECT

public:
	XSlider(QWidget *parent = NULL);
	~XSlider();

	void mousePressEvent(QMouseEvent *e);
};
