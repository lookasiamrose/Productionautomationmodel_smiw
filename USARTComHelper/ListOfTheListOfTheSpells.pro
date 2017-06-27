#-------------------------------------------------
#
# Project created by QtCreator 2017-04-17T22:00:08
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ListOfTheListOfTheSpells
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    uart.cpp \
    orderlist.cpp

HEADERS  += mainwindow.h \
    uart.h \
    orderlist.h

FORMS    += mainwindow.ui
