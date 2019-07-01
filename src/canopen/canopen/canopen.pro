
QT     += core gui widgets serialbus
TARGET = canopen
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += CANOPEN_EXPORT_LIB

CONFIG += c++11

SOURCES += \
    $$PWD/canopenbus.cpp \
    $$PWD/node.cpp \
    $$PWD/canopen.cpp \
    $$PWD/services/emergency.cpp \
    $$PWD/services/nmt.cpp \
    $$PWD/services/pdo.cpp \
    $$PWD/services/sdo.cpp \
    $$PWD/services/service.cpp \
    $$PWD/services/sync.cpp \
    $$PWD/services/timestamp.cpp

HEADERS += \
    $$PWD/canopen_global.h \
    $$PWD/canopenbus.h \
    $$PWD/node.h \
    $$PWD/canopen.h \
    $$PWD/services/emergency.h \
    $$PWD/services/nmt.h \
    $$PWD/services/pdo.h \
    $$PWD/services/sdo.h \
    $$PWD/services/service.h \
    $$PWD/services/services.h \
    $$PWD/services/sync.h \
    $$PWD/services/timestamp.h

INCLUDEPATH += $$PWD/../../od/od/

LIBS += -L"$$PWD/../../../bin" -lod
DEPENDPATH += $$PWD/../../od/od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
