/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "devicedescription.h"

DeviceDescription::DeviceDescription()
{

}

DeviceDescription::~DeviceDescription()
{

}

DeviceModel::Type DeviceDescription::type()
{
    return Description;
}

QMap<QString, QString> DeviceDescription::deviceInfos() const
{
    return _deviceInfos;
}

void DeviceDescription::setDeviceInfos(const QMap<QString, QString> &deviceInfos)
{
    _deviceInfos = deviceInfos;
}

void DeviceDescription::setDeviceInfo(const QString &key, const QString &value)
{
    _deviceInfos.insert(key, value);
}
