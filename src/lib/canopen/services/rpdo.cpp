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

#include "rpdo.h"

#include <QDebug>

RPDO::RPDO(CanOpenBus *bus)
    : Service (bus)
{

}

void RPDO::parseFrame(const QCanBusFrame &frame)
{
    uint8_t nodeId = frame.frameId() & 0x0000007F;
    uint8_t tpdo = (((frame.frameId() & 0x00000380) >> 7) - 3) / 2;

}
