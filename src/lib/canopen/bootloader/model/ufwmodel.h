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

#ifndef UFWMODEL_H
#define UFWMODEL_H

#include "canopen_global.h"

#include <QByteArray>
#include <QList>
#include <QString>

class CANOPEN_EXPORT UfwModel
{
public:
    UfwModel();
    ~UfwModel();

    struct Segment
    {
        uint32_t start;
        uint32_t end;
    };

    uint16_t deviceType() const;
    void setDeviceType(uint16_t device);

    const QString &softwareVersion() const;
    void setSoftwareVersion(const QString &version);

    const QString &buildDate() const;
    void setBuildDate(const QString &date);

    const QList<Segment> &segmentList() const;
    void appendSegment(uint32_t start, uint32_t end);

    const QByteArray &prog() const;
    void setProg(const QByteArray &prog);

private:
    uint16_t _device;
    QString _softwareVersion;
    QString _buildDate;
    QList<Segment> _segmentList;
    QByteArray _prog;
};

#endif  // UFWMODEL_H
