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

#ifndef EDSPARSER_H
#define EDSPARSER_H

#include "od_global.h"

#include "devicedescriptionparser.h"

class OD_EXPORT EdsParser : public DeviceDescriptionParser
{
public:
    EdsParser();
    ~EdsParser();

    DeviceDescription *parse(const QString &path) const;
};

#endif // EDSPARSER_H
