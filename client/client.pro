#-------------------------------------------------
#
# Project created by QtCreator 2011-05-29T14:02:39
#
#-------------------------------------------------

QT += core gui network

TARGET = bin/client
TEMPLATE = app

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

SOURCES += \
    src/main.cpp\
    src/client.cpp \
    src/messagereceivingthread.cpp \
    src/listeningtcpserver.cpp

HEADERS += \
    src/client.h \
    src/messagereceivingthread.h \
    src/listeningtcpserver.h

FORMS += \
    src/client.ui
