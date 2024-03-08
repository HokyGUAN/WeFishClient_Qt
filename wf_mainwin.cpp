/**********************************
 *
 *       WeFish Main Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_mainwin.h"
#include "common/base64.hpp"

#include <QPainter>
#include <QDebug>
#include <QTextBlock>
#include <QApplication>
#include <QMainWindow>
#include <QScreen>


WF_MainWin::WF_MainWin(QWidget *parent, WF_MainSocket* mainSocket,
                       WF_FileSocket* fileSocket, QString UserIconUrl)
    : QWidget(nullptr)
    , width_(920)
    , height_(635)
    , MainSocket(mainSocket)
    , FileSocket(fileSocket)
    , userIconUrl_(UserIconUrl)
{
    Q_UNUSED(parent)
    isClosed = false;
    this->setWindowTitle("WeFish");
    this->setWindowIcon(QIcon(QPixmap(":/Res/Tomphany.jpg")));
    this->setWindowFlags(Qt::FramelessWindowHint);

    SetFrame_ = QRectF(0, 0, 55, height_);
    ListPos_ = QPoint(SetFrame_.width(), 60);
    ListFrame_ = QRectF(ListPos_.x(), ListPos_.y() + 1, 245, height_ - ListPos_.y());
    ChatViewPos_ = QPoint(ListPos_.x() + ListFrame_.width() + 1, ListPos_.y() + 1);
    ChatViewFrame_ = QRectF(ChatViewPos_.x(), ChatViewPos_.y(), width_ - ChatViewPos_.x(), 434);
    ChatInputPos_ = QPoint(ChatViewPos_.x() + 23, 530);
    ChatInputFrame_ = QRectF(ChatInputPos_.x(), ChatInputPos_.y(), width_ - ChatInputPos_.x(), height_ - ChatInputPos_.y() - 45);
    SendBtnPos_ = QPoint(820, 595);
    SendBtnFrame_ = QRectF(SendBtnPos_.x(), SendBtnPos_.y(), 90, 35);

    UserIcon = new QLabel(this);
    UserIcon->setGeometry(7, 37, 40, 40);
    UserIcon->setPixmap(QPixmap(userIconUrl_).scaled(40, 40));
    ChattingIcon = new QLabel(this);
    ChattingIcon->setGeometry(12, 95, 30, 30);
    ChattingIcon->setPixmap(QPixmap(":/Res/chatIcon.png").scaled(30, 30));
    Setting = new WF_Setting(this);
    Setting->setGeometry(12, 592, 30, 30);

    ChatView = new WF_ChatView(this);
    ChatView->setGeometry(QRect(ChatViewFrame_.x(), ChatViewFrame_.y(), ChatViewFrame_.width(), ChatViewFrame_.height()));

    FriendList = new WF_FriendListView(this, MainSocket->name_);
    FriendList->setGeometry(QRect(ListFrame_.x(), ListFrame_.y(), ListFrame_.width(), ListFrame_.height()));
    currentItem_ = FriendList->GetFriend(0);

    ChatInput = new WF_ChatInput(this);
    ChatInput->setGeometry(ChatInputPos_.x(), ChatInputPos_.y(), ChatInputFrame_.width(), ChatInputFrame_.height());
    ChatInput->installEventFilter(this);

    SendButton = new WF_Button(this, "发送(S)", false);
    SendButton->setGeometry(790, 590, 100, 32);

    CloseButton  = new WF_CloseButton(this, 33, 24);
    CloseButton->setGeometry(width_ - 33, 0, 33, 24);

    MinimizeButton = new WF_MinimizeButton(this, 33, 24);
    MinimizeButton->setGeometry(width_ - 66, 0, 33, 24);

    ChatBoxEmojiButton = new WF_ChatBoxEmojiButton(this, 30, 30);
    ChatBoxEmojiButton->setGeometry(ChatViewPos_.x() + 20, 500, 30, 30);

    ChatBoxFileButton = new WF_ChatBoxFileButton(this, 30, 30);
    ChatBoxFileButton->setGeometry(ChatViewPos_.x() + 57, 499, 30, 30);

    NameLabel = new QLabel(this);
    NameLabel->setGeometry(340, 8, 400, 40);
    QFont font = QFont("Microsoft Yahei");
    font.setPixelSize(21);
    NameLabel->setFont(font);
    ImageHandler = new WF_ImageHandler();
    Icon_ = ImageHandler->ImageToBase64(userIconUrl_);

    connect(FriendList, &WF_FriendListView::ClickSig, [=](UserItemData itemdata) {NameLabel->setText(itemdata.Name);});
    connect(FriendList, SIGNAL(ClickSig(UserItemData)), this, SLOT(Flush(UserItemData)));
    connect(FriendList, SIGNAL(AddOthersMessageSig(QPixmap, QString, QString, bool)), ChatView, SLOT(NotifyOthersMessage(QPixmap, QString, QString, bool)));
    connect(FriendList, SIGNAL(AddSelfMessageSig(QPixmap, QString, bool)), ChatView, SLOT(NotifySelfMessage(QPixmap, QString, bool)));
    connect(CloseButton, &QPushButton::clicked, [this] {this->close();});
    connect(MinimizeButton, &QPushButton::clicked, [this] {this->setWindowState(Qt::WindowMinimized);});
    connect(SendButton, SIGNAL(clicked()), this, SLOT(Send()));
    connect(ChatInput, SIGNAL(SendByKeySig()), this, SLOT(Send()));
    connect(ChatBoxFileButton, &QPushButton::clicked, this, &WF_MainWin::doSendFile);
    connect(MainSocket, SIGNAL(eAccountCheckin(int)), FriendList, SLOT(sAccountCheckin(int)));
    connect(MainSocket, &WF_MainSocket::eAccountCheckin, [=](int MyAccount) {account_ = MyAccount; NameLabel->setText("Group"); qDebug() << "MainWin Get My Account:" << account_;});
    connect(MainSocket, SIGNAL(eSayHello(int, QString, QPixmap)), FriendList, SLOT(sSayHello(int, QString, QPixmap)));
    connect(MainSocket, SIGNAL(eOnline(int, QString, QPixmap)), FriendList, SLOT(sOnline(int, QString, QPixmap)));
    connect(MainSocket, SIGNAL(eOffline(int, QString)), FriendList, SLOT(sOffline(int, QString)));
    connect(MainSocket, &WF_MainSocket::eNotify, [this] {QApplication::alert(this);});
    connect(MainSocket, SIGNAL(eNotify(int, QString, QPixmap, int, QString, QString)), FriendList, SLOT(sNotify(int, QString, QPixmap, int, QString, QString)));
    connect(MainSocket, &WF_MainSocket::applicationshutdown, this, &WF_MainWin::ApplicationShutDown);
    connect(MainSocket, &WF_MainSocket::servershutdown, this, &WF_MainWin::ApplicationShutDown);
    connect(FileSocket, SIGNAL(eNotify(int, QString, QPixmap, int, QString, QString)), FriendList, SLOT(sNotify(int, QString, QPixmap, int, QString, QString)));
    connect(ChatView, SIGNAL(PictureContentClicked(QPixmap)), this, SLOT(PictureBrowser(QPixmap)));
    connect(Setting, SIGNAL(eChangedIcon(QString)), FriendList, SLOT(sChangedIcon(QString)));
    connect(Setting, &WF_Setting::eChangedIcon, [=](QString imagePath) {this->UserIcon->setPixmap(QPixmap(imagePath).scaled(40, 40));});
}

WF_MainWin::~WF_MainWin()
{
}

void WF_MainWin::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(245, 245, 245)));
    painter.drawRoundedRect(0, 0, width_, height_, 0, 0);

    painter.setBrush(QBrush(QColor(46,46,46)));
    painter.drawRect(SetFrame_);

    painter.setPen(QColor(198,196,196));
    painter.drawLine(ListPos_, QPoint(width_, 60));
    painter.drawLine(QPoint(300, 0), QPoint(300, height_));
    painter.drawLine(QPoint(301, 496), QPoint(width_, 495));
}

void WF_MainWin::Send()
{
    if (ChatInput->toPlainText() != NULL) {
        QString msg = "";
        QString pic_msg = "";
        QString content_type = "Content";
        QTextDocument *doc = ChatInput->document();
        QTextBlock block = doc->begin();
        QTextCharFormat fmt = block.begin().fragment().charFormat();
        if (fmt.isImageFormat()) {
            content_type = "PicContent";
            if (fmt.toImageFormat().name().split("///").size() > 1) {
                QString path = fmt.toImageFormat().name().split("///")[1];

                QByteArray byteArray = ImageHandler->ImageToBase64(path);
                msg = byteArray.data();
                pic_msg = byteArray.data();
            }
        } else {
            msg = ChatInput->toPlainText();
        }

        myItemdata_ = FriendList->GetFriend(account_);
        UserItemData Friend = FriendList->GetFriend(currentItemAccount_);
        int FriendindexInList = FriendList->GetItemIndexInList(currentItemAccount_);

        if (msg == "") return;

        Friend.HistoryMsg.push_back(QString::number(myItemdata_.Account) + ":" + myItemdata_.Name + ":" + content_type + ":" + msg);
        FriendList->SetFriend(FriendindexInList, Friend);

        jsonrpcpp::request_ptr request(nullptr);
        request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_CONTENT), content_type.toStdString(), \
                                             jsonrpcpp::Parameter("Account", account_, \
                                                                  "Name", myItemdata_.Name.toUtf8(), \
                                                                  "Icon", Icon_, \
                                                                  "ToAccount", currentItem_.Account, \
                                                                  "Content", msg.toUtf8())));
        msg = QString(request->to_json().dump().data());

        QByteArray cipher = MainSocket->cryptor_.Encrypt(msg.toUtf8());
        msg = cipher + "\r\n";

        MainSocket->socket->write(msg.toUtf8());

        if (content_type == "PicContent") {
            ChatView->AppendSelfMessage(myItemdata_.Pic, pic_msg, true);
        } else {
            ChatView->AppendSelfMessage(myItemdata_.Pic, ChatInput->toPlainText(), false);
        }

        ChatInput->clear();
    }
}

void WF_MainWin::doSendFile()
{
    QString FilePath = QFileDialog::getOpenFileName(this,"Choose File","","All(*.*)");
    if (FilePath == "") return;

    myItemdata_ = FriendList->GetFriend(account_);
    UserItemData Friend = FriendList->GetFriend(currentItemAccount_);
    int FriendindexInList = FriendList->GetItemIndexInList(currentItemAccount_);

    QStringList list = FilePath.split("/", QString::SkipEmptyParts);
    QString file_name = list.at(list.size() - 1);

    QString file_location = WF_DIR + "\\wfid_" + QString::number(account_) + "\\" + file_name;
    file_location = file_location.replace("\\", "/");
    file_location = file_location.replace(":", "");

    Friend.HistoryMsg.push_back(QString::number(myItemdata_.Account) + ":" + myItemdata_.Name + \
                                ":Content:" + file_location);
    FriendList->SetFriend(FriendindexInList, Friend);

    emit eSendFile(account_, MainSocket->name_, currentItem_.Account, FilePath);
}

void WF_MainWin::sSendFinished(QString response)
{
    ChatView->AppendSelfMessage(myItemdata_.Pic, response, false);
}

void WF_MainWin::Flush(UserItemData itemdata)
{
    this->ChatView->Clear();
    currentItem_ = itemdata;
    currentItemAccount_ = currentItem_.Account;

    UserItemData tmpItemdata;
    UserItemData myItemdata = FriendList->GetFriend(account_);

    //qDebug() << "My Account:" << myItemdata.Account << " My Name:" << myItemdata.Name;
    //qDebug() << __FUNCTION__ << ": " << itemdata.Account << "-" << itemdata.Name;
    for (auto msg : itemdata.HistoryMsg) {
        int sender_account = 0;
        QString sender_name = "";
        QString content_type = "";
        QString content = "";
        QStringList savedContent = msg.split(":", QString::SkipEmptyParts);
        if (savedContent.size() == 4) {
            sender_account = savedContent.at(0).toInt();
            sender_name = savedContent.at(1);
            content_type = savedContent.at(2);
            content = savedContent.at(3);
        }

        if (sender_account == 0 || sender_name == "" || content_type == "" || content == "") {
            return;
        }

        tmpItemdata = FriendList->GetFriend(sender_account);
        if (!tmpItemdata.Available) {
            tmpItemdata.Pic = FriendList->user_icon_map[sender_account];
        }

        if (account_ != tmpItemdata.Account) {
            if (content_type == "PicContent") {
                this->ChatView->AppendOtherMessage(tmpItemdata.Pic, sender_name, content, true);
            } else {
                this->ChatView->AppendOtherMessage(tmpItemdata.Pic, sender_name, content, false);
            }
        } else {
            if (content_type == "PicContent") {
                this->ChatView->AppendSelfMessage(myItemdata.Pic, content, true);
            } else {
                this->ChatView->AppendSelfMessage(myItemdata.Pic, content, false);
            }
        }
    }
}

void WF_MainWin::ApplicationShutDown(ShutDownReason reason)
{
    if (isClosed) return;
    switch (reason) {
    case REASON_USERCONFLICT: {
        QMessageBox::warning(NULL, "WeFish Warning", "用户已登录，请切换用户", QMessageBox::Yes);
        break;
    }
    case REASON_SERVERLOST:
        QMessageBox::warning(NULL, "WeFish Warning", "服务器已断开", QMessageBox::Yes);
        break;
    }
    this->close();
}

void WF_MainWin::PictureBrowser(QPixmap pic)
{
    int screen_number = QApplication::desktop()->screenNumber(this);
    if (screen_number < 0) screen_number = 0;
    QSize ActivateMonitorSize = QGuiApplication::screens().at(screen_number)->geometry().size();

    QMainWindow* PicBrowserWin = new QMainWindow(this);
    PicBrowserWin->setGeometry(0, 0, ActivateMonitorSize.width(), ActivateMonitorSize.height());
    PicBrowserWin->setWindowState(Qt::WindowMaximized);
    PicBrowserWin->setWindowTitle("WeFish Photo Browser");
    PicBrowserWin->setWindowIcon(QIcon(QPixmap(":/Res/Tomphany.jpg")));

    WF_Browser* picBrowser = new WF_Browser(PicBrowserWin);
    QSize LabelFrameSize(ActivateMonitorSize.width() * 0.9, ActivateMonitorSize.height() * 0.85);
    picBrowser->setAlignment(Qt::AlignCenter);
    picBrowser->setGeometry(ActivateMonitorSize.width() * 0.05,
                            ActivateMonitorSize.height() * 0.05,
                            LabelFrameSize.width(),
                            LabelFrameSize.height());
    picBrowser->SetQpixmap(pic);
    PicBrowserWin->show();
}
