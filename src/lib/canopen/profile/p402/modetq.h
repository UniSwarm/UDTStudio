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

#ifndef MODETQ_H
#define MODETQ_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"

#include <QObject>

class NodeObjectId;
class NodeProfile402;

class CANOPEN_EXPORT ModeTq : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    ModeTq(Node *node, uint8_t axis, NodeProfile402 *nodeProfile402);

    void setTarget(qint16 torque);

    quint16 getSpecificControlWord();

signals:
    void isAppliedTarget();

private:
    Node *_node;
    uint8_t _axis;
    NodeProfile402 *_nodeProfile402;

    quint8 _mode;
    NodeObjectId _targetObjectId;
    quint16 _cmdControlWordSpecific;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // MODETQ_H
