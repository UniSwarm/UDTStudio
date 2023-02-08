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

#include "nodescreenumcmotor.h"

#include "canopen/compositeWidget/motionsensorwidget.h"
#include "canopen/compositeWidget/pidwidget.h"
#include <QLayout>

#include "canopen/profileWidget/p402/p402widget.h"
#include <canopen/compositeWidget/motorwidget.h>

NodeScreenUmcMotor::NodeScreenUmcMotor(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenUmcMotor::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _tabWidget = new QTabWidget();
    _tabWidget->setStyleSheet("\
      QTabWidget > QTabBar::tab:top {\
          color: palette(foreground);\
      }\
      QTabWidget > QTabBar::tab:top:!selected\
      {\
          color: palette(foreground);\
      }\
    ");
    layout->addWidget(_tabWidget);

    _motorConfigWidget = new MotorWidget();

    _pidVelocityWidget = new PidWidget();
    _pidTorqueWidget = new PidWidget();
    _pidPositionWidget = new PidWidget();

    _motionSensorVelocityWidget = new MotionSensorWidget();
    _motionSensorPositionWidget = new MotionSensorWidget();
    _motionSensorTorqueWidget = new MotionSensorWidget();

    _p402Widget = new P402Widget();

    setLayout(layout);
}

QString NodeScreenUmcMotor::title() const
{
    return QString(tr("Axis %1").arg(_axis + 1));
}

QIcon NodeScreenUmcMotor::icon() const
{
    if (_node->vendorId() == 0x04A2)  // UniSwarm
    {
        return QIcon(":/uBoards/umc.png");
    }
    return QIcon();
}

void NodeScreenUmcMotor::setNodeInternal(Node *node, uint8_t axis)
{
    if (node == nullptr)
    {
        return;
    }
    if ((node->profileNumber() != 402) || node->profiles().isEmpty())
    {
        return;
    }
    _axis = axis;
    NodeProfile402 *nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);

    _p402Widget->setProfile(nodeProfile402);
    _tabWidget->addTab(_p402Widget, " " + _p402Widget->title() + " ");

    if (node->vendorId() == 0x04A2)  // UniSwarm
    {
        _motorConfigWidget->setNode(node, axis);
        _tabWidget->addTab(_motorConfigWidget, " " + _motorConfigWidget->title() + " ");

        _motionSensorTorqueWidget->setMode(MotionSensorWidget::MODE_SENSOR_TORQUE);
        _motionSensorTorqueWidget->setNode(node, axis);
        _tabWidget->addTab(_motionSensorTorqueWidget, " " + _motionSensorTorqueWidget->title() + " ");
        _tabWidget->tabBar()->setTabTextColor(_tabWidget->count() - 1, QColor::fromHsv(30, 255, 255));
        _pidTorqueWidget->setMode(PidWidget::MODE_PID_TORQUE);
        _pidTorqueWidget->setNode(node, axis);
        _tabWidget->addTab(_pidTorqueWidget, " " + _pidTorqueWidget->title() + " ");
        _tabWidget->tabBar()->setTabTextColor(_tabWidget->count() - 1, QColor::fromHsv(30, 255, 255));

        _motionSensorVelocityWidget->setMode(MotionSensorWidget::MODE_SENSOR_VELOCITY);
        _motionSensorVelocityWidget->setNode(node, axis);
        _tabWidget->addTab(_motionSensorVelocityWidget, " " + _motionSensorVelocityWidget->title() + " ");
        _tabWidget->tabBar()->setTabTextColor(_tabWidget->count() - 1, QColor::fromHsv(60, 255, 255));
        _pidVelocityWidget->setMode(PidWidget::MODE_PID_VELOCITY);
        _pidVelocityWidget->setNode(node, axis);
        _tabWidget->addTab(_pidVelocityWidget, " " + _pidVelocityWidget->title() + " ");
        _tabWidget->tabBar()->setTabTextColor(_tabWidget->count() - 1, QColor::fromHsv(60, 255, 255));

        _motionSensorPositionWidget->setMode(MotionSensorWidget::MODE_SENSOR_POSITION);
        _motionSensorPositionWidget->setNode(node, axis);
        _tabWidget->addTab(_motionSensorPositionWidget, " " + _motionSensorPositionWidget->title() + " ");
        _tabWidget->tabBar()->setTabTextColor(_tabWidget->count() - 1, QColor::fromHsv(210, 100, 255));
        _pidPositionWidget->setMode(PidWidget::MODE_PID_POSITION);
        _pidPositionWidget->setNode(node, axis);
        _tabWidget->addTab(_pidPositionWidget, " " + _pidPositionWidget->title() + " ");
        _tabWidget->tabBar()->setTabTextColor(_tabWidget->count() - 1, QColor::fromHsv(210, 100, 255));
    }
}
