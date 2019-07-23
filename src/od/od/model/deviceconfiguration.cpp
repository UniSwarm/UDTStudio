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

DeviceConfiguration *DeviceConfiguration::fromDeviceDescription(const DeviceDescription *deviceDescription, uint8_t nodeId)
{
    DeviceConfiguration *deviceConfiguration = new DeviceConfiguration();

    deviceConfiguration->setFileInfos(deviceDescription->fileInfos());
    QString lastName = deviceConfiguration->fileInfos().value("FileName");
    deviceConfiguration->setFileInfo("LastEDS", lastName);
    deviceConfiguration->setFileInfo("FileName", lastName.replace(".eds", ".dcf"));

    deviceConfiguration->setDeviceComissioning("NodeID", QString::number(nodeId));
    deviceConfiguration->setDeviceComissioning("NodeName", lastName.remove(".dcf"));

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
