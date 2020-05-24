TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    camera_utils.cpp \
    torch_utils.cpp \
    cv_utils.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
CONFIG += c++14
HEADERS += \
    camera_utils.h \
    torch_utils.h \
    cv_utils.h

QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
QMAKE_LFLAGS += -fopenmp -D_GLIBCXX_USE_CXX11_ABI=0

#Adding Baumer libraries
win32 {
    SHARED_LIB_FILES = $$files($$PWD/../baumer/lib/*.dll)
    for(FILE, SHARED_LIB_FILES) {
        BASENAME = $$basename(FILE)
        LIBS += -L$$PWD/../baumer/lib/ -l$$replace(BASENAME,.dll,)
    }
}
else:unix {
    SHARED_LIB_FILES = $$files($$PWD/../baumer/lib/lib*.so)
    for(FILE, SHARED_LIB_FILES) {
        BASENAME = $$basename(FILE)
        BASENAME2 = $$replace(BASENAME, lib, )
        LIBS += -L$$PWD/../baumer/lib/ -l$$replace(BASENAME2,.so,)
    }
}

INCLUDEPATH += $$PWD/../baumer/include
DEPENDPATH += $$PWD/../baumer/include

win32 {
    SHARED_LIB_FILES = $$files($$PWD/../libtorch/lib/*.dll)
    for(FILE, SHARED_LIB_FILES) {
        BASENAME = $$basename(FILE)
        LIBS += -L$$PWD/../libtorch/lib/ -l$$replace(BASENAME,.dll,)
    }
}
else:unix {
    SHARED_LIB_FILES = $$files($$PWD/../libtorch/lib/*.so)
    for(FILE, SHARED_LIB_FILES) {
        BASENAME = $$basename(FILE)
        BASENAME2 = $$replace(BASENAME, lib, )
        LIBS += -L$$PWD/../libtorch/lib/ -l$$replace(BASENAME2,.so,)
        message(-L$$PWD/../libtorch/lib/ -l$$replace(BASENAME2,.so,))
    }
#    SHARED_LIB_FILES2 = $$files($$PWD/../libtorch/lib/*.a)
#    for(FILE, SHARED_LIB_FILES2) {
#        BASENAME = $$basename(FILE)
#        BASENAME2 = $$replace(BASENAME, lib, )
#        LIBS += -L$$PWD/../libtorch/lib/ -l$$replace(BASENAME2,.a,)
#    }
#    PRE_TARGETDEPS += $$files($$PWD/../libtorch/lib/*.a)
}

INCLUDEPATH += $$PWD/../libtorch/include
DEPENDPATH += $$PWD/../libtorch/include

# Adding openCV libraries
#unix {
#    CONFIG += link_pkgconfig
#    PKGCONFIG += opencv
#}

INCLUDEPATH += -I/usr/local/include/opencv2

unix {
    SHARED_LIB_FILES = $$files(/usr/local/lib/*.so)
    for(FILE, SHARED_LIB_FILES) {
        BASENAME = $$basename(FILE)
        BASENAME2 = $$replace(BASENAME, lib, )
        LIBS += -L/usr/local/lib/ -l$$replace(BASENAME2,.so,)
#        message(-L/usr/local/lib/ -l$$replace(BASENAME2,.so,))
    }
}
