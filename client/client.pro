#-------------------------------------------------
#
# Project created by QtCreator 2011-05-29T14:02:39
#
#-------------------------------------------------

QT += core gui network

CONFIG += crypto

TARGET = ../bin/client
TEMPLATE = app

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

SOURCES += \
    src/main.cpp\
    src/client.cpp \
    src/messagereceivingthread.cpp \
    src/serverconnectiontcpserver.cpp

HEADERS += \
    src/client.h \
    src/messagereceivingthread.h \
    src/serverconnectiontcpserver.h

FORMS += \
    src/client.ui
