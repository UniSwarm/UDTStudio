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

#include "canopenbus.h"

#include <QDebug>

CanOpenBus::CanOpenBus(QCanBusDevice *canDevice)
{
    _canOpen = nullptr;
    _canDevice = nullptr;
    setCanDevice(canDevice);

    // services
    _serviceDispatcher = new ServiceDispatcher(this);

    _sync = new Sync(this);
    _serviceDispatcher->addService(_sync);

    _timestamp = new TimeStamp(this);
    _serviceDispatcher->addService(_timestamp);

    _nodeDiscover = new NodeDiscover(this);
    _serviceDispatcher->addService(_nodeDiscover);
}

CanOpenBus::~CanOpenBus()
{
    delete _sync;
    delete _timestamp;
    delete _nodeDiscover;
    delete _serviceDispatcher;

    if (_canDevice)
    {
        _canDevice->deleteLater();
    }
}

ServiceDispatcher *CanOpenBus::dispatcher() const
{
    return _serviceDispatcher;
}

CanOpen *CanOpenBus::canOpen() const
{
    return _canOpen;
}

QString CanOpenBus::busName() const
{
    return _busName;
}

void CanOpenBus::setBusName(const QString &busName)
{
    _busName = busName;
}

bool CanOpenBus::isConnected() const
{
    return (_canDevice->state() == QCanBusDevice::ConnectedState);
}

const QList<Node *> &CanOpenBus::nodes() const
{
    return _nodes;
}

Node *CanOpenBus::node(quint8 nodeId)
{
    for (int i = 0; i < _nodes.size(); i++)
    {
        if (_nodes.at(i)->nodeId() == nodeId)
        {
            return _nodes.at(i);
        }
    }
    return nullptr;
}

bool CanOpenBus::existNode(quint8 nodeId)
{
    for (int i = 0; i < _nodes.size(); i++)
    {
        if (_nodes.at(i)->nodeId() == nodeId)
        {
            return true;
        }
    }
    return false;
}

void CanOpenBus::addNode(Node *node)
{
    node->_bus = this;
    _nodes.append(node);

    Q_FOREACH (Service *service, node->services())
    {
        Q_FOREACH (quint32 cobId, service->cobIds())
        {
            _serviceDispatcher->addService(cobId, service);
        }
    }
    emit nodeAdded();
}

void CanOpenBus::exploreBus()
{
    _nodeDiscover->exploreBus();
}

QCanBusDevice *CanOpenBus::canDevice() const
{
    return _canDevice;
}

void CanOpenBus::setCanDevice(QCanBusDevice *canDevice)
{
    _canDevice = canDevice;
    if (_canDevice)
    {
        if (_canDevice->state() == QCanBusDevice::UnconnectedState)
        {
            _canDevice->connectDevice();
        }
        connect(_canDevice, &QCanBusDevice::framesReceived, this, &CanOpenBus::canFrameRec);
        connect(_canDevice, &QCanBusDevice::errorOccurred, this, &CanOpenBus::frameErrorOccurred);
        connect(_canDevice, &QCanBusDevice::framesWritten, this, &CanOpenBus::frameTransmit);
        connect(_canDevice, &QCanBusDevice::stateChanged, this, &CanOpenBus::canState);
    }
}

void CanOpenBus::canFrameRec()
{
    while (_canDevice->framesAvailable() > 0)
    {
        const QCanBusFrame &frame = _canDevice->readFrame();
        _serviceDispatcher->parseFrame(frame);
        emit frameAvailable(frame);
    }
}

void CanOpenBus::canState(QCanBusDevice::CanBusDeviceState state)
{
    emit stateCanOpenChanged(state);
}
