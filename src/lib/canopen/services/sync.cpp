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

#include "sync.h"

#include "canopenbus.h"

const quint8 ONE_SHOT_TIMER = 20;

Sync::Sync(CanOpenBus *bus)
    : Service (bus)
{
    _syncCobId = 0x80;
    _cobIds.append(_syncCobId);
    _syncTimer = new QTimer();
    _status = STOPPED;
    _signalBeforeSync = new QTimer();
    _syncOneShotTimer = new QTimer();
    connect(_syncTimer, &QTimer::timeout, this, &Sync::sendSync);
    connect(_signalBeforeSync, &QTimer::timeout, this, &Sync::signalBeforeSync);
}

Sync::~Sync()
{
    delete _syncTimer;
}

QString Sync::type() const
{
    return QLatin1String("Sync");
}

void Sync::startSync(int ms)
{
    _syncTimer->start(ms);
    _signalBeforeSync->start((ms * 3) / 4);
    _status = STARTED;
}

void Sync::stopSync()
{
    _syncTimer->stop();
    _signalBeforeSync->stop();
    _status = STOPPED;
}

Sync::Status  Sync::status()
{
    return _status;
}

void Sync::sendSync()
{
    if (!bus()->canWrite())
    {
        return;
    }

    QCanBusFrame frameSync;
    frameSync.setFrameId(_syncCobId);
    bus()->writeFrame(frameSync);
    emit syncEmitted();
}

void Sync::sendSyncOneTimeout()
{
    sendSync();
    _status = STOPPED;
}

void Sync::sendSyncOne()
{
    if (_status == STARTED)
    {
        return;
    }
    _status = STARTED;
    emit signalBeforeSync();
    _syncOneShotTimer->singleShot(ONE_SHOT_TIMER, this, SLOT(sendSyncOneTimeout()));
}

void Sync::parseFrame(const QCanBusFrame &frame)
{
    if (frame.frameId() == _syncCobId && frame.payload().isEmpty())
    {
        emit syncEmitted();
    }
}
