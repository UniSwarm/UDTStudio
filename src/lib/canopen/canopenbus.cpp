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

#include "canopen.h"

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
    qDeleteAll(_nodes);

    if (_canDevice)
    {
        _canDevice->deleteLater();
    }
}

ServiceDispatcher *CanOpenBus::dispatcher() const
{
    return _serviceDispatcher;
}

Sync *CanOpenBus::sync() const
{
    return _sync;
}

CanOpen *CanOpenBus::canOpen() const
{
    return _canOpen;
}

quint8 CanOpenBus::busId() const
{
    if (_canOpen)
    {
        return static_cast<quint8>(_canOpen->buses().indexOf(const_cast<CanOpenBus *>(this)));
    }
    return 255;
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
    return _nodesMap.value(nodeId);
}

bool CanOpenBus::existNode(quint8 nodeId)
{
    return _nodesMap.contains(nodeId);
}

void CanOpenBus::addNode(Node *node)
{
    _nodes.append(node);
    _nodesMap.insert(node->nodeId(), node);

    node->setBus(this);
    for (Service *service : node->services())
    {
        _serviceDispatcher->addService(service);
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
