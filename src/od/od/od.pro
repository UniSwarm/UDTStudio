
QT += core

TARGET = od
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += OD_EXPORT_LIB

HEADERS += \
    $$PWD/model/index.h \
    $$PWD/model/subindex.h \
    $$PWD/model/od.h \
    $$PWD/model/datastorage.h \
    $$PWD/parser/devicemodelparser.h \
    $$PWD/generator/generator.h \
    $$PWD/generator/cgenerator.h \
    $$PWD/od_global.h \
    $$PWD/parser/parser.h \
    $$PWD/writer/dcfwriter.h \
    $$PWD/model/deviceconfiguration.h \
    $$PWD/model/devicedescription.h \
    $$PWD/model/devicemodel.h \
    $$PWD/writer/deviceconfigurationwriter.h \
    $$PWD/writer/deviceiniwriter.h

SOURCES += \
    $$PWD/model/index.cpp \
    $$PWD/model/subindex.cpp \
    $$PWD/model/od.cpp \
    $$PWD/model/datastorage.cpp \
    $$PWD/parser/devicemodelparser.cpp \
    $$PWD/generator/generator.cpp \
    $$PWD/generator/cgenerator.cpp \
    $$PWD/parser/parser.cpp \
    $$PWD/writer/dcfwriter.cpp \
    $$PWD/model/deviceconfiguration.cpp \
    $$PWD/model/devicedescription.cpp \
    $$PWD/model/devicemodel.cpp \
    $$PWD/writer/deviceconfigurationwriter.cpp \
    $$PWD/writer/deviceiniwriter.cpp
