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

#ifndef ERRORCONTROL_H
#define ERRORCONTROL_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"
#include "service.h"

#include "nodeod.h"

class CANOPEN_EXPORT ErrorControl : public Service, public NodeOdSubscriber
{
    Q_OBJECT
public:
    ErrorControl(Node *node);

    uint32_t cobId();

    QString type() const override;

    void parseFrame(const QCanBusFrame &frame) override;

private slots:
    void sendNodeGuarding();
    void lifeGuardingEvent();

private:
    void receiveHeartBeat();
    void manageErrorControl(const QCanBusFrame &frame);

    uint32_t _cobId;
    bool _oldToggleBit;

    quint16 _guardTime;
    quint16 _lifeTime;
    QTimer *_guardTimeTimer;
    QTimer *_lifeTimeTimer;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // ERRORCONTROL_H
