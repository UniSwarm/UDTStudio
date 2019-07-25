#ifndef DEVICEINIPARSER_H
#define DEVICEINIPARSER_H

#include "od_global.h"

#include <QTextStream>
#include <QSettings>

#include "model/devicedescription.h"
#include "model/deviceconfiguration.h"

class DeviceIniParser
{
public:
    DeviceIniParser(QSettings *file);

    DataStorage readData(bool *nodeId) const;
    void readFileInfo(DeviceModel *od) const;
    void readDummyUsage(DeviceModel *od) const;
    void readDeviceInfo(DeviceDescription *od) const;
    void readDeviceComissioning(DeviceConfiguration *od) const;
    uint8_t readPdoMapping() const;
    QVariant readLowLimit() const;
    QVariant readHighLimit() const;

    QSettings *_file;
};

#endif // DEVICEINIPARSER_H
