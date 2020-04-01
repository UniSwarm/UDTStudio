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

#include "nodeod.h"
#include "nodeodsubscriber.h"
#include "parser/edsparser.h"
#include "node.h"

NodeOd::NodeOd(Node *node)
    : _node(node)
{
    createMandatoryObject();
}

NodeOd::~NodeOd()
{
    qDeleteAll(_nodeIndexes);
    _nodeIndexes.clear();
}

/**
 * @brief returns the value associated with the key index
 * @param index
 * @return an Index*
 */
NodeIndex *NodeOd::index(quint16 index) const
{
    return _nodeIndexes.value(index);
}

/**
 * @brief inserts a new index. If the index already exist, replaces it with the new index
 * @param index
 */
void NodeOd::addIndex(NodeIndex *index)
{
    _nodeIndexes.insert(index->index(), index);
    index->_nodeOd = this;
}

/**
 * @brief returns the number of index in the map
 * @return number of index
 */
int NodeOd::indexCount() const
{
    return _nodeIndexes.count();
}

/**
 * @brief returns true if the map contains an item with key key; otherwise returns false.
 * @param key
 * @return boolean
 */
bool NodeOd::indexExist(quint16 key) const
{
    return _nodeIndexes.contains(key);
}

void NodeOd::updateObjectFromDevice(quint16 indexDevice, quint8 subindexDevice, const QVariant &value)
{
    if (indexExist(indexDevice))
    {
        index(indexDevice)->subIndex(subindexDevice)->setValue(value);

        notifySubscribers(indexDevice, subindexDevice, value);   // notify subscribers to index/subindex
        notifySubscribers(indexDevice, 0xFFu, value);            // notify subscribers to index with all subindex
        notifySubscribers(0xFFFFu, 0xFFu, value);                // notify subscribers to the full od
    }
}

void NodeOd::createMandatoryObject()
{
    NodeIndex *deviceType = new NodeIndex(0x1000);
    deviceType->setName("Device type");
    deviceType->setObjectType(NodeIndex::VAR);
    deviceType->addSubIndex(new NodeSubIndex(0));
    deviceType->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    deviceType->subIndex(0)->setName("Device type");

    this->addIndex(deviceType);

    NodeIndex *identityObject = new NodeIndex(0x1018);
    identityObject->setName("Identity object");
    identityObject->setObjectType(NodeIndex::RECORD);
    identityObject->addSubIndex(new NodeSubIndex(0));
    identityObject->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(0)->setName("Highest sub-index supported");

    identityObject->addSubIndex(new NodeSubIndex(1));
    identityObject->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(0)->setName("Vendor-ID");

    identityObject->addSubIndex(new NodeSubIndex(2));
    identityObject->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(0)->setName("Product code");

    identityObject->addSubIndex(new NodeSubIndex(3));
    identityObject->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(0)->setName("Revision number");

    identityObject->addSubIndex(new NodeSubIndex(4));
    identityObject->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(0)->setName("Serial number");

    this->addIndex(identityObject);
}

void NodeOd::notifySubscribers(quint16 notifyIndex, quint8 notifySubIndex, const QVariant &value)
{
    quint32 key = (static_cast<quint32>(notifyIndex) << 8) + notifySubIndex;
    QList<Subscriber> interrestedSubscribers = _subscribers.values(key);
    QList<Subscriber>::const_iterator subscriber = interrestedSubscribers.cbegin();
    while (subscriber != interrestedSubscribers.cend())
    {
        NodeOdSubscriber *nodeOdSubscriber = (*subscriber).object;
        nodeOdSubscriber->notifySubscriber(notifyIndex, notifySubIndex, value);
        ++subscriber;
    }
}

bool NodeOd::loadEds(const QString &fileName)
{
    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(fileName);
    _fileName = fileName;

    foreach (Index *index, deviceDescription->indexes())
    {
        NodeIndex *nodeIndex = new NodeIndex(index->index());
        nodeIndex->setName(index->name());
        nodeIndex->setObjectType(static_cast<NodeIndex::ObjectType>(index->objectType()));
        addIndex(nodeIndex);

        foreach (SubIndex *subIndex, index->subIndexes())
        {
            NodeSubIndex *nodeSubIndex = new NodeSubIndex(subIndex->subIndex());
            nodeSubIndex->setName(subIndex->name());
            nodeSubIndex->setAccessType(static_cast<NodeSubIndex::AccessType>(subIndex->accessType()));
            nodeSubIndex->setValue(subIndex->value());
            nodeSubIndex->setDataType(static_cast<NodeSubIndex::DataType>(subIndex->dataType()));
            nodeSubIndex->setLowLimit(subIndex->lowLimit());
            nodeSubIndex->setHighLimit(subIndex->highLimit());
            nodeIndex->addSubIndex(nodeSubIndex);
        }
    }
    qDebug() << ">loadEds :" << fileName;
    return true;
}

QVariant NodeOd::value(NodeObjectId id)
{
    return value(id.index, id.subIndex);
}

QVariant NodeOd::value(quint16 index, quint8 subIndex)
{
    NodeIndex *nodeIndex = this->index(index);
    if (!nodeIndex)
    {
        return QVariant();
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    if (!nodeSubIndex)
    {
        return QVariant();
    }

    return nodeSubIndex->value();
}

void NodeOd::subscribe(NodeOdSubscriber *object, quint16 notifyIndex, quint8 notifySubIndex)
{
    Subscriber subscriber;
    subscriber.object = object;
    subscriber.notifyIndex = notifyIndex;
    subscriber.notifySubIndex = notifySubIndex;
    quint32 key = (static_cast<quint32>(notifyIndex) << 8) + notifySubIndex;
    _subscribers.insert(key, subscriber);
}

void NodeOd::unsubscribe(NodeOdSubscriber *object)
{
    QMultiMap<quint32, Subscriber>::iterator itSub = _subscribers.begin();
    while (itSub != _subscribers.end())
    {
        if (itSub.value().object == object)
        {
            itSub = _subscribers.erase(itSub);
        }
        else
        {
            ++itSub;
        }
    }
}

NodeObjectId::NodeObjectId(quint16 index, quint8 subIndex)
    : index(index), subIndex(subIndex)
{
}
