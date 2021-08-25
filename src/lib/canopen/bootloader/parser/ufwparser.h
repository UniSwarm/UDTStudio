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

#ifndef UFWPARSER_H
#define UFWPARSER_H

#include "canopen_global.h"

#include <QByteArray>
#include <QList>
#include <QString>

class CANOPEN_EXPORT UfwParser
{
public:
    UfwParser(const QString &fileName = QString());

    bool read();
    const QByteArray &prog() const;

    struct Segment
    {
        uint32_t memorySegmentStart;
        uint32_t memorySegmentEnd;
    };

    struct Head
    {
        uint16_t device;
        //        QString version;
        //        QString date;
        uint32_t vendorId;
        uint32_t productId;
        uint32_t revision;
        uint32_t serial;
        uint8_t countSegment;
        QList<Segment *> segmentList;
    };

    const UfwParser::Head &head() const;

private:
    QString _fileName;
    Head *_head;
    QByteArray _prog;
};

#endif // UFWPARSER_H
