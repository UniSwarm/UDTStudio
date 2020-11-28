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

#ifndef NODE_H
#define NODE_H

#include "canopen_global.h"
#include <QMetaType>

#include "nodeod.h"

class CanOpenBus;

class TPDO;
class RPDO;
class Emergency;
class NMT;
class ErrorControl;
class NodeProfile;

class CANOPEN_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    Node(quint8 nodeId, const QString &name = QString(), const QString &edsFileName = QString());
    ~Node();

    CanOpenBus *bus() const;
    quint8 busId() const;
    QList<Service *> services() const;
    NodeOd *nodeOd() const;

    quint8 nodeId() const;
    void setNodeId(const quint8 nodeId);

    const QString &name() const;
    void setName(const QString &name);

    void readObject(const NodeObjectId &id);
    void readObject(const quint16 index, const quint8 subindex, const QMetaType::Type dataType = QMetaType::Type::UnknownType);
    void writeObject(const NodeObjectId &id, const QVariant &data);
    void writeObject(const quint16 index, const quint8 subindex, const QVariant &data);

    void loadEds(const QString &fileName);
    void updateFirmware(const QByteArray &prog);

    enum Status
    {
        INIT,
        PREOP,
        STARTED,
        STOPPED
    };
    Status status() const;
    QString statusStr() const;
    void setStatus(const Status status);

    quint16 manufacturerId() const;
    quint16 productCode() const;
    quint16 profileNumber() const;
    void addProfile(NodeProfile *nodeProfile);
    const QList<NodeProfile *> &profiles() const;
    int countProfile() const;

    const QList<TPDO *> &tpdos() const;
    const QList<RPDO *> &rpdos() const;

    bool isMappedObjectInPdo(const NodeObjectId &object) const;
    RPDO *isMappedObjectInRpdo(const NodeObjectId &object) const;
    TPDO *isMappedObjectInTpdo(const NodeObjectId &object) const;

    void reset();

signals:
    void statusChanged(Node::Status status);

public slots:
    void sendPreop();
    void sendStart();
    void sendStop();
    void sendResetComm();
    void sendResetNode();

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
    QList<Service *> _services;

    QList<NodeProfile *> _nodeProfiles;

    NodeOd *_nodeOd;
};

#endif // NODE_H
