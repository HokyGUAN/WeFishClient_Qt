/**********************************
 *
 *        WeFish FileSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_FILESOCKET_H
#define WF_FILESOCKET_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QNetworkProxy>
#include "common/jsonrpcpp.h"
#include "common/wf_message.h"
#include "wf_misc.h"


extern QString WF_DIR;

class FileStream
{
public:
    FileStream(QString& name) : name_(name) {};
    ~FileStream() = default;

    void Push(QByteArray& content) {
        contents_.append(content);
    };
    QByteArray Pop(void) {
        QByteArray pop = contents_;
        contents_.clear();
        return pop;
    };
    int Size(void) {
        return contents_.size();
    };
    QString Name(void) {
        return name_;
    }

private:
    QString name_;
    QByteArray contents_;
};

class WF_FileSocket : public QTcpSocket
{
    Q_OBJECT
public:
    WF_FileSocket(QString ip, int port, Cryptor& cryptor);
    ~WF_FileSocket();

    void Initialise();
    void doConnect();
    void doDisConnect();
    void doSayhello(int account, QString clientversion);
    void doUpgradeRequest();

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
    void eVersionExpired();
    void eUpgradeProgressChanged(int percent);

private:
    QTcpSocket *socket;
    QString ip_;
    int port_;
    int account_;
    QString name_ = "";
    Cryptor& cryptor_;
    QString key_; // Cryptor Key
    QString rest_msg_ = "";
    QMap<QString, QSharedPointer<FileStream>> container_;
    QSharedPointer<FileStream> upgrade_stream_ = nullptr;

    void doRead();
    void doMessageReceived(QString const &msg);
    void doStateChange(QAbstractSocket::SocketState state);

    void doUpgradeFileTransfer();
};

#endif // WF_FILESOCKET_H
