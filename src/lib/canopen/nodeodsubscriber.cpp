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

#include "nodeodsubscriber.h"

#include <QDebug>

#include "canopen.h"
#include "node.h"

NodeOdSubscriber::NodeOdSubscriber()
{
    _nodeInterrest = nullptr;
}

NodeOdSubscriber::~NodeOdSubscriber()
{
    unRegisterFullOd();
}

void NodeOdSubscriber::notifySubscriber(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    this->odNotify(objId, flags);
}

Node *NodeOdSubscriber::nodeInterrest() const
{
    return _nodeInterrest;
}

void NodeOdSubscriber::setNodeInterrest(Node *nodeInterrest)
{
    // unregister old _nodeInterrest
    if (_nodeInterrest)
    {
        _nodeInterrest->nodeOd()->unsubscribe(this);
        for (NodeObjectId &objId : _objIdList)
        {
            if (objId.node() == _nodeInterrest)
            {
                objId.setBusIdNodeId(0xFF, 0xFF);
            }
        }
    }

    _nodeInterrest = nodeInterrest;

    // register new nodeInterrest
    if (_nodeInterrest)
    {
        for (const NodeObjectId &objId : qAsConst(_objIdList))
        {
            if (objId.isNodeIndependant())
            {
                _nodeInterrest->nodeOd()->subscribe(this, objId.index(), objId.subIndex());

                this->odNotify(objId, SDO::Read);
            }
        }
    }
}

void NodeOdSubscriber::readObject(const NodeObjectId &id)
{
    if (_nodeInterrest)
    {
        _nodeInterrest->readObject(id);
    }
}

void NodeOdSubscriber::readObject(quint16 index, quint8 subindex, QMetaType::Type dataType)
{
    if (_nodeInterrest)
    {
        _nodeInterrest->readObject(index, subindex, dataType);
    }
}

void NodeOdSubscriber::writeObject(const NodeObjectId &id, const QVariant &data)
{
    if (_nodeInterrest)
    {
        _nodeInterrest->writeObject(id, data);
    }
}

void NodeOdSubscriber::writeObject(quint16 index, quint8 subindex, const QVariant &data)
{
    if (_nodeInterrest)
    {
        _nodeInterrest->writeObject(index, subindex, data);
    }
}

void NodeOdSubscriber::registerObjId(const NodeObjectId &objId)
{
    registerKey(objId);
}

void NodeOdSubscriber::registerSubIndex(quint16 index, quint8 subindex)
{
    registerKey(NodeObjectId(index, subindex));
}

void NodeOdSubscriber::registerIndex(quint16 index)
{
    registerKey(NodeObjectId(index, 0xFFu));
}

void NodeOdSubscriber::registerFullOd()
{
    registerKey(NodeObjectId(0xFFFFu, 0xFFu));
}

void NodeOdSubscriber::unRegisterObjId(const NodeObjectId &objId)
{
    unRegisterKey(objId);
}

void NodeOdSubscriber::unRegisterSubIndex(quint16 index, quint8 subindex)
{
    unRegisterKey(NodeObjectId(index, subindex));
}

void NodeOdSubscriber::unRegisterIndex(quint16 index)
{
    unRegisterKey(NodeObjectId(index, 0xFFu));
}

void NodeOdSubscriber::unRegisterFullOd()
{
    _indexSubIndexList.clear();
    _objIdList.clear();
    if (_nodeInterrest)
    {
        _nodeInterrest->nodeOd()->unsubscribe(this);
    }
}

QList<NodeObjectId> NodeOdSubscriber::objIdList() const
{
    return _objIdList;
}

void NodeOdSubscriber::registerKey(const NodeObjectId &objId)
{
    quint64 key = objId.key();
    if (_indexSubIndexList.contains(key))
    {
        return;
    }
    _indexSubIndexList.insert(key);
    _objIdList.append(objId);

    // register on nodeOd
    if (objId.isNodeIndependant())
    {
        if (_nodeInterrest)
        {
            _nodeInterrest->nodeOd()->subscribe(this, objId.index(), objId.subIndex());
        }
    }
    else
    {
        Node *node = objId.node();
        if (node)
        {
            NodeOd *nodeOd = node->nodeOd();
            nodeOd->subscribe(this, objId.index(), objId.subIndex());
        }
    }
}

void NodeOdSubscriber::unRegisterKey(const NodeObjectId &objId)
{
    quint64 key = objId.key();
    if (!_indexSubIndexList.contains(key))
    {
        return;
    }
    _indexSubIndexList.remove(key);
    _objIdList.removeOne(objId);

    // unregister on nodeOd
    if (objId.isNodeIndependant())
    {
        if (_nodeInterrest)
        {
            _nodeInterrest->nodeOd()->unsubscribe(this, objId.index(), objId.subIndex());
        }
    }
    else
    {
        Node *node = objId.node();
        if (node)
        {
            NodeOd *nodeOd = node->nodeOd();
            nodeOd->unsubscribe(this, objId.index(), objId.subIndex());
        }
    }
}
