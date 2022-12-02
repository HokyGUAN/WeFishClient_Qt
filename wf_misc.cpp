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

WF_HeadIcon::WF_HeadIcon(QWidget * parent) : QLabel(parent)
{
    menu = new QMenu("个人信息");
    change_icon = new QAction("更换头像", this);
    getinfo = new QAction("获取信息", this);
    menu->addAction(change_icon);
    menu->addSeparator();
    menu->addAction(getinfo);
    menu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint);
    //menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setStyleSheet(" QMenu {border-radius:5px;font-family:'Microsoft Yahei';font-size:14px;rgb(0,20,255);}"
                        " QMenu::item {height:30px; width:100px;padding-left:20px;border: 1px solid none;}"
                        " QMenu::item:selected {background-color:rgb(0,120,215);padding-left:20px;border: 1px solid rgb(65,173,255);}");

   connect(change_icon, SIGNAL(triggered()), this, SLOT(ChangeIcon()));

}

void WF_HeadIcon::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        menu->exec(event->globalPos());
    }
}

void WF_HeadIcon::ChangeIcon()
{
    Image_Path = QFileDialog::getOpenFileName(this,"Choose Icon","",
                                                  "*Image Files(*.jpg;*.png;*.bmp);;All(*.*)");
    QPixmap icon(Image_Path);
    icon.save(WF_DIR + "\\Self.jpg");

    this->setPixmap(icon.scaled(40, 40));

    emit ChangedIcon(Image_Path);
}

void WF_HeadIcon::enterEvent(QEvent *)
{
    this->setToolTip("个人信息");
}

QString WF_HeadIcon::Get_Image()
{
    return Image_Path;
}
