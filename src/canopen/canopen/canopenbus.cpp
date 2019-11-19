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

#include "canopenbus.h"

#include <QDebug>

CanOpenBus::CanOpenBus(QCanBusDevice *canDevice)
{
    _canDevice = canDevice;
    if (_canDevice)
    {
        if (_canDevice->state() == QCanBusDevice::UnconnectedState)
            _canDevice->connectDevice();
        connect(_canDevice, &QCanBusDevice::framesReceived, this, &CanOpenBus::canFrameRec);
        connect(_canDevice, &QCanBusDevice::stateChanged, this, &CanOpenBus::canState);
    }

    _emergency = new Emergency(this);
    _nmt = new NMT(this);
    _pdo = new PDO(this);
    _sdos.append(new SDO(this));
    _sync = new Sync(this);
    _timestamp = new TimeStamp(this);
}

const QList<Node *> &CanOpenBus::nodes() const
{
    return _nodes;
}

void CanOpenBus::addNode(Node *node)
{
    _nodes.append(node);
}

void CanOpenBus::exploreBus()
{
    //_canDevice->sendSdoReadReq(2, 0x1000, 0);
    _sdos.first()->sendSdoReadReq(2, 0x1000, 0);
}

QCanBusDevice *CanOpenBus::canDevice() const
{
    return _canDevice;
}

void CanOpenBus::canFrameRec()
{
    while (_canDevice->framesAvailable() > 0)
    {
        QCanBusFrame frame = _canDevice->readFrame();
        if ((frame.frameId() & 0x780) == 0x580) // SDO response
        {
        }
        else if (frame.frameId() > 0x180 && frame.frameId() < 0x580)    // PDO receive
        {

        }
        else
            qDebug()<<frame.frameId()<<frame.payload().toHex();
    }
}

void CanOpenBus::canState(QCanBusDevice::CanBusDeviceState state)
{

}
