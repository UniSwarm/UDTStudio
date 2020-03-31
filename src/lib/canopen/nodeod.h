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

#ifndef NODEOD_H
#define NODEOD_H

#include "canopen_global.h"

#include <QMap>
#include <QMultiMap>

#include "model/index.h"
#include "nodeindex.h"

class Node;
class NodeOdSubscriber;

class CANOPEN_EXPORT NodeOd : public QObject
{
    Q_OBJECT
public:
    NodeOd(Node *node);
    virtual ~NodeOd();

    const QMap<quint16, NodeIndex *> &indexes() const;
    NodeIndex *index(quint16 index) const;
    void addIndex(NodeIndex *index);
    int indexCount() const;
    bool indexExist(quint16 key) const;

    void updateObjectFromDevice(quint16 index, quint8 subindex, const QVariant &value);

    bool loadEds(const QString &fileName);

    QVariant value(quint16 index, quint8 subIndex = 0x00);
    void subscribe(NodeOdSubscriber *object, quint16 notifyIndex, quint8 notifySubIndex);
    void unsubscribe(NodeOdSubscriber *object);

private:
    Node *_node;
    QMap<quint16, NodeIndex *> _nodeIndexes;
    QString _fileName;

    void createMandatoryObject();

    struct Subscriber
    {
        NodeOdSubscriber *object;
        quint16 notifyIndex;
        quint8 notifySubIndex;
    };
    QMultiMap<quint32, Subscriber> _subscribers;
    void notifySubscribers(quint16 notifyIndex, quint8 notifySubIndex, const QVariant &value);
};

#endif // NODEOD_H
