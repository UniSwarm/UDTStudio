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

#ifndef MODEPC_H
#define MODEPC_H

#include "canopen_global.h"

#include "modehm.h"

class Node;
class NodeProfile402;

class CANOPEN_EXPORT ModePc : public ModeHm
{
public:
    ModePc(NodeProfile402 *nodeProfile402);

    // ObjectID
    const NodeObjectId &positionDemandValueObjectId() const;
    const NodeObjectId &positionActualValueObjectId() const;
    const NodeObjectId &positionRangeLimitMinObjectId() const;
    const NodeObjectId &positionRangeLimitMaxObjectId() const;
    const NodeObjectId &softwarePositionLimitMinObjectId() const;
    const NodeObjectId &softwarePositionLimitMaxObjectId() const;
    const NodeObjectId &profileVelocityObjectId() const;
    const NodeObjectId &endVelocityObjectId() const;
    const NodeObjectId &maxProfileVelocityObjectId() const;
    const NodeObjectId &maxMotorSpeedObjectId() const;
    const NodeObjectId &profileAccelerationObjectId() const;
    const NodeObjectId &maxAccelerationObjectId() const;
    const NodeObjectId &profileDecelerationObjectId() const;
    const NodeObjectId &maxDecelerationObjectId() const;
    const NodeObjectId &quickStopDecelerationObjectId() const;

protected:
    NodeObjectId _positionDemandValueObjectId;
    NodeObjectId _positionActualValueObjectId;

    NodeObjectId _positionRangeLimitMinObjectId;
    NodeObjectId _positionRangeLimitMaxObjectId;
    NodeObjectId _softwarePositionLimitMinObjectId;
    NodeObjectId _softwarePositionLimitMaxObjectId;

    NodeObjectId _profileVelocityObjectId;
    NodeObjectId _endVelocityObjectId;
    NodeObjectId _maxProfileVelocityObjectId;
    NodeObjectId _maxMotorSpeedObjectId;

    NodeObjectId _profileAccelerationObjectId;
    NodeObjectId _maxAccelerationObjectId;
    NodeObjectId _profileDecelerationObjectId;
    NodeObjectId _maxDecelerationObjectId;
    NodeObjectId _quickStopDecelerationObjectId;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;

    // Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
};

#endif  // MODEPC_H
