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

#ifndef NODEPROFILE402PP_H
#define NODEPROFILE402pP_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"

#include <QObject>

class Node;
class NodeObjectId;
class NodeProfile402;

class CANOPEN_EXPORT NodeProfile402Pp : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
  public:
    NodeProfile402Pp(Node *node, uint8_t axis, NodeProfile402 *nodeProfile402);

    void setTarget(qint32 position);

    quint16 getSpecificControlWord();

    void setEnableRamp(bool ok);
    bool isEnableRamp(void);

    void applyNewSetPoint();  // bit 4 of controlWord front upright
//    bool isUnlockRamp(void);

    void setChangeSetImmediately(bool ok);   // bit 5 of controlWord
    bool isChangeSetImmediately(void);       // bit 5 of controlWord

    void setChangeOnSetPoint(bool ok);  // bit 9 of controlWord
    bool isChangeOnSetPoint(void);      // bit 9 of controlWord

    void setAbsRel(bool ok); // bit 6 of controlWord
    bool isAbsRel(void);     // bit 6 of controlWord

  signals:
    void isAppliedTarget();
    void enableRampEvent(bool ok);

    void changeSetImmediatelyEvent(bool ok);
    void changeOnSetPointEvent(bool ok);
    void absRelEvent(bool ok);

  private:
    Node *_node;
    uint8_t _axis;
    NodeProfile402 *_nodeProfile402;

    quint8 _mode;
    NodeObjectId _controlWordObjectId;
    NodeObjectId _targetObjectId;
    quint16 _cmdControlWordSpecific;

    // NodeOdSubscriber interface
  public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // NODEPROFILE402PP_H
