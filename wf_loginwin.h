/**********************************
 *
 *      WeFish Login Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_LOGINWIN_H
#define WF_LOGINWIN_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

#include "wf_mainwin.h"
#include "wf_filesocket.h"
#include "wf_upgraderwin.h"

class WF_LoginWin : public QWidget
{
    Q_OBJECT
public:
    explicit WF_LoginWin(QWidget *parent = nullptr, QSize* frameSize = nullptr);
    ~WF_LoginWin();

    void paintEvent(QPaintEvent *event);

    void doSetupNetwork();
    void doConfigure();
    void doLogin();
    void doSwitch();
    void doModify();
    void doRegister();

    void doVaildate();

protected:
    QPoint offset_;
    void mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            QPoint startPos = event->globalPos();
            QPoint limitRegion = mapFromGlobal(startPos);
            if (limitRegion.ry() < 50 && limitRegion.rx() < 214) {
                offset_ = startPos - geometry().topLeft();
            }
        }
        QWidget::mousePressEvent(event);
    };
    void mouseMoveEvent(QMouseEvent *event) {
        if (event->buttons() == Qt::LeftButton) {
            QPoint endPos = event->globalPos();
            QPoint limitRegion = mapFromGlobal(endPos);
            if (limitRegion.ry() < 50 && limitRegion.rx() < 214) {
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

private:
    QSize* frameSize_;
    QString host_name_ = "";
    QString ip_ = "";
    int port_ = 0;
    int account_;
    bool network_ready_ = false;
    bool account_validated_ = false;
    QFile* wf_config_file_;
    QSize* mainWinSize_;
    QSize* mainMonitorSize_;
    QString WF_ConfigPath_;
    QString key_;
    Cryptor cryptor_;

    WF_LoginCloseButton* LoginCloseButton;
    WF_LoginSettingButton* LoginSettingButton;

    QPixmap* UserIcon;
    QString UserIconUrl = "";
    QLabel* NameLabel;
    QPushButton* LoginButton;
    QPushButton* UserSwitch;
    QPushButton* UserRegister;
    WF_MainSocket* MainSocket;
    WF_FileSocket* FileSocket;
    WF_MainWin* MainWin;
    WF_ImageHandler* ImageHandler;
    WF_LoginSettingWin* LoginSettingWin;
    WF_LoginRegisterWin* LoginRegisterWin;
    WF_LoginSwitchWin* LoginSwitchWin;
    WF_UpgraderWin* UpgraderWin;
    QSettings* configSetting;

private slots:
    void sUpdateNetworkConfig();
    void sUserRegister(QString name, QString password, QString invite_code, QString icon_path);
    void sUserSwitch(QString account, QString password);

    void sConnected();
    void sUnconnected();
    void sRegistered(int status_code, QString account, QString password);
    void sExpired(int status_code, QString new_invite_code);
    void sRejected(int status_code);
    void sValidate(int status_code, int account, QString name, QString icon);
    void sInvalidate(int status_code);

    void sVersionExpired();
};


#endif // WF_LOGINWIN_H
