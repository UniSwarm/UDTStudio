
#include <QFile>
#include <QMap>
#include <QList>
#include <QRegularExpression>
#include <QDebug>

#include "generator.h"

/**
 * @brief Generator::Generator
 */
Generator::Generator(QString path)
{
    _dir = path;
}

/**
 * @brief Generator::generateH
 * @param od
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
        writeRecordLineH(index, out);
    }

    out << "\n";
    out << "// === struct definitions for memory types ===" << "\n";

    //TODO test read access to know wich memory to use (FLASH/RAM)
    out << "struct sOD_RAM" << "\n";
    out << "{" << "\n";

    foreach (Index *index, indexes)
    {
        writeRamLineH(index, out);
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
 * @brief Generator::generateC
 * @param od
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
        writeRecordCompletion(index, out);
    }

    out << "\n";
    out << "// ============ object dicitonary completion ==============" << "\n";
    out << "const OD_entry_t OD[OD_NB_ELEMENTS] = " << "\n";
    out << "{" << "\n";
    out << "};" << "\n";

    cFile.close();
}

/**
 * @brief Generator::typeToString
 * @param type
 * @return
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

    default:
        return QLatin1String("");
    }
}

/**
 * @brief Generator::varNameToString
 * @param name
 * @return
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
 * @brief Generator::structNameToString
 * @param name
 * @return
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

void Generator::writeRecordLineH(Index *index, QTextStream &out) const
{
    QList<SubIndex*> subIndexes;

    if ( index->objectType() == OD_OBJECT_RECORD)
    {
        out << "typedef struct\n" << "{\n";

        subIndexes = index->subIndexes();
        foreach (const SubIndex *subIndex, subIndexes)
        {
            out << "\t" << typeToString(subIndex->dataType()) << "\t"<< varNameToString(subIndex->parameterName()) << ";" << "\n";
        }
         out << "} " << structNameToString(index->parameterName()) << ";\n";
    }
}

void Generator::writeRamLineH(Index *index, QTextStream &out) const
{
    switch(index->objectType())
    {
    case OD_OBJECT_VAR:
        out << "\t" << typeToString(index->dataType()) << "\t" << varNameToString(index->parameterName()) << ";" << "\n";
        break;

    case OD_OBJECT_RECORD:
        out << "\t" << structNameToString(index->parameterName()) << "\t" << varNameToString(index->parameterName()) << ";" << "\n";
        break;
    }
}

void Generator::writeRamLineC(Index *index, QTextStream &out) const
{
    QList<SubIndex*> subIndexes;
    int cpt = 0;

    switch(index->objectType())
    {
    case OD_OBJECT_VAR:
        out << "\t" << "OD_RAM." << varNameToString(index->parameterName()) << " = ";

        out << dataToString(index);

        out << ";\n";
        break;

    case OD_OBJECT_RECORD:

        subIndexes = index->subIndexes();
        foreach (SubIndex *subIndex, subIndexes)
        {
            out << "\t" << "OD_RAM." << varNameToString(index->parameterName()) << "." << varNameToString(subIndex->parameterName()) << " = ";

            out << dataToString(subIndex);

            out << ";\n";
            cpt++;
        }

        break;
    }
}

void Generator::writeRecordCompletion(Index *index, QTextStream &out) const
{
    if ( index->objectType() == OD_OBJECT_RECORD)
    {
        out << "const OD_entrySubIndex_t OD_Record" << QString::number(index->index()) << "[" << index->nbSubIndex() << "] =\n";
        out << "{\n";
        out << "};\n";
    }
}

