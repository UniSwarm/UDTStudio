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
#include <QRegularExpression>

#include "db/odindexdb.h"

/**
 * @brief default constructor
 */
TexGenerator::TexGenerator()
    : _profile(0)
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
    _profile = deviceDescription->index(0x1000)->subIndex(0)->value().toUInt();

    QString filePathBaseName = QFileInfo(filePath).path() + "/" + QFileInfo(filePath).baseName();
    QString outCom = filePathBaseName + "Communication." + QFileInfo(filePath).suffix();
    QFile texComFile(outCom);
    if (!texComFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString outManu = filePathBaseName + "Manufacturer." + QFileInfo(filePath).suffix();
    QFile texManuFile(outManu);
    if (!texManuFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QString outStandard = filePathBaseName + "Standardized." + QFileInfo(filePath).suffix();
    QFile texStandardFile(outStandard);
    if (!texStandardFile.open(QIODevice::WriteOnly))
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

    QTextStream out(&texComFile);
    writeListIndexComm(communication, &out);
    texComFile.close();

    out.setDevice(&texManuFile);

    if (_profile == 402)
    {
        writeListIndexManufacturer402(manufacturers, &out);
    }
    else
    {
        writeListIndex(manufacturers, &out);
    }

    texManuFile.close();

    out.setDevice(&texStandardFile);
    writeListIndex(standardized, &out);
    texStandardFile.close();

    return true;
}

/**
 * @brief writes a list of index and these parameters
 * @param list of indexes
 */
void TexGenerator::writeListIndex(const QList<Index *> &indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        switch (index->objectType())
        {
            case Index::Object::VAR:
                writeVar(index, out, false);
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

void TexGenerator::writeListIndexComm(const QList<Index *> &indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x1000 && numIndex <= 0x1A03)
        {
            switch (index->objectType())
            {
                case Index::Object::VAR:
                    writeVar(index, out, false);
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

void TexGenerator::writeListIndexManufacturer402(const QList<Index *> &indexes, QTextStream *out)
{
    for (Index *index : indexes)
    {
        uint16_t numIndex = index->index();
        if (numIndex >= 0x2000 && numIndex < 0x41FF)
        {
            switch (index->objectType())
            {
                case Index::Object::VAR:
                    writeVar(index, out, false);
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
void TexGenerator::writeVar(Index *index, QTextStream *out, bool generic)
{
    SubIndex *subIndex = nullptr;
    subIndex = index->subIndex(0);

    if (subIndex == nullptr)
    {
        return;
    }

    QString nameForTex = formatNameIndexForTex(index, generic);
    QString nameIndex = formatNameIndex(index, generic);

    // Line 1 : % 1000 Device Type
    *out << "% " << formatIndex(index, generic) << " " << index->name();
    if (generic)
    {
        *out << "X";
    }

    *out << "\n";

    QString nameCommand = nameForTex;
    nameCommand.prepend("\\name");
    QString indexCommand = nameForTex;
    indexCommand.prepend("\\index");
    QString subIndexCommand = nameForTex;
    subIndexCommand.prepend("\\subIndex");
    QString sectionCommand = nameForTex;
    sectionCommand.prepend("\\section");
    QString dispIndexSubCommand = nameForTex;
    dispIndexSubCommand.prepend("\\dispIndexSub");
    QString dispIndexNameCommand = nameForTex;
    dispIndexNameCommand.prepend("\\dispIndexName");
    QString dispTabCommand = nameForTex;
    dispTabCommand.prepend("\\dispTab");

    // Line 2 : \newcommand{\nameDeviceType}{Device Type}%
    *out << "\\newcommand{" << nameCommand << "}";
    *out << "{" << nameIndex << "}%";
    *out << "\n";

    // Line 3 : \newcommand{\indexDeviceType}{1000}%
    *out << "\\newcommand{" << indexCommand << "}";
    *out << "{" << formatIndex(index, generic) << "}%";
    *out << "\n";

    // Line 4 : \newcommand{\subIndexDeviceType}{0}%
    *out << "\\newcommand{" << subIndexCommand << "}";
    *out << "{0}%";
    *out << "\n";

    // Line 5 : \newcommand{\sectionDeviceType}{\sectionIndexName{\indexDeviceType}{\nameDeviceType}}%
    *out << "\\newcommand{" << sectionCommand << "}";
    *out << "{\\sectionIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // Line 6 : \newcommand{\dispIndexSubDeviceType}{\displayIndexSub{\indexDeviceType}{\subIndexDeviceType}}%
    *out << "\\newcommand{" << dispIndexSubCommand << "}";
    *out << "{\\displayIndexSub";
    *out << "{" << indexCommand << "}";
    *out << "{" << subIndexCommand << "}}%";
    *out << "\n";

    // Line 7 : \newcommand{\dispIndexNameDeviceType}{\displayIndexName{\indexDeviceType}{\nameDeviceType}}%
    *out << "\\newcommand{" << dispIndexNameCommand << "}";
    *out << "{\\displayIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // Line 8 : \newcommand{\dispTabDeviceType}{\displayTabVar{\indexDeviceType}{\subIndexDeviceType}{\nameDeviceType}%
    //          {UINT32}{RO}{402}{-}{-}}%
    *out << "\\newcommand{" << dispTabCommand << "}";
    *out << "{\\displayTabVar";
    *out << "{" << dataTypeStr(subIndex) << "}";

    // Access Type
    writeAccessType(subIndex, out);

    // Default Type
    writeDefaultValue(subIndex, out);

    // Limits / Range
    *out << "{";
    writeLimit(subIndex, out);
    *out << "}";

    // Unit
    *out << "{";
    writeUnit(subIndex, out);
    *out << "}";

    *out << "}%\n";
    *out << "\n";
}

/**
 * @brief writes a record index
 * @param index model
 */
void TexGenerator::writeRecord(Index *index, QTextStream *out, bool generic)
{
    int base = 16;

    QString nameForTex = formatNameIndexForTex(index, generic);
    QString nameIndex = formatNameIndex(index, generic);

    // Line 1 : % 4n06 a@_Motor_statusX
    *out << "% " << formatIndex(index, generic) << " " << index->name();
    if (generic)
    {
        *out << "X";
    }

    *out << "\n";

    QString nameCommand = nameForTex;
    nameCommand.prepend("\\name");
    QString indexCommand = nameForTex;
    indexCommand.prepend("\\index");
    QString subIndexCommand = nameForTex;
    subIndexCommand.prepend("\\subIndex");
    QString sectionCommand = nameForTex;
    sectionCommand.prepend("\\section");
    QString dispIndexSubCommand = nameForTex;
    dispIndexSubCommand.prepend("\\dispIndexSub");
    QString dispIndexNameCommand = nameForTex;
    dispIndexNameCommand.prepend("\\dispIndexName");

    // Line 2 : \newcommand{\nameMotorstatusX}{a@\_Motor\_status}%
    *out << "\\newcommand{" << nameCommand << "}";
    *out << "{" << nameIndex << "}%";
    *out << "\n";

    // Line 3 : \newcommand{\indexMotionstatusX}{4n00}%
    *out << "\\newcommand{" << indexCommand << "}";
    *out << "{" << formatIndex(index, generic) << "}%";
    *out << "\n";

    // Line 4 : \newcommand{\subIndexMotorstatusX}{0}%
    *out << "\\newcommand{" << subIndexCommand << "}";
    *out << "{0}%";
    *out << "\n";

    // Line 5 : \newcommand{\sectionMotorstatusX}{\sectionIndexName{\indexMotorstatusX}{\nameMotorstatusX}}%
    *out << "\\newcommand{" << sectionCommand << "}";
    *out << "{\\sectionIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // Line 6 : \newcommand{\dispIndexSubMotorstatusX}{\displayIndexSub{\indexMotorstatusX}{\subIndexMotorstatusX}}%
    *out << "\\newcommand{" << dispIndexSubCommand << "}";
    *out << "{\\displayIndexSub";
    *out << "{" << indexCommand << "}";
    *out << "{" << subIndexCommand << "}}%";
    *out << "\n";

    // Line 7 : \newcommand{\dispIndexNameMotorstatusX}{\displayIndexName{\indexMotorstatusX}{\nameMotorstatusX}}%
    *out << "\\newcommand{" << dispIndexNameCommand << "}";
    *out << "{\\displayIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // Line 8 :\newcommand{\dispTabMotionstatusXError}{\displayTabRecord{\indexMotionstatusX}{\subIndexMotionstatusXError}{\nameMotionstatusXError}%
    //          {UINT16}{RO,TPDO}{-}{-}{-}{\dispTabLineArraySubIndexMotionstatusX}}%
    QString dispTabCommand = nameForTex;
    dispTabCommand.prepend("\\dispTab");

    *out << "\\newcommand{" << dispTabCommand << "}";
    *out << "{\\displayTabRecord";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}";
    *out << "{RECORD}";
    *out << "{\\dispTabLineRecordSubIndex" + nameForTex + "}}%\n";

    QStringList lineRecordSubIndex;

    for (SubIndex *subIndex : index->subIndexes())
    {
        if ((subIndex->subIndex() == 0) && !((index->index() >= 0x1600 && index->index() <= 0x1603) || (index->index() >= 0x1A00 && index->index() <= 0x1A03)))
        {
            continue;
        }

        // Line 1 : % 4n06.1 Command
        *out << "% " << formatIndex(index, generic) << "." << QString::number(subIndex->subIndex(), base).toUpper() << " " << subIndex->name();
        *out << "\n";

        QString nameSubForTex = formatNameSubIndexForTex(subIndex->name());

        QString nameFull = subIndex->name();
        nameFull.replace("_", "\\_");

        QString subIndexSubCommand = nameSubForTex;
        subIndexSubCommand.prepend(nameForTex);
        subIndexSubCommand.prepend("\\subIndex");

        QString nameSubCommand = nameSubForTex;
        nameSubCommand.prepend(nameForTex);
        nameSubCommand.prepend("\\name");

        QString indexSubCommand = nameSubForTex;
        indexSubCommand.prepend(nameForTex);
        indexSubCommand.prepend("\\index");

        QString paraCommand = nameSubForTex;
        paraCommand.prepend(nameForTex);
        paraCommand.prepend("\\para");

        lineRecordSubIndex.append("\\displayLineRecordSubIndex{" + subIndexSubCommand + "}" + "{\\hyperref[" + indexCommand + subIndexSubCommand + "]{"
                                  + nameSubCommand + "}}" + "{" + dataTypeStr(subIndex) + "}%\n");

        QString dispIndexSubSubCommand = nameSubForTex;
        dispIndexSubSubCommand.prepend(nameForTex);
        dispIndexSubSubCommand.prepend("\\dispIndexSub");

        QString dispIndexNameSubCommand = nameSubForTex;
        dispIndexNameSubCommand.prepend(nameForTex);
        dispIndexNameSubCommand.prepend("\\dispIndexSubName");

        QString dispTabCommand = nameSubForTex;
        dispTabCommand.prepend(nameForTex);
        dispTabCommand.prepend("\\dispTab");

        // Line 2 : \newcommand{\subIndexMotorstatusXCommand}{1}%
        *out << "\\newcommand{" << subIndexSubCommand << "}";
        *out << "{" << QString::number(subIndex->subIndex(), base).toUpper() << "}%";
        *out << "\n";

        // Line 3 : \newcommand{\nameMotorstatusXCommand}{Command}%
        *out << "\\newcommand{" << nameSubCommand << "}";
        *out << "{" << nameFull << "}%";
        *out << "\n";

        // Line 4 : \newcommand{\indexMotorstatusXCommand}{\indexMotorstatusX}%
        *out << "\\newcommand{" << indexSubCommand << "}";
        *out << "{" << indexCommand << "}%";
        *out << "\n";

        // Line 5 : \newcommand{\paraMotorstatusXCommand}{\paraIndexSubName{\indexMotorstatusX}{\subIndexMotorstatusXCommand}{\nameMotorstatusXCommand}}%
        *out << "\\newcommand{" << paraCommand << "}";
        *out << "{\\paraIndexSubName";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}";
        *out << "{" << nameSubCommand << "}}%";
        *out << "\n";

        // Line 6 :
        // \newcommand{\dispIndexSubNameMotorstatusXCommand}{\displayIndexSubName{\indexMotorstatusX}{\subIndexMotorstatusXCommand}{\nameMotorstatusXCommand}}%
        *out << "\\newcommand{" << dispIndexSubSubCommand << "}";
        *out << "{\\displayIndexSub";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}}%";
        *out << "\n";

        // Line 7 :
        // \newcommand{\dispIndexSubNameMotionstatusXError}{\displayIndexSubName{\indexMotionstatusX}{\subIndexMotionstatusXError}{\nameMotionstatusXError}}%
        *out << "\\newcommand{" << dispIndexNameSubCommand << "}";
        *out << "{\\displayIndexSubName";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}";
        *out << "{" << nameSubCommand << "}}%";
        *out << "\n";

        // Line 8 : \newcommand{\dispTabMotorstatusXCommand}{\displayTab{\indexMotorstatusX}{\subIndexMotorstatusXCommand}{\nameMotorstatusXCommand}%
        //          {INT16}{RO, TPDO}{-}{-}{-}}%
        *out << "\\newcommand{" << dispTabCommand << "}";
        *out << "{\\displayTabRecordParameters";
        *out << "{" << dataTypeStr(subIndex) << "}";

        // Access Type
        writeAccessType(subIndex, out);

        // Default Type
        writeDefaultValue(subIndex, out);

        // Limits / Range
        *out << "{";
        writeLimit(subIndex, out);
        *out << "}";

        // Unit
        *out << "{";
        writeUnit(subIndex, out);
        *out << "}";

        *out << "}%\n";
    }

    // Group end : \newcommand{\dispTabLineArraySubIndexMotionstatusX}{%
    //              \displayLineArraySubIndex{\subIndexMotionstatusXError}{\nameMotionstatusXError}%
    //              }%
    *out << "%List subindex\n";
    *out << "\\newcommand{\\dispTabLineRecordSubIndex" << nameForTex << "}{%\n";
    for (int i = 0; i < lineRecordSubIndex.size(); ++i)
    {
        *out << lineRecordSubIndex.at(i);
    }
    *out << "}%\n";
    *out << "\n";
}

/**
 * @brief writes an array index
 * @param index model
 */
void TexGenerator::writeArray(Index *index, QTextStream *out, bool generic)
{
    int base = 16;

    QString nameForTex = formatNameIndexForTex(index, generic);
    QString nameIndex = formatNameIndex(index, generic);

    // Line 1 : % 4n00 a1_Motion_statusX
    *out << "% " << formatIndex(index, generic) << " " << index->name();
    if (generic)
    {
        *out << "X";
    }

    *out << "\n";

    QString nameCommand = nameForTex;
    nameCommand.prepend("\\name");
    QString indexCommand = nameForTex;
    indexCommand.prepend("\\index");
    QString subIndexCommand = nameForTex;
    subIndexCommand.prepend("\\subIndex");
    QString sectionCommand = nameForTex;
    sectionCommand.prepend("\\section");
    QString dispIndexSubCommand = nameForTex;
    dispIndexSubCommand.prepend("\\dispIndexSub");
    QString dispIndexNameCommand = nameForTex;
    dispIndexNameCommand.prepend("\\dispIndexName");

    // Line 2 : \newcommand{\nameMotionstatusX}{a@\_Motion\_status}%
    *out << "\\newcommand{" << nameCommand << "}";
    *out << "{" << nameIndex << "}%";
    *out << "\n";

    // Line 3 : \newcommand{\indexMotionstatusX}{4n00}%
    *out << "\\newcommand{" << indexCommand << "}";
    *out << "{" << formatIndex(index, generic) << "}%";
    *out << "\n";

    // Line 4 : \newcommand{\subIndexMotionstatusX}{0}%
    *out << "\\newcommand{" << subIndexCommand << "}";
    *out << "{0}%";
    *out << "\n";

    // Line 5 : \newcommand{\sectionMotionstatusX}{\sectionIndexName{\indexMotionstatusX}{\nameMotionstatusX}}%
    *out << "\\newcommand{" << sectionCommand << "}";
    *out << "{\\sectionIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // Line 6 : \newcommand{\dispIndexSubMotionstatusX}{\displayIndexSub{\indexMotionstatusX}{\subIndexMotionstatusX}}%
    *out << "\\newcommand{" << dispIndexSubCommand << "}";
    *out << "{\\displayIndexSub";
    *out << "{" << indexCommand << "}";
    *out << "{" << subIndexCommand << "}}%";
    *out << "\n";

    // Line 7 : \newcommand{\dispIndexNameMotionstatusX}{\displayIndexName{\indexMotionstatusX}{\nameMotionstatusX}}%
    *out << "\\newcommand{" << dispIndexNameCommand << "}";
    *out << "{\\displayIndexName";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}}%";
    *out << "\n";

    // Line 8 :\newcommand{\dispTabMotionstatusXError}{\displayTab{\indexMotionstatusX}{\subIndexMotionstatusXError}{\nameMotionstatusXError}%
    //          {UINT16}{RO,TPDO}{-}{-}{-}{\dispTabLineArraySubIndexMotionstatusX}}%
    SubIndex *subIndex = index->subIndex(1);
    QString dispTabCommand = nameForTex;
    dispTabCommand.prepend("\\dispTab");

    *out << "\\newcommand{" << dispTabCommand << "}";
    *out << "{\\displayTabArray";
    *out << "{" << indexCommand << "}";
    *out << "{" << nameCommand << "}";
    *out << "{ARRAY}";
    *out << "{" << dataTypeStr(subIndex) << "}";

    // Access Type
    writeAccessType(subIndex, out);

    // Limits / Range
    *out << "{";
    writeLimit(subIndex, out);
    *out << "}";

    // Unit
    *out << "{";
    writeUnit(subIndex, out);
    *out << "}";

    *out << "{\\dispTabLineArraySubIndex" + nameForTex + "}}%\n";

    QStringList lineArraySubIndex;

    for (SubIndex *subIndex : index->subIndexes())
    {
        if (subIndex->subIndex() == 0)
        {
            continue;
        }

        // Line 1 : % 4n00.1 Error
        *out << "% " << formatIndex(index, generic) << "." << QString::number(subIndex->subIndex(), base).toUpper() << " " << subIndex->name();
        *out << "\n";

        QString nameSubForTex = formatNameSubIndexForTex(subIndex->name());

        QString nameFull = subIndex->name();
        nameFull.replace("_", "\\_");

        QString subIndexSubCommand = nameSubForTex;
        subIndexSubCommand.prepend(nameForTex);
        subIndexSubCommand.prepend("\\subIndex");

        QString nameSubCommand = nameSubForTex;
        nameSubCommand.prepend(nameForTex);
        nameSubCommand.prepend("\\name");

        QString indexSubCommand = nameSubForTex;
        indexSubCommand.prepend(nameForTex);
        indexSubCommand.prepend("\\index");

        lineArraySubIndex.append("\\displayLineArraySubIndex{" + subIndexSubCommand + "}{" + nameSubCommand + "}%\n");

        QString paraCommand = nameSubForTex;
        paraCommand.prepend(nameForTex);
        paraCommand.prepend("\\para");

        QString dispIndexSubSubCommand = nameSubForTex;
        dispIndexSubSubCommand.prepend(nameForTex);
        dispIndexSubSubCommand.prepend("\\dispIndexSub");

        QString dispIndexNameSubCommand = nameSubForTex;
        dispIndexNameSubCommand.prepend(nameForTex);
        dispIndexNameSubCommand.prepend("\\dispIndexSubName");

        QString dispTabCommand = nameSubForTex;
        dispTabCommand.prepend(nameForTex);
        dispTabCommand.prepend("\\dispTab");

        // Line 2 : \newcommand{\subIndexMotionstatusXError}{1}%
        *out << "\\newcommand{" << subIndexSubCommand << "}";
        *out << "{" << QString::number(subIndex->subIndex()).toUpper() << "}%\n";

        // Line 3 : \newcommand{\nameMotionstatusXError}{Error}%
        *out << "\\newcommand{" << nameSubCommand << "}";
        *out << "{" << nameFull << "}%\n";

        // Line 4 : \newcommand{\dispIndexSubMotionstatusXError}{\displayIndexSub{\indexMotionstatusX}{\subIndexMotionstatusXError}}%
        *out << "\\newcommand{" << dispIndexSubSubCommand << "}";
        *out << "{\\displayIndexSub";
        *out << "{" << indexCommand << "}";
        *out << "{" << subIndexSubCommand << "}}%";
        *out << "\n";
    }

    // Group end : \newcommand{\dispTabLineArraySubIndexMotionstatusX}{%
    //              \displayLineArraySubIndex{\subIndexMotionstatusXError}{\nameMotionstatusXError}%
    //              }%
    *out << "%List subindex\n";
    *out << "\\newcommand{\\dispTabLineArraySubIndex" << nameForTex << "}{%\n";
    for (int i = 0; i < lineArraySubIndex.size(); ++i)
    {
        *out << lineArraySubIndex.at(i);
    }

    *out << "}%\n";
    *out << "\n";
}

void TexGenerator::writeUnit(const SubIndex *subIndex, QTextStream *out) const
{
    double scale = ODIndexDb::scale(subIndex->index()->index(), subIndex->subIndex(), static_cast<quint16>(_profile));
    QString scaleDiv;
    if (scale != 1.0 && scale != 0.0)
    {
        scaleDiv = QString::number(scale);
    }

    QString unit = ODIndexDb::unit(subIndex->index()->index(), subIndex->subIndex(), static_cast<quint16>(_profile));
    unit.replace("°", "$^{\\circ}$");
    unit.replace("µ", "$\\mu$");
    unit.replace("%", "\\%");
    unit = unit.trimmed();

    if (unit.isEmpty() && scaleDiv.isEmpty())
    {
        unit = "-";
    }
    *out << (scaleDiv + " " + unit).trimmed();
}

/**
 * @brief writes high limit and low limit of a sub-index if they exist
 * @param sub index model
 */
void TexGenerator::writeLimit(const SubIndex *subIndex, QTextStream *out)
{
    if (subIndex->hasLowLimit() || subIndex->hasHighLimit())
    {
        *out << "[";
    }
    if (subIndex->hasLowLimit())
    {
        *out << subIndex->lowLimit().toString() << " ";
    }
    *out << "-";
    if (subIndex->hasHighLimit())
    {
        *out << " " << subIndex->highLimit().toString();
    }
    if (subIndex->hasLowLimit() || subIndex->hasHighLimit())
    {
        *out << "]";
    }
}

void TexGenerator::writeAccessType(const SubIndex *subIndex, QTextStream *out)
{
    *out << "{" << accessStr(subIndex->accessType());
    if (pdoAccessStr(subIndex->accessType()) != "")
    {
        *out << "," << pdoAccessStr(subIndex->accessType()) << "}";
    }
    else
    {
        *out << "}";
    }
}

void TexGenerator::writeDefaultValue(const SubIndex *subIndex, QTextStream *out)
{
    if (subIndex->value().toString().isEmpty() || subIndex->value().toString().startsWith("__"))
    {
        *out << "{-}";
    }
    else
    {
        *out << "{" << subIndex->value().toString().replace("_", "\\_") << "}";
    }
}

QString TexGenerator::formatNameIndex(Index *index, bool generic)
{
    QString nameIndex = index->name();

    if (generic)
    {
        if (index->index() >= 0x1400 && index->index() < 0x1A04)
        {
            nameIndex.replace(QRegularExpression("[0-9]"), "X");
        }
        else
        {
            nameIndex.replace("a1", "a@");
        }
    }

    nameIndex.replace("_", "\\_");
    return nameIndex;
}

QString TexGenerator::formatNameIndexForTex(Index *index, bool generic)
{
    QString nameForTex = index->name();

    if (generic)
    {
        nameForTex.append("X");
    }
    nameForTex = toCamelCase(nameForTex);

    nameForTex.remove(QRegularExpression("^[a][0-9]"));
    nameForTex = formatNameSubIndexForTex(nameForTex);
    return nameForTex;
}

QString TexGenerator::formatNameSubIndexForTex(QString nameSubIndex)
{
    nameSubIndex = toCamelCase(nameSubIndex);

    nameSubIndex.remove("_");
    nameSubIndex.remove(" ");
    nameSubIndex.remove("-");
    nameSubIndex.replace("0", "A");
    nameSubIndex.replace("1", "B");
    nameSubIndex.replace("2", "C");
    nameSubIndex.replace("3", "D");
    nameSubIndex.replace("4", "E");
    nameSubIndex.replace("5", "F");
    nameSubIndex.replace("6", "G");
    nameSubIndex.replace("7", "H");
    nameSubIndex.replace("8", "I");
    nameSubIndex.replace("9", "J");

    return nameSubIndex;
}

QString TexGenerator::formatIndex(Index *index, bool generic)
{
    int base = 16;
    QString str;

    if (generic)
    {
        if (index->index() >= 0x1400 && index->index() < 0x1A04)
        {
            str = QString::number(index->index(), base).toUpper().replace(3, 1, "n");
        }
        else
        {
            str = QString::number(index->index(), base).toUpper().replace(1, 1, "n");
        }
    }
    else
    {
        str = QString::number(index->index(), base).toUpper();
    }

    return str;
}

QString TexGenerator::toCamelCase(QString &name)
{

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList parts = name.split('_', QString::SkipEmptyParts);
#else
    QStringList parts = name.split('_', Qt::SkipEmptyParts);
#endif
    for (int i = 1; i < parts.size(); ++i)
    {
        parts[i].replace(0, 1, parts[i][0].toUpper());
    }
    return parts.join("");
}

QString TexGenerator::dataTypeStr(SubIndex *subIndex) const
{
    if (ODIndexDb::isQ1516(subIndex->index()->index(), subIndex->subIndex(), _profile))
    {
        return "Q15.16";
    }
    return SubIndex::dataTypeStr(subIndex->dataType());
}

/**
 * @brief returns an access code to his corresponding string format
 * @param access code
 * @return formated string
 */
QString TexGenerator::accessStr(int access) const
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
QString TexGenerator::pdoAccessStr(uint8_t accessType) const
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
