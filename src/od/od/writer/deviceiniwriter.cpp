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

#include "deviceiniwriter.h"

DeviceIniWriter::DeviceIniWriter(QTextStream *file)
{
    _file = file;
}

bool indexLessThan(const Index *i1, const Index *i2)
{
    return i1->index() < i2->index();
}

void DeviceIniWriter::writeObjects(const DeviceModel *deviceModel) const
{
    QList<Index *> mandatories;
    QList<Index *> optionals;
    QList<Index *> manufacturers;

    foreach (Index *index, deviceModel->indexes().values())
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

    *_file  << "[MandatoryObjects]" << "\n";
    writeSupportedIndexes(mandatories);
    *_file  << "\n";
    writeListIndex(mandatories);

    *_file  << "[OptionalObjects]" << "\n";
    writeSupportedIndexes(optionals);
    *_file  << "\n";
    writeListIndex(optionals);

    *_file  << "[ManufacturerObjects]" << "\n";
    writeSupportedIndexes(manufacturers);
    *_file  << "\n";
    writeListIndex(manufacturers);
}

void DeviceIniWriter::writeFileInfo(const QMap<QString, QString> &fileInfos) const
{
    *_file << "[FileInfo]" << "\n";

    writeStringMap(fileInfos);
}

void DeviceIniWriter::writeDeviceComissioning(const QMap<QString, QString> &deviceComissionings) const
{
    *_file << "[DeviceComissioning]" << "\n";

    writeStringMap(deviceComissionings);
}

void DeviceIniWriter::writeDeviceInfo(const QMap<QString, QString> &deviceInfos) const
{
    *_file << "[DeviceInfo]" << "\n";

    writeStringMap(deviceInfos);
}

void DeviceIniWriter::writeDummyUsage(const QMap<QString, QString> &dummyUsages) const
{
    *_file << "[DummyUsage]" << "\n";

    writeStringMap(dummyUsages);
}

void DeviceIniWriter::writeSupportedIndexes(const QList<Index *> &indexes) const
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

void DeviceIniWriter::writeListIndex(const QList<Index *> indexes) const
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
    *_file << "DataType=" << valueToString(subIndex->dataType(), base) << "\n";
    *_file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
    *_file << "DefaultValue=" << dataToString(subIndex->value()) << "\n";
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
        *_file << "DataType=" << valueToString(subIndex->dataType(), base) << "\n";
        *_file << "AccessType=" << accessToString(subIndex->accessType()) << "\n";
        *_file << "DefaultValue=" << dataToString(subIndex->value()) << "\n";
        *_file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\n";
        writeLimit(subIndex);
        *_file << "\n";
    }
}

void DeviceIniWriter::writeArray(Index *index) const
{
    writeRecord(index);
}

void DeviceIniWriter::writeLimit(const SubIndex *subIndex) const
{
    uint8_t flagLimit = subIndex->flagLimit();

    if ((flagLimit & SubIndex::Limit::LOW) == SubIndex::Limit::LOW)
        *_file << "LowLimit=" << subIndex->lowLimit().toString() << "\n";

    if ((flagLimit & SubIndex::Limit::HIGH) == SubIndex::Limit::HIGH)
        *_file << "HighLimit=" << subIndex->highLimit().toString() << "\n";
}

void DeviceIniWriter::writeStringMap(const QMap<QString, QString> &map) const
{
    foreach (const QString &key, map.keys())
    {
        *_file << key << "=" << map.value(key) << "\n";
    }

    *_file << "\n";
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

QString DeviceIniWriter::dataToString(const QVariant &value) const
{
    return value.toString();
}

QString DeviceIniWriter::pdoToString(uint8_t accessType) const
{
    if ((accessType & SubIndex::Access::TPDO) == SubIndex::Access::TPDO || (accessType & SubIndex::Access::RPDO) == SubIndex::Access::RPDO)
        return QString::number(1);

    return QString::number(0);
}
