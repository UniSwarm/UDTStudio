
QT       += core gui widgets

TARGET = udtstudio
TEMPLATE = app
DESTDIR = "$$PWD/../../bin"

CONFIG += c++11

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

HEADERS += \
    $$PWD/mainwindow.h

INCLUDEPATH += $$PWD/../od/od/ $$PWD/../udtgui/udtgui/

LIBS += -L"$$PWD/../../bin" -lod -lcanopen -ludtgui
DEPENDPATH += $$PWD/../od/od/ $$PWD/../udtgui/udtgui/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
