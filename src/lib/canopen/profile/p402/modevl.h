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

#ifndef MODEVL_H
#define MODEVL_H

#include "canopen_global.h"

#include "mode.h"

class NodeObjectId;
class NodeProfile402;

class CANOPEN_EXPORT ModeVl : public Mode
{
    Q_OBJECT
public:
    ModeVl(NodeProfile402 *nodeProfile402);

    void setEnableRamp(bool ok);
    bool isEnableRamp() const;

    void setUnlockRamp(bool ok);
    bool isUnlockRamp() const;

    void setReferenceRamp(bool ok);
    bool isReferenceRamp() const;

    // ObjectID
    const NodeObjectId &targetObjectId() const;
    const NodeObjectId &velocityDemandObjectId() const;
    const NodeObjectId &velocityActualObjectId() const;
    const NodeObjectId &minVelocityMinMaxAmountObjectId() const;
    const NodeObjectId &maxVelocityMinMaxAmountObjectId() const;
    const NodeObjectId &accelerationDeltaSpeedObjectId() const;
    const NodeObjectId &accelerationDeltaTimeObjectId() const;
    const NodeObjectId &decelerationDeltaSpeedObjectId() const;
    const NodeObjectId &decelerationDeltaTimeObjectId() const;
    const NodeObjectId &quickStopDeltaSpeedObjectId() const;
    const NodeObjectId &quickStopDeltaTimeObjectId() const;
    const NodeObjectId &setPointFactorNumeratorObjectId() const;
    const NodeObjectId &setPointFactorDenominatorObjectId() const;
    const NodeObjectId &dimensionFactorNumeratorObjectId() const;
    const NodeObjectId &dimensionFactorDenominatorObjectId() const;

signals:
    void enableRampEvent(bool ok);
    void referenceRampEvent(bool ok);
    void unlockRampEvent(bool ok);

private:
    quint16 _cmdControlWordFlag;

    NodeObjectId _targetObjectId;

    NodeObjectId _velocityDemandObjectId;
    NodeObjectId _velocityActualObjectId;

    NodeObjectId _minVelocityMinMaxAmountObjectId;
    NodeObjectId _maxVelocityMinMaxAmountObjectId;
    NodeObjectId _accelerationDeltaSpeedObjectId;
    NodeObjectId _accelerationDeltaTimeObjectId;
    NodeObjectId _decelerationDeltaSpeedObjectId;
    NodeObjectId _decelerationDeltaTimeObjectId;
    NodeObjectId _quickStopDeltaSpeedObjectId;
    NodeObjectId _quickStopDeltaTimeObjectId;
    NodeObjectId _setPointFactorNumeratorObjectId;
    NodeObjectId _setPointFactorDenominatorObjectId;
    NodeObjectId _dimensionFactorNumeratorObjectId;
    NodeObjectId _dimensionFactorDenominatorObjectId;

    // Mode interface
public:
    void setTarget(qint32 target) override;
    quint16 getSpecificCwFlag() override;
    void setCwDefaultflag() override;
    void readRealTimeObjects() override;
    void readAllObjects() override;
    void reset() override;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // MODEVL_H
