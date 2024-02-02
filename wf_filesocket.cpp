/**********************************
 *
 *        WeFish FileSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_filesocket.h"
#include "common/base64.hpp"

#include <iostream>
#include <fstream>


WF_FileSocket::WF_FileSocket(QString ip, int port, Cryptor& cryptor)
    : ip_(ip)
    , port_(port)
    , cryptor_(cryptor)
{
    Initialise();
}

WF_FileSocket::~WF_FileSocket()
{
}

void WF_FileSocket::Initialise()
{
    socket = new QTcpSocket(this);

    //socket->setProxy(QNetworkProxy::NoProxy);
    socket->connectToHost(QHostAddress(ip_), port_);

    connect(socket, &QTcpSocket::connected, this, &WF_FileSocket::doConnect);
    connect(socket, &QTcpSocket::readyRead, this, &WF_FileSocket::doRead);
    connect(socket, &QTcpSocket::disconnected, this, &WF_FileSocket::doDisConnect);
    connect(socket, &QTcpSocket::stateChanged, this, &WF_FileSocket::doStateChange);
}

void WF_FileSocket::doMessageReceived(QString const &msg)
{
    jsonrpcpp::entity_ptr entity(nullptr);
    try {
        entity = jsonrpcpp::Parser::do_parse(msg.toUtf8().data());
        if (entity->is_response()) {
            jsonrpcpp::response_ptr response = std::dynamic_pointer_cast<jsonrpcpp::Response>(entity);
            jsonrpcpp::parameter_ptr param = std::make_shared<jsonrpcpp::Parameter>(response->result());
            if (param->get("Method") == "SayHello") {
                account_ = param->get("Account");
                int Expired = param->get("Expired");
                if (Expired) {
                    emit eVersionExpired();
                }
            } else if (param->get("Method") == "UpgradeReply") {
                // size_t file_size = param->get("Filesize");
                QString file_name = QString::fromStdString(param->get("Filename"));
                qDebug() << "Upgrade File: " << file_name;

                upgrade_stream_.reset(new FileStream(file_name));
                doUpgradeFileTransfer();
            } else if (param->get("Method") == "UpgradeProcessing") {
                int process = param->get("Process");
                qDebug() << "Process: " << process;

                std::string content = CBASE64::Decode(param->get("Content"));
                QByteArray decrypt(QByteArray::fromRawData(content.c_str(), content.size()));
                upgrade_stream_->Push(decrypt);

                emit eUpgradeProgressChanged(process);
                if (process < 100) {
                    doUpgradeFileTransfer();
                } else {
                    QFile file_stream(WF_DIR + "\\upgrade_test.exe");
                    file_stream.open(QIODevice::WriteOnly);
                    QByteArray data = upgrade_stream_->Pop();
                    file_stream.write(data, data.size());
                    file_stream.close();
                }
            }
        } else if (entity->is_notification()) {
//            jsonrpcpp::notification_ptr notification = std::dynamic_pointer_cast<jsonrpcpp::Notification>(entity);
//            if (notification->method() == "FileTransferNotification") {
//                int from_account = notification->params().get("Account");
//                int to_account = notification->params().get("ToAccount");
//                QString file_name = QString::fromStdString(notification->params().get("Filename"));
//                QString checksum = QString::fromStdString(notification->params().get("Checksum"));
//                int file_size = notification->params().get("Filesize");
//                QString content = QString::fromStdString(notification->params().get("Content"));
//                int status = notification->params().get("Status");

//                qDebug() << "Account: " << from_account;
//                qDebug() << "ToAccount: " << to_account;
//                qDebug() << "Filename: " << file_name;
//                qDebug() << "Filesize: " << file_size;
//                qDebug() << "Checksum: " << checksum;
//                qDebug() << "Content: " << content;
//                qDebug() << "Status: " << status;

//                auto it = container_.find(checksum);
//                if (status == 1 && it == container_.end()) {
//                    QSharedPointer<FileStream> file_stream = QSharedPointer<FileStream>(new FileStream(file_name));
//                    // file_stream->Push(content);
//                    container_.insert(checksum, file_stream);
//                } else if (status == 2 && it != container_.end()) {
//                    (it.value())->Push(content);
//                } else if (status == 0 && it != container_.end()) {
//                    (it.value())->Push(content);
//                    QFile file_stream(file_name);
//                    file_stream.open(QIODevice::WriteOnly|QIODevice::Truncate);
//                    QDataStream out(&file_stream);
//                    out << (it.value())->Pop();
//                    file_stream.close();
//                    container_.erase(it);
//                }
//            }
        } else {
            qDebug() << "Not registered feedback\n";
        }
    } catch(const jsonrpcpp::ParseErrorException& e) {
        qDebug() << "Json Parse Error\n";
    }
}

void WF_FileSocket::doStateChange(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        emit eUnconnected();
        break;
    case QAbstractSocket::ConnectedState:
        emit eConnected();
        break;
    default:
        break;
    }
}

void WF_FileSocket::doConnect()
{
    qDebug() << "File Socket Connected";
}

void WF_FileSocket::doSayhello(int account, QString clientversion)
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "SayHello",
        jsonrpcpp::Parameter("Account", account, "Clientversion", clientversion.toUtf8())));
    QString msg = QString(request->to_json().dump().data());
    //QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = msg + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_FileSocket::doUpgradeRequest()
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "UpgradeRequest",
        jsonrpcpp::Parameter("Account", account_)));
    QString msg = QString(request->to_json().dump().data());
    //QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = msg + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_FileSocket::doUpgradeFileTransfer()
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_FILE), "UpgradeFileTransfer",
        jsonrpcpp::Parameter("Account", account_)));
    QString msg = QString(request->to_json().dump().data());
    //QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = msg + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_FileSocket::doRead()
{
    bool is_bad_package = false;
    QString msg = rest_msg_ + QString::fromUtf8(socket->readAll());
    if (msg.right(2) != "\r\n") {
        is_bad_package = true;
    }
    QStringList strList = msg.split("\r\n", QString::SkipEmptyParts);

    if (is_bad_package) {
        rest_msg_ = strList.at(strList.size() - 1);
        strList.pop_back();
    } else {
        rest_msg_.clear();
    }
    QStringListIterator strIterator(strList);
    while (strIterator.hasNext()) {
        QString msg = strIterator.next();
        //QString decrypt = cryptor_.Decrypt(msg.toUtf8());
        doMessageReceived(msg.toUtf8());
    }
}

void WF_FileSocket::doDisConnect()
{
    qDebug() << "Disconnected";
}
