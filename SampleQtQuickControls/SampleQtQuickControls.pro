TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    camera_utils.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
CONFIG += c++11
HEADERS += \
    camera_utils.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lXmlParser_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lXmlParser_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lXmlParser_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lNodeMapData_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lNodeMapData_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lNodeMapData_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lMathParser_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lMathParser_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lMathParser_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -llog4cpp_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -llog4cpp_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -llog4cpp_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lLog_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lLog_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lLog_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lGenApi_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lGenApi_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lGenApi_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lGCBase_gcc48_v3_2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lGCBase_gcc48_v3_2
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lGCBase_gcc48_v3_2

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lbgapi2_img
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lbgapi2_img
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lbgapi2_img

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lbgapi2_genicam
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lbgapi2_genicam
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lbgapi2_genicam

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_genicam
DEPENDPATH += $$PWD/../baumer/include/bgapi2_genicam

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../baumer/lib/release/ -lbgapi2_ext_sc
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../baumer/lib/debug/ -lbgapi2_ext_sc
else:unix: LIBS += -L$$PWD/../baumer/lib/ -lbgapi2_ext_sc

INCLUDEPATH += $$PWD/../baumer/include/bgapi2_ext_sc
DEPENDPATH += $$PWD/../baumer/include/bgapi2_ext_sc


#This is the correct way, which isnt working for some reason
#win32 {
#    SHARED_LIB_FILES = $$files($$PWD/../baumer/lib/*.dll)
#    for(FILE, SHARED_LIB_FILES) {
#        BASENAME = $$basename(FILE)
#        LIBS += -L$$PWD/../baumer/lib/ -l$$replace(BASENAME,.dll,)
#    }
#}
#else:unix {
#    SHARED_LIB_FILES = $$files($$PWD/../baumer/lib/*.so)
#    for(FILE, SHARED_LIB_FILES) {
#        BASENAME = $$basename(FILE)
#        LIBS += -L$$PWD/../baumer/lib/ -l$$replace(BASENAME,.so,)
#        message(-L$$PWD/../baumer/lib/ -l$$replace(BASENAME,.so,))
#    }
#}

#INCLUDEPATH += $$PWD/../baumer/include
#DEPENDPATH += $$PWD/../baumer/include

#message($$LIBS)
