/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

/**
 * @brief default constructor
 */
DeviceDescription::DeviceDescription()
{
}

/**
 * @brief default destructor
 */
DeviceDescription::~DeviceDescription()
{
}

/**
 * @brief returns the type of the model impletmented
 * @return device model type
 */
DeviceModel::Type DeviceDescription::type()
{
    return Description;
}

/**
 * @brief _deviceInfos getter
 * @return  a map of strings wich contains some device informations
 */
const QMap<QString, QString> &DeviceDescription::deviceInfos() const
{
    return _deviceInfos;
}

/**
 * @brief inserts a new devcie info with the key key and a value of value
 * @param key
 * @param value
 */
void DeviceDescription::setDeviceInfo(const QString &key, const QString &value)
{
    _deviceInfos.insert(key, value);
}
