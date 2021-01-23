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

#include "deviceiniparser.h"

#include <QDebug>
#include <QLocale>
#include <QRegularExpression>

/**
 * @brief constructor
 * @param file to parse
 */
DeviceIniParser::DeviceIniParser(QSettings *file)
{
    _file = file;
}

/**
 * @brief parsesall indexes and sub-indexes fields and completes device model
 * @param device model
 */
void DeviceIniParser::readObjects(DeviceModel *deviceModel) const
{
    readIndexes(deviceModel);
    readSubIndexes(deviceModel);
}

/**
 * @brief parses all indexes fields and completes device model
 * @param device model
 */
void DeviceIniParser::readIndexes(DeviceModel *deviceModel) const
{
    QRegularExpression reIndex("^[0-9A-F]{1,4}$");
    for (const QString &group : _file->childGroups())
    {
        bool ok;
        uint16_t numIndex = 0;
        QRegularExpressionMatch matchIndex = reIndex.match(group);

        if (matchIndex.hasMatch())
        {
            QString matchedIndex = matchIndex.captured(0);
            numIndex = static_cast<uint16_t>(matchedIndex.toInt(&ok, 16));
            Index *index = new Index(numIndex);

            _file->beginGroup(group);
            readIndex(index);
            _file->endGroup();

            deviceModel->addIndex(index);
        }
    }
}

/**
 * @brief parses all sub-indexes fields and completes device model
 * @param device model
 */
void DeviceIniParser::readSubIndexes(DeviceModel *deviceModel) const
{
    QRegularExpression reSub("^([0-9A-F]{4})sub([0-9A-F]+)");
    for (const QString &group : _file->childGroups())
    {
        bool ok;
        uint8_t numSubIndex = 0;
        QRegularExpressionMatch matchSub = reSub.match(group);

        if (matchSub.hasMatch())
        {
            QString matchedSub = matchSub.captured(2);
            numSubIndex = static_cast<uint8_t>(matchedSub.toShort(&ok, 16));
            SubIndex *subIndex = new SubIndex(numSubIndex);

            _file->beginGroup(group);
            readSubIndex(subIndex);
            _file->endGroup();

            matchedSub = matchSub.captured(1);
            uint16_t numIndex = static_cast<uint16_t>(matchedSub.toUInt(&ok, 16));

            if (deviceModel->indexExist(numIndex))
            {
                Index *index = deviceModel->index(numIndex);
                index->addSubIndex(subIndex);
            }
        }
    }
}

/**
 * @brief parses an index field and completes index model
 * @param index model
 */
void DeviceIniParser::readIndex(Index *index) const
{
    uint8_t objectType;
    uint8_t maxSubIndex;
    QString name;

    for (const QString &key : _file->allKeys())
    {
        bool ok;
        QString value = _file->value(key).toString();

        uint8_t base = 10;
        if (value.startsWith("0x", Qt::CaseInsensitive))
        {
            base = 16;
        }

        if (key == "ObjectType")
        {
            objectType = static_cast<uint8_t>(value.toInt(&ok, base));
        }

        else if (key == "ParameterName")
        {
            name = value;
        }

        else if (key == "SubNumber")
        {
            maxSubIndex = static_cast<uint8_t>(value.toInt(&ok, base));
        }
    }

    SubIndex *subIndex = new SubIndex(static_cast<uint8_t>(0));
    readSubIndex(subIndex);

    index->setMaxSubIndex(maxSubIndex);
    index->setObjectType(objectType);
    index->setName(name);
    index->addSubIndex(subIndex);
}

/**
 * @brief parses an index field and completes sub-index model
 * @param sub-index model
 */
void DeviceIniParser::readSubIndex(SubIndex *subIndex) const
{
    bool hasNodeId = 0;
    uint8_t accessType = 0;
    uint16_t dataType = SubIndex::INVALID;
    QString name;
    QVariant data;
    QVariant lowLimit;
    QVariant highLimit;

    for (const QString &key : _file->allKeys())
    {
        QString value = _file->value(key).toString();

        if (key == "AccessType")
        {
            QString accessString = _file->value(key).toString();

            if (accessString == "rw" || accessString == "rwr" || accessString == "rww")
            {
                accessType += SubIndex::READ + SubIndex::WRITE;
            }

            else if (accessString == "wo")
            {
                accessType += SubIndex::WRITE;
            }

            else if (accessString == "ro" || accessString == "const")
            {
                accessType += SubIndex::READ;
            }
        }

        else if (key == "PDOMapping")
        {
            accessType += readPdoMapping();
        }

        else if (key == "ParameterName")
        {
            name = value;
        }

        else if (key == "LowLimit")
        {
            lowLimit = readLowLimit();
        }

        else if (key == "HighLimit")
        {
            highLimit = readHighLimit();
        }

        else if (key == "DataType")
        {
            dataType = readDataType();
        }

        data = readData(&hasNodeId);
    }

    subIndex->setAccessType(static_cast<SubIndex::AccessType>(accessType));
    subIndex->setName(name);
    subIndex->setValue(data);
    subIndex->setDataType(static_cast<SubIndex::DataType>(dataType));
    subIndex->setLowLimit(lowLimit);
    subIndex->setHighLimit(highLimit);
    subIndex->setHasNodeId(hasNodeId);
}

/**
 * @brief read data to correct format from dcf or eds file
 * @param dcf or eds file
 * @return data
 */
QVariant DeviceIniParser::readData(bool *nodeId) const
{
    QString stringValue;

    if (_file->value("DefaultValue").isNull())
    {
        stringValue = "";
    }
    else if (_file->value("DefaultValue").toString().startsWith("$NODEID+"))
    {
        stringValue = _file->value("DefaultValue").toString().mid(8);
        *nodeId = true;
    }

    else
    {
        stringValue = _file->value("DefaultValue").toString();
    }

    uint16_t dataType = readDataType();

    int base = 10;
    if (stringValue.startsWith("0x"))
    {
        base = 16;
    }

    if (stringValue.isEmpty())
    {
        return QVariant();
    }

    bool ok;
    switch (dataType)
    {
    case SubIndex::BOOLEAN:
    case SubIndex::INTEGER8:
    case SubIndex::INTEGER16:
    case SubIndex::INTEGER32:
        return QVariant(stringValue.toInt(&ok, base));

    case SubIndex::INTEGER64:
        return QVariant(stringValue.toLongLong(&ok, base));

    case SubIndex::UNSIGNED8:
    case SubIndex::UNSIGNED16:
    case SubIndex::UNSIGNED32:
        return QVariant(stringValue.toUInt(&ok, base));

    case SubIndex::UNSIGNED64:
        return QVariant(stringValue.toULongLong(&ok, base));

    case SubIndex::REAL32:
        return QVariant(stringValue.toFloat());

    case SubIndex::REAL64:
        return QVariant(stringValue.toDouble());

    case SubIndex::VISIBLE_STRING:
    case SubIndex::OCTET_STRING:
        return QVariant(stringValue);
    }

    return QVariant();
}

/**
 * @brief parses file infos and completes device model
 * @param device model
 */
void DeviceIniParser::readFileInfo(DeviceModel *deviceModel) const
{
    for (const QString &key : _file->allKeys())
    {
        deviceModel->setFileInfo(key, _file->value(key).toString());
    }
}

/**
 * @brief parses dummy usages and completes device model
 * @param device model
 */
void DeviceIniParser::readDummyUsage(DeviceModel *deviceModel) const
{
    for (const QString &key : _file->allKeys())
    {
        deviceModel->setDummyUsage(key, _file->value(key).toString());
    }
}

void DeviceIniParser::readComments(DeviceModel *deviceModel) const
{
    for (const QString &key : _file->allKeys())
    {
        deviceModel->setComment(key, _file->value(key).toString());
    }
}

/**
 * @brief parses device infos and completes device description model
 * @param device description model
 */
void DeviceIniParser::readDeviceInfo(DeviceDescription *deviceDescription) const
{
    for (const QString &key : _file->allKeys())
    {
        deviceDescription->setDeviceInfo(key, _file->value(key).toString());
    }
}

/**
 * @brief parses device comissioning and completes device configuration
 * @param device configuration model
 */
void DeviceIniParser::readDeviceComissioning(DeviceConfiguration *deviceConfiguration) const
{
    for (const QString &key : _file->allKeys())
    {
        deviceConfiguration->addDeviceComissioning(key, _file->value(key).toString());
    }
}

/**
 * @brief parses pdo mapping value and returns it
 * @return 8 bits pdo mapping code
 */
uint8_t DeviceIniParser::readPdoMapping() const
{
    if (_file->value("PDOMapping") == 0)
    {
        return 0;
    }

    QString accessString = _file->value("AccessType").toString();

    if (accessString == "rwr" || accessString == "ro" || accessString == "const")
    {
        return SubIndex::TPDO;
    }

    if (accessString == "rww" || accessString == "wo")
    {
        return SubIndex::RPDO;
    }

    return SubIndex::TPDO + SubIndex::RPDO;
}

/**
 * @brief parses low limit value and returns it
 * @return low limit value
 */
QVariant DeviceIniParser::readLowLimit() const
{
    return QVariant(_file->value("LowLimit"));
}

/**
 * @brief parses high limit value and returns it
 * @return high limit value
 */
QVariant DeviceIniParser::readHighLimit() const
{
    return QVariant(_file->value("HighLimit"));
}

/**
 * @brief parses data type value and returns it
 * @return 16 bits data type code^
 */
uint16_t DeviceIniParser::readDataType() const
{
    QString dataType = _file->value("DataType").toString();

    int base = 10;
    if (dataType.startsWith("0x"))
    {
        base = 16;
    }

    bool ok;
    return static_cast<uint16_t>(dataType.toInt(&ok, base));
}
