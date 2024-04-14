/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef TPDO_H
#define TPDO_H

#include "canopen_global.h"

#include "pdo.h"

#include "nodeod.h"

class CANOPEN_EXPORT TPDO : public PDO
{
    Q_OBJECT
public:
    TPDO(Node *node, quint8 number);

    enum TransmissionType
    {
        TPDO_ACYCLIC = 0x00U,     // synchronou,s (acyclic)
        TPDO_CYCLIC_MIN = 0x01U,  // synchronou,s (cyclic every sync)
        TPDO_CYCLIC_MAX = 0xF0U,  // synchronou,s (cyclic every 240 th SYNC)
        TPDO_RTR_SYNC = 0xFCU,    // RTR-only (synchronou,s)
        TPDO_RTR_EVENT = 0xFDU,   // RTR-only (event-driven)
        TPDO_EVENT_MS = 0xFEU,    // event-driven (manufacturer-specific)
        TPDO_EVENT_DP = 0xFFU     // event-driven (device profile and application profile specific)
    };
    bool setTransmissionType(quint8 type);
    quint8 transmissionType();

    void setSyncStartValue(quint8 syncStartValue);
    quint8 syncStartValue();

signals:
    void tpdoReceived(quint8);

protected:
    QDateTime _lastFrameDateTime;

protected slots:
    void receiveSync();

private:
    QVariant convertQByteArrayToQVariant(QByteArray data, QMetaType::Type type);

    // Service interface
public:
    QString type() const override;
    void parseFrame(const QCanBusFrame &frame) override;
    void setBus(CanOpenBus *bus) override;

    // PDO interface
public:
    bool isTPDO() const override;
    bool isRPDO() const override;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
    const QDateTime &lastFrameDateTime() const;
};

#endif  // TPDO_H
