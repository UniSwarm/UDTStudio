/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include "nodeprofilefactory.h"

#include "indexdb402.h"
#include "node.h"
#include "p402/nodeprofile402.h"

NodeProfileFactory::NodeProfileFactory()
{
}

void NodeProfileFactory::profileFactory(Node *node)
{
    if (node->profileNumber() == 0x192)
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            NodeObjectId cwObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, i);
            if (node->nodeOd()->indexExist(cwObjectId.index()))
            {
                NodeProfile402 *profile402 = new NodeProfile402(node, i);
                node->addProfile(profile402);
            }
        }
    }
}
