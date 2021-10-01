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

void CanOpen::removeBus(CanOpenBus *bus)
{
    instance()->removeBusI(bus);
}

CanOpenBus *CanOpen::bus(const quint8 busId)
{
    QMap<quint8, CanOpenBus *>::const_iterator it = instance()->_busesMap.constFind(busId);
    if (it != instance()->_busesMap.cend())
    {
        return *it;
    }
    return nullptr;
}

void CanOpen::stopAll()
{
    for (CanOpenBus *bus : qAsConst(_buses))
    {
        if (bus->isConnected())
        {
            bus->stopAll();
        }
    }
}

CanOpenBus *CanOpen::addBusI(CanOpenBus *bus)
{
    emit busAboutToBeAdded(bus->busId());
    bus->_canOpen = this;
    bus->_busId = findNewBusId();
    _buses.append(bus);
    _busesMap.insert(bus->busId(), bus);
    emit busAdded(bus->busId());
    return bus;
}

void CanOpen::removeBusI(CanOpenBus *bus)
{
    emit busAboutToBeRemoved(bus->busId());
    _buses.removeOne(bus);
    _busesMap.remove(bus->busId());
    emit busRemoved(bus->busId());
}

quint8 CanOpen::findNewBusId() const
{
    for (quint8 busId = 0; busId < 255; busId++)
    {
        if (!_busesMap.contains(busId))
        {
            return busId;
        }
    }
    return 255;
}
