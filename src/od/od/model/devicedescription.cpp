#include "devicedescription.h"

DeviceDescription::DeviceDescription()
{

}

DeviceDescription::~DeviceDescription()
{

}

QMap<QString, QString> DeviceDescription::deviceInfos() const
{
    return _deviceInfos;
}

void DeviceDescription::setDeviceInfos(const QMap<QString, QString> &deviceInfos)
{
    _deviceInfos = deviceInfos;
}
