#ifndef EDSPARSER_H
#define EDSPARSER_H

#include "od_global.h"

#include "devicedescriptionparser.h"

class OD_EXPORT EdsParser : public DeviceDescriptionParser
{
public:
    EdsParser();
    ~EdsParser();

    DeviceDescription *parse(const QString &path) const;
};

#endif // EDSPARSER_H
