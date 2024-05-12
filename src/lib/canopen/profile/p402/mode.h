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

#ifndef MODE_H
#define MODE_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"
#include <QObject>

#include "nodeprofile402.h"

class CANOPEN_EXPORT Mode : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    Mode(NodeProfile402 *nodeProfile402);

    virtual void setTarget(qint32 tarqet) = 0;
    virtual quint16 getSpecificCwFlag() = 0;
    virtual void setCwDefaultflag() = 0;

    virtual void readRealTimeObjects();
    virtual void readAllObjects();
    virtual void reset();

protected:
    NodeProfile402 *_nodeProfile402;
    NodeObjectId _controlWordObjectId;

    NodeProfile402::OperationMode _mode;
};

#endif  // MODE_H
