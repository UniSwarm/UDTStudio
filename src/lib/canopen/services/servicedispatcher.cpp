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

#include "servicedispatcher.h"

#include "canopen.h"
#include "node.h"
#include <QDebug>

ServiceDispatcher::ServiceDispatcher(CanOpenBus *bus)
    : Service(bus)
{
}

ServiceDispatcher::~ServiceDispatcher()
{
}

QString ServiceDispatcher::type() const
{
    return QStringLiteral("Emergency");
}

void ServiceDispatcher::addService(Service *service)
{
    for (quint32 cobId : service->cobIds())
    {
        _servicesMap.insert(cobId, service);
    }
}

void ServiceDispatcher::removeService(Service *service)
{
    for (quint32 cobId : service->cobIds())
    {
        _servicesMap.remove(cobId, service);
    }
}

void ServiceDispatcher::parseFrame(const QCanBusFrame &frame)
{
    QList<Service *> interrestedServices = _servicesMap.values(frame.frameId());
    QList<Service *>::const_iterator service = interrestedServices.cbegin();
    while (service != interrestedServices.cend())
    {
        (*service)->parseFrame(frame);
        ++service;
    }
}
