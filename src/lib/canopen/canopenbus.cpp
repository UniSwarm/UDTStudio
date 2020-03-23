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

CanOpenBus::CanOpenBus(QCanBusDevice *canDevice)
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

    _emergency = new Emergency(this);
    _nmt = new NMT(this);
    _pdo = new PDO(this);
    _sdos.append(new SDO(this));
    _sync = new Sync(this);
    _timestamp = new TimeStamp(this);

    connect(_sdos.first(), SIGNAL(dataObjetAvailable()),this, SLOT(dataObjetAvailable()));
    connect(_sdos.first(), SIGNAL(dataObjetWritten()),this, SLOT(dataObjetWritten()));
    connect(_nmt, &NMT::nodeFound, this, &CanOpenBus::addNodeFound);
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
        Node *nodeObject = new Node(this);
        nodeObject->setNodeId(nodeId);
        addNode(nodeObject);
        emit nodeAdded();
    }
}

void CanOpenBus::addNode(Node *node)
{
    _nodes.append(node);
}

void CanOpenBus::exploreBus()
{
    _nmt->exploreBus();
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

//        switch (frame.frameId() >> 7)
//        {
//        case 0x0:   // NMT
//        case 0xE:   // NMT error control
//            _nmt->parseFrame(frame);
//            break;
//        case 0x1:   // SYNC/EMCY
//            break;
//        case 0x2:   // TIME
//            break;
//        case 0x3:   // PDO1tx
//        case 0x5:   // PDO2tx
//        case 0x7:   // PDO3tx
//        case 0x9:   // PDO4tx
//        case 0x4:   // PDO1rx
//        case 0x6:   // PDO2rx
//        case 0x8:   // PDO3rx
//        case 0xA:   // PDO4rx
//            _pdo->parseFrame(frame);
//            break;
//        case 0xB:   // SDOtx
//        case 0xC:   // SDOrx
//            _sdos.first()->parseFrame(frame);
//            break;
//        default:
//            qDebug()<<frame.frameId()<<frame.payload().toHex();
//            break;
//        }

        if ((frame.frameId() & 0x780) == 0x580)                         // SDO response
        {
//            _sdos.first()->parseFrame(frame);

            for (int i = 0; i < _nodes.size(); ++i)
            {
                if (_nodes.at(i)->nodeId() == (frame.frameId() & 0xFF))
                    {
                        _nodes.at(i)->_sdos->parseFrame(frame);
                    }
                //List.append(QString::number(_bus->nodes().at(i)->nodeId()));
            }

        }
        else if (frame.frameId() > 0x180 && frame.frameId() < 0x580)    // PDO receive
        {
            _pdo->parseFrame(frame);
        }
        else if (frame.frameId() > 0x700 && frame.frameId() < 0x77f) // NMT Error Control
        {
            _nmt->parseFrame(frame);
        }
        else
        {
            qDebug()<<frame.frameId()<<frame.payload().toHex();
        }


        emit frameAvailable(frame);
    }
}

void CanOpenBus::canState(QCanBusDevice::CanBusDeviceState state)
{
    emit stateCanOpenChanged(state);
}

void CanOpenBus::readObjet(uint8_t nodeId, Index &index, uint8_t subindex)
{
    _sdos.first()->uploadData(nodeId, index, subindex);
}

void CanOpenBus::writeObjet(uint8_t nodeId, Index &index, uint8_t subindex)
{
    _sdos.first()->downloadData(nodeId, index, subindex);
}

void CanOpenBus::dataObjetAvailable()
{
    emit objetAvailable();
}

void CanOpenBus::dataObjetWritten()
{
    emit objetWritten();
}
