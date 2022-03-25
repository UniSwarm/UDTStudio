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

#include "busdriver/canbusdriver.h"
#include "node.h"
#include "services/services.h"

#include <QMap>

class CanOpen;

class CANOPEN_EXPORT CanOpenBus : public QObject
{
    Q_OBJECT
public:
    CanOpenBus(CanBusDriver *canBusDriver = Q_NULLPTR);
    ~CanOpenBus();

    QString busName() const;

    CanOpen *canOpen() const;
    quint8 busId() const;

    const QList<Node *> &nodes() const;
    Node *node(quint8 nodeId);
    void addNode(Node *node);
    void removeNode(Node *node);
    bool existNode(quint8 nodeId);

    CanBusDriver *canBusDriver() const;
    void setCanBusDriver(CanBusDriver *canBusDriver);
    bool isConnected() const;
    bool canWrite() const;
    bool writeFrame(const QCanBusFrame &frame);

    const QList<QCanBusFrame> &canFramesLog() const;

    ServiceDispatcher *dispatcher() const;
    Sync *sync() const;

public slots:
    void exploreBus();
    void stopAll();
    void setBusName(const QString &busName);

signals:
    void frameAvailable(int id);

    void nodeAboutToBeAdded(int nodeId);
    void nodeAdded(int nodeId);
    void nodeAboutToBeRemoved(int nodeId);
    void nodeRemoved(int nodeId);

    void connectedChanged(bool);
    void busNameChanged(const QString &);

protected slots:
    void canFrameRec();
    void notifyForNewFrames();
    void updateState();

protected:
    friend class CanOpen;
    CanOpen *_canOpen;
    quint8 _busId;

    QString _busName;
    QMap<quint8, Node *> _nodesMap;
    QList<Node *> _nodes;
    CanBusDriver *_canBusDriver;

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

#endif  // CANOPENBUS_H
