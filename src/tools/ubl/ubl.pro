#-------------------------------------------------
#
# Project created by QtCreator 2020-03-04T09:00:16
#
#-------------------------------------------------

QT       += core gui serialbus widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ubl
TEMPLATE = app
DESTDIR = "$$PWD/../../../bin"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        hexfile.cpp \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        hexfile.h \
        mainwindow.h

FORMS +=

INCLUDEPATH += $$PWD/../../lib/canopen/ $$PWD/../../lib/od/ $$PWD/../../lib/udtgui/can/canSettingsDialog/ $$PWD/../../lib/udtgui/can/canFrameListView/

LIBS += -L"$$PWD/../../../bin" -lod -ludtgui -lcanopen
#DEPENDPATH += $$PWD/../od/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES +=

RESOURCES +=
