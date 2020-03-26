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

#include "services/services.h"
#include "nodeod.h"

class CanOpenBus;

class CANOPEN_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    Node(quint8 nodeId, const QString &name = QString());
    ~Node();

    CanOpenBus *bus() const;
    QList<Service *> services() const;

    quint8 nodeId() const;
    void setNodeId(const quint8 &nodeId);

    QString name() const;
    void setName(const QString &name);

    void readObjet(Index &index, uint8_t subindex);
    void writeObjet(Index &index, uint8_t subindex);

    void loadEds(const QString &fileName);
    void updateFirmware(const QByteArray &prog);

    QString device();
    QString manuDeviceName();
    QString manufacturerHardwareVersion();
    QString manufacturerSoftwareVersion();

    enum Status
    {
        INIT,
        PREOP,
        STARTED,
        STOPPED
    };
    Status status() const;
    QString statusStr() const;
    void setStatus(Status status);

public slots:
    void sendStart();
    void sendStop();
    void sendResetComm();
    void sendResetNode();

protected:
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

    friend class CanOpenBus;
    CanOpenBus *_bus;
    NodeOd *_nodeOd;
};

#endif // NODE_H
