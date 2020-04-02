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

#ifndef NODEODSUBSCRIBER_H
#define NODEODSUBSCRIBER_H

#include "canopen_global.h"

#include <QSet>
#include <QVariant>

class Node;

class CANOPEN_EXPORT NodeOdSubscriber
{
public:
    NodeOdSubscriber();
    virtual ~NodeOdSubscriber();

    void notifySubscriber(quint16 index, quint8 subIndex, const QVariant &value);

protected:
    Node *nodeInterrest() const;
    void setNodeInterrest(Node *nodeInterrest);

    void readObject(quint16 index, quint8 subindex, QMetaType::Type dataType = QMetaType::UnknownType);

    QList<quint32> indexSubIndexList() const;

    void registerSubIndex(quint16 index, quint8 subindex);
    void registerIndex(quint16 index);
    void registerFullOd();

    virtual void odNotify(quint16 index, quint8 subindex, const QVariant &value) =0;

private:
    Node *_nodeInterrest;
    QSet<quint32> _indexSubIndexList;
    void registerKey(quint16 index = 0xFFFFu, quint8 subindex = 0xFFu);
};

#endif // NODEODSUBSCRIBER_H
