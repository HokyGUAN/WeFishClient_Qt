/**********************************
 *
 *        WeFish MainSocket
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_mainsocket.h"


WF_MainSocket::WF_MainSocket(QString ip, int port, Cryptor& cryptor)
    : cryptor_(cryptor)
    , ip_(ip)
    , port_(port)
{
    socketSetup();

    ImageHandler = new WF_ImageHandler();
}

WF_MainSocket::~WF_MainSocket()
{
}

void WF_MainSocket::socketSetup()
{
    socket = new QTcpSocket(this);

    socket->setProxy(QNetworkProxy::NoProxy);
    //qDebug() << "Proxy Type: " << socket->proxy().applicationProxy().type();
    socket->connectToHost(QHostAddress(ip_), port_);

    connect(socket, &QTcpSocket::connected, this, &WF_MainSocket::doConnect);
    connect(socket, &QTcpSocket::readyRead, this, &WF_MainSocket::doRead);
    connect(socket, &QTcpSocket::disconnected, this, &WF_MainSocket::doDisConnect);
    connect(socket, &QTcpSocket::stateChanged, this, &WF_MainSocket::doStateChange);
}

void WF_MainSocket::doMessageReceived(QString const &msg)
{  
    jsonrpcpp::entity_ptr entity(nullptr);
    try {
        entity = jsonrpcpp::Parser::do_parse(msg.toUtf8().data());
        if (entity->is_response()) {
            jsonrpcpp::response_ptr response = std::dynamic_pointer_cast<jsonrpcpp::Response>(entity);
            jsonrpcpp::parameter_ptr param = std::make_shared<jsonrpcpp::Parameter>(response->result());
            if (param->get("Method") == "SayHello") {
                account_ = param->get("Account");
                emit eAccountCheckin(account_);
                qDebug() << "My Account: " << account_;
                QString active_list_str = QString::fromStdString(param->get("ActiveList"));
                QStringList AList = active_list_str.split("#W#F#", QString::SkipEmptyParts);
                QStringListIterator strIterator(AList);
                while (strIterator.hasNext()) {
                    QString str = strIterator.next();
                    int account = str.split("-W-F-", QString::SkipEmptyParts).at(0).toInt();
                    QString name = str.split("-W-F-", QString::SkipEmptyParts).at(1);
                    QByteArray icon_byte_array = str.split("-W-F-", QString::SkipEmptyParts).at(2).toUtf8();
                    QPixmap icon = ImageHandler->Base64ToImage(icon_byte_array);

                    qDebug() << "Onlined Account: " << account << ", Name: " << name;

                    if (account == account_) {userConflictTimes_++;}
                    if (userConflictTimes_ > 1) {emit applicationshutdown(REASON_USERCONFLICT);}
                    emit eSayHello(account, name, icon);
                }
            } else if (param->get("Method") == "RegisterRequest") {
                int status_code = param->get("StatusCode");
                if (status_code == 0) {
                    emit eRegistered(status_code, QString::fromStdString(param->get("Account")), QString::fromStdString(param->get("Password")));
                } else if (status_code == 1) {
                    emit eExpired(status_code, QString::fromStdString(param->get("NewInviteCode")));
                } else if (status_code == 2) {
                    emit eRejected(status_code);
                }
            } else if (param->get("Method") == "PermissionRequest") {
                int status_code = param->get("StatusCode");
                if (status_code == 0) {
                    account_ = param->get("Account");
                    QString name = QString::fromStdString(param->get("Name"));
                    name_ = name;
                    QString icon = QString::fromStdString(param->get("Icon"));
                    UserIconBA_ = icon.toUtf8();
                    emit eValidate(param->get("StatusCode"), account_, name_, icon);
                } else {
                    emit eInvalidate(param->get("StatusCode"));
                }
            }
        } else if (entity->is_notification()) {
            jsonrpcpp::notification_ptr notification = std::dynamic_pointer_cast<jsonrpcpp::Notification>(entity);
            if (notification->method() == "ContentNotification") {
                int account = notification->params().get("Account");
                std::string name = notification->params().get("Name");
                std::string icon_str = notification->params().get("Icon");
                QByteArray icon_byte_array = QString::fromStdString(icon_str).toUtf8();
                QPixmap icon = ImageHandler->Base64ToImage(icon_byte_array);
                int to_account = notification->params().get("ToAccount");
                std::string content = notification->params().get("Content");
                emit eNotify(account, QString::fromStdString(name), icon, to_account, QString::fromStdString(content), "Content");
            } else if (notification->method() == "PicContentNotification") {
                int account = notification->params().get("Account");
                std::string name = notification->params().get("Name");
                std::string icon_str = notification->params().get("Icon");
                QByteArray icon_byte_array = QString::fromStdString(icon_str).toUtf8();
                QPixmap icon = ImageHandler->Base64ToImage(icon_byte_array);
                int to_account = notification->params().get("ToAccount");
                std::string content = notification->params().get("Content");
                emit eNotify(account, QString::fromStdString(name), icon, to_account, QString::fromStdString(content), "PicContent");
            } else if (notification->method() == "OnlineNotification") {
                qDebug() << "online notify\n";
                int account = notification->params().get("Account");
                std::string name = notification->params().get("Name");
                std::string icon_str = notification->params().get("Icon");
                QByteArray icon_byte_array = QString::fromStdString(icon_str).toUtf8();
                QPixmap icon = ImageHandler->Base64ToImage(icon_byte_array);
                emit eOnline(account, QString::fromStdString(name), icon);
            } else if (notification->method() == "OfflineNotification") {
                int account = notification->params().get("Account");
                std::string name = notification->params().get("Name");
                emit eOffline(account, QString::fromStdString(name));
            }
        } else {
            qDebug() << "Not registered feedback\n";
        }
    } catch(const jsonrpcpp::ParseErrorException& e) {
        qDebug() << "Json Parse Error\n";
    }
}

void WF_MainSocket::doStateChange(QAbstractSocket::SocketState state)
{
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        emit eUnconnected();
        break;
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ConnectingState:
        break;
    case QAbstractSocket::ConnectedState:
        emit eConnected();
        break;
    case QAbstractSocket::BoundState:
        break;
    case QAbstractSocket::ClosingState:
        break;
    case QAbstractSocket::ListeningState:
        break;
    }
}

void WF_MainSocket::doConnect()
{
    qDebug() << "Connected";
}

void WF_MainSocket::doPermission(int account, QString password)
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "PermissionRequest", \
                      jsonrpcpp::Parameter("Account", account, "Password", password.toUtf8())));
    QString msg = QString(request->to_json().dump().data());
    QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = cipher + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_MainSocket::doRegister(QString name, QString password, QString invite_code, QString icon_path)
{
    QByteArray IconArray = ImageHandler->ImageToBase64(icon_path);

    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "RegisterRequest", \
                      jsonrpcpp::Parameter("Name", name.toUtf8(), "Password", password.toUtf8(), "Invitecode", invite_code.toUtf8(), "Icon", IconArray.data())));
    QString msg = QString(request->to_json().dump().data());
    QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = cipher + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_MainSocket::doSayhello(int account)
{
    jsonrpcpp::request_ptr request(nullptr);
    request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_SETTING), "SayHello", jsonrpcpp::Parameter("Account", account, "Name", name_.toUtf8(), "Icon", UserIconBA_.data())));
    QString msg = QString(request->to_json().dump().data());
    //QByteArray byteArray = msg.toUtf8();
    //byteArray = byteArray.toBase64();
    QByteArray cipher = cryptor_.Encrypt(msg.toUtf8());
    msg = cipher + "\r\n";
    socket->write(msg.toUtf8());
}

void WF_MainSocket::doRead()
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
        //QByteArray byteArray = msg.toUtf8();
        //QString decoded = QByteArray::fromBase64(byteArray);
        QString decrypt = cryptor_.Decrypt(msg.toUtf8());
        doMessageReceived(decrypt.toUtf8());
    }
}

void WF_MainSocket::doDisConnect()
{
    qDebug() << "Disconnected";
    emit servershutdown(REASON_SERVERLOST);
}
