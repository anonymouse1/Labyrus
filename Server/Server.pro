#-------------------------------------------------
#
# Project created by QtCreator 2012-09-08T21:38:23
#
#-------------------------------------------------

QT       += core

QT       -= gui

QT       += network

greaterThan(QT_MAJOR_VERSION, 4): DEFINES += QT4


TARGET = Server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    server.cpp \
    mainclass.cpp \
    player.cpp

HEADERS += \
    server.h \
    mainclass.h \
    player.h
