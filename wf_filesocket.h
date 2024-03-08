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


class WF_FileSocket : public QObject
{
    Q_OBJECT
public:
    WF_FileSocket(QString ip, int port);
    ~WF_FileSocket();

    void doConnect();
    void doDisConnect();

public slots:
    void sInitialise();
    void sReconnect(QString ip, int port);
    void sUpgradeRequest(int code);
    void sSendFile(int account, QString name, int to_account, QString file_path);

signals:
    void eConnected();
    void eUnconnected();
    void eSayHello(int others_account, QString others_name, QPixmap others_icon);
    void eVersionExpired();
    void eVersionLatest();
    void eSendFinished(QString response);
    void eNotify(int sender_account, QString sender_name, QPixmap sender_icon, int to_account, QString content, QString content_type);
    void eUpgradeProgressChanged(int percent);

private:
    QTcpSocket *socket = NULL;
    QString ip_;
    int port_;
    int account_;
    QString key_; // Cryptor Key
    Cryptor cryptor_;
    QString rest_msg_ = "";
    QMap<QString, QSharedPointer<FileStream>> container_;
    QSharedPointer<FileStream> upgrade_stream_ = nullptr;
    QString account_dir_str_;

    void doVersionCheck(QString clientversion);
    void doSayHello(int account);
    void doRead();
    void doMessageReceived(QString const &msg);
    void doStateChange(QAbstractSocket::SocketState state);
    void doUpgradeFileTransfer();
};

#endif // WF_FILESOCKET_H
