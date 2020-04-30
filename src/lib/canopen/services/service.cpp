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

#include "service.h"

#include "canopenbus.h"

Service::Service(CanOpenBus *bus)
    : _bus(bus), _node(nullptr)
{
}

Service::Service(Node *node)
    : _bus(node->bus()), _node(node)
{
}

Service::~Service()
{
}

void Service::setBus(CanOpenBus *bus)
{
    _bus = bus;
}

void Service::reset()
{
}

const QList<quint32> &Service::cobIds() const
{
    return _cobIds;
}

CanOpenBus *Service::bus() const
{
    if (_node)
    {
        return _node->bus();
    }
    return _bus;
}

QCanBusDevice *Service::canDevice() const
{
    if (_bus)
    {
        return _bus->canDevice();
    }
    if (_node)
    {
        if (_node->bus())
        {
            return _node->bus()->canDevice();
        }
    }
    return nullptr;
}

Node *Service::node() const
{
    return _node;
}
