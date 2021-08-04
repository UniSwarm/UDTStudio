
QT += core gui widgets serialbus charts

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
    $$PWD/canopen/nodeod/nodeoditemdelegate.h \
    $$PWD/canopen/nodeod/nodeodtreeview.h \
    $$PWD/canopen/nodeod/nodeodfilterproxymodel.h \
    $$PWD/canopen/nodeod/nodeodwidget.h \
    $$PWD/canopen/datalogger/dataloggerwidget.h \
    $$PWD/canopen/datalogger/dataloggertreeview.h \
    $$PWD/canopen/datalogger/dataloggermodel.h \
    $$PWD/canopen/datalogger/dataloggermanagerwidget.h \
    $$PWD/canopen/datalogger/dataloggerchartswidget.h \
    $$PWD/canopen/pdo/nodepdomappingwidget.h \
    $$PWD/canopen/pdo/pdomappingpainter.h \
    $$PWD/canopen/pdo/pdomappingview.h \
    $$PWD/canopen/pdo/pdomappingwidget.h \
    $$PWD/canopen/widget/abstractindexwidget.h \
    $$PWD/canopen/widget/indexcheckbox.h \
    $$PWD/canopen/widget/indexcombobox.h \
    $$PWD/canopen/widget/indexlabel.h \
    $$PWD/canopen/widget/indexspinbox.h \
    $$PWD/screen/nodescreenswidget.h \
    $$PWD/screen/nodescreen.h \
    $$PWD/screen/nodescreenhome.h \
    $$PWD/screen/nodescreenod.h \
    $$PWD/screen/nodescreenpdo.h \
    $$PWD/screen/nodescreenumcmotor.h \
    $$PWD/screen/nodescreensynchro.h \
    $$PWD/canopen/compositeWidget/motionsensorwidget.h \
    $$PWD/canopen/compositeWidget/motorwidget.h \
    $$PWD/canopen/compositeWidget/pidwidget.h \
    $$PWD/canopen/datalogger/dataloggersingleton.h \
    $$PWD/canopen/profileWidget/p402/p402dtywidget.h \
    $$PWD/canopen/profileWidget/p402/p402ipwidget.h \
    $$PWD/canopen/profileWidget/p402/p402mode.h \
    $$PWD/canopen/profileWidget/p402/p402optionwidget.h \
    $$PWD/canopen/profileWidget/p402/p402ppwidget.h \
    $$PWD/canopen/profileWidget/p402/p402tqwidget.h \
    $$PWD/canopen/profileWidget/p402/p402vlwidget.h \
    $$PWD/canopen/profileWidget/p402/p402widget.h \
    $$PWD/canopen/profileWidget/p401/p401channelwidget.h \
    $$PWD/canopen/profileWidget/p401/p401inputwidget.h \
    $$PWD/canopen/profileWidget/p401/p401outputwidget.h \
    $$PWD/canopen/profileWidget/p401/p401widget.h \
    $$PWD/screen/nodescreenuio.h \
    $$PWD/canopen/profileWidget/p401/p401inputoptionwidget.h \
    $$PWD/canopen/profileWidget/p401/p401outputoptionwidget.h

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
    $$PWD/canopen/nodeod/nodeoditemdelegate.cpp \
    $$PWD/canopen/nodeod/nodeodtreeview.cpp \
    $$PWD/canopen/nodeod/nodeodfilterproxymodel.cpp \
    $$PWD/canopen/nodeod/nodeodwidget.cpp \
    $$PWD/canopen/datalogger/dataloggerwidget.cpp \
    $$PWD/canopen/datalogger/dataloggertreeview.cpp \
    $$PWD/canopen/datalogger/dataloggermodel.cpp \
    $$PWD/canopen/datalogger/dataloggermanagerwidget.cpp \
    $$PWD/canopen/datalogger/dataloggerchartswidget.cpp \
    $$PWD/canopen/pdo/nodepdomappingwidget.cpp \
    $$PWD/canopen/pdo/pdomappingpainter.cpp \
    $$PWD/canopen/pdo/pdomappingview.cpp \
    $$PWD/canopen/pdo/pdomappingwidget.cpp \
    $$PWD/canopen/widget/abstractindexwidget.cpp \
    $$PWD/canopen/widget/indexcheckbox.cpp \
    $$PWD/canopen/widget/indexcombobox.cpp \
    $$PWD/canopen/widget/indexlabel.cpp \
    $$PWD/canopen/widget/indexspinbox.cpp \
    $$PWD/screen/nodescreenswidget.cpp \
    $$PWD/screen/nodescreen.cpp \
    $$PWD/screen/nodescreenhome.cpp \
    $$PWD/screen/nodescreenod.cpp \
    $$PWD/screen/nodescreenpdo.cpp \
    $$PWD/screen/nodescreenumcmotor.cpp \
    $$PWD/screen/nodescreensynchro.cpp \
    $$PWD/canopen/compositeWidget/motionsensorwidget.cpp \
    $$PWD/canopen/compositeWidget/motorwidget.cpp \
    $$PWD/canopen/compositeWidget/pidwidget.cpp \
    $$PWD/canopen/datalogger/dataloggersingleton.cpp \
    $$PWD/canopen/profileWidget/p402/p402dtywidget.cpp \
    $$PWD/canopen/profileWidget/p402/p402ipwidget.cpp \
    $$PWD/canopen/profileWidget/p402/p402mode.cpp \
    $$PWD/canopen/profileWidget/p402/p402optionwidget.cpp \
    $$PWD/canopen/profileWidget/p402/p402ppwidget.cpp \
    $$PWD/canopen/profileWidget/p402/p402tqwidget.cpp \
    $$PWD/canopen/profileWidget/p402/p402vlwidget.cpp \
    $$PWD/canopen/profileWidget/p402/p402widget.cpp \
    $$PWD/canopen/profileWidget/p401/p401channelwidget.cpp \
    $$PWD/canopen/profileWidget/p401/p401inputwidget.cpp \
    $$PWD/canopen/profileWidget/p401/p401outputwidget.cpp \
    $$PWD/canopen/profileWidget/p401/p401widget.cpp \
    $$PWD/screen/nodescreenuio.cpp \
    $$PWD/canopen/profileWidget/p401/p401inputoptionwidget.cpp \
    $$PWD/canopen/profileWidget/p401/p401outputoptionwidget.cpp

INCLUDEPATH += $$PWD/../../lib/od/ $$PWD/../../lib/canopen/

LIBS += -L"$$PWD/../../../bin" -lod -lcanopen
DEPENDPATH += $$PWD/../../lib/od/ $$PWD/../../lib/canopen/
unix:{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

RESOURCES += $$PWD/img.qrc
