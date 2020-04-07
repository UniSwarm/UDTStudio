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

#include "dldata.h"

#include "canopen.h"

DLData::DLData(const NodeObjectId &objectId)
    :_objectId(objectId)
{
    _node = nullptr;
    CanOpenBus *bus = CanOpen::bus(objectId.busId);
    if (bus)
    {
        _node = bus->node(objectId.busId);
    }
}

const NodeObjectId &DLData::objectId() const
{
    return _objectId;
}

quint64 DLData::key() const
{
    return _objectId.key();
}

Node *DLData::node() const
{
    return _node;
}
