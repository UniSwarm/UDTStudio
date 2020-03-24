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

#ifndef RPDO_H
#define RPDO_H

#include "canopen_global.h"

#include "service.h"

class CANOPEN_EXPORT RPDO : public Service
{
    Q_OBJECT
public:
    RPDO(Node *node, quint8 number);

    uint32_t cobIdPdo1();
    uint32_t cobIdPdo2();
    uint32_t cobIdPdo3();
    uint32_t cobIdPdo4();

    QString type() const override;

    void parseFrame(const QCanBusFrame &frame) override;

private:
    quint8 _number;
    uint32_t _cobIdPdo1;
    uint32_t _cobIdPdo2;
    uint32_t _cobIdPdo3;
    uint32_t _cobIdPdo4;
};

#endif // RPDO_H
