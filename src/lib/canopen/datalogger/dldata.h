/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include <QColor>

class CANOPEN_EXPORT DLData
{
public:
    DLData(const NodeObjectId &objectId);

    const NodeObjectId &objectId() const;
    NodeSubIndex *nodeSubIndex() const;
    quint64 key() const;
    Node *node() const;

    bool isActive() const;
    void setActive(bool active);

    // apparence attributes
    QString name() const;
    void setName(const QString &name);

    QColor color() const;
    void setColor(const QColor &color);

    // values and times access
    const QList<qreal> &values() const;
    double firstValue() const;
    double lastValue() const;
    int valuesCount() const;

    const QList<QDateTime> &times() const;
    QDateTime firstDateTime() const;
    QDateTime lastDateTime() const;

    // add / remove dada
    void appendData(qreal value, const QDateTime &dateTime);
    void clear();
    bool isEmpty() const;

    // stats
    qreal min() const;
    qreal max() const;
    void resetMinMax();

    bool isQ1516() const;
    void setQ1516(bool q1516);

    qreal scale() const;
    void setScale(qreal scale);

    QString unit() const;
    void setUnit(const QString &unit);

    void exportCSVData(const QString &fileName);

    bool hasChanged() const;
    void setHasChanged(bool hasChanged);

protected:
    NodeObjectId _objectId;
    NodeSubIndex *_nodeSubIndex;
    Node *_node;
    bool _active;
    bool _hasChanged;

    QString _name;
    QColor _color;
    qreal _scale;

    QList<qreal> _values;
    QList<QDateTime> _times;

    qreal _min;
    qreal _max;

    bool _q1516;
    QString _unit;
};

#endif  // DLDATA_H
