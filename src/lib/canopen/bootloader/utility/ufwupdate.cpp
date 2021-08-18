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

#include "ufwupdate.h"

#include "../utility/phantomremover.h"
#include "node.h"

#include <QFile>

UfwUpdate::UfwUpdate(Node *node, UfwParser *ufw)
    : _node(node)
    , _ufw(ufw)
{
}

void UfwUpdate::setUfw(UfwParser *ufw)
{
    _ufw = ufw;
}

int UfwUpdate::update()
{
    if (!_ufw)
    {
        return -1;
    }

    uint32_t start2 = 0xD000;

    QByteArray prog = _ufw->prog().mid(0xD000, 0x1000);
    PhantomRemover phantomRemover;
    QByteArray prog2 = phantomRemover.remove(prog);
    prog2.prepend(reinterpret_cast<char *>(&start2), sizeof(start2));
    _node->updateFirmware(prog2);

    return 0;
}
