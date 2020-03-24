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

#include "model/deviceconfiguration.h"
#include "services/services.h"

class CanOpenBus;

class CANOPEN_EXPORT Node : public QObject
{
    Q_OBJECT
public:
    Node(CanOpenBus *bus);
    ~Node();

    CanOpenBus *bus() const;
    void setBus(CanOpenBus *bus);

    uint32_t nodeId() const;
    void setNodeId(const uint32_t &nodeId);

    QString name() const;
    void setName(const QString &name);

    void addEds(const QString &fileName);
    void updateFirmware(const QByteArray &prog);

    QString device();
    QString manuDeviceName();
    QString manufacturerHardwareVersion();
    QString manufacturerSoftwareVersion();

    enum Status {
        PREOP,
        STARTED,
        STOPPED
    };

protected:
    uint32_t _nodeId;
    QString _name;
    Status _status;

    // services
    QList<SDO *> _sdoClients;
    QList<TPDO *> _tpdos;
    QList<RPDO *> _rpdos;
    Emergency *_emergency;
    NMT *_nmt;

    CanOpenBus *_bus;
    DeviceConfiguration *_deviceConfiguration;
};

#endif // NODE_H
