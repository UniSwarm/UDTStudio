/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "deviceconfiguration.h"

/**
 * @brief default constructor
 */
DeviceConfiguration::DeviceConfiguration()
{
}

/**
 * @brief default destructor
 */
DeviceConfiguration::~DeviceConfiguration()
{
}

/**
 * @brief give the type of the model wich is implemented
 * @return device model type
 */
DeviceModel::Type DeviceConfiguration::type()
{
    return Configuration;
}

/**
 * @brief _deviceCommissionings getter
 * @return map of string wich contains device comissionings
 */
const QMap<QString, QString> &DeviceConfiguration::deviceComissionings() const
{
    return _deviceComissionings;
}

/**
 * @brief inserts a new device comissioning with the key key and a value of value
 * if there is already an item with the key key, that item's value is replaced with value
 * @param key
 * @param value
 */
void DeviceConfiguration::addDeviceComissioning(const QString &key, const QString &value)
{
    _deviceComissionings.insert(key, value);
}

QString DeviceConfiguration::nodeId() const
{
    return _deviceComissionings.value("NodeID");
}

/**
 * @brief sets a new CANopen node-id
 * @param nodeId
 */
void DeviceConfiguration::setNodeId(const QString &nodeId)
{
    _deviceComissionings.insert("NodeID", nodeId);
}

/**
 * @brief sets a new node name
 * @param nodeName
 */
void DeviceConfiguration::setNodeName(const QString &nodeName)
{
    _deviceComissionings.insert("NodeName", nodeName);
}

/**
 * @brief sets a new communication baudrate
 * @param baudrate
 */
void DeviceConfiguration::setBaudrate(const QString &baudrate)
{
    _deviceComissionings.insert("Baudrate", baudrate);
}

/**
 * @brief sets a new network number
 * @param network number
 */
void DeviceConfiguration::setNetNumber(const QString &netNumber)
{
    _deviceComissionings.insert("NetNumber", netNumber);
}

/**
 * @brief sets a new network name
 * @param networkName
 */
void DeviceConfiguration::setNetworkName(const QString &networkName)
{
    _deviceComissionings.insert("NetworkName", networkName);
}

/**
 * @brief sets a new lss serila number
 * @param lssSerialNumber
 */
void DeviceConfiguration::setLssSerialNumber(const QString &lssSerialNumber)
{
    _deviceComissionings.insert("LssSerialNumber", lssSerialNumber);
}

/**
 * @brief converts a device descrtiption model to a device configuration model
 * @param device description model
 * @param node id to add in the configuration model
 * @return device configuration model
 */
DeviceConfiguration *DeviceConfiguration::fromDeviceDescription(const DeviceDescription *deviceDescription, uint8_t nodeId)
{
    DeviceConfiguration *deviceConfiguration = new DeviceConfiguration();

    deviceConfiguration->setFileInfos(deviceDescription->fileInfos());
    QString lastName = deviceConfiguration->fileInfos().value("FileName");
    deviceConfiguration->setFileInfo("LastEDS", lastName);
    deviceConfiguration->setFileInfo("FileName", lastName.replace(".eds", ".dcf"));

    deviceConfiguration->setNodeId(QString::number(nodeId));
    deviceConfiguration->setDummyUsages(deviceDescription->dummyUsages());

    for (Index *index: deviceDescription->indexes())
    {
        deviceConfiguration->addIndex(new Index(*index));
    }

    for (Index *index : deviceConfiguration->indexes())
    {
        for (SubIndex *subIndex : index->subIndexes())
        {
            if (subIndex->hasNodeId())
            {
                QString value = subIndex->value().toString();

                uint8_t base = 10;
                if (value.startsWith("0x"))
                {
                    base = 16;
                }

                bool ok;
                subIndex->setValue(value.toUInt(&ok, base) + nodeId);
            }
        }
    }

    return deviceConfiguration;
}
