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

#include "fastdatalogger.h"

#include <QDataStream>
#include <QDebug>

FastDataLogger::FastDataLogger(Node *node)
    : _node(node)
{
    registerIndex(0x2100);  // register all status objects
    setNodeInterrest(node);

    _dataType1 = SourceDataInvalid;
    _dataType2 = SourceDataInvalid;

    _status = StatusOff;
}

Node *FastDataLogger::node() const
{
    return _node;
}

FastDataLoggerConfig &FastDataLogger::config()
{
    return _config;
}

const FastDataLoggerConfig &FastDataLogger::config() const
{
    return _config;
}

void FastDataLogger::start()
{
    writeObject(0x2101, 0x01, 1);
    readObject(0x2100, 0x01);  // trigger read status
}

void FastDataLogger::stop()
{
    writeObject(0x2101, 0x01, 0);
}

void FastDataLogger::commitConfig()
{
    writeObject(0x2101, 0x02, objIdToU32(_config.data1_objId()));
    writeObject(0x2101, 0x03, objIdToU32(_config.data2_objId()));
    writeObject(0x2101, 0x04, _config.frequencyDivider());
    writeObject(0x2101, 0x05, objIdToU32(_config.trigger_objId()));
    writeObject(0x2101, 0x06, (uint8_t)_config.triggerType());
    writeObject(0x2101, 0x07, _config.triggerValue());

    _dataType1 = typeFromObjId(_config.data1_objId());
    _dataType2 = typeFromObjId(_config.data2_objId());

    readObject(0x2100, 0x01);  // trigger read status
}

const QList<qreal> &FastDataLogger::valuesData1() const
{
    return _valuesData1;
}

const QList<qreal> &FastDataLogger::valuesData2() const
{
    return _valuesData2;
}

uint32_t FastDataLogger::objIdToU32(const NodeObjectId &objId)
{
    return ((uint32_t)objId.index() << 16) + ((uint32_t)objId.subIndex() << 8);
}

NodeObjectId FastDataLogger::u32ToObjId(uint32_t u32)
{
    return NodeObjectId((quint16)u32 >> 16, (quint8)u32 >> 8);
}

QList<qreal> FastDataLogger::byteArrayToReals(const QByteArray &byteArray, SourceDataType dataType)
{
    QList<qreal> reals;

    QDataStream stream(byteArray);
    stream.setByteOrder(QDataStream::LittleEndian);

    while (!stream.atEnd())
    {
        qreal valueDouble;
        switch (dataType)
        {
            case FastDataLogger::SourceDataInvalid:
                return reals;

            case FastDataLogger::SourceDataU8:
            {
                uint8_t value;
                stream >> value;
                valueDouble = value;
                break;
            }

            case FastDataLogger::SourceDataI8:
            {
                int8_t value;
                stream >> value;
                valueDouble = value;
                break;
            }

            case FastDataLogger::SourceDataU16:
            {
                uint16_t value;
                stream >> value;
                valueDouble = value;
                break;
            }

            case FastDataLogger::SourceDataI16:
            {
                int16_t value;
                stream >> value;
                valueDouble = value;
                break;
            }

            case FastDataLogger::SourceDataU32:
            {
                uint32_t value;
                stream >> value;
                valueDouble = value;
                break;
            }

            case FastDataLogger::SourceDataI32:
            {
                int32_t value;
                stream >> value;
                valueDouble = value;
                break;
            }

            case FastDataLogger::SourceDataFloat:
            {
                float value;
                stream >> value;
                valueDouble = value;
                break;
            }
        }

        // if (Q15)
        //{
        // valueDouble /= 65536.0;
        //}
        valueDouble /= 100.0;

        reals.append(valueDouble);
    }

    return reals;
}

FastDataLogger::Status FastDataLogger::status() const
{
    return _status;
}

QString FastDataLogger::statusStr() const
{
    switch (_status)
    {
        case FastDataLogger::StatusInvalidTrigger:
            return tr("Invalid trigger");

        case FastDataLogger::StatusInvalidData:
            return tr("Invalid data");

        case FastDataLogger::StatusOff:
            return tr("Off");

        case FastDataLogger::StatusWaitingForTrigger:
            return tr("Waiting for trigger...");

        case FastDataLogger::StatusLogging:
            return tr("Logging...");

        case FastDataLogger::StatusDataReady:
            return tr("Data ready");
    }
    return QString();
}

FastDataLogger::SourceDataType FastDataLogger::typeFromObjId(const NodeObjectId &data_objId)
{
    NodeSubIndex *subIndex = nodeInterrest()->nodeOd()->subIndex(data_objId);

    if (!subIndex)
    {
        return SourceDataInvalid;
    }

    switch (subIndex->dataType())
    {
        case NodeSubIndex::UNSIGNED8:
            return SourceDataU8;

        case NodeSubIndex::INTEGER8:
            return SourceDataI8;

        case NodeSubIndex::UNSIGNED16:
            return SourceDataU16;

        case NodeSubIndex::INTEGER16:
            return SourceDataI16;

        case NodeSubIndex::UNSIGNED32:
            return SourceDataU32;

        case NodeSubIndex::INTEGER32:
            return SourceDataI32;

        case NodeSubIndex::REAL32:
            return SourceDataFloat;

        default:
            return SourceDataInvalid;
    }
}

void FastDataLogger::setStatus(Status status)
{
    if (status != _status)
    {
        _status = status;
        emit statusChanged(_status);
    }
}

void FastDataLogger::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::Error)
    {
        // TODO manage errors
        return;
    }

    if (objId.index() == 0x2100)
    {
        switch (objId.subIndex())
        {
            case 1:  // Status
                setStatus(static_cast<Status>(_node->nodeOd()->value(objId).toUInt()));

                if (_status == StatusDataReady)
                {
                    readObject(0x2100, 0x02);  // read data 1
                }
                break;

            case 2:  // Data 1
                _valuesData1 = byteArrayToReals(_node->nodeOd()->value(objId).toByteArray(), _dataType1);
                if (_dataType2 != SourceDataInvalid)
                {
                    readObject(0x2100, 0x03);  // read data 2
                }
                else
                {
                    emit dataAvailable();
                }
                break;

            case 3:  // Data 2
                _valuesData2 = byteArrayToReals(_node->nodeOd()->value(objId).toByteArray(), _dataType2);
                emit dataAvailable();
                break;
        }
    }
}
