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

#include "servicedispatcher.h"

#include <QDebug>

ServiceDispatcher::ServiceDispatcher(CanOpenBus *bus)
    : Service (bus)
{
}

ServiceDispatcher::~ServiceDispatcher()
{
}

QString ServiceDispatcher::type() const
{
    return QLatin1String("Emergency");
}

void ServiceDispatcher::addService(uint32_t canId, Service *service)
{
    _servicesMap.insert(canId, service);
}

void ServiceDispatcher::parseFrame(const QCanBusFrame &frame)
{
    QMultiMap<quint32, Service *>::iterator i = _servicesMap.find(frame.frameId());
    qDebug() << "> ServiceDispatcher::parseFrame" << QString::number(frame.frameId(), 16) << frame.payload().toHex(' ').toUpper();
    while (i != _servicesMap.end())
    {
        Service *service = i.value();
        qDebug() << service->type();
        service->parseFrame(frame);
        ++i;
    }
}
