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

#ifndef SERVICE_H
#define SERVICE_H

#include "canopen_global.h"

#include <QObject>
#include <QString>
#include <QCanBusFrame>
#include <QCanBusDevice>

class CanOpenBus;
class Node;

class CANOPEN_EXPORT Service : public QObject
{
    Q_OBJECT
public:
    Service(CanOpenBus *bus);
    Service(Node *node);
    virtual ~Service();

    virtual void setBus(CanOpenBus *bus);

    virtual void reset();

    virtual QString type() const = 0;

    virtual void parseFrame(const QCanBusFrame &frame) = 0;

    const QList<quint32> &cobIds() const;

    CanOpenBus *bus() const;
    QCanBusDevice *canDevice() const;
    Node *node() const;

protected:
    CanOpenBus *_bus;
    Node *_node;
    QList<quint32> _cobIds;
};

#endif // SERVICE_H
