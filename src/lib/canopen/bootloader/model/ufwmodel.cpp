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

#include "ufwmodel.h"

UfwModel::UfwModel()
{
    _device = 0;
}

uint16_t UfwModel::deviceType() const
{
    return _device;
}

void UfwModel::setDeviceType(uint16_t device)
{
    _device = device;
}

const QString &UfwModel::softwareVersion() const
{
    return _softwareVersion;
}

void UfwModel::setSoftwareVersion(const QString &version)
{
    _softwareVersion = version;
}

const QString &UfwModel::buildDate() const
{
    return _buildDate;
}

void UfwModel::setBuildDate(const QString &date)
{
    _buildDate = date;
}

const QList<UfwModel::Segment> &UfwModel::segmentList() const
{
    return _segmentList;
}

void UfwModel::appendSegment(uint32_t start, uint32_t end)
{
    Segment segment;
    segment.start = start;
    segment.end = end;
    _segmentList.append(segment);
}

const QByteArray &UfwModel::prog() const
{
    return _prog;
}

void UfwModel::setProg(const QByteArray &prog)
{
    _prog = prog;
}
