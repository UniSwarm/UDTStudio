
QT     += core gui widgets network
TARGET = canopen
TEMPLATE = lib
DESTDIR = "$$PWD/../../../bin"

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += CANOPEN_EXPORT_LIB

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}

SOURCES += \
    $$PWD/canopen.cpp \
    $$PWD/canopenbus.cpp \
    $$PWD/node.cpp \
    $$PWD/nodeod.cpp \
    $$PWD/nodeindex.cpp \
    $$PWD/nodesubindex.cpp \
    $$PWD/nodeobjectid.cpp \
    $$PWD/nodeodsubscriber.cpp \
    $$PWD/services/service.cpp \
    $$PWD/services/emergency.cpp \
    $$PWD/services/nmt.cpp \
    $$PWD/services/pdo.cpp \
    $$PWD/services/tpdo.cpp \
    $$PWD/services/rpdo.cpp \
    $$PWD/services/sdo.cpp \
    $$PWD/services/sync.cpp \
    $$PWD/services/timestamp.cpp \
    $$PWD/services/errorcontrol.cpp \
    $$PWD/services/servicedispatcher.cpp \
    $$PWD/services/nodediscover.cpp \
    $$PWD/datalogger/datalogger.cpp \
    $$PWD/datalogger/dldata.cpp \
    $$PWD/profile/nodeprofile.cpp \
    $$PWD/profile/nodeprofilefactory.cpp \
    $$PWD/profile/p402/nodeprofile402.cpp \
    $$PWD/profile/p402/mode.cpp \
    $$PWD/profile/p402/modedty.cpp \
    $$PWD/profile/p402/modeip.cpp \
    $$PWD/profile/p402/modetq.cpp \
    $$PWD/profile/p402/modevl.cpp \
    $$PWD/profile/p402/modepp.cpp \
    $$PWD/indexdb.cpp \
    $$PWD/indexdb401.cpp \
    $$PWD/indexdb402.cpp \
    $$PWD/busdriver/qcanbusframe.cpp \
    $$PWD/busdriver/canbusdriver.cpp \
    $$PWD/busdriver/canbustcpudt.cpp \
    $$PWD/bootloader/bootloader.cpp \
    $$PWD/bootloader/model/ufwmodel.cpp \
    $$PWD/bootloader/parser/hexparser.cpp \
    $$PWD/bootloader/parser/ufwparser.cpp \
    $$PWD/bootloader/utility/hexmerger.cpp \
    $$PWD/bootloader/utility/phantomremover.cpp \
    $$PWD/bootloader/utility/ufwupdate.cpp \
    $$PWD/bootloader/writer/hexwriter.cpp \
    $$PWD/bootloader/writer/ufwwriter.cpp \
    $$PWD/profile/p402/modecp.cpp \
    $$PWD/profile/p402/modehm.cpp \
    $$PWD/profile/p402/modepc.cpp

HEADERS += \
    $$PWD/canopen.h \
    $$PWD/canopen_global.h \
    $$PWD/canopenbus.h \
    $$PWD/node.h \
    $$PWD/nodeod.h \
    $$PWD/nodeindex.h \
    $$PWD/nodesubindex.h \
    $$PWD/nodeobjectid.h \
    $$PWD/nodeodsubscriber.h \
    $$PWD/services/service.h \
    $$PWD/services/services.h \
    $$PWD/services/emergency.h \
    $$PWD/services/nmt.h \
    $$PWD/services/pdo.h \
    $$PWD/services/tpdo.h \
    $$PWD/services/rpdo.h \
    $$PWD/services/sdo.h \
    $$PWD/services/sync.h \
    $$PWD/services/timestamp.h \
    $$PWD/services/errorcontrol.h \
    $$PWD/services/servicedispatcher.h \
    $$PWD/services/nodediscover.h \
    $$PWD/datalogger/datalogger.h \
    $$PWD/datalogger/dldata.h \
    $$PWD/profile/nodeprofilefactory.h \
    $$PWD/profile/p402/nodeprofile402.h \
    $$PWD/profile/nodeprofile.h \
    $$PWD/profile/p402/mode.h \
    $$PWD/profile/p402/modedty.h \
    $$PWD/profile/p402/modeip.h \
    $$PWD/profile/p402/modepp.h \
    $$PWD/profile/p402/modetq.h \
    $$PWD/profile/p402/modevl.h \
    $$PWD/indexdb.h \
    $$PWD/indexdb401.h \
    $$PWD/indexdb402.h \
    $$PWD/busdriver/qcanbusframe.h \
    $$PWD/busdriver/canbusdriver.h \
    $$PWD/busdriver/canbustcpudt.h \
    $$PWD/bootloader/bootloader.h \
    $$PWD/bootloader/model/ufwmodel.h \
    $$PWD/bootloader/parser/hexparser.h \
    $$PWD/bootloader/parser/ufwparser.h \
    $$PWD/bootloader/utility/hexmerger.h \
    $$PWD/bootloader/utility/phantomremover.h \
    $$PWD/bootloader/utility/ufwupdate.h \
    $$PWD/bootloader/writer/hexwriter.h \
    $$PWD/bootloader/writer/ufwwriter.h \
    $$PWD/profile/p402/modecp.h \
    $$PWD/profile/p402/modehm.h \
    $$PWD/profile/p402/modepc.h

unix:{
    SOURCES += $$PWD/busdriver/canbussocketcan.cpp
    HEADERS += $$PWD/busdriver/canbussocketcan.h
}

LIBS += -L"$$PWD/../../../bin"
android:LIBS += -lod_$${QT_ARCH}
else:LIBS += -lod

INCLUDEPATH += $$PWD/../od/
DEPENDPATH += $$PWD/../od/
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
