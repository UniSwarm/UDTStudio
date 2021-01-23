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

#ifndef DEVICECONFIGURATIONPARSER_H
#define DEVICECONFIGURATIONPARSER_H

#include "od_global.h"

#include "model/deviceconfiguration.h"

class OD_EXPORT DeviceConfigurationParser
{
public:
    DeviceConfigurationParser();
    virtual ~DeviceConfigurationParser();

    virtual DeviceConfiguration *parse(const QString &path) const = 0;
};

#endif // DEVICECONFIGURATIONPARSER_H
