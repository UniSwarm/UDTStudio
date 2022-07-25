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

#include "p402modewidget.h"

P402ModeWidget::P402ModeWidget(QWidget *parent)
    : QWidget(parent)
{
    _nodeProfile402 = nullptr;
}

NodeProfile402 *P402ModeWidget::nodeProfile402() const
{
    return _nodeProfile402;
}

Node *P402ModeWidget::node() const
{
    if (_nodeProfile402 == nullptr)
    {
        return nullptr;
    }
    return _nodeProfile402->node();
}

uint P402ModeWidget::axis() const
{
    if (_nodeProfile402 == nullptr)
    {
        return 0;
    }
    return _nodeProfile402->axis();
}

void P402ModeWidget::readRealTimeObjects()
{
    if (_nodeProfile402 != nullptr)
    {
        _nodeProfile402->readRealTimeObjects();
    }
}

void P402ModeWidget::readAllObjects()
{
    if (_nodeProfile402 != nullptr)
    {
        _nodeProfile402->readAllObjects();
    }
}

void P402ModeWidget::reset()
{
}

void P402ModeWidget::stop()
{
}

void P402ModeWidget::setProfile(NodeProfile402 *nodeProfile402)
{
    _nodeProfile402 = nodeProfile402;
    if (nodeProfile402 == nullptr)
    {
        setNodeInterrest(nullptr);
        setIProfile(nullptr);
        return;
    }

    setNodeInterrest(nodeProfile402->node());
    setIProfile(nodeProfile402);
}
