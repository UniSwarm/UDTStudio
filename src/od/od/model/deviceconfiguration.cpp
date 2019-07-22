#include "deviceconfiguration.h"

DeviceConfiguration::DeviceConfiguration()
{

}

DeviceConfiguration::~DeviceConfiguration()
{
    qDeleteAll(_deviceCommissioning);
    _deviceCommissioning.clear();
}

QMap<QString, QString *> DeviceConfiguration::deviceCommissioning() const
{
    return _deviceCommissioning;
}

void DeviceConfiguration::setDeviceCommissioning(const QMap<QString, QString *> &deviceCommissioning)
{
    _deviceCommissioning = deviceCommissioning;
}
