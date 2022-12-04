/**********************************
 *
 *        WeFish TcpSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_tcpsocket.h"

WF_TcpSocket::WF_TcpSocket(QString name, QString userIconUrl, QString ip, int port)
    : name_(name)
    , userIconUrl_(userIconUrl)
    , ip_(ip)
    , port_(port)
    , rest_msg_("")
    , userConflictTimes_(0)
{
    socketSetup();

    ImageHandler = new WF_ImageHandler();
}

WF_TcpSocket::~WF_TcpSocket()
{
}

void WF_TcpSocket::socketSetup()
{
    socket = new QTcpSocket(this);

    socket->connectToHost(QHostAddress(ip_), port_);

    connect(socket, &QTcpSocket::connected, this, &WF_TcpSocket::doConnect);
    connect(socket, &QTcpSocket::readyRead, this, &WF_TcpSocket::doRead);
    connect(socket, &QTcpSocket::disconnected, this, &WF_TcpSocket::doDisConnect);
}

void WF_TcpSocket::doMessageReceived(QString const &msg)
{  
    jsonrpcpp::entity_ptr entity(nullptr);
    try {
        entity = jsonrpcpp::Parser::do_parse(msg.toUtf8().data());
        if (entity->is_response()) {
            jsonrpcpp::response_ptr response = std::dynamic_pointer_cast<jsonrpcpp::Response>(entity);
            jsonrpcpp::parameter_ptr param = std::make_shared<jsonrpcpp::Parameter>(response->result());
            emit myid(param->get("IDinGroup"));
            QString active_list_str = QString::fromStdString(param->get("ActiveList"));
            QStringList AList = active_list_str.split("#W#F#", QString::SkipEmptyParts);
            QStringListIterator strIterator(AList);
            while (strIterator.hasNext()) {
                QString str = strIterator.next();
                int id = str.split("-W-F-", QString::SkipEmptyParts).at(0).toInt();
                QString name = str.split("-W-F-", QString::SkipEmptyParts).at(1);
                QByteArray icon_byte_array = str.split("-W-F-", QString::SkipEmptyParts).at(2).toUtf8();
                QPixmap icon = ImageHandler->Base64ToImage(icon_byte_array);
                if (name == name_) {userConflictTimes_++;}
                if (userConflictTimes_ > 1) {emit applicationshutdown(REASON_USERCONFLICT);}
                emit sayhello(id, name, icon);
            }
        } else if (entity->is_notification()) {
            jsonrpcpp::notification_ptr notification = std::dynamic_pointer_cast<jsonrpcpp::Notification>(entity);
            if (notification->method() == "ContentNotification") {
                int id = notification->params().get("IDinGroup");
                std::string who_str = notification->params().get("Who");
                int to_id = notification->params().get("ToID");
                std::string content_str = notification->params().get("Content");
                emit notify(id, QString::fromStdString(who_str), to_id, QString::fromStdString(content_str), "Content");
            } else if (notification->method() == "PicContentNotification") {
                int id = notification->params().get("IDinGroup");
                std::string who_str = notification->params().get("Who");
                int to_id = notification->params().get("ToID");
                std::string content_str = notification->params().get("Content");
                emit notify(id, QString::fromStdString(who_str), to_id, QString::fromStdString(content_str), "PicContent");
            } else if (notification->method() == "OnlineNotification") {
                int id = notification->params().get("IDinGroup");
                std::string who_str = notification->params().get("Who");
                std::string icon_str = notification->params().get("Icon");
                QByteArray icon_byte_array = QString::fromStdString(icon_str).toUtf8();
                QPixmap icon = ImageHandler->Base64ToImage(icon_byte_array);
                emit online(id, QString::fromStdString(who_str), icon);
            } else if (notification->method() == "OfflineNotification") {
                int id = notification->params().get("IDinGroup");
                std::string who_str = notification->params().get("Who");
                emit offline(id, QString::fromStdString(who_str));
            }
        } else {
            qDebug() << "Not registered feedback\n";
        }
    } catch(const jsonrpcpp::ParseErrorException& e) {
        qDebug() << "Json Parse Error\n";
    }
}

void WF_TcpSocket::doConnect()
{
    qDebug() << "Connected";
    QString msg;
    jsonrpcpp::request_ptr request(nullptr);
    QByteArray picbyteArray = ImageHandler->ImageToBase64(userIconUrl_);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_CONTENT), "SayHello", jsonrpcpp::Parameter("Who", name_.toUtf8(), "Icon", picbyteArray.data())));
    msg = QString(request->to_json().dump().data());

    QByteArray byteArray = msg.toUtf8();
    byteArray = byteArray.toBase64();
    msg = byteArray + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_TcpSocket::doRead()
{
    bool isbad = false;

    QString msg = rest_msg_ + QString::fromUtf8(socket->readAll());

    if (msg.right(2) != "\r\n") {
        isbad = true;
    }
    QStringList strList = msg.split("\r\n", QString::SkipEmptyParts);
    //qDebug() << strList;

    if (isbad) {
        rest_msg_ = strList.at(strList.size() - 1);
        strList.pop_back();
    } else {
        rest_msg_.clear();
    }
    QStringListIterator strIterator(strList);
    while (strIterator.hasNext()) {
        QString msg = strIterator.next();
        QByteArray byteArray = msg.toUtf8();
        QString decoded = QByteArray::fromBase64(byteArray);
        doMessageReceived(decoded);
    }
}

void WF_TcpSocket::doDisConnect()
{
    qDebug() << "Disconnected";
    emit servershutdown(REASON_SERVERLOST);
}
