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

#ifndef CANOPENBUS_H
#define CANOPENBUS_H

#include "canopen_global.h"

#include "node.h"
#include "services/services.h"

#include <QList>
#include <QCanBusDevice>

class CanOpen;

class CANOPEN_EXPORT CanOpenBus : public QObject
{
    Q_OBJECT
public:
    CanOpenBus(QCanBusDevice *canDevice = Q_NULLPTR);
    ~CanOpenBus();

    CanOpen *canOpen() const;
    ServiceDispatcher *dispatcher() const;

    QString busName() const;
    void setBusName(const QString &busName);

    const QList<Node *> &nodes() const;
    Node *node(quint8 nodeId);
    void addNode(Node *node);
    bool existNode(quint8 nodeId);

    QCanBusDevice *canDevice() const;
    void setCanDevice(QCanBusDevice *canDevice);

protected slots:
    void canFrameRec();
    void canState(QCanBusDevice::CanBusDeviceState state);

public slots:
    void exploreBus();
    void dataObjetAvailable();
    void dataObjetWritten();

signals:
    void frameAvailable(QCanBusFrame frame);
    void frameErrorOccurred(QCanBusDevice::CanBusError error);
    void frameTransmit(qint64 framesCount);
    void stateCanOpenChanged(QCanBusDevice::CanBusDeviceState state);

    void objetAvailable();
    void objetWritten();
    void nodeAdded();

protected:
    friend class CanOpen;
    CanOpen *_canOpen;
    QString _busName;
    QList<Node *> _nodes;
    QCanBusDevice *_canDevice;

    // services
    ServiceDispatcher *_serviceDispatcher;
    Sync *_sync;
    TimeStamp *_timestamp;

private slots:
    void addNodeFound(quint8 nodeId);
};

#endif // CANOPENBUS_H
