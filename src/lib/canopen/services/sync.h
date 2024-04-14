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

#ifndef SYNC_H
#define SYNC_H

#include "canopen_global.h"

#include "service.h"

#include <QTimer>

class CANOPEN_EXPORT Sync : public Service
{
    Q_OBJECT
public:
    Sync(CanOpenBus *bus);
    ~Sync() override;

    void startSync(int ms);
    void stopSync();

    enum Status
    {
        STARTED,
        STOPPED
    };
    Status status();

public slots:
    void sendSyncOne();

private slots:
    void sendSync();
    void sendSyncOneTimeout();

signals:
    void syncEmitted();
    void signalBeforeSync();
    void syncOneRequested();

private:
    Status _status;
    QTimer *_syncTimer;
    QTimer *_signalBeforeSync;
    uint32_t _syncCobId;

    // Service interface
public:
    QString type() const override;
    void parseFrame(const QCanBusFrame &frame) override;
};

#endif  // SYNC_H
