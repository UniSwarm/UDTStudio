#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include "od_global.h"

#include "devicemodel.h"
#include "devicedescription.h"

#include <QMap>

class OD_EXPORT DeviceConfiguration : public DeviceModel
{
public:
    DeviceConfiguration();
    ~DeviceConfiguration();

    Type type();

    QMap<QString, QString> deviceComissionings() const;

    void setDeviceComissioning(const QString &key, const QString &value);
    void setNodeId(const QString &nodeName);
    void setNodeName(const QString &nodeName);
    void setBaudrate(const QString &baudrate);
    void setNetNumber(const QString &netNumber);
    void setNetworkName(const QString &networkName);
    void setLssSerialNumber(const QString &lssSerialNumber);

    static DeviceConfiguration *fromDeviceDescription(const DeviceDescription *deviceDescription, uint8_t nodeId);

private:
    QMap<QString, QString> _deviceComissionings;
};

#endif // DEVICECONFIGURATION_H
