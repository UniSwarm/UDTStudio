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

#include "nodeobjectid.h"

NodeObjectId::NodeObjectId(quint8 bus, quint8 nodeId, quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : bus(bus), nodeId(nodeId), index(index), subIndex(subIndex), dataType(dataType)
{
}

NodeObjectId::NodeObjectId(quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : index(index), subIndex(subIndex), dataType(dataType)
{
}
