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

#include "devicemodel.h"

DeviceModel::DeviceModel()
{

}

DeviceModel::~DeviceModel()
{
    qDeleteAll(_indexes);
    _indexes.clear();
}

DeviceModel::Type DeviceModel::type()
{
    return Invalid;
}

QMap<QString, QString> DeviceModel::fileInfos() const
{
    return _fileInfos;
}

void DeviceModel::setFileInfos(const QMap<QString, QString> &fileInfos)
{
    _fileInfos = fileInfos;
}

void DeviceModel::setFileInfo(const QString &key, const QString &value)
{
    _fileInfos.insert(key, value);
}

QMap<QString, QString> DeviceModel::dummyUsages() const
{
    return _dummyUsages;
}

void DeviceModel::setDummyUsages(const QMap<QString, QString> &dummyUsages)
{
    _dummyUsages = dummyUsages;
}

void DeviceModel::setDummyUsage(const QString &key, const QString &value)
{
    _dummyUsages.insert(key, value);
}

QMap<uint16_t, Index *> DeviceModel::indexes() const
{
    return _indexes;
}

Index *DeviceModel::index(const uint16_t &index) const
{
    return _indexes.value(index);
}

void DeviceModel::addIndex(Index *index)
{
    _indexes.insert(index->index(), index);
}

int DeviceModel::indexCount() const
{
    return _indexes.count();
}

void DeviceModel::setIndexes(const QMap<uint16_t, Index *> &indexes)
{
    _indexes = indexes;
}

QString DeviceModel::fileName() const
{
    return _fileInfos.value("FileName");
}

void DeviceModel::setFileName(const QString &name)
{
    _fileInfos.insert("FileName", name);
}
