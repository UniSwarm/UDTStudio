
QT += core gui widgets serialbus

TARGET = udtgui
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += UDTGUI_EXPORT_LIB

HEADERS += \
    $$PWD/udtgui_global.h \
    $$PWD/od/oditem.h \
    $$PWD/od/oditemmodel.h \
    $$PWD/od/odtreeview.h \
    $$PWD/od/odtreeviewdelegate.h \
    $$PWD/can/canSettingsDialog/cansettingsdialog.h \
    $$PWD/can/canFrameListView/canframelistview.h \
    $$PWD/can/canFrameListView/canframemodel.h \
    canopen/busnodesmodel.h \
    canopen/busnodestreeview.h

SOURCES += \
    $$PWD/od/oditem.cpp \
    $$PWD/od/oditemmodel.cpp \
    $$PWD/od/odtreeview.cpp \
    $$PWD/od/odtreeviewdelegate.cpp \
    $$PWD/can/canSettingsDialog/cansettingsdialog.cpp \
    $$PWD/can/canFrameListView/canframelistview.cpp \
    $$PWD/can/canFrameListView/canframemodel.cpp \
    canopen/busnodesmodel.cpp \
    canopen/busnodestreeview.cpp

INCLUDEPATH += $$PWD/../../lib/od/ $$PWD/../../lib/canopen/

LIBS += -L"$$PWD/../../../bin" -lod -lcanopen
DEPENDPATH += $$PWD/../../lib/od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
ls

