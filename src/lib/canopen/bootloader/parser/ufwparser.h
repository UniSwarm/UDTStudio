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

class UfwModel;

class CANOPEN_EXPORT UfwParser
{
public:
    UfwParser();

    UfwModel *parse(const QString &fileName) const;

    bool read();
    const QByteArray &prog() const;
};

#endif // UFWPARSER_H
