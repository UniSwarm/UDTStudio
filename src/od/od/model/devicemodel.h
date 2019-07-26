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

#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include "od_global.h"

#include <QMap>

#include "index.h"

class OD_EXPORT DeviceModel
{
public:
    DeviceModel();
    ~DeviceModel();

    enum Type
    {
        Invalid,
        Description,
        Configuration
    };
    virtual Type type();

    QMap<QString, QString> fileInfos() const;
    void setFileInfos(const QMap<QString, QString> &fileInfos);
    void setFileInfo(const QString &key, const QString &value);

    QMap<QString, QString> dummyUsages() const;
    void setDummyUsages(const QMap<QString, QString> &dummyUsages);
    void setDummyUsage(const QString &key, const QString &value);

    QMap<uint16_t, Index *> indexes() const;
    Index *index(const uint16_t &index) const;
    void addIndex(Index *index);
    int indexCount() const;
    void setIndexes(const QMap<uint16_t, Index *> &indexes);

    QString fileName() const;
    void setFileName(const QString &name);

private:
    QMap<QString, QString> _fileInfos;
    QMap<QString, QString> _dummyUsages;
    QMap<uint16_t, Index *> _indexes;
};

#endif // DEVICEMODEL_H
