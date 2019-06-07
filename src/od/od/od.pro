
QT += core

TARGET = od
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += OD_EXPORT_LIB

HEADERS += \
    $$PWD/model/index.h \
    $$PWD/model/subindex.h \
    $$PWD/model/od.h \
    $$PWD/model/datatype.h \
    $$PWD/parser/edsparser.h \
    $$PWD/generator/generator.h \
    $$PWD/od_global.h

SOURCES += \
    $$PWD/model/index.cpp \
    $$PWD/model/subindex.cpp \
    $$PWD/model/od.cpp \
    $$PWD/model/datatype.cpp \
    $$PWD/parser/edsparser.cpp \
    $$PWD/generator/generator.cpp

