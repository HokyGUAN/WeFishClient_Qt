/**********************************
 *
 *     WeFish Upgrader Window
 *
 * Author: hoky.guan@tymphany.com
 *
***********************************/
#ifndef WF_UPGRADERWIN_H
#define WF_UPGRADERWIN_H

#include "wf_misc.h"
#include "wf_filesocket.h"

#include <QProgressBar>


class WF_UpgraderWin : public WF_BaseWin
{
    Q_OBJECT
public:
    WF_UpgraderWin(QWidget *parent, WF_FileSocket* fileSocket);
    ~WF_UpgraderWin() {};

    void paintEvent(QPaintEvent *event);

    void doUpgradeRequest();
    void doUpgradeComplete();

private:
    QLabel *Percentage;
    QProgressBar *Bar;
    QPushButton *UpgradedButton;
    WF_FileSocket *FileSocket;

private slots:
    void sProgressUpdate(int percent);

};

#endif // WF_UPGRADERWIN_H
