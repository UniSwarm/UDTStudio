
#include <QSettings>
#include <QRegularExpression>
#include <QString>

#include "edsparsor.h"

EdsParsor::EdsParsor()
{

}

void EdsParsor::parse(OD *od)
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
    Index *index;
    Index *record;
    SubIndex *subIndex;
    DataType *data;

    QString edsFile = "/home/alexis/Documents/doc/CANOpen/eds/301.eds";
    QSettings eds(edsFile, QSettings::IniFormat);
    QRegularExpression reSub("([0-9]+)(sub[0-9]+)");
    QRegularExpression reIndex("([0-9]+)");

    foreach (const QString &group, eds.childGroups())
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
            record = od->index(matchedSub.toInt(&ok));
            isSubIndex = true;
        }
        else if (matchIndex.hasMatch())
        {
            QString matchedIndex = matchIndex.captured(0);
            indexNumber = matchedIndex.toInt(&ok);
        }
        else
        {
            continue;
        }

        eds.beginGroup(group);

        foreach (const QString &key, eds.allKeys())
        {
            // TODO check if start with 0x
            if (key == "AccessType")
            {
                accessString = eds.value(key).toString();

                if (accessString == "rw")
                    accessType = OD_ACCESS_READ_WRITE;

                else if (accessString == "wo")
                    accessType = OD_ACCESS_WRITE_ONLY;

                else if (accessString == "ro")
                    accessType = OD_ACCESS_READ_ONLY;

                else if (accessString == "const")
                    accessType = OD_ACCESS_CONST;
            }

            else if (key == "DataType")
            {
                dataType = eds.value(key).toString().toInt(&ok, 16);

                switch(dataType)
                {
                case OD_TYPE_REAL32:
                    data = new DataType(eds.value("DefaultValue").toString().toFloat(&ok));

                case OD_TYPE_REAL64:
                    data = new DataType(eds.value("DefaultValue").toString().toDouble(&ok));

                default:
                    data = new DataType(eds.value("DefaultValue").toString().toInt(&ok, 16));
                }
            }

            else if (key == "ObjectType")
                objectType = eds.value(key).toString().toInt(&ok, 16);

            else if (key == "ParameterName")
                parameterName = eds.value(key).toString();

            else if (key == "SubNumber")
                subNumber = eds.value(key).toString().toInt(&ok, 10);

        }

        eds.endGroup();

        if (isSubIndex)
        {
            subIndex = new SubIndex(dataType, objectType, accessType, parameterName);
            subIndex->addData(data);
            record->addSubIndex(subIndex);
        }
        else
        {
            index = new Index(dataType, objectType, accessType, parameterName, indexNumber, subNumber);

            if (objectType == OD_OBJECT_VAR)
                index->addData(data);

            od->addIndex(index);
        }
    }
}
