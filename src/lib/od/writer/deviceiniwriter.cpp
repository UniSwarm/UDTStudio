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

#include "deviceiniwriter.h"

#include <QCollator>
#include <QDateTime>

/**
 * @brief constructor
 * @param file
 */
DeviceIniWriter::DeviceIniWriter(QTextStream *file)
{
    _file = file;
    _isDescription = false;
}

/**
 * @brief compares 2 index numbers
 * @param index number 1
 * @param index number 2
 * @return true if i1 < i2, else faulse
 */
bool indexLessThan(const Index *i1, const Index *i2)
{
    return i1->index() < i2->index();
}

/**
 * @brief writes all the indexes and sub-indexes
 * @param device model
 */
void DeviceIniWriter::writeObjects(const DeviceModel *deviceModel) const
{
    QList<Index *> mandatories;
    QList<Index *> optionals;
    QList<Index *> manufacturers;

    for (Index *index : deviceModel->indexes().values())
    {
        uint16_t numIndex = index->index();

        if (numIndex == 0x1000 || numIndex == 0x1001 || numIndex == 0x1018)
        {
            mandatories.append(index);
        }
        else if (numIndex >= 0x2000 && numIndex < 0x6000)
        {
            manufacturers.append(index);
        }
        else
        {
            optionals.append(index);
        }
    }

    std::sort(mandatories.begin(), mandatories.end(), indexLessThan);
    std::sort(optionals.begin(), optionals.end(), indexLessThan);
    std::sort(manufacturers.begin(), manufacturers.end(), indexLessThan);

    *_file << "[MandatoryObjects]"
           << "\r\n";
    writeSupportedIndexes(mandatories);
    *_file << "\r\n";
    writeListIndex(mandatories);

    *_file << "[OptionalObjects]"
           << "\r\n";
    writeSupportedIndexes(optionals);
    *_file << "\r\n";
    writeListIndex(optionals);

    *_file << "[ManufacturerObjects]"
           << "\r\n";
    writeSupportedIndexes(manufacturers);
    *_file << "\r\n";
    writeListIndex(manufacturers);
}

/**
 * @brief writes files infos
 * @param map of file infos
 */
void DeviceIniWriter::writeFileInfo(QMap<QString, QString> fileInfos) const
{
    *_file << "[FileInfo]"
           << "\r\n";

    QString date = QDateTime::currentDateTime().toString("MM:dd:yyyy");
    QString time = QDateTime::currentDateTime().toString("hh:mmAP");

    fileInfos.insert("ModificationDate", date);
    fileInfos.insert("ModificationTime", time);

    writeStringMap(fileInfos);
}

/**
 * @brief writes device comissionings
 * @param map of device comissionings
 */
void DeviceIniWriter::writeDeviceComissioning(const QMap<QString, QString> &deviceComissionings) const
{
    *_file << "[DeviceComissioning]"
           << "\r\n";

    writeStringMap(deviceComissionings);
}

/**
 * @brief writes device infos
 * @param map of device infos
 */
void DeviceIniWriter::writeDeviceInfo(const QMap<QString, QString> &deviceInfos) const
{
    *_file << "[DeviceInfo]"
           << "\r\n";

    writeStringMap(deviceInfos);
}

/**
 * @brief writes dummy usages
 * @param map of dummy usages
 */
void DeviceIniWriter::writeDummyUsage(const QMap<QString, QString> &dummyUsages) const
{
    *_file << "[DummyUsage]"
           << "\r\n";

    writeStringMap(dummyUsages);
}

void DeviceIniWriter::writeComments(const QMap<QString, QString> &comments) const
{
    *_file << "[Comments]"
           << "\r\n";

    *_file << "Lines=" << comments.size() << "\r\n";

    writeStringMap(comments);
}

/**
 * @brief writes supported indexes fields
 * @param list of index
 */
void DeviceIniWriter::writeSupportedIndexes(const QList<Index *> &indexes) const
{
    *_file << "SupportedObjects=" << indexes.count() << "\r\n";

    int base = 16;
    int i = 1;
    for (Index *index : indexes)
    {
        *_file << i << "=" << valueToString(index->index(), base) << "\r\n";
        i++;
    }
}

/**
 * @brief writes a list of index and these parameters
 * @param list of indexes
 */
void DeviceIniWriter::writeListIndex(const QList<Index *> indexes) const
{
    for (Index *index : indexes)
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

/**
 * @brief writes an index and these parameters
 * @param index model
 */
void DeviceIniWriter::writeIndex(Index *index) const
{
    SubIndex *subIndex;
    subIndex = index->subIndex(0);

    if (subIndex == nullptr)
    {
        return;
    }

    int base = 16;

    *_file << "[" << QString::number(index->index(), base).toUpper() << "]\r\n";
    *_file << "ParameterName=" << index->name() << "\r\n";
    *_file << "ObjectType=" << valueToString(index->objectType(), base, 1) << "\r\n";
    *_file << "DataType=" << valueToString(subIndex->dataType(), base, 4) << "\r\n";
    *_file << "AccessType=" << accessToString(subIndex->accessType()) << "\r\n";
    if (subIndex->value().isValid())
    {
        *_file << "DefaultValue=" << defaultValue(subIndex) << "\r\n";
    }
    *_file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\r\n";
    if (subIndex->objFlags() != 0)
    {
        *_file << "ObjFlags=" << valueToString(subIndex->objFlags(), base) << "\r\n";
    }
    writeLimit(subIndex);
    *_file << "\r\n";
}

/**
 * @brief writes a record index
 * @param index model
 */
void DeviceIniWriter::writeRecord(Index *index) const
{
    int base = 16;

    *_file << "[" << QString::number(index->index(), base).toUpper() << "]\r\n";
    *_file << "ParameterName=" << index->name() << "\r\n";
    *_file << "ObjectType=" << valueToString(index->objectType(), base) << "\r\n";
    *_file << "SubNumber=" << valueToString(index->subIndexesCount()) << "\r\n";
    *_file << "\r\n";

    for (SubIndex *subIndex : index->subIndexes())
    {
        *_file << "[" << QString::number(index->index(), base).toUpper() << "sub" << QString::number(subIndex->subIndex(), 16).toUpper() << "]\r\n";
        *_file << "ParameterName=" << subIndex->name() << "\r\n";
        *_file << "ObjectType=" << valueToString(Index::Object::VAR, base, 1) << "\r\n";
        *_file << "DataType=" << valueToString(subIndex->dataType(), base, 4) << "\r\n";
        *_file << "AccessType=" << accessToString(subIndex->accessType()) << "\r\n";
        if (subIndex->value().isValid())
        {
            *_file << "DefaultValue=" << defaultValue(subIndex) << "\r\n";
        }
        *_file << "PDOMapping=" << pdoToString(subIndex->accessType()) << "\r\n";
        if (subIndex->objFlags() != 0)
        {
            *_file << "ObjFlags=" << valueToString(subIndex->objFlags(), base) << "\r\n";
        }
        writeLimit(subIndex);
        *_file << "\r\n";
    }
}

/**
 * @brief writes an array index
 * @param index model
 */
void DeviceIniWriter::writeArray(Index *index) const
{
    writeRecord(index);
}

/**
 * @brief writes high limit and low limit of a sub-index if they exist
 * @param sub index model
 */
void DeviceIniWriter::writeLimit(const SubIndex *subIndex) const
{
    if (subIndex->hasLowLimit())
    {
        *_file << "LowLimit=" << subIndex->lowLimit().toString() << "\r\n";
    }

    if (subIndex->hasHighLimit())
    {
        *_file << "HighLimit=" << subIndex->highLimit().toString() << "\r\n";
    }
}

/**
 * @brief writes all keys and values of a map of string
 * @param map of string
 */
void DeviceIniWriter::writeStringMap(const QMap<QString, QString> &map) const
{
    QList<QString> keys = map.keys();

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(keys.begin(),
              keys.end(),
              [&collator](const QString &file1, const QString &file2)
              {
                  return collator.compare(file1, file2) < 0;
              });

    for (const QString &key : qAsConst(keys))
    {
        *_file << key << "=" << map.value(key) << "\r\n";
    }

    *_file << "\r\n";
}

/**
 * @brief returns an int to the correct string format depending of the base
 * @param value to format
 * @param base, 16 or 10
 * @return formated string
 */
QString DeviceIniWriter::valueToString(int value, int base, int width) const
{
    switch (base)
    {
        case 10:
            return QString::number(value);

        case 16:
            QString v = QString::number(value, base).rightJustified(width, '0').toUpper();
            if (width > 0)
            {
                v = v.right(width);
            }
            return "0x" + v;
    }

    return QString("");
}

/**
 * @brief returns an access code to his corresponding string format
 * @param access code
 * @return formated string
 */
QString DeviceIniWriter::accessToString(int access) const
{
    switch (access)
    {
        case SubIndex::READ:
        case SubIndex::READ + SubIndex::TPDO:
            return QString("ro");

        case SubIndex::READ + SubIndex::CONST:
            return QString("const");

        case SubIndex::WRITE:
        case SubIndex::WRITE + SubIndex::RPDO:
            return QString("wo");

        case SubIndex::READ + SubIndex::WRITE:
        case SubIndex::READ + SubIndex::WRITE + SubIndex::TPDO + SubIndex::RPDO:
            return QString("rw");

        case SubIndex::READ + SubIndex::WRITE + SubIndex::TPDO:
            return QString("rwr");

        case SubIndex::READ + SubIndex::WRITE + SubIndex::RPDO:
            return QString("rww");
    }

    return "";
}

/**
 * @brief converts a QVariant to a string
 * @param QVariant
 * @return string
 */
QString DeviceIniWriter::dataToString(const QVariant &value) const
{
    if (value.type() == QVariant::String)
    {
        return "\"" + value.toString() + "\"";
    }
    return value.toString();
}

/**
 * @brief return 1 if accessType has a mapping pdo flag, else 0
 * @param 8 bits access type code
 * @return 1 or 0 as a string
 */
QString DeviceIniWriter::pdoToString(uint8_t accessType) const
{
    if ((accessType & SubIndex::TPDO) == SubIndex::TPDO || (accessType & SubIndex::RPDO) == SubIndex::RPDO)
    {
        return QString::number(1);
    }

    return QString::number(0);
}

bool DeviceIniWriter::isDescription() const
{
    return _isDescription;
}

void DeviceIniWriter::setDescription(bool description)
{
    _isDescription = description;
}

QString DeviceIniWriter::defaultValue(const SubIndex *subIndex) const
{
    if (subIndex->hasNodeId() && _isDescription)
    {
        QString string = "$NODEID";
        if (subIndex->value().toInt() != 0)
        {
            string += "+" + valueToString(subIndex->value().toInt(), 16, subIndex->length() * 2);
        }
        return string;
    }
    else if (subIndex->isHexValue())
    {
        return valueToString(subIndex->value().toInt(), 16, subIndex->length() * 2);
    }
    else
    {
        return dataToString(subIndex->value());
    }
}
