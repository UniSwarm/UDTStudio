
QT += core

TARGET = od
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += OD_EXPORT_LIB

HEADERS += \
    $$PWD/model/index.h \
    $$PWD/model/subindex.h \
    $$PWD/generator/generator.h \
    $$PWD/generator/cgenerator.h \
    $$PWD/generator/texgenerator.h \
    $$PWD/od_global.h \
    $$PWD/writer/dcfwriter.h \
    $$PWD/model/deviceconfiguration.h \
    $$PWD/model/devicedescription.h \
    $$PWD/model/devicemodel.h \
    $$PWD/writer/deviceconfigurationwriter.h \
    $$PWD/writer/deviceiniwriter.h \
    $$PWD/writer/devicedescriptionwriter.h \
    $$PWD/writer/edswriter.h \
    $$PWD/parser/dcfparser.h \
    $$PWD/parser/deviceconfigurationparser.h \
    $$PWD/parser/deviceiniparser.h \
    $$PWD/parser/devicedescriptionparser.h \
    $$PWD/parser/edsparser.h \
    $$PWD/db/oddb.h \
    $$PWD/db/odindexdb.h

SOURCES += \
    $$PWD/model/index.cpp \
    $$PWD/model/subindex.cpp \
    $$PWD/generator/generator.cpp \
    $$PWD/generator/cgenerator.cpp \
    $$PWD/generator/texgenerator.cpp \
    $$PWD/writer/dcfwriter.cpp \
    $$PWD/model/deviceconfiguration.cpp \
    $$PWD/model/devicedescription.cpp \
    $$PWD/model/devicemodel.cpp \
    $$PWD/writer/deviceconfigurationwriter.cpp \
    $$PWD/writer/deviceiniwriter.cpp \
    $$PWD/writer/devicedescriptionwriter.cpp \
    $$PWD/writer/edswriter.cpp \
    $$PWD/parser/dcfparser.cpp \
    $$PWD/parser/deviceconfigurationparser.cpp \
    $$PWD/parser/deviceiniparser.cpp \
    $$PWD/parser/devicedescriptionparser.cpp \
    $$PWD/parser/edsparser.cpp \
    $$PWD/db/oddb.cpp \
    $$PWD/db/odindexdb.cpp
