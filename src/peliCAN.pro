#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T13:10:55
#
#-------------------------------------------------

QT       += core gui script 

CONFIG	 += qwt

DEFINES  += USE_SOCKET_CAN

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = peliCAN
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    csocketcan.cpp \
    ccancomm.cpp \
    connectdialog.cpp \
    ccanfilter.cpp \
    ccansignal.cpp \
    raw_tree_model.cpp \
    aboutdialog.cpp \
    signal_tree_model.cpp \
    dialogsignaltree.cpp \
    csignaldisplay.cpp \
    dialograw2readable.cpp \
    multi_plot.cpp \
    cslcan.cpp \
    global_config.cpp

HEADERS  += mainwindow.h \
    csocketcan.h \
    ccancomm.h \
    connectdialog.h \
    ccanfilter.h \
    ccansignal.h \
    raw_tree_model.h \
    aboutdialog.h \
    signal_tree_model.h \
    dialogsignaltree.h \
    csignaldisplay.h \
    dialograw2readable.h \
    multi_plot.h \
    cslcan.h \
    global_config.h

FORMS    += mainwindow.ui \
    connectdialog.ui \
    aboutdialog.ui \
    dialogsignaltree.ui \
    dialograw2readable.ui

RESOURCES += \
    resources.qrc
