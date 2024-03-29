/**********************************
 *
 *      WeFish Login Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_loginwin.h"
#include "wf_config.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

#include <QHostAddress>
#include <QTcpSocket>


WF_LoginWin::WF_LoginWin(QWidget *parent, QSize* frameSize)
    : QWidget(parent)
    , frameSize_(frameSize)
    , key_(WF_CRYPTOR_KEY)
    , cryptor_(Cryptor(key_.toUtf8()))
{
    mainWinSize_ = new QSize(920, 635);
    QScreen *monitor = qApp->primaryScreen();
    this->setGeometry((monitor->size().width() - frameSize_->width()) / 2,
                  (monitor->size().height() - frameSize_->height()) / 2,
                  frameSize_->width(),
                  frameSize_->height());
//    this->setWindowTitle("WeFish");
//    this->setWindowIcon(QIcon(QPixmap(":/Res/Tomphany.jpg")));
    this->setMinimumSize(frameSize_->width(), frameSize_->height());
    this->setMaximumSize(frameSize_->width(), frameSize_->height());
    this->setWindowFlags(Qt::FramelessWindowHint);

    int screen_number = QApplication::desktop()->screenNumber(this);
    if (screen_number < 0) screen_number = 0;
    mainMonitorSize_ = new QSize(QGuiApplication::screens().at(screen_number)->geometry().size());

    ImageHandler = new WF_ImageHandler();

    LoginCloseButton  = new WF_LoginCloseButton(this, 33, 24);
    LoginCloseButton->setGeometry(247, 0, 33, 24);
    LoginSettingButton  = new WF_LoginSettingButton(this, 33, 24);
    LoginSettingButton->setGeometry(214, 0, 33, 24);

    LoginSettingWin = new WF_LoginSettingWin(this);
    LoginRegisterWin = new WF_LoginRegisterWin(this);
    LoginSwitchWin = new WF_LoginSwitchWin(this);
    UpgraderWin = new WF_UpgraderWin(this);
    NameLabel = new QLabel(this);

    WF_ConfigPath_ = WF_CONF_DIR + "\\Config.ini";
    configSetting = new QSettings(WF_ConfigPath_, QSettings::IniFormat);

    doSetupNetwork();

    QString FileSocketIP = configSetting->value("Network/SERVER_IP", WF_SERVER_IP).toString();
    int FileSocketPort = configSetting->value("Network/SERVER_PORT", WF_SERVER_PORT).toInt();

    FileTransferTask_ = new QThread;
    FileSocket = new WF_FileSocket(FileSocketIP, FileSocketPort + 1);
    FileSocket->moveToThread(FileTransferTask_);
    connect(FileTransferTask_, &QThread::started, FileSocket, &WF_FileSocket::sInitialise);
    connect(this, SIGNAL(eReInitFileSocket(QString, int)), FileSocket, SLOT(sReconnect(QString, int)));
    connect(this, SIGNAL(eUpgradeRequest(int)), FileSocket, SLOT(sUpgradeRequest(int)));
    FileTransferTask_->start();
    connect(FileSocket, SIGNAL(eVersionExpired()), this, SLOT(sVersionExpired()));
    connect(FileSocket, SIGNAL(eVersionLatest()), this, SLOT(sVersionLatest()));
    connect(FileSocket, SIGNAL(eUpgradeProgressChanged(int)), UpgraderWin, SLOT(sProgressUpdate(int)));

    LoginButton = new QPushButton(this);
    LoginButton->setGeometry((frameSize_->width() - 180) / 2, 265, 180, 35);
    QFont login_btn_font = QFont("Microsoft Yahei");
    login_btn_font.setPixelSize(15);
    LoginButton->setFont(login_btn_font);
    LoginButton->setText("进入微娱");
    LoginButton->setStyleSheet("background: rgb(7, 193, 96); border-radius:4px; color:rgb(255, 255, 255);outset;");

    UserSwitch = new QPushButton(this);
    UserSwitch->setGeometry(60, 320, 80, 20);
    QFont switch_btn_font = QFont("Microsoft Yahei");
    switch_btn_font.setPixelSize(14);
    UserSwitch->setFont(switch_btn_font);
    UserSwitch->setText("切换用户");
    UserSwitch->setStyleSheet("background: rgb(255, 255, 255); border-radius:4px; color:rgb(67, 104, 149);outset;");

    UserRegister = new QPushButton(this);
    UserRegister->setGeometry(140, 320, 80, 20);
    QFont register_btn_font = QFont("Microsoft Yahei");
    register_btn_font.setPixelSize(14);
    UserRegister->setFont(register_btn_font);
    UserRegister->setText("注册账号");
    UserRegister->setStyleSheet("background: rgb(255, 255, 255); border-radius:4px; color:rgb(67, 104, 149);outset;");

    connect(LoginCloseButton, &QPushButton::clicked, [this] {this->close();});
    connect(LoginSettingButton, &QPushButton::clicked, this, &WF_LoginWin::doModify);

    connect(UserSwitch, &QPushButton::clicked, this, &WF_LoginWin::doSwitch);
    connect(UserRegister, &QPushButton::clicked, this, &WF_LoginWin::doRegister);
    connect(LoginButton, &QPushButton::clicked, this, &WF_LoginWin::doLogin);
    connect(LoginSettingWin, SIGNAL(eUpdateNetworkConfig()), this, SLOT(sUpdateNetworkConfig()));
    connect(LoginRegisterWin, SIGNAL(eUserRegister(QString, QString, QString, QString)), this, SLOT(sUserRegister(QString, QString, QString, QString)));
    connect(LoginSwitchWin, SIGNAL(eUserSwitch(QString, QString)), this, SLOT(sUserSwitch(QString, QString)));
}

WF_LoginWin::~WF_LoginWin()
{
}

void WF_LoginWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(253, 254, 254)));
    painter.drawRoundedRect(0, 0, frameSize_->width(), frameSize_->height(), 7, 7);
    painter.setPen(QPen(Qt::gray));
    QFont font = QFont("Microsoft Yahei");
    font.setPixelSize(13);
    painter.setFont(font);
    painter.drawText(QRect(8, 7, 50, 20), "WeFish");

    if (UserIconUrl == "") {
        UserIconUrl = ":/Res/Tomphany.jpg";
    }
    UserIcon = new QPixmap(UserIconUrl);
    painter.drawPixmap(QRect((frameSize_->width() - 80) / 2, 80, 80, 80), *UserIcon);

    if (host_name_ != "") {
        QFont nameFont("Microsoft Yahei");
        nameFont.setPixelSize(21);
        QFontMetrics font_matrics(nameFont);
        int name_width = font_matrics.width(host_name_);
        int name_height = font_matrics.lineSpacing();
        NameLabel->setGeometry((frameSize_->width() - name_width) / 2, 180, 115, name_height);
        NameLabel->setFont(nameFont);
        NameLabel->setText(host_name_);
    }
}

void WF_LoginWin::doSetupNetwork()
{
    ip_ = configSetting->value("Network/SERVER_IP", WF_SERVER_IP).toString();
    port_ = configSetting->value("Network/SERVER_PORT", WF_SERVER_PORT).toInt();

    qDebug() << "Server IP: " + ip_;
    qDebug() << "Server Port: " + QString::number(port_);
    MainSocket = new WF_MainSocket(ip_, port_, cryptor_);
    connect(MainSocket, SIGNAL(eConnected()), this, SLOT(sConnected()));
    connect(MainSocket, SIGNAL(eUnconnected()), this, SLOT(sUnconnected()));
    connect(MainSocket, SIGNAL(eRegistered(int, QString, QString)), this, SLOT(sRegistered(int, QString, QString)));
    connect(MainSocket, SIGNAL(eExpired(int, QString)), this, SLOT(sExpired(int, QString)));
    connect(MainSocket, SIGNAL(eRejected(int)), this, SLOT(sRejected(int)));
    connect(MainSocket, SIGNAL(eValidate(int, int, QString, QString)), this, SLOT(sValidate(int, int, QString, QString)));
    connect(MainSocket, SIGNAL(eInvalidate(int)), this, SLOT(sInvalidate(int)));

    emit eReInitFileSocket(ip_, port_ + 1);
}

void WF_LoginWin::doVaildate()
{
    if (MainSocket == NULL)
        return;
    int account = configSetting->value("User/ACCOUNT", 0).toInt();
    QString encrypted_password = configSetting->value("User/PW", "xxxxxx").toString();
    QString password = cryptor_.Decrypt(encrypted_password.toUtf8());
    if (account != 0 && password != "xxxxxx") {
        MainSocket->doPermission(account, password);
    } else {
        qDebug() << "Failed to get account and password";
        doSwitch();
    }
}

void WF_LoginWin::doConfigure()
{
    wf_config_file_ = new QFile(WF_ConfigPath_);
    if (wf_config_file_->exists()) {
        ip_ = configSetting->value("Network/SERVER_IP", WF_SERVER_IP).toString();
        port_ = configSetting->value("Network/SERVER_PORT", WF_SERVER_PORT).toInt();
        qDebug() << "Succeed to Read Config";
        qDebug() << "Server IP: " + ip_;
        qDebug() << "Server Port: " + QString::number(port_);
    }
}

void WF_LoginWin::doLogin()
{
    if (network_ready_ == false) {
        doModify();
        return;
    } else if (account_validated_ == false) {
        doSwitch();
        return;
    }
    MainSocket->doSayhello(account_);
    MainWin = new WF_MainWin(this, this->MainSocket, this->FileSocket, UserIconUrl);
    connect(MainWin, SIGNAL(eSendFile(int, QString, int, QString)), FileSocket, SLOT(sSendFile(int, QString, int, QString)));
    connect(FileSocket, SIGNAL(eSendFinished(QString)), MainWin, SLOT(sSendFinished(QString)));
    MainWin->setGeometry((mainMonitorSize_->width() - mainWinSize_->width()) / 2,
                         (mainMonitorSize_->height() - mainWinSize_->height()) / 2,
                         mainWinSize_->width(),
                         mainWinSize_->height());
    this->hide();
//    this->close();
    MainWin->show();
}

void WF_LoginWin::doSwitch()
{
    LoginSwitchWin->setGeometry(this->pos().rx() + 285, this->pos().ry(), 320, 210);
    LoginSwitchWin->show();
}

void WF_LoginWin::doModify()
{
    LoginSettingWin->setGeometry(this->pos().rx() + 285, this->pos().ry(), 280, 320);
    LoginSettingWin->show();
}

void WF_LoginWin::doRegister()
{
    LoginRegisterWin->setGeometry(this->pos().rx() + 285, this->pos().ry(), 440, 270);
    LoginRegisterWin->show();
}

void WF_LoginWin::sUpdateNetworkConfig()
{
    doSetupNetwork();
}

void WF_LoginWin::sConnected()
{
    network_ready_ = true;
    doVaildate();
}

void WF_LoginWin::sUnconnected()
{
    network_ready_ = false;
}

void WF_LoginWin::sUserRegister(QString name, QString password, QString invite_code, QString icon_path)
{
    qDebug() << "Name: " << name;
    qDebug() << "Password: " << password;
    qDebug() << "InviteCode: " << invite_code;
    qDebug() << "IconPath: " << icon_path;
    MainSocket->doRegister(name, password, invite_code, icon_path);
}

void WF_LoginWin::sUserSwitch(QString account, QString password)
{
    qDebug() << "Account: " << account;
    qDebug() << "Password: " << password;
    configSetting->setValue("/User/ACCOUNT", account);
    QString encrypted_password = cryptor_.Encrypt(password.toUtf8());
    configSetting->setValue("/User/PW", encrypted_password);
    doVaildate();
}

void WF_LoginWin::sRegistered(int status_code, QString account, QString password)
{
    qDebug() << "Registered - Status Code: " << status_code;
    qDebug() << "Account: " << account;
    qDebug() << "Password: " << password;
    QMessageBox::information(this, "WeFish Info", "注册成功，请记住你的ID：" + account, QMessageBox::Yes);
    configSetting->setValue("/User/ACCOUNT", account);
    QString encrypted_password = cryptor_.Encrypt(password.toUtf8());
    configSetting->setValue("/User/PW", encrypted_password);
    configSetting->sync();
    doVaildate();
}

void WF_LoginWin::sExpired(int status_code, QString new_invite_code)
{
    qDebug() << "Expired - Status Code: " << status_code;
    qDebug() << "New Invite Code: " << new_invite_code;
    QMessageBox::information(this, "WeFish Info", "邀请码过期，新邀请码为：" + new_invite_code, QMessageBox::Yes);
}

void WF_LoginWin::sRejected(int status_code)
{
    qDebug() << "Rejected - Status Code: " << status_code;
    QMessageBox::information(this, "WeFish Info", "注册失败，邀请码错误！", QMessageBox::Yes);
}

void WF_LoginWin::sValidate(int status_code, int account, QString name, QString icon)
{
    qDebug() << "Validate - Status Code: " << status_code;
    qDebug() << "Name: " << name;
    QPixmap image = ImageHandler->Base64ToImage(icon.toUtf8());
    image.save(WF_ALL_DIR + "\\pcache", "JPEG");
    UserIconUrl = WF_ALL_DIR + "\\pcache";
    account_ = account;
    host_name_ = name;
    account_validated_ = true;
    emit eUpgradeRequest(account_);
    this->update();
}

void WF_LoginWin::sInvalidate(int status_code)
{
    qDebug() << "Invalidate - Status Code: " << status_code;
    account_validated_ = false;
    if (!version_expired_) {
        doSwitch();
    }
}

void WF_LoginWin::sVersionExpired()
{
    qDebug() << "Version Expired!!!";
    version_expired_ = true;
    this->hide();
    UpgraderWin->setGeometry(this->pos().rx() - 100, this->pos().ry(), 500, 300);
    UpgraderWin->show();
    emit eUpgradeRequest(0);
}

void WF_LoginWin::sVersionLatest()
{
    qDebug() << "Version Latest!!!";
    version_expired_ = false;
}
