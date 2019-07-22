/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "dcfparser.h"

#include <QRegularExpression>

//TODO handle fields CompactSubObj and ObjFlags (see 306_1)
//TODO handle octal format (0) (see 306_1)

/**
 * @brief default constructor
 */
DcfParser::DcfParser()
{

}

/**
 * @brief parses a dcf file and completes an object dictionary
 * @param input file path
 * @return object dictionary
 */
DeviceModel *DcfParser::parse(const QString &path) const
{
    bool ok;
    bool isSubIndex;
    uint16_t indexNumber;
    uint8_t accessType;
    uint8_t objectType;
    uint8_t subNumber;
    QString accessString;
    QString parameterName;
    Index *index = nullptr;
    SubIndex *subIndex;
    QVariant lowLimit;
    QVariant highLimit;
    uint8_t flagLimit;

    DeviceModel *od;
    od = new DeviceConfiguration;

    QSettings dcf(path, QSettings::IniFormat);
    QRegularExpression reSub("([0-Z]{4})(sub)([0-9]+)");
    QRegularExpression reIndex("([0-Z]{4})");

    foreach (const QString &group, dcf.childGroups())
    {
        isSubIndex = false;
        accessType = 0;
        objectType = 0;
        subNumber = 0;
        flagLimit = 0;

        QRegularExpressionMatch matchSub = reSub.match(group);
        QRegularExpressionMatch matchIndex = reIndex.match(group);

        if (matchSub.hasMatch())
        {
            QString matchedSub = matchSub.captured(1);
            index = od->index((uint16_t)matchedSub.toUInt(&ok, 16));
            isSubIndex = true;
            subNumber = (uint8_t)matchSub.captured(3).toShort(&ok, 10);
        }
        else if (matchIndex.hasMatch())
        {
            QString matchedIndex = matchIndex.captured(0);
            indexNumber = (uint16_t)matchedIndex.toInt(&ok, 16);
        }
        else if (group == "FileInfo")
        {
            dcf.beginGroup(group);
            readFileInfo(od, dcf);
            dcf.endGroup();
            continue;
        }
        else
            continue;

        dcf.beginGroup(group);

        foreach (const QString &key, dcf.allKeys())
        {
            QString value = dcf.value(key).toString();

            uint8_t base = 10;
            if (value.startsWith("0x", Qt::CaseInsensitive))
                base = 16;

            if (key == "AccessType")
            {
                accessString = dcf.value(key).toString();

                if (accessString == "rw" || accessString == "rwr" || accessString == "rww")
                    accessType += SubIndex::Access::READ + SubIndex::Access::WRITE;

                else if (accessString == "wo")
                    accessType += SubIndex::Access::WRITE;

                else if (accessString == "ro" || accessString == "const")
                    accessType += SubIndex::Access::READ;
            }

            else if (key == "ObjectType")
                objectType = (uint8_t)value.toInt(&ok, base);

            else if (key == "ParameterName")
                parameterName = value;

            else if (key == "SubNumber")
                subNumber = (uint8_t)value.toInt(&ok, base);

            else if (key == "PDOMapping")
                accessType += readPdoMapping(dcf);

            else if (key == "LowLimit")
            {
                lowLimit = readLowLimit(dcf);
                flagLimit += SubIndex::Limit::LOW;
            }

            else if (key == "HighLimit")
            {
                highLimit = readHighLimit(dcf);
                flagLimit += SubIndex::Limit::HIGH;
            }
        }

        DataStorage data = readData(dcf);
        dcf.endGroup();

        if (isSubIndex)
        {
            switch (index->objectType())
            {
            case Index::Object::RECORD:
            case Index::Object::ARRAY:
                subIndex = new SubIndex(subNumber);
                subIndex->setAccessType(accessType);
                subIndex->setName(parameterName);
                subIndex->setData(data);
                subIndex->setFlagLimit(flagLimit);

                if (flagLimit & SubIndex::Limit::LOW)
                    subIndex->setLowLimit(lowLimit);

                if (flagLimit & SubIndex::Limit::HIGH)
                    subIndex->setHighLimit(highLimit);

                index->addSubIndex(subIndex);
                break;
            }
        }
        else
        {
            index = new Index(indexNumber);
            index->setObjectType(objectType);
            index->setName(parameterName);
            index->setMaxSubIndex(subNumber);

            if (objectType == Index::Object::VAR)
            {
                subIndex = new SubIndex(0);
                subIndex->setAccessType(accessType);
                subIndex->setName(parameterName);
                subIndex->setData(data);
                subIndex->setFlagLimit(flagLimit);

                if (flagLimit & SubIndex::Limit::LOW)
                    subIndex->setLowLimit(lowLimit);

                if (flagLimit & SubIndex::Limit::HIGH)
                    subIndex->setHighLimit(highLimit);

                index->addSubIndex(subIndex);
            }
            od->addIndex(index);
        }
    }
    return od;
}

/**
 * @brief read data to correct format from dcf file
 * @param dcf file
 * @return data
 */
DataStorage DcfParser::readData(const QSettings &dcf) const
{
    bool ok;
    uint16_t dataType = (uint16_t)dcf.value("DataType").toString().toShort(&ok, 16);

    QString dataString = dcf.value("DefaultValue").toString();
    uint8_t base = 10;
    if (dataString.startsWith("0x", Qt::CaseInsensitive))
        base = 16;

    DataStorage data;
    data.setDataType(dataType);
    switch(dataType)
    {
    case DataStorage::Type::BOOLEAN:
        if (dataString.toShort(&ok, base))
            data.setValue(true);
        else
            data.setValue(false);
        break;

    case DataStorage::Type::INTEGER8:
        data.setValue(dataString.toShort(&ok, base));
        break;

    case DataStorage::Type::INTEGER16:
        data.setValue(dataString.toShort(&ok, base));
        break;

    case DataStorage::Type::INTEGER32:
        data.setValue(dataString.toInt(&ok, base));
        break;

    case DataStorage::Type::INTEGER64:
        data.setValue(dataString.toInt(&ok, base));
        break;

    case DataStorage::Type::UNSIGNED8:
        data.setValue(dataString.toInt(&ok, base));
        break;

    case DataStorage::Type::UNSIGNED16:
        data.setValue(dataString.toUShort(&ok, base));
        break;

    case DataStorage::Type::UNSIGNED32:
        data.setValue(dataString.toUInt(&ok, base));
        break;

    case DataStorage::Type::UNSIGNED64:
        data.setValue(dataString.toUInt(&ok, base));
        break;

    case DataStorage::Type::REAL32:
        data.setValue(dataString.toFloat(&ok));
        break;

    case DataStorage::Type::REAL64:
        data.setValue(dataString.toDouble(&ok));
        break;

    case DataStorage::Type::VISIBLE_STRING:
        data.setValue(dataString);
        break;

    case DataStorage::Type::OCTET_STRING:
        data.setValue(dataString);
        break;
    }

    return data;
}

void DcfParser::readFileInfo(DeviceModel *od, const QSettings &dcf) const
{
    foreach (const QString &key, dcf.allKeys())
    {
       od->setFileInfo(key, dcf.value(key).toString());
    }
}

uint8_t DcfParser::readPdoMapping(const QSettings &dcf) const
{
    if (dcf.value("PDOMapping") == 0)
        return 0;

    QString accessString = dcf.value("AccessType").toString();

    if (accessString == "rwr" || accessString == "ro" || accessString == "const")
        return SubIndex::Access::TPDO;

    if (accessString == "rww" || accessString == "wo")
        return SubIndex::Access::RPDO;

    return SubIndex::Access::TPDO + SubIndex::Access::RPDO;
}

QVariant DcfParser::readLowLimit(const QSettings &dcf) const
{
    return QVariant(dcf.value("LowLimit"));
}

QVariant DcfParser::readHighLimit(const QSettings &dcf) const
{
    return QVariant(dcf.value("HighLimit"));
}
