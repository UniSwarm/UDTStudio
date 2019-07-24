#include "deviceiniwriter.h"

DeviceIniWriter::DeviceIniWriter()
{
}

void DeviceIniWriter::writeFileInfo(QMap<QString, QString> fileInfos, QTextStream &file) const
{
    file << "[FileInfo]" << "\n";

    foreach (const QString &key, fileInfos.keys())
    {
        file << key << "=" << fileInfos.value(key) << "\n";
    }

    file << "\n";
}

void DeviceIniWriter::writeDeviceComissioning(QMap<QString, QString> fileComissionings, QTextStream &file) const
{
    file << "[DeviceComissioning]" << "\n";

    foreach (const QString &key, fileComissionings.keys())
    {
        file << key << "=" << fileComissionings.value(key) << "\n";
    }

    file << "\n";
}

void DeviceIniWriter::writeDummyUsage(QTextStream &file) const
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

void DeviceIniWriter::writeSupportedIndexes(QList<Index *> indexes, QTextStream &file) const
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

void DeviceIniWriter::writeListIndex(QList<Index *> indexes, QTextStream &file) const
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

void DeviceIniWriter::writeIndex(Index *index, QTextStream &file) const
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
    file << "DefaultValue=" << dataToString(subIndex->data()) << "\n";
    file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
    writeLimit(subIndex, file);
    file << "\n";
}

void DeviceIniWriter::writeRecord(Index *index, QTextStream &file) const
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
        file << "DefaultValue=" << dataToString(subIndex->data()) << "\n";
        file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
        writeLimit(subIndex, file);
        file << "\n";
    }
}

void DeviceIniWriter::writeArray(Index *index, QTextStream &file) const
{
    writeRecord(index, file);
}

void DeviceIniWriter::writeLimit(SubIndex *subIndex, QTextStream &file) const
{
    uint8_t flagLimit = subIndex->flagLimit();

    if ((flagLimit & SubIndex::Limit::LOW) == SubIndex::Limit::LOW)
        file << "LowLimit=" << subIndex->lowLimit().toString() << "\n";

    if ((flagLimit & SubIndex::Limit::HIGH) == SubIndex::Limit::HIGH)
        file << "HighLimit=" << subIndex->highLimit().toString() << "\n";
}

QString DeviceIniWriter::valueToString(int value, int base) const
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

QString DeviceIniWriter::accessToString(int access) const
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

QString DeviceIniWriter::dataToString(DataStorage data) const
{
    return data.value().toString();
}

QString DeviceIniWriter::pdoToString(uint8_t accessType) const
{
    if ((accessType & SubIndex::Access::TPDO) == SubIndex::Access::TPDO || (accessType & SubIndex::Access::RPDO) == SubIndex::Access::RPDO)
        return QString::number(1);

    return QString::number(0);
}
