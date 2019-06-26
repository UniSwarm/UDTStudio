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

#include "cdfparser.h"

#include <QRegularExpression>

/**
 * @brief default constructor
 */
CdfParser::CdfParser()
{

}

/**
 * @brief parses a cdf file and completes an object dictionary
 * @param input file path
 * @return object dictionary
 */
OD *CdfParser::parse(const QString &path) const
{
    bool ok;
    bool isSubIndex;
    uint16_t indexNumber;
    uint8_t accessType;
    uint16_t dataType;
    uint8_t objectType;
    uint8_t subNumber;
    QString accessString;
    QString parameterName;
    Index *index = nullptr;
    SubIndex *subIndex;

    OD *od;
    od = new OD;

    QSettings cdf(path, QSettings::IniFormat);
    QRegularExpression reSub("([0-Z]{4})(sub)([0-9]+)");
    QRegularExpression reIndex("([0-Z]{4})");

    foreach (const QString &group, cdf.childGroups())
    {
        isSubIndex = false;
        accessType = 0;
        dataType = 0;
        objectType = 0;
        subNumber = 0;

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
        else
            continue;

        cdf.beginGroup(group);

        foreach (const QString &key, cdf.allKeys())
        {
            QString value = cdf.value(key).toString();

            uint8_t base = 10;
            if (value.startsWith("0x", Qt::CaseInsensitive))
                base = 16;

            if (key == "AccessType")
            {
                accessString = cdf.value(key).toString();

                if (accessString == "rw")
                    accessType = SubIndex::Access::READ_WRITE;

                else if (accessString == "wo")
                    accessType = SubIndex::Access::WRITE_ONLY;

                else if (accessString == "ro")
                    accessType = SubIndex::Access::READ_ONLY;

                else if (accessString == "const")
                    accessType = SubIndex::Access::CONST;
            }

            else if (key == "DataStorage")
                dataType = (uint16_t)value.toInt(&ok, base);

            else if (key == "ObjectType")
                objectType = (uint8_t)value.toInt(&ok, base);

            else if (key == "ParameterName")
                parameterName = value;

            else if (key == "SubNumber")
                subNumber = (uint8_t)value.toInt(&ok, base);
        }

        DataStorage data = readData(cdf);
        cdf.endGroup();

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
                index->addSubIndex(subIndex);
            }

            od->addIndex(index);
        }
    }
    return od;
}

/**
 * @brief read data to correct format from cdf file
 * @param cdf file
 * @return data
 */
DataStorage CdfParser::readData(const QSettings &cdf) const
{
    bool ok;
    uint16_t dataType = cdf.value("DataType").toString().toShort(&ok, 16);

    QString dataString = cdf.value("DefaultValue").toString();
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
