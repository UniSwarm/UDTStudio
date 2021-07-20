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

#include "canopenbus.h"

#include <QDebug>

#include "canopen.h"

CanOpenBus::CanOpenBus(CanBusDriver *canBusDriver)
{
    _canOpen = nullptr;
    _canBusDriver = nullptr;
    setCanBusDriver(canBusDriver);
    _spyMode = false;

    // services
    _serviceDispatcher = new ServiceDispatcher(this);

    _sync = new Sync(this);
    _serviceDispatcher->addService(_sync);

    _timestamp = new TimeStamp(this);
    _serviceDispatcher->addService(_timestamp);

    _nodeDiscover = new NodeDiscover(this);
    _serviceDispatcher->addService(_nodeDiscover);

    // can frame logger
    _canFrameLogId = 0;
    _canFramesLogTimer = new QTimer();
    connect(_canFramesLogTimer, &QTimer::timeout, this, &CanOpenBus::notifyForNewFrames);
    _canFramesLogTimer->start(100);
}

CanOpenBus::~CanOpenBus()
{
    delete _sync;
    delete _timestamp;
    delete _nodeDiscover;
    delete _serviceDispatcher;
    qDeleteAll(_nodes);

    if (_canBusDriver)
    {
        _canBusDriver->deleteLater();
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
        return static_cast<quint8>(CanOpen::buses().indexOf(const_cast<CanOpenBus *>(this)));
    }
    return 255;
}

QString CanOpenBus::busName() const
{
    return _busName;
}

void CanOpenBus::setBusName(const QString &busName)
{
    bool changed = (_busName != busName);
    _busName = busName;
    if (changed)
    {
        emit busNameChanged(_busName);
    }
}

const QList<Node *> &CanOpenBus::nodes() const
{
    return _nodes;
}

Node *CanOpenBus::node(const quint8 nodeId)
{
    return _nodesMap.value(nodeId);
}

bool CanOpenBus::existNode(const quint8 nodeId)
{
    return _nodesMap.contains(nodeId);
}

void CanOpenBus::addNode(Node *node)
{
    _nodes.append(node);
    _nodesMap.insert(node->nodeId(), node);

    node->setBus(this);
    QListIterator<Service *> service(node->services());
    while (service.hasNext())
    {
        _serviceDispatcher->addService(service.next());
    }
    emit nodeAdded();
}

void CanOpenBus::exploreBus()
{
    _nodeDiscover->exploreBus();
}

const QList<QCanBusFrame> &CanOpenBus::canFramesLog() const
{
    return _canFramesLog;
}

CanBusDriver *CanOpenBus::canBusDriver() const
{
    return _canBusDriver;
}

void CanOpenBus::setCanBusDriver(CanBusDriver *canBusDriver)
{
    if (_canBusDriver)
    {
        _canBusDriver->deleteLater();
    }

    _canBusDriver = canBusDriver;
    if (_canBusDriver)
    {
        if (_canBusDriver->state() == CanBusDriver::DISCONNECTED)
        {
            _canBusDriver->connectDevice();
        }
        connect(_canBusDriver, &CanBusDriver::framesReceived, this, &CanOpenBus::canFrameRec);
        connect(_canBusDriver, &CanBusDriver::stateChanged, this, &CanOpenBus::updateState);
    }
}

bool CanOpenBus::isConnected() const
{
    if (!_canBusDriver)
    {
        return false;
    }
    return (_canBusDriver->state() == CanBusDriver::CONNECTED);
}

bool CanOpenBus::canWrite() const
{
    if (!_canBusDriver || _spyMode)
    {
        return false;
    }
    return true;
}

bool CanOpenBus::writeFrame(const QCanBusFrame &frame)
{
    if (!canWrite())
    {
        return false;
    }
    _canBusDriver->writeFrame(frame);
    QCanBusFrame emitFrame = frame;
    emitFrame.setTimeStamp(QCanBusFrame::TimeStamp::fromMicroSeconds(QDateTime::currentMSecsSinceEpoch() * 1000));
    emitFrame.setLocalEcho(true);
    _canFramesLog.append(emitFrame);
    return true;
}

void CanOpenBus::canFrameRec()
{
    if (!_canBusDriver)
    {
        return;
    }
    QCanBusFrame frame = _canBusDriver->readFrame();
    while (frame.isValid())
    {
        frame.setTimeStamp(QCanBusFrame::TimeStamp::fromMicroSeconds(QDateTime::currentMSecsSinceEpoch() * 1000));
        _serviceDispatcher->parseFrame(frame);
        _canFramesLog.append(frame);

        frame = _canBusDriver->readFrame();
    }
}

void CanOpenBus::notifyForNewFrames()
{
    if (_canFrameLogId < _canFramesLog.count())
    {
        _canFrameLogId = _canFramesLog.count();
        emit frameAvailable(_canFrameLogId);
    }
}

void CanOpenBus::updateState()
{
    emit connectedChanged(isConnected());
}
