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

#ifndef RPDO_H
#define RPDO_H

#include "canopen_global.h"

#include "pdo.h"

#include "nodeobjectid.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"

class CANOPEN_EXPORT RPDO : public PDO
{
    Q_OBJECT
public:
    RPDO(Node *node, quint8 number);

    enum TransmissionType
    {
        RPDO_SYNC_MIN = 0x00U,  // synchronous (acyclic)
        RPDO_SYNC_MAX = 0xF0U,  // synchronous (cyclic every 240 th SYNC)
        RPDO_EVENT_MS = 0xFEU,  // event-driven (manufacturer-specific)
        RPDO_EVENT_DP = 0xFFU   // event-driven (device profile and application profile specific)
    };
    bool setTransmissionType(quint8 type);
    quint8 transmissionType();

    void write(const NodeObjectId &object, const QVariant &data);
    void clearDataWaiting() override;

protected slots:
    void receiveSync();
    void prepareAndSendData();

private:
    QMap<quint64, QVariant> _dataObjectCurrentMapped;
    QByteArray _rpdoDataToSendReqPayload;
    bool sendData();
    void convertQVariantToQDataStream(QDataStream &request, const QVariant &data, QMetaType::Type type);

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
};

#endif  // RPDO_H
