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
#include "model/deviceconfiguration.h"
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

Node *NodeOd::node() const
{
    return _node;
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

const QMap<quint16, NodeIndex *> &NodeOd::indexes() const
{
    return _nodeIndexes;
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

void NodeOd::updateObjectFromDevice(quint16 indexDevice, quint8 subindexDevice, const QVariant &value, SDO::FlagsRequest flags)
{
    if (indexExist(indexDevice))
    {
        if (flags != SDO::Error)
        {
            if (index(indexDevice)->subIndexExist(subindexDevice))
            {
                index(indexDevice)->subIndex(subindexDevice)->clearError();
                index(indexDevice)->subIndex(subindexDevice)->setValue(value);
            }
        }
    }

    quint32 key = (static_cast<quint32>(indexDevice) << 8) + subindexDevice;
    notifySubscribers(key, indexDevice, subindexDevice, flags);   // notify subscribers to index/subindex

    key = (static_cast<quint32>(indexDevice) << 8) + 0xFFu;
    notifySubscribers(key, indexDevice, subindexDevice, flags);   // notify subscribers to index with all subindex

    key = (static_cast<quint32>(0xFFFFu) << 8) + 0xFFu;
    notifySubscribers(key, indexDevice, subindexDevice, flags);   // notify subscribers to the full od
}

void NodeOd::setErrorObject(quint16 indexDevice, quint8 subindexDevice, quint32 error)
{
    if (indexExist(indexDevice))
    {
        index(indexDevice)->subIndex(subindexDevice)->setError(error);
    }
}

quint32 NodeOd::errorObject(const NodeObjectId &id)
{
    return errorObject(id.index, id.subIndex);
}

quint32 NodeOd::errorObject(quint16 indexDevice, quint8 subindexDevice)
{
    if (indexExist(indexDevice))
    {
        return index(indexDevice)->subIndex(subindexDevice)->error();
    }
    return 0;
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
    identityObject->subIndex(1)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(1)->setName("Vendor-ID");

    identityObject->addSubIndex(new NodeSubIndex(2));
    identityObject->subIndex(2)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(2)->setName("Product code");

    identityObject->addSubIndex(new NodeSubIndex(3));
    identityObject->subIndex(3)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(3)->setName("Revision number");

    identityObject->addSubIndex(new NodeSubIndex(4));
    identityObject->subIndex(4)->setDataType(NodeSubIndex::UNSIGNED32);
    identityObject->subIndex(4)->setName("Serial number");

    this->addIndex(identityObject);
}

void NodeOd::notifySubscribers(quint32 key, quint16 notifyIndex, quint8 notifySubIndex, SDO::FlagsRequest flags)
{
    QList<Subscriber> interrestedSubscribers = _subscribers.values(key);
    QList<Subscriber>::const_iterator subscriber = interrestedSubscribers.cbegin();
    while (subscriber != interrestedSubscribers.cend())
    {
        NodeOdSubscriber *nodeOdSubscriber = (*subscriber).object;
        NodeObjectId objId(_node->busId(), _node->nodeId(), notifyIndex, notifySubIndex);
        nodeOdSubscriber->notifySubscriber(objId,  flags);
        ++subscriber;
    }
}

bool NodeOd::loadEds(const QString &fileName)
{
    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(fileName);
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, _node->nodeId());
    _fileName = fileName;

    for (Index *odIndex : deviceConfiguration->indexes())
    {
        NodeIndex *nodeIndex;
        nodeIndex = index(odIndex->index());
        if (!nodeIndex)
        {
            nodeIndex = new NodeIndex(odIndex->index());
        }
        nodeIndex->setName(odIndex->name());
        nodeIndex->setObjectType(static_cast<NodeIndex::ObjectType>(odIndex->objectType()));
        addIndex(nodeIndex);

        for (SubIndex *odSubIndex : odIndex->subIndexes())
        {
            NodeSubIndex *nodeSubIndex;
            nodeSubIndex = nodeIndex->subIndex(odSubIndex->subIndex());
            if (!nodeSubIndex)
            {
                nodeSubIndex = new NodeSubIndex(odSubIndex->subIndex());
            }
            if (!nodeSubIndex->value().isValid())
            {
                nodeSubIndex->setValue(odSubIndex->value());
            }
            nodeSubIndex->setName(odSubIndex->name());
            nodeSubIndex->setAccessType(static_cast<NodeSubIndex::AccessType>(odSubIndex->accessType()));
            nodeSubIndex->setDataType(static_cast<NodeSubIndex::DataType>(odSubIndex->dataType()));
            nodeSubIndex->setLowLimit(odSubIndex->lowLimit());
            nodeSubIndex->setHighLimit(odSubIndex->highLimit());
            nodeIndex->addSubIndex(nodeSubIndex);
        }
    }
    qDebug() << ">NodeOd::loadEds :" << fileName;
    delete deviceDescription;
    delete deviceConfiguration;
    return true;
}

QMetaType::Type NodeOd::dataType(const NodeObjectId &id)
{
    return dataType(id.index, id.subIndex);
}

QMetaType::Type NodeOd::dataType(quint16 index, quint8 subIndex)
{
    NodeIndex *nodeIndex = this->index(index);
    if (!nodeIndex)
    {
        return QMetaType::UnknownType;
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    if (!nodeSubIndex)
    {
        return QMetaType::UnknownType;
    }

    return dataTypeCiaToQt(nodeSubIndex->dataType());
}

QVariant NodeOd::value(const NodeObjectId &id)
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

QDateTime NodeOd::lastModification(const NodeObjectId &id)
{
    return lastModification(id.index, id.subIndex);
}

QDateTime NodeOd::lastModification(quint16 index, quint8 subIndex)
{
    NodeIndex *nodeIndex = this->index(index);
    if (!nodeIndex)
    {
        return QDateTime();
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    if (!nodeSubIndex)
    {
        return QDateTime();
    }

    return nodeSubIndex->lastModification();
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

QMetaType::Type NodeOd::dataTypeCiaToQt(NodeSubIndex::DataType type)
{
    switch (type)
    {
    case NodeSubIndex::NONE:
        break;
    case NodeSubIndex::VISIBLE_STRING:
    case NodeSubIndex::OCTET_STRING:
    case NodeSubIndex::UNICODE_STRING:
        return QMetaType::QByteArray;
    case NodeSubIndex::TIME_OF_DAY:
        break;
    case NodeSubIndex::TIME_DIFFERENCE:
        break;
    case NodeSubIndex::DDOMAIN:
        return QMetaType::QByteArray;

    case NodeSubIndex::BOOLEAN:
    case NodeSubIndex::UNSIGNED8:
        return QMetaType::UChar;
    case NodeSubIndex::INTEGER8:
        return QMetaType::SChar;

    case NodeSubIndex::UNSIGNED16:
        return QMetaType::UShort;
    case NodeSubIndex::INTEGER16:
        return QMetaType::Short;

    case NodeSubIndex::UNSIGNED24:
        return QMetaType::UnknownType;
    case NodeSubIndex::INTEGER24:
        return QMetaType::UnknownType;

    case NodeSubIndex::UNSIGNED32:
        return QMetaType::UInt;
    case NodeSubIndex::INTEGER32:
        return QMetaType::Int;
    case NodeSubIndex::REAL32:
        return QMetaType::Float;

    case NodeSubIndex::UNSIGNED40:
        return QMetaType::UnknownType;
    case NodeSubIndex::INTEGER40:
        return QMetaType::UnknownType;

    case NodeSubIndex::UNSIGNED48:
        return QMetaType::UnknownType;
    case NodeSubIndex::INTEGER48:
        return QMetaType::UnknownType;

    case NodeSubIndex::UNSIGNED56:
        return QMetaType::UnknownType;
    case NodeSubIndex::INTEGER56:
        return QMetaType::UnknownType;

    case NodeSubIndex::UNSIGNED64:
        return QMetaType::ULongLong;
    case NodeSubIndex::INTEGER64:
        return QMetaType::LongLong;
    case NodeSubIndex::REAL64:
        return QMetaType::Double;
    }
    return QMetaType::UnknownType;
}
