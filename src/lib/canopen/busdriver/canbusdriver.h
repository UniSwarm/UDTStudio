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

#include "busdriver/qcanbusframe.h"

class CANOPEN_EXPORT CanBusDriver : public QObject
{
    Q_OBJECT
public:
    CanBusDriver(QString adress);

    const QString &adress() const;
    enum State
    {
        DISCONNECTED,
        CONNECTED,
        ERROR
    };
    State state() const;

    virtual bool connectDevice();
    virtual void disconnectDevice();

    virtual QCanBusFrame readFrame();
    virtual bool writeFrame(const QCanBusFrame &qtframe);

signals:
    void framesReceived();
    void stateChanged(CanBusDriver::State);

protected:
    QString _adress;
    void setState(State state);

private:
    State _state;
};

#endif  // CANBUSDRIVER_H
