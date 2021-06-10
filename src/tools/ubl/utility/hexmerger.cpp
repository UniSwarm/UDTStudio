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

#include "hexmerger.h"

HexMerger::HexMerger()
{
}

void HexMerger::merge(const QByteArray &app1, const QByteArray &app2, int startApp2, int endApp2)
{
    _prog.append(app1);
    _prog.replace(startApp2, endApp2 - startApp2, app2.mid(startApp2, endApp2 - startApp2));
}

const QByteArray &HexMerger::prog() const
{
    return _prog;
}
