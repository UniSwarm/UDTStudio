
QT += core
TARGET = od
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += OD_EXPORT_LIB

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}

HEADERS += \
    $$PWD/od_global.h \
    $$PWD/db/oddb.h \
    $$PWD/db/odindexdb.h \
    $$PWD/generator/cgenerator.h \
    $$PWD/generator/csvgenerator.h \
    $$PWD/generator/generator.h \
    $$PWD/generator/texgenerator.h \
    $$PWD/model/deviceconfiguration.h \
    $$PWD/model/devicedescription.h \
    $$PWD/model/devicemodel.h \
    $$PWD/model/index.h \
    $$PWD/model/subindex.h \
    $$PWD/parser/dcfparser.h \
    $$PWD/parser/deviceconfigurationparser.h \
    $$PWD/parser/devicedescriptionparser.h \
    $$PWD/parser/deviceiniparser.h \
    $$PWD/parser/edsparser.h \
    $$PWD/utility/configurationapply.h \
    $$PWD/utility/odmerger.h \
    $$PWD/utility/profileduplicate.h \
    $$PWD/writer/dcfwriter.h \
    $$PWD/writer/deviceconfigurationwriter.h \
    $$PWD/writer/devicedescriptionwriter.h \
    $$PWD/writer/deviceiniwriter.h \
    $$PWD/writer/edswriter.h \

SOURCES += \
    $$PWD/db/oddb.cpp \
    $$PWD/db/odindexdb.cpp \
    $$PWD/generator/cgenerator.cpp \
    $$PWD/generator/csvgenerator.cpp \
    $$PWD/generator/generator.cpp \
    $$PWD/generator/texgenerator.cpp \
    $$PWD/model/deviceconfiguration.cpp \
    $$PWD/model/devicedescription.cpp \
    $$PWD/model/devicemodel.cpp \
    $$PWD/model/index.cpp \
    $$PWD/model/subindex.cpp \
    $$PWD/parser/dcfparser.cpp \
    $$PWD/parser/deviceconfigurationparser.cpp \
    $$PWD/parser/devicedescriptionparser.cpp \
    $$PWD/parser/deviceiniparser.cpp \
    $$PWD/parser/edsparser.cpp \
    $$PWD/utility/configurationapply.cpp \
    $$PWD/utility/odmerger.cpp \
    $$PWD/utility/profileduplicate.cpp \
    $$PWD/writer/dcfwriter.cpp \
    $$PWD/writer/deviceconfigurationwriter.cpp \
    $$PWD/writer/devicedescriptionwriter.cpp \
    $$PWD/writer/deviceiniwriter.cpp \
    $$PWD/writer/edswriter.cpp

isEmpty(PREFIX)
{
    PREFIX=/usr/local
}
target.path=$$PREFIX/bin
!isEmpty(target.path): INSTALLS += target
