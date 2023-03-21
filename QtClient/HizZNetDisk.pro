#-------------------------------------------------
#
# Project created by QtCreator 2023-03-10T16:25:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HizZNetDisk
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    logindialog.cpp \
    package.cpp \
    tcpTool.cpp \
    mainwindow.cpp \
    file_grid_layout.cpp \
    fileitem.cpp \
    fileindexlable.cpp \
    downloaditem.cpp \
    selectfiledialog.cpp \
    fileindexlist.cpp \
    uploadfilethreadpool.cpp \
    attribute.cpp \
    newfolderdialog.cpp

HEADERS += \
    myhelper.h \
    logindialog.h \
    package.h \
    tcpTool.h \
    mainwindow.h \
    file_grid_layout.h \
    fileitem.h \
    fileindexlable.h \
    downloaditem.h \
    selectfiledialog.h \
    fileindexlist.h \
    uploadfilethreadpool.h \
    attribute.h \
    newfolderdialog.h

FORMS += \
    logindialog.ui \
    mainwindow.ui \
    fileitem.ui \
    downloaditem.ui \
    selectfiledialog.ui \
    fileindexlist.ui \
    attribute.ui \
    newfolderdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
