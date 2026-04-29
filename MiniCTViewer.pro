#-------------------------------------------------
#
# Project created by QtCreator 2026-04-28T22:19:40
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MiniCTViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageview.cpp

HEADERS  += mainwindow.h \
    imageview.h

FORMS    += mainwindow.ui

CONFIG+= C++11
