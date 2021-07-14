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

#ifndef CANOPENBUS_H
#define CANOPENBUS_H

#include "canopen_global.h"

#include <QObject>

#include "node.h"
#include "services/services.h"

#include <QCanBusDevice>
#include <QMap>

class CanOpen;

class CANOPEN_EXPORT CanOpenBus : public QObject
{
    Q_OBJECT
public:
    CanOpenBus(QCanBusDevice *canDevice = Q_NULLPTR);
    ~CanOpenBus();

    CanOpen *canOpen() const;
    quint8 busId() const;
    ServiceDispatcher *dispatcher() const;

    Sync *sync() const;

    QString busName() const;
    void setBusName(const QString &busName);

    bool isConnected() const;

    const QList<Node *> &nodes() const;
    Node *node(const quint8 nodeId);
    void addNode(Node *node);
    bool existNode(const quint8 nodeId);

    QCanBusDevice *canDevice() const;
    void setCanDevice(QCanBusDevice *canDevice);
    bool canWrite() const;
    bool writeFrame(const QCanBusFrame &frame);

    const QList<QCanBusFrame> &canFramesLog() const;

protected slots:
    void canFrameRec();
    void canState(QCanBusDevice::CanBusDeviceState state);
    void notifyForNewFrames();

public slots:
    void exploreBus();

signals:
    void frameAvailable(int id);
    void frameErrorOccurred(QCanBusDevice::CanBusError error);
    void stateCanOpenChanged(QCanBusDevice::CanBusDeviceState state);

    void nodeAdded();

protected:
    friend class CanOpen;
    CanOpen *_canOpen;

    QString _busName;
    QMap<quint8, Node *> _nodesMap;
    QList<Node *> _nodes;
    QCanBusDevice *_canDevice;

    // CAN frames logger
    QList<QCanBusFrame> _canFramesLog;
    int _canFrameLogId;
    QTimer *_canFramesLogTimer;

    // services
    ServiceDispatcher *_serviceDispatcher;
    NodeDiscover *_nodeDiscover;
    Sync *_sync;
    TimeStamp *_timestamp;

    // spy mode
    bool _spyMode;
};

#endif // CANOPENBUS_H
