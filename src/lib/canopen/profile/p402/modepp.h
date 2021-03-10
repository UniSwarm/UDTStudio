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

#ifndef MODEPP_H
#define MODEPP_H

#include "canopen_global.h"

#include "mode.h"

class Node;
class NodeProfile402;

class CANOPEN_EXPORT ModePp : public Mode
{
    Q_OBJECT
public:
    ModePp(NodeProfile402 *nodeProfile402);

    void newSetPoint(bool ok); // bit 4 of controlWord front upright
    bool isNewSetPoint(void); // bit 4 of controlWord front upright

    void setChangeSetImmediately(bool ok); // bit 5 of controlWord
    bool isChangeSetImmediately(void); // bit 5 of controlWord

    void setAbsRel(bool ok); // bit 6 of controlWord
    bool isAbsRel(void); // bit 6 of controlWord

    void setChangeOnSetPoint(bool ok); // bit 9 of controlWord
    bool isChangeOnSetPoint(void); // bit 9 of controlWord

signals:
    void changeSetImmediatelyEvent(bool ok);
    void absRelEvent(bool ok);
    void changeOnSetPointEvent(bool ok);

private:
    quint8 _mode;
    NodeObjectId _targetObjectId;
    quint16 _cmdControlWordFlag;

    // Mode interface
public:
    void setTarget(qint32 target) override;
    quint16 getSpecificCwFlag() override;
    void setCwDefaultflag() override;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // MODEPP_H
