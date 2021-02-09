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

#ifndef MODE_H
#define MODE_H

#include "canopen_global.h"

#include "../../services/service.h"

#include "nodeodsubscriber.h"

class NodeProfile402;

class CANOPEN_EXPORT Mode : public QObject, public NodeOdSubscriber
{
public:
    Mode(NodeProfile402 *nodeProfile402);

protected:
    Node *_node;
    NodeProfile402 *_nodeProfile402;
    uint8_t _axisId;
};

#endif // MODE_H
