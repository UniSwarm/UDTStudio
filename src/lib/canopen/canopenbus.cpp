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

CanOpenBus::CanOpenBus(CanOpen *canOpen, QCanBusDevice *canDevice)
 : _canOpen(canOpen)
{
    _canDevice = nullptr;
    setCanDevice(canDevice);

    _serviceDispatcher = new ServiceDispatcher(this);
    _sync = new Sync(this);
    _timestamp = new TimeStamp(this);

    connect(_serviceDispatcher, &ServiceDispatcher::nodeFound, this, &CanOpenBus::addNodeFound);
}

CanOpenBus::~CanOpenBus()
{
    delete _sync;
    delete _timestamp;
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

const QList<Node *> &CanOpenBus::nodes() const
{
    return _nodes;
}

Node *CanOpenBus::node(uint8_t nodeId)
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

bool CanOpenBus::existNode(uint8_t nodeId)
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
void CanOpenBus::addNodeFound(uint8_t nodeId)
{
    if (existNode(nodeId) == false)
    {
        Node *nodeObject = new Node(this, nodeId);
        addNode(nodeObject);
        emit nodeAdded();
        qDebug() << "> CanOpenBus::addNodeFound" << "Add NodeID : " << nodeId;
    }
}

void CanOpenBus::addNode(Node *node)
{
    _nodes.append(node);

    Q_FOREACH (Service *service, node->services())
    {
        Q_FOREACH (quint32 cobId, service->cobIds())
        {
            _serviceDispatcher->addService(cobId, service);
        }
    }
}

void CanOpenBus::exploreBus()
{
    QCanBusFrame frameNodeGuarding;

    if (!_canDevice)
    {
        return;
    }

    for (quint8 i = 1; i <= 127; i++)
    {
        frameNodeGuarding.setFrameId(0x700 + i);
        frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
        _canDevice->writeFrame(frameNodeGuarding);
    }
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

void CanOpenBus::readObjet(uint8_t nodeId, Index &index, uint8_t subindex)
{
    //_sdos.first()->uploadData(nodeId, index, subindex);
}

void CanOpenBus::writeObjet(uint8_t nodeId, Index &index, uint8_t subindex)
{
    //_sdos.first()->downloadData(nodeId, index, subindex);
}

void CanOpenBus::dataObjetAvailable()
{
    emit objetAvailable();
}

void CanOpenBus::dataObjetWritten()
{
    emit objetWritten();
}
