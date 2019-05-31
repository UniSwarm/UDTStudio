
#include <QFile>
#include <QMap>
#include <QList>
#include <QRegularExpression>
#include <QDebug>

#include "generator.h"

/**
 * @brief Generator::Generator
 */
Generator::Generator()
{

}

/**
 * @brief Generator::generateH
 * @param od
 */
void Generator::generateH(OD *od) const
{
    QFile hFile("/home/alexis/Documents/code/uCANopen/OD.h");

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
        out << writeRecordLineH(index);
    }

    out << "\n";
    out << "// === struct definitions for memory types ===" << "\n";

    //TODO test read access to know wich memory to use (FLASH/RAM)
    out << "struct sOD_RAM" << "\n";
    out << "{" << "\n";

    foreach (Index *index, indexes)
    {
        out << writeRamLineH(index);
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

QString Generator::writeRecordLineH(Index *index) const
{
    QString line;
    QList<SubIndex*> subIndexes;

    if ( index->objectType() == OD_OBJECT_RECORD)
    {
        line.append("typedef struct\n");
        line.append("{\n");

        subIndexes = index->subIndexes();
        foreach (const SubIndex *subIndex, subIndexes)
        {
            line.append("\t");
            line.append(typeToString(subIndex->dataType()));
            line.append("\t");
            line.append(varNameToString(subIndex->parameterName()));
            line.append(";\n");
        }
        line.append("} ");
        line.append(structNameToString(index->parameterName()));
        line.append(";\n");
    }

    return line;
}

QString Generator::writeRamLineH(Index *index) const
{
    QString line;

    switch(index->objectType())
    {
    case OD_OBJECT_VAR:
        line.append("\t").append(typeToString(index->dataType())).append("\t").append(varNameToString(index->parameterName()));
        line.append(";").append("\n");
        break;

    case OD_OBJECT_RECORD:
        line.append("\t").append(structNameToString(index->parameterName())).append("\t");
        line.append(varNameToString(index->parameterName())).append(";").append("\n");
        break;
    }

    return line;
}
