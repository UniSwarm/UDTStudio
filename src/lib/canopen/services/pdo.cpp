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

#include "pdo.h"

#include "canopenbus.h"
#include "services/services.h"

#include <QDataStream>
#include <QDebug>

#define PDO_INDEX_MASK 0xFFFF0000
#define PDO_SUBINDEX_MASK 0x0000FF00
#define PDO_DATASIZE_MASK 0x000000FF
#define COBID_MASK 0x7FFFFFFF
#define COBID_VALID_NOT_VALID 0x80000000u

PDO::PDO(Node *node, quint8 number)
    : Service(node)
    , _pdoNumber(number)
{
    _statusPdo = STATE_NONE;
    _stateMapping = STATE_FREE;

    _objectIdFsm = 0;

    _waitingConf.transType = 0;
    _waitingConf.eventTimer = 0;
    _waitingConf.inhibitTime = 0;
    _waitingConf.syncStartValue = 0;
}

quint32 PDO::cobId() const
{
    return _cobId;
}

quint8 PDO::pdoNumber() const
{
    return _pdoNumber;
}

void PDO::reset()
{
    _statusPdo = STATE_NONE;
    _stateMapping = STATE_FREE;

    _objectCurrentMapped.clear();
    _objectToMap.clear();
    createListObjectMapped();
}

const QList<NodeObjectId> &PDO::currentMappind() const
{
    return _objectCurrentMapped;
}

bool PDO::hasMappedObject() const
{
    return !_objectCurrentMapped.isEmpty();
}

bool PDO::isMappedObject(const NodeObjectId &object) const
{
    for (const NodeObjectId &objectIterator : qAsConst(_objectCurrentMapped))
    {
        if ((objectIterator.index() == object.index()) && (objectIterator.subIndex() == object.subIndex()))
        {
            return true;
        }
    }
    return false;
}

bool PDO::canInsertObjectAtBitPos(const NodeObjectId &object, int bitPos) const
{
    return canInsertObjectAtBitPos(_objectCurrentMapped, object, bitPos);
}

bool PDO::canInsertObjectAtBitPos(const QList<NodeObjectId> &objectList, const NodeObjectId &object, int bitPos) const
{
    NodeSubIndex *nodeSubIndex = object.nodeSubIndex();
    if (!nodeSubIndex)
    {
        return false;
    }
    if (isTPDO() != nodeSubIndex->hasTPDOAccess())
    {
        return false;
    }
    if (isRPDO() != nodeSubIndex->hasRPDOAccess())
    {
        return false;
    }
    if (objectList.count() + 1 > maxMappingObjectCount())
    {
        return false;
    }

    if (bitPos < 0 || bitPos >= maxMappingBitSize())
    {
        return false;
    }

    int totalBitSize = 0;
    for (const NodeObjectId &objId : objectList)
    {
        int bitSize = objId.bitSize();
        if (bitPos > totalBitSize && bitPos < (totalBitSize + bitSize))
        {
            return false;
        }
        totalBitSize += bitSize;
    }
    if (totalBitSize + nodeSubIndex->bitLength() > maxMappingBitSize())
    {
        return false;
    }
    if (bitPos > totalBitSize)
    {
        return false;
    }

    return true;
}

int PDO::maxMappingBitSize() const
{
    // TODO add CAN Fd mode case
    return 64;
}

int PDO::mappingBitSize() const
{
    return PDO::mappingBitSize(_objectCurrentMapped);
}

int PDO::maxMappingObjectCount() const
{
    NodeIndex *mappingParam = _node->nodeOd()->index(_objectMappingId);
    if (!mappingParam)
    {
        return 0;
    }
    return mappingParam->subIndexesCount() - 1;
}

int PDO::mappingObjectCount() const
{
    return _objectCurrentMapped.count();
}

int PDO::mappingBitSize(const QList<NodeObjectId> &objectList)
{
    int totalBitSize = 0;
    for (const NodeObjectId &objId : objectList)
    {
        totalBitSize += objId.bitSize();
    }
    return totalBitSize;
}

int PDO::indexAtBitPos(int bitPos) const
{
    return PDO::indexAtBitPos(_objectCurrentMapped, bitPos);
}

int PDO::indexAtBitPos(const QList<NodeObjectId> &objectList, int bitPos)
{
    int totalBitSize = 0;
    int index = 0;
    for (const NodeObjectId &objId : objectList)
    {
        int bitSize = objId.bitSize();
        if (bitPos >= totalBitSize && bitPos < (totalBitSize + bitSize))
        {
            return index;
        }
        totalBitSize += bitSize;
        index++;
    }
    if (bitPos == totalBitSize)
    {
        return index;
    }
    return -1;
}

/**
 * @brief Start to read all settings (Comm and Mapping) from device
 */
void PDO::readMapping()
{
    _statusPdo = STATE_READ;
    _objectIdFsm = 0;
    readCommParam();
}

void PDO::writeMapping(const QList<NodeObjectId> &objectList)
{
    quint8 size = 0;
    for (const NodeObjectId &objectId : qAsConst(objectList))
    {
        size += objectId.bitSize();
        if (size > 64)
        {
            setError(ERROR_EXCEED_PDO_LENGTH);
            return;
        }
    }
    _objectToMap = objectList;
    for (NodeObjectId &objectId : _objectToMap)
    {
        if (objectId.dataType() == QMetaType::Type::UnknownType)
        {
            objectId.setDataType(_node->nodeOd()->dataType(objectId));
        }
    }

    _statusPdo = STATE_WRITE;
    _stateMapping = STATE_FREE;
    processMapping();
}

bool PDO::isEnabled() const
{
    NodeObjectId object(_objectCommId, PDO_COMM_COB_ID);
    quint32 cobIb = _node->nodeOd()->value(object).toUInt();

    if ((cobIb & COBID_VALID_NOT_VALID) == COBID_VALID_NOT_VALID)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void PDO::setEnabled(bool enabled)
{
    _statusPdo = STATE_NONE;
    NodeObjectId object(_objectCommId, PDO_COMM_COB_ID);
    if (enabled)
    {
        _node->writeObject(object.index(), object.subIndex(), QVariant(_node->nodeOd()->value(_objectCommList[PDO_COMM_COB_ID]).toUInt() & COBID_MASK));
    }
    else
    {
        _node->writeObject(object.index(), object.subIndex(), QVariant(_node->nodeOd()->value(_objectCommList[PDO_COMM_COB_ID]).toUInt() | COBID_VALID_NOT_VALID));
    }
}

qreal PDO::inhibitTimeMs() const
{
    NodeObjectId object(_objectCommId, PDO_COMM_INHIBIT_TIME);
    return _node->nodeOd()->value(object).toUInt() / 10.0;
}

void PDO::setInhibitTimeMs(qreal inhibitTimeMs)
{
    _statusPdo = STATE_NONE;
    _waitingConf.inhibitTime = inhibitTimeMs * 10.0;
    _node->writeObject(_objectCommList[2].index(), PDO_COMM_INHIBIT_TIME, _waitingConf.inhibitTime);
}

quint32 PDO::eventTimerMs() const
{
    NodeObjectId object(_objectCommId, PDO_COMM_EVENT_TIMER);
    return _node->nodeOd()->value(object).toUInt();
}

void PDO::setEventTimerMs(quint32 eventTimerMs)
{
    _statusPdo = STATE_NONE;
    _waitingConf.eventTimer = eventTimerMs;
    _node->writeObject(_objectCommList[3].index(), PDO_COMM_EVENT_TIMER, _waitingConf.eventTimer);
}

/**
 * @brief management response from device after setInhibitTime, setEventTimer, setTransmissionType and setSyncStartValue
 */
void PDO::managementRespWriteCommParam(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags == SDO::FlagsRequest::Error)
    {
        if (objId.subIndex() == PDO_COMM_TRANSMISSION_TYPE)
        {
            _waitingConf.transType = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }
        else if (objId.subIndex() == PDO_COMM_INHIBIT_TIME)
        {
            _waitingConf.inhibitTime = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }
        else if (objId.subIndex() == PDO_COMM_EVENT_TIMER)
        {
            _waitingConf.eventTimer = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }
        else if (objId.subIndex() == PDO_COMM_SYNC_START_VALUE)
        {
            _waitingConf.syncStartValue = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }

        /*qDebug() << ">PDO::odNotify : Index:SubIndex"
                 << QString("0x%1").arg(QString::number(objId.index(), 16))
                 << ":" << objId.subIndex()
                 << ", Error : " << _node->nodeOd()->errorObject(objId);*/
        setError(ERROR_WRITE_PARAM);
        return;
    }
}

/**
 * @brief management response from device after readCommParam and readMappingParam
 */
void PDO::managementRespReadCommAndMapping(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index() == _objectCommId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            /*qDebug() << ">PDO::odNotify : Index:SubIndex"
                     << QString("0x%1").arg(QString::number(objId.index(), 16))
                     << ":" << objId.subIndex()
                     << ", Error : " << _node->nodeOd()->errorObject(objId);*/

            setError(ERROR_GENERAL_ERROR);
            _objectIdFsm++;
        }
        else
        {
            if (objId.subIndex() == PDO_COMM_TRANSMISSION_TYPE)
            {
                _waitingConf.transType = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex() == PDO_COMM_INHIBIT_TIME)
            {
                _waitingConf.inhibitTime = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex() == PDO_COMM_EVENT_TIMER)
            {
                _waitingConf.eventTimer = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex() == PDO_COMM_SYNC_START_VALUE)
            {
                _waitingConf.syncStartValue = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
        }
        _objectIdFsm++;
        NodeObjectId objectComm(_objectCommList.at(0));
        if (_objectIdFsm > (static_cast<quint8>(_node->nodeOd()->value(objectComm).toUInt()) - 1))
        {
            _objectIdFsm = 0;
            readMappingParam();
            return;
        }
        readCommParam();
    }

    if (objId.index() == _objectMappingId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            /*qDebug() << ">PDO::odNotify : Index:SubIndex"
                     << QString("0x%1").arg(QString::number(objId.index(), 16))
                     << ":" << objId.subIndex()
                     << ", Error : " << _node->nodeOd()->errorObject(objId);*/
            setError(ERROR_GENERAL_ERROR);
            _stateMapping = STATE_FREE;
            return;
        }

        NodeObjectId objectComm(_node->busId(), _node->nodeId(), _objectMappingId, 0);
        if (_objectIdFsm > static_cast<quint8>(_node->nodeOd()->value(objectComm).toUInt()))
        {
            _objectIdFsm = 0;
            _statusPdo = STATE_NONE;
            _stateMapping = STATE_FREE;
            createListObjectMapped();
            return;
        }
        readMappingParam();
    }
}

/**
 * @brief Read Comm Param from device
 */
void PDO::readCommParam()
{
    if (_objectIdFsm < _objectCommList.size())
    {
        _node->readObject(_objectCommList[_objectIdFsm]);
    }
}

/**
 * @brief Read Mapping Param from device
 */
void PDO::readMappingParam()
{
    NodeObjectId objectId(_node->busId(), _node->nodeId(), _objectMappingId, _objectIdFsm);
    _node->readObject(objectId);
    _objectIdFsm++;
}

bool PDO::createListObjectMapped()
{
    if (!_node->nodeOd()->indexExist(_objectCommList[0].index()))
    {
        return false;
    }
    if ((_node->nodeOd()->value(_objectCommList[0]).toUInt() & COBID_VALID_NOT_VALID) == COBID_VALID_NOT_VALID)
    {
        setError(ERROR_COBID_NOT_VALID);
        return false;
    }
    if (isRPDO())
    {
        clearDataWaiting();
    }
    _objectCurrentMapped.clear();
    NodeObjectId objectMapping(_objectMappingId, 0);
    quint8 numberEntries = static_cast<quint8>(_node->nodeOd()->value(objectMapping).toUInt());

    for (quint8 i = 1; i <= numberEntries; i++)
    {
        NodeObjectId objectId(_node->busId(), _node->nodeId(), _objectMappingId, i);

        quint32 mapping = _node->nodeOd()->value(objectId).toUInt();
        quint8 subIndexMapping = (mapping & PDO_SUBINDEX_MASK) >> 8;
        quint16 indexMapping = mapping >> 16;

        if (checkIndex(indexMapping))
        {
            NodeObjectId object(_node->busId(), _node->nodeId(), indexMapping, subIndexMapping, _node->nodeOd()->dataType(indexMapping, subIndexMapping));
            _objectCurrentMapped.append(object);
        }
    }

    if (_node->nodeOd()->indexExist(objectMapping.index()))
    {
        numberEntries = static_cast<quint8>(_objectCurrentMapped.size());
        _node->nodeOd()->index(objectMapping.index())->subIndex(objectMapping.subIndex())->setValue(numberEntries);
    }
    emit mappingChanged();
    return true;
}

bool PDO::checkIndex(quint16 index)
{
    if (index != 0)
    {
        return true;
    }
    return false;
}
/**
 * @brief management response from device after processMapping
 */
void PDO::managementRespProcessMapping(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index() == _objectCommId) && (_stateMapping == STATE_FREE))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            setError(ERROR_DEACTIVATE_COBID);
            _statusPdo = STATE_NONE;
            _stateMapping = STATE_FREE;
            return;
        }
        _stateMapping = STATE_DEACTIVATE;
        processMapping();
    }
    if (objId.index() == _objectMappingId)
    {
        if (_stateMapping == STATE_DEACTIVATE)
        {
            _objectIdFsm = 0;
            if (_objectToMap.isEmpty())
            {
                _stateMapping = STATE_MODIFY;
            }
            else
            {
                _stateMapping = STATE_DISABLE;
            }
            processMapping();
        }
        if (_stateMapping == STATE_DISABLE)
        {
            if (flags == SDO::FlagsRequest::Error)
            {
                // ERROR so cobId is invalid and mapping is disable
                qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index(), 16)) << ":" << objId.subIndex()
                         << ", Error : " << _node->nodeOd()->errorObject(objId);
                setError(ERROR_MODIFY_MAPPING);
                _stateMapping = STATE_FREE;
                return;
            }
            _objectIdFsm++;
            if (_objectIdFsm == _objectToMap.size())
            {
                _stateMapping = STATE_MODIFY;
            }
            processMapping();
        }
        if (_stateMapping == STATE_MODIFY)
        {
            if (flags == SDO::FlagsRequest::Error)
            {
                // ERROR so cobId is invalid and mapping is disable
                qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index(), 16)) << ":" << objId.subIndex()
                         << ", Error : " << _node->nodeOd()->errorObject(objId);
                setError(ERROR_GENERAL_ERROR);
                _stateMapping = STATE_FREE;
                return;
            }
            _stateMapping = STATE_ENABLE;
            processMapping();
        }
    }
    if ((objId.index() == _objectCommId) && objId.subIndex() == 0x01 && (_stateMapping == STATE_ENABLE))
    {
        _stateMapping = STATE_ACTIVATE;
        processMapping();
    }
}

void PDO::processMapping()
{
    if (_node->status() == Node::Status::STOPPED || _node->status() == Node::Status::INIT)
    {
        return;
    }

    switch (_stateMapping)
    {
    case STATE_FREE:
        // Deactivate the PDO
        _node->writeObject(
            _objectCommList[PDO_COMM_COB_ID].index(), _objectCommList[PDO_COMM_COB_ID].subIndex(), QVariant(_node->nodeOd()->value(_objectCommList[PDO_COMM_COB_ID]).toUInt() | COBID_VALID_NOT_VALID));
        break;

    case STATE_DEACTIVATE:
        // Disable the mapping
        if (((_node->nodeOd()->value(_objectCommList[PDO_COMM_COB_ID]).toUInt() & COBID_VALID_NOT_VALID) == COBID_VALID_NOT_VALID))
        {
            _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(0)));
        }
        else
        {
            setError(ERROR_DISABLE_MAPPING);
            _stateMapping = STATE_FREE;
        }
        break;

    case STATE_DISABLE:
        // Modify the mapping
        if (_objectIdFsm < _objectToMap.size())
        {
            quint32 map = 0;
            map = static_cast<quint32>(_objectToMap.at(_objectIdFsm).index() << 16);
            map = map | (static_cast<quint16>(_objectToMap.at(_objectIdFsm).subIndex() << 8));
            map = map | _objectToMap.at(_objectIdFsm).bitSize();
            _node->writeObject(_objectMappingId, _objectIdFsm + 1, map);
        }
        else
        {
            _node->writeObject(_objectMappingId, _objectIdFsm + 1, 0);
        }
        break;

    case STATE_MODIFY:
        // Enable the mapping
        _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(_objectToMap.size())));
        break;

    case STATE_ENABLE:
        // Activate the PDO
        _node->writeObject(
            _objectCommList[PDO_COMM_COB_ID].index(), _objectCommList[PDO_COMM_COB_ID].subIndex(), QVariant(_node->nodeOd()->value(_objectCommList[PDO_COMM_COB_ID]).toInt() & COBID_MASK));
        break;

    case STATE_ACTIVATE:
        _statusPdo = STATE_NONE;
        _stateMapping = STATE_FREE;
        _objectToMap.clear();
        _objectIdFsm = 0;
        createListObjectMapped();
        break;
    }
}

void PDO::setError(ErrorPdo error)
{
    emit errorOccurred(error);
}

void PDO::clearDataWaiting()
{
}
