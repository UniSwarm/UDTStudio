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

#ifndef NODEPROFILE_H
#define NODEPROFILE_H

#include "canopen_global.h"

#include "../services/service.h"

#include "nodeodsubscriber.h"

class Node;

class CANOPEN_EXPORT NodeProfile : public QObject, public NodeOdSubscriber
{
public:
    NodeProfile(Node *node, quint8 axis = 0);

    quint8 busId() const;
    quint8 nodeId() const;
    Node *node() const;

    quint8 axis() const;

    virtual void start(int msec) = 0;
    virtual void stop() = 0;
    virtual bool status() const;
    virtual void readRealTimeObjects() const = 0;
    virtual void readAllObjects() const = 0;
    virtual quint16 profileNumber() const = 0;
    virtual QString profileNumberStr() const = 0;
    virtual void reset() = 0;

protected:
    Node *_node;
    quint8 _axis;
};

#endif  // NODEPROFILE_H
