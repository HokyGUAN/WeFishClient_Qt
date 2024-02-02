/**********************************
 *
 *           WeFish Misc
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_misc.h"
#include "QMouseEvent"
#include <QDebug>

WF_ImageHandler::WF_ImageHandler()
{
}

WF_ImageHandler::~WF_ImageHandler()
{
}

QByteArray WF_ImageHandler::ImageToBase64(QString ImgPath)
{
    QImage image(ImgPath);
    QPixmap pixmap = QPixmap::fromImage(image);
    QByteArray byteArray;
    QBuffer buf(&byteArray);

    pixmap.save(&buf, "jpg", 20);
    QByteArray hexed = byteArray.toBase64();
    buf.close();

    return hexed;
}

QPixmap WF_ImageHandler::Base64ToImage(QByteArray byteArray) {
    QByteArray Ret_byteArray = QByteArray::fromBase64(byteArray);
    QBuffer buffer(&Ret_byteArray);
    buffer.open(QIODevice::WriteOnly);
    QPixmap image;
    image.loadFromData(Ret_byteArray);

    return image;
}

WF_Setting::WF_Setting(QWidget * parent) : QLabel(parent)
{
    this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
    menu = new QMenu("个人信息");
    change_icon = new QAction("更换头像", this);
    setting = new QAction("设置", this);
    menu->addAction(change_icon);
    //menu->addSeparator();
    menu->addAction(setting);
    menu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    //menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setStyleSheet("QMenu {"
                        "width:140px;"
                        "height:90px;"
                        "font-size:14px;"
                        "background-color: rgb(41, 41, 41);"
                        "color: rgb(140,140,140);"
                        "border-radius: 0px; }"
                        "QMenu::item {"
                        "width:140px;"
                        "height:45px;"
                        "padding-left:10px;"
                        "margin:0px;"
                        "background-color: transparent;}"
                        "QMenu::item:selected {"
                        "background-color: rgb(48, 49, 51);}");

   connect(change_icon, SIGNAL(triggered()), this, SLOT(sChangeIcon()));
}

void WF_Setting::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
        QPoint gp = event->globalPos();
        QPoint pos = mapToGlobal(this->pos());
        int Xoffset = gp.rx() - pos.rx();
        menu->exec(QPoint(gp.rx() - Xoffset + 30, gp.ry() - 70));
    }
}

void WF_Setting::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
}

void WF_Setting::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setToolTip("更多");
    this->setStyleSheet("border-image:url(:/Res/whiteSetting.png);outset;");
}

void WF_Setting::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    this->setStyleSheet("border-image:url(:/Res/graySetting.png);outset;");
}

void WF_Setting::sChangeIcon()
{
    Image_Path = QFileDialog::getOpenFileName(this,"Choose Icon","",
                                                  "*Image Files(*.jpg;*.png;*.bmp);;All(*.*)");
    if (Image_Path != "") {
        QPixmap icon(Image_Path);
        icon.save(WF_DIR + "\\Self.jpg");

        emit eChangedIcon(Image_Path);
    }
}

WF_LoginSettingWin::WF_LoginSettingWin(QWidget * parent) : WF_BaseWin(nullptr)
{
    Q_UNUSED(parent)
    this->setMinimumSize(280, 320);
    this->setMaximumSize(280, 320);
    this->CloseButton->setGeometry(247, 0, 33, 24);

    configSetting = new QSettings(WF_DIR+"\\Config.ini", QSettings::IniFormat);

    IPLabel = new QLabel(this);
    IPLabel->setGeometry(35, 80, 100, 25);
    IPLabel->setText("IP: ");
    IPLabel->setFont(QFont("Microsoft Yahei", 12));
    PortLabel = new QLabel(this);
    PortLabel->setGeometry(35, 145, 100, 25);
    PortLabel->setText("Port: ");
    PortLabel->setFont(QFont("Microsoft Yahei", 12));

    IPLineEdit = new QLineEdit(this);
    IPLineEdit->setGeometry(130, 80, 100, 25);
    IPLineEdit->setFont(QFont("Microsoft Yahei", 12));
    IPLineEdit->setText(configSetting->value("Network/SERVER_IP", "10.13.3.23").toString());
    PortLineEdit = new QLineEdit(this);
    PortLineEdit->setGeometry(130, 145, 100, 25);
    PortLineEdit->setFont(QFont("Microsoft Yahei", 12));
    PortLineEdit->setText(configSetting->value("Network/SERVER_PORT", "6688").toString());

    ModifyBtn = new QPushButton(this);
    ModifyBtn->setGeometry(50, 235, 180, 35);
    ModifyBtn->setFont(QFont("Microsoft Yahei", 10));
    ModifyBtn->setText("保存配置");
    ModifyBtn->setStyleSheet("background: rgb(7, 193, 96); border-radius:4px; color:rgb(255, 255, 255);outset;");

    connect(ModifyBtn, &QPushButton::clicked, this, &WF_LoginSettingWin::doSave);
}

void WF_LoginSettingWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(0, 0, 280, 320, 0, 0);
    painter.setPen(QPen(Qt::gray));
    painter.setFont(QFont("Microsoft Yahei", 10));
    painter.drawText(QRect(8, 7, 50, 20), "WeFish");
}


void WF_LoginSettingWin::doSave()
{
    if (IPLineEdit->text() == "" || PortLineEdit->text() == "") {
        QMessageBox::information(this, "WeFish Info", "不能为空！", QMessageBox::Yes);
    } else {
        qDebug() << "Update Configuration - IP: " << IPLineEdit->text();
        configSetting->setValue("/Network/SERVER_IP", IPLineEdit->text());
        qDebug() << "Update Configuration - Port: " << PortLineEdit->text();
        configSetting->setValue("/Network/SERVER_PORT", PortLineEdit->text());
        configSetting->sync();

        emit eUpdateNetworkConfig();
        this->close();
    }
}

WF_LoginRegisterWin::WF_LoginRegisterWin(QWidget * parent) : WF_BaseWin(nullptr)
{
    Q_UNUSED(parent)
    this->setMinimumSize(440, 270);
    this->setMaximumSize(440, 270);
    this->CloseButton->setGeometry(407, 0, 33, 24);

    configSetting = new QSettings(WF_DIR+"\\Config.ini", QSettings::IniFormat);

    NameLabel = new QLabel(this);
    NameLabel->setGeometry(160, 50, 120, 25);
    NameLabel->setText("Name: ");
    NameLabel->setFont(QFont("Microsoft Yahei", 12));
    PasswordLabel = new QLabel(this);
    PasswordLabel->setGeometry(160, 95, 120, 25);
    PasswordLabel->setText("Password: ");
    PasswordLabel->setFont(QFont("Microsoft Yahei", 12));
    CodeLabel = new QLabel(this);
    CodeLabel->setGeometry(160, 140, 120, 25);
    CodeLabel->setText("InviteCode: ");
    CodeLabel->setFont(QFont("Microsoft Yahei", 12));

    NameLineEdit = new QLineEdit(this);
    NameLineEdit->setGeometry(260, 50, 130, 25);
    NameLineEdit->setFont(QFont("Microsoft Yahei", 12));
    PasswordLineEdit = new QLineEdit(this);
    PasswordLineEdit->setGeometry(260, 95, 130, 25);
    PasswordLineEdit->setFont(QFont("Microsoft Yahei", 12));
    PasswordLineEdit->setEchoMode(QLineEdit::Password);
    CodeLineEdit = new QLineEdit(this);
    CodeLineEdit->setGeometry(260, 140, 130, 25);
    CodeLineEdit->setFont(QFont("Microsoft Yahei", 12));

    ChooseIconBtn = new QPushButton(this);
    ChooseIconBtn->setGeometry(50, 135, 80, 20);
    ChooseIconBtn->setFont(QFont("Microsoft Yahei", 10));
    ChooseIconBtn->setText("修改头像");
    ChooseIconBtn->setStyleSheet("background: rgb(255, 255, 255); border-radius:4px; color:rgb(67, 104, 149);outset;");

    RegisterBtn = new QPushButton(this);
    RegisterBtn->setGeometry(130, 200, 180, 35);
    RegisterBtn->setFont(QFont("Microsoft Yahei", 10));
    RegisterBtn->setText("注册");
    RegisterBtn->setStyleSheet("background: rgb(7, 193, 96); border-radius:4px; color:rgb(255, 255, 255);outset;");

    connect(ChooseIconBtn, &QPushButton::clicked, this, &WF_LoginRegisterWin::doChooseIcon);
    connect(RegisterBtn, &QPushButton::clicked, this, &WF_LoginRegisterWin::doRegister);
}

void WF_LoginRegisterWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(0, 0, 440, 270, 0, 0);
    painter.setPen(QPen(Qt::gray));
    painter.setFont(QFont("Microsoft Yahei", 10));
    painter.drawText(QRect(8, 7, 50, 20), "WeFish");

    if (UserIconUrl == "") {
        UserIcon = new QPixmap(":/Res/Tomphany.jpg");
    } else {
        UserIcon = new QPixmap(UserIconUrl);
    }
    painter.drawPixmap(QRect(50, 50, 80, 80), *UserIcon);
}

void WF_LoginRegisterWin::doChooseIcon()
{
    QString Image_Path = QFileDialog::getOpenFileName(this,"Choose Icon","",
                                                  "*Image Files(*.jpg;*.png;*.bmp);;All(*.*)");
    if (Image_Path != "") {
        ImagePath = Image_Path;
        UserIconUrl = Image_Path;
        this->update();
    }
}

void WF_LoginRegisterWin::doRegister()
{
    if (NameLineEdit->text() == "" || PasswordLineEdit->text() == "" || \
        CodeLineEdit->text() == "" || ImagePath == "") {
        QMessageBox::information(this, "WeFish Info", "不能为空, 包括头像！", QMessageBox::Yes);
    } else {
        emit eUserRegister(NameLineEdit->text(), PasswordLineEdit->text(), CodeLineEdit->text(), ImagePath);
        this->close();
    }
}

WF_LoginSwitchWin::WF_LoginSwitchWin(QWidget * parent) : WF_BaseWin(nullptr)
{
    Q_UNUSED(parent)
    this->setMinimumSize(320, 210);
    this->setMaximumSize(320, 210);
    this->CloseButton->setGeometry(287, 0, 33, 24);

    AccountLabel = new QLabel(this);
    AccountLabel->setGeometry(40, 50, 120, 25);
    AccountLabel->setText("Account: ");
    AccountLabel->setFont(QFont("Microsoft Yahei", 12));
    PasswordLabel = new QLabel(this);
    PasswordLabel->setGeometry(40, 100, 120, 25);
    PasswordLabel->setText("Password: ");
    PasswordLabel->setFont(QFont("Microsoft Yahei", 12));

    AccountLineEdit = new QLineEdit(this);
    AccountLineEdit->setGeometry(140, 50, 130, 25);
    AccountLineEdit->setFont(QFont("Microsoft Yahei", 12));
    PasswordLineEdit = new QLineEdit(this);
    PasswordLineEdit->setGeometry(140, 100, 130, 25);
    PasswordLineEdit->setFont(QFont("Microsoft Yahei", 12));
    PasswordLineEdit->setEchoMode(QLineEdit::Password);

    SwitchBtn = new QPushButton(this);
    SwitchBtn->setGeometry(70, 150, 180, 35);
    SwitchBtn->setFont(QFont("Microsoft Yahei", 10));
    SwitchBtn->setText("切换用户");
    SwitchBtn->setStyleSheet("background: rgb(7, 193, 96); border-radius:4px; color:rgb(255, 255, 255);outset;");

    connect(SwitchBtn, &QPushButton::clicked, this, &WF_LoginSwitchWin::doUserSwitch);
}

void WF_LoginSwitchWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(0, 0, 320, 210, 0, 0);
    painter.setPen(QPen(Qt::gray));
    painter.setFont(QFont("Microsoft Yahei", 10));
    painter.drawText(QRect(8, 7, 50, 20), "WeFish");
}

void WF_LoginSwitchWin::doUserSwitch()
{
    if (AccountLineEdit->text() == "" || PasswordLineEdit->text() == "") {
        QMessageBox::information(this, "WeFish Info", "不能为空！", QMessageBox::Yes);
    } else {
        eUserSwitch(AccountLineEdit->text(), PasswordLineEdit->text());
        this->close();
    }
}

WF_Filer::WF_Filer(QString FilePath) : filer_(new QFile(FilePath))
{
}

QString WF_Filer::Read(QString contain, QString delimiter, int num)
{
    QString readRet;
    QStringList ConfigRetList;
    if (filer_->exists()) {
        filer_->open(QIODevice::ReadOnly);
        while (filer_->atEnd() == false) {
           readRet = filer_->readLine();
           if(readRet.contains(contain, Qt::CaseSensitive)) {
               ConfigRetList = readRet.split(delimiter, QString::SkipEmptyParts);
               if (ConfigRetList.size() > num) return ConfigRetList.at(num);
           }
        }
        filer_->close();
    }
    return "";
}

