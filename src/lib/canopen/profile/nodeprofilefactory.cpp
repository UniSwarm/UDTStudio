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

NodeProfileFactory::NodeProfileFactory(Node *node): _node(node)
{
    if (_node->profileNumber() == 0x192)
    {
        axisDiscover();
        _axisCount = 0;
        _controlWordListObjectId.clear();
    }
}

void NodeProfileFactory::axisDiscover()
{
    setNodeInterrest(_node);

    for (uint8_t i = 0; i < 8; i++)
    {
        NodeObjectId cwObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, i);
        cwObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        registerObjId({cwObjectId});
        _node->readObject(cwObjectId);
    }
}

void NodeProfileFactory::managementAxisDiscover(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!(flags & SDO::Error))
    {
        for (int i = 0; i < _controlWordListObjectId.size(); i++)
        {
            if (_controlWordListObjectId[i].index() == objId.index())
            {
                return;
            }
        }
        _controlWordListObjectId.append(objId);
    }

    _axisCount++;
    if (_axisCount == 8)
    {
        _axisCount = static_cast<uint8_t>(_controlWordListObjectId.size());
        for (uint8_t i = 0; i < _axisCount; i++)
        {
            NodeProfile402 *axisNode = new NodeProfile402(_node, i);
            _node->addProfile(axisNode);
        }
        _axisCount = 0;
        _controlWordListObjectId.clear();
        unRegisterFullOd();
    }
}

void NodeProfileFactory::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (_node->profileNumber() == 0x192)
    {
        managementAxisDiscover(objId, flags);
    }
}
