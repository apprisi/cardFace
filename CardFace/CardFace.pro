#-------------------------------------------------
#
# Project created by QtCreator 2016-09-18T09:54:05
#
#-------------------------------------------------

QT       += core gui\
    sql multimedia\
    network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CardFace
TEMPLATE = app

TRANSLATIONS += ts/qtcn.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    infdialog.cpp \
    alldata.cpp \
    searchdialog.cpp \
    setipdialog.cpp \
    logindialog.cpp

HEADERS  += mainwindow.h \
    infdialog.h \
    alldata.h \
    searchdialog.h \
    setipdialog.h \
    logindialog.h

FORMS    += mainwindow.ui \
    infdialog.ui \
    alldata.ui \
    searchdialog.ui \
    setipdialog.ui \
    logindialog.ui

INCLUDEPATH += ../facedata/inc \
    ../facedata/src \
    ../netdatamgr/inc \
    ../netdatamgr/src \
    ../hkcapsdk/inc \
    ../hkcapsdk/src \
    ../hksdkvc \
    ../public/include/hk \
    ../nelvspeaker/inc \
    ../nelvcarnotify \

build_pass:CONFIG(debug, debug|release) {
	win32:LIBS += facedatad.lib -L../bin64
	win32:LIBS += hkcapsdkd.lib -L../bin64
	win32:LIBS += facedatad.lib nelvcarnotifyd.lib
	LIBS += netdatamgrd.lib
    TARGET = $$join(TARGET,,,d)
}

build_pass:CONFIG(release, debug|release) {
	 win32:LIBS += facedata.lib -L../bin64
	 win32:LIBS += hkcapsdk.lib -L../bin64
	 win32:LIBS += facedata.lib nelvcarnotify.lib
	 LIBS += netdatamgr.lib
}


win32:LIBS += hksdkvc.lib nelvspeaker.lib dogauthority.lib -L../bin64


DESTDIR = ../bin64

RESOURCES += \
    res.qrc

RC_ICONS =../CardFace/res/safe.ico
