/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#include "../../udtgui_global.h"

#include "utils/nodewidget.h"

#include "profile/p402/nodeprofile402.h"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QToolBar>

class Node;
class AbstractIndexWidget;
class IndexSpinBox;
class IndexLabel;
class IndexComboBox;
class IndexCheckBox;
class IndexBar;

class UDTGUI_EXPORT MotorWidget : public NodeWidget
{
    Q_OBJECT
public:
    explicit MotorWidget(QWidget *parent = nullptr);

    QString title() const;

public slots:
    void setNode(Node *node, uint8_t axis = 0);

protected:
    uint8_t _axis;
    NodeProfile402 *_nodeProfile402;

    void setINode();

    // Creation widgets
    void createWidgets();
    bool _created;

    // Toolbar
    QToolBar *createToolBarWidgets();
    QAction *_lockAction;

    // Motor Config
    QGroupBox *createMotorConfigWidgets();
    QGroupBox *_motorConfigGroupBox;
    IndexComboBox *_motorTypeComboBox;
    IndexSpinBox *_peakCurrentSpinBox;
    IndexSpinBox *_burstCurrentSpinBox;
    IndexSpinBox *_burstDurationSpinBox;
    IndexSpinBox *_sustainedCurrentSpinBox;

    IndexSpinBox *_maxVelocitySpinBox;
    IndexSpinBox *_velocityConstantSpinBox;
    IndexSpinBox *_currentConstantSpinBox;
    IndexCheckBox *_reverseMotorPolarityCheckBox;
    IndexCheckBox *_reverseHallPolarityCheckBox;

    // Motor Status
    QGroupBox *createMotorStatusWidgets();
    IndexBar *_bridgeCommandBar;
    IndexLabel *_motorCurrentLabel;
    IndexLabel *_motorTorqueLabel;
    IndexLabel *_motorVelocityLabel;
    IndexLabel *_motorPositionLabel;
    IndexLabel *_bridgeTemp1Label;
    IndexLabel *_bridgeTemp2Label;

    // BLDC Config
    QGroupBox *createBldcConfigWidgets();
    QGroupBox *_bldcConfigGroupBox;
    IndexSpinBox *_polePairSpinBox;

    // BLDC Status
    QGroupBox *createBldcStatusWidgets();
    IndexLabel *_hallRawValueLabel;
    IndexLabel *_hallPhaseLabel;
    IndexLabel *_electricalAngleLabel;

    // Brake Config
    QGroupBox *createBrakeConfigWidgets();
    QGroupBox *_brakeConfigGroupBox;
    IndexComboBox *_brakeModeComboBox;
    IndexSpinBox *_brakeExitationTimeSpinBox;
    IndexSpinBox *_brakeExitationDutySpinBox;
    IndexSpinBox *_brakeDutySpinBox;
    IndexSpinBox *_brakeReleaseDelaySpinBox;
    IndexSpinBox *_brakeReleaseToOESpinBox;
    IndexSpinBox *_brakeClosingDelaySpinBox;
    IndexSpinBox *_brakeClosingToIdleSpinBox;
    IndexCheckBox *_brakeBypassCheckBox;

    void updateNodeStatus(Node::Status status);
    void updateState();
    void lockUnlockConfig();

    void mapCurrents();
    void monitorCurrents();

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif  // MOTORCONFIG_H
