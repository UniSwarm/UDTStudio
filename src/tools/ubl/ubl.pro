
QT       += core gui widgets
TARGET = ubl
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

SOURCES += \
        $$PWD/main.cpp \
	$$PWD/mainwindow.cpp \
	$$PWD/mainconsole.cpp

HEADERS += \
        $$PWD/mainwindow.h \
	$$PWD/mainconsole.h

LIBS += -L"$$PWD/../../../bin"
android:LIBS += -lod_$${QT_ARCH} -lcanopen_$${QT_ARCH} -ludtgui_$${QT_ARCH}
else:LIBS += -lod -lcanopen -ludtgui

INCLUDEPATH += $$PWD/../../lib/canopen/ $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/
DEPENDPATH += $$PWD/../lib/od/ $$PWD/../lib/canopen/ $$PWD/../lib/udtgui/
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
