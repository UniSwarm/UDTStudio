#ifndef DEVICECONFIGURATIONPARSER_H
#define DEVICECONFIGURATIONPARSER_H

#include "od_global.h"

#include "model/deviceconfiguration.h"

class OD_EXPORT DeviceConfigurationParser
{
public:
    DeviceConfigurationParser();
    virtual ~DeviceConfigurationParser();

    virtual DeviceConfiguration *parse(const QString &path) const = 0;
};

#endif // DEVICECONFIGURATIONPARSER_H
