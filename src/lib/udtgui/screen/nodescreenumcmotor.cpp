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

#include <QLayout>

#include "canopen/widgetDebug/widgetdebug.h"

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

    _widgetDebug = new WidgetDebug();

    setLayout(layout);
}

QString NodeScreenUmcMotor::title() const
{
    return QString(tr("Axis %1").arg(_axis + 1));
}

void NodeScreenUmcMotor::setNodeInternal(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    if ((node->profileNumber() != 0x192) || node->profiles().isEmpty())
    {
        return;
    }

    _axis = axis;

    _widgetDebug->setNode(node, axis);
    _tabWidget->addTab(_widgetDebug, " " + _widgetDebug->title() + " ");

    _pidVelocityWidget->setNode(node, axis);
    _pidVelocityWidget->setMode(PidWidget::MODE_PID_VELOCITY);
    _tabWidget->addTab(_pidVelocityWidget, " " + _pidVelocityWidget->title() + " ");

    _pidTorqueWidget->setNode(node, axis);
    _pidTorqueWidget->setMode(PidWidget::MODE_PID_TORQUE);
    _tabWidget->addTab(_pidTorqueWidget, " " + _pidTorqueWidget->title() + " ");

    _pidPositionWidget->setNode(node, axis);
    _pidPositionWidget->setMode(PidWidget::MODE_PID_POSITION);
    _tabWidget->addTab(_pidPositionWidget, " " + _pidPositionWidget->title() + " ");
}
