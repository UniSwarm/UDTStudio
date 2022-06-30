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

#include "cgenerator.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QMap>
#include <QRegularExpression>

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
    Q_UNUSED(deviceDescription)
    Q_UNUSED(filePath)
    return false;
}

/**
 * @brief generates object dictionnary .h and .c files
 * @param device description model based on eds or xdd files
 * @param output file name
 * @param CANopen node-id
 */
bool CGenerator::generate(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId)
{
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, nodeId);
    return generate(deviceConfiguration, filePath);
}

/**
 * @brief generates object dictionnary .h file
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 */
bool CGenerator::generateH(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    QFile hFile(filePath);

    if (!hFile.open(QIODevice::WriteOnly))
    {
        appendError(QString("Cannot open file %1\n").arg(filePath));
        return false;
    }

    QTextStream out(&hFile);

    out << "/**\n";
    out << " * Generated od_data.h file\n";

    QString date = QDateTime::currentDateTime().toString("dd-MM-yyyy");
    QString time = QDateTime::currentDateTime().toString("hh:mm AP");
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
    out << "#define OD_OBJECTS_COUNT " << deviceConfiguration->indexCount() << "\n";
    out << "\n";
    out << "// ===== struct definitions for records ======="
        << "\n";

    QMap<uint16_t, Index *> indexes = deviceConfiguration->indexes();

    for (Index *index : indexes)
    {
        if (index->objectType() == Index::RECORD)
        {
            writeRecordDefinitionH(index, out);
        }
        if (index->objectType() == Index::ARRAY)
        {
            writeArrayDefinitionH(index, out);
        }
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
    out << "extern const OD_entry_t OD[OD_OBJECTS_COUNT];"
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

    if (_errorStr.isEmpty())
    {
        hFile.close();
        return true;
    }
    hFile.remove();
    return false;
}

/**
 * @brief generates object dictionnary .c file
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 */
bool CGenerator::generateC(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    QFile cFile(filePath);

    if (!cFile.open(QIODevice::WriteOnly))
    {
        appendError(QString("Cannot open file %1\n").arg(filePath));
        return false;
    }

    QTextStream out(&cFile);

    out << "/**\n";
    out << " * Generated od_data.c file\n";

    QString date = QDateTime::currentDateTime().toString("dd-MM-yyyy");
    QString time = QDateTime::currentDateTime().toString("hh:mm AP");
    out << " * Creation date: " << date << "\n";
    out << " * Creation time: " << time << "\n";

    out << " */\n";
    out << "\n";
    out << "#include \"od_data.h\""
        << "\n"
        << "\n";

    out << "#define STRINGIZE(x) #x"
        << "\n";
    out << "#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)"
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

    QList<Index *> commIndexes;  // Communication profile area
    QList<Index *> msIndexes;    // Manufacturer-specific profile area
    QList<Index *> appIndexes;   // Standardized profile area

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

    out << "// Communication profile area, Indexes 0x1000 to 0x1FFF"
        << "\n";
    out << "void od_initCommIndexes(void)"
        << "\n";
    out << "{";
    writeInitRamC(commIndexes, out);
    out << "}"
        << "\n";
    out << "\n";
    out << "// Manufacturer-specific profile area, Indexes 0x2000 to 0x5FFF"
        << "\n";
    out << "void od_initMSIndexes(void)"
        << "\n";
    out << "{";
    writeInitRamC(msIndexes, out);
    out << "}"
        << "\n";
    out << "\n";
    out << "// Standardized profile area, Indexes 0x6000 to 0x9FFF"
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
        writeSubentriesList(index, out);
    }

    out << "// ============ object dictionary completion =============="
        << "\n";
    out << "const OD_entry_t OD[OD_OBJECTS_COUNT] = "
        << "\n";
    out << "{"
        << "\n";
    out << "//  {index, typeObject, nbSubIndex, subEntries}"
        << "\n";
    for (Index *index : indexes)
    {
        writeOdCompletionC(index, out);
    }
    out << "};";
    out << "\n";
    out << "\n";

    writeSetNodeId(deviceConfiguration, out);

    if (_errorStr.isEmpty())
    {
        cFile.close();
        return true;
    }
    cFile.remove();
    return false;
}

bool CGenerator::generateHStruct(DeviceConfiguration *deviceConfiguration, const QString &filePath, uint16_t min, uint16_t max, const QString &structName)
{
    QFile hFile(filePath);

    if (!hFile.open(QIODevice::WriteOnly))
    {
        appendError(QString("Cannot open file %1\n").arg(filePath));
        return false;
    }

    QTextStream out(&hFile);

    out << "/**\n";
    out << " * Generated partial OD struct file\n";

    QString date = QDateTime::currentDateTime().toString("dd-MM-yyyy");
    QString time = QDateTime::currentDateTime().toString("hh:mm AP");
    out << " * Creation date: " << date << "\n";
    out << " * Creation time: " << time << "\n";

    QString defineName = QFileInfo(filePath).fileName().toUpper().replace(' ', '_').replace('.', '_');
    out << " */\n";
    out << "\n";
    out << "#ifndef " << defineName << "\n";
    out << "#define " << defineName << "\n";
    out << "\n";
    out << "#include \"od_data.h\""
        << "\n";
    out << "\n";

    out << "struct " << structName << "\n";
    out << "{"
        << "\n";

    QMap<uint16_t, Index *> indexes = deviceConfiguration->indexes();
    for (Index *index : indexes)
    {
        if (index->index() >= min && index->index() <= max)
        {
            writeIndexH(index, out);
        }
    }

    out << "};"
        << "\n";
    out << "\n";
    out << "#endif  // " << defineName << "\n";

    if (_errorStr.isEmpty())
    {
        hFile.close();
        return true;
    }
    hFile.remove();
    return false;
}

/**
 * @brief converts a data type to a string
 * @param data type
 * @return data type to C format
 */
QString CGenerator::typeToString(SubIndex::DataType type)
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
            return QLatin1String("");
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
    modified = name;

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
    switch (subIndex->dataType())
    {
        case SubIndex::OCTET_STRING:
        case SubIndex::VISIBLE_STRING:
        case SubIndex::UNICODE_STRING:
            return stringNameToString(subIndex);

        case SubIndex::REAL32:
            return QString::number(subIndex->value().toReal(), 'f', 7) + QStringLiteral("f");

        case SubIndex::REAL64:
            return QString::number(subIndex->value().toReal(), 'f', 16) + QStringLiteral("F");

        case SubIndex::UNSIGNED8:
        case SubIndex::UNSIGNED16:
        case SubIndex::UNSIGNED24:
        case SubIndex::UNSIGNED32:
        case SubIndex::UNSIGNED40:
        case SubIndex::UNSIGNED48:
        case SubIndex::UNSIGNED56:
        case SubIndex::UNSIGNED64:
            return subIndex->value().toString() + QStringLiteral("u");

        default:
            return subIndex->value().toString();
    }
}

QString CGenerator::objectTypeToEnumString(uint16_t objectType)
{
    switch (objectType)
    {
        case Index::OBJECT_NULL:
            return QStringLiteral("OD_OBJECT_NULL");

        case Index::OBJECT_DOMAIN:
            return QStringLiteral("OD_OBJECT_DOMAIN");

        case Index::DEFTYPE:
            return QStringLiteral("OD_OBJECT_DEFTYPE");

        case Index::DEFSTRUCT:
            return QStringLiteral("OD_OBJECT_DEFSTRUCT");

        case Index::VAR:
            return QStringLiteral("OD_OBJECT_VAR");

        case Index::ARRAY:
            return QStringLiteral("OD_OBJECT_ARRAY");

        case Index::RECORD:
            return QStringLiteral("OD_OBJECT_RECORD");

        default:
            return QString::number(objectType, 16);
    }
}

QString CGenerator::dataTypeToEnumString(uint16_t dataType)
{
    switch (dataType)
    {
        case SubIndex::BOOLEAN:
            return QStringLiteral("OD_TYPE_BOOLEAN");

        case SubIndex::INTEGER8:
            return QStringLiteral("OD_TYPE_INTEGER8");

        case SubIndex::INTEGER16:
            return QStringLiteral("OD_TYPE_INTEGER16");

        case SubIndex::INTEGER32:
            return QStringLiteral("OD_TYPE_INTEGER32");

        case SubIndex::UNSIGNED8:
            return QStringLiteral("OD_TYPE_UNSIGNED8");

        case SubIndex::UNSIGNED16:
            return QStringLiteral("OD_TYPE_UNSIGNED16");

        case SubIndex::UNSIGNED32:
            return QStringLiteral("OD_TYPE_UNSIGNED32");

        case SubIndex::REAL32:
            return QStringLiteral("OD_TYPE_REAL32");

        case SubIndex::VISIBLE_STRING:
            return QStringLiteral("OD_TYPE_VISIBLE_STRING");

        case SubIndex::OCTET_STRING:
            return QStringLiteral("OD_TYPE_OCTET_STRING");

        case SubIndex::UNICODE_STRING:
            return QStringLiteral("OD_TYPE_UNICODE_STRING");

        case SubIndex::TIME_OF_DAY:
            return QStringLiteral("OD_TYPE_TIME_OF_DAY");

        case SubIndex::TIME_DIFFERENCE:
            return QStringLiteral("OD_TYPE_TIME_DIFFERENCE");

        case SubIndex::DDOMAIN:
            return QStringLiteral("OD_TYPE_DOMAIN");

        case SubIndex::INTEGER24:
            return QStringLiteral("OD_TYPE_INTEGER24");

        case SubIndex::REAL64:
            return QStringLiteral("OD_TYPE_REAL64");

        case SubIndex::INTEGER40:
            return QStringLiteral("OD_TYPE_INTEGER40");

        case SubIndex::INTEGER48:
            return QStringLiteral("OD_TYPE_INTEGER48");

        case SubIndex::INTEGER56:
            return QStringLiteral("OD_TYPE_INTEGER56");

        case SubIndex::INTEGER64:
            return QStringLiteral("OD_TYPE_INTEGER64");

        case SubIndex::UNSIGNED24:
            return QStringLiteral("OD_TYPE_UNSIGNED24");

        case SubIndex::UNSIGNED40:
            return QStringLiteral("OD_TYPE_UNSIGNED40");

        case SubIndex::UNSIGNED48:
            return QStringLiteral("OD_TYPE_UNSIGNED48");

        case SubIndex::UNSIGNED56:
            return QStringLiteral("OD_TYPE_UNSIGNED56");

        case SubIndex::UNSIGNED64:
            return QStringLiteral("OD_TYPE_UNSIGNED64");
    }

    return QString();
}

QString CGenerator::accessToEnumString(uint8_t acces)
{
    QString accessToEnumString;

    if ((acces & SubIndex::READ) != 0)
    {
        accessToEnumString += "OD_ACCESS_READ";
    }
    if ((acces & SubIndex::WRITE) != 0)
    {
        if (!accessToEnumString.isEmpty())
        {
            accessToEnumString += " | ";
        }
        accessToEnumString += "OD_ACCESS_WRITE";
    }
    if ((acces & SubIndex::TPDO) != 0)
    {
        if (!accessToEnumString.isEmpty())
        {
            accessToEnumString += " | ";
        }
        accessToEnumString += "OD_ACCESS_TPDO";
    }
    if ((acces & SubIndex::RPDO) != 0)
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
 * @brief convert a string name
 * @param sub-index
 * @param sub-index number for arrays
 * @return string name for C file
 */
QString CGenerator::stringNameToString(const SubIndex *subIndex)
{
    if (subIndex->subIndex() == 0)
    {
        return varNameToString(subIndex->name()) + "Str";
    }

    return varNameToString(subIndex->name()) + "Str" + QString::number(subIndex->subIndex());
}

/**
 * @brief writes a record's structure in a .h file
 * @param record
 * @param .h file
 */
void CGenerator::writeRecordDefinitionH(Index *index, QTextStream &hFile)
{
    if (index->objectType() != Index::RECORD)
    {
        return;
    }

    QString structName = structNameToString(index->name());
    if (_typeSetTable.contains(structName))
    {
        return;
    }

    hFile << "typedef struct"
          << "  // 0x" << toUHex(index->index()) << "\n{\n";

    QList<SubIndex *> recordFields;
    for (SubIndex *subIndex : index->subIndexes())
    {
        QString dataType = typeToString(subIndex->dataType());
        if (dataType.isEmpty())
        {
            continue;
        }
        recordFields.append(subIndex);
    }
    /*std::sort(recordFields.begin(),
              recordFields.end(),
              [](const SubIndex *a, const SubIndex *b) -> bool
              {
                  return a->length() > b->length();
              });*/
    for (SubIndex *subIndex : recordFields)
    {
        hFile << "    _od_align " << typeToString(subIndex->dataType()) << " " << varNameToString(subIndex->name()) << ";  // sub" << subIndex->subIndex() << "\n";
    }
    hFile << "} " << structName << ";\n\n";

    _typeSetTable.insert(structName);
}

void CGenerator::writeArrayDefinitionH(Index *index, QTextStream &hFile)
{
    if (index->objectType() != Index::ARRAY)
    {
        return;
    }

    QString structName = structNameToString(index->name());
    if (_typeSetTable.contains(structName))
    {
        return;
    }

    hFile << "typedef struct"
          << "  // 0x" << toUHex(index->index()) << "\n{\n";
    hFile << "    _od_align uint8_t sub0;"
          << "\n";
    hFile << "    _od_align " << typeToString(index->subIndex(1)->dataType()) << " data[" << index->subIndexesCount() - 1 << "];"
          << "\n";
    hFile << "} " << structName << ";\n\n";
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
        case Index::VAR:
            if (!index->subIndexExist(0))
            {
                return;
            }

            if (index->subIndex(0)->dataType() == SubIndex::VISIBLE_STRING || index->subIndex(0)->dataType() == SubIndex::OCTET_STRING
                || index->subIndex(0)->dataType() == SubIndex::UNICODE_STRING)
            {
                return;
            }

            dataType = typeToString(index->subIndex(0)->dataType());
            if (dataType.isEmpty())
            {
                break;
            }

            hFile << "    " << dataType << " _od_align " << varNameToString(index->name()) << ";";
            break;

        case Index::ARRAY:
        case Index::RECORD:
            hFile << "    " << structNameToString(index->name()) << " _od_align " << varNameToString(index->name()) << ";";
            break;

        default:
            break;
    }

    hFile << "  // 0x" << toUHex(index->index()) << "\n";
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
            if (index->subIndex(0)->dataType() == SubIndex::VISIBLE_STRING || index->subIndex(0)->dataType() == SubIndex::OCTET_STRING
                || index->subIndex(0)->dataType() == SubIndex::UNICODE_STRING)
            {
                break;
            }
            if (!index->subIndex(0)->value().isValid())
            {
                break;
            }

            cFile << "    OD_RAM." << varNameToString(index->name());
            cFile << " = ";
            cFile << dataToString(index->subIndex(0));
            cFile << ";";
            cFile << "  // 0x" << toUHex(index->index());
            cFile << "\n";
            written++;
            break;

        case Index::Object::RECORD:
            subIndexes = index->subIndexes();
            for (SubIndex *subIndex : qAsConst(subIndexes))
            {
                if (!subIndex->value().isValid())
                {
                    continue;
                }
                cFile << "    OD_RAM." << varNameToString(index->name()) << "." << varNameToString(subIndex->name());
                cFile << " = ";
                cFile << dataToString(subIndex);
                cFile << ";";
                cFile << "  // 0x" << toUHex(index->index()) << "." << subIndex->subIndex();
                cFile << "\n";
                written++;
            }
            break;

        case Index::Object::ARRAY:
            for (int i = 0; i < index->subIndexesCount(); i++)
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
                if (i == 0)
                {
                    cFile << "    OD_RAM." << varNameToString(index->name()) << ".sub0 = ";
                }
                else
                {
                    cFile << "    OD_RAM." << varNameToString(index->name()) << ".data[" << i - 1 << "] = ";
                }
                cFile << dataToString(index->subIndex(i));
                cFile << ";";
                cFile << "  // 0x" << toUHex(index->index()) << "." << i;
                cFile << "\n";
                written++;
            }
            break;

        default:
            break;
    }
    return written;
}

/**
 * @brief writes a record initialization in ram in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeSubentriesList(Index *index, QTextStream &cFile)
{
    cFile << "static const OD_entrySubIndex_t od_Sub" << toUHex(index->index()) << "[] =\n";
    cFile << "{\n";

    switch (index->objectType())
    {
        case Index::VAR:
            if (index->subIndexExist(0))
            {
                writeSubentry(index->subIndex(0), cFile);
            }
            break;

        case Index::ARRAY:
            if (index->subIndexExist(0))
            {
                writeSubentry(index->subIndex(0), cFile);
            }
            if (index->subIndexExist(1))
            {
                writeSubentry(index->subIndex(1), cFile);
            }
            break;

        case Index::RECORD:
            for (SubIndex *subIndex : index->subIndexes())
            {
                writeSubentry(subIndex, cFile);
            }
            break;

        default:
            break;
    }

    cFile << "};\n\n";
}

void CGenerator::writeSubentry(const SubIndex *subIndex, QTextStream &cFile)
{
    cFile << "    {";

    // OD_entrySubIndex_t.subNumber
    cFile << subIndex->subIndex() << ", ";

    // OD_entrySubIndex_t.accessPDOmapping
    cFile << accessToEnumString(subIndex->accessType()) << ", ";

    // OD_entrySubIndex_t.typeObject
    cFile << dataTypeToEnumString(subIndex->dataType()) << ", ";

    // OD_entrySubIndex_t.ptData
    switch (subIndex->index()->objectType())
    {
        case Index::VAR:
            if (subIndex->dataType() == SubIndex::VISIBLE_STRING || subIndex->dataType() == SubIndex::OCTET_STRING || subIndex->dataType() == SubIndex::UNICODE_STRING)
            {
                cFile << "(void*)" << varNameToString(subIndex->name()) << "Str";
            }
            else
            {
                cFile << "(void*)&OD_RAM." << varNameToString(subIndex->name());
            }
            break;

        case Index::ARRAY:
            cFile << "(void*)&OD_RAM." << varNameToString(subIndex->index()->name());
            if (subIndex->subIndex() == 0)
            {
                cFile << ".sub0";
            }
            else
            {
                cFile << ".data";
            }
            break;

        case Index::RECORD:
            cFile << "(void*)&OD_RAM." << varNameToString(subIndex->index()->name());
            cFile << "." << varNameToString(subIndex->name());
            break;

        default:
            break;
    }

    cFile << "},"
          << "\n";
}

/**
 * @brief writes an object dictionary entry initialisation in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeOdCompletionC(Index *index, QTextStream &cFile)
{
    if (!index->subIndexExist(0))
    {
        return;
    }

    cFile << "    "
          << "{";

    // OD_entry_t.index
    cFile << "0x" << toUHex(index->index()) << ", ";

    // OD_entry_t.typeObject
    cFile << objectTypeToEnumString(index->objectType()).leftJustified(16, ' ') << ", ";

    // OD_entry_t.nbSubIndex
    cFile << QString::number(index->subIndexesCount()).toUpper().rightJustified(3, ' ') << ", ";

    // OD_entry_t.subEntries
    cFile << "od_Sub" << toUHex(index->index());

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
    QString value;
    switch (subIndex->dataType())
    {
        case SubIndex::VISIBLE_STRING:
        case SubIndex::OCTET_STRING:
            cFile << "static const char " << stringNameToString(subIndex) << "[]"
                  << " = ";
            value = subIndex->value().toString();
            if (value.startsWith("__") && value.endsWith("__") && value.size() > 4)  // value contain preprocessor value
            {
                value = "STRINGIZE_VALUE_OF(" + value + ")";
            }
            else
            {
                value = "\"" + value + "\"";
            }
            cFile << value << ";\n";
            break;

        case SubIndex::UNICODE_STRING:
            cFile << "static char " << stringNameToString(subIndex) << "[]"
                  << " = ";
            value = subIndex->value().toString();
            value = "\"" + value + "\"";
            cFile << value << ";\n";
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
void CGenerator::writeInitRamC(const QList<Index *> &indexes, QTextStream &cFile)
{
    uint8_t lastObjectType = 0;
    int written = 1;
    for (Index *index : indexes)
    {
        if ((index->objectType() != lastObjectType || index->objectType() == Index::Object::RECORD || index->objectType() == Index::Object::ARRAY) && written != 0)
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
    switch (index->objectType())
    {
        case Index::VAR:
            hFile << "#define OD_" << varNameToString(index->name()).toUpper() << " OD_RAM." << varNameToString(index->name()) << "\n";
            hFile << "#define OD_INDEX" << toUHex(index->index()) << " OD_RAM." << varNameToString(index->name()) << "\n";
            break;

        case Index::ARRAY:
            hFile << "#define OD_" << varNameToString(index->name()).toUpper() << " OD_RAM." << varNameToString(index->name()) << ".data\n";
            hFile << "#define OD_INDEX" << toUHex(index->index()) << " OD_RAM." << varNameToString(index->name()) << ".data\n";

            hFile << "#define OD_" << varNameToString(index->name()).toUpper() << "_COUNT " << index->subIndexesCount() - 1 << "\n";
            hFile << "#define OD_INDEX" << toUHex(index->index()) << "_COUNT " << index->subIndexesCount() - 1 << "\n";
            for (SubIndex *subIndex : index->subIndexes())
            {
                uint8_t numSubIndex = subIndex->subIndex();
                if (numSubIndex == 0)
                {
                    continue;
                }

                hFile << "#define OD_INDEX" << toUHex(index->index()) << "_" << toUHex(numSubIndex);
                hFile << " OD_INDEX" << toUHex(index->index()) << "[" << toUHex(numSubIndex - 1) << "]"
                      << "\n";
            }
            break;

        case Index::RECORD:
            hFile << "#define OD_" << varNameToString(index->name()).toUpper() << " OD_RAM." << varNameToString(index->name()) << "\n";
            hFile << "#define OD_INDEX" << toUHex(index->index()) << " OD_RAM." << varNameToString(index->name()) << "\n";

            for (SubIndex *subIndex : index->subIndexes())
            {
                hFile << "#define OD_INDEX" << toUHex(index->index()) << "_" << toUHex(subIndex->subIndex()) << " OD_INDEX" << toUHex(index->index()) << "."
                      << varNameToString(subIndex->name()) << "\n";
            }
            break;

        default:
            break;
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
                        cFile << "OD_INDEX" << toUHex(index->index());
                        break;

                    case Index::Object::RECORD:
                    case Index::Object::ARRAY:
                        cFile << "OD_INDEX" << toUHex(index->index()) << "_" << QString::number(subIndex->subIndex(), 16);
                        break;

                    default:
                        break;
                }

                uint value = subIndex->value().toUInt() - deviceConfiguration->nodeId().toUInt();
                cFile << " = 0x" << toUHex(value) << "u + "
                      << "nodeId";

                cFile << ";  // " << index->name() << " : " << subIndex->name() << "\n";
            }
        }
    }

    cFile << "}\n";
}

template <typename T>
QString CGenerator::toUHex(T value)
{
    return QString::number(value, 16).toUpper() /*.rightJustified(sizeof(value) * 2, '0')*/;
}
