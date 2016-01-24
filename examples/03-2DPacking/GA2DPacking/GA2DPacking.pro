#-------------------------------------------------
#
# Project created by QtCreator 2016-01-24T17:39:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GA2DPacking
TEMPLATE = app

CONFIG += c++11
INCLUDEPATH += ../../../src/


SOURCES += main.cpp\
        mainwindow.cpp \
    painter.cpp

HEADERS  += mainwindow.h \
    ../../../src/sga.hpp \
    painter.h \
    ga.h

FORMS    += mainwindow.ui
