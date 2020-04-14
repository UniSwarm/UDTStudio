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

    quint32 cobId();
    quint8 pdoNumber();

    const QList<NodeObjectId> &currentMappind() const;

    void writeMapping(const QList<NodeObjectId> &objectList);
    void readMapping();

    bool isEnabled();
    void setEnabled(bool b);

    bool hasMappedObject();

    void setInhibitTime(quint32 inhibitTime);
    quint32 inhibitTime();

    void setEventTimer(quint32 eventTimer);
    quint32 eventTimer();

    enum ErrorPdo
    {
        ERROR_COBID_NOT_VALID,
        ERROR_WRITE_ONLY,
        ERROR_READ_ONLY,
        ERROR_NO_OBJECT,
        ERROR_NO_SUBINDEX,
        ERROR_CANNOT_MAP_PDO,
        ERROR_EXCEED_PDO_LENGTH,
        ERROR_PARAM_IMCOMPATIBILITY,
        ERROR_GENERAL_ERROR
    };

signals:
    void mappingChanged();

protected:
    quint32 _cobId;
    quint8 _pdoNumber;

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

    quint8 _iFsm;

    quint16 _objectMappingId;
    quint16 _objectCommId;

    QList<NodeObjectId> _objectCommList;

    void notifyReadPdo(const NodeObjectId &objId, SDO::FlagsRequest flags);
    void notifyWritePdo(const NodeObjectId &objId, SDO::FlagsRequest flags);

    QList<NodeObjectId> _objectCurrentMapped;
    QList<NodeObjectId> _objectToMap;

    bool sendData();
    void convertQVariantToQDataStream(QDataStream &request, const QVariant &data);

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

private:

    void readCommParam();
    void readMappingParam();
    void processMapping();
    bool createListObjectMapped();
};

#endif // PDO_H
