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

#ifndef PDO_H
#define PDO_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"
#include "service.h"

#include "nodeobjectid.h"
#include "nodeod.h"

class RPDO;
class TPDO;

class CANOPEN_EXPORT PDO : public Service, public NodeOdSubscriber
{
    Q_OBJECT
public:
    PDO(Node *node, quint8 number);

    quint32 cobId() const;
    quint8 pdoNumber() const;

    // PDO enable
    bool isEnabled() const;
    void setEnabled(bool enabled);

    // PDO currentMappind access
    void readMapping();
    const QList<NodeObjectId> &currentMappind() const;
    bool hasMappedObject() const;
    bool isMappedObject(const NodeObjectId &object) const;

    // PDO mappind availability and modify
    void writeMapping(const QList<NodeObjectId> &objectList);
    bool canInsertObjectAtBitPos(const NodeObjectId &object, int bitPos) const;
    bool canInsertObjectAtBitPos(const QList<NodeObjectId> &objectList, const NodeObjectId &object, int bitPos) const;
    int maxMappingBitSize() const;
    int mappingBitSize() const;
    int maxMappingObjectCount() const;
    int mappingObjectCount() const;
    static int mappingBitSize(const QList<NodeObjectId> &objectList);
    int indexAtBitPos(int bitPos) const;
    static int indexAtBitPos(const QList<NodeObjectId> &objectList, int bitPos);

    virtual bool isTPDO() const = 0;
    virtual bool isRPDO() const = 0;

    // common PDO settings
    qreal inhibitTimeMs() const;
    void setInhibitTimeMs(qreal inhibitTimeMs);

    quint32 eventTimerMs() const;
    void setEventTimerMs(quint32 eventTimerMs);

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
        ERROR_WRITE_PARAM,
        ERROR_MODIFY_MAPPING,
        ERROR_DEACTIVATE_COBID,
        ERROR_DISABLE_MAPPING,
        ERROR_GENERAL_ERROR
    };

signals:
    void mappingChanged();
    void enabledChanged(bool enabled);
    void cobIdChanged(quint32 cobId);
    void errorOccurred(PDO::ErrorPdo error);

protected:
    quint32 _cobId;
    quint8 _pdoNumber;

    enum StatusPdo
    {
        STATE_NONE,
        STATE_READ,
        STATE_WRITE
    };
    StatusPdo _statusPdo;

    enum StateMapping
    {
        STATE_FREE,
        STATE_DEACTIVATE,
        STATE_DISABLE,
        STATE_MODIFY,
        STATE_ENABLE,
        STATE_ACTIVATE
    };
    StateMapping _stateMapping;

    quint8 _objectIdFsm;

    quint16 _objectMappingId;
    quint16 _objectCommId;

    QList<NodeObjectId> _objectCommList;

    void managementRespWriteCommParam(const NodeObjectId &objId, NodeOd::FlagsRequest flags);
    void managementRespReadCommAndMapping(const NodeObjectId &objId, NodeOd::FlagsRequest flags);
    void managementRespProcessMapping(const NodeObjectId &objId, NodeOd::FlagsRequest flags);
    bool createListObjectMapped();

    QList<NodeObjectId> _currentMappedObjectsId;
    QList<NodeObjectId> _objectToMap;

    enum CommParam
    {
        PDO_COMM_NUMBER = 0x00,
        PDO_COMM_COB_ID = 0x01,
        PDO_COMM_TRANSMISSION_TYPE = 0x02,
        PDO_COMM_INHIBIT_TIME = 0x03,
        PDO_COMM_RESERVED = 0x04,
        PDO_COMM_EVENT_TIMER = 0x05,
        PDO_COMM_SYNC_START_VALUE = 0x06
    };

    struct PDO_conf
    {
        quint8 transType;
        quint32 inhibitTime;  // The value is defined as multiple of 100 μs. The value of 0 shall disable the inhibit time.
        quint32 eventTimer;   // The value is defined as multiple of 1 ms. The value of 0 shall disable the event-timer.
        quint8 syncStartValue;
    };
    PDO_conf _waitingConf;

    void setError(ErrorPdo error);

private:
    void readCommParam();
    void readMappingParam();
    void processMapping();

    bool checkIndex(quint16 index);
    virtual void clearDataWaiting();

    // Service interface
public:
    void reset() override;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // PDO_H
