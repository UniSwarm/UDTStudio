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

#include "devicemodel.h"

/**
 * @brief default constructor
 */
DeviceModel::DeviceModel()
{
}

/**
 * @brief default destructor
 */
DeviceModel::~DeviceModel()
{
    qDeleteAll(_indexes);
}

/**
 * @brief retrun the device model type wich is implemented
 * @return device model type
 */
DeviceModel::Type DeviceModel::type()
{
    return Invalid;
}

/**
 * @brief _fileInfos getter
 * @return a map of strings wich constains some file informations
 */
const QMap<QString, QString> &DeviceModel::fileInfos() const
{
    return _fileInfos;
}

void DeviceModel::setFileInfos(const QMap<QString, QString> &fileInfos)
{
    _fileInfos = fileInfos;
}

/**
 * @brief inserts a new file information with the key key and a value of value
 * @param key
 * @param value
 */
void DeviceModel::setFileInfo(const QString &key, const QString &value)
{
    _fileInfos.insert(key, value);
}

/**
 * @brief _dummyUsages getter
 * @return a map of strings wich contains the dummy usages
 */
const QMap<QString, QString> &DeviceModel::dummyUsages() const
{
    return _dummyUsages;
}

void DeviceModel::setDummyUsages(const QMap<QString, QString> &dummyUsages)
{
    _dummyUsages = dummyUsages;
}

/**
 * @brief inserts a new dummy usage with the key key and a value of value
 * @param key
 * @param value
 */
void DeviceModel::setDummyUsage(const QString &key, const QString &value)
{
    _dummyUsages.insert(key, value);
}

const QMap<QString, QString> &DeviceModel::comments() const
{
    return _comments;
}

void DeviceModel::setComments(const QMap<QString, QString> &comments)
{
    _comments = comments;
}

void DeviceModel::setComment(const QString &key, const QString &value)
{
    if (key != "Lines")
    {
        _comments.insert(key, value);
    }
}

/**
 * @brief _indexes getter
 * @return a map of index wich contains the index of the device model
 */
const QMap<uint16_t, Index *> &DeviceModel::indexes() const
{
    return _indexes;
}

QMap<uint16_t, Index *> &DeviceModel::indexes()
{
    return _indexes;
}

/**
 * @brief returns the value associated with the key index
 * @param index
 * @return an Index*
 */
Index *DeviceModel::index(uint16_t index) const
{
    return _indexes.value(index);
}

/**
 * @brief returns the value associated with the name of index
 * @param index
 * @return an Index*
 */
Index *DeviceModel::index(const QString &name) const
{
    for (Index *index : _indexes)
    {
        if (index->name() == name)
        {
            return index;
        }
    }
    return nullptr;
}

/**
 * @brief inserts a new index. If the index already exist, replaces it with the new index
 * @param index
 */
void DeviceModel::addIndex(Index *index)
{
    if (index == nullptr)
    {
        return;
    }
    if (_indexes.contains(index->index()))
    {
        return;
    }
    _indexes.insert(index->index(), index);
}

/**
 * @brief returns the number of index in the map
 * @return number of index
 */
int DeviceModel::indexCount() const
{
    return _indexes.count();
}

/**
 * @brief returns true if the map contains an item with key key; otherwise returns false.
 * @param key
 * @return boolean
 */
bool DeviceModel::indexExist(uint16_t index) const
{
    return _indexes.contains(index);
}

/**
 * @brief returns true if the map contains an item with name of index; otherwise returns false.
 * @param key
 * @return boolean
 */
bool DeviceModel::indexExist(const QString &name) const
{
    for (Index *index : _indexes)
    {
        if (index->name() == name)
        {
            return true;
        }
    }
    return false;
}

void DeviceModel::deleteIndex(Index *index)
{
    _indexes.remove(index->index());
}

SubIndex *DeviceModel::subIndex(uint16_t index, uint8_t subIndex) const
{
    Index *mindex = _indexes.value(index);
    if (mindex == nullptr)
    {
        return nullptr;
    }

    return mindex->subIndex(subIndex);
}

SubIndex *DeviceModel::subIndex(const QString &index, const QString &subIndex) const
{
    Index *mindex = this->index(index);
    if (mindex == nullptr)
    {
        return nullptr;
    }

    return mindex->subIndex(subIndex);
}

bool DeviceModel::subIndexExist(uint16_t index, uint8_t subIndex) const
{
    return (this->subIndex(index, subIndex) != nullptr);
}

bool DeviceModel::subIndexExist(const QString &index, const QString &subIndex) const
{
    return (this->subIndex(index, subIndex) != nullptr);
}

QVariant DeviceModel::subIndexValue(uint16_t index, uint8_t subIndex, const QVariant &defaultValue) const
{
    SubIndex *msubIndex = this->subIndex(index, subIndex);
    if (msubIndex == nullptr)
    {
        return defaultValue;
    }
    return msubIndex->value();
}

/**
 * @brief return the name of the device file
 * @return string
 */
QString DeviceModel::fileName() const
{
    return _fileInfos.value("FileName");
}

/**
 * @brief set the name of the file
 * @param new name
 */
void DeviceModel::setFileName(const QString &name)
{
    _fileInfos.insert("FileName", name);
}
