#-------------------------------------------------
#
# Project created by QtCreator 2012-09-08T22:04:35
#
#-------------------------------------------------

QT       += core gui

QT       += network

QT       += opengl
LIBS += -L/usr/lib -lglut -lGLU
greaterThan(QT_MAJOR_VERSION, 4):

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    startdialog.cpp \
    drawgl.cpp \
    connection.cpp \
    networkclass.cpp \
    calculationthread.cpp \
    messagesstack.cpp \
    model.cpp

HEADERS  += mainwindow.h \
    startdialog.h \
    drawgl.h \
    connection.h \
    networkclass.h \
    fieldinformation.h \
    calculationthread.h \
    messagesstack.h \
    model.h

FORMS    += \
    startDialog.ui \
    mainwindow.ui \
    connection.ui
