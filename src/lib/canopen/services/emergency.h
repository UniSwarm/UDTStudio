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

#ifndef EMERGENCY_H
#define EMERGENCY_H

#include "canopen_global.h"

#include "service.h"

class CANOPEN_EXPORT Emergency : public Service
{
    Q_OBJECT
public:
    Emergency(Node *node);

    uint32_t cobId() const;

signals:
    void emergencyHappened(uint16_t errorCode, uint8_t errorClass, QByteArray errorDesc);

private:
    uint32_t _cobId;

    // Service interface
public:
    QString type() const override;
    void parseFrame(const QCanBusFrame &frame) override;
};

#endif  // EMERGENCY_H
