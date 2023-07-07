QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./netapi/netapi.pri)
INCLUDEPATH += ./netapi/

include(./md5/md5.pri)
INCLUDEPATH += ./md5/

include(./audioapi/audioapi.pri)
INCLUDEPATH += ./AudioApi/

include(./videoapi/videoapi.pri)
INCLUDEPATH += ./VideoApi/



SOURCES += \
    ckernel.cpp \
    logindialog.cpp \
    main.cpp \
    maindialog.cpp \
    roomdialog.cpp \
    userwidget.cpp

HEADERS += \
    ckernel.h \
    logindialog.h \
    maindialog.h \
    roomdialog.h \
    userwidget.h

FORMS += \
    logindialog.ui \
    maindialog.ui \
    roomdialog.ui \
    userwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    resource/images/1.jpg \
    resource/images/Help.png \
    resource/images/bold.png \
    resource/images/cancel.png \
    resource/images/clear.png \
    resource/images/color.png \
    resource/images/down.png \
    resource/images/file.ico \
    resource/images/folders.png \
    resource/images/font.png \
    resource/images/groupChat 2.png \
    resource/images/hat.png \
    resource/images/history.png \
    resource/images/ic_max_huanyuan.png \
    resource/images/ic_max_zuida.png \
    resource/images/ic_sysmen.png \
    resource/images/icon1.png \
    resource/images/icon2.png \
    resource/images/message.ico \
    resource/images/message.png \
    resource/images/ok.png \
    resource/images/q151020-01.png \
    resource/images/qq151217-02.png \
    resource/images/quit.png \
    resource/images/register.png \
    resource/images/right.png \
    resource/images/save.png \
    resource/images/searchbox_button.png \
    resource/images/send.png \
    resource/images/sett.png \
    resource/images/share.png \
    resource/images/style.png \
    resource/images/tuer.png \
    resource/images/under.png \
    resource/tx/16.png \
    resource/tx/17.png \
    resource/tx/18.png \
    resource/tx/19.png \
    resource/tx/20.png \
    resource/tx/21.png \
    resource/tx/22.png \
    resource/tx/23.png \
    resource/tx/24.png \
    resource/tx/25.png \
    resource/tx/26.png \
    resource/tx/27.png \
    resource/tx/28.png \
    resource/tx/29.png \
    resource/tx/30.png \
    resource/tx/31.png \
    resource/tx/32.png \
    resource/tx/33.png \
    resource/tx/34.png \
    resource/tx/35.png \
    resource/tx/36.jpg \
    素材/images/1.jpg \
    素材/images/Help.png \
    素材/images/bold.png \
    素材/images/cancel.png \
    素材/images/clear.png \
    素材/images/color.png \
    素材/images/down.png \
    素材/images/file.ico \
    素材/images/folders.png \
    素材/images/font.png \
    素材/images/groupChat 2.png \
    素材/images/hat.png \
    素材/images/history.png \
    素材/images/ic_max_huanyuan.png \
    素材/images/ic_max_zuida.png \
    素材/images/ic_sysmen.png \
    素材/images/icon1.png \
    素材/images/icon2.png \
    素材/images/message.ico \
    素材/images/message.png \
    素材/images/ok.png \
    素材/images/q151020-01.png \
    素材/images/qq151217-02.png \
    素材/images/quit.png \
    素材/images/register.png \
    素材/images/right.png \
    素材/images/save.png \
    素材/images/searchbox_button.png \
    素材/images/send.png \
    素材/images/sett.png \
    素材/images/share.png \
    素材/images/style.png \
    素材/images/tuer.png \
    素材/images/under.png \
    素材/tx/0.png \
    素材/tx/1.png \
    素材/tx/10.png \
    素材/tx/11.png \
    素材/tx/12.png \
    素材/tx/13.png \
    素材/tx/14.png \
    素材/tx/15.png \
    素材/tx/16.png \
    素材/tx/17.png \
    素材/tx/18.png \
    素材/tx/19.png \
    素材/tx/2.png \
    素材/tx/20.png \
    素材/tx/21.png \
    素材/tx/22.png \
    素材/tx/23.png \
    素材/tx/24.png \
    素材/tx/25.png \
    素材/tx/26.png \
    素材/tx/27.png \
    素材/tx/28.png \
    素材/tx/29.png \
    素材/tx/3.png \
    素材/tx/30.png \
    素材/tx/31.png \
    素材/tx/32.png \
    素材/tx/33.png \
    素材/tx/34.png \
    素材/tx/35.png \
    素材/tx/36.jpg \
    素材/tx/4.png \
    素材/tx/5.png \
    素材/tx/6.png \
    素材/tx/7.png \
    素材/tx/8.png \
    素材/tx/9.png
