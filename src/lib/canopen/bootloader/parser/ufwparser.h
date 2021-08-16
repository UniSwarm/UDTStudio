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
#include <QString>

class CANOPEN_EXPORT UfwParser
{
public:
    UfwParser(const QString &fileName = QString());

    bool read();
    const QByteArray &prog() const;

    struct Head
    {
        uint32_t deviceModel;
        uint32_t memoryBlockStart1;
        uint32_t memoryBlockEnd1;
        uint32_t memoryBlockStart2;
        uint32_t memoryBlockEnd2;
    };

    const UfwParser::Head &head() const;

private:
    QString _fileName;
    Head _head;
    QByteArray _prog;
};

#endif // UFWPARSER_H