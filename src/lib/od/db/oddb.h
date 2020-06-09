/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#ifndef ODDB_H
#define ODDB_H

#include "od_global.h"
#include <QMap>
#include <QString>

class OD_EXPORT OdDb
{
public:
    OdDb(QString directory = ".");

    void addDirectory(const QString &directory);
    QString file(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber);
    void refreshFile();

private:
    quint32 _deviceType;
    quint32 _vendorID;
    quint32 _productCode;
    quint32 _revisionNumber;

    QMultiMap<QByteArray, QPair<quint32, QString>> _mapFile;
    QList<QString> _directoryList;

    void searchFile();
};

#endif // ODDB_H
