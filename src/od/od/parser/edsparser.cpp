#include "edsparser.h"

#include <QRegularExpression>
#include <QSettings>

#include "deviceiniparser.h"

EdsParser::EdsParser()
{
}

EdsParser::~EdsParser()
{
}

DeviceDescription *EdsParser::parse(const QString &path) const
{
    bool ok;
    bool isSubIndex;
    bool hasNodeId;
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

    DeviceDescription *od = new DeviceDescription;

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

        // field only in .eds files
        else if (group == "DeviceInfo")
        {
            file.beginGroup(group);
            parser.readDeviceInfo(od);
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
                objectType = (uint8_t)value.toInt(&ok, base);

            else if (key == "ParameterName")
                parameterName = value;

            else if (key == "SubNumber")
                subNumber = (uint8_t)value.toInt(&ok, base);

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
        }

        DataStorage data = parser.readData(&hasNodeId);
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
                subIndex->setData(data);
                subIndex->setFlagLimit(flagLimit);
                subIndex->setHasNodeId(hasNodeId);

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
                subIndex->setData(data);
                subIndex->setFlagLimit(flagLimit);
                subIndex->setHasNodeId(hasNodeId);

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
