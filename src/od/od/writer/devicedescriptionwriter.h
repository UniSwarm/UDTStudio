#ifndef DEVICEDESCRIPTIONWRITER_H
#define DEVICEDESCRIPTIONWRITER_H

#include "od_global.h"

#include "model/devicedescription.h"

class OD_EXPORT DeviceDescriptionWriter
{
public:
    DeviceDescriptionWriter();
    virtual ~DeviceDescriptionWriter();

    virtual void write(const DeviceDescription *deviceDescription, const QString &filePath) const = 0;
};

#endif // DEVICEDESCRIPTIONWRITER_H
