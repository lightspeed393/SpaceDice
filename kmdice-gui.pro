#-------------------------------------------------
#
# Project created by QtCreator 2018-10-22T15:10:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kmdice-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    functions.cpp \
    stuff.c

HEADERS  += mainwindow.h \
    functions.h \
    stuff.h

FORMS    += mainwindow.ui

LIBS += -lcurl -ljansson

windows:INCLUDEPATH += ./include
windows:QMAKE_CXXFLAGS += /Zc:strictStrings-
#windows:LIBS += $$PWD/lib/libcurl_imp.lib $$PWD/lib/jansson.lib
windows:LIBS += -L$$PWD/lib/