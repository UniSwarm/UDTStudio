/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
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

#include "bootloader/bootloader.h"
#include "canopenbus.h"
#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"
#include "profile/nodeprofilefactory.h"
#include "profile/p402/nodeprofile402.h"
#include "services/services.h"

Node::Node(quint8 nodeId, const QString &name, const QString &edsFileName)
    : _nodeId(nodeId)
{
    _status = UNKNOWN;
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

    _bootloader = new Bootloader(this);

    if (!edsFileName.isEmpty())
    {
        loadEds(edsFileName);
        NodeProfileFactory::profileFactory(this);
    }
}

Node::~Node()
{
    qDeleteAll(_sdoClients);
    qDeleteAll(_tpdos);
    qDeleteAll(_rpdos);
    qDeleteAll(_nodeProfiles);
    delete _emergency;
    delete _nmt;
    delete _errorControl;
    delete _bootloader;

    delete _nodeOd;
}

void Node::setBus(CanOpenBus *bus)
{
    _bus = bus;
    for (Service *service : qAsConst(_services))
    {
        service->setBus(bus);
    }
    reset();
}

CanOpenBus *Node::bus() const
{
    return _bus;
}

quint8 Node::busId() const
{
    if (_bus != nullptr)
    {
        return _bus->busId();
    }
    return 255;
}

quint8 Node::nodeId() const
{
    return _nodeId;
}

void Node::setNodeId(quint8 nodeId)
{
    _nodeId = nodeId;
}

Node::Status Node::status() const
{
    return _status;
}

QString Node::statusStr() const
{
    switch (_status)
    {
        case Node::UNKNOWN:
            return tr("unknown");

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
    bool changed = (status != _status);
    _status = status;
    if (changed)
    {
        emit statusChanged(_status);
    }
}

const QString &Node::name() const
{
    return _name;
}

void Node::setName(const QString &name)
{
    bool changed = (name != _name);
    _name = name;
    if (changed)
    {
        emit nameChanged(_name);
    }
}

quint16 Node::manufacturerId() const
{
    return static_cast<quint16>(nodeOd()->value(0x1018, 1).toUInt());
}

quint16 Node::productCode() const
{
    return static_cast<quint16>(nodeOd()->value(0x1018, 2).toUInt());
}

quint16 Node::profileNumber() const
{
    return static_cast<quint16>(nodeOd()->value(0x1000).toUInt() & 0xFFFF);
}

NodeOd *Node::nodeOd() const
{
    return _nodeOd;
}

void Node::readObject(const NodeObjectId &id)
{
    readObject(id.index(), id.subIndex(), id.dataType());
}

void Node::readObject(quint16 index, quint8 subindex, QMetaType::Type dataType)
{
    if (_status == STOPPED || _status == UNKNOWN)
    {
        return;
    }

    TPDO *tpdoMapped = isMappedObjectInTpdo(NodeObjectId(index, subindex, dataType));
    if (tpdoMapped != nullptr)
    {
        if (tpdoMapped->isEnabled() && _status == STARTED && _bus->sync()->status() == Sync::STARTED)
        {
            return;
        }
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

void Node::writeObject(quint16 index, quint8 subindex, const QVariant &data)
{
    if (_status == STOPPED || _status == UNKNOWN)
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
    if ((isMappedObjectInRpdo(NodeObjectId(index, subindex)) != nullptr) && _status == STARTED && _bus->sync()->status() == Sync::STARTED)
    {
        bool writtenInRpdo = false;
        for (RPDO *rpdo : qAsConst(_rpdos))
        {
            if (rpdo->isMappedObject(object) && rpdo->isEnabled())
            {
                rpdo->write(object, data);
                writtenInRpdo = true;
            }
        }

        if (writtenInRpdo)
        {
            return;
        }
    }

    _sdoClients.at(0)->downloadData(index, subindex, mdata);
}

void Node::loadEds(const QString &fileName)
{
    _nodeOd->loadEds(fileName);
    emit edsFileChanged(fileName);
}

const QString &Node::edsFileName() const
{
    return _nodeOd->edsFileName();
}

void Node::addProfile(NodeProfile *nodeProfile)
{
    _nodeProfiles.append(nodeProfile);
}

const QList<NodeProfile *> &Node::profiles() const
{
    return _nodeProfiles;
}

int Node::profilesCount() const
{
    return _nodeProfiles.count();
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
    for (RPDO *rpdo : qAsConst(_rpdos))
    {
        if (rpdo->isMappedObject(object))
        {
            return true;
        }
    }
    for (TPDO *tpdo : qAsConst(_tpdos))
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
        if (rpdo->isMappedObject(object))
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

Bootloader *Node::bootloader() const
{
    return _bootloader;
}

QList<Service *> Node::services() const
{
    return _services;
}

void Node::reset()
{
    _nodeOd->resetAllObjects();
    for (Service *service : qAsConst(_services))
    {
        service->reset();
    }
    for (NodeProfile *nodeProfile : qAsConst(_nodeProfiles))
    {
        nodeProfile->reset();
    }
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
