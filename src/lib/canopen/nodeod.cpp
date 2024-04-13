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

#include "nodeod.h"

#include "indexdb.h"
#include "model/deviceconfiguration.h"
#include "node.h"
#include "nodeodsubscriber.h"
#include "parser/edsparser.h"
#include "writer/dcfwriter.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

NodeOd::NodeOd(Node *node)
    : _node(node)
{
    createMandatoryObjects();
}

NodeOd::~NodeOd()
{
    // Remove reference of this instance to all subcriber
    QMultiMap<quint32, Subscriber>::iterator itSub = _subscribers.begin();
    while (itSub != _subscribers.end())
    {
        itSub.value().object->_nodeInterrest = nullptr;
        ++itSub;
    }

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
            updateObjectFromDevice(subIndex->index(), subIndex->subIndex(), subIndex->value(), NodeOd::Read);
        }
    }
}

bool NodeOd::loadEds(const QString &fileName)
{
    QString mfileName(fileName);
    mfileName = QFileInfo(mfileName).canonicalFilePath();

    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(mfileName);
    if (deviceDescription == nullptr)
    {
        return false;
    }
    _edsFileInfos = deviceDescription->fileInfos();

    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, _node->nodeId());
    _edsFileName = mfileName;

    for (Index *odIndex : qAsConst(deviceConfiguration->indexes()))
    {
        NodeIndex *nodeIndex;
        nodeIndex = index(odIndex->index());
        if (nodeIndex == nullptr)
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
            if (nodeSubIndex == nullptr)
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
            nodeSubIndex->setScale(IndexDb::scale(nodeSubIndex->objectId(), _node->profileNumber()));
            nodeSubIndex->setUnit(IndexDb::unit(nodeSubIndex->objectId(), _node->profileNumber()));
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
    QString mfileName(fileName);
    if (mfileName.isEmpty())
    {
        return false;
    }
    if (!mfileName.endsWith(QStringLiteral(".dcf")))
    {
        mfileName.append(QStringLiteral(".dcf"));
    }

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
    dcfWriter.write(&deviceConfiguration, mfileName);
    return true;
}

bool NodeOd::exportConf(const QString &fileName) const
{
    QString mfileName(fileName);
    if (mfileName.isEmpty())
    {
        return false;
    }
    if (!mfileName.endsWith(QStringLiteral(".conf")))
    {
        mfileName.append(QStringLiteral(".conf"));
    }

    QFile file(mfileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    QTextStream stream(&file);
    stream << "[Default]" << '\n';
    for (NodeIndex *index : _nodeIndexes)
    {
        for (NodeSubIndex *subIndex : index->subIndexes())
        {
            if (subIndex->isWritable() && subIndex->value() != subIndex->defaultValue() && subIndex->dataType() != NodeSubIndex::DDOMAIN)
            {
                stream << index->name() << '.' << subIndex->name() << '=' << subIndex->value().toInt() << '\n';
            }
        }
    }
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
bool NodeOd::indexExist(const quint16 index) const
{
    return _nodeIndexes.contains(index);
}

NodeSubIndex *NodeOd::subIndex(quint16 index, quint8 subIndex) const
{
    NodeIndex *nodeIndex = this->index(index);
    if (nodeIndex == nullptr)
    {
        return nullptr;
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    if (nodeSubIndex == nullptr)
    {
        return nullptr;
    }

    return nodeSubIndex;
}

NodeSubIndex *NodeOd::subIndex(const NodeObjectId &id) const
{
    return subIndex(id.index(), id.subIndex());
}

bool NodeOd::subIndexExist(quint16 index, quint8 subIndex) const
{
    NodeIndex *nodeIndex = this->index(index);
    if (nodeIndex == nullptr)
    {
        return false;
    }

    NodeSubIndex *nodeSubIndex = nodeIndex->subIndex(subIndex);
    return (nodeSubIndex != nullptr);
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

void NodeOd::setErrorObject(quint16 index, quint8 subIndex, quint32 error) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (nodeSubIndex == nullptr)
    {
        return;
    }

    nodeSubIndex->setError(error);
}

quint32 NodeOd::errorObject(const NodeObjectId &id) const
{
    return errorObject(id.index(), id.subIndex());
}

quint32 NodeOd::errorObject(quint16 index, quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (nodeSubIndex == nullptr)
    {
        return 0;
    }

    return nodeSubIndex->error();
}

QVariant NodeOd::value(const NodeObjectId &id) const
{
    return value(id.index(), id.subIndex());
}

QVariant NodeOd::value(quint16 index, quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (nodeSubIndex == nullptr)
    {
        return QVariant();
    }

    return nodeSubIndex->value();
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

QMetaType::Type NodeOd::dataType(const NodeObjectId &id) const
{
    return dataType(id.index(), id.subIndex());
}

QMetaType::Type NodeOd::dataType(quint16 index, quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (nodeSubIndex == nullptr)
    {
        return QMetaType::UnknownType;
    }

    return dataTypeCiaToQt(nodeSubIndex->dataType());
}

QDateTime NodeOd::lastModification(const NodeObjectId &id) const
{
    return lastModification(id.index(), id.subIndex());
}

QDateTime NodeOd::lastModification(quint16 index, quint8 subIndex) const
{
    NodeSubIndex *nodeSubIndex = this->subIndex(index, subIndex);
    if (nodeSubIndex == nullptr)
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

void NodeOd::unsubscribe(NodeOdSubscriber *object, quint16 notifyIndex, quint8 notifySubIndex)
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

void NodeOd::updateObjectFromDevice(quint16 index, quint8 subindex, const QVariant &value, NodeOd::FlagsRequest flags, const QDateTime &modificationDate)
{
    NodeSubIndex *nodeSubIndex = subIndex(index, subindex);
    if (nodeSubIndex == nullptr)
    {
        return;
    }

    if ((flags & NodeOd::Error) == 0)
    {
        nodeSubIndex->clearError();
        nodeSubIndex->setValue(value, modificationDate);
    }
    else
    {
        nodeSubIndex->setError(static_cast<quint32>(value.toUInt()));
    }

    quint32 key = (static_cast<quint32>(index) << 8) + subindex;
    notifySubscribers(key, index, subindex, flags);  // notify subscribers to index/subindex

    key = (static_cast<quint32>(index) << 8) + 0xFFU;
    notifySubscribers(key, index, subindex, flags);  // notify subscribers to index with all subindex

    key = (static_cast<quint32>(0xFFFFU) << 8) + 0xFFU;
    notifySubscribers(key, index, subindex, flags);  // notify subscribers to the full od
}

void NodeOd::createMandatoryObjects()
{
    NodeIndex *deviceType = new NodeIndex(0x1000);
    deviceType->setName(QStringLiteral("Device type"));
    deviceType->setObjectType(NodeIndex::VAR);
    deviceType->addSubIndex(new NodeSubIndex(0));
    deviceType->subIndex(0)->setAccessType(NodeSubIndex::READ);
    deviceType->subIndex(0)->setDataType(NodeSubIndex::UNSIGNED32);
    deviceType->subIndex(0)->setName(QStringLiteral("Device type"));
    addIndex(deviceType);

    NodeIndex *identityObject = new NodeIndex(0x1018);
    identityObject->setName(QStringLiteral("Identity object"));
    identityObject->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndex;
    subIndex = new NodeSubIndex(0);
    subIndex->setDataType(NodeSubIndex::UNSIGNED8);
    subIndex->setName(QStringLiteral("Highest sub-index supported"));
    subIndex->setAccessType(NodeSubIndex::READ);
    subIndex->setValue(QVariant(static_cast<uint>(4)));
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(1);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName(QStringLiteral("Vendor-ID"));
    subIndex->setAccessType(NodeSubIndex::READ);
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(2);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName(QStringLiteral("Product code"));
    subIndex->setAccessType(NodeSubIndex::READ);
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(3);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName(QStringLiteral("Revision number"));
    subIndex->setAccessType(NodeSubIndex::READ);
    identityObject->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(4);
    subIndex->setDataType(NodeSubIndex::UNSIGNED32);
    subIndex->setName(QStringLiteral("Serial number"));
    subIndex->setAccessType(NodeSubIndex::READ);
    identityObject->addSubIndex(subIndex);

    addIndex(identityObject);
}

void NodeOd::createBootloaderObjects()
{
    NodeIndex *versionHard = new NodeIndex(0x1009);
    versionHard->setName(QStringLiteral("Manufacturer Hardware Version"));
    versionHard->setObjectType(NodeIndex::VAR);
    versionHard->addSubIndex(new NodeSubIndex(0));
    versionHard->subIndex(0)->setDataType(NodeSubIndex::VISIBLE_STRING);
    versionHard->subIndex(0)->setAccessType(NodeSubIndex::READ);
    addIndex(versionHard);

    NodeIndex *version = new NodeIndex(0x100A);
    version->setName(QStringLiteral("Manufacturer Software Version"));
    version->setObjectType(NodeIndex::VAR);
    version->addSubIndex(new NodeSubIndex(0));
    version->subIndex(0)->setDataType(NodeSubIndex::VISIBLE_STRING);
    version->subIndex(0)->setAccessType(NodeSubIndex::READ);
    addIndex(version);

    NodeIndex *program = new NodeIndex(0x1F50);
    program->setName(QStringLiteral("Program"));
    program->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndex;
    subIndex = new NodeSubIndex(0);
    subIndex->setDataType(NodeSubIndex::UNSIGNED8);
    subIndex->setName(QStringLiteral("Number of Entries"));
    subIndex->setValue(1);
    subIndex->setAccessType(NodeSubIndex::READ);
    program->addSubIndex(subIndex);

    subIndex = new NodeSubIndex(1);
    subIndex->setDataType(NodeSubIndex::DDOMAIN);
    subIndex->setName(QStringLiteral("Program_1"));
    subIndex->setAccessType(static_cast<NodeSubIndex::AccessType>(NodeSubIndex::READ | NodeSubIndex::WRITE));
    program->addSubIndex(subIndex);
    addIndex(program);

    NodeIndex *programControl = new NodeIndex(0x1F51);
    programControl->setName(QStringLiteral("Program control"));
    programControl->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndexProgramControl;
    subIndexProgramControl = new NodeSubIndex(0);
    subIndexProgramControl->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexProgramControl->setName(QStringLiteral("Number of Entries"));
    subIndexProgramControl->setValue(1);
    subIndexProgramControl->setAccessType(NodeSubIndex::READ);
    programControl->addSubIndex(subIndexProgramControl);

    subIndexProgramControl = new NodeSubIndex(1);
    subIndexProgramControl->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexProgramControl->setName(QStringLiteral("Program_1"));
    subIndexProgramControl->setAccessType(static_cast<NodeSubIndex::AccessType>(NodeSubIndex::READ | NodeSubIndex::WRITE));
    programControl->addSubIndex(subIndexProgramControl);
    addIndex(programControl);

    NodeIndex *date = new NodeIndex(0x2003);
    date->setName(QStringLiteral("Firmware_build_date"));
    date->setObjectType(NodeIndex::VAR);
    date->addSubIndex(new NodeSubIndex(0));
    date->subIndex(0)->setDataType(NodeSubIndex::VISIBLE_STRING);
    date->subIndex(0)->setName(QStringLiteral("Firmware_build_date"));
    date->subIndex(0)->setAccessType(NodeSubIndex::READ);
    addIndex(date);

    NodeIndex *conf = new NodeIndex(0x2040);
    conf->setName(QStringLiteral("Comunication_config"));
    conf->setObjectType(NodeIndex::ARRAY);

    NodeSubIndex *subIndexconf;
    subIndexconf = new NodeSubIndex(0);
    subIndexconf->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexconf->setName(QStringLiteral("Number of Entries"));
    subIndexconf->setValue(1);
    subIndexconf->setAccessType(NodeSubIndex::READ);
    conf->addSubIndex(subIndexconf);

    subIndexconf = new NodeSubIndex(1);
    subIndexconf->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexconf->setAccessType(static_cast<NodeSubIndex::AccessType>(NodeSubIndex::READ | NodeSubIndex::WRITE));
    subIndexconf->setName(QStringLiteral("Node_ID"));
    conf->addSubIndex(subIndexconf);

    subIndexconf = new NodeSubIndex(2);
    subIndexconf->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexconf->setAccessType(static_cast<NodeSubIndex::AccessType>(NodeSubIndex::READ | NodeSubIndex::WRITE));
    subIndexconf->setName(QStringLiteral("Bit_rate"));
    conf->addSubIndex(subIndexconf);

    if (!indexExist(conf->_index))
    {
        addIndex(conf);
    }

    NodeIndex *bootloader = new NodeIndex(0x2050);
    bootloader->setName(QStringLiteral("Bootloader"));
    bootloader->setObjectType(NodeIndex::RECORD);

    NodeSubIndex *subIndexbootloader;
    subIndexbootloader = new NodeSubIndex(0);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexbootloader->setName(QStringLiteral("Highest sub-index supported"));
    subIndexbootloader->setValue(1);
    subIndexbootloader->setAccessType(NodeSubIndex::AccessType::READ);
    bootloader->addSubIndex(subIndexbootloader);

    subIndexbootloader = new NodeSubIndex(1);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED16);
    subIndexbootloader->setAccessType(NodeSubIndex::AccessType::WRITE);
    subIndexbootloader->setName(QStringLiteral("Key"));
    bootloader->addSubIndex(subIndexbootloader);

    subIndexbootloader = new NodeSubIndex(2);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexbootloader->setAccessType(NodeSubIndex::AccessType::WRITE);
    subIndexbootloader->setName(QStringLiteral("Checksum"));
    bootloader->addSubIndex(subIndexbootloader);

    subIndexbootloader = new NodeSubIndex(3);
    subIndexbootloader->setDataType(NodeSubIndex::UNSIGNED8);
    subIndexbootloader->setAccessType(NodeSubIndex::AccessType::READ);
    subIndexbootloader->setName(QStringLiteral("Status"));
    bootloader->addSubIndex(subIndexbootloader);

    if (!indexExist(bootloader->_index))
    {
        addIndex(bootloader);
    }
}

const QMap<QString, QString> &NodeOd::edsFileInfos() const
{
    return _edsFileInfos;
}

void NodeOd::notifySubscribers(quint32 key, quint16 notifyIndex, quint8 notifySubIndex, NodeOd::FlagsRequest flags)
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
