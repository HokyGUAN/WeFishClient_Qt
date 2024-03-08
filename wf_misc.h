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
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QPainter>
#include <QMessageBox>
#include <QSharedPointer>
#include <QDebug>
#include <QFileDialog>

#include "wf_button.h"

#include "Cryptor.h"


extern QString WF_DIR;
extern QString WF_ALL_DIR;
extern QString WF_CONF_DIR;
extern QString WF_UPDT_DIR;
typedef enum e_shutdownReason {
    REASON_USERCONFLICT = 0,
    REASON_SERVERLOST,
} ShutDownReason;

class WF_ImageHandler : public QObject
{
    Q_OBJECT
public:
    explicit WF_ImageHandler();
    ~WF_ImageHandler();

    QByteArray ImageToBase64(QString ImgPath);
    QPixmap Base64ToImage(QByteArray bytearray);
};

class WF_BaseWin : public QMainWindow
{
    Q_OBJECT
public:
    WF_BaseWin(QWidget *parent = nullptr) : QMainWindow(parent) {
        CloseButton  = new WF_LoginCloseButton(this, 33, 24);
        connect(CloseButton, &QPushButton::clicked, [this] {this->close();});
        this->setWindowFlags(Qt::FramelessWindowHint);

    };
    ~WF_BaseWin() {};

protected:
    QPoint offset_;
    void mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            QPoint startPos = event->globalPos();
            QPoint limitRegion = mapFromGlobal(startPos);
            if (limitRegion.ry() < 50) {
                offset_ = startPos - geometry().topLeft();
            }
        }
        QWidget::mousePressEvent(event);
    };
    void mouseMoveEvent(QMouseEvent *event) {
        if (event->buttons() == Qt::LeftButton) {
            QPoint endPos = event->globalPos();
            QPoint limitRegion = mapFromGlobal(endPos);
            if (limitRegion.ry() < 50) {
                move(endPos - offset_);
            }
        }
        QWidget::mouseMoveEvent(event);
    };
    void mouseReleaseEvent(QMouseEvent *event) {
         QWidget::mouseReleaseEvent(event);
    };
    virtual void closeEvent(QCloseEvent *event) {
        event->accept();
    };

    WF_LoginCloseButton* CloseButton;
};

class WF_Setting : public QLabel
{
    Q_OBJECT
public:
    WF_Setting(QWidget * parent);
    ~WF_Setting() {};

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *) Q_DECL_OVERRIDE;

private:
    QMenu* menu;
    QAction * change_icon ;
    QAction * setting;
    QString Image_Path;

signals:
    void eChangedIcon(QString path);

public slots:
    void sChangeIcon();
};

class WF_LoginSettingWin : public WF_BaseWin
{
    Q_OBJECT
public:
    WF_LoginSettingWin(QWidget * parent);
    ~WF_LoginSettingWin() {};

    void paintEvent(QPaintEvent *event);

    void doSave();

signals:
    void eUpdateNetworkConfig();

private:
    QSettings* configSetting;
    QLabel* IPLabel;
    QLabel* PortLabel;
    QLineEdit* IPLineEdit;
    QLineEdit* PortLineEdit;
    QPushButton* ModifyBtn;
};

class WF_LoginRegisterWin : public WF_BaseWin
{
    Q_OBJECT
public:
    WF_LoginRegisterWin(QWidget * parent);
    ~WF_LoginRegisterWin() {};

    void paintEvent(QPaintEvent *event);

    void doChooseIcon();
    void doRegister();

signals:
    void eChangedIcon(QString image_path);
    void eUserRegister(QString name, QString password, QString invite_code, QString Icon_path);

private:
    QSettings* configSetting;
    QString ImagePath;
    QLabel* NameLabel;
    QLabel* PasswordLabel;
    QLabel* CodeLabel;
    QLineEdit* NameLineEdit;
    QLineEdit* PasswordLineEdit;
    QLineEdit* CodeLineEdit;
    QPushButton* ChooseIconBtn;
    QPushButton* RegisterBtn;
    QPixmap* UserIcon;
    QString UserIconUrl = "";
};

class WF_LoginSwitchWin : public WF_BaseWin
{
    Q_OBJECT
public:
    WF_LoginSwitchWin(QWidget * parent);
    ~WF_LoginSwitchWin() {};

    void paintEvent(QPaintEvent *event);

    void doUserSwitch();

signals:
    void eUserSwitch(QString account, QString password);

private:
    QSettings* configSetting;
    QLabel* AccountLabel;
    QLabel* PasswordLabel;
    QLineEdit* AccountLineEdit;
    QLineEdit* PasswordLineEdit;
    QPushButton* SwitchBtn;


};

class WF_Filer
{
public:
    WF_Filer(QString FilePath);
    ~WF_Filer() {};

    QString Read(QString contain, QString delimiter, int num);
    void Append(QString content);
    void Replace();

private:
    QFile* filer_;
};

#endif // WF_MISC_H
