#-------------------------------------------------
#
# Project created by QtCreator 2011-05-29T12:09:01
#
#-------------------------------------------------

QT += core gui network sql

TARGET = ../bin/server
TEMPLATE = app

OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

SOURCES += \
    src/main.cpp\
    src/server.cpp \
    src/sendmessagethread.cpp \
    src/message.cpp \
    src/addmessagedialog.cpp

HEADERS += \
    src/server.h \
    src/sendmessagethread.h \
    src/message.h \
    src/addmessagedialog.h

FORMS += \
    src/server.ui \
    src/addmessagedialog.ui
