    #include <QFile>
#include <QLatin1String>

#include "dcfwriter.h"

DcfWriter::DcfWriter()
{

}

bool indexLessThan(const Index *i1, const Index *i2)
{
    return i1->index() < i2->index();
}


void DcfWriter::write(OD *od, const QString &dir) const
{
    QFile dcfFile(dir + "/out.dcf");

    if (!dcfFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&dcfFile);

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
    file << "DefautValue=" << valueToString(subIndex->data().value().toInt(), base) << "\n";
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
        file << "ParameterName=" << index->name() << "\n";
        file << "ObjectType=" << valueToString(Index::Object::VAR, base) << "\n";
        file << "DataType=" << valueToString(subIndex->data().dataType(), base) << "\n";
        file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
        file << "DefautValue=" << valueToString(subIndex->data().value().toInt(), base) << "\n";
        file << "\n";
    }
}

void DcfWriter::writeArray(Index *index, QTextStream &file) const
{
    writeRecord(index, file);
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
    case SubIndex::Access::CONST:
        return QString("const");

    case SubIndex::Access::READ_ONLY:
        return QString("ro");

    case SubIndex::Access::WRITE_ONLY:
        return QString("ro");

     case SubIndex::Access::READ_WRITE:
        return QString("rw");
    }

    return "";
}

