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


QString WF_DIR = "C:\\Users\\"+QStandardPaths::writableLocation(QStandardPaths::HomeLocation).section("/", -1, -1)+"\\Documents\\WeFishFiles";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir wf_dir(WF_DIR);
    if (!wf_dir.exists()) {
        wf_dir.mkdir(WF_DIR);
    }

    QString WF_DesktopLinkPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "\\WeFish.lnk";
    QFile WF_DesktopLink(WF_DesktopLinkPath);
    if (WF_DesktopLink.exists()) {
        qDebug() << "WeFish Desktop Link do exist";
    } else {
        qDebug() << "WeFish Desktop Link do not exist";
        QFile WF_Exe(WF_DIR + "\\WeFish.exe");
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
