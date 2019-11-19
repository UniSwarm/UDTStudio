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

#ifndef SDO_H
#define SDO_H

#include "canopen_global.h"

#include "service.h"

class CANOPEN_EXPORT SDO : public Service
{
    Q_OBJECT
public:
    SDO(CanOpenBus *bus);

    virtual void parseFrame(const QCanBusFrame &frame);

//protected:
    void sendSdoReadReq(uint8_t nodeId, uint16_t index, uint8_t subindex);
    void sendSdoWriteReq(uint8_t nodeId, uint16_t index, uint8_t subindex, const QVariant &value);
};

#endif // SDO_H
