/**********************************
 *
 *           WeFish Misc
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_MISC_H
#define WF_MISC_H

#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QPixmap>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QDataStream>
#include <QLabel>
#include <QMenu>
#include <QLabel>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

extern QString WF_DIR;

class WF_ImageHandler : public QObject
{
    Q_OBJECT
public:
    explicit WF_ImageHandler();
    ~WF_ImageHandler();

    QByteArray ImageToBase64(QString ImgPath);
    QPixmap Base64ToImage(QByteArray bytearray);
};

class WF_HeadIcon : public QLabel
{
    Q_OBJECT
public:
    WF_HeadIcon(QWidget * parent);
    ~WF_HeadIcon() {};
    QString Get_Image();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;

private:
    QMenu* menu;
    QAction * change_icon ;
    QAction * getinfo;
    QString Image_Path;

    QDialog *dig_open;

signals:
    void ChangedIcon(QString path);

public slots:
    void ChangeIcon();
};

#endif // WF_MISC_H
