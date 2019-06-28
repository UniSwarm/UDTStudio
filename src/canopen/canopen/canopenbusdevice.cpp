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

#include "canopenbusdevice.h"

#include <QDebug>

CanOpenBusDevice::CanOpenBusDevice(QCanBusDevice *canDevice)
    : _canDevice(canDevice)
{
    if (_canDevice)
    {
        if (_canDevice->state() == QCanBusDevice::UnconnectedState)
            _canDevice->connectDevice();
        connect(_canDevice, &QCanBusDevice::framesReceived, this, &CanOpenBusDevice::canFrameRec);
        connect(_canDevice, &QCanBusDevice::stateChanged, this, &CanOpenBusDevice::canState);
    }
}

void CanOpenBusDevice::sendSync()
{
    if (!_canDevice)
        return;

    QCanBusFrame frameSync;
    frameSync.setFrameId(0x080);
    _canDevice->writeFrame(frameSync);
}

void CanOpenBusDevice::sendNmt(uint8_t node_id, uint8_t cmd)
{
    QByteArray nmtStopPayload;
    nmtStopPayload.append((char)cmd);
    nmtStopPayload.append((char)node_id);
    QCanBusFrame frameNmt;
    frameNmt.setFrameId(0x000);
    frameNmt.setPayload(nmtStopPayload);
    _canDevice->writeFrame(frameNmt);
}

void CanOpenBusDevice::sendStart(uint8_t node_id)
{
    sendNmt(node_id, 0x01);
}

void CanOpenBusDevice::sendStop(uint8_t node_id)
{
    sendNmt(node_id, 0x02);
}

void CanOpenBusDevice::sendPDO(uint8_t nodeId, uint8_t pdoNum, QByteArray data)
{
    if (!_canDevice)
        return;

    QCanBusFrame framePDO;
    framePDO.setFrameId(0x080 + pdoNum * 0x100 + nodeId);
    framePDO.setPayload(data);
    _canDevice->writeFrame(framePDO);
}

void CanOpenBusDevice::sendSdoReadReq(uint8_t nodeId, uint16_t index, uint8_t subindex)
{
    QByteArray nmtStopPayload;
    nmtStopPayload.append(0x40);
    nmtStopPayload.append(index >> 8);
    nmtStopPayload.append(index & 0xFF);
    nmtStopPayload.append(subindex);
    nmtStopPayload.append((char)0);
    nmtStopPayload.append((char)0);
    nmtStopPayload.append((char)0);
    nmtStopPayload.append((char)0);

    QCanBusFrame frameNmt;
    frameNmt.setFrameId(0x600 + nodeId);
    frameNmt.setPayload(nmtStopPayload);
    _canDevice->writeFrame(frameNmt);
}

QCanBusDevice *CanOpenBusDevice::canDevice() const
{
    return _canDevice;
}

void CanOpenBusDevice::canFrameRec()
{
    while (_canDevice->framesAvailable() > 0)
    {
        QCanBusFrame frame = _canDevice->readFrame();
        if ((frame.frameId() & 0x780) == 0x580) // SDO response
        {
            bool error = false;
            uint8_t node_id = frame.frameId() & 0x07F;
            uint8_t size;

            uint8_t sdoCmd = frame.payload().at(0);
            if ((sdoCmd & 0xF0) == 0x40)    // SDO read req response
            {
                if (sdoCmd == 0x4F)
                    size = 8;
                else if (sdoCmd == 0x4B)
                    size = 16;
                else if (sdoCmd == 0x43)
                    size = 32;
                else
                    error = true;
                qDebug()<<frame.payload().toHex()<<node_id<<size;
            }
        }
        else if (frame.frameId() > 0x180 && frame.frameId() < 0x580)    // PDO receive
        {

        }
        else
            qDebug()<<frame.frameId()<<frame.payload().toHex();
    }
}

void CanOpenBusDevice::canState(QCanBusDevice::CanBusDeviceState state)
{

}
