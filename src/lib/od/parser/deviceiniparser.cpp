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
    : _file(file)
{
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
    QRegularExpression reIndex(QStringLiteral("^[0-9A-F]{1,4}$"));
    for (const QString &group : _file->childGroups())
    {
        bool ok = false;
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
    QRegularExpression reSub(QStringLiteral("^([0-9A-F]{4})sub([0-9A-F]+)"));
    for (const QString &group : _file->childGroups())
    {
        bool ok = false;
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

            if (numIndex == 0x2040 && subIndex->subIndex() == 1)  // Communication_config.Node_ID
            {
                subIndex->setValue(0);
            }

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
    uint8_t objectType = 0;
    uint8_t maxSubIndex = 0;
    QString name;

    for (const QString &key : _file->allKeys())
    {
        bool ok = false;
        QString value = _file->value(key).toString();

        uint8_t base = 10;
        if (value.startsWith(QStringLiteral("0x"), Qt::CaseInsensitive))
        {
            base = 16;
        }

        if (key == QStringLiteral("ObjectType"))
        {
            objectType = static_cast<uint8_t>(value.toInt(&ok, base));
        }

        else if (key == QStringLiteral("ParameterName"))
        {
            name = value;
        }

        else if (key == QStringLiteral("SubNumber"))
        {
            maxSubIndex = static_cast<uint8_t>(value.toInt(&ok, base));
        }
    }

    SubIndex *subIndex = new SubIndex(static_cast<uint8_t>(0));
    readSubIndex(subIndex);

    index->setMaxSubIndex(maxSubIndex);
    index->setObjectType(static_cast<Index::Object>(objectType));
    index->setName(name);
    index->addSubIndex(subIndex);
}

/**
 * @brief parses an index field and completes sub-index model
 * @param sub-index model
 */
void DeviceIniParser::readSubIndex(SubIndex *subIndex) const
{
    bool hasNodeId = false;
    bool isHexValue = false;
    uint8_t accessType = 0;
    uint16_t dataType = SubIndex::INVALID;
    QString name;
    QVariant data;
    QVariant lowLimit;
    QVariant highLimit;
    uint32_t objFlags = 0;

    for (const QString &key : _file->allKeys())
    {
        QString value = _file->value(key).toString();

        if (key == QStringLiteral("AccessType"))
        {
            QString accessString = _file->value(key).toString();

            if (accessString == QStringLiteral("rw") || accessString == QStringLiteral("rwr") || accessString == QStringLiteral("rww"))
            {
                accessType += SubIndex::READ + SubIndex::WRITE;
            }
            else if (accessString == QStringLiteral("wo"))
            {
                accessType += SubIndex::WRITE;
            }
            else if (accessString == QStringLiteral("ro"))
            {
                accessType += SubIndex::READ;
            }
            else if (accessString == QStringLiteral("const"))
            {
                accessType += SubIndex::READ;
                accessType += SubIndex::CONST;
            }
        }
        else if (key == QStringLiteral("PDOMapping"))
        {
            accessType += readPdoMapping();
        }
        else if (key == QStringLiteral("ParameterName"))
        {
            name = value;
        }
        else if (key == QStringLiteral("LowLimit"))
        {
            lowLimit = readLowLimit();
        }
        else if (key == QStringLiteral("HighLimit"))
        {
            highLimit = readHighLimit();
        }
        else if (key == QStringLiteral("DataType"))
        {
            dataType = readDataType();
        }
        else if (key == QStringLiteral("ObjFlags"))
        {
            objFlags = readObjFlags();
        }

        data = readData(&hasNodeId, &isHexValue);
    }

    subIndex->setAccessType(static_cast<SubIndex::AccessType>(accessType));
    subIndex->setName(name);
    subIndex->setValue(data);
    subIndex->setDataType(static_cast<SubIndex::DataType>(dataType));
    subIndex->setLowLimit(lowLimit);
    subIndex->setHighLimit(highLimit);
    subIndex->setHasNodeId(hasNodeId);
    subIndex->setHexValue(isHexValue);
    subIndex->setObjFlags(objFlags);
}

/**
 * @brief read data to correct format from dcf or eds file
 * @param dcf or eds file
 * @return data
 */
QVariant DeviceIniParser::readData(bool *nodeId, bool *isHexValue) const
{
    QString stringValue;

    if (_file->value(QStringLiteral("DefaultValue")).isNull())
    {
        stringValue = QStringLiteral("");
    }
    else if (_file->value(QStringLiteral("DefaultValue")).toString().startsWith(QStringLiteral("$NODEID")))
    {
        stringValue = _file->value(QStringLiteral("DefaultValue")).toString().mid(8);
        if (stringValue.isEmpty())
        {
            stringValue = QStringLiteral("0");
        }
        *nodeId = true;
    }
    else
    {
        stringValue = _file->value(QStringLiteral("DefaultValue")).toString();
    }

    uint16_t dataType = readDataType();

    int base = 0;
    if (stringValue.startsWith(QStringLiteral("0x")))
    {
        base = 16;
        *isHexValue = true;
    }
    else
    {
        base = 10;
        *isHexValue = false;
    }

    if (stringValue.isEmpty())
    {
        return QVariant();
    }

    bool ok = false;
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
        case SubIndex::UNICODE_STRING:
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
    if (_file->value(QStringLiteral("PDOMapping")) == 0)
    {
        return 0;
    }

    QString accessString = _file->value(QStringLiteral("AccessType")).toString();

    if (accessString == QStringLiteral("rwr") || accessString == QStringLiteral("ro") || accessString == QStringLiteral("const"))
    {
        return SubIndex::TPDO;
    }

    if (accessString == QStringLiteral("rww") || accessString == QStringLiteral("wo"))
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
    return QVariant(_file->value(QStringLiteral("LowLimit")));
}

/**
 * @brief parses high limit value and returns it
 * @return high limit value
 */
QVariant DeviceIniParser::readHighLimit() const
{
    return QVariant(_file->value(QStringLiteral("HighLimit")));
}

/**
 * @brief parses data type value and returns it
 * @return 16 bits data type code^
 */
uint16_t DeviceIniParser::readDataType() const
{
    QString dataType = _file->value(QStringLiteral("DataType")).toString();

    int base = 10;
    if (dataType.startsWith(QStringLiteral("0x")))
    {
        base = 16;
    }

    bool ok = false;
    return static_cast<uint16_t>(dataType.toInt(&ok, base));
}

uint32_t DeviceIniParser::readObjFlags() const
{
    QString objFlags = _file->value(QStringLiteral("ObjFlags")).toString();

    int base = 10;
    if (objFlags.startsWith(QStringLiteral("0x")))
    {
        base = 16;
    }

    bool ok = false;
    return static_cast<uint32_t>(objFlags.toInt(&ok, base));
}
