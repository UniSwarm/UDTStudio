#ifndef DCFPARSER_H
#define DCFPARSER_H

#include "od_global.h"

#include "deviceconfigurationparser.h"

class OD_EXPORT DcfParser : public DeviceConfigurationParser
{
public:
    DcfParser();
    ~DcfParser();

    DeviceConfiguration *parse(const QString &path) const;
};

#endif // DCFPARSER_H
