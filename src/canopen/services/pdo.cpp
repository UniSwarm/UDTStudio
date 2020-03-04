/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "pdo.h"

#include <QDebug>

PDO::PDO(CanOpenBus *bus)
    : Service (bus)
{

}

void PDO::parseFrame(const QCanBusFrame &frame)
{
    uint8_t nodeId = frame.frameId() & 0x0000007F;
    uint8_t tpdo = (((frame.frameId() & 0x00000380) >> 7) - 3) / 2;

    if (tpdo == 0)
    {
        uint16_t status = *((uint16_t*)(&frame.payload().data()[0]));
        int16_t speed = *((int16_t*)(&frame.payload().data()[2]));
        int32_t pos = *((int32_t*)(&frame.payload().data()[4]));
        double angle = pos * 360.0 / 524288.0;
        qDebug() << "nodeId : " << nodeId << angle << speed << status;
        //_canopen_pos[nodeId] = pos;
    }
    if (tpdo == 1)
    {
        int16_t speed = *((int16_t*)(&frame.payload().data()[0]));
        int16_t out = *((int16_t*)(&frame.payload().data()[2]));
        int16_t error = *((int16_t*)(&frame.payload().data()[4]));
        int16_t in = *((int16_t*)(&frame.payload().data()[6]));
        qDebug() << "> nodeId : " << nodeId << speed << out << error << in;
    }
}
