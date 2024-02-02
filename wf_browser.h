/**********************************
 *
 *      WeFish Photo Browser
 *
 * Author: Kim.He@tymphany.com
 * date  : 12/1 2022
 *
***********************************/
#ifndef WF_BROWSER_H
#define WF_BROWSER_H

#include <QLabel>
#include <QKeyEvent>
#include <QDebug>

class WF_Browser : public QLabel
{
    Q_OBJECT
public:
    WF_Browser(QWidget *parent = nullptr);
    ~WF_Browser() {};

    void SetPicPath(QString strPath);//设置图片路径
    void SetQpixmap(QPixmap pic);//设置QPixmap
    void SetPic(QImage Image); //设置图片
    void setPenWidth(int w) { m_intPenWidth = w; };
    void setZoom(bool bZoom) { m_bZoom = bZoom; }//设置是否允许滚动缩放

protected:
    void paintEvent(QPaintEvent *event)Q_DECL_OVERRIDE;                        //QPaint画图
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;               //鼠标滚轮滚动
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;    //鼠标双击还原图片大小事件
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;          //鼠标摁下
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;           //鼠标移动
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;        //鼠标放开
    //键盘事件（按住ctrl 并滚动滚轮才可以放大或者缩小图片）
    void keyPressEvent(QKeyEvent *event)Q_DECL_OVERRIDE;
    void  keyReleaseEvent(QKeyEvent *event)Q_DECL_OVERRIDE;


private:
    QImage m_Image;           //原始图片
    QPixmap m_bTempPixmap; //当前画图展示的缩放移动后的图片

    QString m_strImagePath="";  //图片全路径名

    qreal ZoomValue = 1.0;  //鼠标缩放值
    int XPtInterval = 0;    //平移X轴的值
    int YPtInterval = 0;    //平移Y轴的值
    bool Pressed = false;   //鼠标是否被摁压
    bool m_bZoom = true; //是否按住滚动缩放
    QPoint OldPos;          //旧的鼠标位置
    QPoint m_nMovePos;
    int m_intPenWidth = 1;

private slots:
    void OnZoomInImage();       //图片放大
    void OnZoomOutImage();      //图片缩小
    void OnPresetImage();       //图片还原

signals:
    void SigColor(int, int, int);
};

#endif // WF_BROWSER_H
