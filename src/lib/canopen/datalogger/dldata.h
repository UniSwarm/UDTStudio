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

#ifndef DLDATA_H
#define DLDATA_H

#include "canopen_global.h"

#include "nodeobjectid.h"

#include "node.h"

class CANOPEN_EXPORT DLData
{
public:
    DLData(const NodeObjectId &objectId);

    const NodeObjectId &objectId() const;
    quint64 key() const;
    Node *node() const;

    QString name() const;
    void setName(const QString &name);

    void appendData(double value, const QDateTime &dateTime);
    double lastValue() const;

protected:
    NodeObjectId _objectId;
    Node *_node;
    QString _name;

    QList <double> _values;
    QList <QDateTime> _times;
};

#endif // DLDATA_H
