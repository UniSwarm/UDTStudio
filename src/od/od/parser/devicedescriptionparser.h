#ifndef DEVICEDESCRIPTIONPARSER_H
#define DEVICEDESCRIPTIONPARSER_H

#include "od_global.h"

#include "model/devicedescription.h"

class OD_EXPORT DeviceDescriptionParser
{
public:
    DeviceDescriptionParser();
    virtual ~DeviceDescriptionParser();

    virtual DeviceDescription *parse(const QString &path) const = 0;
};

#endif // DEVICEDESCRIPTIONPARSER_H
