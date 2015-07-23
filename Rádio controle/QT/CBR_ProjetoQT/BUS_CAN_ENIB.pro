#-------------------------------------------------
#
# Project created by QtCreator 2013-05-22T13:52:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BUS_CAN_ENIB
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    Serial.cpp

HEADERS  += widget.h \
    Serial.h

FORMS    += widget.ui
