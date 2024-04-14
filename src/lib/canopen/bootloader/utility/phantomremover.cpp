/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "phantomremover.h"

PhantomRemover::PhantomRemover()
{
}

const QByteArray &PhantomRemover::remove(const QByteArray &prog)
{
    int index = 0;
    while (index < prog.size())
    {
        _prog.append(prog.mid(index, 3));
        index += 4;
    }

    return _prog;
}

const QByteArray &PhantomRemover::replace(const QByteArray &prog)
{
    int index = 4;
    while (index < prog.size())
    {
        _prog.replace(index, 1, "00");
        index += 4;
    }

    return _prog;
}
