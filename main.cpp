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

    QSize* LoginWinSize = new QSize(280, 350);
    WF_LoginWin w(nullptr, LoginWinSize);

    w.show();
    return a.exec();
}
