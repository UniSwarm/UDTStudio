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

#include "canbusdriver.h"

CanBusDriver::CanBusDriver(const QString &adress)
    : _adress(adress)
{
    _state = DISCONNECTED;
}

CanBusDriver::State CanBusDriver::state() const
{
    return _state;
}

bool CanBusDriver::connectDevice()
{
    return false;
}

void CanBusDriver::disconnectDevice()
{
}

QCanBusFrame CanBusDriver::readFrame()
{
    return QCanBusFrame();
}

bool CanBusDriver::writeFrame(const QCanBusFrame &qtframe)
{
    Q_UNUSED(qtframe);
    return false;
}

void CanBusDriver::setState(const State &state)
{
    bool stateChange = (_state != state);
    _state = state;
    if (stateChange)
    {
        emit stateChanged(_state);
    }
}

const QString &CanBusDriver::adress() const
{
    return _adress;
}
