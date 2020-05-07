#-------------------------------------------------
#
# Project created by QtCreator 2020-05-07T08:55:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SampleProject
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    status.cpp \
    log.cpp \
    report.cpp

HEADERS  += mainwindow.h \
    status.h \
    log.h \
    report.h

FORMS    += mainwindow.ui \
    status.ui \
    log.ui \
    report.ui
