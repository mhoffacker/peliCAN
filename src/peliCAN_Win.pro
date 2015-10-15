#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T13:10:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
lessThan(QT_MAJOR_VERSION, 5): CONFIG += serialport

TARGET = peliCAN
TEMPLATE = app
MOC_DIR = moc
OBJECTS_DIR = obj
UI_DIR = ui

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O0
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE -= -Os

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O0

unix {
    DEFINES  += USE_SOCKET_CAN
	DEFINES  += USE_QUADI
}

win32 {
    DEFINES  += __WINDOWS__
	DEFINES  += USE_QUADI
}


SOURCES += main.cpp\
        mainwindow.cpp \
    csocketcan.cpp \
    ccancomm.cpp \
    connectdialog.cpp \
    raw_tree_model.cpp \
    aboutdialog.cpp \
    cslcan.cpp \
    global_config.cpp \
    send_widget.cpp \
    dialogsenddata.cpp \
    send_widget_special.cpp \
    dialogquadi.cpp \
    libcandbc/dbcModel.c \
    libcandbc/dbcReader.c \
    libcandbc/dbcWriter.c \
    libcandbc/lexer.c \
    libcandbc/parser.c \
    ccanmessage.cpp \
    dialogdataview.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    csocketcan.h \
    ccancomm.h \
    connectdialog.h \
    raw_tree_model.h \
    aboutdialog.h \
    cslcan.h \
    global_config.h \
    send_widget.h \
    dialogsenddata.h \
    send_widget_special.h \
    dialogquadi.h \
    libcandbc/dbcModel.h \
    libcandbc/dbcReader.h \
    libcandbc/dbcTypes.h \
    libcandbc/dbcWriter.h \
    libcandbc/parser.h \
    ccanmessage.h \
    dialogdataview.h \
    qcustomplot.h

FORMS    += mainwindow.ui \
    connectdialog.ui \
    aboutdialog.ui \
    send_widget.ui \
    dialogsenddata.ui \
    send_widget_special.ui \
    dialogquadi.ui \
    dialogdataview.ui

RESOURCES += \
    resources.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release/lib/ -lqwtd

INCLUDEPATH += $$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release
DEPENDPATH += $$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release/lib/libqwt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release/lib/libqwtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release/lib/qwt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../../../qt/build-qwt-Desktop_Qt_5_5_0_MinGW_32bit-Release/lib/qwtd.lib
