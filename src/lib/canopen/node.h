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

#ifndef NODE_H
#define NODE_H

#include "canopen_global.h"

#include <QObject>

#include <QMetaType>

#include "nodeod.h"

class CanOpenBus;

class TPDO;
class RPDO;
class Emergency;
class NMT;
class ErrorControl;
class NodeProfile;
class Bootloader;

class CANOPEN_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    Node(quint8 nodeId, const QString &name = QString(), const QString &edsFileName = QString());
    ~Node() override;

    CanOpenBus *bus() const;
    quint8 busId() const;

    quint8 nodeId() const;
    void setNodeId(quint8 nodeId);

    enum Status
    {
        UNKNOWN,
        INIT,
        PREOP,
        STARTED,
        STOPPED
    };
    Status status() const;
    QString statusStr() const;
    void setStatus(Status status);

    const QString &name() const;
    quint32 vendorId() const;
    quint32 productCode() const;
    quint16 profileNumber() const;

    // store / restore
    enum StoreSegment
    {
        StoreAll = 1,
        StoreCom = 2,
        StoreApp = 3,
        StoreMan = 4
    };
    void store(StoreSegment segment);

    enum RestoreSegment
    {
        RestoreFactoryAll = 1,
        RestoreFactoryCom = 2,
        RestoreFactoryApp = 3,
        RestoreFactoryMan = 4,
        RestoreSavedAll = 5,
        RestoreSavedCom = 6,
        RestoreSavedApp = 7,
        RestoreSavedMan = 8
    };
    void restore(RestoreSegment segment);

    // Node od
    NodeOd *nodeOd() const;
    void readObject(const NodeObjectId &id);
    void readObject(quint16 index, quint8 subindex, QMetaType::Type dataType = QMetaType::UnknownType);
    void writeObject(const NodeObjectId &id, const QVariant &data);
    void writeObject(quint16 index, quint8 subindex, const QVariant &data);

    void loadEds(const QString &fileName);
    const QString &edsFileName() const;

    // Profiles
    void addProfile(NodeProfile *nodeProfile);
    const QList<NodeProfile *> &profiles() const;
    int profilesCount() const;

    // PDOs
    const QList<TPDO *> &tpdos() const;
    const QList<RPDO *> &rpdos() const;
    bool isMappedObjectInPdo(const NodeObjectId &object) const;
    RPDO *rpdoMappedObject(const NodeObjectId &object) const;
    TPDO *tpdoMappedObject(const NodeObjectId &object) const;

    Bootloader *bootloader() const;

    QList<Service *> services() const;

    void reset();

public slots:
    void setName(const QString &name);

    void sendPreop();
    void sendStart();
    void sendStop();
    void sendResetComm();
    void sendResetNode();
    void sendStatusReq();

signals:
    void nameChanged(const QString &);
    void statusChanged(Node::Status);
    void edsFileChanged(const QString &);

protected:
    friend class CanOpenBus;
    void setBus(CanOpenBus *bus);
    CanOpenBus *_bus;

    quint8 _nodeId;
    QString _name;
    Status _status;

    // services
    QList<SDO *> _sdoClients;
    QList<TPDO *> _tpdos;
    QList<RPDO *> _rpdos;
    Emergency *_emergency;
    NMT *_nmt;
    ErrorControl *_errorControl;
    Bootloader *_bootloader;
    QList<Service *> _services;

    QList<NodeProfile *> _nodeProfiles;

    NodeOd *_nodeOd;
};

#endif  // NODE_H
