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

#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include "od_global.h"

#include <QMap>

#include "index.h"

class OD_EXPORT DeviceModel
{
public:
    DeviceModel();
    virtual ~DeviceModel();

    enum Type
    {
        Invalid,
        Description,
        Configuration
    };
    virtual Type type();

    const QMap<QString, QString> &fileInfos() const;
    void setFileInfos(const QMap<QString, QString> &fileInfos);
    void setFileInfo(const QString &key, const QString &value);

    const QMap<QString, QString> &dummyUsages() const;
    void setDummyUsages(const QMap<QString, QString> &dummyUsages);
    void setDummyUsage(const QString &key, const QString &value);

    const QMap<QString, QString> &comments() const;
    void setComments(const QMap<QString, QString> &comments);
    void setComment(const QString &key, const QString &value);

    const QMap<uint16_t, Index *> &indexes() const;
    QMap<uint16_t, Index *> &indexes();
    Index *index(uint16_t index) const;
    Index *index(QString name) const;
    void addIndex(Index *index);
    int indexCount() const;
    bool indexExist(uint16_t index) const;
    bool indexExist(QString name) const;
    void deleteIndex(Index *index);

    SubIndex *subIndex(uint16_t index, uint8_t subIndex) const;
    SubIndex *subIndex(QString index, QString subIndex) const;
    bool subIndexExist(uint16_t index, uint8_t subIndex) const;
    bool subIndexExist(QString index, QString subIndex) const;
    QVariant subIndexValue(uint16_t index, uint8_t subIndex, const QVariant &defaultValue = QVariant()) const;

    QString fileName() const;
    void setFileName(const QString &name);

private:
    QMap<QString, QString> _fileInfos;
    QMap<QString, QString> _dummyUsages;
    QMap<QString, QString> _comments;
    QMap<uint16_t, Index *> _indexes;
};

#endif  // DEVICEMODEL_H
