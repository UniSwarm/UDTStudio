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

#ifndef NODEINDEX_H
#define NODEINDEX_H

#include "canopen_global.h"

#include "nodeobjectid.h"
#include "nodesubindex.h"

class Node;
class NodeOd;

class CANOPEN_EXPORT NodeIndex
{
public:
    NodeIndex(const quint16 &index);
    NodeIndex(const NodeIndex &other);
    ~NodeIndex();

    quint8 busId() const;
    quint8 nodeId() const;
    Node *node() const;
    NodeOd *nodeOd() const;
    NodeObjectId objectId() const;

    quint16 index() const;
    void setIndex(const quint16 &index);

    const QString &name() const;
    void setName(const QString &name);

    // =========== Object type ====================
    enum ObjectType
    {
        NONE = 0x01,
        OBJECT_NULL = 0x00,
        OBJECT_DOMAIN = 0x02,
        DEFTYPE = 0x05,
        DEFSTRUCT = 0x06,
        VAR = 0x07,
        ARRAY = 0x08,
        RECORD = 0x09
    };
    ObjectType objectType() const;
    void setObjectType(const ObjectType &objectType);
    static QString objectTypeStr(const ObjectType &objectType);

    // =========== Sub indexes ====================
    const QMap<quint8, NodeSubIndex *> &subIndexes() const;
    NodeSubIndex *subIndex(quint8 subIndex) const;
    void addSubIndex(NodeSubIndex *subIndex);
    int subIndexesCount();
    bool subIndexExist(quint8 subIndex);

private:
    friend class NodeOd;
    NodeOd *_nodeOd;

    quint16 _index;
    QString _name;
    ObjectType _objectType;

    QMap<quint8, NodeSubIndex *> _nodeSubIndexes;
};

#endif  // NODEINDEX_H
