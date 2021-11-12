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

#ifndef DEVICEDESCRIPTIONWRITER_H
#define DEVICEDESCRIPTIONWRITER_H

#include "od_global.h"

#include "model/devicedescription.h"

class OD_EXPORT DeviceDescriptionWriter
{
public:
    DeviceDescriptionWriter();
    virtual ~DeviceDescriptionWriter();

    virtual void write(const DeviceDescription *deviceDescription, const QString &filePath) const = 0;
};

#endif  // DEVICEDESCRIPTIONWRITER_H
