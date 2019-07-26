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

#include <QSettings>
#include <QRegularExpression>

#include "deviceiniparser.h"

DcfParser::DcfParser()
{
}

DcfParser::~DcfParser()
{
}

DeviceConfiguration *DcfParser::parse(const QString &path) const
{
    bool ok;
    bool isSubIndex;
    bool hasNodeId;
    uint16_t indexNumber;
    uint8_t accessType;
    uint8_t objectType;
    uint16_t dataType;
    uint8_t subNumber;
    QString accessString;
    QString parameterName;
    Index *index = nullptr;
    SubIndex *subIndex;
    QVariant lowLimit;
    QVariant highLimit;
    uint8_t flagLimit;

    DeviceConfiguration *od;
    od = new DeviceConfiguration;

    QSettings file(path, QSettings::IniFormat);
    DeviceIniParser parser(&file);

    QRegularExpression reSub("([0-Z]{4})(sub)([0-9]+)");
    QRegularExpression reIndex("([0-Z]{4})");

    foreach (const QString &group, file.childGroups())
    {
        isSubIndex = false;
        hasNodeId = false;
        accessType = 0;
        objectType = 0;
        subNumber = 0;
        flagLimit = 0;
        dataType = 0;

        QRegularExpressionMatch matchSub = reSub.match(group);
        QRegularExpressionMatch matchIndex = reIndex.match(group);

        if (matchSub.hasMatch())
        {
            QString matchedSub = matchSub.captured(1);
            index = od->index(static_cast<uint16_t>(matchedSub.toUInt(&ok, 16)));
            isSubIndex = true;
            subNumber = static_cast<uint8_t>(matchSub.captured(3).toShort(&ok, 10));
        }

        else if (matchIndex.hasMatch())
        {
            QString matchedIndex = matchIndex.captured(0);
            indexNumber = static_cast<uint16_t>(matchedIndex.toInt(&ok, 16));
        }

        else if (group == "FileInfo")
        {
            file.beginGroup(group);
            parser.readFileInfo(od);
            file.endGroup();
            continue;
        }

        else if (group == "DummyUsage")
        {
            file.beginGroup(group);
            parser.readDummyUsage(od);
            file.endGroup();
            continue;
        }

        // field only in .dcf files
        else if (group == "DeviceComissioning")
        {
            file.beginGroup(group);
            parser.readDeviceComissioning(od);
            file.endGroup();
            continue;
        }

        else
            continue;

        file.beginGroup(group);

        foreach (const QString &key, file.allKeys())
        {
            QString value = file.value(key).toString();

            uint8_t base = 10;
            if (value.startsWith("0x", Qt::CaseInsensitive))
                base = 16;

            if (key == "AccessType")
            {
                accessString = file.value(key).toString();

                if (accessString == "rw" || accessString == "rwr" || accessString == "rww")
                    accessType += SubIndex::Access::READ + SubIndex::Access::WRITE;

                else if (accessString == "wo")
                    accessType += SubIndex::Access::WRITE;

                else if (accessString == "ro" || accessString == "const")
                    accessType += SubIndex::Access::READ;
            }

            else if (key == "ObjectType")
                objectType = static_cast<uint8_t>(value.toInt(&ok, base));

            else if (key == "ParameterName")
                parameterName = value;

            else if (key == "SubNumber")
                subNumber = static_cast<uint8_t>(value.toInt(&ok, base));

            else if (key == "PDOMapping")
                accessType += parser.readPdoMapping();

            else if (key == "LowLimit")
            {
                lowLimit = parser.readLowLimit();
                flagLimit += SubIndex::Limit::LOW;
            }

            else if (key == "HighLimit")
            {
                highLimit = parser.readHighLimit();
                flagLimit += SubIndex::Limit::HIGH;
            }

            else if (key == "DataType")
            {
                dataType = parser.readDataType();
            }
        }

        QVariant value= parser.readData(&hasNodeId);
        file.endGroup();

        if (isSubIndex)
        {
            switch (index->objectType())
            {
            case Index::Object::RECORD:
            case Index::Object::ARRAY:
                subIndex = new SubIndex(subNumber);
                subIndex->setAccessType(accessType);
                subIndex->setName(parameterName);
                subIndex->setValue(value);
                subIndex->setFlagLimit(flagLimit);
                subIndex->setHasNodeId(hasNodeId);
                subIndex->setDataType(dataType);

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
                subIndex = new SubIndex(static_cast<uint8_t>(0));
                subIndex->setAccessType(accessType);
                subIndex->setName(parameterName);
                subIndex->setValue(value);
                subIndex->setFlagLimit(flagLimit);
                subIndex->setHasNodeId(hasNodeId);
                subIndex->setDataType(dataType);

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
