
QT       += core gui serialbus widgets

TARGET = ubl
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

SOURCES += \
        $$PWD/main.cpp \
	$$PWD/mainwindow.cpp \
	$$PWD/programdownload.cpp \
	$$PWD/parser/hexparser.cpp \
	$$PWD/writer/hexwriter.cpp \
	$$PWD/utility/hexmerger.cpp \
	$$PWD/parser/uniparser.cpp \
	$$PWD/process/mergeProcess.cpp \
	$$PWD/ubl.cpp \
	$$PWD/utility/createbinary.cpp \
	$$PWD/utility/phantomremove.cpp \
	$$PWD/process/updateprocess.cpp

HEADERS += \
        $$PWD/mainwindow.h \
	$$PWD/programdownload.h \
	$$PWD/parser/hexparser.h \
	$$PWD/writer/hexwriter.h \
	$$PWD/utility/hexmerger.h \
	$$PWD/parser/uniparser.h \
	$$PWD/process/mergeProcess.h \
	$$PWD/ubl.h \
	$$PWD/utility/createbinary.h \
	$$PWD/utility/phantomremove.h \
	$$PWD/process/updateprocess.h

INCLUDEPATH += $$PWD/../../lib/canopen/ $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/

LIBS += -L"$$PWD/../../../bin" -lod -lcanopen -ludtgui
DEPENDPATH += $$PWD/../lib/od/ $$PWD/../lib/udtgui/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

