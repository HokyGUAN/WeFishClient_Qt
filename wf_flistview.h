/**********************************
 *
 *     WeFish Friendlist View
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_FLISTVIEW_H
#define WF_FLISTVIEW_H

#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QListView>
#include <QDebug>

#include <QMetaType>
#include <QList>
#include <QMap>

typedef struct userItemData
{
    bool Available;
    QPixmap Pic;
    QString Name;
    int Account;
    bool Unread;
    QList<QString> HistoryMsg;

    userItemData() {
        Available = false;
        Pic = QPixmap();
        Name = "";
        Account = -1;
        Unread = false;
    }
} UserItemData;
Q_DECLARE_METATYPE(UserItemData);


class WF_FriendItem : public QStyledItemDelegate
{
public:
    const int kNameFontSize = 15;
    const QPoint kIconPos = QPoint(11, 11);
    const QSize kIconSize = QSize(45, 45);
    const QPoint kNamePos = QPoint(2*kIconPos.x() + kIconSize.width(), kIconPos.y() + 2);

    WF_FriendItem() {};
    ~WF_FriendItem() {};

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class WF_FriendListView : public QListView
{
    Q_OBJECT
public:
    explicit WF_FriendListView(QWidget *parent = nullptr, QString name = "");
    ~WF_FriendListView() {};

    void AddFriend(UserItemData ItemData);
    void RemoveFriend(int Account);
    UserItemData GetFriend(int Account);
    UserItemData GetFriend(QString Name);
    int GetItemIndexInList(int Account);
    void SetFriend(int IndexInList, UserItemData itemdata);

    QMap<int, QPixmap> user_icon_map;

private:
    int account_;
    QString hostName_;
    QStandardItemModel* wf_f_model;

    UserItemData currentFriendItem_;

private slots:
    void sAccountCheckin(int MyAccount) {account_ = MyAccount; qDebug() << "FriendListView Get My Account" << account_;};
    void ListClick(QModelIndex index);
    void sSayHello(int others_account, QString others_name, QPixmap others_icon);
    void sOnline(int others_account, QString others_name, QPixmap others_icon);
    void sOffline(int others_account, QString others_name);
    void sNotify(int sender_account, QString sender_name, QPixmap icon, int to_id, QString content, QString content_type);
    void sChangedIcon(QString pic_path);

signals:
    void ClickSig(UserItemData itemdata);
    void AddOthersMessageSig(QPixmap icon, QString name, QString content, bool isPic);
    void AddSelfMessageSig(QPixmap icon, QString content, bool isPic);
};


#endif // WF_FLISTVIEW_H
