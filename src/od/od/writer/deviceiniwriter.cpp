#include "deviceiniwriter.h"

DeviceIniWriter::DeviceIniWriter(QTextStream *file)
{
    _file = file;
}

void DeviceIniWriter::writeFileInfo(QMap<QString, QString> fileInfos) const
{
    *_file << "[FileInfo]" << "\n";

    foreach (const QString &key, fileInfos.keys())
    {
        *_file << key << "=" << fileInfos.value(key) << "\n";
    }

    *_file << "\n";
}

void DeviceIniWriter::writeDeviceComissioning(QMap<QString, QString> fileComissionings) const
{
    *_file << "[DeviceComissioning]" << "\n";

    foreach (const QString &key, fileComissionings.keys())
    {
        *_file << key << "=" << fileComissionings.value(key) << "\n";
    }

    *_file << "\n";
}

void DeviceIniWriter::writeDummyUsage(QMap <QString, QString> dummyUsages) const
{
    *_file << "[DummyUsage]" << "\n";

    foreach (const QString &key, dummyUsages.keys())
    {
        *_file << key << "=" << dummyUsages.value(key) << "\n";
    }

    *_file << "\n";
}

void DeviceIniWriter::writeSupportedIndexes(QList<Index *> indexes) const
{
    *_file << "SupportedObjects=" << indexes.count() << "\n";

    int base = 16;
    int i = 1;
    foreach (Index *index, indexes)
    {
        *_file << i << "=" << valueToString(index->index(), base) << "\n";
        i++;
    }
}

void DeviceIniWriter::writeListIndex(QList<Index *> indexes) const
{
    foreach (Index *index, indexes)
    {
        switch (index->objectType())
        {
        case Index::Object::VAR:
            writeIndex(index);
            break;

        case Index::Object::RECORD:
            writeRecord(index);
            break;

        case Index::Object::ARRAY:
            writeArray(index);
            break;
        }
    }
}

void DeviceIniWriter::writeIndex(Index *index) const
{
    SubIndex *subIndex;
    subIndex = index->subIndex(0);

    if (subIndex == nullptr)
        return;

    int base = 16;

    *_file << "[" << QString::number(index->index(), base).toUpper() << "]\n";
    *_file << "ParameterName=" << index->name() << "\n";
    *_file << "ObjectType=" << valueToString(index->objectType(), base) << "\n";
    *_file << "DataType=" << valueToString(subIndex->data().dataType(), base) << "\n";
    *_file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
    *_file << "DefaultValue=" << dataToString(subIndex->data()) << "\n";
    *_file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
    writeLimit(subIndex);
    *_file << "\n";
}

void DeviceIniWriter::writeRecord(Index *index) const
{
    int base = 16;

    *_file << "[" << QString::number(index->index(), base).toUpper() << "]\n";
    *_file << "ParameterName=" << index->name() << "\n";
    *_file << "ObjectType=" << valueToString(index->objectType(), base) << "\n";
    *_file << "SubNumber=" << valueToString(index->subIndexesCount(), base) << "\n";
    *_file << "\n";

    foreach (SubIndex *subIndex, index->subIndexes())
    {
        *_file << "[" << QString::number(index->index(), base).toUpper() << "sub" << subIndex->subIndex() << "]\n";
        *_file << "ParameterName=" << subIndex->name() << "\n";
        *_file << "ObjectType=" << valueToString(Index::Object::VAR, base) << "\n";
        *_file << "DataType=" << valueToString(subIndex->data().dataType(), base) << "\n";
        *_file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
        *_file << "DefaultValue=" << dataToString(subIndex->data()) << "\n";
        *_file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
        writeLimit(subIndex);
        *_file << "\n";
    }
}

void DeviceIniWriter::writeArray(Index *index) const
{
    writeRecord(index);
}

void DeviceIniWriter::writeLimit(SubIndex *subIndex) const
{
    uint8_t flagLimit = subIndex->flagLimit();

    if ((flagLimit & SubIndex::Limit::LOW) == SubIndex::Limit::LOW)
        *_file << "LowLimit=" << subIndex->lowLimit().toString() << "\n";

    if ((flagLimit & SubIndex::Limit::HIGH) == SubIndex::Limit::HIGH)
        *_file << "HighLimit=" << subIndex->highLimit().toString() << "\n";
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
