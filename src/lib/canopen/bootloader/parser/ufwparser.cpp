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

#include "ufwparser.h"

#include <QFile>

UfwParser::UfwParser(const QString &fileName)
{
    _fileName = fileName;
}

bool UfwParser::read()
{
    QByteArray uni;
    QFile file(_fileName);
    if (!file.open(QFile::ReadOnly))
    {
        return false;
    }
    else
    {
        uni = file.read(file.size());
        file.close();
    }

    _head = *reinterpret_cast<Head*>(uni.data());
    _prog = uni.mid(sizeof(Head), uni.size());
    return true;
}

const QByteArray &UfwParser::prog() const
{
    return _prog;
}

const UfwParser::Head &UfwParser::head() const
{
    return _head;
}
