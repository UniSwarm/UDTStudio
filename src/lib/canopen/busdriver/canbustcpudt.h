/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2021 UniSwarm
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

#ifndef CANBUSTCPUDT_H
#define CANBUSTCPUDT_H

#include "canopen_global.h"

#include "canbusdriver.h"

#include <QMutex>
#include <QQueue>
#include <QTcpSocket>

class CANOPEN_EXPORT CanBusTcpUDT : public CanBusDriver
{
    Q_OBJECT
public:
    CanBusTcpUDT(const QString &adress);

    // CanBusDriver interface
public:
    bool connectDevice() override;
    void disconnectDevice() override;
    QCanBusFrame readFrame() override;
    bool writeFrame(const QCanBusFrame &qtframe) override;

private:
    QMutex _socketMutex;
    QTcpSocket *_sock;
    QQueue<QCanBusFrame> _queue;

protected slots:
    void readTCP();

protected:
    int readPacket(QByteArray data);
    void stateChanged(QAbstractSocket::SocketState socketState);
};

#endif  // CANBUSTCPUDT_H
