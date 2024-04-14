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

#include <QDebug>

#include "canopenbus.h"
#include "nmt.h"

enum
{
    NMT_CS_START = 0x01,
    NMT_CS_STOP = 0x02,
    NMT_CS_PRE_OP = 0x80,
    NMT_CS_RESET_NODE = 0x81,
    NMT_CS_RESET_COMM = 0x82
};

NMT::NMT(Node *node)
    : Service(node)
{
    _cobId = 0x000;
    _nodeId = node->nodeId();
    _cobIds.append(_cobId);
}

QString NMT::type() const
{
    return QStringLiteral("NMT");
}

uint32_t NMT::cobId() const
{
    return _cobId;
}

void NMT::sendPreop()
{
    sendNmt(NMT_CS_PRE_OP);
    _node->setStatus(Node::PREOP);
}

void NMT::sendStart()
{
    sendNmt(NMT_CS_START);
    _node->setStatus(Node::STARTED);
}

void NMT::sendStop()
{
    sendNmt(NMT_CS_STOP);
    _node->setStatus(Node::STOPPED);
}

void NMT::sendResetComm()
{
    sendNmt(NMT_CS_RESET_COMM);
}

void NMT::sendResetNode()
{
    sendNmt(NMT_CS_RESET_NODE);
}

void NMT::sendNodeGuarding()
{
    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(0x700 + _nodeId);
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    bus()->writeFrame(frameNodeGuarding);
}

void NMT::parseFrame(const QCanBusFrame &frame)
{
    Q_UNUSED(frame)
}

void NMT::sendNmt(quint8 cmd)
{
    if (!bus()->canWrite())
    {
        return;
    }

    QByteArray nmtStopPayload;
    nmtStopPayload.append(static_cast<char>(cmd));
    nmtStopPayload.append(static_cast<char>(_nodeId));
    QCanBusFrame frameNmt;
    frameNmt.setFrameId(_cobId);
    frameNmt.setPayload(nmtStopPayload);
    bus()->writeFrame(frameNmt);
}
