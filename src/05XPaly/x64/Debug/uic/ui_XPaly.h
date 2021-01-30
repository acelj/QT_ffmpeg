/********************************************************************************
** Form generated from reading UI file 'XPaly.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPALY_H
#define UI_XPALY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "XSlider.h"
#include "XVideoWidget.h"

QT_BEGIN_NAMESPACE

class Ui_XPalyClass
{
public:
    XVideoWidget *video;
    QPushButton *openFile;
    XSlider *playPos;
    QPushButton *isPlay;

    void setupUi(QWidget *XPalyClass)
    {
        if (XPalyClass->objectName().isEmpty())
            XPalyClass->setObjectName(QStringLiteral("XPalyClass"));
        XPalyClass->resize(1280, 720);
        video = new XVideoWidget(XPalyClass);
        video->setObjectName(QStringLiteral("video"));
        video->setGeometry(QRect(0, 0, 1280, 720));
        openFile = new QPushButton(XPalyClass);
        openFile->setObjectName(QStringLiteral("openFile"));
        openFile->setGeometry(QRect(290, 620, 91, 31));
        playPos = new XSlider(XPalyClass);
        playPos->setObjectName(QStringLiteral("playPos"));
        playPos->setGeometry(QRect(30, 670, 1211, 31));
        playPos->setMaximum(999);
        playPos->setOrientation(Qt::Horizontal);
        isPlay = new QPushButton(XPalyClass);
        isPlay->setObjectName(QStringLiteral("isPlay"));
        isPlay->setGeometry(QRect(400, 620, 91, 31));

        retranslateUi(XPalyClass);

        QMetaObject::connectSlotsByName(XPalyClass);
    } // setupUi

    void retranslateUi(QWidget *XPalyClass)
    {
        XPalyClass->setWindowTitle(QApplication::translate("XPalyClass", "XPaly", Q_NULLPTR));
        openFile->setText(QApplication::translate("XPalyClass", "\346\211\223\345\274\200\346\226\207\344\273\266", Q_NULLPTR));
        isPlay->setText(QApplication::translate("XPalyClass", "\346\222\255\346\224\276", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class XPalyClass: public Ui_XPalyClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPALY_H
