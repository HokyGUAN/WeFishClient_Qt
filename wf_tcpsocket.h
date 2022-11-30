/**********************************
 *
 *        WeFish TcpSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_TCPSOCKET_H
#define WF_TCPSOCKET_H

#include <QHostAddress>
#include <QTcpSocket>
#include <deque>
#include "common/jsonrpcpp.h"
#include "common/wf_message.h"
#include "wf_misc.h"

class WF_TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    WF_TcpSocket(QString name, QString userIconUrl);
    ~WF_TcpSocket();

    void socketSetup();
    void doConnect();
    void doRead();
    void doMessageReceived(QString const &msg);

    QTcpSocket *socket;
    QString name_;
    std::deque<QString> msg_;

signals:
    void myid(int MyIDinGroup);
    void sayhello(int others_id, QString others_name, QPixmap others_icon);
    void online(int others_id, QString others_name, QPixmap others_icon);
    void offline(int others_id, QString others_name);
    void notify(int sender_id, QString sender_name, int to_id, QString content, QString content_type);

private:
    QString userIconUrl_;
    QString rest_msg_;

    WF_TcpSocket* TcpSocket;
    WF_ImageHandler* ImageHandler;
};


#endif // WF_TCPSOCKET_H
