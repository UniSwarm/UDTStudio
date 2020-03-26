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

Node::Node(quint8 nodeId, const QString &name)
    : _nodeId(nodeId)
{
    _status = INIT;
    _bus = nullptr;
    _nodeOd = new NodeOd(this);

    if (name.isEmpty())
    {
        _name = QString("Node %1").arg(_nodeId);
    }
    else
    {
        _name = name;
    }

    // services
    _emergency = new Emergency(this);
    _services.append(_emergency);

    _nmt = new NMT(this);
    _services.append(_nmt);

    _errorControl = new ErrorControl(this);
    _services.append(_errorControl);

    SDO *sdo = new SDO(this);
    _sdoClients.append(sdo);
    _services.append(sdo);

    for (quint8 i = 0; i < 4; i++)
    {
        RPDO *rpdo = new RPDO(this, i);
        _rpdos.append(rpdo);
        _services.append(rpdo);
    }

    for (quint8 i = 0; i < 4; i++)
    {
        TPDO *tpdo = new TPDO(this, i);
        _tpdos.append(tpdo);
        _services.append(tpdo);
    }
}

Node::~Node()
{
    qDeleteAll(_sdoClients);
    qDeleteAll(_tpdos);
    qDeleteAll(_rpdos);
    delete _emergency;
    delete _nmt;
    delete _nodeOd;
}

CanOpenBus *Node::bus() const
{
    return _bus;
}

QList<Service *> Node::services() const
{
    return _services;
}

quint8 Node::nodeId() const
{
    return _nodeId;
}

void Node::setNodeId(const quint8 &nodeId)
{
    _nodeId = nodeId;
}

QString Node::name() const
{
    return _name;
}

void Node::setName(const QString &name)
{
    _name = name;
}

Node::Status Node::status() const
{
    return _status;
}

QString Node::statusStr() const
{
    switch (_status)
    {
    case Node::INIT:
        return tr("init");
    case Node::PREOP:
        return tr("preop");
    case Node::STARTED:
        return tr("started");
    case Node::STOPPED:
        return tr("stopped");
    }
    return QString();
}

void Node::setStatus(Status status)
{
    _status = status;
}

void Node::sendStart()
{
    _nmt->sendStart();
}

void Node::sendStop()
{
    _nmt->sendStop();
}

void Node::sendResetComm()
{
    _nmt->sendResetComm();
}

void Node::sendResetNode()
{
    _nmt->sendResetNode();
}

void Node::readObjet(Index &index, uint8_t subindex)
{
    _sdoClients.at(0)->uploadData(index, subindex);
}

void Node::writeObjet(Index &index, uint8_t subindex)
{
    _sdoClients.at(0)->downloadData(index, subindex);
}

void Node::loadEds(const QString &fileName)
{
    _nodeOd->loadEds(fileName);
}

void Node::updateFirmware(const QByteArray &prog)
{
    uint16_t index = 0x1F50;
    uint8_t subindex = 0x01;
    Index *index2;

    if (_nodeOd->indexExist(index))
    {
        index2 = _nodeOd->index(index);
        index2->subIndex(subindex)->setValue(prog);
        _sdoClients.at(0)->downloadData(*index2, subindex);
    }
}

QString Node::device()
{
    uint16_t index = 0x1000;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (_nodeOd->indexExist(index))
    {
        index2 = _nodeOd->index(index);
        _sdoClients.at(0)->uploadData(*index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}

QString Node::manuDeviceName()
{
    uint16_t index = 0x1008;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (_nodeOd->indexExist(index))
    {
        index2 = _nodeOd->index(index);
        _sdoClients.at(0)->uploadData(*index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
QString Node::manufacturerHardwareVersion()
{
    uint16_t index = 0x1009;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (_nodeOd->indexExist(index))
    {
        index2 = _nodeOd->index(index);
        _sdoClients.at(0)->uploadData(*index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
QString Node::manufacturerSoftwareVersion()
{
    uint16_t index = 0x100a;
    uint8_t subindex = 0x00;
    Index *index2 = nullptr;

    if (_nodeOd->indexExist(index))
    {
        index2 = _nodeOd->index(index);
        _sdoClients.at(0)->uploadData(*index2, subindex);
    }
    return index2->subIndex(subindex)->value().toString();
}
