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

    uint32_t cobId();

    void sendNmt(quint8 node_id, quint8 cmd);
    void sendStart(quint8 node_id);
    void sendStop(quint8 node_id);
    void sendResetComm(quint8 node_id);
    void sendResetNode(quint8 node_id);

    QString type() const override;

    void parseFrame(const QCanBusFrame &frame) override;

private:
    uint32_t _cobId;
};

#endif // NMT_H
