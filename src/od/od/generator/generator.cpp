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

#include "generator.h"

/**
 * @brief constructor: set where files will be generated
 * @param directory path
 */
Generator::Generator(QString path)
{
    _dir = path;
}

/**
 * @brief Generate OD.h file
 * @param object dictionary
 */
void Generator::generateH(OD *od) const
{
    QFile hFile(_dir + "/OD.h");

    if (!hFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&hFile);
    out << "\n";
    out << "#ifndef OD_H" << "\n";
    out << "#define OD_H" << "\n";
    out << "\n";
    out << "#include \"SDO.h\"" << "\n";
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
    out << "\n";
    out << "// ============== function ==================" << "\n";
    out << "void OD_initRam();" << "\n";
    out << "\n";
    out << "#endif // OD_H";

    hFile.close();
}

/**
 * @brief Generate OD.c file
 * @param object dictionary
 */
void Generator::generateC(OD *od) const
{
    QFile cFile(_dir + "/OD.c");

    if (!cFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&cFile);
    out << "\n";
    out << "#include \"OD.h\"" << "\n";
    out << "\n";
    out << "// ==================== initialization =====================" << "\n";
    out << "// struct sOD_RAM OD_RAM;" << "\n";
    out << "struct sOD_RAM OD_RAM;" << "\n";
    out << "\n";
    out << "void OD_initRam()" << "\n";
    out << "{" << "\n";

    QMap<uint16_t, Index*> indexes = od->indexes();
    QList<SubIndex*> subIndexes;

    foreach (Index *index, indexes)
    {
        writeRamLineC(index, out);
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

    cFile.close();
}

/**
 * @brief converts a data type to a string
 * @param data type
 * @return data type to C format
 */
QString Generator::typeToString(const uint16_t &type) const
{
    switch(type)
    {
    case OD_TYPE_INTEGER8:
       return QLatin1String("int8_t");

    case OD_TYPE_INTEGER16:
        return QLatin1String("int16_t");

    case OD_TYPE_INTEGER32:
        return QLatin1String("int32_t");

    case OD_TYPE_INTEGER64:
        return QLatin1String("int64_t");

    case OD_TYPE_UNSIGNED8:
       return QLatin1String("uint8_t");

    case OD_TYPE_UNSIGNED16:
        return QLatin1String("uint16_t");

    case OD_TYPE_UNSIGNED32:
        return QLatin1String("uint32_t");

    case OD_TYPE_UNSIGNED64:
        return QLatin1String("uint64_t");

    case OD_TYPE_REAL32:
        return QLatin1String("float32_t");

    case OD_TYPE_REAL64:
        return QLatin1String("float64_t");

    case OD_TYPE_VISIBLE_STRING:
        return QLatin1String("vstring_t");

    default:
        return QLatin1String("");
    }
}

/**
 * @brief modifies variable name
 * @param variable name
 * @return variable name to C format
 */
QString Generator::varNameToString(const QString &name) const
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
QString Generator::structNameToString(const QString &name) const
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
QString Generator::dataToString(const SubIndex *index, uint8_t subNumber) const
{
    QString data;

    //TODO change data parameters to handle arrays
    switch (index->dataType())
    {
    case OD_TYPE_INTEGER8:
        data += "0x" + QString::number(index->data(subNumber)->toInt8(), 16).right(2).toUpper();
        break;

    case OD_TYPE_INTEGER16:
        data = "0x" + QString::number(index->data(subNumber)->toInt16(), 16).right(4).toUpper();
        break;

    case OD_TYPE_INTEGER32:
        data = "0x" + QString::number(index->data(subNumber)->toInt32(), 16).right(8).toUpper();
        break;

    case OD_TYPE_INTEGER64:
        data = "0x" + QString::number(index->data(subNumber)->toInt64(), 16).right(16).toUpper();
        break;

    case OD_TYPE_UNSIGNED8:
        data = "0x" + QString::number(index->data(subNumber)->toUInt8(), 16).toUpper();
        break;

    case OD_TYPE_UNSIGNED16:
        data = "0x" + QString::number(index->data(subNumber)->toUInt16(), 16).toUpper();
        break;

    case OD_TYPE_UNSIGNED32:
        data = "0x" + QString::number(index->data(subNumber)->toUInt32(), 16).toUpper();
        break;

    case OD_TYPE_UNSIGNED64:
        data = "0x" + QString::number(index->data(subNumber)->toUInt64(), 16).toUpper();
        break;

    case OD_TYPE_REAL32:
        data = "0x" + QString::number(index->data(subNumber)->toFloat32());
        break;

    case OD_TYPE_REAL64:
        data = "0x" + QString::number(index->data(subNumber)->toFloat64());
        break;

    case OD_TYPE_VISIBLE_STRING:
        data = index->data(subNumber)->toVString();

        if (data.left(2) != "0x")
        {
            data.prepend("\"");
            data.append("\"");
        }

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
QString Generator::typeObjectToString(const SubIndex *subIndex) const
{
    QString typeObject;

    typeObject = "0x" + QString::number(subIndex->objectType());

    if (subIndex->dataType() <= 0x000F)
        typeObject += "00";

    else if (subIndex->dataType() <= 0x00FF)
        typeObject += "0";

    typeObject += QString::number(subIndex->dataType());

    return typeObject;
}

/**
 * @brief writes a record's structure in a .h file
 * @param record
 * @param .h file
 */
void Generator::writeRecordDefinitionH(Index *index, QTextStream &hFile) const
{
    QList<SubIndex*> subIndexes;

    if ( index->objectType() == OD_OBJECT_RECORD)
    {
        hFile << "typedef struct\n" << "{\n";

        subIndexes = index->subIndexes();
        foreach (const SubIndex *subIndex, subIndexes)
        {
            hFile << "    " << typeToString(subIndex->dataType()) << " "<< varNameToString(subIndex->parameterName()) << ";" << "\n";
        }
         hFile << "} " << structNameToString(index->parameterName()) << ";\n\n";
    }
}

/**
 * @brief writes an index definition in a .h file
 * @param index
 * @param .h file
 */
void Generator::writeIndexH(Index *index, QTextStream &hFile) const
{
    switch(index->objectType())
    {
    case OD_OBJECT_VAR:
        hFile << "    " << typeToString(index->dataType()) << " " << varNameToString(index->parameterName()) << ";" << "\n";
        break;

    case OD_OBJECT_RECORD:
        hFile << "    " << structNameToString(index->parameterName()) << " " << varNameToString(index->parameterName()) << ";" << "\n";
        break;

    case OD_OBJECT_ARRAY:
        hFile << "    " << typeToString(index->dataType()) << " " << varNameToString(index->parameterName());
        hFile << "[" << index->nbSubIndex()-1 << "]" << ";" << "\n";
        break;
    }
}

/**
 * @brief writes an index initialization in RAM in a .c file
 * @param index
 * @param .c file
 */
void Generator::writeRamLineC(Index *index, QTextStream &cFile) const
{
    QList<SubIndex*> subIndexes;
    QList<DataType*> datas;

    int cpt = 0;

    switch(index->objectType())
    {
    case OD_OBJECT_VAR:
        cFile << "    " << "OD_RAM." << varNameToString(index->parameterName());
        cFile << " = ";
        cFile << dataToString(index, 0);
        cFile << ";\n";
        break;

    case OD_OBJECT_RECORD:
        subIndexes = index->subIndexes();
        foreach (SubIndex *subIndex, subIndexes)
        {
            cFile << "    " << "OD_RAM." << varNameToString(index->parameterName()) << "." << varNameToString(subIndex->parameterName());
            cFile << " = ";
            cFile << dataToString(subIndex, 0);
            cFile << ";\n";
            cpt++;
        }
        break;

    case OD_OBJECT_ARRAY:
        datas = index->datas();
        datas.removeFirst();
        cpt = 0;
        foreach (DataType *data, datas)
        {
            cFile << "    " << "OD_RAM." << varNameToString(index->parameterName()) << "[" << cpt << "]";
            cFile << " = ";
            cFile << dataToString(index, cpt+1);
            cFile << ";\n";
            cpt++;
        }
        break;
    }
}
/**
 * @brief writes a record initialization in ram in a .c file
 * @param index
 * @param .c file
 */
void Generator::writeRecordCompletionC(Index *index, QTextStream &cFile) const
{
    if ( index->objectType() == OD_OBJECT_RECORD)
    {
        cFile << "const OD_entrySubIndex_t OD_Record" << QString::number(index->index(), 16).toUpper() << "[" << index->nbSubIndex() << "] =\n";
        cFile << "{\n";

        foreach (SubIndex *subIndex, index->subIndexes())
        {
            cFile << "    " << "{(void*)&OD_RAM." << varNameToString(index->parameterName());
            cFile << "." << varNameToString(subIndex->parameterName());
            //TODO PDOmapping
            cFile << ", " << subIndex->length() << ", " << typeObjectToString(subIndex) << ", " << "0x" <<  subIndex->accessType() << ", " << subIndex->subNumber();
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
void Generator::writeOdCompletionC(Index *index, QTextStream &cFile) const
{
    cFile << "    " << "{";
    //TODO PDOmapping
    cFile << "0x" << QString::number(index->index(), 16).toUpper() << ", " << "0x";

    switch (index->objectType())
    {
    case OD_OBJECT_VAR:
        cFile << "0";
        break;

    case OD_OBJECT_RECORD:
        cFile << index->nbSubIndex()-1;
        break;

    case OD_OBJECT_ARRAY:
        cFile << index->nbSubIndex()-1;
        break;
    }

    cFile << ", ";

    switch (index->objectType())
    {
    case OD_OBJECT_VAR:
        cFile << "(void*)&OD_RAM." << varNameToString(index->parameterName());
        break;

    case OD_OBJECT_RECORD:
        cFile << "(void*)OD_Record" << QString::number(index->index(), 16).toUpper();
        break;

    case OD_OBJECT_ARRAY:
        cFile << "(void*)OD_RAM." << varNameToString(index->parameterName());
        break;
    }

    cFile << ", ";
    cFile << index->length() << ", " << typeObjectToString(index) << ", " << "0x" << index->accessType();
    cFile << "},";
    cFile << "\n";
}

