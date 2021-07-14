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

#include "canbustcpudt.h"

#include <QDataStream>
#include <QDebug>

CanBusTcpUDT::CanBusTcpUDT(const QString &adress)
    : CanBusDriver(adress)
{
    _sock = new QTcpSocket;
    _state = DISCONNECTED;
    QObject::connect(_sock, &QIODevice::readyRead, this, &CanBusTcpUDT::readTCP);
}

bool CanBusTcpUDT::connectDevice()
{
    _sock->connectToHost(_adress, 5050);
    _state = CONNECTED;
    return _sock->waitForConnected(100);
}

void CanBusTcpUDT::disconnectDevice()
{
    _sock->disconnectFromHost();
    _state = DISCONNECTED;
}

QCanBusFrame CanBusTcpUDT::readFrame()
{
    if (_queue.isEmpty())
    {
        return QCanBusFrame(QCanBusFrame::InvalidFrame);
    }

    return _queue.dequeue();
}

bool CanBusTcpUDT::writeFrame(const QCanBusFrame &qtframe)
{
    quint8 flags;
    quint8 dlc;
    switch (qtframe.frameType())
    {
    case QCanBusFrame::UnknownFrame:
    case QCanBusFrame::InvalidFrame:
        return false;

    case QCanBusFrame::DataFrame:
        if (qtframe.hasExtendedFrameFormat())
        {
            flags = 2;
        }
        else
        {
            flags = 1;
        }
        break;

    case QCanBusFrame::ErrorFrame:
        flags = 3;
        break;

    case QCanBusFrame::RemoteRequestFrame:
        flags = 4;
        break;
    }

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    dlc = qtframe.payload().size();

    stream << static_cast<quint8>('U');  // Magic id
    stream << static_cast<quint8>(0);    // bus id
    stream << flags;                     // flags
    stream << dlc;                       // dlc

    stream << static_cast<quint32>(qtframe.frameId());  // frame id
    data.append(qtframe.payload());

    // qDebug() << data.size() << data.toHex().toUpper() << qtframe.payload().toHex().toUpper();
    return (_sock->write(data) != -1);
}

void CanBusTcpUDT::readTCP()
{
    QByteArray data = _sock->readAll();
    int id = 0;
    int rec = 0;

    while (rec >= 0)
    {
        rec = readPacket(data.mid(id));
        id += rec;
    }
}

int CanBusTcpUDT::readPacket(QByteArray data)
{
    QDataStream stream(&data, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint8 magic;
    quint8 busid;
    quint8 flags;
    quint8 dlc;
    quint32 frameId;
    stream >> magic;
    stream >> busid;
    stream >> flags;
    stream >> dlc;
    stream >> frameId;

    if (magic != 'U' || busid != 0)
    {
        return -1;
    }

    QCanBusFrame qtFrame;
    qtFrame.setFrameId(frameId);

    QByteArray payLoad;
    for (int i = 0; i < dlc; i++)
    {
        payLoad.append(data[i + 8]);
    }
    qtFrame.setPayload(payLoad);

    qtFrame.setFrameType(QCanBusFrame::DataFrame);
    if (flags == 4)
    {
        qtFrame.setFrameType(QCanBusFrame::RemoteRequestFrame);
    }

    _queue.append(qtFrame);
    emit framesReceived();

    return dlc + 8;
}
