
QT       += core gui serialbus widgets

TARGET = ubl
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

SOURCES += \
        $$PWD/main.cpp \
	$$PWD/mainwindow.cpp \
	$$PWD/process/updateprocess.cpp \
	$$PWD/mainconsole.cpp

HEADERS += \
        $$PWD/mainwindow.h \
	$$PWD/process/updateprocess.h \
	$$PWD/mainconsole.h

INCLUDEPATH += $$PWD/../../lib/canopen/ $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/

LIBS += -L"$$PWD/../../../bin" -lod -lcanopen -ludtgui
DEPENDPATH += $$PWD/../lib/od/ $$PWD/../lib/udtgui/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

