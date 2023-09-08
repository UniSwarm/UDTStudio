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

#include "motorwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexbar.h"
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include <datalogger/datalogger.h>
#include <services/rpdo.h>
#include <services/tpdo.h>

#include <indexdb402.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QStandardItemModel>
#include <QWidget>

MotorWidget::MotorWidget(QWidget *parent)
    : NodeWidget(parent)
{
    _axis = 0;
    _nodeProfile402 = nullptr;
    _created = false;
}

QString MotorWidget::title() const
{
    return tr("Motor");
}

void MotorWidget::setNode(Node *node, uint8_t axis)
{
    NodeWidget::setNode(node);
    if (node == nullptr)
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    setINode();
}

void MotorWidget::setINode()
{
    if (node() == nullptr || !_created)
    {
        return;
    }

    connect(node(), &Node::statusChanged, this, &MotorWidget::updateNodeStatus);
    if (!node()->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(node()->profiles()[_axis]);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &MotorWidget::updateState);
    }

    // motor config
    _motorTypeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_TYPE, _axis));
    _peakCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_PEAK_CURRENT, _axis));
    _burstCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_BURST_CURRENT, _axis));
    _burstDurationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_BURST_DURATION, _axis));
    _sustainedCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_SUSTAINED_CURRENT, _axis));
    _currentConstantSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_CURRENT_CONSTANT, _axis));
    _maxVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_MAX_VELOCITY, _axis));
    _velocityConstantSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_VELOCITY_CONSTANT, _axis));
    _reverseMotorPolarityCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_FLAGS, _axis));

    // motor status
    _bridgeTemp1Label->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DRIVER_TEMPERATURE, _axis, 0));
    _bridgeTemp2Label->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DRIVER_TEMPERATURE, _axis, 1));
    _bridgeCommandBar->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_COMMAND, _axis));
    _motorCurrentLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_CURRENT, _axis));
    _motorTorqueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_TORQUE, _axis));
    _motorVelocityLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_VELOCITY, _axis));
    _motorPositionLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_POSITION, _axis));

    // BLDC config
    _polePairSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_CONFIG_POLE_PAIR, _axis));
    _reverseHallPolarityCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_FLAGS, _axis));

    // BLDC status
    _hallRawValueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_STATUS_HALL_RAW, _axis));
    _hallPhaseLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_STATUS_HALL_PHASE, _axis));
    _electricalAngleLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_STATUS_ELECTRICAL_ANGLE, _axis));

    // Brake config
    _brakeModeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_MODE, _axis));
    _brakeExitationTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_EXITATION_TIME, _axis));
    _brakeExitationDutySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_EXITATION_DUTY, _axis));
    _brakeDutySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_ACTIVATED_DUTY, _axis));
    _brakeReleaseDelaySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_RELEASE_DELAY, _axis));
    _brakeReleaseToOESpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_RELEASE_TO_OE, _axis));
    _brakeClosingDelaySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_CLOSING_DELAY, _axis));
    _brakeClosingToIdleSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BRAKE_CLOSING_TO_IDLE, _axis));
    _brakeBypassCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_DIGITAL_OUTPUTS_PHYSICAL_OUTPUTS, _axis));

    NodeWidget::setNode(node());
    updateNodeStatus(node()->status());
    updateState();
}

void MotorWidget::updateNodeStatus(Node::Status status)
{
    setEnabled(status != Node::STOPPED);
}

void MotorWidget::updateState()
{
    _lockAction->blockSignals(true);
    if (_nodeProfile402->currentState() == NodeProfile402::STATE_OperationEnabled)
    {
        _motorConfigGroupBox->setEnabled(false);
        _bldcConfigGroupBox->setEnabled(false);
        _brakeConfigGroupBox->setEnabled(false);
        _lockAction->setChecked(true);
    }
    else
    {
        _motorConfigGroupBox->setEnabled(true);
        _bldcConfigGroupBox->setEnabled(true);
        _brakeConfigGroupBox->setEnabled(true);
        _lockAction->setChecked(false);
    }
    _lockAction->blockSignals(false);
    _lockAction->setEnabled((_nodeProfile402->status() == NodeProfile402::NODEPROFILE_STARTED));
}

void MotorWidget::createWidgets()
{
    QWidget *motorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motorWidget);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(0);

    _motorConfigGroupBox = createMotorConfigWidgets();
    actionLayout->addWidget(_motorConfigGroupBox);
    actionLayout->addWidget(createMotorStatusWidgets());
    _bldcConfigGroupBox = createBldcConfigWidgets();
    actionLayout->addWidget(_bldcConfigGroupBox);
    actionLayout->addWidget(createBldcStatusWidgets());
    _brakeConfigGroupBox = createBrakeConfigWidgets();
    actionLayout->addWidget(_brakeConfigGroupBox);

    QScrollArea *motorScrollArea = new QScrollArea;
    motorScrollArea->setWidget(motorWidget);
    motorScrollArea->setWidgetResizable(true);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(0);
    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2, 2, 2, 0);
    toolBarLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addItem(toolBarLayout);
    vBoxLayout->addWidget(motorScrollArea);
    setLayout(vBoxLayout);

    _created = true;
}

QToolBar *MotorWidget::createToolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Motor commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-update.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotorWidget::readAll);

    toolBar->addSeparator();

    // lock / unlock action
    _lockAction = toolBar->addAction(tr("Lock/unlock config objects"));
    _lockAction->setEnabled(false);
    _lockAction->setCheckable(true);
    QIcon iconLockUnlock;
    iconLockUnlock.addFile(":/icons/img/icons8-lock.png", QSize(), QIcon::Normal, QIcon::On);
    iconLockUnlock.addFile(":/icons/img/icons8-unlock.png", QSize(), QIcon::Normal, QIcon::Off);
    _lockAction->setIcon(iconLockUnlock);
    _lockAction->setShortcut(QKeySequence("Ctrl+L"));
    _lockAction->setStatusTip(tr("Editing of config parameters is not possible in OE mode, go to SO to unlock"));
    connect(_lockAction, &QAction::triggered, this, &MotorWidget::lockUnlockConfig);

    toolBar->addSeparator();

    // currents actions
    QAction *mapCurrentsAction = toolBar->addAction(tr("Map currents"));
    mapCurrentsAction->setShortcut(QKeySequence("Ctrl+M"));
    connect(mapCurrentsAction, &QAction::triggered, this, &MotorWidget::mapCurrents);

    QAction *monitorCurrentsAction = toolBar->addAction(tr("Logger currents"));
    monitorCurrentsAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(monitorCurrentsAction, &QAction::triggered, this, &MotorWidget::monitorCurrents);

    return toolBar;
}

QGroupBox *MotorWidget::createMotorConfigWidgets()
{
    QGroupBox *motorConfigGroupBox = new QGroupBox(tr("Motor config"));
    IndexFormLayout *configLayout = new IndexFormLayout();

    _motorTypeComboBox = new IndexComboBox();
    _motorTypeComboBox->addItem(tr("No motor"), QVariant(static_cast<uint16_t>(0x0000)));
    _motorTypeComboBox->insertSeparator(_motorTypeComboBox->count());

    _motorTypeComboBox->addItem(tr("DC brushed"));
    dynamic_cast<QStandardItemModel *>(_motorTypeComboBox->model())->item(_motorTypeComboBox->count() - 1)->setEnabled(false);
    _motorTypeComboBox->addItem(tr("DC motor"), QVariant(static_cast<uint16_t>(0x0101)));
    _motorTypeComboBox->insertSeparator(_motorTypeComboBox->count());

    _motorTypeComboBox->addItem(tr("BLDC brushless"));
    dynamic_cast<QStandardItemModel *>(_motorTypeComboBox->model())->item(_motorTypeComboBox->count() - 1)->setEnabled(false);
    _motorTypeComboBox->addItem(tr("BLDC trapezoidal with hall"), QVariant(static_cast<uint16_t>(0x0201)));
    _motorTypeComboBox->addItem(tr("BLDC sinusoidal with hall"), QVariant(static_cast<uint16_t>(0x0202)));
    _motorTypeComboBox->addItem(tr("BLDC sinusoidal with incremental encoder"), QVariant(static_cast<uint16_t>(0x0203)));
    configLayout->addRow(tr("&Motor type:"), _motorTypeComboBox);
    addIndexWidget(_motorTypeComboBox);

    configLayout->addLineSeparator();

    _peakCurrentSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("P&eak current:"), _peakCurrentSpinBox);
    addIndexWidget(_peakCurrentSpinBox);

    _burstCurrentSpinBox = new IndexSpinBox();
    addIndexWidget(_burstCurrentSpinBox);
    _burstDurationSpinBox = new IndexSpinBox();
    addIndexWidget(_burstDurationSpinBox);
    configLayout->addDualRow(tr("Burst &current:"), _burstCurrentSpinBox, _burstDurationSpinBox, tr("/"));

    _sustainedCurrentSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("&Sustained current:"), _sustainedCurrentSpinBox);
    addIndexWidget(_sustainedCurrentSpinBox);

    _currentConstantSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("C&urrent constant:"), _currentConstantSpinBox);
    addIndexWidget(_currentConstantSpinBox);

    configLayout->addLineSeparator();

    _maxVelocitySpinBox = new IndexSpinBox();
    configLayout->addRow(tr("M&ax velocity:"), _maxVelocitySpinBox);
    addIndexWidget(_maxVelocitySpinBox);

    _velocityConstantSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("Ve&locity constant:"), _velocityConstantSpinBox);
    addIndexWidget(_velocityConstantSpinBox);

    configLayout->addLineSeparator();

    _reverseMotorPolarityCheckBox = new IndexCheckBox();
    _reverseMotorPolarityCheckBox->setBitMask(1);
    configLayout->addRow(tr("&Invert motor direction:"), _reverseMotorPolarityCheckBox);
    addIndexWidget(_reverseMotorPolarityCheckBox);

    motorConfigGroupBox->setLayout(configLayout);
    return motorConfigGroupBox;
}

QGroupBox *MotorWidget::createMotorStatusWidgets()
{
    QGroupBox *statusGroupBox = new QGroupBox(tr("Motor status"));
    IndexFormLayout *statusLayout = new IndexFormLayout();

    _bridgeCommandBar = new IndexBar();
    statusLayout->addRow(tr("Command duty cycle:"), _bridgeCommandBar);
    _bridgeCommandBar->setUnit("%");
    _bridgeCommandBar->setScale(100.0 / 32768.0);
    _bridgeCommandBar->setRange(-100, 100);
    adddynamicIndexWidget(_bridgeCommandBar);

    _motorCurrentLabel = new IndexLabel();
    statusLayout->addRow(tr("Current:"), _motorCurrentLabel);
    adddynamicIndexWidget(_motorCurrentLabel);

    _motorTorqueLabel = new IndexLabel();
    // _motorTorqueLabel->setUnit(" Nm");
    statusLayout->addRow(tr("Torque:"), _motorTorqueLabel);
    adddynamicIndexWidget(_motorTorqueLabel);

    _motorVelocityLabel = new IndexLabel();
    // _motorVelocityLabel->setUnit(" rpm");
    statusLayout->addRow(tr("Velocity:"), _motorVelocityLabel);
    adddynamicIndexWidget(_motorVelocityLabel);

    _motorPositionLabel = new IndexLabel();
    // _motorPositionLabel->setUnit(" tr");
    statusLayout->addRow(tr("Position:"), _motorPositionLabel);
    adddynamicIndexWidget(_motorPositionLabel);

    _bridgeTemp1Label = new IndexLabel();
    statusLayout->addRow(tr("Temperature bridge 1:"), _bridgeTemp1Label);
    adddynamicIndexWidget(_bridgeTemp1Label);

    _bridgeTemp2Label = new IndexLabel();
    statusLayout->addRow(tr("Temperature bridge 2:"), _bridgeTemp2Label);
    adddynamicIndexWidget(_bridgeTemp2Label);

    statusGroupBox->setLayout(statusLayout);
    return statusGroupBox;
}

QGroupBox *MotorWidget::createBldcConfigWidgets()
{
    QGroupBox *configGroupBox = new QGroupBox(tr("BLDC config"));
    IndexFormLayout *configLayout = new IndexFormLayout();

    _polePairSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("P&ole pair:"), _polePairSpinBox);
    addIndexWidget(_polePairSpinBox);

    _reverseHallPolarityCheckBox = new IndexCheckBox();
    _reverseHallPolarityCheckBox->setBitMask(2);
    configLayout->addRow(tr("Invert &hall polarity:"), _reverseHallPolarityCheckBox);
    addIndexWidget(_reverseHallPolarityCheckBox);

    configGroupBox->setLayout(configLayout);
    return configGroupBox;
}

QGroupBox *MotorWidget::createBldcStatusWidgets()
{
    QGroupBox *statusGroupBox = new QGroupBox(tr("BLDC status"));
    IndexFormLayout *statusLayout = new IndexFormLayout();

    _hallRawValueLabel = new IndexLabel();
    statusLayout->addRow(tr("Hall raw:"), _hallRawValueLabel);
    adddynamicIndexWidget(_hallRawValueLabel);

    _hallPhaseLabel = new IndexLabel();
    statusLayout->addRow(tr("Hall phase:"), _hallPhaseLabel);
    adddynamicIndexWidget(_hallPhaseLabel);

    _electricalAngleLabel = new IndexLabel();
    statusLayout->addRow(tr("Electrical angle:"), _electricalAngleLabel);
    adddynamicIndexWidget(_electricalAngleLabel);

    statusGroupBox->setLayout(statusLayout);
    return statusGroupBox;
}

QGroupBox *MotorWidget::createBrakeConfigWidgets()
{
    QGroupBox *configGroupBox = new QGroupBox(tr("Brake config"));
    IndexFormLayout *configLayout = new IndexFormLayout();

    _brakeModeComboBox = new IndexComboBox();
    _brakeModeComboBox->addItem(tr("Disabled"), QVariant(static_cast<uint8_t>(0x00)));
    _brakeModeComboBox->addItem(tr("Open drain, NC"), QVariant(static_cast<uint8_t>(0x02)));
    _brakeModeComboBox->addItem(tr("Open drain, NO"), QVariant(static_cast<uint8_t>(0x03)));
    _brakeModeComboBox->addItem(tr("Open source, NC"), QVariant(static_cast<uint8_t>(0x04)));
    _brakeModeComboBox->addItem(tr("Open source, NO"), QVariant(static_cast<uint8_t>(0x05)));
    _brakeModeComboBox->addItem(tr("Push-pull, NC"), QVariant(static_cast<uint8_t>(0x06)));
    _brakeModeComboBox->addItem(tr("Push-pull, NO"), QVariant(static_cast<uint8_t>(0x07)));
    configLayout->addRow(tr("&Brake mode:"), _brakeModeComboBox);
    addIndexWidget(_brakeModeComboBox);

    _brakeExitationDutySpinBox = new IndexSpinBox();
    addIndexWidget(_brakeExitationDutySpinBox);
    _brakeExitationTimeSpinBox = new IndexSpinBox();
    addIndexWidget(_brakeExitationTimeSpinBox);
    configLayout->addDualRow(tr("&Excitation:"), _brakeExitationDutySpinBox, _brakeExitationTimeSpinBox, tr("/"));

    _brakeDutySpinBox = new IndexSpinBox();
    configLayout->addRow(tr("Activated duty cycle:"), _brakeDutySpinBox);
    addIndexWidget(_brakeDutySpinBox);

    _brakeReleaseDelaySpinBox = new IndexSpinBox();
    addIndexWidget(_brakeReleaseDelaySpinBox);
    _brakeReleaseToOESpinBox = new IndexSpinBox();
    addIndexWidget(_brakeReleaseToOESpinBox);
    configLayout->addDualRow(tr("&Release delays:"), _brakeReleaseDelaySpinBox, _brakeReleaseToOESpinBox, tr("-"));

    _brakeClosingDelaySpinBox = new IndexSpinBox();
    addIndexWidget(_brakeClosingDelaySpinBox);
    _brakeClosingToIdleSpinBox = new IndexSpinBox();
    addIndexWidget(_brakeClosingToIdleSpinBox);
    configLayout->addDualRow(tr("&Closing delays:"), _brakeClosingDelaySpinBox, _brakeClosingToIdleSpinBox, tr("-"));

    _brakeBypassCheckBox = new IndexCheckBox();
    _brakeBypassCheckBox->setBitMask(1);
    configLayout->addRow(tr("&Brake override:"), _brakeBypassCheckBox);
    addIndexWidget(_brakeBypassCheckBox);

    configGroupBox->setLayout(configLayout);
    return configGroupBox;
}

void MotorWidget::lockUnlockConfig()
{
    if (_lockAction->isChecked())
    {
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
    }
    else
    {
        _nodeProfile402->goToState(NodeProfile402::STATE_SwitchedOn);
    }
    _lockAction->blockSignals(true);
    _lockAction->setChecked(!_lockAction->isChecked());  // Keep action in the previous state
    _lockAction->blockSignals(false);
}

void MotorWidget::mapCurrents()
{
    QList<NodeObjectId> currentsHLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsHLObjectsList.append(NodeObjectId(node()->busId(), node()->nodeId(), 0x2802, i));
    }
    QList<NodeObjectId> currentsLLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsLLObjectsList.append(NodeObjectId(node()->busId(), node()->nodeId(), 0x2803, i));
    }

    _nodeProfile402->node()->tpdos().at(2)->writeMapping(currentsHLObjectsList);
    _nodeProfile402->node()->tpdos().at(3)->writeMapping(currentsLLObjectsList);
}

void MotorWidget::monitorCurrents()
{
    QList<NodeObjectId> currentsHLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsHLObjectsList.append(NodeObjectId(node()->busId(), node()->nodeId(), 0x2802, i));
    }
    QList<NodeObjectId> currentsLLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsLLObjectsList.append(NodeObjectId(node()->busId(), node()->nodeId(), 0x2803, i));
    }

    DataLogger *dataLoggerHL = new DataLogger();
    DataLoggerWidget *dataLoggerWidgetHL = new DataLoggerWidget(dataLoggerHL);
    dataLoggerWidgetHL->setTitle(tr("HL currents node %1").arg(_nodeProfile402->nodeId()));
    dataLoggerHL->addData(currentsHLObjectsList);
    dataLoggerWidgetHL->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidgetHL, &QObject::destroyed, dataLoggerHL, &DataLogger::deleteLater);
    dataLoggerWidgetHL->show();
    dataLoggerWidgetHL->raise();
    dataLoggerWidgetHL->activateWindow();

    DataLogger *dataLoggerLL = new DataLogger();
    DataLoggerWidget *dataLoggerWidgetLL = new DataLoggerWidget(dataLoggerLL);
    dataLoggerWidgetLL->setTitle(tr("LL currents node %1").arg(_nodeProfile402->nodeId()));
    dataLoggerLL->addData(currentsLLObjectsList);
    dataLoggerWidgetLL->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidgetLL, &QObject::destroyed, dataLoggerLL, &DataLogger::deleteLater);
    dataLoggerWidgetLL->show();
    dataLoggerWidgetLL->raise();
    dataLoggerWidgetLL->activateWindow();
}

void MotorWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if (!_created)
    {
        createWidgets();
        setINode();
    }

    NodeWidget::showEvent(event);
}
