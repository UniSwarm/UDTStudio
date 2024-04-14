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

#ifndef NODEODSUBSCRIBER_H
#define NODEODSUBSCRIBER_H

#include "canopen_global.h"

#include <QSet>
#include <QVariant>

#include "nodeobjectid.h"
#include "nodeod.h"
#include "services/sdo.h"

class Node;

class CANOPEN_EXPORT NodeOdSubscriber
{
public:
    NodeOdSubscriber();
    virtual ~NodeOdSubscriber();

    void notifySubscriber(const NodeObjectId &objId, NodeOd::FlagsRequest flags);

    QList<NodeObjectId> objIdList() const;

protected:
    Node *nodeInterrest() const;
    void setNodeInterrest(Node *nodeInterrest);

    void readObject(const NodeObjectId &id);
    void readObject(quint16 index, quint8 subindex, QMetaType::Type dataType = QMetaType::UnknownType);
    void writeObject(const NodeObjectId &id, const QVariant &data);
    void writeObject(quint16 index, quint8 subindex, const QVariant &data);

    void registerObjId(const NodeObjectId &objId);
    void registerSubIndex(quint16 index, quint8 subindex);
    void registerIndex(quint16 index);
    void registerFullOd();

    void unRegisterObjId(const NodeObjectId &objId);
    void unRegisterSubIndex(quint16 index, quint8 subindex);
    void unRegisterIndex(quint16 index);
    void unRegisterFullOd();

    virtual void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) = 0;  // TODO constify flags param

private:
    friend class NodeOd;

    Node *_nodeInterrest;
    QSet<quint64> _indexSubIndexList;
    QList<NodeObjectId> _objIdList;
    void registerKey(const NodeObjectId &objId);
    void unRegisterKey(const NodeObjectId &objId);
};

#endif  // NODEODSUBSCRIBER_H
