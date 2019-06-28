
QT     += core gui widgets serialbus
TARGET = canopen
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += CANOPEN_EXPORT_LIB

CONFIG += c++11

SOURCES += \
    $$PWD/canopenbus.cpp \
    $$PWD/canopenbusdevice.cpp \
    $$PWD/node.cpp \
    $$PWD/canopen.cpp

HEADERS += \
    $$PWD/canopen_global.h \
    $$PWD/canopenbus.h \
    $$PWD/canopenbusdevice.h \
    $$PWD/node.h \
    $$PWD/canopen.h
