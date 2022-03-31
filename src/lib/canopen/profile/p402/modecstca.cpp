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

#include "modecstca.h"

ModeCstca::ModeCstca(NodeProfile402 *nodeProfile402)
    : ModeTc(nodeProfile402)
{
    _mode = NodeProfile402::OperationMode::CSTCA;
    _cmdControlWordFlag = 0;
}

void ModeCstca::setAngle(uint16_t angle)
{
    _nodeProfile402->node()->writeObject(_commutationAngleObjectId, QVariant(static_cast<quint16>(angle)));
}

void ModeCstca::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(static_cast<qint16>(target)));
}

quint16 ModeCstca::getSpecificCwFlag()
{
    return _cmdControlWordFlag;
}

void ModeCstca::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModeCstca::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_torqueDemandObjectId);
    _nodeProfile402->node()->readObject(_torqueActualValueObjectId);
}

void ModeCstca::readAllObjects()
{
    readRealTimeObjects();
    _nodeProfile402->node()->readObject(_targetObjectId);
    _nodeProfile402->node()->readObject(_targetSlopeObjectId);
    _nodeProfile402->node()->readObject(_maxTorqueObjectId);
}

void ModeCstca::reset()
{
    _nodeProfile402->node()->readObject(_targetObjectId);
}

void ModeCstca::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}
