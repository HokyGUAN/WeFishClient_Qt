/**********************************
 *
 *           WeFish Main
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#include "wf_loginwin.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QDebug>
#include <QStandardPaths>


#ifdef QT_NO_DEBUG
QString WF_DIR = "C:\\Users\\"+QStandardPaths::writableLocation(QStandardPaths::HomeLocation).section("/", -1, -1)+"\\Documents\\WeFish Files";
#else
QString WF_DIR = "C:\\Users\\"+QStandardPaths::writableLocation(QStandardPaths::HomeLocation).section("/", -1, -1)+"\\Documents\\WeFish Files Debug";
#endif
QString WF_ALL_DIR = WF_DIR + "\\All Users";
QString WF_CONF_DIR = WF_ALL_DIR + "\\config";
QString WF_UPDT_DIR = WF_ALL_DIR + "\\updt";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir wf_dir(WF_DIR);
    if (!wf_dir.exists()) {
        wf_dir.mkdir(WF_DIR);
    }
    QDir wf_all_dir(WF_ALL_DIR);
    if (!wf_all_dir.exists()) {
        wf_all_dir.mkdir(WF_ALL_DIR);
    }
    QDir wf_conf_dir(WF_CONF_DIR);
    if (!wf_conf_dir.exists()) {
        wf_conf_dir.mkdir(WF_CONF_DIR);
    }
    QDir wf_updt_dir(WF_UPDT_DIR);
    if (!wf_updt_dir.exists()) {
        wf_updt_dir.mkdir(WF_UPDT_DIR);
    }

    QString WF_DesktopLinkPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "\\WeFish.lnk";
    QFile WF_DesktopLink(WF_DesktopLinkPath);
    if (WF_DesktopLink.exists()) {
        qDebug() << "WeFish Desktop Link do exist";
    } else {
        qDebug() << "WeFish Desktop Link do not exist";
        QFile WF_Exe(WF_UPDT_DIR + "\\WeFish.exe");
        if (WF_Exe.exists()) {
            qDebug() << "WeFish Exe in WF_DIR do exist";
            qDebug() << "Create WeFish Desktop Link";
            WF_Exe.link(WF_DesktopLinkPath);
        }
    }

    QSize* LoginWinSize = new QSize(280, 380);
    WF_LoginWin w(nullptr, LoginWinSize);

    w.show();
    return a.exec();
}
