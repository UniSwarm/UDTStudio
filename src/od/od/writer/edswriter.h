#ifndef EDSWRITER_H
#define EDSWRITER_H

#include "od_global.h"

#include "model/devicedescription.h"

class OD_EXPORT EdsWriter : public DeviceDescription
{
public:
    EdsWriter();
    ~EdsWriter();

    void write(const DeviceDescription *deviceDescription, const QString &filePath) const;
};

#endif // EDSWRITER_H
