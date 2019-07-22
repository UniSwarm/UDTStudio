#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include "od_global.h"

#include "devicemodel.h"

#include <QMap>

class OD_EXPORT DeviceConfiguration : public DeviceModel
{
public:
    DeviceConfiguration();
    ~DeviceConfiguration();

    QMap<QString, QString> deviceCommissioning() const;
    void setDeviceCommissioning(const QMap<QString, QString> &deviceCommissioning);

private:
    QMap<QString, QString> _deviceCommissioning;
};

#endif // DEVICECONFIGURATION_H
