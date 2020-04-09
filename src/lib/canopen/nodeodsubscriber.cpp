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

#include "nodeodsubscriber.h"

#include <QDebug>

#include "node.h"
#include "canopen.h"

NodeOdSubscriber::NodeOdSubscriber()
{
    _nodeInterrest = nullptr;
}

NodeOdSubscriber::~NodeOdSubscriber()
{
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
    }

    _nodeInterrest = nodeInterrest;

    // register new nodeInterrest
    if (_nodeInterrest)
    {
        for (const NodeObjectId &objId : _objIdList)
        {
            if (objId.isNodeIndependant())
            {
                _nodeInterrest->nodeOd()->subscribe(this, objId.index, objId.subIndex);

                this->odNotify(objId, SDO::Read);
            }
        }
    }
}

void NodeOdSubscriber::readObject(quint16 index, quint8 subindex, QMetaType::Type dataType)
{
    if (_nodeInterrest)
    {
        _nodeInterrest->readObject(index, subindex, dataType);
    }
}

void NodeOdSubscriber::registerObjId(const NodeObjectId &objId)
{
    registerKey(objId);
}

void NodeOdSubscriber::registerSubIndex(quint16 index, quint8 subindex)
{
    registerKey(index, subindex);
}

void NodeOdSubscriber::registerIndex(quint16 index)
{
    registerKey(index);
}

void NodeOdSubscriber::registerFullOd()
{
    registerKey();
}

QList<NodeObjectId> NodeOdSubscriber::objIdList() const
{
    return _objIdList;
}

void NodeOdSubscriber::registerKey(quint16 index, quint8 subindex)
{
    registerKey({index, subindex});
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
            _nodeInterrest->nodeOd()->subscribe(this, objId.index, objId.subIndex);
        }
    }
    else
    {
        Node *node = objId.node();
        if (node)
        {
            NodeOd *nodeOd = node->nodeOd();
            nodeOd->subscribe(this, objId.index, objId.subIndex);
        }
    }
}
