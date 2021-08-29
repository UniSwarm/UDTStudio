
QT += core
TARGET = cood
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

HEADERS += \

SOURCES += \
    $$PWD/cood.cpp

LIBS += -L"$$PWD/../../../bin"
android:LIBS += -lod_$${QT_ARCH}
else:LIBS += -lod

INCLUDEPATH += $$PWD/../../lib/od/
DEPENDPATH += $$PWD/../../lib/od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
