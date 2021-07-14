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

#ifndef CANBUSDRIVER_H
#define CANBUSDRIVER_H

#include "canopen_global.h"

#include <QObject>

#include <QCanBusFrame>

class CANOPEN_EXPORT CanBusDriver : public QObject
{
    Q_OBJECT
public:
    CanBusDriver(const QString &adress);

    const QString &adress() const;
    enum State
    {
        DISCONNECTED,
        CONNECTED,
        ERROR
    };
    State state() const;

    virtual bool connectDevice() = 0;
    virtual void disconnectDevice() = 0;

    virtual QCanBusFrame readFrame() = 0;
    virtual bool writeFrame(const QCanBusFrame &qtframe) = 0;

signals:
    void framesReceived();

protected:
    QString _adress;
    State _state;
};

#endif // CANBUSDRIVER_H
