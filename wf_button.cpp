/**********************************
 *
 *        WeFish Button
 *
 * Author: Kim.He@tymphany.com
 * date  : 11/24 2022
 *
***********************************/
#include "wf_button.h"
#include <QMenu>
#include <QDebug>
#include <QFileDialog>

WF_Button::WF_Button(QWidget *parent, QString name, bool menu)
    : QPushButton(parent)
    , menu_flag(menu)
{
    this->setText(name);
    QFont font = QFont("Microsoft Yahei");
    font.setPixelSize(13);
    this->setFont(font);
    this->setStyleSheet("QPushButton{background-color:rgb(248, 248, 248);border-radius:2px;color:rgb(7, 193, 96);outset;}");

    if (menu_flag) {
        btn_menu = new QMenu(this);
        actionIpSet       = new QAction("发送截图", this);
        actionAuthorize   = new QAction("转为私聊", this);
        btn_menu->addAction(actionIpSet);
        btn_menu->addAction(actionAuthorize);
        btn_menu->addSeparator();

        btn_menu->setWindowFlags(btn_menu->windowFlags() | Qt::FramelessWindowHint);
        btn_menu->setAttribute(Qt::WA_TranslucentBackground);
        btn_menu->setStyleSheet(" QMenu {border-radius:5px;font-family:'Microsoft Yahei';font-size:14px;rgb(0,20,255);}"
                            " QMenu::item {height:30px; width:100px;padding-left:20px;border: 1px solid none;}"
                            " QMenu::item:selected {background-color:rgb(0,120,215);padding-left:20px;border: 1px solid rgb(65,173,255);}");
    }
}

void WF_Button::mousePressEvent(QMouseEvent *event)
{
    this->setStyleSheet("QPushButton{background-color:rgb(210, 210, 210);border-radius:2px;color:rgb(7, 193, 96);outset;}");
    emit clicked();
    if(menu_flag == 1)
        btn_menu->exec(event->globalPos());
}

void WF_Button::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QPushButton{background-color:rgb(248, 248, 248);border-radius:2px;color:rgb(7, 193, 96);outset;}");
}

void WF_Button::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QPushButton{background-color:rgb(210, 210, 210);border-radius:2px;color:rgb(7, 193, 96);outset;}");
}

void WF_Button::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QPushButton{background-color:rgb(248, 248, 248);border-radius:2px;color:rgb(7, 193, 96);outset;}");
}

WF_CloseButton::WF_CloseButton(QWidget * parent, int x, int y) : QToolButton(parent)
{

    this->setStyleSheet("QToolButton{border-image:url(:/Res/whiteClose.png);outset;}");
    this->setFixedSize(x,y);
}

WF_CloseButton::~WF_CloseButton()
{
}

void WF_CloseButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    this->setStyleSheet("QToolButton{border-image:url(:/Res/RedClose.png);outset;}");
}

void WF_CloseButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/RedClose.png);outset;}");
}

void WF_CloseButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/RedClose.png);outset;}");
}

void WF_CloseButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/whiteClose.png);outset;}");
}


WF_MinimizeButton::WF_MinimizeButton(QWidget * parent, int x, int y) : QToolButton(parent)
{
    this->setStyleSheet("QToolButton{border-image:url(:/Res/whiteMinimize.png);outset;}");
    this->setFixedSize(x,y);
}

WF_MinimizeButton::~WF_MinimizeButton()
{
}

void WF_MinimizeButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    this->setStyleSheet("QToolButton{border-image:url(:/Res/grayMinimize.png);outset;}");
}

void WF_MinimizeButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/whiteMinimize.png);outset;}");
}

void WF_MinimizeButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/grayMinimize.png);outset;}");
}

void WF_MinimizeButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/whiteMinimize.png);outset;}");
}


WF_LoginCloseButton::WF_LoginCloseButton(QWidget * parent, int x, int y) : QToolButton(parent)
{

    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginWClose.png);outset;}");
    this->setFixedSize(x,y);
}

WF_LoginCloseButton::~WF_LoginCloseButton()
{
}

void WF_LoginCloseButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    this->setStyleSheet("QToolButton{border-image:url(:/Res/RedClose.png);outset;}");
}

void WF_LoginCloseButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/RedClose.png);outset;}");
}

void WF_LoginCloseButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/RedClose.png);outset;}");
}

void WF_LoginCloseButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginWClose.png);outset;}");
}


WF_LoginSettingButton::WF_LoginSettingButton(QWidget * parent, int x, int y) : QToolButton(parent)
{

    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginWSetting.png);outset;}");
    this->setFixedSize(x,y);
}

WF_LoginSettingButton::~WF_LoginSettingButton()
{
}

void WF_LoginSettingButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginGSetting.png);outset;}");
}

void WF_LoginSettingButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginGSetting.png);outset;}");
}

void WF_LoginSettingButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginGSetting.png);outset;}");
}

void WF_LoginSettingButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/loginWSetting.png);outset;}");
}


WF_ChatBoxEmojiButton::WF_ChatBoxEmojiButton(QWidget * parent, int x, int y) : QToolButton(parent)
{

    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatIconPkgWhite.png);outset;}");
    this->setFixedSize(x,y);
}

WF_ChatBoxEmojiButton::~WF_ChatBoxEmojiButton()
{
}

void WF_ChatBoxEmojiButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatIconPkgGreen.png);outset;}");
}

void WF_ChatBoxEmojiButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatIconPkgWhite.png);outset;}");
}

void WF_ChatBoxEmojiButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatIconPkgBlack.png);outset;}");
}

void WF_ChatBoxEmojiButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatIconPkgWhite.png);outset;}");
}


WF_ChatBoxFileButton::WF_ChatBoxFileButton(QWidget * parent, int x, int y) : QToolButton(parent)
{

    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatFileWhite.png);outset;}");
    this->setFixedSize(x,y);
}

WF_ChatBoxFileButton::~WF_ChatBoxFileButton()
{
}

void WF_ChatBoxFileButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked();
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatFileGreen.png);outset;}");
}

void WF_ChatBoxFileButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatFileWhite.png);outset;}");
}

void WF_ChatBoxFileButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatFileBlack.png);outset;}");
}

void WF_ChatBoxFileButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("QToolButton{border-image:url(:/Res/ChatFileWhite.png);outset;}");
}
