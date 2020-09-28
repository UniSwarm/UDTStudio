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

#ifndef CANOPEN_H
#define CANOPEN_H

#include "canopen_global.h"

#include "canopenbus.h"

#include <QList>

class CANOPEN_EXPORT CanOpen : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CanOpen)

public:
    static const QList<CanOpenBus *> &buses();
    static CanOpenBus *addBus(CanOpenBus *bus);
    static CanOpenBus *bus(const quint8 busId);

    static inline CanOpen *instance()
    {
        if (!CanOpen::_instance)
        {
            CanOpen::_instance = new CanOpen();
        }
        return CanOpen::_instance;
    }

    static inline void release()
    {
        delete CanOpen::_instance;
    }

signals:
    void busChanged();

protected:
    CanOpen();
    ~CanOpen();
    QList<CanOpenBus *> _buses;
    CanOpenBus *addBusI(CanOpenBus *bus);

    static CanOpen *_instance;
};

#endif // CANOPEN_H
