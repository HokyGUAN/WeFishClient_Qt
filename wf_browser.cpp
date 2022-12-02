/**********************************
 *
 *      WeFish Photo Browser
 *
 * Author: Kim.He@tymphany.com
 * date  : 12/1 2022
 *
***********************************/
#include "wf_browser.h"
#include<QFile>
#include<QPainter>


WF_Browser::WF_Browser(QWidget *parent) : QLabel(parent)
{
   //开启鼠标追踪：即使没有按下鼠标键，移动鼠标时也可以捕获鼠标的移动事件。
   setMouseTracking(true);
}

void WF_Browser::SetPicPath(QString strPath)//设置图片路径
{
    m_Image = QImage(); //清空
    m_strImagePath = strPath;
    if (! m_Image.load(m_strImagePath)) {
        //如果load失败可能是格式错误
        QFile file(strPath);
        if (file.open(QIODevice::ReadOnly)) {
           m_Image.loadFromData(file.readAll());
        }
    }
    update();
}

void WF_Browser::SetQpixmap(QPixmap pic)//设置QPixmap
{
    m_Image =  pic.toImage();
    update();
}

void WF_Browser::SetPic(QImage Image)
{
    m_Image = Image;
}

void WF_Browser::paintEvent(QPaintEvent *event)
{
    // 设置一个画家painter,  在空白的tmpPixMap上进行绘制的
    QPainter painter(this);
    QPixmap tmpPixMap(this->width(), this->height());
    tmpPixMap.fill(Qt::transparent);

    // 根据窗口计算应该显示的图片的大小
    int width = qMin(m_Image.width(), this->width());
    int height = width * 1.0 / (m_Image.width() * 1.0 / m_Image.height());
    height = qMin(height, this->height());
    width = height * 1.0 * (m_Image.width() * 1.0 / m_Image.height());

    // 平移
    painter.translate(this->width() / 2 + XPtInterval, this->height() / 2 + YPtInterval);

    // 缩放
    painter.scale(ZoomValue, ZoomValue);
    //qDebug()<<"ZoomValue:" <<ZoomValue;

    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    painter.drawImage(picRect, m_Image);

    m_bTempPixmap = tmpPixMap;
}

//键盘事件（按住ctrl 并滚动滚轮才可以放大或者缩小图片）
void WF_Browser::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        setZoom(true);
    }
}

void  WF_Browser::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        setZoom(false);
    }
}

//图片放大
void WF_Browser::OnZoomInImage()
{
    ZoomValue += 0.05;
    update();
}

//图片缩小
void WF_Browser::OnZoomOutImage()
{
    ZoomValue -= 0.05;
    if (ZoomValue <= 0) {
        ZoomValue = 0.05;
        return;
    }
    update();
}

//鼠标滚轮滚动
void WF_Browser::wheelEvent(QWheelEvent *event)
{
    if (!m_bZoom) return;
    int value = event->delta();
    if (value > 0)  //放大
        OnZoomInImage();
    else            //缩小
        OnZoomOutImage();
    update();
}

//鼠标双击还原图片大小事件
void WF_Browser::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    OnPresetImage();
}

//图片还原
void WF_Browser::OnPresetImage()
{
    ZoomValue = 1.0;
    XPtInterval = 0;
    YPtInterval = 0;
    update();
}

//鼠标摁下
void WF_Browser::mousePressEvent(QMouseEvent *event)
{
    OldPos = event->pos();
    Pressed = true;
}

//鼠标松开
void WF_Browser::mouseMoveEvent(QMouseEvent *event)
{
    if (!Pressed) {//如果不是按下拖动，则显示实时坐标的rgb
        m_nMovePos = event->pos();
        QRgb qRgb = m_bTempPixmap.toImage().pixel(m_nMovePos);
        emit SigColor(qRed(qRgb), qGreen(qRgb), qBlue(qRgb));
        return QWidget::mouseMoveEvent(event);
    }
    this->setCursor(Qt::SizeAllCursor);
    QPoint pos = event->pos();
    int xPtInterval = pos.x() - OldPos.x();
    int yPtInterval = pos.y() - OldPos.y();

    XPtInterval += xPtInterval;
    YPtInterval += yPtInterval;
    //qDebug()<<" XPtInterval:" <<XPtInterval<<" YPtInterval:" <<YPtInterval<<" xPtInterval:" <<xPtInterval<<" yPtInterval:" <<yPtInterval;
    OldPos = pos;
    update();
}

void WF_Browser::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    Pressed = false;
    setCursor(Qt::ArrowCursor);
}
