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
OD* CdfParser::parse(QString path)
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
    DataType *data;

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
        data = nullptr;

        QRegularExpressionMatch matchSub = reSub.match(group);
        QRegularExpressionMatch matchIndex = reIndex.match(group);

        if (matchSub.hasMatch())
        {
            QString matchedSub = matchSub.captured(1);
            index = od->index(matchedSub.toInt(&ok, 16));
            isSubIndex = true;
            subNumber = matchSub.captured(3).toShort(&ok, 10);
        }
        else if (matchIndex.hasMatch())
        {
            QString matchedIndex = matchIndex.captured(0);
            indexNumber = matchedIndex.toInt(&ok, 16);
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

            else if (key == "DataType")
                dataType = value.toInt(&ok, base);

            else if (key == "ObjectType")
                objectType = value.toInt(&ok, base);

            else if (key == "ParameterName")
                parameterName = value;

            else if (key == "SubNumber")
                subNumber = value.toInt(&ok, base);
        }

        data = readData(cdf);
        cdf.endGroup();

        if (isSubIndex)
        {
            switch (index->objectType())
            {
            case SubIndex::Object::RECORD:
                subIndex = new SubIndex(dataType, objectType, accessType, parameterName);
                subIndex->setSubNumber(subNumber);
                subIndex->addData(data);
                index->addSubIndex(subIndex);
                break;

            case SubIndex::Object::ARRAY:
                index->setDataType(dataType);
                index->setAccessType(accessType);
                index->addData(data);
                break;
            }
        }
        else
        {
            index = new Index(dataType, objectType, accessType, parameterName, indexNumber, subNumber);

            if (objectType == SubIndex::Object::VAR)
                index->addData(data);

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
DataType* CdfParser::readData(const QSettings &cdf) const
{
    bool ok;
    int16_t dataType = cdf.value("DataType").toString().toShort(&ok, 16);

    QString dataString = cdf.value("DefaultValue").toString();
    uint8_t base = 10;
    if (dataString.startsWith("0x", Qt::CaseInsensitive))
        base = 16;

    DataType *data = nullptr;
    switch(dataType)
    {
    case DataType::Type::BOOLEAN:
        if (dataString.toShort(&ok, base))
            data = new DataType(true);
        else
            data = new DataType(false);
        break;

    case DataType::Type::INTEGER8:
        data = new DataType(dataString.toShort(&ok, base));
        break;

    case DataType::Type::INTEGER16:
        data = new DataType(dataString.toShort(&ok, base));
        break;

    case DataType::Type::INTEGER32:
        data = new DataType(dataString.toInt(&ok, base));
        break;

    case DataType::Type::INTEGER64:
        data = new DataType((int64_t)dataString.toLong(&ok, base));
        break;

    case DataType::Type::UNSIGNED8:
        data = new DataType(dataString.toInt(&ok, base));
        break;

    case DataType::Type::UNSIGNED16:
        data = new DataType(dataString.toUShort(&ok, base));
        break;

    case DataType::Type::UNSIGNED32:
        data = new DataType(dataString.toUInt(&ok, base));
        break;

    case DataType::Type::UNSIGNED64:
        data = new DataType((uint64_t)dataString.toULong(&ok, base));
        break;

    case DataType::Type::REAL32:
        data = new DataType(dataString.toFloat(&ok));
        break;

    case DataType::Type::REAL64:
        data = new DataType(dataString.toDouble(&ok));
        break;

    case DataType::Type::VISIBLE_STRING:
        data = new DataType(dataString);
        break;

    case DataType::Type::OCTET_STRING:
        data = new DataType(dataString);
        break;
    }

    return data;
}
