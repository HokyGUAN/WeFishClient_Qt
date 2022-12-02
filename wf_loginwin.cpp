/**********************************
 *
 *      WeFish Login Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_loginwin.h"

#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

WF_LoginWin::WF_LoginWin(QWidget *parent, QSize* frameSize)
    : QWidget(parent)
    , frameSize_(frameSize)
    , host_name_("")
{
    QSize mainWinSize(920, 635);
    QScreen *monitor = qApp->primaryScreen();
    this->setGeometry((monitor->size().width() - frameSize_->width()) / 2,
                  (monitor->size().height() - frameSize_->height()) / 2,
                  frameSize_->width(),
                  frameSize_->height());
    this->setWindowTitle("WeFish");
    this->setWindowIcon(QIcon(QPixmap(":/Res/Tomphany.jpg")));

    int screen_number = QApplication::desktop()->screenNumber(this);
    if (screen_number < 0) screen_number = 0;
    QSize mainMonitorSize = QGuiApplication::screens().at(screen_number)->geometry().size();

    QString SelfIconPath = WF_DIR + "\\Self.jpg";
    if (QFile(SelfIconPath).exists()) {
        UserIconUrl = SelfIconPath;
    } else {
        UserIconUrl = ":/Res/Tomphany.jpg";
    }
    UserIcon = new QPixmap(UserIconUrl);

    QString readRet;
    QStringList ConfigRetList;
    QString WF_ConfigPath = WF_DIR + "\\Config";
    wf_config_file_ = new QFile(WF_ConfigPath);
    if (wf_config_file_->exists()) {
        configFileExist_ = true;

        wf_config_file_->open(QIODevice::ReadOnly);
        while (wf_config_file_->atEnd() == false) {
           readRet = wf_config_file_->readLine();
           if(readRet.contains("<>HOSTNAME<>",Qt::CaseSensitive)) {
               ConfigRetList = readRet.split("<>", QString::SkipEmptyParts);
               if (ConfigRetList.size() > 1) host_name_ = ConfigRetList.at(1);
           }
           if(readRet.contains("<>SERVER<>",Qt::CaseSensitive)) {
               ConfigRetList = readRet.split("<>", QString::SkipEmptyParts);
               if (ConfigRetList.size() > 2) {
                   ip_ = ConfigRetList.at(1);
                   port_ = ConfigRetList.at(2).toInt();
               } else {
                   ip_ = "103.46.128.53";
                   port_ = 55817;
               }
           }
        }
        wf_config_file_->close();
        qDebug() << "Succeed to Read Config HostName:" + host_name_;
        qDebug() << "Server Address:" + ip_ + "-" + QString::number(port_);
        QFont nameFont("Microsoft Yahei", 16);
        QFontMetrics font_matrics(nameFont);
        int name_width = font_matrics.width(host_name_);
        int name_height = font_matrics.lineSpacing();
        NameLabel = new QLabel(this);
        NameLabel->setGeometry((frameSize_->width() - name_width) / 2, 150, 115, name_height);
        NameLabel->setFont(nameFont);
        NameLabel->setText(host_name_);
    } else {
        configFileExist_ = false;
        LineInput = new QLineEdit(this);
        LineInput->setGeometry((frameSize_->width() - 115) / 2, 150, 115, 25);
        LineInput->setPlaceholderText("Name");
        LineInput->setFont(QFont("Microsoft Yahei", 13));
    }

    LoginButton = new QPushButton(this);
    LoginButton->setGeometry((frameSize_->width() - 180) / 2, 235, 180, 35);
    LoginButton->setFont(QFont("Microsoft Yahei", 11));
    LoginButton->setText("进入微娱");
    LoginButton->setStyleSheet("background: rgb(7, 193, 96); border-radius:4px; color:rgb(255, 255, 255);outset;");

    connect(LoginButton, &QPushButton::clicked, [=] {
        if (configFileExist_ == false) {
            wf_config_file_->open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
            QString StrToWrite = "<>HOSTNAME<>" + LineInput->text() + "<>\n";
            wf_config_file_->write(StrToWrite.toUtf8());
            StrToWrite = "<>SERVER<>103.46.128.53<>55817<>";
            wf_config_file_->write(StrToWrite.toUtf8());
            host_name_ = LineInput->text();
            ip_ = "103.46.128.53";
            port_ = 55817;
            wf_config_file_->close();
        }
        if (host_name_ != "") {
            this->TcpSocket = new WF_TcpSocket(host_name_, UserIconUrl, ip_, port_);
            MainWin = new WF_MainWin(this, this->TcpSocket, UserIconUrl);
            MainWin->setGeometry((mainMonitorSize.width() - mainWinSize.width()) / 2,
                                 (mainMonitorSize.height() - mainWinSize.height()) / 2,
                                 mainWinSize.width(),
                                 mainWinSize.height());
            this->hide();
            MainWin->show();
        }
    });
}

WF_LoginWin::~WF_LoginWin()
{
}

void WF_LoginWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(0, 0, frameSize_->width(), frameSize_->height(), 3, 3);

    painter.drawPixmap(QRect((frameSize_->width() - 80) / 2, 50, 80, 80), *UserIcon);
}


