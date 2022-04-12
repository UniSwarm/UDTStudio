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

#ifndef NODESCREENUMCMOTOR_H
#define NODESCREENUMCMOTOR_H

#include "udtgui_global.h"

#include "nodescreen.h"

#include <QTabWidget>

class PidWidget;
class NodeProfile402;
class P402Widget;
class MotionSensorWidget;
class MotorWidget;

class UDTGUI_EXPORT NodeScreenUmcMotor : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenUmcMotor(QWidget *parent = nullptr);

protected:
    uint8_t _axis;
    void createWidgets();
    QTabWidget *_tabWidget;

    MotorWidget *_motorConfigWidget;

    PidWidget *_pidTorqueWidget;
    MotionSensorWidget *_motionSensorTorqueWidget;

    PidWidget *_pidVelocityWidget;
    MotionSensorWidget *_motionSensorVelocityWidget;

    PidWidget *_pidPositionWidget;
    MotionSensorWidget *_motionSensorPositionWidget;

    P402Widget *_p402Widget;

    // NodeScreen interface
public:
    QString title() const override;
    QIcon icon() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;
};

#endif  // NODESCREENUMCMOTOR_H
