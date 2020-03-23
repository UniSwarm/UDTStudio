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

#ifndef NMT_H
#define NMT_H

#include "canopen_global.h"

#include "service.h"

class CANOPEN_EXPORT NMT : public Service
{
    Q_OBJECT
public:
    NMT(CanOpenBus *bus);

    void sendNmt(uint8_t node_id, uint8_t cmd);
    void sendStart(uint8_t node_id);
    void sendStop(uint8_t node_id);
    void exploreBus();

    virtual void parseFrame(const QCanBusFrame &frame);

signals:
    void nodeFound(uint8_t node);

private:
    void manageErrorControl(const QCanBusFrame &frame);
};

#endif // NMT_H
