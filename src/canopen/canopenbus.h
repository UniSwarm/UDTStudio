/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include <QCanBusDevice>
#include "node.h"
#include "services/services.h"

#include <QList>
#include <QMap>

class CANOPEN_EXPORT CanOpenBus : public QObject
{
    Q_OBJECT
public:
    CanOpenBus(QCanBusDevice *canDevice = Q_NULLPTR);

    const QList<Node *> &nodes() const;
    void addNode(Node *node);

    QCanBusDevice *canDevice() const;

protected slots:
    void canFrameRec();
    void canState(QCanBusDevice::CanBusDeviceState state);

public slots:
    void exploreBus();

protected:
    QList<Node *> _nodes;
    QCanBusDevice *_canDevice;

    // services
    QMap<uint32_t, Service *> _serviceId;
    Emergency *_emergency;
    NMT *_nmt;
    PDO *_pdo;
    QList<SDO *> _sdos;
    Sync *_sync;
    TimeStamp *_timestamp;
};

#endif // CANOPENBUS_H
