
QT += core gui widgets

TARGET = udtgui
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += UDTGUI_EXPORT_LIB

HEADERS += \
    $$PWD/udtgui_global.h \
    $$PWD/od/oditem.h \
    $$PWD/od/oditemmodel.h \
    $$PWD/od/odtreeview.h \
    $$PWD/od/odtreeviewdelegate.h

SOURCES += \
    $$PWD/od/oditem.cpp \
    $$PWD/od/oditemmodel.cpp \
    $$PWD/od/odtreeview.cpp \
    $$PWD/od/odtreeviewdelegate.cpp

INCLUDEPATH += $$PWD/../../lib/od/ $$PWD/../../lib/canopen/

LIBS += -L"$$PWD/../../../bin" -lod -lcanopen
DEPENDPATH += $$PWD/../../lib/od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
ls
