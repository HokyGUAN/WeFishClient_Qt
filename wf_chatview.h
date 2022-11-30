/**********************************
 *
 *        WeFish Chat View
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_CHATVIEW_H
#define WF_CHATVIEW_H

#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMouseEvent>

class BaseItem : public QGraphicsRectItem
{
public:
    explicit BaseItem() {};
    virtual ~BaseItem();

    virtual bool ContentClicked(QPointF pos);
    virtual QPixmap GetPic();
    virtual int Resize(int width);
};

class SelfMsgFrameItem : public BaseItem
{
public:
    const int kMsgFontSize = 14;
    const QSize kIconSize = QSize(45, 45);
    const QMargins kMargins = QMargins(12,11,12,11);
    const int kMarginLeft = 40;

    explicit SelfMsgFrameItem(QPixmap icon, QString msg, int width, bool isPic);
    virtual ~SelfMsgFrameItem();

    bool ContentClicked(QPointF pos);
    QPixmap GetPic() {return pic_;};
    virtual int Resize(int width);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString msg_;
    bool isPic_;
    QGraphicsPixmapItem icon_;
    QPoint IconPos_;
    QSize content_size_;
    QPixmap pic_;
    QSize pic_content_size_;
    QFont font_;
    int width_;
};

class OthersMsgFrameItem : public BaseItem
{
public:
    const int kMsgFontSize = 14;
    const int kNameFontSize = 13;
    const int kMarginRight = 40;

    const QPoint kNamePos = QPoint(75, 2);
    const QSize kIconSize = QSize(45, 45);
    const QPoint kIconPos = QPoint(20, 8);
    const QMargins kMargins = QMargins(12,11,12,11);

    QPoint kBorderPos;
    QPoint kTextPos;


    explicit OthersMsgFrameItem(QPixmap icon,QString name, QString msg, bool isPic);
    virtual ~OthersMsgFrameItem();

    bool ContentClicked(QPointF pos);
    QPixmap GetPic() {return pic_;};
    virtual int Resize(int width);

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString msg_;
    bool isPic_;
    QGraphicsPixmapItem icon_;
    QGraphicsSimpleTextItem name_;
    QPolygon triangle_;
    QSize content_size_;
    QPixmap pic_;
    QSize pic_content_size_;
    QFont font_;
};

class WF_ChatView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit WF_ChatView(QWidget *parent = nullptr);
    ~WF_ChatView();
    void Resize(int width);
    void Clear();
    void AppendSelfMessage(QPixmap icon, QString msg, bool isPic);
    void AppendOtherMessage(QPixmap icon,QString name, QString msg, bool isPic);

protected:
    virtual void mousePressEvent(QMouseEvent *e);

private:
    QList<BaseItem*> items_;

private slots:
    void NotifyOthersMessage(QPixmap icon, QString name, QString content, bool isPic);
    void NotifySelfMessage(QPixmap icon, QString content, bool isPic);

signals:
    void PictureContentClicked(QPixmap pic);
};

#endif // WF_CHATVIEW_H
