
QT += core

TARGET = cood
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

HEADERS += \

SOURCES += \
    $$PWD/main.cpp

INCLUDEPATH += $$PWD/../../lib/od/

LIBS += -L"$$PWD/../../../bin" -lod
DEPENDPATH += $$PWD/../../lib/od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
