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

#include <QCanBusDevice>
#include "node.h"
#include "services/services.h"
#include "model/deviceconfiguration.h"

#include <QList>
#include <QMap>

class CANOPEN_EXPORT CanOpenBus : public QObject
{
    Q_OBJECT
public:
    CanOpenBus(QCanBusDevice *canDevice = Q_NULLPTR);

    const QList<Node *> &nodes() const;
    Node *node(uint8_t nodeId);
    void addNode(Node *node);
    bool existNode(uint8_t nodeId);

    void readObjet(uint8_t nodeId, Index &index, uint8_t subindex);
    void writeObjet(uint8_t nodeId, Index &index, uint8_t subindex);
    QCanBusDevice *canDevice() const;

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

public:
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

  private:

    void addNodeFound(uint8_t nodeId);
};

#endif // CANOPENBUS_H
