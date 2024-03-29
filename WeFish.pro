QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/base64.cc \
    main.cpp \
    wf_browser.cpp \
    wf_button.cpp \
    wf_chatinput.cpp \
    wf_chatview.cpp \
    wf_filesocket.cpp \
    wf_flistview.cpp \
    wf_loginwin.cpp \
    wf_mainsocket.cpp \
    wf_mainwin.cpp \
    wf_misc.cpp \
    wf_upgraderwin.cpp

HEADERS += \
    common/base64.hpp \
    common/json.h \
    common/jsonrpcpp.h \
    common/wf_message.h \
    wf_browser.h \
    wf_button.h \
    wf_chatinput.h \
    wf_chatview.h \
    wf_config.h \
    wf_filesocket.h \
    wf_flistview.h \
    wf_loginwin.h \
    wf_mainsocket.h \
    wf_mainwin.h \
    wf_misc.h \
    wf_upgraderwin.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Res.qrc

DISTFILES += \
    exe_ico.rc \
    hoky.ico

RC_FILE = exe_ico.rc

INCLUDEPATH += $$PWD/.

LIBS += -L$$PWD/. -lLibCryptor
