
QT       += core gui widgets charts network
TARGET = udtstudio
TEMPLATE = app
DESTDIR = "$$PWD/../../bin"

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

HEADERS += \
    $$PWD/mainwindow.h

LIBS += -L"$$PWD/../../bin"
android:LIBS += -lod_$${QT_ARCH} -lcanopen_$${QT_ARCH} -ludtgui_$${QT_ARCH}
else:LIBS += -lod -lcanopen -ludtgui

INCLUDEPATH += $$PWD/../lib/od/ $$PWD/../lib/udtgui/ $$PWD/../lib/canopen/
DEPENDPATH += $$PWD/../lib/od/ $$PWD/../lib/udtgui/ $$PWD/../lib/canopen/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

RESOURCES += $$PWD/../../contrib/QDarkStyleSheet/qdarkstyle/style.qrc

win32|win64
{
    RC_FILE = udtstudio.rc
}
