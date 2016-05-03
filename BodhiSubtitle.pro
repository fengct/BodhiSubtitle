#-------------------------------------------------
#
# Project created by QtCreator 2016-02-19T20:36:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    bodhisession.cpp

HEADERS  += mainwindow.h \
    config.h \
    const.h \
    bodhiapp.h \
    bodhicore.h \
    parser.h \
    srtarchive.h \
    srtview.h \
    bodhisession.h

FORMS    += mainwindow.ui
