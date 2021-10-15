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

#include "../model/ufwmodel.h"

#include <QByteArray>
#include <QStringList>

class CANOPEN_EXPORT UfwWriter
{
public:
    UfwWriter();

    int create(uint16_t type, QString version, QString date, QStringList segment, const QByteArray &hex);

    const QByteArray &binary() const;

    void setBinary(const QByteArray &newBinary);

private:
    UfwModel *_ufwModel;
    QByteArray _ufwByteArray;

    int append(const QByteArray &app, QStringList addresses);
    int checkAddresses(QStringList addresses);
};

#endif // UFWWRITER_H
