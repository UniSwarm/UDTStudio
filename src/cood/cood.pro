
QT += core

TARGET = cood
TEMPLATE = app
DESTDIR = "$$PWD/../../bin"

HEADERS += \

SOURCES += \
    $$PWD/main.cpp

INCLUDEPATH += $$PWD/../od/

LIBS += -L"$$PWD/../../bin" -lod
DEPENDPATH += $$PWD/../od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
