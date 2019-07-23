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

void DeviceConfiguration::setNodeId(const QString &nodeName)
{
    _deviceComissionings.insert("NodeID", nodeName);
}

void DeviceConfiguration::setNodeName(const QString &nodeName)
{
    _deviceComissionings.insert("NodeName", nodeName);
}

void DeviceConfiguration::setBaudrate(const QString &baudrate)
{
    _deviceComissionings.insert("Baudrate", baudrate);
}

void DeviceConfiguration::setNetNumber(const QString &netNumber)
{
    _deviceComissionings.insert("NetNumber", netNumber);
}

void DeviceConfiguration::setNetworkName(const QString &networkName)
{
    _deviceComissionings.insert("NetworkName", networkName);
}

void DeviceConfiguration::setLssSerialNumber(const QString &lssSerialNumber)
{
    _deviceComissionings.insert("LssSerialNumber", lssSerialNumber);
}

DeviceConfiguration *DeviceConfiguration::fromDeviceDescription(const DeviceDescription *deviceDescription, uint8_t nodeId)
{
    DeviceConfiguration *deviceConfiguration = new DeviceConfiguration();

    deviceConfiguration->setFileInfos(deviceDescription->fileInfos());
    QString lastName = deviceConfiguration->fileInfos().value("FileName");
    deviceConfiguration->setFileInfo("LastEDS", lastName);
    deviceConfiguration->setFileInfo("FileName", lastName.replace(".eds", ".dcf"));

    deviceConfiguration->setNodeId(QString::number(nodeId));
    deviceConfiguration->setDummyUsages(deviceDescription->dummyUsages());

    deviceConfiguration->setIndexes(deviceDescription->indexes());
    foreach (Index *index, deviceConfiguration->indexes())
    {
        foreach (SubIndex *subIndex, index->subIndexes())
        {
            if (subIndex->hasNodeId())
            {
                QString value = subIndex->data().value().toString();

                uint8_t base = 10;
                if (value.startsWith("0x"))
                    base = 16;

                bool ok;
                subIndex->data().setValue(value.toUInt(&ok, base) + nodeId);
            }
        }
    }

    return deviceConfiguration;
}
