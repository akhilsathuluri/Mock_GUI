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
    report.cpp \
    camera.cpp

HEADERS  += mainwindow.h \
    status.h \
    log.h \
    report.h \
    camera.h

FORMS    += mainwindow.ui \
    status.ui \
    log.ui \
    report.ui \
    camera.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lbgapi2_genicam
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lbgapi2_genicam
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lbgapi2_genicam

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam
