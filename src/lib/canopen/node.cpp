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

#include <QDebug>
#include "node.h"

#include "canopenbus.h"
#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"
#include "services/services.h"
#include "profile/p402/nodeprofile402.h"

Node::Node(quint8 nodeId, const QString &name, const QString &edsFileName)
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

    if (!edsFileName.isEmpty())
    {
        loadEds(edsFileName);
    }
    _nodeProfiles.clear();
}

Node::~Node()
{
    qDeleteAll(_sdoClients);
    qDeleteAll(_tpdos);
    qDeleteAll(_rpdos);
    delete _emergency;
    delete _nmt;
    delete _errorControl;

    _nodeOd->deleteLater();
}

CanOpenBus *Node::bus() const
{
    return _bus;
}

quint8 Node::busId() const
{
    if (_bus)
    {
        return _bus->busId();
    }
    return 255;
}

QList<Service *> Node::services() const
{
    return _services;
}

NodeOd *Node::nodeOd() const
{
    return _nodeOd;
}

quint8 Node::nodeId() const
{
    return _nodeId;
}

void Node::setNodeId(const quint8 nodeId)
{
    _nodeId = nodeId;
}

const QString &Node::name() const
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

void Node::setStatus(const Status status)
{
    bool changed = (status != _status);
    _status = status;
    if (changed)
    {
        emit statusChanged(_status);
    }
}

quint16 Node::profileNumber() const
{
    return static_cast<quint16>(nodeOd()->value(0x1000).toUInt() & 0xFFFF);
}

void Node::addProfile(NodeProfile *nodeProfile)
{
    // TODO mamagement multi-axis
    _nodeProfiles.append(nodeProfile);
}

const QList<NodeProfile *> &Node::profiles() const
{
    return _nodeProfiles;
}

void Node::sendPreop()
{
    _nmt->sendPreop();
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

void Node::setBus(CanOpenBus *bus)
{
    _bus = bus;
    for (Service *service : _services)
    {
        service->setBus(bus);
    }
}

void Node::reset()
{
    _nodeOd->resetValue();

    for (Service *service : _services)
    {
        service->reset();
    }
}

const QList<RPDO *> &Node::rpdos() const
{
    return _rpdos;
}

const QList<TPDO *> &Node::tpdos() const
{
    return _tpdos;
}

bool Node::isMappedObjectInPdo(const NodeObjectId &object) const
{
    for (RPDO *rpdo : _rpdos)
    {
        if (rpdo->isMappedObject(object))
        {
            return true;
        }
    }
    for (TPDO *tpdo : _tpdos)
    {
        if (tpdo->isMappedObject(object))
        {
            return true;
        }
    }
    return false;
}

RPDO *Node::isMappedObjectInRpdo(const NodeObjectId &object) const
{
    for (RPDO *rpdo : _rpdos)
    {
        if ( rpdo->isMappedObject(object))
        {
            return rpdo;
        }
    }
    return nullptr;
}

TPDO *Node::isMappedObjectInTpdo(const NodeObjectId &object) const
{
    for (TPDO *tpdo : _tpdos)
    {
        if (tpdo->isMappedObject(object))
        {
            return tpdo;
        }
    }
    return nullptr;
}

void Node::readObject(const NodeObjectId &id)
{
    readObject(id.index(), id.subIndex(), id.dataType());
}

void Node::readObject(const quint16 index, const quint8 subindex, const QMetaType::Type dataType)
{
    if (status() == STOPPED)
    {
        return;
    }
    if (isMappedObjectInTpdo(NodeObjectId(index, subindex, dataType)) && status() == STARTED && (_bus->sync()->status() == Sync::STARTED))
    {
        return;
    }
    QMetaType::Type mdataType = dataType;
    if (mdataType == QMetaType::Type::UnknownType)
    {
        mdataType = _nodeOd->dataType(index, subindex);
    }
    _sdoClients.at(0)->uploadData(index, subindex, mdataType);
}

void Node::writeObject(const NodeObjectId &id, const QVariant &data)
{
    writeObject(id.index(), id.subIndex(), data);
}

void Node::writeObject(const quint16 index, const quint8 subindex, const QVariant &data)
{
    if (status() == STOPPED)
    {
        return;
    }
    QVariant mdata = data;
    QMetaType::Type mdataType = _nodeOd->dataType(index, subindex);
    if (mdataType != QMetaType::Type::UnknownType)
    {
        if (mdataType != QMetaType::Type(data.type()))
        {
            mdata.convert(mdataType);
        }
    }

    NodeObjectId object(busId(), nodeId(), index, subindex);
    if (isMappedObjectInRpdo(NodeObjectId(index, subindex)) && status() == STARTED && (_bus->sync()->status() == Sync::STARTED))
    {
        for (RPDO *rpdo : _rpdos)
        {
            if (rpdo->isMappedObject(object))
            {
                rpdo->write(object, data);
            }
        }
        return;
    }
    _sdoClients.at(0)->downloadData(index, subindex, mdata);
}

void Node::loadEds(const QString &fileName)
{
    _nodeOd->loadEds(fileName);
}

void Node::updateFirmware(const QByteArray &prog)
{
    quint16 index = 0x1F50;
    quint8 subindex = 0x01;
    _sdoClients.at(0)->downloadData(index, subindex, prog);
}
