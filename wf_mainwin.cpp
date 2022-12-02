/**********************************
 *
 *       WeFish Main Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_mainwin.h"

#include <QPainter>
#include <QDebug>
#include <QTextBlock>
#include <QApplication>
#include <QMainWindow>
#include <QScreen>

WF_MainWin::WF_MainWin(QWidget *parent, WF_TcpSocket *TcpSocket, QString UserIconUrl)
    : QWidget(nullptr)
    , width_(920)
    , height_(635)
    , tcp(TcpSocket)
    , userIconUrl_(UserIconUrl)
    , currentItemId_(0)
{
    Q_UNUSED(parent)
    this->setWindowTitle("WeFish");
    this->setWindowIcon(QIcon(QPixmap(":/Res/Tomphany.jpg")));
    this->setWindowFlags(Qt::FramelessWindowHint);

    SetFrame_ = QRectF(0, 0, 55, height_);
    ListPos_ = QPoint(SetFrame_.width(), 60);
    ListFrame_ = QRectF(ListPos_.x(), ListPos_.y() + 1, 245, height_ - ListPos_.y());
    ChatViewPos_ = QPoint(ListPos_.x() + ListFrame_.width() + 1, ListPos_.y() + 1);
    ChatViewFrame_ = QRectF(ChatViewPos_.x(), ChatViewPos_.y(), width_ - ChatViewPos_.x(), 434);
    ChatInputPos_ = QPoint(ChatViewPos_.x() + 24, 496);
    ChatInputFrame_ = QRectF(ChatInputPos_.x(), ChatInputPos_.y(), width_ - ChatInputPos_.x(), height_ - ChatInputPos_.y() - 45);
    SendBtnPos_ = QPoint(820, 595);
    SendBtnFrame_ = QRectF(SendBtnPos_.x(), SendBtnPos_.y(), 90, 35);

    UserIcon = new WF_HeadIcon(this);
    UserIcon->setGeometry(7, 37, 40, 40);
    UserIcon->setPixmap(QPixmap(UserIconUrl).scaled(40, 40));

    ChatView = new WF_ChatView(this);
    ChatView->setGeometry(QRect(ChatViewFrame_.x(), ChatViewFrame_.y(), ChatViewFrame_.width(), ChatViewFrame_.height()));

    FriendList = new WF_FriendListView(this, tcp->name_);
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

    NameLabel = new QLabel(this);
    NameLabel->setGeometry(340, 8, 400, 40);
    NameLabel->setFont(QFont("Microsoft Yahei", 16));
    ImageHandler = new WF_ImageHandler();

    connect(FriendList, &WF_FriendListView::ClickSig, [=](UserItemData itemdata) {NameLabel->setText(itemdata.Name);});
    connect(FriendList, SIGNAL(ClickSig(UserItemData)), this, SLOT(Flush(UserItemData)));
    connect(FriendList, SIGNAL(AddOthersMessageSig(QPixmap, QString, QString, bool)), ChatView, SLOT(NotifyOthersMessage(QPixmap, QString, QString, bool)));
    connect(FriendList, SIGNAL(AddSelfMessageSig(QPixmap, QString, bool)), ChatView, SLOT(NotifySelfMessage(QPixmap, QString, bool)));
    connect(CloseButton, &QPushButton::clicked, [this]{this->close();});
    connect(MinimizeButton, &QPushButton::clicked, [this]{this->setWindowState(Qt::WindowMinimized);});
    connect(SendButton, SIGNAL(clicked()), this, SLOT(Send()));
    connect(ChatInput, SIGNAL(SendByKeySig()), this, SLOT(Send()));
    connect(tcp, SIGNAL(myid(int)), FriendList, SLOT(MyID(int)));
    connect(tcp, &WF_TcpSocket::myid, [=](int MyIDinGroup) {myId_ = MyIDinGroup; NameLabel->setText("Group"); qDebug() << "MainWin Get MyID:" << myId_;});
    connect(tcp, SIGNAL(sayhello(int, QString, QPixmap)), FriendList, SLOT(SayHello(int, QString, QPixmap)));
    connect(tcp, SIGNAL(online(int, QString, QPixmap)), FriendList, SLOT(Online(int, QString, QPixmap)));
    connect(tcp, SIGNAL(offline(int, QString)), FriendList, SLOT(Offline(int, QString)));
    connect(tcp, &WF_TcpSocket::notify, [this]{QApplication::alert(this);});
    connect(tcp, SIGNAL(notify(int, QString, int, QString, QString)), FriendList, SLOT(Notify(int, QString, int, QString, QString)));
    connect(ChatView, SIGNAL(PictureContentClicked(QPixmap)), this, SLOT(PictureBrowser(QPixmap)));
    connect(UserIcon, SIGNAL(ChangedIcon(QString)), FriendList, SLOT(ChangedIcon(QString)));
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

        myItemdata_ = FriendList->GetFriend(myId_);
        UserItemData Friend = FriendList->GetFriend(currentItemId_);
        int FriendindexInList = FriendList->GetItemIndexInList(currentItemId_);
        if (msg != "") {
            Friend.HistoryMsg.push_back(myItemdata_.Name + ":" + content_type + ":" + msg);
            FriendList->SetFriend(FriendindexInList, Friend);

            jsonrpcpp::request_ptr request(nullptr);
            request.reset(new jsonrpcpp::Request(jsonrpcpp::Id(MESSAGE_TYPE_CONTENT), content_type.toStdString(), \
                                                 jsonrpcpp::Parameter("IDinGroup", myId_, \
                                                                      "Who", myItemdata_.Name.toUtf8(), \
                                                                      "ToID", currentItem_.ID, \
                                                                      "Content", msg.toUtf8())));
            msg = QString(request->to_json().dump().data());

            QByteArray byteArray = msg.toUtf8();
            byteArray = byteArray.toBase64();
            msg = byteArray + "\r\n";
            tcp->socket->write(msg.toUtf8());

            if (content_type == "PicContent") {
                ChatView->AppendSelfMessage(myItemdata_.Pic, pic_msg, true);
            } else {
                ChatView->AppendSelfMessage(myItemdata_.Pic, ChatInput->toPlainText(), false);
            }
        }
        ChatInput->clear();
    }
}

void WF_MainWin::Flush(UserItemData itemdata)
{
    this->ChatView->Clear();
    currentItem_ = itemdata;
    currentItemId_ = currentItem_.ID;

    QString exact_name;
    UserItemData tmpItemdata;
    UserItemData myItemdata = FriendList->GetFriend(myId_);

    //qDebug() << "MyID:" << myItemdata.ID << " MyName:" << myItemdata.Name;
    //qDebug() << __FUNCTION__ << ": " << itemdata.ID << "-" << itemdata.Name;
    for (auto msg : itemdata.HistoryMsg) {
        QString sender_name = "";
        QString content_type = "";
        QString content = "";
        QStringList savedContent = msg.split(":", QString::SkipEmptyParts);
        if (savedContent.size() == 3) {
            sender_name = savedContent.at(0);
            content_type = savedContent.at(1);
            content = savedContent.at(2);
        }

        //qDebug() << "SenderName:" << sender_name << " Content:" << content;
        if (sender_name != "" && content_type != "" && content != "") {
            tmpItemdata = FriendList->GetFriend(sender_name);

            if (itemdata.ID == 0) {
                exact_name = sender_name;
            } else {
                exact_name = "";
            }

            if (sender_name != myItemdata.Name) {
                if (content_type == "PicContent") {
                    this->ChatView->AppendOtherMessage(tmpItemdata.Pic, exact_name, content, true);
                } else {
                    this->ChatView->AppendOtherMessage(tmpItemdata.Pic, exact_name, content, false);
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

    QLabel* picLabel = new QLabel(PicBrowserWin);
    QSize LabelFrameSize(qRound(ActivateMonitorSize.width() * 0.8), qRound(ActivateMonitorSize.height()* 0.8));
    picLabel->setGeometry((ActivateMonitorSize.width() * 0.2) / 2,
                          (ActivateMonitorSize.height()* 0.1) / 2,
                           LabelFrameSize.width(),
                           LabelFrameSize.height());
    picLabel->setAlignment(Qt::AlignCenter);
    if (pic.width() >= LabelFrameSize.width() || pic.height() >= LabelFrameSize.height()) {
        double rate = 0.95;
        while (rate > 0 && (qRound(pic.width() * rate) >= LabelFrameSize.width() || qRound(pic.height() * rate) >= LabelFrameSize.height())) {
            rate -= 0.05;
        }
        picLabel->setPixmap(pic.scaled(pic.width() * rate, pic.height() * rate, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        picLabel->setPixmap(pic.scaled(pic.width(), pic.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    PicBrowserWin->show();
}
