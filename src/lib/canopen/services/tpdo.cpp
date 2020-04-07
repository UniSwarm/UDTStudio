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

#include "tpdo.h"

#include <QDebug>

#include "canopenbus.h"

TPDO::TPDO(Node *node, quint8 number)
    : PDO(node), _number(number)
{
    _cobIds.append(0x180 + 0x100 * _number + node->nodeId());
}

QString TPDO::type() const
{
    return QLatin1String("TPDO") + QString::number(_number + 1, 10);
}

void TPDO::parseFrame(const QCanBusFrame &frame)
{
    Q_UNUSED(frame);
//    uint8_t nodeId = frame.frameId() & 0x0000007F;
//    uint8_t tpdo = (((frame.frameId() & 0x00000380) >> 7) - 3) / 2;
}

quint8 TPDO::number() const
{
    return _number;
}

void TPDO::odNotify(const NodeObjectId &objId, const QVariant &value)
{

}
