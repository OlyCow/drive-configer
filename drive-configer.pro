#-------------------------------------------------
#
# Project created by QtCreator 2015-01-24T18:07:19
#
#-------------------------------------------------

CONFIG	  += c++11

QT		  += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET	  = drive-configer
TEMPLATE  = app


SOURCES += main.cpp\
        configwindow.cpp

HEADERS  += configwindow.h

FORMS    += configwindow.ui

RESOURCES += \
    icons.qrc

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
