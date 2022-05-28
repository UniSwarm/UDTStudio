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

#include "udtguimanager.h"

#include "canopen/nodeod/nodeoditemmodel.h"
#include "qdebug.h"

UdtGuiManager *UdtGuiManager::_instance = nullptr;

UdtGuiManager::UdtGuiManager()
{
}

NodeOdItemModel *UdtGuiManager::nodeOdItemModel(Node *node)
{
    if (node == nullptr)
    {
        return nullptr;
    }

    auto it = UdtGuiManager::instance()->_nodeOdItemModels.constFind(node);
    if (it != UdtGuiManager::instance()->_nodeOdItemModels.constEnd())
    {
        return it.value();
    }

    NodeOdItemModel *model = new NodeOdItemModel();
    model->setNode(node);
    UdtGuiManager::instance()->_nodeOdItemModels.insert(node, model);
    return model;
}

UdtGuiManager *UdtGuiManager::instance()
{
    if (UdtGuiManager::_instance == nullptr)
    {
        UdtGuiManager::_instance = new UdtGuiManager();
    }
    return UdtGuiManager::_instance;
}
