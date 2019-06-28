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

#ifndef CANOPENBUSDEVICE_H
#define CANOPENBUSDEVICE_H

#include "canopen_global.h"

#include <QCanBusDevice>

class CANOPEN_EXPORT CanOpenBusDevice : public QObject
{
    Q_OBJECT
public:
    CanOpenBusDevice(QCanBusDevice *canDevice);

    void sendSync();
    void sendNmt(uint8_t node_id, uint8_t cmd);
    void sendStart(uint8_t node_id);
    void sendStop(uint8_t node_id);

    void sendPDO(uint8_t nodeId, uint8_t pdoNum, QByteArray data);

    void sendSdoReadReq(uint8_t nodeId, uint16_t index, uint8_t subindex);

    QCanBusDevice *canDevice() const;

protected slots:
    void canFrameRec();
    void canState(QCanBusDevice::CanBusDeviceState state);

signals:
    void sdoReceived();

protected:
    QCanBusDevice *_canDevice;
};

#endif // CANOPENBUSDEVICE_H
