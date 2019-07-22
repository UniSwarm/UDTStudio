#ifndef DEVICEDESCRIPTION_H
#define DEVICEDESCRIPTION_H

#include "od_global.h"

#include <QMap>

#include "devicemodel.h"

class OD_EXPORT DeviceDescription : public DeviceModel
{
public:
    DeviceDescription();
    ~DeviceDescription();

    QMap<QString, QString *> deviceInfos() const;
    void setDeviceInfos(const QMap<QString, QString *> &deviceInfos);

private:
    QMap<QString, QString *> _deviceInfos;
};

#endif // DEVICEDESCRIPTION_H
