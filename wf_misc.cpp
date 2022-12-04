/**********************************
 *
 *           WeFish Misc
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_misc.h"
#include "QMouseEvent"
#include <QFileDialog>
#include <QDebug>

WF_ImageHandler::WF_ImageHandler()
{
}

WF_ImageHandler::~WF_ImageHandler()
{
}

QByteArray WF_ImageHandler::ImageToBase64(QString ImgPath)
{
    QImage image(ImgPath);
    QPixmap pixmap = QPixmap::fromImage(image);
    QByteArray byteArray;
    QBuffer buf(&byteArray);

    pixmap.save(&buf, "jpg", 20);
    QByteArray hexed = byteArray.toBase64();
    buf.close();

    return hexed;
}

QPixmap WF_ImageHandler::Base64ToImage(QByteArray byteArray) {
    QByteArray Ret_byteArray = QByteArray::fromBase64(byteArray);
    QBuffer buffer(&Ret_byteArray);
    buffer.open(QIODevice::WriteOnly);
    QPixmap image;
    image.loadFromData(Ret_byteArray);

    return image;
}

WF_Setting::WF_Setting(QWidget * parent) : QLabel(parent)
{
    this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
    menu = new QMenu("个人信息");
    change_icon = new QAction("更换头像", this);
    setting = new QAction("设置", this);
    menu->addAction(change_icon);
    //menu->addSeparator();
    menu->addAction(setting);
    menu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    //menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setStyleSheet("QMenu {"
                        "width:140px;"
                        "height:90px;"
                        "font-size:14px;"
                        "background-color: rgb(41, 41, 41);"
                        "color: rgb(140,140,140);"
                        "border-radius: 0px; }"
                        "QMenu::item {"
                        "width:140px;"
                        "height:45px;"
                        "padding-left:10px;"
                        "margin:0px;"
                        "background-color: transparent;}"
                        "QMenu::item:selected {"
                        "background-color: rgb(48, 49, 51);}");

   connect(change_icon, SIGNAL(triggered()), this, SLOT(ChangeIcon()));
}

void WF_Setting::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
        QPoint gp = event->globalPos();
        QPoint pos = mapToGlobal(this->pos());
        int Xoffset = gp.rx() - pos.rx();
        menu->exec(QPoint(gp.rx() - Xoffset + 30, gp.ry() - 70));
    }
}

void WF_Setting::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
}

void WF_Setting::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setToolTip("更多");
    this->setStyleSheet("border-image:url(:/Res/whiteSetting.png);outset;");
}

void WF_Setting::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
}

void WF_Setting::ChangeIcon()
{
    Image_Path = QFileDialog::getOpenFileName(this,"Choose Icon","",
                                                  "*Image Files(*.jpg;*.png;*.bmp);;All(*.*)");
    QPixmap icon(Image_Path);
    icon.save(WF_DIR + "\\Self.jpg");

    emit ChangedIcon(Image_Path);
}
