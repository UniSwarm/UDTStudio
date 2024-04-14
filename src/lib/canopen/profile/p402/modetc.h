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

#ifndef MODETC_H
#define MODETC_H

#include "canopen_global.h"

#include "modehm.h"

class Node;
class NodeProfile402;

class CANOPEN_EXPORT ModeTc : public ModeHm
{
public:
    ModeTc(NodeProfile402 *nodeProfile402);

    // ObjectId
    const NodeObjectId &targetObjectId() const;
    const NodeObjectId &torqueDemandObjectId() const;
    const NodeObjectId &torqueActualValueObjectId() const;
    const NodeObjectId &commutationAngleObjectId() const;
    const NodeObjectId &torqueOffsetObjectId() const;
    const NodeObjectId &targetSlopeObjectId() const;
    const NodeObjectId &maxTorqueObjectId() const;
    const NodeObjectId &maxMotorSpeedObjectId() const;
    const NodeObjectId &motorRatedTorqueObjectId() const;

protected:
    NodeObjectId _targetObjectId;

    NodeObjectId _torqueDemandObjectId;
    NodeObjectId _torqueActualValueObjectId;

    NodeObjectId _commutationAngleObjectId;

    NodeObjectId _torqueOffsetObjectId;
    NodeObjectId _targetSlopeObjectId;
    NodeObjectId _maxTorqueObjectId;
    NodeObjectId _maxMotorSpeedObjectId;

    NodeObjectId _motorRatedTorqueObjectId;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

    // Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
};

#endif  // MODETC_H
