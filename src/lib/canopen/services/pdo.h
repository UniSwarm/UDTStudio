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

#ifndef PDO_H
#define PDO_H

#include "canopen_global.h"

#include "nodeobjectid.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"
#include "service.h"

class RPDO;
class TPDO;

class CANOPEN_EXPORT PDO : public Service, public NodeOdSubscriber
{
    Q_OBJECT
public:
    PDO(Node *node, quint8 number);

    virtual QString type() const = 0;
    virtual void parseFrame(const QCanBusFrame &frame) = 0;
    virtual void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) = 0;
    virtual void setBus(CanOpenBus *bus) = 0;

    void mapObjectList(QList<NodeObjectId> objectList);
    QList<NodeObjectId> currentMappedObjectList() const;

    void applyMapping();
    void refreshPdo();

private:

    void readCommParam();
    void readMappingParam();
    void processMapping();
    bool createListObjectMapped();

protected:

    enum StatePdo
    {
        STATE_NONE,
        STATE_READ,
        STATE_WRITE
    };
    StatePdo statusPdo;

    enum StateMapping
    {
        STATE_FREE,
        STATE_DEACTIVATE,
        STATE_DISABLE,
        STATE_MODIFY,
        STATE_ENABLE,
        STATE_ACTIVATE

    };
    StateMapping state;

    quint8 _number;
    quint32 _cobId;
    quint8 _nodeId;
    NodeOd *_nodeOd;
    quint8 _increment;

    quint16 _objectMappingId;
    quint16 _objectCommId;
    QList<NodeObjectId> _objectCommList;

    void notifyReadPdo(const NodeObjectId &objId, SDO::FlagsRequest flags);
    void notifyWritePdo(const NodeObjectId &objId, SDO::FlagsRequest flags);

    QList<NodeObjectId> _objectCurrentMapped;
    QList<NodeObjectId> _objectToMap;

    bool sendData(const QByteArray data);
    void arrangeData(QByteArray &request, const QVariant &data);

    enum CommParam
    {
        PDO_COMM_COB_ID = 0x01,
        PDO_COMM_TRASMISSION_TYPE = 0x02,
        PDO_COMM_INHIBIT_TIME = 0x03,
        PDO_COMM_RESERVED = 0x04,
        PDO_COMM_EVENT_TIMER = 0x05,
        PDO_COMM_SYNC_START_VALUE = 0x06
    };
    struct PDO_conf
    {
        quint8 transType;
        quint32 inhibitTime; // The value is defined as multiple of 100 μs. The value of 0 shall disable the inhibit time.
        quint32 eventTimer; // The value is defined as multiple of 1 ms. The value of 0 shall disable the event-timer.
        quint8 syncStartValue;
    };

    PDO_conf _waitingConf;
};

#endif // PDO_H
