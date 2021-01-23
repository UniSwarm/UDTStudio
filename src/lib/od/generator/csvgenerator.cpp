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

#include "csvgenerator.h"
#include <QFileInfo>

/**
 * @brief default constructor
 */
CsvGenerator::CsvGenerator()
{

}

/**
 * @brief default destructor
 */
CsvGenerator::~CsvGenerator()
{
}

/**
 * @brief generates object dictionnary .csv files
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 * @return true
 */
bool CsvGenerator::generate(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    Q_UNUSED(deviceConfiguration);
    Q_UNUSED(filePath);
    return true;
}

/**
 * @brief generates object dictionnary .csv files
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 * @return true
 */
bool CsvGenerator::generate(DeviceDescription *deviceDescription, const QString &filePath)
{
    QString filePathBaseName = QFileInfo(filePath).path() + "/" + QFileInfo(filePath).baseName();
    QString outCom =  filePathBaseName + "Communication." + QFileInfo(filePath).suffix();
    QFile csvComFile(outCom);
    if (!csvComFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString outManu = filePathBaseName + "Manufacturer." + QFileInfo(filePath).suffix();
    QFile csvManuFile(outManu);
    if (!csvManuFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString outStandard = filePathBaseName + "Standardized." + QFileInfo(filePath).suffix();
    QFile csvStandardFile(outStandard);
    if (!csvStandardFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QList<Index *> mandatories;
    QList<Index *> optionals;
    QList<Index *> manufacturers;

    for (Index *index : deviceDescription->indexes().values())
    {
        uint16_t numIndex = index->index();

        if (numIndex >= 0x1000 && numIndex < 0x2000)
        {
            mandatories.append(index);
        }
        else if (numIndex >= 0x2000 && numIndex < 0x6000)
        {
            manufacturers.append(index);
        }
        else
        {
            optionals.append(index);
        }
    }

    QTextStream out(&csvComFile);
    out << "index,subNumber,subIndex,name,objectType,dataType,accessType,pdoMapping,defaultValue,lowLimit,highLimit,\n";
    writeListIndex(mandatories, &out);
    csvComFile.close();

    out.setDevice(&csvManuFile);
    out << "index,subNumber,subIndex,name,objectType,dataType,accessType,pdoMapping,defaultValue,lowLimit,highLimit,\n";
    writeListIndex(manufacturers, &out);
    csvManuFile.close();

    out.setDevice(&csvStandardFile);
    out << "index,subNumber,subIndex,name,objectType,dataType,accessType,pdoMapping,defaultValue,lowLimit,highLimit,\n";
    writeListIndex(optionals, &out);
    csvStandardFile.close();

    return true;
}

/**
 * @brief writes a list of index and these parameters
 * @param list of indexes
 */
void CsvGenerator::writeListIndex(const QList<Index *> indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        switch (index->objectType())
        {
        case Index::Object::VAR:
            writeIndex(index, out);
            break;

        case Index::Object::RECORD:
            writeRecord(index, out);
            break;

        case Index::Object::ARRAY:
            writeArray(index, out);
            break;
        }
    }
}

/**
 * @brief writes an index and these parameters
 * @param index model
 */
void CsvGenerator::writeIndex(Index *index, QTextStream *out)
{
    SubIndex *subIndex;
    subIndex = index->subIndex(0);

    if (subIndex == nullptr)
    {
        return;
    }

    int base = 16;

    *out << QString::number(index->index(), base).toUpper() << ",";     // index
    *out << ",";                                                        // subNumber
    *out << "0,";                                                       // subIndex
    QString name = index->name();
    *out << name.replace("_"," ") << ",";                                // Name
    *out << index->objectTypeStr(index->objectType()) << ",";           // objectType
    *out << subIndex->dataTypeStr(subIndex->dataType()) << ",";         // dataType
    *out << accessToString(subIndex->accessType()) << ",";              // accessType
    *out << pdoToString(subIndex->accessType()) << ",";                 // pdoMapping
    *out << subIndex->value().toString() << ",";                        // defaultValue

    writeLimit(subIndex, out);
    *out << "\n";
}

/**
 * @brief writes a record index
 * @param index model
 */
void CsvGenerator::writeRecord(Index *index, QTextStream *out)
{
    int base = 16;

    *out << QString::number(index->index(), base).toUpper() << ",";     // index
    *out << index->subIndexesCount() << ",";                            // subNumber
    *out << ",";                                                        // subIndex
    QString name = index->name();
    *out << name.replace("_"," ") << ",";                               // Name
    *out << index->objectTypeStr(index->objectType()) << ",";           // objectType
    *out << ",,,,,,";                                             // dataType,accessType,pdoMapping,defaultValue,lowLimit,highLimit
    *out << "\n";

    for (SubIndex *subIndex : index->subIndexes())
    {
        *out << QString::number(index->index(), base).toUpper() << ","; // index
        *out << ",";                                                   // subNumber
        *out << subIndex->subIndex() << ",";                            // subIndex
        QString name = subIndex->name();
        *out << name.replace("_"," ") << ",";                           // Name
        *out << index->objectTypeStr(Index::Object::VAR) << ",";        // objectType
        *out << subIndex->dataTypeStr(subIndex->dataType()) << ",";     // dataType
        *out << accessToString(subIndex->accessType()) << ",";          // accessType
        *out << pdoToString(subIndex->accessType()) << ",";             // pdoMapping
        *out << subIndex->value().toString() << ",";                    // defaultValue

        writeLimit(subIndex, out);
        *out << "\n";
    }
}

/**
 * @brief writes an array index
 * @param index model
 */
void CsvGenerator::writeArray(Index *index, QTextStream *out)
{
    writeRecord(index, out);
}

/**
 * @brief writes high limit and low limit of a sub-index if they exist
 * @param sub index model
 */
void CsvGenerator::writeLimit(const SubIndex *subIndex, QTextStream *out)
{
    if (subIndex->hasLowLimit())
    {
        *out << subIndex->lowLimit().toString() << ",";
    }
    else
    {
         *out << ",";
    }

    if (subIndex->hasHighLimit())
    {
        *out << subIndex->highLimit().toString() << ",";
    }
    else
    {
        *out << ",";
    }
}

/**
 * @brief returns an access code to his corresponding string format
 * @param access code
 * @return formated string
 */
QString CsvGenerator::accessToString(int access)
{
    switch (access)
    {
    case SubIndex::READ:
    case SubIndex::READ + SubIndex::TPDO:
        return QString("ro");

    case SubIndex::WRITE:
    case SubIndex::WRITE + SubIndex::RPDO:
        return QString("wo");

    case SubIndex::READ + SubIndex::WRITE:
    case SubIndex::READ + SubIndex::WRITE + SubIndex::TPDO + SubIndex::RPDO:
        return QString("rw");

    case SubIndex::READ + SubIndex::WRITE + SubIndex::TPDO:
        return QString("rwr");

    case SubIndex::READ + SubIndex::WRITE + SubIndex::RPDO:
        return QString("rww");
    }

    return "";
}

/**
 * @brief return 1 if accessType has a mapping pdo flag, else 0
 * @param 8 bits access type code
 * @return 1 or 0 as a string
 */
QString CsvGenerator::pdoToString(uint8_t accessType)
{
    if ((accessType & SubIndex::TPDO) == SubIndex::TPDO || (accessType & SubIndex::RPDO) == SubIndex::RPDO)
    {
        return "yes";
    }

    return "-";
}
