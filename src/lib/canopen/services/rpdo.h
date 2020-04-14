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

#include "nodeobjectid.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"
#include "pdo.h"
#include "service.h"

class CANOPEN_EXPORT RPDO : public PDO
{
    Q_OBJECT
public:
    RPDO(Node *node, quint8 number);

    QString type() const override;
    void parseFrame(const QCanBusFrame &frame) override;
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
    virtual void setBus(CanOpenBus *bus) override;

    bool setTransmissionType(quint8  type);
    quint8 transmissionType();

    enum TransmissionType
    {
        RPDO_SYNC_MIN = 0x00u, // synchronous (acyclic)
        RPDO_SYNC_MAX = 0xF0u, // synchronous (cyclic every 240 th SYNC)
        RPDO_EVENT_MS = 0xFEu, // event-driven (manufacturer-specific)
        RPDO_EVENT_DP = 0xFFu // event-driven (device profile and application profile specific)
    };

protected slots:
    void receiveSync();

private:
    void saveData();
};

#endif // RPDO_H
