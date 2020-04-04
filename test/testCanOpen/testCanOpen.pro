QT       += core gui widgets serialbus

TARGET = testCanOpen
TEMPLATE = app
DESTDIR = "$$PWD/../../bin"

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

HEADERS += \
    $$PWD/mainwindow.h

INCLUDEPATH += $$PWD/../../src/lib/od/ $$PWD/../../src/lib/udtgui/ $$PWD/../../src/lib/canopen/

LIBS += -L"$$PWD/../../bin" -lod -lcanopen -ludtgui
#DEPENDPATH += $$PWD/../../od/od/ $$PWD/../../udtgui/udtgui/
#unix:{
#    QMAKE_LFLAGS_RPATH=
#    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
#}
