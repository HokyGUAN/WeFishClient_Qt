/**********************************
 *
 *        WeFish Button
 *
 * Author: Kim.He@tymphany.com
 * date  : 11/24 2022
 *
***********************************/
#ifndef WF_BUTTON_H
#define WF_BUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include <QToolButton>

class WF_Button : public QPushButton
{
    Q_OBJECT
public:
    WF_Button(QWidget *parent, QString name, bool menu);
    ~WF_Button() {};

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *)Q_DECL_OVERRIDE;

private:
    bool menu_flag;
    QMenu* btn_menu;
    QAction* actionIpSet ;
    QAction* actionAuthorize;
};

class WF_CloseButton : public QToolButton
{
    Q_OBJECT
public:
    WF_CloseButton(QWidget * parent, int x, int y);
    ~WF_CloseButton();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event)Q_DECL_OVERRIDE;
};

class WF_MinimizeButton : public QToolButton
{
    Q_OBJECT
public:
    WF_MinimizeButton(QWidget * parent, int x, int y);
    ~WF_MinimizeButton();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event)Q_DECL_OVERRIDE;
};

class WF_LoginCloseButton : public QToolButton
{
    Q_OBJECT
public:
    WF_LoginCloseButton(QWidget * parent, int x, int y);
    ~WF_LoginCloseButton();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event)Q_DECL_OVERRIDE;
};

class WF_LoginSettingButton : public QToolButton
{
    Q_OBJECT
public:
    WF_LoginSettingButton(QWidget * parent, int x, int y);
    ~WF_LoginSettingButton();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event)Q_DECL_OVERRIDE;
};

class WF_ChatBoxEmojiButton : public QToolButton
{
    Q_OBJECT
public:
    WF_ChatBoxEmojiButton(QWidget * parent, int x, int y);
    ~WF_ChatBoxEmojiButton();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event)Q_DECL_OVERRIDE;
};

class WF_ChatBoxFileButton : public QToolButton
{
    Q_OBJECT
public:
    WF_ChatBoxFileButton(QWidget * parent, int x, int y);
    ~WF_ChatBoxFileButton();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *event)Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event)Q_DECL_OVERRIDE;
};

#endif // WF_BUTTON_H
