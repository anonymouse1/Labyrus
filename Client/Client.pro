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
    commandsend.cpp \
    drawgl.cpp \
    drawthread.cpp \
    connection.cpp

HEADERS  += mainwindow.h \
    startdialog.h \
    commandsend.h \
    drawgl.h \
    drawthread.h \
    connection.h

FORMS    += \
    startDialog.ui \
    command.ui \
    mainwindow.ui \
    connection.ui
