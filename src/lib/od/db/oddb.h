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

#ifndef ODDB_H
#define ODDB_H

#include "od_global.h"

#include <QMap>
#include <QString>

class OD_EXPORT OdDb
{
    Q_DISABLE_COPY(OdDb)
public:
    static void addDirectory(const QString &directory);
    static void addDirectory(const QStringList &directories);
    static QString file(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber);
    static void refreshFile();

    static const QList<QString> &edsFiles();

    static inline OdDb *instance()
    {
        if (!OdDb::_instance)
        {
            OdDb::_instance = new OdDb();
            _instance->init();
        }
        return OdDb::_instance;
    }

    static inline void release()
    {
        delete OdDb::_instance;
    }

private:
    OdDb();
    void init();

    QMultiMap<QByteArray, QPair<quint32, QString>> _mapFiles;
    QList<QString> _edsFiles;
    QList<QString> _directoryList;

    void searchFile(const QString &directory);

    static OdDb *_instance;
};

#endif // ODDB_H
