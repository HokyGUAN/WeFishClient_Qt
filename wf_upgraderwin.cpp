/**********************************
 *
 *     WeFish Process Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/

#include "wf_upgraderwin.h"


WF_UpgraderWin::WF_UpgraderWin(QWidget *parent, WF_FileSocket* fileSocket)
    : WF_BaseWin(nullptr)
    , FileSocket(fileSocket)
{
    Q_UNUSED(parent)
    this->setMinimumSize(500, 270);
    this->setMaximumSize(500, 270);
    this->CloseButton->setGeometry(467, 0, 33, 24);

    Bar = new QProgressBar(this);
    Bar->setRange(0, 100);
    Bar->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    Bar->setStyleSheet("QProgressBar{background:rgb(255,255,255); border-radius:4px;} QProgressBar::chunk{background:rgb(7, 193, 96);}");
    Bar->setGeometry(35, 140, 430, 28);
    Bar->setFormat("%p%");

    UpgradedButton = new QPushButton(this);
    UpgradedButton->setGeometry(150, 215, 180, 35);
    UpgradedButton->setFont(QFont("Microsoft Yahei", 11));
    UpgradedButton->setText("更新中");
    UpgradedButton->setStyleSheet("background: rgb(7, 193, 96); border-radius:4px; color:rgb(255, 255, 255);outset;");
    UpgradedButton->setEnabled(false);

    connect(FileSocket, SIGNAL(eUpgradeProgressChanged(int)), this, SLOT(sProgressUpdate(int)));
    connect(UpgradedButton, &QPushButton::clicked, this, &WF_UpgraderWin::doUpgradeComplete);
}

void WF_UpgraderWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(0, 0, 500, 300, 0, 0);
    painter.setPen(QPen(Qt::gray));
    painter.setFont(QFont("Microsoft Yahei", 10));
    painter.drawText(QRect(8, 7, 200, 20), "WeFish Upgrader");
    painter.setFont(QFont("Microsoft Yahei", 15));
    painter.drawText(QRect(42, 60, 400, 30), "New Version Upgrading......");
}

void WF_UpgraderWin::doUpgradeRequest()
{
    FileSocket->doUpgradeRequest();
}

void WF_UpgraderWin::sProgressUpdate(int percent)
{
    Bar->setValue(percent);
    if (percent == 100) {
        UpgradedButton->setText("更新完成");
        UpgradedButton->setEnabled(true);
    }
}

void WF_UpgraderWin::doUpgradeComplete()
{
    qDebug() << "UpgradeComplete";
    this->close();
}
