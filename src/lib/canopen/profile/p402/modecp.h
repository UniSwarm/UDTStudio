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

#ifndef MODECP_H
#define MODECP_H

#include "canopen_global.h"

#include "modepc.h"

class CANOPEN_EXPORT ModeCp : public ModePc
{
    Q_OBJECT
public:
    ModeCp(NodeProfile402 *nodeProfile402);

    void setAbsRel(bool ok);    // bit 6 of controlWord
    bool isAbsRel(void) const;  // bit 6 of controlWord

    // ObjectID
    const NodeObjectId &targetObjectId() const;

signals:
    void absRelEvent(bool ok);

private:
    quint16 _cmdControlWordFlag;

    NodeObjectId _targetObjectId;

    // Mode interface
public:
    void setTarget(qint32 target) override;
    quint16 getSpecificCwFlag() override;
    void setCwDefaultflag() override;
    void readAllObjects() override;
};

#endif  // MODECP_H
