#ifndef DEVICECONFIGURATIONWRITER_H
#define DEVICECONFIGURATIONWRITER_H

#include "od_global.h"

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT DeviceConfigurationWriter
{
public:
    DeviceConfigurationWriter();
    virtual ~DeviceConfigurationWriter();

    virtual void write(const DeviceConfiguration *deviceConfiguration, const QString &dir) const = 0;
    virtual void write(const DeviceDescription *deviceDescription, const QString &dir, uint8_t nodeId) const = 0;
};

#endif // DEVICECONFIGURATIONWRITER_H
