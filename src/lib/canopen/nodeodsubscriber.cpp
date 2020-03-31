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

NodeOdSubscriber::NodeOdSubscriber()
{
    _nodeInterrest = nullptr;
}

void NodeOdSubscriber::notifySubscriber(quint16 index, quint8 subindex, const QVariant &value)
{
    this->odNotify(index, subindex, value);
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
        for (quint32 key : _indexSubIndexList)
        {
            quint16 index = static_cast<quint16>(key >> 8);
            quint8 subIndex = static_cast<quint8>(key & 0xFFu);
            _nodeInterrest->nodeOd()->subscribe(this, index, subIndex);
        }
    }
}

QList<quint32> NodeOdSubscriber::indexSubIndexList() const
{
    return _indexSubIndexList.toList();
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

void NodeOdSubscriber::registerKey(quint16 index, quint8 subindex)
{
    quint32 key = (static_cast<quint32>(index) << 8) + static_cast<quint32>(subindex);
    if (_indexSubIndexList.contains(key))
    {
        return;
    }
    _indexSubIndexList.insert(key);

    if (_nodeInterrest)
    {
        _nodeInterrest->nodeOd()->subscribe(this, index, subindex);
    }
}
