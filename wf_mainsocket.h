/**********************************
 *
 *        WeFish TcpSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_MAINSOCKET_H
#define WF_MAINSOCKET_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QNetworkProxy>
#include "common/jsonrpcpp.h"
#include "common/wf_message.h"
#include "wf_misc.h"


class WF_MainSocket : public QTcpSocket
{
    Q_OBJECT
public:
    WF_MainSocket(QString ip, int port, Cryptor& cryptor);
    ~WF_MainSocket();

    void socketSetup();
    void doConnect();
    void doRead();
    void doMessageReceived(QString const &msg);
    void doDisConnect();
    void doStateChange(QAbstractSocket::SocketState state);

    void doPermission(int account, QString password);
    void doRegister(QString name, QString password, QString invite_code, QString icon_path);
    void doSayhello(int account);

    QTcpSocket *socket;
    Cryptor& cryptor_;
    QString name_ = "";

signals:
    void eConnected();
    void eUnconnected();
    void eAccountCheckin(int account);
    void eSayHello(int others_account, QString others_name, QPixmap others_icon);
    void eOnline(int others_account, QString others_name, QPixmap others_icon);
    void eOffline(int others_account, QString others_name);
    void eNotify(int sender_account, QString sender_name, QPixmap sender_icon, int to_account, QString content, QString content_type);
    void applicationshutdown(ShutDownReason reason);
    void servershutdown(ShutDownReason reason);
    void eRegistered(int status_code, QString account, QString password);
    void eExpired(int status_code, QString new_invite_code);
    void eRejected(int status_code);
    void eValidate(int status_code, int account, QString name, QString icon);
    void eInvalidate(int status_code);

private:
    QString ip_;
    int port_;
    int account_;
    QByteArray UserIconBA_;
    QString rest_msg_ = "";
    int userConflictTimes_ = 0;
    QString key_; // Cryptor Key

    WF_ImageHandler* ImageHandler;
};


#endif // WF_TCPSOCKET_H
