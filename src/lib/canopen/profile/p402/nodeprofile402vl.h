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

#ifndef NODEPROFILE402VL_H
#define NODEPROFILE402VL_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"

#include <QObject>

class Node;
class NodeObjectId;

class CANOPEN_EXPORT NodeProfile402Vl : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    NodeProfile402Vl(Node *node);

    void setTarget(qint16 velocity);

    void enableMode();

    void setEnableRamp(bool ok);
    bool isEnableRamp(void);

    void setUnlockRamp(bool ok);
    bool isUnlockRamp(void);

    void setReferenceRamp(bool ok);
    bool isReferenceRamp(void);

signals:
    void isAppliedTarget();
    void enableRampEvent(bool ok);
    void referenceRampEvent(bool ok);
    void unlockRampEvent(bool ok);

private:
    Node *_node;

    NodeObjectId _controlWordObjectId;
    quint16 _cmdControlWord;
    NodeObjectId _targetObjectId;

    bool _enableRamp;
    bool _referenceRamp;
    bool _unlockRamp;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // NODEPROFILE402VL_H
