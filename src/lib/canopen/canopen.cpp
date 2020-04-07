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

#include "canopen.h"

CanOpen *CanOpen::_instance = nullptr;

CanOpen::CanOpen()
{
}

CanOpen::~CanOpen()
{
    qDeleteAll(_buses);
}

const QList<CanOpenBus *> &CanOpen::buses()
{
    return instance()->_buses;
}

CanOpenBus *CanOpen::addBus(CanOpenBus *bus)
{
    return instance()->addBusI(bus);
}

CanOpenBus *CanOpen::bus(quint8 busId)
{
    if (busId >= instance()->_buses.count())
    {
        return nullptr;
    }
    return instance()->_buses.at(busId);
}

CanOpenBus *CanOpen::addBusI(CanOpenBus *bus)
{
    bus->_canOpen = this;
    _buses.append(bus);
    emit busChanged();
    return bus;
}
