#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include "od_global.h"

#include "devicemodel.h"
#include "devicedescription.h"

#include <QMap>

class OD_EXPORT DeviceConfiguration : public DeviceModel
{
public:
    DeviceConfiguration();
    ~DeviceConfiguration();

    QMap<QString, QString> deviceComissionings() const;
    void setDeviceComissioning(const QString &key, const QString &value);

    static DeviceConfiguration *fromDeviceDescription(const DeviceDescription *deviceDescription, uint8_t nodeId);

private:
    QMap<QString, QString> _deviceComissionings;
};

#endif // DEVICECONFIGURATION_H
