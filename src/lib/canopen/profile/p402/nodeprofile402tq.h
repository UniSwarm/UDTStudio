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

#ifndef NODEPROFILE402TQ_H
#define NODEPROFILE402TQ_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"

#include <QObject>

class NodeObjectId;
class NodeProfile402;

class CANOPEN_EXPORT NodeProfile402Tq : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    NodeProfile402Tq(Node *node, NodeProfile402 *nodeProfile402);

    void setTarget(qint16 torque);

signals:
    void isAppliedTarget();

private:
    Node *_node;
    NodeProfile402 *_nodeProfile402;

    NodeObjectId _targetObjectId;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // NODEPROFILE402TQ_H
