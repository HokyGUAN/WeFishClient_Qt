/**********************************
 *
 *      WeFish Login Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_LOGINWIN_H
#define WF_LOGINWIN_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

#include "wf_mainwin.h"

class WF_LoginWin : public QWidget
{
    Q_OBJECT
public:
    explicit WF_LoginWin(QWidget *parent = nullptr, QSize* frameSize = nullptr);
    ~WF_LoginWin();

    void paintEvent(QPaintEvent *event);

private:
    QSize* frameSize_;
    QString host_name_;
    QString ip_;
    int port_;
    QFile* wf_config_file_;

    QPixmap* UserIcon;
    QString UserIconUrl;
    QLabel* NameLabel;
    QLineEdit* LineInput;
    QPushButton* LoginButton;
    WF_TcpSocket* TcpSocket;
    WF_MainWin* MainWin;
};


#endif // WF_LOGINWIN_H
