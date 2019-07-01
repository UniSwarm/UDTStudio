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

#include "od.h"

/**
 * @brief constructor
 */
OD::OD()
{

}

/**
 * @brief destructor
 */
OD::~OD()
{
    qDeleteAll(_indexes);
    _indexes.clear();
}

/**
 * @brief find an index
 * @param index number
 * @return index or wrong value if index number does not exist
 */
Index* OD::index(const uint16_t &index) const
{
    return _indexes.value(index);
}

/**
 * @brief adds a index to the object dictionary
 * @param index
 */
void OD::addIndex(Index *index)
{
    _indexes.insert(index->index(), index);
}

/**
 * @brief counts the number of indexes in the object dictionary
 * @return the number of indexes in the object dictionary
 */
int OD::indexCount() const
{
    return _indexes.count();
}

/**
 * @brief _indexes getter
 * @return map of index
 */
QMap<uint16_t, Index*> &OD::indexes()
{
    return _indexes;
}

QMap<QString, QString> OD::fileInfos() const
{
    return _fileInfos;
}

void OD::setFileInfos(const QMap<QString, QString> &fileInfos)
{
    _fileInfos = fileInfos;
}

void OD::addFileInfo(const QString &key, const QString &value)
{
    _fileInfos.insert(key, value);
}

