
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
#RESOURCES += $$PWD/../../contrib/QD2/qdarkstyle/dark/style.qrc

isEmpty(PREFIX)
{
    PREFIX=/usr/local
}
target.path=$$PREFIX/bin
unix
{
    QMAKE_SUBSTITUTES += udtstudio.desktop.in
    desktop.path  = $$PREFIX/share/applications
    desktop.files = udtstudio.desktop
    INSTALLS += desktop
    icons.path    = $$PREFIX/share/icons
    icons.files   = ../lib/udtgui/img/udtstudio.ico
    INSTALLS += icons
}
!isEmpty(target.path): INSTALLS += target

win32|win64
{
    RC_FILE = udtstudio.rc
}
