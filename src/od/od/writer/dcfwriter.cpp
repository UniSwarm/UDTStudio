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

#include "dcfwriter.h"

#include <QFile>
#include <QLatin1String>

DcfWriter::DcfWriter()
{

}

bool indexLessThan(const Index *i1, const Index *i2)
{
    return i1->index() < i2->index();
}


void DcfWriter::write(DeviceModel *od, const QString &dir) const
{
    QFile dcfFile(dir + "/out.dcf");

    if (!dcfFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&dcfFile);

    writeFileInfo(od->fileInfos(), out);
    writeDummyUsage(out);

    QList<Index *> mandatories;
    QList<Index *> optionals;
    QList<Index *> manufacturers;

    foreach (Index *index, od->indexes().values())
    {
        uint16_t numIndex = index->index();

        if (numIndex == 0x1000 || numIndex == 0x1001 || numIndex == 0x1018)
            mandatories.append(index);

        else if (numIndex >= 0x2000 && numIndex < 0x6000)
            manufacturers.append(index);

        else
            optionals.append(index);
    }

    qSort(mandatories.begin(), mandatories.end(), indexLessThan);
    qSort(optionals.begin(), optionals.end(), indexLessThan);
    qSort(manufacturers.begin(), manufacturers.end(), indexLessThan);

    out << "[MandatoryObjects]" << "\n";
    writeSupportedIndexes(mandatories, out);
    out << "\n";
    writeListIndex(mandatories, out);

    out << "[OptionalObjects]" << "\n";
    writeSupportedIndexes(optionals, out);
    out << "\n";
    writeListIndex(optionals, out);

    out << "[ManufacturerObjects]" << "\n";
    writeSupportedIndexes(manufacturers, out);
    out << "\n";
    writeListIndex(manufacturers, out);
}

void DcfWriter::writeFileInfo(QMap<QString, QString> fileInfos, QTextStream &file) const
{
    file << "[FileInfo]" << "\n";

    foreach (const QString &key, fileInfos.keys())
    {
        file << key << "=" << fileInfos.value(key) << "\n";
    }

    file << "\n";
}

void DcfWriter::writeDummyUsage(QTextStream &file) const
{
    file << "[DummyUsage]\n";
    file << "Dummy0001=0\n";
    file << "Dummy0002=1\n";
    file << "Dummy0003=1\n";
    file << "Dummy0004=1\n";
    file << "Dummy0005=1\n";
    file << "Dummy0006=1\n";
    file << "Dummy0007=1\n";
    file << "\n";
}

void DcfWriter::writeSupportedIndexes(QList<Index *> indexes, QTextStream &file) const
{
    file << "SupportedObjects=" << indexes.count() << "\n";

    int base = 16;
    int i = 1;
    foreach (Index *index, indexes)
    {
        file << i << "=" << valueToString(index->index(), base) << "\n";
        i++;
    }
}

void DcfWriter::writeListIndex(QList<Index *> indexes, QTextStream &file) const
{
    foreach (Index *index, indexes)
    {
        switch (index->objectType())
        {
        case Index::Object::VAR:
            writeIndex(index, file);
            break;

        case Index::Object::RECORD:
            writeRecord(index, file);
            break;

        case Index::Object::ARRAY:
            writeArray(index, file);
            break;
        }
    }
}

void DcfWriter::writeIndex(Index *index, QTextStream &file) const
{
    SubIndex *subIndex;
    subIndex = index->subIndex(0);

    if (subIndex == nullptr)
        return;

    int base = 16;

    file << "[" << QString::number(index->index(), base).toUpper() << "]\n";
    file << "ParameterName=" << index->name() << "\n";
    file << "ObjectType=" << valueToString(index->objectType(), base) << "\n";
    file << "DataType=" << valueToString(subIndex->data().dataType(), base) << "\n";
    file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
    file << "DefaultValue=" << dataToString(subIndex->data(), base) << "\n";
    file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
    writeLimit(subIndex, file);
    file << "\n";
}

void DcfWriter::writeRecord(Index *index, QTextStream &file) const
{
    int base = 16;

    file << "[" << QString::number(index->index(), base).toUpper() << "]\n";
    file << "ParameterName=" << index->name() << "\n";
    file << "ObjectType=" << valueToString(index->objectType(), base) << "\n";
    file << "SubNumber=" << valueToString(index->subIndexesCount(), base) << "\n";
    file << "\n";

    foreach (SubIndex *subIndex, index->subIndexes())
    {
        file << "[" << QString::number(index->index(), base).toUpper() << "sub" << subIndex->subIndex() << "]\n";
        file << "ParameterName=" << subIndex->name() << "\n";
        file << "ObjectType=" << valueToString(Index::Object::VAR, base) << "\n";
        file << "DataType=" << valueToString(subIndex->data().dataType(), base) << "\n";
        file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
        file << "DefaultValue=" << dataToString(subIndex->data(), base) << "\n";
        file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
        writeLimit(subIndex, file);
        file << "\n";
    }
}

void DcfWriter::writeArray(Index *index, QTextStream &file) const
{
    writeRecord(index, file);
}

void DcfWriter::writeLimit(SubIndex *subIndex, QTextStream &file) const
{
    uint8_t flagLimit = subIndex->flagLimit();

    if ((flagLimit & SubIndex::Limit::LOW) == SubIndex::Limit::LOW)
        file << "LowLimit=" << subIndex->lowLimit().toString() << "\n";

    if ((flagLimit & SubIndex::Limit::HIGH) == SubIndex::Limit::HIGH)
        file << "HighLimit=" << subIndex->highLimit().toString() << "\n";
}

QString DcfWriter::valueToString(int value, int base) const
{
    switch (base)
    {
    case 10:
        return QString(value);

    case 16:
        return QString("0x" + QString::number(value, base).toUpper());
    }

    return QString("");
}

QString DcfWriter::accessToString(int access) const
{
    switch (access)
    {
    case SubIndex::Access::READ:
    case SubIndex::Access::READ + SubIndex::Access::TPDO:
        return QString("ro");

    case SubIndex::Access::WRITE:
    case SubIndex::Access::WRITE + SubIndex::Access::RPDO:
        return QString("ro");

    case SubIndex::Access::READ + SubIndex::Access::WRITE:
    case SubIndex::Access::READ + SubIndex::Access::WRITE + SubIndex::Access::TPDO + SubIndex::Access::RPDO:
        return QString("rw");

    case SubIndex::Access::READ + SubIndex::Access::WRITE + SubIndex::Access::TPDO:
        return QString("rwr");

    case SubIndex::Access::READ + SubIndex::Access::WRITE + SubIndex::Access::RPDO:
        return QString("rww");
    }

    return "";
}

QString DcfWriter::dataToString(DataStorage data, int base) const
{
    QString string;

    switch (base)
    {
    case 8:
        string += "0";
        break;

    case 16:
        string += "0x";
        break;
    }

    switch (data.dataType())
    {
    case DataStorage::Type::INTEGER8:
        if (data.toInt8() < 0)
            return QString::number(data.toInt8());

        string += QString::number(data.toInt8(), base).toUpper();
        break;


    case DataStorage::Type::INTEGER16:
        if (data.toInt16() < 0)
            return QString::number(data.toInt16());

        string += QString::number(data.toInt16(), base).toUpper();
        break;

    case DataStorage::Type::INTEGER32:
        if (data.toInt32() < 0)
            return QString::number(data.toInt32());

        string += QString::number(data.toInt32(), base).toUpper();
        break;

    case DataStorage::Type::INTEGER64:
        if (data.toInt64() < 0)
            return QString::number(data.toInt64());

        string += QString::number(data.toInt64(), base).toUpper();
        break;

    case DataStorage::Type::UNSIGNED8:
        string += QString::number(data.toUInt8(), base).toUpper();
        break;

    case DataStorage::Type::UNSIGNED16:
        string += QString::number(data.toUInt16(), base).toUpper();
        break;

    case DataStorage::Type::UNSIGNED32:
        string += QString::number(data.toUInt32(), base).toUpper();
        break;

    case DataStorage::Type::UNSIGNED64:
        string += QString::number(data.toUInt64(), base).toUpper();
        break;

    case DataStorage::Type::REAL32:
        string += QString::number(data.toFloat32());
        break;

    case DataStorage::Type::REAL64:
        string += QString::number(data.toFloat64());
        break;

    case DataStorage::Type::OCTET_STRING:
    case DataStorage::Type::VISIBLE_STRING:
        return data.value().toString();
    }

    return string;
}

QString DcfWriter::pdoToString(uint8_t accessType) const
{
    if ((accessType & SubIndex::Access::TPDO) == SubIndex::Access::TPDO || (accessType & SubIndex::Access::RPDO) == SubIndex::Access::RPDO)
        return QString::number(1);

    return QString::number(0);
}
