
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
    $$PWD/canopen/busmanagerwidget.h \
    $$PWD/canopen/busnodesmanagerview.h \
    $$PWD/canopen/busnodesmodel.h \
    $$PWD/canopen/busnodestreeview.h \
    $$PWD/canopen/nodemanagerwidget.h \
    $$PWD/canopen/nodeod/nodeoditem.h \
    $$PWD/canopen/nodeod/nodeoditemmodel.h \
    $$PWD/canopen/nodeod/nodeodtreeview.h \
    $$PWD/canopen/nodeod/nodeodfilterproxymodel.h

SOURCES += \
    $$PWD/od/oditem.cpp \
    $$PWD/od/oditemmodel.cpp \
    $$PWD/od/odtreeview.cpp \
    $$PWD/od/odtreeviewdelegate.cpp \
    $$PWD/can/canSettingsDialog/cansettingsdialog.cpp \
    $$PWD/can/canFrameListView/canframelistview.cpp \
    $$PWD/can/canFrameListView/canframemodel.cpp \
    $$PWD/canopen/busmanagerwidget.cpp \
    $$PWD/canopen/busnodesmanagerview.cpp \
    $$PWD/canopen/busnodesmodel.cpp \
    $$PWD/canopen/busnodestreeview.cpp \
    $$PWD/canopen/nodemanagerwidget.cpp \
    $$PWD/canopen/nodeod/nodeoditem.cpp \
    $$PWD/canopen/nodeod/nodeoditemmodel.cpp \
    $$PWD/canopen/nodeod/nodeodtreeview.cpp \
    $$PWD/canopen/nodeod/nodeodfilterproxymodel.cpp

INCLUDEPATH += $$PWD/../../lib/od/ $$PWD/../../lib/canopen/

LIBS += -L"$$PWD/../../../bin" -lod -lcanopen
DEPENDPATH += $$PWD/../../lib/od/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

RESOURCES += $$PWD/img.qrc
