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

#include "nodescreenumcmotor.h"

#include "canopen/compositeWidget/pidwidget.h"
#include "profile/p402/nodeprofile402.h"

#include <QLayout>

NodeScreenUmcMotor::NodeScreenUmcMotor(QWidget *parent) : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenUmcMotor::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    _tabWidget = new QTabWidget();
    layout->addWidget(_tabWidget);

    _pidVelocityWidget = new PidWidget();
    _pidTorqueWidget = new PidWidget();
    _pidPositionWidget = new PidWidget();

    setLayout(layout);
}

QString NodeScreenUmcMotor::title() const
{
    return QString(tr("UMC Motor"));
}

void NodeScreenUmcMotor::setNodeInternal(Node *node)
{
    if (!_node)
    {
        return;
    }

    _pidVelocityWidget->setNode(node);
    _pidVelocityWidget->setMode(NodeProfile402::VL);
    _tabWidget->addTab(_pidVelocityWidget, " " + _pidVelocityWidget->title() + " ");
    _pidTorqueWidget->setNode(node);
    _pidTorqueWidget->setMode(NodeProfile402::TQ);
    _tabWidget->addTab(_pidTorqueWidget, " " + _pidTorqueWidget->title() + " ");
    _pidPositionWidget->setNode(node);
    _pidPositionWidget->setMode(NodeProfile402::PP);
    _tabWidget->addTab(_pidPositionWidget, " " + _pidPositionWidget->title() + " ");
}
