#ifndef DEVICECONFIGURATIONWRITER_H
#define DEVICECONFIGURATIONWRITER_H

#include "od_global.h"

#include "model/deviceconfiguration.h"

class OD_EXPORT DeviceConfigurationWriter
{
public:
    DeviceConfigurationWriter();
    virtual ~DeviceConfigurationWriter();

    virtual void write(const DeviceConfiguration *od, const QString &dir) const = 0;
};

#endif // DEVICECONFIGURATIONWRITER_H
