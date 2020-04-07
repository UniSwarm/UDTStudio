
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
    $$PWD/nodeobjectid.cpp \
    $$PWD/nodeod.cpp \
    $$PWD/nodeodsubscriber.cpp \
    $$PWD/nodeindex.cpp \
    $$PWD/nodesubindex.cpp \
    $$PWD/canopen.cpp \
    $$PWD/services/emergency.cpp \
    $$PWD/services/nmt.cpp \
    $$PWD/services/tpdo.cpp \
    $$PWD/services/rpdo.cpp \
    $$PWD/services/sdo.cpp \
    $$PWD/services/service.cpp \
    $$PWD/services/sync.cpp \
    $$PWD/services/timestamp.cpp \
    $$PWD/services/errorcontrol.cpp \
    $$PWD/services/servicedispatcher.cpp \
    $$PWD/services/nodediscover.cpp \
    $$PWD/datalogger/datalogger.cpp \
    $$PWD/datalogger/dldata.cpp


HEADERS += \
    $$PWD/canopen_global.h \
    $$PWD/canopenbus.h \
    $$PWD/node.h \
    $$PWD/nodeobjectid.h \
    $$PWD/nodeod.h \
    $$PWD/nodeodsubscriber.h \
    $$PWD/nodeindex.h \
    $$PWD/nodesubindex.h \
    $$PWD/canopen.h \
    $$PWD/services/emergency.h \
    $$PWD/services/nmt.h \
    $$PWD/services/tpdo.h \
    $$PWD/services/rpdo.h \
    $$PWD/services/sdo.h \
    $$PWD/services/service.h \
    $$PWD/services/services.h \
    $$PWD/services/sync.h \
    $$PWD/services/timestamp.h \
    $$PWD/services/errorcontrol.h \
    $$PWD/services/servicedispatcher.h \
    $$PWD/services/nodediscover.h \
    $$PWD/datalogger/datalogger.h \
    $$PWD/datalogger/dldata.h

INCLUDEPATH += $$PWD/../od/

LIBS += -L"$$PWD/../../../bin" -lod
DEPENDPATH += $$PWD/../od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
