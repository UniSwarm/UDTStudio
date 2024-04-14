/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef DEVICECONFIGURATION_H
#define DEVICECONFIGURATION_H

#include "od_global.h"

#include "devicedescription.h"
#include "devicemodel.h"

#include <QMap>

class OD_EXPORT DeviceConfiguration : public DeviceModel
{
public:
    DeviceConfiguration();
    ~DeviceConfiguration() override;

    Type type() override;

    const QMap<QString, QString> &deviceComissionings() const;

    void addDeviceComissioning(const QString &key, const QString &value);
    QString nodeId() const;
    void setNodeId(const QString &nodeId);
    void setNodeName(const QString &nodeName);
    void setBaudrate(const QString &baudrate);
    void setNetNumber(const QString &netNumber);
    void setNetworkName(const QString &networkName);
    void setLssSerialNumber(const QString &lssSerialNumber);

    static DeviceConfiguration *fromDeviceDescription(const DeviceDescription *deviceDescription, uint8_t nodeId);

private:
    QMap<QString, QString> _deviceComissionings;
};

#endif  // DEVICECONFIGURATION_H
