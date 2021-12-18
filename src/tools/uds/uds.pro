
QT += core gui widgets
TARGET = uds
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

HEADERS += \

SOURCES += \
    $$PWD/uds.cpp

LIBS += -L"$$PWD/../../../bin"
android:LIBS += -lod_$${QT_ARCH} -ludtgui_$${QT_ARCH}
else:LIBS += -lod -ludtgui

INCLUDEPATH += $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/
DEPENDPATH += $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

isEmpty(PREFIX)
{
    PREFIX=/usr/local
}
target.path=$$PREFIX/bin
!isEmpty(target.path): INSTALLS += target
