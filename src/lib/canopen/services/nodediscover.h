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

#ifndef NODEDISCOVER_H
#define NODEDISCOVER_H

#include "canopen_global.h"

#include "service.h"

#include <QQueue>
#include <QTimer>

#include "db/oddb.h"

class CANOPEN_EXPORT NodeDiscover : public Service
{
    Q_OBJECT
public:
    NodeDiscover(CanOpenBus *bus);
    ~NodeDiscover();

    QString type() const override;

    void parseFrame(const QCanBusFrame &frame) override;

    void exploreBus();
    void exploreNode(quint8 nodeId);

protected slots:
    void exploreBusNext();
    void exploreNodeNext();

protected:
    OdDb *_db;

    // explorer bus
    quint8 _exploreBusNodeId;
    QTimer _exploreBusTimer;

    // explorer node
    QQueue<quint8> _nodeIdToExplore;
    quint8 _exploreNodeCurrentId;
    QTimer _exploreNodeTimer;
    int _exploreNodeState;
};

#endif // NODEDISCOVER_H
