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

#include <QDebug>

#include <writer/dcfwriter.h>

#include "model/deviceconfiguration.h"
#include "indexdb.h"
#include "node.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"
#include "parser/edsparser.h"

NodeOd::NodeOd(Node *node)
    : _node(node)
{
    createMandatoryObjects();
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

void NodeOd::resetAllObjects()
{
    for (NodeIndex *index : qAsConst(_nodeIndexes))
    {
        for (NodeSubIndex *subIndex : index->subIndexes())
        {
            subIndex->resetValue();
        }
    }
}

bool NodeOd::loadEds(const QString &fileName)
{
    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(fileName);
    if (!deviceDescription)
    {
        return false;
    }
    _edsFileInfos = deviceDescription->fileInfos();

    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, _node->nodeId());
    _edsFileName = fileName;

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
            nodeSubIndex->setDefaultValue(odSubIndex->value());
            nodeSubIndex->setName(odSubIndex->name());
            nodeSubIndex->setAccessType(static_cast<NodeSubIndex::AccessType>(odSubIndex->accessType()));
            nodeSubIndex->setDataType(static_cast<NodeSubIndex::DataType>(odSubIndex->dataType()));
            nodeSubIndex->setLowLimit(odSubIndex->lowLimit());
            nodeSubIndex->setHighLimit(odSubIndex->highLimit());
            nodeIndex->addSubIndex(nodeSubIndex);
            nodeSubIndex->setQ1516(IndexDb::isQ1516(nodeSubIndex->objectId(), _node->profileNumber()));
        }
    }

    delete deviceDescription;
    delete deviceConfiguration;

    return true;
}

const QString &NodeOd::edsFileName() const
{
    return _edsFileName;
}

bool NodeOd::exportDcf(const QString &fileName) const
{
    DeviceConfiguration deviceConfiguration;
    deviceConfiguration.setNodeId(QString::number(_node->nodeId()));
    deviceConfiguration.setNodeName(_node->name());

    for (NodeIndex *nodeIndex : _nodeIndexes)
    {
        Index *index = new Index(nodeIndex->index());
        index->setName(nodeIndex->name());
        index->setObjectType(static_cast<Index::Object>(nodeIndex->objectType()));
        deviceConfiguration.addIndex(index);

        for (NodeSubIndex *nodeSubIndex : nodeIndex->subIndexes())
        {
            SubIndex *subIndex = new SubIndex(nodeSubIndex->subIndex());
            subIndex->setName(nodeSubIndex->name());
            subIndex->setAccessType(static_cast<SubIndex::AccessType>(nodeSubIndex->accessType()));
            subIndex->setDataType(static_cast<SubIndex::DataType>(nodeSubIndex->dataType()));
            if (nodeSubIndex->error() != 0)
            {
                subIndex->setValue(QVariant());
            }
            else
            {
                subIndex->setValue(nodeSubIndex->value());
            }
            index->addSubIndex(subIndex);
        }
    }

    DcfWriter dcfWriter;
    dcfWriter.write(&deviceConfiguration, fileName);
    return true;
}

const QMap<quint16, NodeIndex *> &NodeOd::indexes() const
{
    return _nodeIndexes;
}

/**
 * @brief returns the value associated with the key index
 * @param index
 * @return an Index*
 */
NodeIndex *NodeOd::index(const quint16 index) const
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
bool NodeOd::indexExist(const quint16 index) const
{
    return _nodeIndexes.contains(index);
}

NodeSubIndex *NodeOd::subIndex(const quint16 index, const quint8 subIndex) const
{
    NodeIndex *nodeIndex = this->index(index);
    if (!nodeIndex)
    {
        return nullptr;
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    if (!nodeSubIndex)
    {
        return nullptr;
    }

    return nodeSubIndex;
}

bool NodeOd::subIndexExist(const quint16 index, const quint8 subIndex) const
{
    NodeIndex *nodeIndex = this->index(index);
    if (!nodeIndex)
    {
        return false;
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    if (!nodeSubIndex)
    {
        return false;
    }

    return true;
}

int NodeOd::subIndexCount() const
{
    int count = 0;
    for (NodeIndex *index : _nodeIndexes)
    {
        count += index->subIndexesCount();
    }
    return count;
}

void NodeOd::setErrorObject(const quint16 index, const quint8 subIndex, const quint32 error)
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (!nodeSubIndex)
    {
        return;
    }

    nodeSubIndex->setError(error);
}

quint32 NodeOd::errorObject(const NodeObjectId &id) const
{
    return errorObject(id.index(), id.subIndex());
}

quint32 NodeOd::errorObject(const quint16 index, const quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (!nodeSubIndex)
    {
        return 0;
    }

    return nodeSubIndex->error();
}

QVariant NodeOd::value(const NodeObjectId &id) const
{
    return value(id.index(), id.subIndex());
}

QVariant NodeOd::value(const quint16 index, const quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (!nodeSubIndex)
    {
        return QVariant();
    }

    return nodeSubIndex->value();
}

QMetaType::Type NodeOd::dataTypeCiaToQt(const NodeSubIndex::DataType type)
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

QMetaType::Type NodeOd::dataType(const NodeObjectId &id) const
{
    return dataType(id.index(), id.subIndex());
}

QMetaType::Type NodeOd::dataType(const quint16 index, const quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (!nodeSubIndex)
    {
        return QMetaType::UnknownType;
    }

    return dataTypeCiaToQt(nodeSubIndex->dataType());
}

QDateTime NodeOd::lastModification(const NodeObjectId &id) const
{
    return lastModification(id.index(), id.subIndex());
}

QDateTime NodeOd::lastModification(const quint16 index, const quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (!nodeSubIndex)
    {
        return QDateTime();
    }

    return nodeSubIndex->lastModification();
}

void NodeOd::subscribe(NodeOdSubscriber *object, const quint16 notifyIndex, const quint8 notifySubIndex)
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

void NodeOd::unsubscribe(NodeOdSubscriber *object, const quint16 notifyIndex, const quint8 notifySubIndex)
{
    QMultiMap<quint32, Subscriber>::iterator itSub = _subscribers.begin();
    while (itSub != _subscribers.end())
    {
        if ((*itSub).object == object && (*itSub).notifyIndex == notifyIndex && (*itSub).notifySubIndex == notifySubIndex)
        {
            itSub = _subscribers.erase(itSub);
        }
        else
        {
            ++itSub;
        }
    }
}

void NodeOd::updateObjectFromDevice(const quint16 indexDevice, const quint8 subindexDevice, const QVariant &value, const SDO::FlagsRequest flags)
{
    if (indexExist(indexDevice))
    {
        if (index(indexDevice)->subIndexExist(subindexDevice))
        {
            if (!(flags & SDO::Error))
            {
                index(indexDevice)->subIndex(subindexDevice)->clearError();
                index(indexDevice)->subIndex(subindexDevice)->setValue(value);
            }
            else
            {
                index(indexDevice)->subIndex(subindexDevice)->setError(static_cast<quint32>(value.toUInt()));
            }
        }
    }

    quint32 key = (static_cast<quint32>(indexDevice) << 8) + subindexDevice;
    notifySubscribers(key, indexDevice, subindexDevice, flags); // notify subscribers to index/subindex

    key = (static_cast<quint32>(indexDevice) << 8) + 0xFFu;
    notifySubscribers(key, indexDevice, subindexDevice, flags); // notify subscribers to index with all subindex

    key = (static_cast<quint32>(0xFFFFu) << 8) + 0xFFu;
    notifySubscribers(key, indexDevice, subindexDevice, flags); // notify subscribers to the full od
}

void NodeOd::store(uint8_t subIndex, uint32_t signature)
{
    NodeObjectId store = IndexDb::getObjectId(IndexDb::OD_STORE, subIndex);
    _node->writeObject(store, signature);
}

void NodeOd::restore(uint8_t subIndex, uint32_t signature)
{
    NodeObjectId restore = IndexDb::getObjectId(IndexDb::OD_RESTORE, subIndex);
    _node->writeObject(restore, signature);
}

void NodeOd::createMandatoryObjects()
{
    NodeIndex *deviceType = new NodeIndex(0x1000);
    deviceType->setName("Device type");
    deviceType->setObjectType(NodeIndex::VAR);
    deviceType->addSubIndex(new NodeSubIndex(0));
    deviceType->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    deviceType->subIndex(0)->setName("Device type");
    addIndex(deviceType);

    NodeIndex *identityObject = new NodeIndex(0x1018);
    identityObject->setName("Identity object");
    identityObject->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndex;
    subIndex = new NodeSubIndex(0);
    subIndex->setDataType(NodeSubIndex::UNSIGNED8);
    subIndex->setName("Highest sub-index supported");
    subIndex->setValue(4);
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(1);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName("Vendor-ID");
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(2);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName("Product code");
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(3);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName("Revision number");
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(4);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName("Serial number");
    identityObject->addSubIndex(subIndex);

    addIndex(identityObject);
}

void NodeOd::createBootloaderObjects()
{
    NodeIndex *program = new NodeIndex(0x1F50);
    program->setName("Program");
    program->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndex;
    subIndex = new NodeSubIndex(0);
    subIndex->setDataType(NodeSubIndex::UNSIGNED8);
    subIndex->setName("Highest sub-index supported");
    subIndex->setValue(1);
    program->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(1);
    subIndex->setDataType(NodeSubIndex::DDOMAIN);
    subIndex->setName("Program_1");
    program->addSubIndex(subIndex);
    addIndex(program);

    NodeIndex *programControl = new NodeIndex(0x1F51);
    programControl->setName("Program control");
    programControl->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndexProgramControl;
    subIndexProgramControl = new NodeSubIndex(0);
    subIndexProgramControl->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexProgramControl->setName("Highest sub-index supported");
    subIndexProgramControl->setValue(1);
    programControl->addSubIndex(subIndexProgramControl);

    subIndexProgramControl = new NodeSubIndex(1);
    subIndexProgramControl->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexProgramControl->setName("Program_1");
    subIndexProgramControl->setAccessType(static_cast<NodeSubIndex::AccessType>(0x03));
    programControl->addSubIndex(subIndexProgramControl);
    addIndex(programControl);

    NodeIndex *bootloader = new NodeIndex(0x2050);
    bootloader->setName("Bootloader");
    bootloader->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndexbootloader;
    subIndexbootloader = new NodeSubIndex(0);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexbootloader->setName("Highest sub-index supported");
    subIndexbootloader->setValue(1);
    bootloader->addSubIndex(subIndexbootloader);

    subIndexbootloader = new NodeSubIndex(1);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED16);
    subIndexbootloader->setName("Key");
    bootloader->addSubIndex(subIndexbootloader);

    subIndexbootloader = new NodeSubIndex(2);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexbootloader->setName("Checksum");
    bootloader->addSubIndex(subIndexbootloader);

    subIndexbootloader = new NodeSubIndex(3);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexbootloader->setName("Status");
    bootloader->addSubIndex(subIndexbootloader);

    addIndex(bootloader);
}

const QMap<QString, QString> &NodeOd::edsFileInfos() const
{
    return _edsFileInfos;
}

void NodeOd::notifySubscribers(quint32 key, quint16 notifyIndex, quint8 notifySubIndex, SDO::FlagsRequest flags)
{
    QList<Subscriber> interrestedSubscribers = _subscribers.values(key);
    QList<Subscriber>::const_iterator subscriber = interrestedSubscribers.cbegin();
    while (subscriber != interrestedSubscribers.cend())
    {
        NodeOdSubscriber *nodeOdSubscriber = (*subscriber).object;
        NodeObjectId objId(_node->busId(), _node->nodeId(), notifyIndex, notifySubIndex);
        nodeOdSubscriber->notifySubscriber(objId, flags);
        ++subscriber;
    }
}
