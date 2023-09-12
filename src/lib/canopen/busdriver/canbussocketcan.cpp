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

#include "canbussocketcan.h"

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

#include <linux/can.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
using namespace std;

int createSocketCan(const QString &adress)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    // open socket
    int can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_socket < 0)
    {
        close(can_socket);
        return -1;
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, adress.toLocal8Bit().data());

    if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0)
    {
        close(can_socket);
        return -1;
    }

    addr.can_ifindex = ifr.ifr_ifindex;

    fcntl(can_socket, F_SETFL, O_NONBLOCK);

#ifndef Q_OS_ANDROID
    // TODO find a way to do that...
    if (bind(can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(can_socket);
        return -1;
    }
#endif

    return can_socket;
}

CanBusSocketCAN::CanBusSocketCAN(const QString &adress)
    : CanBusDriver(adress)
{
    _can_socket = -1;
    _readNotifier = nullptr;
    _errorNotifier = nullptr;
}

CanBusSocketCAN::~CanBusSocketCAN()
{
    disconnectDevice();
}

bool CanBusSocketCAN::connectDevice()
{
    QMutexLocker socketLocker(&_socketMutex);

    _can_socket = createSocketCan(_adress);
    if (_can_socket < 0)
    {
        return false;
    }

    _readNotifier = new CanBusSocketCANNotifierThead(this);
    _readNotifier->start();

    _errorNotifier = new QSocketNotifier(_can_socket, QSocketNotifier::Exception, this);
    connect(_errorNotifier, &QSocketNotifier::activated, this, &CanBusSocketCAN::handleError);

    setState(CONNECTED);
    return true;
}

void CanBusSocketCAN::disconnectDevice()
{
    QMutexLocker socketLocker(&_socketMutex);
    if (_can_socket == -1)
    {
        return;
    }

    _readNotifier->terminate();
    _errorNotifier->setEnabled(false);
    close(_can_socket);
    _can_socket = -1;
    setState(DISCONNECTED);
    _readNotifier->deleteLater();
    _errorNotifier->deleteLater();
}

QCanBusFrame CanBusSocketCAN::readFrame()
{
    QMutexLocker socketLocker(&_socketMutex);
    QCanBusFrame qtFrame;

    struct can_frame frame;

    int recvbytes = read(_can_socket, &frame, sizeof(struct can_frame));
    if (recvbytes < 0)
    {
        qtFrame.setFrameType(QCanBusFrame::InvalidFrame);
        return qtFrame;
    }

    struct timeval tv;
    ioctl(_can_socket, SIOCGSTAMP_OLD, &tv);
    qtFrame.setTimeStamp(QCanBusFrame::TimeStamp(tv.tv_sec, tv.tv_usec));

    qtFrame.setFrameId(frame.can_id & 0x1FFFFFFF);

    QByteArray payLoad;
    for (int i = 0; i < frame.can_dlc; i++)
    {
        payLoad.append(frame.data[i]);
    }
    qtFrame.setPayload(payLoad);

    qtFrame.setFrameType(QCanBusFrame::DataFrame);
    if ((frame.can_id & CAN_RTR_FLAG) != 0)
    {
        qtFrame.setFrameType(QCanBusFrame::RemoteRequestFrame);
    }

    return qtFrame;
}

bool CanBusSocketCAN::writeFrame(const QCanBusFrame &qtframe)
{
    QMutexLocker socketLocker(&_socketMutex);
    int retval;
    struct can_frame frame;

    frame.can_id = qtframe.frameId();

    if (qtframe.frameType() == QCanBusFrame::RemoteRequestFrame)
    {
        frame.can_id += CAN_RTR_FLAG;
    }

    frame.can_dlc = qtframe.payload().size();
    for (int i = 0; i < qtframe.payload().size(); i++)
    {
        frame.data[i] = qtframe.payload()[i];
    }

    retval = write(_can_socket, &frame, sizeof(struct can_frame));
    return (retval == sizeof(struct can_frame));
}

void CanBusSocketCAN::notifyRead()
{
    emit framesReceived();
}

void CanBusSocketCAN::handleError()
{
    disconnectDevice();
}

CanBusSocketCANNotifierThead::CanBusSocketCANNotifierThead(CanBusSocketCAN *driver)
    : QThread(driver)
{
    _driver = driver;
}

CanBusSocketCANNotifierThead::~CanBusSocketCANNotifierThead()
{
    terminate();
}

void CanBusSocketCANNotifierThead::run()
{
    fd_set rdfs;
    bool running = true;

    _can_socket = createSocketCan(_driver->_adress);

    while (running)
    {
        FD_ZERO(&rdfs);
        FD_SET(_can_socket, &rdfs);

        if (select(_can_socket + 1, &rdfs, nullptr, nullptr, nullptr) <= 0)
        {
            terminate();
            running = false;
        }
        else
        {
            struct can_frame frame;
            read(_can_socket, &frame, sizeof(struct can_frame));
            _driver->notifyRead();
        }
    }
}
