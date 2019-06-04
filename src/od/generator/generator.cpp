
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

    out << "\n";
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
    out << "#endif // OD_H" << "\n";

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

    out << "\n";
    out << "// ============ object dicitonary completion ==============" << "\n";
    out << "const OD_entry_t OD[OD_NB_ELEMENTS] = " << "\n";
    out << "{" << "\n";

    foreach (Index *index, indexes)
    {
        writeOdCompletionC(index, out);
    }

    out << "};" << "\n";

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
       return QLatin1String("uint8_t\t");

    case OD_TYPE_UNSIGNED16:
        return QLatin1String("uint16_t");

    case OD_TYPE_UNSIGNED32:
        return QLatin1String("uint32_t");

    case OD_TYPE_UNSIGNED64:
        return QLatin1String("uint64_t");

    case OD_TYPE_REAL32:
        return QLatin1String("float32_t)");

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
        if(modified[i] == QChar(' '))
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
QString Generator::dataToString(const SubIndex *index) const
{
    QString data;

    //TODO change data parameters to handle arrays
    switch (index->dataType())
    {
    case OD_TYPE_INTEGER8:
        data = "0x" + QString::number(index->data(0)->toInt8());
        break;

    case OD_TYPE_INTEGER16:
        data = "0x" + QString::number(index->data(0)->toInt16());
        break;

    case OD_TYPE_INTEGER32:
        data = "0x" + QString::number(index->data(0)->toInt32());
        break;

    case OD_TYPE_INTEGER64:
        data = "0x" + QString::number(index->data(0)->toInt64());
        break;

    case OD_TYPE_UNSIGNED8:
        data = "0x" + QString::number(index->data(0)->toUInt8());
        break;

    case OD_TYPE_UNSIGNED16:
        data = "0x" + QString::number(index->data(0)->toUInt16());
        break;

    case OD_TYPE_UNSIGNED32:
        data = "0x" + QString::number(index->data(0)->toUInt32());
        break;

    case OD_TYPE_UNSIGNED64:
        data = "0x" + QString::number(index->data(0)->toUInt64());
        break;

    case OD_TYPE_REAL32:
        data = "0x" + QString::number(index->data(0)->toFloat32());
        break;

    case OD_TYPE_REAL64:
        data = "0x" + QString::number(index->data(0)->toFloat64());
        break;

    default:
        data = "";
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
            hFile << "\t" << typeToString(subIndex->dataType()) << "\t"<< varNameToString(subIndex->parameterName()) << ";" << "\n";
        }
         hFile << "} " << structNameToString(index->parameterName()) << ";\n";
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
        hFile << "\t" << typeToString(index->dataType()) << "\t" << varNameToString(index->parameterName()) << ";" << "\n";
        break;

    case OD_OBJECT_RECORD:
        hFile << "\t" << structNameToString(index->parameterName()) << "\t" << varNameToString(index->parameterName()) << ";" << "\n";
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
    int cpt = 0;

    switch(index->objectType())
    {
    case OD_OBJECT_VAR:
        cFile << "\t" << "OD_RAM." << varNameToString(index->parameterName()) << " = ";

        cFile << dataToString(index);

        cFile << ";\n";
        break;

    case OD_OBJECT_RECORD:

        subIndexes = index->subIndexes();
        foreach (SubIndex *subIndex, subIndexes)
        {
            cFile << "\t" << "OD_RAM." << varNameToString(index->parameterName()) << "." << varNameToString(subIndex->parameterName()) << " = ";

            cFile << dataToString(subIndex);

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
        cFile << "const OD_entrySubIndex_t OD_Record" << QString::number(index->index()) << "[" << index->nbSubIndex() << "] =\n";
        cFile << "{\n";

        foreach (SubIndex *subIndex, index->subIndexes())
        {
            cFile << "\t" << "{(void*)&OD_RAM." << varNameToString(index->parameterName());
            cFile << "." << varNameToString(subIndex->parameterName());
            //TODO PDOmapping
            cFile << ", " << subIndex->length() << ", " << typeObjectToString(subIndex) << ", " << "0x" <<  subIndex->accessType() << "},";
            cFile << "\n";
        }

        cFile << "};\n";
    }
}

/**
 * @brief writes an object dictionary entry initialisation in a .c file
 * @param index
 * @param .c file
 */
void Generator::writeOdCompletionC(Index *index, QTextStream &cFile) const
{
    cFile << "\t" << "{";
    //TODO PDOmapping
    cFile << "0x" << index->index() << ", " << "0x";

    switch (index->objectType())
    {
    case OD_OBJECT_VAR:
        cFile << "0";
        break;

    case OD_OBJECT_RECORD:
        cFile << index->nbSubIndex()-1;
        break;

    case OD_OBJECT_ARRAY:
        cFile << index->nbSubIndex();
        break;
    }

    cFile << ", ";

    switch (index->objectType())
    {
    case OD_OBJECT_VAR:
        cFile << "(void*)&OD_RAM." << varNameToString(index->parameterName());
        break;

    case OD_OBJECT_RECORD:
        cFile << "(void*)OD_Record" << index->index();
        break;
    }

    cFile << ", ";
    cFile << index->length() << ", " << typeObjectToString(index) << ", " << "0x" << index->accessType();
    cFile << "},";
    cFile << "\n";
}

