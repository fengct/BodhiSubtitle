#-------------------------------------------------
#
# Project created by QtCreator 2016-02-19T20:36:27
#
#-------------------------------------------------

QT       += core gui multimedia 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quickwidgets

TARGET = BodhiSubtitle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    config.cpp \
    bodhiapp.cpp \
    srtarchive.cpp \
    parser.cpp \
    bodhicore.cpp \
    srtview.cpp \
    bodhisession.cpp \
    bodhiplayer.cpp \
    commands.cpp \
    textview.cpp

HEADERS  += mainwindow.h \
    config.h \
    const.h \
    bodhiapp.h \
    bodhicore.h \
    parser.h \
    srtarchive.h \
    srtview.h \
    bodhisession.h \
    bodhiplayer.h \
    commands.h \
    textview.h \
    h/charsetdetect.h

FORMS    += mainwindow.ui

win32: LIBS += -L$$PWD/libs/ -lcharsetdetect
