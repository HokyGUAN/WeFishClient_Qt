/**********************************
 *
 *     WeFish Friendlist View
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_flistview.h"

#include <QDebug>
#include <QPainter>
#include "wf_misc.h"

void WF_FriendItem::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        painter->save();
        QVariant var = index.data(Qt::UserRole+1);
        UserItemData ItemData = var.value<UserItemData>();

        QRectF rect(option.rect.x(), option.rect.y(), option.rect.width()-1, option.rect.height()-1);

        painter->setPen(QPen(QColor(220,220,220)));
        painter->setBrush(QColor(220,220,220));
        painter->drawRect(rect);

        if (option.state.testFlag(QStyle::State_MouseOver)) {
            painter->setPen(QPen(QColor(230,231,232)));
            painter->setBrush(QColor(230,231,232));
            painter->drawRect(rect);
        }
        if (option.state.testFlag(QStyle::State_Selected)) {
            painter->setPen(QPen(QColor(198,198,198)));
            painter->setBrush(QColor(198,198,198));
            painter->drawRect(rect);
        }

        painter->drawPixmap(kIconPos.x(), kIconPos.y() + option.rect.y(), kIconSize.width(), kIconSize.height(), ItemData.Pic);
        painter->setPen(QPen(Qt::black));
        painter->setFont(QFont("Microsoft Yahei", 10));
        painter->drawText(QRect(kNamePos.x(), kNamePos.y() + option.rect.y(), 245-kNamePos.x(), 20), ItemData.Name);

        if (ItemData.Unread) {
            painter->setPen(QPen(QColor(255,59,48)));
            painter->setBrush(QColor(255,59,48));
            painter->drawEllipse(QRectF(49.5, 5.5 + option.rect.y(), 10, 10));
        }

        painter->restore();
    }
}

QSize WF_FriendItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QSize(option.rect.width(), 67);
}

WF_FriendListView::WF_FriendListView(QWidget *parent, QString name)
    : QListView(parent)
    , hostName_(name)
{
    wf_f_model = new QStandardItemModel(this);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setStyleSheet("QListView{border:0px;background:rgb(220,220,220)}");

    UserItemData ItemData;
    ItemData.Available = true;
    ItemData.Pic = QPixmap(":/Res/Tomphany.jpg");
    ItemData.Name = "Group";
    ItemData.Account = 0;
    this->AddFriend(ItemData);
    currentFriendItem_ = ItemData;

    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(ListClick(QModelIndex)));
}

void WF_FriendListView::AddFriend(UserItemData ItemData)
{
    QStandardItem* standarditem = new QStandardItem();
    standarditem->setData(QVariant::fromValue(ItemData), Qt::UserRole+1);
    wf_f_model->appendRow(standarditem);
    WF_FriendItem* item = new WF_FriendItem();
    this->setItemDelegateForRow(wf_f_model->rowCount()-1, item);
    this->setModel(wf_f_model);
}

void WF_FriendListView::RemoveFriend(int Account)
{
    for (int i = 0; i < wf_f_model->rowCount(); ++i) {
        if (Account == wf_f_model->item(i, 0)->data().value<UserItemData>().Account) {
            wf_f_model->removeRow(i);
            break;
        }
    }
    this->setModel(wf_f_model);
}

UserItemData WF_FriendListView::GetFriend(int Account)
{
    UserItemData itemdata;
    for (int i = 0; i < wf_f_model->rowCount(); ++i) {
        if (Account == wf_f_model->item(i, 0)->data().value<UserItemData>().Account) {
            itemdata = wf_f_model->item(i, 0)->data().value<UserItemData>();
        }
    }
    return itemdata;
}

UserItemData WF_FriendListView::GetFriend(QString Name)
{
    UserItemData itemdata;
    for (int i = 0; i < wf_f_model->rowCount(); ++i) {
        if (Name == wf_f_model->item(i, 0)->data().value<UserItemData>().Name) {
            itemdata = wf_f_model->item(i, 0)->data().value<UserItemData>();
        }
    }
    return itemdata;
}

int WF_FriendListView::GetItemIndexInList(int Account)
{
    for (int i = 0; i < wf_f_model->rowCount(); ++i) {
        if (Account == wf_f_model->item(i, 0)->data().value<UserItemData>().Account) {
            return i;
        }
    }
    return -1;
}

void WF_FriendListView::SetFriend(int IndexInList, UserItemData itemdata)
{
    QStandardItem* standarditem = new QStandardItem();
    standarditem->setData(QVariant::fromValue(itemdata), Qt::UserRole+1);
    wf_f_model->setItem(IndexInList, standarditem);
    this->setModel(wf_f_model);
}

void WF_FriendListView::sSayHello(int others_account, QString others_name, QPixmap others_icon)
{
    qDebug() << "WF_MainWin SayHello: " << others_account << "-" << others_name << "-" << others_icon;
    user_icon_map.insert(others_account, others_icon);
    UserItemData ItemData;
    ItemData.Available = true;
    ItemData.Account = others_account;
    ItemData.Name = others_name;
    ItemData.Pic = others_icon;
    this->AddFriend(ItemData);
}

void WF_FriendListView::sOnline(int others_account, QString others_name, QPixmap others_icon)
{
    qDebug() << "WF_MainWin Online: " << others_account << "-" << others_name << "-" << others_icon;
    user_icon_map.insert(others_account, others_icon);
    UserItemData ItemData;
    ItemData.Available = true;
    ItemData.Account = others_account;
    ItemData.Name = others_name;
    ItemData.Pic = others_icon;
    this->AddFriend(ItemData);
}

void WF_FriendListView::sOffline(int others_account, QString others_name)
{
    Q_UNUSED(others_name)
    this->RemoveFriend(others_account);
}

void WF_FriendListView::sNotify(int sender_account, QString sender_name, QPixmap icon, int to_account, QString content, QString content_type)
{
    UserItemData senderItemdata;
    int indexInList;
    QString nameInMsg;
    QPixmap senderIcon;

    if (content_type != "FileContent") {
        user_icon_map.insert(sender_account, icon);
    }

    if (to_account == 0) {
        senderItemdata = this->GetFriend(to_account);
        indexInList = this->GetItemIndexInList(to_account);
        //UserItemData tmpItemdata = this->GetFriend(sender_account);
        //if (!tmpItemdata.Available) senderIcon = icon;
        senderIcon = icon;
        nameInMsg = sender_name;
    } else {
        senderItemdata = this->GetFriend(sender_account);
        indexInList = this->GetItemIndexInList(sender_account);
        senderIcon = this->GetFriend(sender_account).Pic;
        nameInMsg = "";
    }

    if (!senderItemdata.Available || indexInList == -1) return;

    //qDebug() << __FUNCTION__ << "Sender Account:" << senderItemdata.Account << " Name:" << senderItemdata.Name << " IndexInList:" << indexInList;
    senderItemdata.HistoryMsg.push_back(QString::number(sender_account) + ":" + sender_name + ":" + content_type + ":" + content);
    //qDebug() << __FUNCTION__ << "SenderItemData History Msg: " << senderItemdata.HistoryMsg.at(senderItemdata.HistoryMsg.size() - 1);

    if (senderItemdata.Account != currentFriendItem_.Account) {
        senderItemdata.Unread = true;
    } else {
        if (content_type == "Content") {
            if (sender_account == account_) {
                emit AddSelfMessageSig(QPixmap(WF_DIR + "\\Self.jpg"), content, false);
            } else {
                emit AddOthersMessageSig(senderIcon, nameInMsg, content, false);
            }
        } else if (content_type == "PicContent") {
            if (sender_account == account_) {
                emit AddSelfMessageSig(QPixmap(WF_DIR + "\\Self.jpg"), content, true);
            } else {
                emit AddOthersMessageSig(senderIcon, nameInMsg, content, true);
            }
        } else if (content_type == "FileContent") {
            if (sender_account == account_) {
                emit AddSelfMessageSig(QPixmap(WF_DIR + "\\Self.jpg"), content, false);
            } else {
                emit AddOthersMessageSig(senderIcon, nameInMsg, content, false);
            }
        }
    }
    this->SetFriend(indexInList, senderItemdata);
}

void WF_FriendListView::sChangedIcon(QString pic_path)
{
    UserItemData myItemdata;
    myItemdata = this->GetFriend(account_);
    myItemdata.Pic = QPixmap(pic_path);
    this->SetFriend(this->GetItemIndexInList(account_), myItemdata);
}

void WF_FriendListView::ListClick(QModelIndex index)
{
    int CurRowIdx = this->currentIndex().row();
    currentFriendItem_ = index.data(Qt::UserRole+1).value<UserItemData>();
    currentFriendItem_.Unread = false;
    this->SetFriend(CurRowIdx, currentFriendItem_);
    emit ClickSig(currentFriendItem_);
}
