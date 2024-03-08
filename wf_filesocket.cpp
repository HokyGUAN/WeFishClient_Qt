/**********************************
 *
 *        WeFish FileSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_filesocket.h"
#include "common/base64.hpp"
#include "wf_config.h"

#include <iostream>
#include <fstream>
#include <QStandardPaths>


WF_FileSocket::WF_FileSocket(QString ip, int port)
    : ip_(ip)
    , port_(port)
    , key_(WF_CRYPTOR_KEY)
    , cryptor_(Cryptor(key_.toUtf8()))
{
}

WF_FileSocket::~WF_FileSocket()
{
}

void WF_FileSocket::sInitialise()
{
    socket = new QTcpSocket(this);

    socket->connectToHost(QHostAddress(ip_), port_);

    connect(socket, &QTcpSocket::connected, this, &WF_FileSocket::doConnect);
    connect(socket, &QTcpSocket::readyRead, this, &WF_FileSocket::doRead);
    connect(socket, &QTcpSocket::disconnected, this, &WF_FileSocket::doDisConnect);
    connect(socket, &QTcpSocket::stateChanged, this, &WF_FileSocket::doStateChange);

    doVersionCheck(WF_VERSION);
}

void WF_FileSocket::sReconnect(QString ip, int port)
{
    ip_ = ip;
    port_ = port;
    if (socket != NULL) {
        qDebug() << "Change Object";
        free(socket);
        socket = NULL;
    }
    socket = new QTcpSocket(this);

    socket->connectToHost(QHostAddress(ip_), port_);

    connect(socket, &QTcpSocket::connected, this, &WF_FileSocket::doConnect);
    connect(socket, &QTcpSocket::readyRead, this, &WF_FileSocket::doRead);
    connect(socket, &QTcpSocket::disconnected, this, &WF_FileSocket::doDisConnect);
    connect(socket, &QTcpSocket::stateChanged, this, &WF_FileSocket::doStateChange);

    doVersionCheck(WF_VERSION);
}

void WF_FileSocket::doMessageReceived(QString const &msg)
{
    jsonrpcpp::entity_ptr entity(nullptr);
    try {
        entity = jsonrpcpp::Parser::do_parse(msg.toUtf8().data());
        if (entity->is_response()) {
            jsonrpcpp::response_ptr response = std::dynamic_pointer_cast<jsonrpcpp::Response>(entity);
            jsonrpcpp::parameter_ptr param = std::make_shared<jsonrpcpp::Parameter>(response->result());

            if (param->get("Method") == "VersionChecked") {
                int Expired = param->get("Expired");
                if (Expired) {
                    emit eVersionExpired();
                } else {
                    emit eVersionLatest();
                }
            } else if (param->get("Method") == "SayHello") {
                account_ = param->get("Account");
                account_dir_str_ = WF_DIR + "\\wfid_" + QString::number(account_);
                QDir wf_account_dir(account_dir_str_);
                if (!wf_account_dir.exists()) {
                    wf_account_dir.mkdir(account_dir_str_);
                }
            } else if (param->get("Method") == "UpgradeReply") {
                // size_t file_size = param->get("Filesize");
                QString file_name = QString::fromStdString(param->get("Filename"));
                qDebug() << "Upgrade File: " << file_name;

                upgrade_stream_.reset(new FileStream(file_name));
                doUpgradeFileTransfer();
            } else if (param->get("Method") == "UpgradeProcessing") {
                int process = param->get("Process");

                std::string content = CBASE64::Decode(param->get("Content"));
                QByteArray decrypt(QByteArray::fromRawData(content.c_str(), content.size()));
                upgrade_stream_->Push(decrypt);

                emit eUpgradeProgressChanged(process);
                if (process < 100) {
                    doUpgradeFileTransfer();
                } else {
                    QFile file_stream(WF_UPDT_DIR + "\\WeFish.exe");
                    file_stream.open(QIODevice::WriteOnly);
                    QByteArray data = upgrade_stream_->Pop();
                    file_stream.write(data, data.size());
                    QString WF_DesktopLinkPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "\\WeFish.lnk";
                    file_stream.link(WF_DesktopLinkPath);
                    file_stream.close();
                    QString WF_DesktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "\\WeFish.exe";
                    QFile WF_DesktopExe(WF_DesktopPath);
                    if (WF_DesktopExe.exists()) {
                        QFile::remove(WF_DesktopPath);
                    }
                }
            }
        } else if (entity->is_notification()) {
            jsonrpcpp::notification_ptr notification = std::dynamic_pointer_cast<jsonrpcpp::Notification>(entity);
            if (notification->method() == "FileTransferNotification") {
                int from_account = notification->params().get("Account");
                QString name = QString::fromStdString(notification->params().get("Name"));
                int to_account = notification->params().get("ToAccount");
                QString file_name = QString::fromStdString(notification->params().get("Filename"));
                QString checksum = QString::fromStdString(notification->params().get("Checksum"));
                size_t file_size = notification->params().get("Filesize");
                std::string content = CBASE64::Decode(notification->params().get("Content"));
                int status = notification->params().get("Status");
                QByteArray decrypt(QByteArray::fromRawData(content.c_str(), content.size()));
//                qDebug() << "Account: " << from_account;
//                qDebug() << "ToAccount: " << to_account;
//                qDebug() << "Filename: " << file_name;
//                qDebug() << "Filesize: " << file_size;
//                qDebug() << "Checksum: " << checksum;
//                qDebug() << "Content: " << decrypt;
//                qDebug() << "Status: " << status;

                int process = 0;
                auto it = container_.find(checksum);
                if (status == 1 && it == container_.end()) {
                    QSharedPointer<FileStream> file_stream = QSharedPointer<FileStream>(new FileStream(file_name));
                    container_.insert(checksum, file_stream);
                } else if (status == 2 && it != container_.end()) {
                    (it.value())->Push(decrypt);
                    process = ((size_t)(it.value())->Size() * 100) / file_size;
                } else if (status == 0 && it != container_.end()) {
                    (it.value())->Push(decrypt);
                    process = ((size_t)(it.value())->Size() * 100) / file_size;

                    QString file_location = account_dir_str_ + "\\" + file_name;
                    QFile file_stream(file_location);
                    file_location = file_location.replace("\\", "/");
                    file_location = file_location.replace(":", "");
                    file_stream.open(QIODevice::WriteOnly|QIODevice::Truncate);
                    QByteArray data = (it.value())->Pop();
                    file_stream.write(data, data.size());
                    file_stream.close();
                    container_.erase(it);
                    emit eNotify(from_account, name, QPixmap(), to_account, file_location, "FileContent");
                }
            }
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

void WF_FileSocket::doVersionCheck(QString clientversion)
{
    qDebug() << "Current Version: " << clientversion;
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "VersionCheck",
        jsonrpcpp::Parameter("Clientversion", clientversion.toUtf8())));
    QString msg = QString(request->to_json().dump().data());
    QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = cipher + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_FileSocket::doSayHello(int account)
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "SayHello",
        jsonrpcpp::Parameter("Account", account)));
    QString msg = QString(request->to_json().dump().data());
    QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = cipher + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_FileSocket::sSendFile(int account, QString name, int to_account, QString file_path)
{
    bool head = true;

    QFile file_stream(file_path);
    if (!file_stream.open(QIODevice::ReadOnly)) {
        return;
    }
    int file_size = file_stream.size();

    QString checksum = "b60b0ce5bbab49f5ec134022ed7a908e";

    QStringList list = file_path.split("/", QString::SkipEmptyParts);
    QString file_name = list.at(list.size() - 1);
    QString file_location = account_dir_str_ + "\\" + file_name;
    file_location = file_location.replace("\\", "/");
    file_location = file_location.replace(":", "");

    int data_consume = 0;

    jsonrpcpp::request_ptr request(nullptr);
#define M_BLOCK_SIZE 3000000
    int block_size = M_BLOCK_SIZE;
    while (!file_stream.atEnd()) {
        if (head) {
            head = false;
            request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_FILE), "FileTransfer",
                            jsonrpcpp::Parameter("Account", account, "Name", name.toStdString(), "ToAccount", to_account, "Status", 1,
                                                "Filename", file_name.toStdString(), "Filesize", file_size, "Checksum", checksum.toStdString(),
                                                "Content", "")));
        } else {
            if (file_size - data_consume < block_size) {
                block_size = file_size - data_consume;
                QByteArray content = file_stream.read(block_size);
                std::string encrypt = CBASE64::Encode(content.data(), content.size());
                request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_FILE), "FileTransfer",
                    jsonrpcpp::Parameter("Account", account, "Name", name.toStdString(), "ToAccount", to_account, "Status", 0,
                                        "Filename", file_name.toStdString(), "Filesize", file_size, "Checksum", checksum.toStdString(),
                                        "Content", encrypt)));
                data_consume += block_size;
                emit eSendFinished(file_location);
            } else {
                QByteArray content = file_stream.read(block_size);
                std::string encrypt = CBASE64::Encode(content.data(), content.size());
                request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_FILE), "FileTransfer",
                    jsonrpcpp::Parameter("Account", account, "Name", name.toStdString(), "ToAccount", to_account, "Status", 2,
                                        "Filename", file_name.toStdString(), "Filesize", file_size, "Checksum", checksum.toStdString(),
                                        "Content", encrypt)));
                data_consume += block_size;
            }
        }

        QString msg = QString(request->to_json().dump().data());
        QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
        msg = cipher + "\r\n";
        socket->write(msg.toUtf8());
    }
    file_stream.close();
}

void WF_FileSocket::sUpgradeRequest(int code)
{
    if (code) {
        doSayHello(code);
    } else {
        jsonrpcpp::request_ptr request(nullptr);
        request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "UpgradeRequest",
            jsonrpcpp::Parameter("Code", code)));
        QString msg = QString(request->to_json().dump().data());
        QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
        msg = cipher + "\r\n";
        socket->write(msg.toUtf8());
    }
}

void WF_FileSocket::doUpgradeFileTransfer()
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_FILE), "UpgradeFileTransfer",
        jsonrpcpp::Parameter("Account", account_)));
    QString msg = QString(request->to_json().dump().data());
    QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = cipher + "\r\n";
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
        QString decrypt = cryptor_.Decrypt(msg.toUtf8());
        doMessageReceived(decrypt.toUtf8());
    }
}

void WF_FileSocket::doDisConnect()
{
    qDebug() << "File Socket Disconnected";
}
