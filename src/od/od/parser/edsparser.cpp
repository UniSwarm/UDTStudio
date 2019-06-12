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

#include "edsparser.h"

#include <QRegularExpression>

EdsParser::EdsParser(QString path)
{
    _edsFile = path;
}

/**
 * @brief parses an eds file and completes an object dictionary
 * @param object dictionary
 */
void EdsParser::parse(OD *od)
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

    QSettings eds(_edsFile, QSettings::IniFormat);
    QRegularExpression reSub("([0-Z]{4})(sub)([0-9]+)");
    QRegularExpression reIndex("([0-Z]{4})");

    foreach (const QString &group, eds.childGroups())
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

        eds.beginGroup(group);

        foreach (const QString &key, eds.allKeys())
        {
            // TODO check if start with 0x
            if (key == "AccessType")
            {
                accessString = eds.value(key).toString();

                if (accessString == "rw")
                    accessType = OD::_Access::READ_WRITE;

                else if (accessString == "wo")
                    accessType = OD::_Access::WRITE_ONLY;

                else if (accessString == "ro")
                    accessType = OD::_Access::READ_ONLY;

                else if (accessString == "const")
                    accessType = OD::_Access::CONST;
            }

            else if (key == "DataType")
                dataType = eds.value(key).toString().toInt(&ok, 16);

            else if (key == "ObjectType")
                objectType = eds.value(key).toString().toInt(&ok, 16);

            else if (key == "ParameterName")
                parameterName = eds.value(key).toString();

            else if (key == "SubNumber")
                subNumber = eds.value(key).toString().toInt(&ok, 10);
        }

        data = readData(eds);
        eds.endGroup();

        if (isSubIndex)
        {
            switch (index->objectType())
            {
            case OD::_Object::RECORD:
                subIndex = new SubIndex(dataType, objectType, accessType, parameterName);
                subIndex->setSubNumber(subNumber);
                subIndex->addData(data);
                index->addSubIndex(subIndex);
                break;

            case OD::_Object::ARRAY:
                index->setDataType(dataType);
                index->setAccessType(accessType);
                index->addData(data);
                break;
            }
        }
        else
        {
            index = new Index(dataType, objectType, accessType, parameterName, indexNumber, subNumber);

            if (objectType == OD::_Object::VAR)
                index->addData(data);

            od->addIndex(index);
        }
    }
}

DataType* EdsParser::readData(const QSettings &eds) const
{
    bool ok;
    int16_t dataType = eds.value("DataType").toString().toShort(&ok, 16);

    QString dataString = eds.value("DefaultValue").toString();
    uint8_t base = 10;
    if (dataString.startsWith("0x", Qt::CaseInsensitive))
        base = 16;

    DataType *data = nullptr;
    switch(dataType)
    {
    case OD::_Type::BOOLEAN:
        if (dataString.toShort(&ok, base))
            data = new DataType(true);
        else
            data = new DataType(false);
        break;

    case OD::_Type::INTEGER8:
        data = new DataType(dataString.toShort(&ok, base));
        break;

    case OD::_Type::INTEGER16:
        data = new DataType(dataString.toShort(&ok, base));
        break;

    case OD::_Type::INTEGER32:
        data = new DataType(dataString.toInt(&ok, base));
        break;

    case OD::_Type::INTEGER64:
        data = new DataType((int64_t)dataString.toLong(&ok, base));
        break;

    case OD::_Type::UNSIGNED8:
        data = new DataType(dataString.toInt(&ok, base));
        break;

    case OD::_Type::UNSIGNED16:
        data = new DataType(dataString.toUShort(&ok, base));
        break;

    case OD::_Type::UNSIGNED32:
        data = new DataType(dataString.toUInt(&ok, base));
        break;

    case OD::_Type::UNSIGNED64:
        data = new DataType((uint64_t)dataString.toULong(&ok, base));
        break;

    case OD::_Type::REAL32:
        data = new DataType(dataString.toFloat(&ok));
        break;

    case OD::_Type::REAL64:
        data = new DataType(dataString.toDouble(&ok));
        break;

    case OD::_Type::VISIBLE_STRING:
        data = new DataType(dataString);
        break;

    case OD::_Type::OCTET_STRING:
        data = new DataType(dataString);
        break;
    }

    return data;
}
