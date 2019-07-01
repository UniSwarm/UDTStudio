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

#include <QFile>
#include <QMap>
#include <QList>
#include <QRegularExpression>
#include <QDebug>

#include "cgenerator.h"

/**
 * @brief default constructor
 */
CGenerator::CGenerator()
{

}

/**
 * @brief generate OD.h and OD.c files
 * @param object dictionary
 * @param output directory path
 */
void CGenerator::generate(OD *od, const QString &dir) const
{
    generateH(od, dir);
    generateC(od, dir);
}

/**
 * @brief Generate OD.h file
 * @param object dictionary
 */
void CGenerator::generateH(OD *od, const QString &dir) const
{
    QFile hFile(dir + "/od_data.h");

    if (!hFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&hFile);

    out << "/**\n";
    out << " * Generated .h file\n";
    out << " */\n";
    out << "\n";
    out << "#ifndef OD_DATA_H" << "\n";
    out << "#define OD_DATA_H" << "\n";
    out << "\n";
    out << "#include \"sdo.h\"" << "\n";
    out << "\n";
    out << "// == Number of entries in object dictionary ==" << "\n";
    out << "#define OD_NB_ELEMENTS " << od->indexCount() << "\n";
    out << "\n";
    out << "// ===== struct definitions for records =======" << "\n";

    QMap<uint16_t, Index*> indexes = od->indexes();

    foreach (Index *index, indexes)
    {
        writeRecordDefinitionH(index, out);
    }

    out << "// === struct definitions for memory types ===" << "\n";

    //TODO test read access to know wich memory to use (FLASH/RAM)
    out << "struct sOD_RAM" << "\n";
    out << "{" << "\n";

    foreach (Index *index, indexes)
    {
        writeIndexH(index, out);
    }

    out << "};" << "\n";
    out << "\n";

    //TODO declararion of FLASH memory
    out << "// extern declaration for RAM and FLASH struct" << "\n";
    out << "extern const struct sOD_FLASH OD_FLASH;" << "\n";
    out << "\n";
    out << "// ======== extern declaration of OD ========" << "\n";
    out << "extern const OD_entry_t OD[OD_NB_ELEMENTS];" << "\n";
    out << "extern struct sOD_RAM OD_RAM;" << "\n";
    out << "\n";
    out << "// ============== function ==================" << "\n";
    out << "void od_initRam(void);" << "\n";
    out << "\n";
    out << "#endif // OD_DATA_H";
    out << "\n";

    hFile.close();
}

/**
 * @brief Generate OD.c file
 * @param object dictionary
 */
void CGenerator::generateC(OD *od, const QString &dir) const
{
    QFile cFile(dir + "/od_data.c");

    if (!cFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&cFile);

    out << "/**\n";
    out << " * Generated .c file\n";
    out << " */\n";
    out << "\n";
    out << "#include \"od_data.h\"" << "\n";
    out << "\n";
    out << "// ==================== initialization =====================" << "\n";
    out << "struct sOD_RAM OD_RAM;" << "\n";
    out << "\n";

    QMap<uint16_t, Index*> indexes = od->indexes();

    foreach (Index *index, indexes)
    {
        if (index->maxSubIndex() > 0)
        {
            foreach (SubIndex* subIndex, index->subIndexes())
            {
                writeCharLineC(subIndex, out);
            }
            continue;
        }

        writeCharLineC(index->subIndex(0), out);
    }

    out << "\n";
    out << "void od_initRam()" << "\n";
    out << "{";

    uint8_t lastOT = 0;
    foreach (Index *index, indexes)
    {
        if (index->objectType() != lastOT
                || index->objectType() == Index::Object::RECORD
                || index->objectType() == Index::Object::ARRAY)
            out << "\n";
        writeRamLineC(index, out);
        lastOT = index->objectType();
    }

    out << "}" << "\n";
    out << "\n";

    //TODO initialize struct for FLASH memory
    out << "// ==================== record completion =================" << "\n";

    foreach (Index *index, indexes)
    {
        writeRecordCompletionC(index, out);
    }

    out << "// ============ object dicitonary completion ==============" << "\n";
    out << "const OD_entry_t OD[OD_NB_ELEMENTS] = " << "\n";
    out << "{" << "\n";

    foreach (Index *index, indexes)
    {
        writeOdCompletionC(index, out);
    }

    out << "};";
    out << "\n";

    cFile.close();
}

/**
 * @brief converts a data type to a string
 * @param data type
 * @return data type to C format
 */
QString CGenerator::typeToString(const uint16_t &type) const
{
    switch(type)
    {
    case DataStorage::Type::INTEGER8:
       return QLatin1String("int8_t");

    case DataStorage::Type::INTEGER16:
        return QLatin1String("int16_t");

    case DataStorage::Type::INTEGER32:
        return QLatin1String("int32_t");

    case DataStorage::Type::INTEGER64:
        return QLatin1String("int64_t");

    case DataStorage::Type::BOOLEAN:
    case DataStorage::Type::UNSIGNED8:
       return QLatin1String("uint8_t");

    case DataStorage::Type::UNSIGNED16:
        return QLatin1String("uint16_t");

    case DataStorage::Type::UNSIGNED32:
        return QLatin1String("uint32_t");

    case DataStorage::Type::UNSIGNED64:
        return QLatin1String("uint64_t");

    case DataStorage::Type::REAL32:
        return QLatin1String("float32_t");

    case DataStorage::Type::REAL64:
        return QLatin1String("float64_t");

    case DataStorage::Type::VISIBLE_STRING:
        return QLatin1String("vstring_t");

    case DataStorage::Type::OCTET_STRING:
        return QLatin1String("ostring_t");

    default:
        return QLatin1String("");
    }
}

/**
 * @brief modifies variable name
 * @param variable name
 * @return variable name to C format
 */
QString CGenerator::varNameToString(const QString &name) const
{
    QString modified;
    modified = name.toLower();

    for (int i=0; i < modified.count(); i++)
    {
        if (modified[i] == QChar('-'))
            modified[i] = QChar(' ');

        if (modified[i] == QChar(' '))
            modified[i+1] = modified[i+1].toUpper();
    }

    modified = modified.remove(QChar(' '));

    return modified;
}

/**
 * @brief modifies structure name
 * @param structure name
 * @return structure name to C format
 */
QString CGenerator::structNameToString(const QString &name) const
{
    QString modified;
    QString end("_t");
    modified = name.toLower();
    modified = modified.replace(QChar(' '), QChar('_'));
    modified = modified.append(end);
    return modified;
}

/**
 * @brief converts a sub-index's data
 * @param sub-index which contains data
 * @return data to C hexadecimal format
 */
QString CGenerator::dataToString(const SubIndex *subIndex) const
{
    QString data;

    switch (subIndex->data().dataType())
    {
    case DataStorage::Type::INTEGER8:
        data += "0x" + QString::number(subIndex->data().toInt8(), 16).right(2).toUpper();
        break;

    case DataStorage::Type::INTEGER16:
        data = "0x" + QString::number(subIndex->data().toInt16(), 16).right(4).toUpper();
        break;

    case DataStorage::Type::INTEGER32:
        data = "0x" + QString::number(subIndex->data().toInt32(), 16).right(8).toUpper();
        break;

    case DataStorage::Type::INTEGER64:
        data = "0x" + QString::number(subIndex->data().toInt64(), 16).right(16).toUpper();
        break;

    case DataStorage::Type::UNSIGNED8:
        data = "0x" + QString::number(subIndex->data().toUInt8(), 16).toUpper();
        break;

    case DataStorage::Type::UNSIGNED16:
        data = "0x" + QString::number(subIndex->data().toUInt16(), 16).toUpper();
        break;

    case DataStorage::Type::UNSIGNED32:
        data = "0x" + QString::number(subIndex->data().toUInt32(), 16).toUpper();
        break;

    case DataStorage::Type::UNSIGNED64:
        data = "0x" + QString::number(subIndex->data().toUInt64(), 16).toUpper();
        break;

    case DataStorage::Type::REAL32:
        data = "0x" + QString::number(subIndex->data().toFloat32());
        break;

    case DataStorage::Type::REAL64:
        data = "0x" + QString::number(subIndex->data().toFloat64());
        break;

    case DataStorage::Type::OCTET_STRING:
    case DataStorage::Type::VISIBLE_STRING:
        data = stringNameToString(subIndex);
        break;

    default:
        data = "0x0";
    }

    return data;
}

/**
 * @brief converts data type and object type
 * @param sub-index
 * @return a C hexadecimal value coded on 16 bits
 */
QString CGenerator::typeObjectToString(Index *index, uint8_t subIndex, bool isInRecord = false) const
{
    QString typeObject;

    if (isInRecord)
        typeObject = "0x7";

    else
        typeObject = "0x" + QString::number(index->objectType());

    if (index->subIndex(subIndex)->data().dataType() <= 0x000F)
        typeObject += "00";

    else if (index->subIndex(subIndex)->data().dataType() <= 0x00FF)
        typeObject += "0";

    typeObject += QString::number(index->subIndex(subIndex)->data().dataType(), 16).toUpper();

    return typeObject;
}

/**
 * @brief convert a string name
 * @param sub-index
 * @param sub-index number for arrays
 * @return string name for C file
 */
QString CGenerator::stringNameToString(const SubIndex *subIndex) const
{
    QString string;

    if (subIndex->subIndex() == 0)
        string = varNameToString(subIndex->name()) + "Str";
    else
        string = varNameToString(subIndex->name()) + "Str" + QString::number(subIndex->subIndex());

    return string;
}

/**
 * @brief writes a record's structure in a .h file
 * @param record
 * @param .h file
 */
void CGenerator::writeRecordDefinitionH(Index *index, QTextStream &hFile) const
{
    if ( index->objectType() == Index::Object::RECORD)
    {
        hFile << "typedef struct" << "  // 0x" << QString::number(index->index(), 16) << "\n{\n";

        foreach (const SubIndex *subIndex, index->subIndexes())
        {
            hFile << "    " << typeToString(subIndex->data().dataType()) << " "<< varNameToString(subIndex->name()) << ";" << "\n";
        }
        hFile << "} " << structNameToString(index->name()) << ";\n\n";
    }
}

/**
 * @brief writes an index definition in a .h file
 * @param index
 * @param .h file
 */
void CGenerator::writeIndexH(Index *index, QTextStream &hFile) const
{
    switch(index->objectType())
    {
    case Index::Object::VAR:
        hFile << "    " << typeToString(index->subIndex(0)->data().dataType()) << " " << varNameToString(index->name()) << ";";
        break;

    case Index::Object::RECORD:
        hFile << "    " << structNameToString(index->name()) << " " << varNameToString(index->name()) << ";";
        break;

    case Index::Object::ARRAY:
        hFile << "    " << typeToString(index->subIndex(1)->data().dataType()) << " " << varNameToString(index->name());
        hFile << "[" << index->maxSubIndex()-1 << "]" << ";";
        break;
    }

    hFile << "  // 0x" << QString::number(index->index(), 16) << "\n";
}

/**
 * @brief writes an index initialization in RAM in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeRamLineC(Index *index, QTextStream &cFile) const
{
    QMap<uint8_t, SubIndex *> subIndexes;

    switch(index->objectType())
    {
    case Index::Object::VAR:
        cFile << "    " << "OD_RAM." << varNameToString(index->name());
        cFile << " = ";
        cFile << dataToString(index->subIndex(0));
        cFile << ";";
        cFile << "  // 0x" << QString::number(index->index(), 16);
        cFile << "\n";
        break;

    case Index::Object::RECORD:
        subIndexes = index->subIndexes();
        foreach (SubIndex *subIndex, subIndexes)
        {
            cFile << "    " << "OD_RAM." << varNameToString(index->name()) << "." << varNameToString(subIndex->name());
            cFile << " = ";
            cFile << dataToString(subIndex);
            cFile << ";";
            cFile << "  // 0x" << QString::number(index->index(), 16) << "." << subIndex->subIndex();
            cFile << "\n";
        }
        break;

    case Index::Object::ARRAY:
        for (uint8_t i = 1; i<index->subIndexesCount(); i++)
        {
            cFile << "    " << "OD_RAM." << varNameToString(index->name()) << "[" << i - 1 << "]";
            cFile << " = ";
            cFile << dataToString(index->subIndex(i));
            cFile << ";";
            cFile << "  // 0x" << QString::number(index->index(), 16) << "." << i-1;
            cFile << "\n";
        }
        break;
    }
}

/**
 * @brief writes a record initialization in ram in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeRecordCompletionC(Index *index, QTextStream &cFile) const
{
    if ( index->objectType() == Index::Object::RECORD)
    {
        cFile << "const OD_entrySubIndex_t OD_Record" << QString::number(index->index(), 16).toUpper() << "[" << index->maxSubIndex() << "] =\n";
        cFile << "{\n";

        foreach (SubIndex *subIndex, index->subIndexes())
        {
            cFile << "    " << "{(void*)&OD_RAM." << varNameToString(index->name());
            cFile << "." << varNameToString(subIndex->name());
            //TODO PDOmapping
            cFile << ", " << subIndex->data().length() << ", " << typeObjectToString(index, subIndex->subIndex(), true) << ", " << "0x" <<  subIndex->accessType() << ", " << subIndex->subIndex();
            cFile << "}," << "\n";
        }

        cFile << "};\n\n";
    }
}

/**
 * @brief writes an object dictionary entry initialisation in a .c file
 * @param index
 * @param .c file
 */
void CGenerator::writeOdCompletionC(Index *index, QTextStream &cFile) const
{
    cFile << "    " << "{";
    //TODO PDOmapping
    cFile << "0x" << QString::number(index->index(), 16).toUpper() << ", " << "0x";

    switch (index->objectType())
    {
    case Index::Object::VAR:
        cFile << "0";
        break;

    case Index::Object::RECORD:
        cFile << index->maxSubIndex()-1;
        break;

    case Index::Object::ARRAY:
        cFile << index->maxSubIndex()-1;
        break;
    }

    cFile << ", ";

    switch (index->objectType())
    {
    case Index::Object::VAR:
        cFile << "(void*)&OD_RAM." << varNameToString(index->name()) << ", ";
        cFile << index->subIndex(0)->data().length() <<  ", " << typeObjectToString(index, 0) << ", " << "0x" << index->subIndex(0)->accessType();
        break;

    case Index::Object::RECORD:
        cFile << "(void*)OD_Record" << QString::number(index->index(), 16).toUpper() << ", ";
        cFile << "0" <<  ", " << "0x9000" << ", " << "0x0";
        break;

    case Index::Object::ARRAY:
        cFile << "(void*)OD_RAM." << varNameToString(index->name()) << ", ";
        cFile << index->subIndex(1)->data().length() <<  ", " << typeObjectToString(index, 1) << ", " << "0x" << index->subIndex(1)->accessType();
        break;
    }

    cFile << "},";
    cFile << "\n";
}

/**
 * @brief write char[] initialization in a C file
 * @param sub-index
 * @param C file
 * @param sub-index-number for arrays
 */
void CGenerator::writeCharLineC(SubIndex *subIndex, QTextStream &cFile) const
{
    switch (subIndex->data().dataType())
    {
    case DataStorage::Type::VISIBLE_STRING:
    case DataStorage::Type::OCTET_STRING:
        cFile << "const char " << stringNameToString(subIndex) << "[]" << " = " << "\"" << subIndex->data().toString() << "\"" << ";\n";
        break;
    }
}
