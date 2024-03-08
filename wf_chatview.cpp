/**********************************
 *
 *        WeFish Chat View
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_chatview.h"
#include "wf_misc.h"

#include <QPainter>
#include <QMargins>
#include <QTextOption>

#include <QDebug>
#include <QTextEdit>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QPoint>
#include <QTransform>
//#include <QPolygon>


BaseItem::~BaseItem()
{
}

bool BaseItem::ContentClicked(QPointF pos)
{
    Q_UNUSED(pos)
    return false;
}

QPixmap BaseItem::GetPic()
{
    return QPixmap();
}

int BaseItem::Resize(int width)
{
    Q_UNUSED(width)
    return 0;
}

SelfMsgFrameItem::SelfMsgFrameItem(QPixmap icon, QString msg, int width, bool isPic)
{
    msg_ = msg;
    isPic_ = isPic;
    if (isPic_) {
        WF_ImageHandler * imagehandler = new WF_ImageHandler();
        QByteArray byte_array = msg_.toUtf8();
        pic_ = imagehandler->Base64ToImage(byte_array);
    }

    IconPos_ = QPoint(width - kIconSize.width() - 20, 8);
    icon_.setPixmap(icon.scaled(kIconSize.width(), kIconSize.height()));
    icon_.setPos(IconPos_);

    font_ = QFont("Microsoft YaHei");
    font_.setPixelSize(14);
    icon_.setParentItem(this);
}

SelfMsgFrameItem::~SelfMsgFrameItem()
{
}

QRectF SelfMsgFrameItem::boundingRect() const
{
    if (isPic_) {
        int width = width_ - 20 - kIconSize.width() - 10 - kMargins.left() - kMargins.right() - pic_content_size_.width();
        QRectF border(width, IconPos_.y(), pic_content_size_.width() + kMargins.left() + kMargins.right(), pic_content_size_.height() + kMargins.top() + kMargins.bottom());
        return QRectF(0,0,border.width(),border.height());
    } else {
        int width = width_ - 20 - kIconSize.width() - 10 - kMargins.left() - kMargins.right() - content_size_.width();
        QRectF border(width, IconPos_.y(), content_size_.width() + kMargins.left() + kMargins.right(), content_size_.height() + kMargins.top() + kMargins.bottom());
        return QRectF(0,0,border.width(),border.height());
    }
}

void SelfMsgFrameItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRectF border;
    QSize rnd(10,10);

    if (isPic_) {
        int width = width_ - 20 - kIconSize.width() - 10 - kMargins.left() - kMargins.right() - pic_content_size_.width();
        border = QRectF(width, IconPos_.y(), pic_content_size_.width() + kMargins.left() + kMargins.right(), pic_content_size_.height() + kMargins.top() + kMargins.bottom());
    } else {
        int width = width_ - 20 - kIconSize.width() - 10 - kMargins.left() - kMargins.right() - content_size_.width();
        border = QRectF(width, IconPos_.y(), content_size_.width() + kMargins.left() + kMargins.right(), content_size_.height() + kMargins.top() + kMargins.bottom());
    }

    painter->setBrush(QColor(0,204,0));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(border.x(), border.y(), border.width(), border.height(), rnd.width(), rnd.height());

    QPolygonF triangle;
    triangle.append(QPoint(IconPos_.x()-3, IconPos_.y() + kIconSize.height()/2));
    triangle.append(QPoint(IconPos_.x()-10, IconPos_.y() + kIconSize.height()/2 - 5));
    triangle.append(QPoint(IconPos_.x()-10, IconPos_.y() + kIconSize.height()/2 + 5));
    painter->drawPolygon(triangle);

    if (isPic_) {
        painter->drawPixmap(border.x() + kMargins.left(), border.y() + kMargins.top(), pic_content_size_.width(), pic_content_size_.height(), pic_.scaled(pic_content_size_.width(), pic_content_size_.width()));
    } else {
        QPen ContentPen;
        ContentPen.setColor(QColor(51, 51, 51));
        painter->setPen(ContentPen);
        QTextOption TextOp(Qt::AlignLeft);
        TextOp.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter->setFont(font_);
        QRectF text_rect(border.x() + kMargins.left(), border.y() + kMargins.top(), content_size_.width(), content_size_.height());
        painter->drawText(text_rect, msg_, TextOp);
    }
}

bool SelfMsgFrameItem::ContentClicked(QPointF pos)
{
    if (isPic_) {
        if (pos.ry() > this->y() && pos.ry() < this->y() + pic_content_size_.height() + kMargins.top() + kMargins.bottom()) {
            return true;
        }
    }
    return false;
}

int SelfMsgFrameItem::Resize(int width)
{
    width_ = width;
    if (isPic_) {
        int row_width_limit = width_ - kMargins.right() - kMargins.left() - kIconSize.width() - 20 - 10;
        if (pic_.width() > row_width_limit) {
            int rate = pic_.width() / row_width_limit + 1;
            pic_content_size_ = QSize(pic_.width() / rate, pic_.height() / rate);
        } else {
            pic_content_size_ = QSize(pic_.width(), pic_.height());
        }
        return pic_content_size_.height() + kMargins.top() + kMargins.bottom() + IconPos_.y();
    } else {
        int expect_length = (width / 2) + ((width / 2) / 3);
        int row_width_limit = expect_length - kMargins.right() - kMargins.left() - kIconSize.width() - 20 - 10;

        font_.setPixelSize(kMsgFontSize);
        QFontMetrics font_matrics(font_);
        int content_total_width = font_matrics.width(msg_);
        int height_per_row = font_matrics.lineSpacing();

        if(row_width_limit < content_total_width) {
            int row = content_total_width / row_width_limit;
            ++row;
            int content_total_height = row * height_per_row;
            content_size_.setWidth(row_width_limit);
            content_size_.setHeight(content_total_height);
        } else {
            int content_total_height = height_per_row;
            content_size_.setWidth(content_total_width);
            content_size_.setHeight(content_total_height);
        }
        return content_size_.height() + kMargins.top() + kMargins.bottom() + IconPos_.y();
    }
}

OthersMsgFrameItem::OthersMsgFrameItem(QPixmap icon,QString name, QString msg, bool isPic)
{
    msg_ = msg;
    isPic_ = isPic;
    if (isPic_) {
        WF_ImageHandler * imagehandler = new WF_ImageHandler();
        QByteArray byte_array = msg_.toUtf8();
        pic_ = imagehandler->Base64ToImage(byte_array);
    }

    icon_.setPixmap(icon.scaled(kIconSize.width(), kIconSize.height()));
    icon_.setPos(kIconPos);
    icon_.setParentItem(this);

    font_ = QFont("Microsoft YaHei", 14);
    font_.setPixelSize(kNameFontSize);

    if (name != "") {
        name_.setText(name);
        name_.setPos(kNamePos);
        name_.setFont(font_);
        name_.setBrush(QColor(153, 153, 153));
        name_.setParentItem(this);
        kBorderPos = QPoint(kNamePos.x(), kNamePos.y()+25);
        triangle_.append(QPoint(kBorderPos.x()-7, kIconPos.y() + kIconSize.height()));
        triangle_.append(QPoint(kBorderPos.x(), kIconPos.y() + kIconSize.height() - 5));
        triangle_.append(QPoint(kBorderPos.x(), kIconPos.y() + kIconSize.height() + 5));
    } else {
        kBorderPos = QPoint(kNamePos.x(), kIconPos.y());
        triangle_.append(QPoint(kBorderPos.x()-7, kIconPos.y() + kIconSize.height()/2));
        triangle_.append(QPoint(kBorderPos.x(), kIconPos.y() + kIconSize.height()/2 - 5));
        triangle_.append(QPoint(kBorderPos.x(), kIconPos.y() + kIconSize.height()/2 + 5));
    }
    kTextPos = QPoint(kBorderPos.x()+ kMargins.left(), kBorderPos.y() + kMargins.top());
}

OthersMsgFrameItem::~OthersMsgFrameItem()
{
}

QRectF OthersMsgFrameItem::boundingRect() const
{
    if (isPic_) {
        QRectF border(kBorderPos.x(), kBorderPos.y(), pic_content_size_.width() + kMargins.left() + kMargins.right(), pic_content_size_.height() + kMargins.top() + kMargins.bottom());
        return QRectF(0,0,border.width(),border.height());
    } else {
        QRectF border(kBorderPos.x(), kBorderPos.y(), content_size_.width() + kMargins.left() + kMargins.right(), content_size_.height() + kMargins.top() + kMargins.bottom());
        return QRectF(0,0,border.width(),border.height());
    }
}

void OthersMsgFrameItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRectF border;
    QSize rnd(10,10);
    if (isPic_) {
        border = QRectF(kBorderPos.x(), kBorderPos.y(), pic_content_size_.width() + kMargins.left() + kMargins.right(), pic_content_size_.height() + kMargins.top() + kMargins.bottom());
    } else {
        border = QRectF(kBorderPos.x(), kBorderPos.y(), content_size_.width() + kMargins.left() + kMargins.right(), content_size_.height() + kMargins.top() + kMargins.bottom());
    }

    painter->setBrush(QBrush(Qt::white));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(border.x(), border.y(), border.width(), border.height(), rnd.width(), rnd.height());

    painter->drawPolygon(triangle_);

    if (isPic_) {
        painter->drawPixmap(kTextPos.x(), kTextPos.y(), pic_content_size_.width(), pic_content_size_.height(), pic_.scaled(pic_content_size_.width(), pic_content_size_.width()));
    } else {
        QPen ContentPen;
        ContentPen.setColor(QColor(51, 51, 51));
        painter->setPen(ContentPen);
        QTextOption TextOp(Qt::AlignLeft);
        TextOp.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter->setFont(font_);
        QRectF text_rect(kTextPos.x(), kTextPos.y(), content_size_.width(), content_size_.height());
        painter->drawText(text_rect, msg_, TextOp);
    }
}

bool OthersMsgFrameItem::ContentClicked(QPointF pos)
{
    //qDebug() << this->x() << ":" << this->y();
    if (isPic_) {
        if (pos.ry() > this->y() && pos.ry() < this->y() + pic_content_size_.height() + kMargins.top() + kMargins.bottom()) {
            return true;
        }
    }
    return false;
}

int OthersMsgFrameItem::Resize(int width)
{ 
    if (isPic_) {
        int row_width_limit = width - kTextPos.x();
        if (pic_.width() > row_width_limit) {
            int rate = pic_.width() / row_width_limit + 1;
            pic_content_size_ = QSize(pic_.width() / rate, pic_.height() / rate);
        } else {
            pic_content_size_ = QSize(pic_.width(), pic_.height());
        }
        return pic_content_size_.height() + kMargins.top() + kMargins.bottom() + kBorderPos.y();
    } else {
        int expect_length = (width / 2) + ((width / 2) / 3);
        int row_width_limit = expect_length - kTextPos.x();

        font_.setPixelSize(kMsgFontSize);
        QFontMetrics font_matrics(font_);
        int content_total_width = font_matrics.width(msg_);
        int height_per_row = font_matrics.lineSpacing();

        if(row_width_limit < content_total_width) {
            int row = content_total_width / row_width_limit;
            ++row;
            int content_total_height = row * height_per_row;
            content_size_.setWidth(row_width_limit);
            content_size_.setHeight(content_total_height);
        } else {
            int content_total_height = height_per_row;
            content_size_.setWidth(content_total_width);
            content_size_.setHeight(content_total_height);
        }
        return content_size_.height() + kMargins.top() + kMargins.bottom() + kBorderPos.y();
    }

}


const int kMarkRole = Qt::UserRole;
const int kRoleOtherMsg = kMarkRole + 1;
const int kRoleSelfMsg = kRoleOtherMsg + 1;
WF_ChatView::WF_ChatView(QWidget *parent) : QGraphicsView(parent)
{
    this->setScene(new QGraphicsScene());
    this->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    this->setStyleSheet("background: rgb(250,250,250); border:0px");
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

WF_ChatView::~WF_ChatView()
{
}

void WF_ChatView::Resize(int width)
{
    int height = 15;
    for (BaseItem* item : items_)
    {
        item->setPos(0, height);
        height = height + item->Resize(width);
        height += 15;
    }
    this->scene()->setSceneRect(QRectF(0, 0, width, height));
}

void WF_ChatView::Clear()
{
    for (auto it = items_.begin(); it != items_.end(); ++it) {
        this->scene()->removeItem(*it);
        delete *it;
    }
    items_.clear();
    this->scene()->update();
}

void WF_ChatView::AppendSelfMessage(QPixmap icon, QString msg, bool isPic)
{
    BaseItem* item = new SelfMsgFrameItem(icon, msg, width(), isPic);
    scene()->addItem(item);
    items_.push_back(item);
    Resize(width());
    scene()->update();
    QScrollBar *vScrollBar = verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());
}

void WF_ChatView::AppendOtherMessage(QPixmap icon, QString name, QString msg, bool isPic)
{
    BaseItem* item = new OthersMsgFrameItem(icon, name, msg, isPic);
    scene()->addItem(item);
    items_.push_back(item);
    Resize(width());
    scene()->update();
    QScrollBar *vScrollBar = verticalScrollBar();
    vScrollBar->setValue(vScrollBar->maximum());
}

void WF_ChatView::NotifyOthersMessage(QPixmap icon, QString name, QString content, bool isPic)
{
    this->AppendOtherMessage(icon, name, content, isPic);
}

void WF_ChatView::NotifySelfMessage(QPixmap icon, QString content, bool isPic)
{
    this->AppendSelfMessage(icon, content, isPic);
}

void WF_ChatView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        //QPoint pos = mapFromGlobal(gp);
        QPointF pos = mapToScene(e->pos());
        //qDebug() << pos.rx() << ":" << pos.ry();

        for (auto it = items_.begin(); it != items_.end(); ++it) {
            if ((*it)->ContentClicked(pos)) {
                emit PictureContentClicked((*it)->GetPic());
                break;
            }
        }
    }
}
