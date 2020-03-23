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

#include "node.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"
#include "services/services.h"

Node::Node(CanOpenBus *bus)
    : _bus(bus)
{

    _emergency = new Emergency(_bus);
    _nmt = new NMT(_bus);
    _rpdo.append(new RPDO(_bus));
    _tpdo.append(new TPDO(_bus));
    _sdoClient.append(new SDO(_bus));
}

void Node::parseFrame(const QCanBusFrame &frame)
{
            switch (frame.frameId() >> 7)
            {
            case 0x0:   // NMT
            case 0xE:   // NMT error control
                //_nmt->parseFrame(frame);
                break;
            case 0x1:   // SYNC/EMCY
                break;
            case 0x2:   // TIME
                break;
            case 0x3:   // PDO1tx
            case 0x5:   // PDO2tx
            case 0x7:   // PDO3tx
            case 0x9:   // PDO4tx
            case 0x4:   // PDO1rx
            case 0x6:   // PDO2rx
            case 0x8:   // PDO3rx
            case 0xA:   // PDO4rx
                //_pdo->parseFrame(frame);
                break;
            case 0xB:   // SDOtx
            case 0xC:   // SDOrx
                _sdoClient.at(0)->parseFrame(frame);
                break;
            default:
                //qDebug()<<frame.frameId()<<frame.payload().toHex();
                break;
            }


}

uint32_t Node::nodeId() const
{
    return _nodeId;
}

void Node::setNodeId(const uint32_t &nodeId)
{
    _nodeId = nodeId;
}

void Node::addEds(QString fileName)
{
    EdsParser parser;

    DeviceDescription *deviceDescription = parser.parse(fileName);
    deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, static_cast<uint8_t>(_nodeId));
}

void Node::updateFirmware(const QByteArray &prog)
{
    uint16_t index = 0x1F50;
    uint8_t subindex = 0x01;
    Index *index2;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        index2->subIndex(subindex)->setValue(prog);
        _sdoClient.at(0)->downloadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
}

QString Node::device()
{
    uint16_t index = 0x1000;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdoClient.at(0)->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}

QString Node::manuDeviceName()
{
    uint16_t index = 0x1008;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdoClient.at(0)->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
QString Node::manufacturerHardwareVersion()
{
    uint16_t index = 0x1009;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdoClient.at(0)->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
QString Node::manufacturerSoftwareVersion()
{
    uint16_t index = 0x100a;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdoClient.at(0)->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
