/**********************************
 *
 *       WeFish Main Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_MAINWIN_H
#define WF_MAINWIN_H

#include <QWidget>
#include <QLabel>

#include "wf_chatview.h"
#include "wf_flistview.h"
#include "wf_chatinput.h"
#include "wf_button.h"
#include "wf_tcpsocket.h"
#include "wf_browser.h"
#include "wf_misc.h"

class WF_MainWin : public QWidget
{
    Q_OBJECT
public:
    explicit WF_MainWin(QWidget *parent = nullptr, WF_TcpSocket *TcpSocket = nullptr, QString UserIconUrl = NULL);
    ~WF_MainWin();

    void paintEvent(QPaintEvent *event);

protected:
    void mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            QPoint startPos = event->globalPos();
            QPoint limitRegion = mapFromGlobal(startPos);
            if (limitRegion.ry() < 50) {
                offset_ = startPos - geometry().topLeft();
            }
        }
        QWidget::mousePressEvent(event);
    };
    void mouseMoveEvent(QMouseEvent *event) {
        if (event->buttons() == Qt::LeftButton) {
            QPoint endPos = event->globalPos();
            QPoint limitRegion = mapFromGlobal(endPos);
            if (limitRegion.ry() < 50) {
                move(endPos - offset_);
            }
        }
        QWidget::mouseMoveEvent(event);
    };
    void mouseReleaseEvent(QMouseEvent *event) {
         QWidget::mouseReleaseEvent(event);
    };
    virtual void closeEvent(QCloseEvent *event) {
        tcp->socket->disconnectFromHost();
        event->accept();
    };

    bool eventFilter(QObject *obj, QEvent *event) {
        if(obj == ChatInput) {
            if(event->type() == QEvent::KeyPress) {
                 QKeyEvent *k = static_cast<QKeyEvent *>(event);
                 if(k->key() == Qt::Key_Return) {
                     Send();
                     return true;
                 }
            }
        }
        return QWidget::eventFilter(obj, event);
    };

private:
    QPoint offset_;
    int width_;
    int height_;
    QRectF SetFrame_;
    QPoint ListPos_;
    QRectF ListFrame_;
    QPoint ChatViewPos_;
    QPoint ChatInputPos_;
    QPoint SendBtnPos_;
    QRectF ChatViewFrame_;
    QRectF ChatInputFrame_;
    QRectF SendBtnFrame_;
    QLabel* NameLabel;

    WF_HeadIcon* UserIcon;
    WF_ChatView* ChatView;
    WF_FriendListView* FriendList;
    WF_ChatInput* ChatInput;
    WF_Button* SendButton;
    WF_CloseButton* CloseButton;
    WF_MinimizeButton* MinimizeButton;
    WF_TcpSocket* tcp;
    WF_ImageHandler* ImageHandler;
    QString userIconUrl_;

    int myId_;
    int currentItemId_;
    UserItemData myItemdata_;
    UserItemData currentItem_;

private slots:
    void Send();
    void Flush(UserItemData itemdata);
    void ApplicationShutDown(ShutDownReason reason);
    void PictureBrowser(QPixmap pic);
};

#endif // WF_MAINWIN_H
