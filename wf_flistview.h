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
typedef struct userItemData
{
    bool Available;
    QPixmap Pic;
    QString Name;
    int ID;
    bool Unread;
    QList<QString> HistoryMsg;

    userItemData() {
        Available = false;
        Pic = QPixmap();
        Name = "";
        ID = -1;
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
    void RemoveFriend(int ID);
    UserItemData GetFriend(int ID);
    UserItemData GetFriend(QString Name);
    int GetItemIndexInList(int ID);
    void SetFriend(int IndexInList, UserItemData itemdata);

private:
    int myId_;
    QString hostName_;
    QStandardItemModel* wf_f_model;

    UserItemData currentFriendItem_;

private slots:
    void MyID(int ID) {myId_ = ID; qDebug() << "FriendListView Get MyID" << myId_;};
    void ListClick(QModelIndex index);
    void SayHello(int others_id, QString others_name, QPixmap others_icon);
    void Online(int others_id, QString others_name, QPixmap others_icon);
    void Offline(int others_id, QString others_name);
    void Notify(int sender_id, QString sender_name, int to_id, QString content, QString content_type);
    void ChangedIcon(QString pic_path);

signals:
    void ClickSig(UserItemData itemdata);
    void AddOthersMessageSig(QPixmap icon, QString name, QString content, bool isPic);
    void AddSelfMessageSig(QPixmap icon, QString content, bool isPic);
};


#endif // WF_FLISTVIEW_H
