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

#include <QDebug>

#include "nmt.h"
#include "canopenbus.h"

#define NMT_CS_START        0x01
#define NMT_CS_STOP         0x02
#define NMT_CS_PRE_OP       0x80
#define NMT_CS_RESET_NODE   0x81
#define NMT_CS_RESET_COMM   0x82

NMT::NMT(Node *node)
    : Service (node)
{
    _cobId = 0x000;
    _nodeId = node->nodeId();
    _cobIds.append(_cobId);
}

QString NMT::type() const
{
    return QLatin1String("NMT");
}

uint32_t NMT::cobId()
{
    return _cobId;
}

void NMT::sendPreop()
{
    sendNmt(NMT_CS_PRE_OP);
}

void NMT::sendStart()
{
    sendNmt(NMT_CS_START);
}

void NMT::sendStop()
{
    sendNmt(NMT_CS_STOP);
}

void NMT::sendResetComm()
{
    sendNmt(NMT_CS_RESET_COMM);
}

void NMT::sendResetNode()
{
    sendNmt(NMT_CS_RESET_NODE);
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
