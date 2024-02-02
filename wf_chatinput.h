/**********************************
 *
 *       WeFish Chat Input
 *
 * Author: Kim.He@tymphany.com
 * date  : 11/24 2022
 *
***********************************/
#ifndef WF_CHATINPUT_H
#define WF_CHATINPUT_H

#include <QTextEdit>


class WF_ChatInput : public QTextEdit
{
    Q_OBJECT
public:
    WF_ChatInput(QWidget *parent);
    ~WF_ChatInput();

    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);

private:
    void dropImage(const QUrl& url, const QImage& image);
    void dropTextFile(const QUrl& url);

signals:
    void SendByKeySig();
};

#endif // WF_CHATINPUT_H
