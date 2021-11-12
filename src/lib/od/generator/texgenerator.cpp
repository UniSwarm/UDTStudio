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

#include "texgenerator.h"
#include <QFileInfo>

/**
 * @brief default constructor
 */
TexGenerator::TexGenerator()
{
}

/**
 * @brief default destructor
 */
TexGenerator::~TexGenerator()
{
}

/**
 * @brief generates object dictionnary .Tex files
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 * @return true
 */
bool TexGenerator::generate(DeviceConfiguration *deviceConfiguration, const QString &filePath)
{
    Q_UNUSED(deviceConfiguration)
    Q_UNUSED(filePath)
    return true;
}

/**
 * @brief generates object dictionnary .Tex files
 * @param device configuration model based on dcf or xdd files
 * @param output file name
 * @return true
 */
bool TexGenerator::generate(DeviceDescription *deviceDescription, const QString &filePath)
{
    QString filePathBaseName = QFileInfo(filePath).path() + "/" + QFileInfo(filePath).baseName();
    QString outCom = filePathBaseName + "Communication." + QFileInfo(filePath).suffix();
    QFile TexComFile(outCom);
    if (!TexComFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString outManu = filePathBaseName + "Manufacturer." + QFileInfo(filePath).suffix();
    QFile TexManuFile(outManu);
    if (!TexManuFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString outStandard = filePathBaseName + "Standardized." + QFileInfo(filePath).suffix();
    QFile TexStandardFile(outStandard);
    if (!TexStandardFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QList<Index *> communication;
    QList<Index *> standardized;
    QList<Index *> manufacturers;

    for (Index *index : deviceDescription->indexes().values())
    {
        uint16_t numIndex = index->index();

        if (numIndex >= 0x1000 && numIndex < 0x2000)
        {
            communication.append(index);
        }
        else if (numIndex >= 0x2000 && numIndex < 0x6000)
        {
            manufacturers.append(index);
        }
        else if (numIndex >= 0x6000 && numIndex < 0x6800)
        {
            standardized.append(index);
        }
    }

    QTextStream out(&TexComFile);
    writeListIndexComm(communication, &out);
    TexComFile.close();

    out.setDevice(&TexManuFile);

    if (deviceDescription->index(0x1000)->subIndex(0)->value().toUInt() == 402)
    {
        writeListIndexManufacturer402(manufacturers, &out);
    }
    else
    {
        writeListIndex(manufacturers, &out);
    }

    TexManuFile.close();

    out.setDevice(&TexStandardFile);
    writeListIndex(standardized, &out);
    TexStandardFile.close();

    return true;
}

/**
 * @brief writes a list of index and these parameters
 * @param list of indexes
 */
void TexGenerator::writeListIndex(const QList<Index *> indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        switch (index->objectType())
        {
        case Index::Object::VAR:
            writeIndex(index, out, false);
            break;

        case Index::Object::RECORD:
            writeRecord(index, out, false);
            break;

        case Index::Object::ARRAY:
            writeArray(index, out, false);
            break;
        }
    }
}

void TexGenerator::writeListIndexComm(const QList<Index *> indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x1000 && numIndex < 0x1400)
        {
            switch (index->objectType())
            {
            case Index::Object::VAR:
                writeIndex(index, out, false);
                break;

            case Index::Object::RECORD:
                writeRecord(index, out, false);
                break;

            case Index::Object::ARRAY:
                writeArray(index, out, false);
                break;
            }
        }
    }

    // Write only Object master without subindex
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x1400 && numIndex < 0x1A04)
        {
            writeIndex(index, out, false);
        }
    }

    // Write generic Object with X
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
         if (numIndex >= 0x1400 && numIndex < 0x1A04)
         {
             if (numIndex == 0x1400 || numIndex == 0x1600 || numIndex == 0x1800 || numIndex == 0x1A00)
             {
                 switch (index->objectType())
                 {
                 case Index::Object::VAR:
                     writeIndex(index, out, true);
                     break;

                 case Index::Object::RECORD:
                     writeRecord(index, out, true);
                     break;

                 case Index::Object::ARRAY:
                     writeArray(index, out, true);
                     break;
                 }
             }
             else
             {
                 writeIndex(index, out, true);
             }
         }
    }
}

void TexGenerator::writeListIndexManufacturer402(const QList<Index *> indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x2000 && numIndex < 0x3000)
        {
            switch (index->objectType())
            {
            case Index::Object::VAR:
                writeIndex(index, out, false);
                break;

            case Index::Object::RECORD:
                writeRecord(index, out, false);
                break;

            case Index::Object::ARRAY:
                writeArray(index, out, false);
                break;
            }
        }
    }

    // Write only Object master without subindex
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x4000 && numIndex < 0x40FF)
        {
            writeIndex(index, out, false);
        }
    }
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if ((numIndex >= 0x4000 && numIndex < 0x4040)
            || (numIndex >= 0x4080 && numIndex < 0x40FF))
        {
            switch (index->objectType())
            {
            case Index::Object::VAR:
                writeIndex(index, out, true);
                break;

            case Index::Object::RECORD:
                writeRecord(index, out, true);
                break;

            case Index::Object::ARRAY:
                writeArray(index, out, true);
                break;
            }
        }
        else if (numIndex >= 0x4040 && numIndex < 0x4080)
        {
            writeIndex(index, out, true);
        }
    }
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x40FF && numIndex < 0x4200)
        {
            switch (index->objectType())
            {
            case Index::Object::VAR:
                writeIndex(index, out, false);
                break;

            case Index::Object::RECORD:
                writeRecord(index, out, false);
                break;

            case Index::Object::ARRAY:
                writeArray(index, out, false);
                break;
            }
        }
    }
}

/**
 * @brief writes an index and these parameters
 * @param index model
 */
void TexGenerator::writeIndex(Index *index, QTextStream *out, bool generic)
{
    SubIndex *subIndex;
    subIndex = index->subIndex(0);

    if (subIndex == nullptr)
    {
        return;
    }

    int base = 16;

    QString nameObject = index->name();
    QString nameFull = index->name();

    if (generic)
    {
        if (index->index() >= 0x1400 && index->index() < 0x1A04)
        {
            nameObject.append("X");
            nameFull.replace(QRegExp("[0-9]"), "X");
            *out << "% " << QString::number(index->index(), base).toUpper().replace(3, 1, "n") << " " << nameFull;
        }
        else
        {
            nameObject.append("X");
            nameFull.replace("a1", "a@");
            *out << "% " << QString::number(index->index(), base).toUpper().replace(1, 1, "n") << " " << nameFull << "X";
        }
    }
    else
    {
        *out << "% " << QString::number(index->index(), base).toUpper() << " " << nameObject;
    }
    *out << "\n";

    nameFull.replace("_", "\\_");

    nameObject.remove(QRegExp("^[a][0-9]"));
    nameObject.remove("_");
    nameObject.remove(" ");
    nameObject.remove("-");
    nameObject.replace("0", "A");
    nameObject.replace("1", "B");
    nameObject.replace("2", "C");
    nameObject.replace("3", "D");
    nameObject.replace("4", "E");
    nameObject.replace("5", "F");
    nameObject.replace("6", "G");
    nameObject.replace("7", "H");
    nameObject.replace("8", "I");
    nameObject.replace("9", "J");

//    QString nameFull = index->name();
//    nameFull.replace("_", "\\_");
//    if (generic)
//    {
//        nameObject.append("X");
//        nameFull.replace("a1", "a@");
//    }

    QString nameCommand = nameObject;
    nameCommand.prepend("\\name");
    QString indexCommand = nameObject;
    indexCommand.prepend("\\index");
    QString subIndexCommand = nameObject;
    subIndexCommand.prepend("\\subIndex");
    QString sectionCommand = nameObject;
    sectionCommand.prepend("\\section");
    QString dispIndexSubCommand = nameObject;
    dispIndexSubCommand.prepend("\\dispIndexSub");
    QString dispIndexNameCommand = nameObject;
    dispIndexNameCommand.prepend("\\dispIndexName");
    QString dispTabCommand = nameObject;
    dispTabCommand.prepend("\\dispTab");

    // nameCommand >> \newcommand{\nameControlword}{a1$\\_$Controlword}%
    *out << "\\newcommand{" << nameCommand << "}";
    *out << "{" << nameFull << "}%";
    *out << "\n";

    // indexCommand >> \newcommand{\indexControlword}{6040}%
    *out << "\\newcommand{" << indexCommand << "}";
    if (generic)
    {
        if (index->index() >= 0x1400 && index->index() < 0x1A04)
        {
            *out << "{" << QString::number(index->index(), base).toUpper().replace(3, 1, "n") << "}%";
        }
        else
        {
            *out << "{" << QString::number(index->index(), base).toUpper().replace(1, 1, "n") << "}%";
        }
    }
    else
    {
        *out << "{" << QString::number(index->index(), base).toUpper() << "}%";
    }

    *out << "\n";

    // subIndexCommand >> \newcommand{\subIndexControlword}{0}%
    *out << "\\newcommand{" << subIndexCommand << "}";
    *out << "{0}%";
    *out << "\n";

    // sectionCommand >> \newcommand{\sectionControlword}{\sectionIndexName{\indexControlword}{\nameControlword}}%
    *out << "\\newcommand{" << sectionCommand << "}";
    *out << "{\\sectionIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // dispIndexSubCommand >> \newcommand{\dispIndexSubControlword}{\displayIndexSub{\indexControlword}{\subIndexControlword}}%
    *out << "\\newcommand{" << dispIndexSubCommand << "}";
    *out << "{\\displayIndexSub";
    *out << "{" << indexCommand << "}";
    *out << "{" << subIndexCommand << "}}%";
    *out << "\n";

    // dispIndexNameCommand >> \newcommand{\dispIndexNameControlword}{\displayIndexName{\indexControlword}{\nameControlword}}%
    *out << "\\newcommand{" << dispIndexNameCommand << "}";
    *out << "{\\displayIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // dispTabCommand >> \newcommand{\dispTabControlword}{\displayTab{\indexControlword}{\subIndexControlword}{\nameControlword}%
    //                   {UINT16}{RW,RPDO}{-}{-}{;}}%
    *out << "\\newcommand{" << dispTabCommand << "}";
    *out << "{\\displayTab";
    *out << "{" << indexCommand << "}";
    *out << "{" << subIndexCommand << "}";
    *out << "{" << nameCommand << "}%";
    *out << "\n";
    *out << "{" << subIndex->dataTypeStr(subIndex->dataType()) << "}";
    *out << "{" << accessToString(subIndex->accessType());
    if (pdoToString(subIndex->accessType()) != "")
    {
        *out << "," << pdoToString(subIndex->accessType()) << "}";
    }
    else
    {
        *out << "}";
    }
    if (subIndex->value().toString().isEmpty())
    {
        *out << "{-}";
    }
    else
    {
        *out << "{" << subIndex->value().toString() << "}";
    }
    *out << "{-}";
    *out << "{";

    writeLimit(subIndex, out);
    *out << "}}%\n";
    *out << "\n";
}

/**
 * @brief writes a record index
 * @param index model
 */
void TexGenerator::writeRecord(Index *index, QTextStream *out, bool generic)
{
    int base = 16;

    QString nameObject = index->name();
    QString nameFull = index->name();

    if (generic)
    {
        if (index->index() >= 0x1400 && index->index() < 0x1A04)
        {
            nameObject.append("X");
            nameFull.replace(QRegExp("[0-9]"), "X");
            *out << "% " << QString::number(index->index(), base).toUpper().replace(3, 1, "n") << " " << nameFull;
        }
        else
        {
            nameObject.append("X");
            nameFull.replace("a1", "a@");
            *out << "% " << QString::number(index->index(), base).toUpper().replace(1, 1, "n") << " " << nameFull << "X";
        }
    }
    else
    {
        *out << "% " << QString::number(index->index(), base).toUpper() << " " << nameObject;
    }
    *out << "\n";

    nameFull.replace("_", "\\_");
    nameObject.remove(QRegExp("^[a][0-9]"));
    nameObject.remove("_");
    nameObject.remove(" ");
    nameObject.remove("-");
    nameObject.replace("0", "A");
    nameObject.replace("1", "B");
    nameObject.replace("2", "C");
    nameObject.replace("3", "D");
    nameObject.replace("4", "E");
    nameObject.replace("5", "F");
    nameObject.replace("6", "G");
    nameObject.replace("7", "H");
    nameObject.replace("8", "I");
    nameObject.replace("9", "J");

//    QString nameFull = index->name();
//    nameFull.replace("_", "\\_");
//    if (generic)
//    {
//        nameObject.append("X");
//        nameFull.replace("a1", "a@");
//    }

    QString nameCommand = nameObject;
    nameCommand.prepend("\\name");
    QString indexCommand = nameObject;
    indexCommand.prepend("\\index");
    QString subIndexCommand = nameObject;
    subIndexCommand.prepend("\\subIndex");
    QString sectionCommand = nameObject;
    sectionCommand.prepend("\\section");
    QString dispIndexSubCommand = nameObject;
    dispIndexSubCommand.prepend("\\dispIndexSub");
    QString dispIndexNameCommand = nameObject;
    dispIndexNameCommand.prepend("\\dispIndexName");

    // nameCommand >> \newcommand{\nameStatusword}{a1$\\_$Statusword}%
    *out << "\\newcommand{" << nameCommand << "}";
    *out << "{" << nameFull << "}%";
    *out << "\n";

    // indexCommand >> \newcommand{\indexStatusword}{6041}%
    *out << "\\newcommand{" << indexCommand << "}";
    if (generic)
    {
        if (index->index() >= 0x1400 && index->index() < 0x1A04)
        {
            *out << "{" << QString::number(index->index(), base).toUpper().replace(3, 1, "n") << "}%";
        }
        else
        {
            *out << "{" << QString::number(index->index(), base).toUpper().replace(1, 1, "n") << "}%";
        }
    }
    else
    {
        *out << "{" << QString::number(index->index(), base).toUpper() << "}%";
    }
    *out << "\n";

    // subIndexCommand >> \newcommand{\subIndexStatusword}{0}%
    *out << "\\newcommand{" << subIndexCommand << "}";
    *out << "{0}%";
    *out << "\n";

    // sectionCommand >> \newcommand{\sectionStatusword}{\sectionIndexName{\indexStatusword}{\nameStatusword}}%
    *out << "\\newcommand{" << sectionCommand << "}";
    *out << "{\\sectionIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // dispIndexSubCommand >> \newcommand{\dispIndexSubStatusword}{\displayIndexSub{\indexStatusword}{\subIndexStatusword}}%
    *out << "\\newcommand{" << dispIndexSubCommand << "}";
    *out << "{\\displayIndexSub";
    *out << "{" << indexCommand << "}";
    *out << "{" << subIndexCommand << "}}%";
    *out << "\n";

    // dispIndexNameCommand >> \newcommand{\dispIndexNameStatusword}{\displayIndexName{\indexStatusword}{\nameStatusword}}%
    *out << "\\newcommand{" << dispIndexNameCommand << "}";
    *out << "{\\displayIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    for (SubIndex *subIndex : index->subIndexes())
    {
        if (subIndex->subIndex() == 0)
        {
            continue;
        }

        QString nameSubObject = subIndex->name();
        if (generic)
        {
            if (index->index() >= 0x1400 && index->index() < 0x1A04)
            {
                *out << "% " << QString::number(index->index(), base).toUpper().replace(3, 1, "n") << "." << QString::number(subIndex->subIndex(), base).toUpper() << " " << nameSubObject;
            }
            else
            {
                *out << "% " << QString::number(index->index(), base).toUpper().replace(1, 1, "n") << "." << QString::number(subIndex->subIndex(), base).toUpper() << " " << nameSubObject;
            }
        }
        else
        {
            *out << "% " << QString::number(index->index(), base).toUpper() << "." << QString::number(subIndex->subIndex(), base).toUpper() << " " << nameSubObject;
        }

        *out << "\n";
        nameSubObject.remove("_");
        nameSubObject.remove(" ");
        nameSubObject.remove("-");
        nameSubObject.replace("0", "A");
        nameSubObject.replace("1", "B");
        nameSubObject.replace("2", "C");
        nameSubObject.replace("3", "D");
        nameSubObject.replace("4", "E");
        nameSubObject.replace("5", "F");
        nameSubObject.replace("6", "G");
        nameSubObject.replace("7", "H");
        nameSubObject.replace("8", "I");
        nameSubObject.replace("9", "J");

        QString nameFull = subIndex->name();
        nameFull.replace("_", "\\_");

        QString subIndexSubCommand = nameSubObject;
        subIndexSubCommand.prepend(nameObject);
        subIndexSubCommand.prepend("\\subIndex");

        QString nameSubCommand = nameSubObject;
        nameSubCommand.prepend(nameObject);
        nameSubCommand.prepend("\\name");

        QString indexSubCommand = nameSubObject;
        indexSubCommand.prepend(nameObject);
        indexSubCommand.prepend("\\index");

        QString paraCommand = nameSubObject;
        paraCommand.prepend(nameObject);
        paraCommand.prepend("\\para");

        QString dispIndexSubSubCommand = nameSubObject;
        dispIndexSubSubCommand.prepend(nameObject);
        dispIndexSubSubCommand.prepend("\\dispIndexSub");

        QString dispIndexNameSubCommand = nameSubObject;
        dispIndexNameSubCommand.prepend(nameObject);
        dispIndexNameSubCommand.prepend("\\dispIndexSubName");

        QString dispTabCommand = nameSubObject;
        dispTabCommand.prepend(nameObject);
        dispTabCommand.prepend("\\dispTab");

        // subIndexCommand >> \newcommand{\subIndexvlDecelerationvlDecelerationdeltaspeed}{1}%
        *out << "\\newcommand{" << subIndexSubCommand << "}";
        *out << "{" << QString::number(subIndex->subIndex(), base).toUpper() << "}%";
        *out << "\n";

        // nameCommand >> \newcommand{\namevlDecelerationvlDecelerationdeltaspeed}{vl$\\_$Deceleration$\\_$delta$\\_$speed}%
        *out << "\\newcommand{" << nameSubCommand << "}";
        *out << "{" << nameFull << "}%";
        *out << "\n";

        // indexCommand >> \newcommand{\indexvlDecelerationvlDecelerationdeltaspeed}{\indexvlDeceleration}%
        *out << "\\newcommand{" << indexSubCommand << "}";
        *out << "{" << indexCommand << "}%";
        *out << "\n";

        // paraCommand >> \newcommand{\paravlMinMaxvlMin}{\paraIndexSubName{\indexvlMinMax}{\subIndexvlMinMaxvlMin}{\namevlMinMaxvlMin}}%
        *out << "\\newcommand{" << paraCommand << "}";
        *out << "{\\paraIndexSubName";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}";
        *out << "{" << nameSubCommand << "}}%";
        *out << "\n";

        // dispIndexSubCommand >> \newcommand{\dispIndexSubvlDecelerationvlDecelerationdeltaspeed}{\displayIndexSub{\indexvlDeceleration}
        //                                 {\subIndexvlDecelerationvlDecelerationdeltaspeed}}%
        *out << "\\newcommand{" << dispIndexSubSubCommand << "}";
        *out << "{\\displayIndexSub";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}}%";
        *out << "\n";

        // dispIndexNameCommand >> \newcommand{\dispIndexSubNamevlDecelerationvlDecelerationdeltaspeed}{\displayIndexSubName{\indexvlDeceleration}
        //                                       {\subIndexvlDecelerationvlDecelerationdeltaspeed}{\namevlDecelerationvlDecelerationdeltaspeed}}%
        *out << "\\newcommand{" << dispIndexNameSubCommand << "}";
        *out << "{\\displayIndexSubName";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}";
        *out << "{" << nameSubCommand << "}}%";
        *out << "\n";

        // dispTabCommand >> \newcommand{\dispTabvlDecelerationvlDecelerationdeltaspeed}{\displayTab{\indexvlDeceleration}
        //                          {\subIndexvlDecelerationvlDecelerationdeltaspeed}{\namevlDecelerationvlDecelerationdeltaspeed}%
        //                   {UINT32}{RW,RPDO}{1}{-}{0x1;0xFFFFFFFF}}%
        *out << "\\newcommand{" << dispTabCommand << "}";
        *out << "{\\displayTab";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}";
        *out << "{" << nameSubCommand << "}%";
        *out << "\n";
        *out << "{" << subIndex->dataTypeStr(subIndex->dataType()) << "}";
        *out << "{" << accessToString(subIndex->accessType());
        if (pdoToString(subIndex->accessType()) != "")
        {
            *out << "," << pdoToString(subIndex->accessType()) << "}";
        }
        else
        {
            *out << "}";
        }
        if (subIndex->value().toString().isEmpty())
        {
            *out << "{-}";
        }
        else
        {
            *out << "{" << subIndex->value().toString() << "}";
        }
        *out << "{-}";
        *out << "{";

        writeLimit(subIndex, out);
        *out << "}}%\n";
    }
    *out << "\n";
}

/**
 * @brief writes an array index
 * @param index model
 */
void TexGenerator::writeArray(Index *index, QTextStream *out, bool generic)
{
    writeRecord(index, out, generic);
}

/**
 * @brief writes high limit and low limit of a sub-index if they exist
 * @param sub index model
 */
void TexGenerator::writeLimit(const SubIndex *subIndex, QTextStream *out)
{
    if (subIndex->hasLowLimit())
    {
        *out << subIndex->lowLimit().toString() << " ";
    }
    *out << "-";
    if (subIndex->hasHighLimit())
    {
        *out << " " << subIndex->highLimit().toString();
    }
}

/**
 * @brief returns an access code to his corresponding string format
 * @param access code
 * @return formated string
 */
QString TexGenerator::accessToString(int access)
{
    switch (access)
    {
    case SubIndex::READ:
    case SubIndex::READ + SubIndex::TPDO:
        return QString("RO");

    case SubIndex::WRITE:
    case SubIndex::WRITE + SubIndex::RPDO:
        return QString("WO");

    case SubIndex::READ + SubIndex::WRITE:
    case SubIndex::READ + SubIndex::WRITE + SubIndex::TPDO + SubIndex::RPDO:
        return QString("RW");

    case SubIndex::READ + SubIndex::WRITE + SubIndex::TPDO:
        return QString("RW");

    case SubIndex::READ + SubIndex::WRITE + SubIndex::RPDO:
        return QString("RW");
    }

    return "";
}

/**
 * @brief return 1 if accessType has a mapping pdo flag, else 0
 * @param 8 bits access type code
 * @return 1 or 0 as a string
 */
QString TexGenerator::pdoToString(uint8_t accessType)
{
    if ((accessType & SubIndex::TPDO) == SubIndex::TPDO)
    {
        return "TPDO";
    }

    if ((accessType & SubIndex::RPDO) == SubIndex::RPDO)
    {
        return "RPDO";
    }

    return "";
}
