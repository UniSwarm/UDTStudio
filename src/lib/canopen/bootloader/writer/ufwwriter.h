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

#ifndef UFWWRITER_H
#define UFWWRITER_H

#include "canopen_global.h"

#include <QByteArray>
#include <QStringList>

class CANOPEN_EXPORT UfwWriter
{
public:
    UfwWriter();

    int create(const QByteArray &hex, QString type, QStringList segment);

    const QByteArray &binary() const;

private:
    struct Head
    {
        uint32_t deviceModel;
        uint32_t memoryBlockStart1;
        uint32_t memoryBlockend1;
        uint32_t memoryBlockStart2;
        uint32_t memoryBlockend2;
    };
    Head _head;

    QByteArray _binary;

    int append(const QByteArray &app, QStringList addresses);
    int checkAddresses(QStringList addresses);
};

#endif // UFWWRITER_H
