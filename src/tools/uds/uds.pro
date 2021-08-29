
QT += core gui widgets

TARGET = uds
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

HEADERS += \

SOURCES += \
    $$PWD/uds.cpp

INCLUDEPATH += $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/

LIBS += -L"$$PWD/../../../bin" -lod -ludtgui
DEPENDPATH += $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
