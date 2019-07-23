#include "deviceconfiguration.h"

DeviceConfiguration::DeviceConfiguration()
{

}

DeviceConfiguration::~DeviceConfiguration()
{

}

QMap<QString, QString> DeviceConfiguration::deviceComissionings() const
{
    return _deviceComissionings;
}

void DeviceConfiguration::setDeviceComissioning(const QString &key, const QString &value)
{
    _deviceComissionings.insert(key, value);
}
