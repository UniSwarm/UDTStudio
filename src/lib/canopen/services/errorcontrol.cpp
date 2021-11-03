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

#include "errorcontrol.h"

#include <QDebug>

#include "canopenbus.h"

ErrorControl::ErrorControl(Node *node)
    : Service(node)
{
    _cobId = 0x700;
    _cobIds.append(_cobId + node->nodeId());
    _oldToggleBit = false;

    _guardTimeTimer = new QTimer();
    _lifeTimeTimer = new QTimer();
    connect(_guardTimeTimer, &QTimer::timeout, this, &ErrorControl::sendNodeGuarding);
    connect(_lifeTimeTimer, &QTimer::timeout, this, &ErrorControl::lifeGuardingEvent);

    _guardTime = 0x100C;
    _lifeTime = 0x100D;

    registerObjId({_guardTime, 0});
    setNodeInterrest(node);
}

QString ErrorControl::type() const
{
    return QLatin1String("ErrorControl");
}

void ErrorControl::parseFrame(const QCanBusFrame &frame)
{
    if (frame.payload().size() == 1)
    {
        if (static_cast<uint8_t>(frame.payload().at(0)) == 0x0)
        {
            // BootUp
            _node->setStatus(Node::Status::PREOP);
            _node->reset();
            _oldToggleBit = false;
            return;
        }
    }

    /*if (_node->nodeOd()->indexExist(_lifeTime))
    {
        qint32 timer;
        if (_node->nodeOd()->value(_lifeTime, 0).toUInt() != 0)
        {
            timer = (_node->nodeOd()->value(_lifeTime, 0).toInt()) * _node->nodeOd()->value(_guardTime, 0).toInt();
            _lifeTimeTimer->start(timer);
        }
        else
        {
            timer = _node->nodeOd()->value(_guardTime, 0).toInt();
            _lifeTimeTimer->start(timer + (timer / 2));
        }
    }*/
    manageErrorControl(frame);
}

void ErrorControl::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index() == _guardTime) && _node->nodeOd()->indexExist(_guardTime))
    {
        if ((flags != SDO::FlagsRequest::Error) && (_node->nodeOd()->value(_guardTime, 0).toUInt() != 0))
        {
            //_guardTimeTimer->start(_node->nodeOd()->value(_guardTime, 0).toInt());
        }
        else
        {
            _guardTimeTimer->stop();
            _lifeTimeTimer->stop();
        }
    }
}

void ErrorControl::receiveHeartBeat()
{
    if (!bus()->canWrite())
    {
        return;
    }

    // heartbeat consumers
    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(_cobId + _node->nodeId());
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    bus()->writeFrame(frameNodeGuarding);
}

void ErrorControl::sendNodeGuarding()
{
    if (!bus()->canWrite())
    {
        return;
    }

    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(_cobId + _node->nodeId());
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    bus()->writeFrame(frameNodeGuarding);
}

void ErrorControl::lifeGuardingEvent()
{
    // qDebug() << ">>ErrorControl::lifeGuardingEvent : node error, dont answer";
}

void ErrorControl::manageErrorControl(const QCanBusFrame &frame)
{
    if (frame.payload().size() == 1)
    {
        bool actualToggleBit = (frame.payload().at(0) >> 7);
        if (_oldToggleBit == actualToggleBit)
        {
            // ERROR TogleBit -> connection lost
            // return;
        }
        _oldToggleBit = actualToggleBit;

        switch (frame.payload().at(0) & 0x7F)
        {
            case 4:  // Stopped
                _node->setStatus(Node::Status::STOPPED);
                break;

            case 5:  // Operational
                _node->setStatus(Node::Status::STARTED);
                break;

            case 127:  // Pre-operational
                _node->setStatus(Node::Status::PREOP);
                break;

            default:
                // qDebug() << "Error control : error state" << QString::number(frame.frameId(), 16).toUpper() << frame.payload().toHex().toUpper();
                break;
        }
    }
}
