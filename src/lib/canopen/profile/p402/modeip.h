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

#ifndef MODEIP_H
#define MODEIP_H

#include "canopen_global.h"

#include "mode.h"

class NodeObjectId;
class NodeProfile402;

class CANOPEN_EXPORT ModeIp : public Mode
{
    Q_OBJECT
public:
    ModeIp(NodeProfile402 *nodeProfile402);

    void setEnableRamp(bool ok);
    bool isEnableRamp(void);


public slots:
    void bufferClear();


signals:
    void enableRampEvent(bool ok);

private:
    quint8 _mode;
    NodeObjectId _targetObjectId;
    NodeObjectId _bufferClearObjectId;

    quint16 _cmdControlWordFlag;

    NodeObjectId _positionDemandValueObjectId;
    NodeObjectId _positionActualValueObjectId;

    NodeObjectId _timePeriodUnitObjectId;
    NodeObjectId _timePeriodIndexObjectId;

    NodeObjectId _positionRangelLimitMinObjectId;
    NodeObjectId _positionRangelLimitMaxObjectId;
    NodeObjectId _softwarePositionLimitMinObjectId;
    NodeObjectId _softwarePositionLimitMaxObjectId;

    NodeObjectId _maxProfileVelocityObjectId;
    NodeObjectId _maxMotorSpeedObjectId;

    NodeObjectId _homeOffsetObjectId;
    NodeObjectId _polarityObjectId;

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
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // MODEIP_H
