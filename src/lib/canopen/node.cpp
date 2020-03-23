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

    _sdos = new SDO(_bus);
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
        _sdos->downloadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
}

QString Node::device()
{
    uint16_t index = 0x1000;
    uint8_t subindex = 0x00;
    Index *index2;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdos->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}

QString Node::manuDeviceName()
{
    uint16_t index = 0x1008;
    uint8_t subindex = 0x00;
    Index *index2;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdos->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
QString Node::manufacturerHardwareVersion()
{
    uint16_t index = 0x1009;
    uint8_t subindex = 0x00;
    Index *index2;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdos->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
QString Node::manufacturerSoftwareVersion()
{
    uint16_t index = 0x100a;
    uint8_t subindex = 0x00;
    Index *index2;

    if (deviceConfiguration->indexExist(index))
    {
        index2 = deviceConfiguration->index(index);
        _sdos->uploadData(static_cast<uint8_t>(_nodeId), *index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}