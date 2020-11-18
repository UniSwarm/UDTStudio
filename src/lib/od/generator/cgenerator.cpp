/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QMap>
#include <QRegularExpression>

#include "cgenerator.h"

/**
 * @brief default constructor
 */
CGenerator::CGenerator()
{
}

/**
 * @brief default destructor
 */
CGenerator::~CGenerator()
{
}

/**
 * @brief generates object dictionnary .h and .c files
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 * @return true
 */
bool CGenerator::generate(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    generateH(deviceConfiguration, filePath);
    generateC(deviceConfiguration, filePath);

    return true;
}

/**
 * @brief generates object dictionnary .h and .c files
 * @param device description model based on eds or xdd files
 * @param output file name
 * @return false
 */
bool CGenerator::generate(DeviceDescription *deviceDescription, const QString &filePath)
{
    Q_UNUSED(deviceDescription);
    Q_UNUSED(filePath);
    return false;
}

/**
 * @brief generates object dictionnary .h and .c files
 * @param device description model based on eds or xdd files
 * @param output file name
 * @param CANopen node-id
 */
void CGenerator::generate(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId)
{
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, nodeId);
    generate(deviceConfiguration, filePath);
}

/**
 * @brief generates object dictionnary .h file
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 */
void CGenerator::generateH(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    QFile hFile(filePath);

    if (!hFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream out(&hFile);

    out << "/**\n";
    out << " * Generated .h file\n";

    QString date = QDateTime().currentDateTime().toString("dd-MM-yyyy");
    QString time = QDateTime().currentDateTime().toString("hh:mm AP");
    out << " * Creation date: " << date << "\n";
    out << " * Creation time: " << time << "\n";

    out << " */\n";
    out << "\n";
    out << "#ifndef OD_DATA_H"
        << "\n";
    out << "#define OD_DATA_H"
        << "\n";
    out << "\n";
    out << "#include \"co_od.h\""
        << "\n";
    out << "\n";
    out << "// == Number of entries in object dictionary =="
        << "\n";
    out << "#define OD_NB_ELEMENTS " << deviceConfiguration->indexCount() << "\n";
    out << "\n";
    out << "// ===== struct definitions for records ======="
        << "\n";

    QMap<uint16_t, Index *> indexes = deviceConfiguration->indexes();

    for (Index *index : indexes)
    {
        writeRecordDefinitionH(index, out);
    }

    out << "// === struct definitions for memory types ==="
        << "\n";

    // TODO test read access to know wich memory to use (FLASH/RAM)
    out << "struct sOD_RAM"
        << "\n";
    out << "{"
        << "\n";

    for (Index *index : indexes)
    {
        writeIndexH(index, out);
    }

    out << "};"
        << "\n";
    out << "\n";

    // TODO declararion of FLASH memory
    out << "// extern declaration for RAM and FLASH struct"
        << "\n";
    out << "extern const struct sOD_FLASH OD_FLASH;"
        << "\n";
    out << "\n";
    out << "// ======== extern declaration of OD ========"
        << "\n";
    out << "extern const OD_entry_t OD[OD_NB_ELEMENTS];"
        << "\n";
    out << "extern struct sOD_RAM OD_RAM;"
        << "\n";
    out << "\n";

    for (Index *index : indexes)
    {
        writeDefineH(index, out);
    }

    out << "// ============== function =================="
        << "\n";
    out << "void od_initCommIndexes(void);"
        << "\n";
    out << "void od_initMSIndexes(void);"
        << "\n";
    out << "void od_initAppIndexes(void);"
        << "\n";
    out << "void od_setNodeId(uint8_t nodeId);"
        << "\n";
    out << "\n";
    out << "#endif // OD_DATA_H";
    out << "\n";

    hFile.close();
}

/**
 * @brief generates object dictionnary .c file
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 */
void CGenerator::generateC(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    QFile cFile(filePath);

    if (!cFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream out(&cFile);

    out << "/**\n";
    out << " * Generated .c file\n";

    QString date = QDateTime().currentDateTime().toString("dd-MM-yyyy");
    QString time = QDateTime().currentDateTime().toString("hh:mm AP");
    out << " * Creation date: " << date << "\n";
    out << " * Creation time: " << time << "\n";

    out << " */\n";
    out << "\n";
    out << "#include \"od_data.h\""
        << "\n";
    out << "\n";
    out << "// ==================== initialization ====================="
        << "\n";
    out << "struct sOD_RAM OD_RAM;"
        << "\n";
    out << "\n";

    QMap<uint16_t, Index *> indexes = deviceConfiguration->indexes();

    for (Index *index : indexes)
    {
        if (index->maxSubIndex() > 0)
        {
            for (SubIndex *subIndex : index->subIndexes())
            {
                writeCharLineC(subIndex, out);
            }
            continue;
        }

        if (index->subIndexExist(0))
        {
            writeCharLineC(index->subIndex(0), out);
        }
    }

    out << "\n";

    QList<Index *> commIndexes; //Communication profile area
    QList<Index *> msIndexes;   //Manufacturer-specific profile area
    QList<Index *> appIndexes;  //Standardized profile area



    for (Index *index : indexes)
    {
        if (index->index() < 0x1000)
        {
            ;
        }
        else if (index->index() < 0x2000)
        {
            commIndexes.append(index);
        }
        else if (index->index() < 0x6000)
        {
            msIndexes.append(index);
        }
        else
        {
            appIndexes.append(index);
        }
    }

    out << "//Communication profile area, Indexes 0x1000 to 0x1FFF"
        << "\n";
    out << "void od_initCommIndexes(void)"
        << "\n";
    out << "{";
    writeInitRamC(commIndexes, out);
    out << "}"
        << "\n";    
    out << "\n";
    out << "//Manufacturer-specific profile area, Indexes 0x2000 to 0x5FFF"
        << "\n";
    out << "void od_initMSIndexes(void)"
        << "\n";
    out << "{";
    writeInitRamC(msIndexes, out);
    out << "}"
        << "\n";
    out << "\n";
    out << "//Standardized profile area, Indexes 0x6000 to 0x9FFF"
        << "\n";
    out << "void od_initAppIndexes(void)"
        << "\n";
    out << "{";
    writeInitRamC(appIndexes, out);
    out << "}"
        << "\n";
    out << "\n";

    // TODO initialize struct for FLASH memory
    out << "// ==================== record completion ================="
        << "\n";

    for (Index *index : indexes)
    {
        writeRecordCompletionC(index, out);
    }

    out << "// ============ object dicitonary completion =============="
        << "\n";
    out << "const OD_entry_t OD[OD_NB_ELEMENTS] = "
        << "\n";
    out << "{"
        << "\n";

    for (Index *index : indexes)
    {
        writeOdCompletionC(index, out);
    }

    out << "};";
    out << "\n";
    out << "\n";

    writeSetNodeId(deviceConfiguration, out);

    cFile.close();
}

/**
 * @brief converts a data type to a string
 * @param data type
 * @return data type to C format
 */
QString CGenerator::typeToString(const uint16_t &type)
{
    switch (type)
    {
    case SubIndex::INTEGER8:
        return QLatin1String("int8_t");

    case SubIndex::INTEGER16:
        return QLatin1String("int16_t");

    case SubIndex::INTEGER32:
        return QLatin1String("int32_t");

    case SubIndex::INTEGER64:
        return QLatin1String("int64_t");

    case SubIndex::BOOLEAN:
    case SubIndex::UNSIGNED8:
        return QLatin1String("uint8_t");

    case SubIndex::UNSIGNED16:
        return QLatin1String("uint16_t");

    case SubIndex::UNSIGNED32:
        return QLatin1String("uint32_t");

    case SubIndex::UNSIGNED64:
        return QLatin1String("uint64_t");

    case SubIndex::REAL32:
        return QLatin1String("float32_t");

    case SubIndex::REAL64:
        return QLatin1String("float64_t");

    case SubIndex::VISIBLE_STRING:
        return QLatin1String("vstring_t");

    case SubIndex::OCTET_STRING:
        return QLatin1String("ostring_t");

    case SubIndex::DDOMAIN:
        return QLatin1String("OD_domain_t");

    default:
        return nullptr;
    }
}

/**
 * @brief modifies variable name
 * @param variable name
 * @return variable name to C format
 */
QString CGenerator::varNameToString(const QString &name)
{
    QString modified;
    modified = name.toLower();

    for (int i = 0; i < modified.count(); i++)
    {
        if (modified[i] == QChar('-'))
        {
            modified[i] = QChar(' ');
        }

        if (modified[i] == QChar(' '))
        {
            modified[i + 1] = modified[i + 1].toUpper();
        }
    }

    modified = modified.remove(QChar(' '));

    return modified;
}

/**
 * @brief modifies structure name
 * @param structure name
 * @return structure name to C format
 */
QString CGenerator::structNameToString(const QString &name)
{
    QString modified;
    modified = name.toLower();
    modified = modified.replace(QChar(' '), QChar('_'));
    modified = modified.append("_t");
    return modified;
}

/**
 * @brief converts a sub-index's data
 * @param sub-index which contains data
 * @return data to C hexadecimal format
 */
QString CGenerator::dataToString(const SubIndex *subIndex)
{
    QString data;

    switch (subIndex->dataType())
    {
    case SubIndex::OCTET_STRING:
    case SubIndex::VISIBLE_STRING:
    case SubIndex::UNICODE_STRING:
        data = stringNameToString(subIndex);
        break;

    case SubIndex::REAL32:
        data = QString::number(subIndex->value().toReal(), 'f', 7) + QStringLiteral("f");
        break;

    case SubIndex::REAL64:
        data = QString::number(subIndex->value().toReal(), 'f', 16) + QStringLiteral("F");
        break;

    case SubIndex::UNSIGNED8:
    case SubIndex::UNSIGNED16:
    case SubIndex::UNSIGNED24:
    case SubIndex::UNSIGNED32:
    case SubIndex::UNSIGNED40:
    case SubIndex::UNSIGNED48:
    case SubIndex::UNSIGNED56:
    case SubIndex::UNSIGNED64:
        data = subIndex->value().toString() + QStringLiteral("u");
        break;

    default:
        data = subIndex->value().toString();
        break;
    }

    return data;
}

QString CGenerator::objectTypeToEnumString(const uint16_t objectType)
{
    QString typeObject;

    switch (objectType)
    {
    case Index::OBJECT_NULL:
        typeObject = "OD_OBJECT_NULL";
        break;

    case Index::OBJECT_DOMAIN:
        typeObject = "OD_OBJECT_DOMAIN";
        break;

    case Index::DEFTYPE:
        typeObject = "OD_OBJECT_DEFTYPE";
        break;

    case Index::DEFSTRUCT:
        typeObject = "OD_OBJECT_DEFSTRUCT";
        break;

    case Index::VAR:
        typeObject = "OD_OBJECT_VAR";
        break;

    case Index::ARRAY:
        typeObject = "OD_OBJECT_ARRAY";
        break;

    case Index::RECORD:
        typeObject = "OD_OBJECT_RECORD";
        break;

    default:
        typeObject = QString::number(objectType, 16);
        break;
    }

    return typeObject;
}

QString CGenerator::dataTypeToEnumString(const uint16_t dataType)
{
    QString dataTypeEnumString;

    switch (dataType)
    {
    case SubIndex::BOOLEAN:
        dataTypeEnumString = "OD_TYPE_BOOLEAN";
        break;

    case SubIndex::INTEGER8:
        dataTypeEnumString = "OD_TYPE_INTEGER8";
        break;

    case SubIndex::INTEGER16:
        dataTypeEnumString = "OD_TYPE_INTEGER16";
        break;

    case SubIndex::INTEGER32:
        dataTypeEnumString = "OD_TYPE_INTEGER32";
        break;

    case SubIndex::UNSIGNED8:
        dataTypeEnumString = "OD_TYPE_UNSIGNED8";
        break;

    case SubIndex::UNSIGNED16:
        dataTypeEnumString = "OD_TYPE_UNSIGNED16";
        break;

    case SubIndex::UNSIGNED32:
        dataTypeEnumString = "OD_TYPE_UNSIGNED32";
        break;

    case SubIndex::REAL32:
        dataTypeEnumString = "OD_TYPE_REAL32";
        break;

    case SubIndex::VISIBLE_STRING:
        dataTypeEnumString = "OD_TYPE_VISIBLE_STRING";
        break;

    case SubIndex::OCTET_STRING:
        dataTypeEnumString = "OD_TYPE_OCTET_STRING";
        break;

    case SubIndex::UNICODE_STRING:
        dataTypeEnumString = "OD_TYPE_UNICODE_STRING";
        break;

    case SubIndex::TIME_OF_DAY:
        dataTypeEnumString = "OD_TYPE_TIME_OF_DAY";
        break;

    case SubIndex::TIME_DIFFERENCE:
        dataTypeEnumString = "OD_TYPE_TIME_DIFFERENCE";
        break;

    case SubIndex::DDOMAIN:
        dataTypeEnumString = "OD_TYPE_DOMAIN";
        break;

    case SubIndex::INTEGER24:
        dataTypeEnumString = "OD_TYPE_INTEGER24";
        break;

    case SubIndex::REAL64:
        dataTypeEnumString = "OD_TYPE_REAL64";
        break;

    case SubIndex::INTEGER40:
        dataTypeEnumString = "OD_TYPE_INTEGER40";
        break;

    case SubIndex::INTEGER48:
        dataTypeEnumString = "OD_TYPE_INTEGER48";
        break;

    case SubIndex::INTEGER56:
        dataTypeEnumString = "OD_TYPE_INTEGER56";
        break;

    case SubIndex::INTEGER64:
        dataTypeEnumString = "OD_TYPE_INTEGER64";
        break;

    case SubIndex::UNSIGNED24:
        dataTypeEnumString = "OD_TYPE_UNSIGNED24";
        break;

    case SubIndex::UNSIGNED40:
        dataTypeEnumString = "OD_TYPE_UNSIGNED40";
        break;

    case SubIndex::UNSIGNED48:
        dataTypeEnumString = "OD_TYPE_UNSIGNED48";
        break;

    case SubIndex::UNSIGNED56:
        dataTypeEnumString = "OD_TYPE_UNSIGNED56";
        break;

    case SubIndex::UNSIGNED64:
        dataTypeEnumString = "OD_TYPE_UNSIGNED64";
        break;
    }

    return dataTypeEnumString;
}

QString CGenerator::accessToEnumString(const uint8_t acces)
{
    QString accessToEnumString;

    if (acces & SubIndex::READ)
    {
        accessToEnumString += "OD_ACCESS_READ";
    }
    if (acces & SubIndex::WRITE)
    {
        if (!accessToEnumString.isEmpty())
        {
            accessToEnumString += " | ";
        }
        accessToEnumString += "OD_ACCESS_WRITE";
    }
    if (acces & SubIndex::TPDO)
    {
        if (!accessToEnumString.isEmpty())
        {
            accessToEnumString += " | ";
        }
        accessToEnumString += "OD_ACCESS_TPDO";
    }
    if (acces & SubIndex::RPDO)
    {
        if (!accessToEnumString.isEmpty())
        {
            accessToEnumString += " | ";
        }
        accessToEnumString += "OD_ACCESS_RPDO";
    }

    return accessToEnumString;
}

/**
 * @brief converts data type and object type
 * @param sub-index
 * @return a C hexadecimal value coded on 16 bits
 */
QString CGenerator::typeObjectToString(Index *index, uint8_t subIndex, bool isInRecord = false)
{
    QString typeObject;

    if (isInRecord)
    {
        typeObject = objectTypeToEnumString(Index::VAR);
    }
    else
    {
        typeObject = objectTypeToEnumString(index->objectType());
    }
    typeObject += " | ";

    typeObject += dataTypeToEnumString(index->subIndex(subIndex)->dataType());

    return typeObject;
}

/**
 * @brief convert a string name
 * @param sub-index
 * @param sub-index number for arrays
 * @return string name for C file
 */
QString CGenerator::stringNameToString(const SubIndex *subIndex)
{
    QString string;

    if (subIndex->subIndex() == 0)
    {
        string = varNameToString(subIndex->name()) + "Str";
    }
    else
    {
        string = varNameToString(subIndex->name()) + "Str" + QString::number(subIndex->subIndex());
    }

    return string;
}

/**
 * @brief writes a record's structure in a .h file
 * @param record
 * @param .h file
 */
void CGenerator::writeRecordDefinitionH(Index *index, QTextStream &hFile)
{
    if (index->objectType() != Index::Object::RECORD)
    {
        return;
    }

    QString structName = structNameToString(index->name());

    if (_typeSetTable.contains(structName))
    {
        return;
    }

    hFile << "typedef struct"
          << "  // 0x" << QString::number(index->index(), 16) << "\n{\n";

    for (SubIndex *subIndex : index->subIndexes())
    {
        QString dataType = typeToString(subIndex->dataType());
        if (dataType == nullptr)
        {
            continue;
        }

        hFile << "    " << dataType << " " << varNameToString(subIndex->name()) << ";"
              << "\n";
    }
    hFile << "} " << structName << ";\n\n";

    _typeSetTable.insert(structName);
}

/**
 * @brief writes an index definition in a .h file
 * @param index
 * @param .h file
 */
void CGenerator::writeIndexH(Index *index, QTextStream &hFile)
{
    QString dataType;
    switch (index->objectType())
    {
    case Index::Object::VAR:
        if (!index->subIndexExist(0))
        {
            return;
        }

        if (index->subIndex(0)->dataType() == SubIndex::VISIBLE_STRING || index->subIndex(0)->dataType() == SubIndex::OCTET_STRING || index->subIndex(0)->dataType() == SubIndex::UNICODE_STRING)
        {
            return;
        }

        dataType = typeToString(index->subIndex(0)->dataType());
        if (dataType == nullptr)
        {
            break;
        }

        hFile << "    " << dataType << " " << varNameToString(index->name()) << ";";
        break;

    case Index::Object::RECORD:
        hFile << "    " << structNameToString(index->name()) << " " << varNameToString(index->name()) << ";";
        break;

    case Index::Object::ARRAY:
        if (!index->subIndexExist(1))
        {
            break;
        }

        dataType = typeToString(index->subIndex(1)->dataType());
        if (dataType == nullptr)
        {
            break;
        }

        hFile << "    " << dataType << " " << varNameToString(index->name());
        hFile << "[" << index->maxSubIndex() - 1 << "]"
              << ";";

        break;
    }

    hFile << "  // 0x" << QString::number(index->index(), 16) << "\n";
}

/**
 * @brief writes an index initialization in RAM in a .c file
 * @param index
 * @param .c file
 */
int CGenerator::writeRamLineC(Index *index, QTextStream &cFile)
{
    QMap<uint8_t, SubIndex *> subIndexes;
    int written = 0;

    switch (index->objectType())
    {
    case Index::Object::VAR:
        if (!index->subIndexExist(0))
        {
            break;
        }
        if (index->subIndex(0)->dataType() == SubIndex::VISIBLE_STRING || index->subIndex(0)->dataType() == SubIndex::OCTET_STRING || index->subIndex(0)->dataType() == SubIndex::UNICODE_STRING)
        {
            break;
        }
        if (!index->subIndex(0)->value().isValid())
        {
            break;
        }

        cFile << "    "
              << "OD_RAM." << varNameToString(index->name());
        cFile << " = ";
        cFile << dataToString(index->subIndex(0));
        cFile << ";";
        cFile << "  // 0x" << QString::number(index->index(), 16);
        cFile << "\n";
        written++;
        break;

    case Index::Object::RECORD:
        subIndexes = index->subIndexes();
        for (SubIndex *subIndex : subIndexes)
        {
            if (!subIndex->value().isValid())
            {
                continue;
            }
            cFile << "    "
                  << "OD_RAM." << varNameToString(index->name()) << "." << varNameToString(subIndex->name());
            cFile << " = ";
            cFile << dataToString(subIndex);
            cFile << ";";
            cFile << "  // 0x" << QString::number(index->index(), 16) << "." << subIndex->subIndex();
            cFile << "\n";
            written++;
        }
        break;

    case Index::Object::ARRAY:
        for (uint8_t i = 1; i < index->subIndexesCount(); i++)
        {
            if (!index->subIndexExist(i))
            {
                continue;
            }
            if (index->subIndex(i)->dataType() == SubIndex::DDOMAIN)
            {
                continue;
            }
            if (!index->subIndex(i)->value().isValid())
            {
                continue;
            }
            cFile << "    "
                  << "OD_RAM." << varNameToString(index->name()) << "[" << i - 1 << "]";
            cFile << " = ";
            cFile << dataToString(index->subIndex(i));
            cFile << ";";
            cFile << "  // 0x" << QString::number(index->index(), 16) << "." << i - 1;
            cFile << "\n";
            written++;
        }
        break;
    }
    return written;
}

/**
 * @brief writes a record initialization in ram in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeRecordCompletionC(Index *index, QTextStream &cFile)
{
    if (index->objectType() == Index::Object::RECORD)
    {
        cFile << "static const OD_entrySubIndex_t OD_Record" << QString::number(index->index(), 16).toUpper() << "[" << index->maxSubIndex() << "] =\n";
        cFile << "{\n";

        for (SubIndex *subIndex : index->subIndexes())
        {
            cFile << "    "
                  << "{(void*)&OD_RAM." << varNameToString(index->name());
            cFile << "." << varNameToString(subIndex->name());
            // TODO PDOmapping
            cFile << ", " << typeObjectToString(index, subIndex->subIndex(), true) << ", ";
            cFile << accessToEnumString(subIndex->accessType()) << ", ";
            cFile << subIndex->subIndex();
            cFile << "},"
                  << "\n";
        }

        cFile << "};\n\n";
    }
}

/**
 * @brief writes an object dictionary entry initialisation in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeOdCompletionC(Index *index, QTextStream &cFile)
{
    cFile << "    "
          << "{";

    // OD_entry_t.index
    cFile << "0x" << QString::number(index->index(), 16).toUpper() << ", "
          << "0x";

    // OD_entry_t.nbSubIndex
    switch (index->objectType())
    {
    case Index::Object::VAR:
        cFile << "0";
        break;

    case Index::Object::RECORD:
        cFile << index->maxSubIndex() - 1;
        break;

    case Index::Object::ARRAY:
        cFile << index->maxSubIndex() - 1;
        break;
    }
    cFile << ", ";

    // OD_entry_t.ptData, OD_entry_t.typeObject, OD_entry_t.accessPDOmapping
    switch (index->objectType())
    {
    case Index::Object::VAR:
        if (!index->subIndexExist(0))
        {
            break;
        }
        // OD_entry_t.ptData
        if (index->subIndex(0)->dataType() == SubIndex::VISIBLE_STRING || index->subIndex(0)->dataType() == SubIndex::OCTET_STRING || index->subIndex(0)->dataType() == SubIndex::UNICODE_STRING)
        {
            cFile << "(void*)" << stringNameToString(index->subIndex(0)) << ", ";
        }
        else
        {
            cFile << "(void*)&OD_RAM." << varNameToString(index->name()) << ", ";
        }

        // OD_entry_t.typeObject
        cFile << typeObjectToString(index, 0) << ", ";

        // OD_entry_t.accessPDOmapping
        cFile << accessToEnumString(index->subIndex(0)->accessType());
        break;

    case Index::Object::RECORD:
        // OD_entry_t.ptData
        cFile << "(void*)OD_Record" << QString::number(index->index(), 16).toUpper() << ", ";

        // OD_entry_t.typeObject
        cFile << objectTypeToEnumString(Index::RECORD) << ", ";

        // OD_entry_t.accessPDOmapping
        cFile << "0x0";
        break;

    case Index::Object::ARRAY:
        if (!index->subIndexExist(1))
        {
            break;
        }

        // OD_entry_t.ptData
        cFile << "(void*)OD_RAM." << varNameToString(index->name()) << ", ";

        // OD_entry_t.typeObject
        cFile << typeObjectToString(index, 1) << ", ";

        // OD_entry_t.accessPDOmapping
        cFile << accessToEnumString(index->subIndex(1)->accessType());
        break;
    }

    cFile << "},";
    cFile << "\n";
}

/**
 * @brief writes char[] initialization in a C file
 * @param sub-index
 * @param C file
 * @param sub-index-number for arrays
 */
void CGenerator::writeCharLineC(const SubIndex *subIndex, QTextStream &cFile)
{
    switch (subIndex->dataType())
    {
    case SubIndex::VISIBLE_STRING:
    case SubIndex::OCTET_STRING:
        cFile << "static const char " << stringNameToString(subIndex) << "[]"
              << " = "
              << "\"" << subIndex->value().toString() << "\""
              << ";\n";
        break;
    default:
        break;
    }
}

/**
 * @brief write ram initialization in c file
 * @param indexes
 * @param C file
 */
void CGenerator::writeInitRamC(QList<Index *> indexes, QTextStream &cFile)
{
    uint8_t lastObjectType = 0;
    int written = 1;
    for (Index *index : indexes)
    {
        if ((index->objectType() != lastObjectType || index->objectType() == Index::Object::RECORD || index->objectType() == Index::Object::ARRAY)
            && written != 0)
        {
            cFile << "\n";
        }
        written = writeRamLineC(index, cFile);
        lastObjectType = index->objectType();
    }
}

/**
 * @brief write custom defines for index and sub-indexes
 * @param index model
 * @param .h file
 */
void CGenerator::writeDefineH(Index *index, QTextStream &hFile)
{
    hFile << "#define "
          << "OD_" << varNameToString(index->name()).toUpper() << " OD_RAM." << varNameToString(index->name()) << "\n";
    hFile << "#define "
          << "OD_INDEX" << QString::number(index->index(), 16).toUpper() << " OD_RAM." << varNameToString(index->name()) << "\n";

    if (index->objectType() == Index::Object::RECORD)
    {
        for (SubIndex *subIndex : index->subIndexes())
        {
            hFile << "#define "
                  << "OD_INDEX" << QString::number(index->index(), 16).toUpper() << "_" << QString::number(subIndex->subIndex(), 16).toUpper();
            hFile << " OD_INDEX" << QString::number(index->index(), 16).toUpper() << "." << varNameToString(subIndex->name()) << "\n";
        }
    }

    else if (index->objectType() == Index::Object::ARRAY)
    {
        for (SubIndex *subIndex : index->subIndexes())
        {
            uint8_t numSubIndex = subIndex->subIndex();

            if (numSubIndex == 0)
            {
                continue;
            }

            hFile << "#define "
                  << "OD_INDEX" << QString::number(index->index(), 16).toUpper() << "_" << QString::number(numSubIndex, 16).toUpper();
            hFile << " OD_INDEX" << QString::number(index->index(), 16).toUpper() << "[" << QString::number(numSubIndex - 1, 16).toUpper() << "]"
                  << "\n";
        }
    }

    hFile << "\n";
}

void CGenerator::writeSetNodeId(DeviceConfiguration *deviceConfiguration, QTextStream &cFile)
{
    cFile << "void od_setNodeId(uint8_t nodeId)\n";
    cFile << "{\n";

    for (Index *index : deviceConfiguration->indexes())
    {
        for (SubIndex *subIndex : index->subIndexes())
        {
            if (subIndex->hasNodeId())
            {
                cFile << "    ";

                switch (index->objectType())
                {
                case Index::Object::VAR:
                    cFile << "OD_INDEX" << QString::number(index->index(), 16).toUpper();
                    break;

                case Index::Object::RECORD:
                case Index::Object::ARRAY:
                    cFile << "OD_INDEX" << QString::number(index->index(), 16).toUpper() << "_" << QString::number(subIndex->subIndex(), 16);
                    break;
                }

                uint value = subIndex->value().toUInt() - deviceConfiguration->nodeId().toUInt();
                cFile << " = 0x" << QString::number(value, 16).toUpper() << "u + " << "nodeId";

                cFile << ";  // " << index->name() << " : " << subIndex->name() << "\n";
            }
        }
    }

    cFile << "}\n";
}
