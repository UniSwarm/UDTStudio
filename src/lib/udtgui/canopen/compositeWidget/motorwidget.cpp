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

#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "indexdb402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QWidget>

MotorWidget::MotorWidget(QWidget *parent)
    : QWidget(parent)
{
    createWidgets();
}

Node *MotorWidget::node() const
{
    return _node;
}

QString MotorWidget::title() const
{
    return tr("Motor");
}

void MotorWidget::setNode(Node *node, uint8_t axis)
{
    _node = node;
    if (!_node)
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    connect(_node, &Node::statusChanged, this, &MotorWidget::statusNodeChanged);
    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &MotorWidget::stateChanged);
    }

    _motorTypeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_TYPE, _axis));
    _peakCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_PEAK_CURRENT, _axis));
    _polePairSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_POLE_PAIR, _axis));
    _maxVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_MAX_VELOCITY, _axis));
    _velocityConstantSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_VELOCITY_CONSTANT, _axis));
    _currentConstantSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_CURRENT_CONSTANT, _axis));
    _brakeBypassCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_DIGITAL_OUTPUTS_PHYSICAL_OUTPUTS, _axis));
    _reverseMotorPolarityCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_CONFIG_BIT, _axis));
    _reverseHallPolarityCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_CONFIG_BIT, _axis));
    _motorTypeComboBox->setNode(node);
    _peakCurrentSpinBox->setNode(node);
    _polePairSpinBox->setNode(node);
    _maxVelocitySpinBox->setNode(node);
    _velocityConstantSpinBox->setNode(node);
    _currentConstantSpinBox->setNode(node);
    _brakeBypassCheckBox->setNode(node);
    _reverseMotorPolarityCheckBox->setNode(node);
    _reverseHallPolarityCheckBox->setNode(node);

    _hallRawValueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_HALL_RAW_VALUE, _axis));
    _hallPhaseLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_HALL_PHASE, _axis));
    _bridgePweredPhaseLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_POWERED_PHASE, _axis));
    _bridgeCommandLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_COMMAND, _axis));
    _bridgeTemp1Label->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DRIVER_TEMPERATURE, _axis, 0));
    _bridgeTemp2Label->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DRIVER_TEMPERATURE, _axis, 1));
    _hallRawValueLabel->setNode(node);
    _hallPhaseLabel->setNode(node);
    _bridgePweredPhaseLabel->setNode(node);
    _bridgeCommandLabel->setNode(node);
    _bridgeTemp1Label->setNode(node);
    _bridgeTemp2Label->setNode(node);
}

void MotorWidget::statusNodeChanged(Node::Status status)
{
    setEnabled(status != Node::STOPPED);
}

void MotorWidget::stateChanged()
{
    if (_nodeProfile402->currentState() == NodeProfile402::STATE_OperationEnabled)
    {
        _motorConfigGroupBox->setEnabled(false);
        _enableButton->setEnabled(true);
        _informationLabel->setText(tr("Not available in \"Operation Enabled\""));
    }
    else
    {
        _motorConfigGroupBox->setEnabled(true);
        _enableButton->setEnabled(false);
        _informationLabel->setText("");
    }
}

void MotorWidget::readAllObject()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void MotorWidget::createWidgets()
{
    QWidget *motionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motionSensorWidget);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(0);

    actionLayout->addWidget(informationWidgets());
    _motorConfigGroupBox = motorConfigWidgets();
    actionLayout->addWidget(_motorConfigGroupBox);
    actionLayout->addWidget(motorStatusWidgets());

    QScrollArea *motionSensorScrollArea = new QScrollArea;
    motionSensorScrollArea->setWidget(motionSensorWidget);
    motionSensorScrollArea->setWidgetResizable(true);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2, 2);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(toolBarWidgets());
    vBoxLayout->addWidget(motionSensorScrollArea);
    setLayout(vBoxLayout);
}

QToolBar *MotorWidget::toolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Data logger commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotorWidget::readAllObject);
    return toolBar;
}

QGroupBox *MotorWidget::informationWidgets()
{
    QGroupBox *informationGroupBox = new QGroupBox(tr("Information"));
    informationGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QVBoxLayout *informationLayout = new QVBoxLayout();

    _informationLabel = new QLabel();
    _informationLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    informationLayout->addWidget(_informationLabel);

    _enableButton = new QPushButton(tr("Unlock config, Go to \"Switched On\""));
    connect(_enableButton, &QPushButton::clicked, this, &MotorWidget::goEnableButton);
    informationLayout->addWidget(_enableButton);

    informationGroupBox->setLayout(informationLayout);
    return informationGroupBox;
}

QGroupBox *MotorWidget::motorConfigWidgets()
{
    QGroupBox *_motorConfigGroupBox = new QGroupBox(tr("Motor config"));
    QFormLayout *configLayout = new QFormLayout();

    _motorTypeComboBox = new IndexComboBox();
    _motorTypeComboBox->addItem(tr("NO motor"), QVariant(static_cast<uint16_t>(0)));
    _motorTypeComboBox->insertSeparator(_motorTypeComboBox->count());
    _motorTypeComboBox->addItem(tr("DC motor"), QVariant(static_cast<uint16_t>(0x0101)));
    _motorTypeComboBox->insertSeparator(_motorTypeComboBox->count());
    _motorTypeComboBox->addItem(tr("BLDC trapezoidal"), QVariant(static_cast<uint16_t>(0x0201)));
    _motorTypeComboBox->addItem(tr("BLDC sinusoidal with hall"), QVariant(static_cast<uint16_t>(0x0202)));
    _motorTypeComboBox->addItem(tr("BLDC sinusoidal with incremental encoder"), QVariant(static_cast<uint16_t>(0x0203)));
    configLayout->addRow(tr("&Motor type:"), _motorTypeComboBox);
    _indexWidgets.append(_motorTypeComboBox);

    _peakCurrentSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("P&eak current:"), _peakCurrentSpinBox);
    _indexWidgets.append(_peakCurrentSpinBox);

    _polePairSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("P&ole pair:"), _polePairSpinBox);
    _indexWidgets.append(_polePairSpinBox);

    _maxVelocitySpinBox = new IndexSpinBox();
    configLayout->addRow(tr("M&ax velocity:"), _maxVelocitySpinBox);
    _indexWidgets.append(_maxVelocitySpinBox);

    _velocityConstantSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("Ve&locity constant:"), _velocityConstantSpinBox);
    _indexWidgets.append(_velocityConstantSpinBox);

    _currentConstantSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("C&urrent constant:"), _currentConstantSpinBox);
    _indexWidgets.append(_currentConstantSpinBox);

    _brakeBypassCheckBox = new IndexCheckBox();
    _brakeBypassCheckBox->setBitMask(1);
    configLayout->addRow(tr("&Break:"), _brakeBypassCheckBox);
    _indexWidgets.append(_brakeBypassCheckBox);

    _reverseMotorPolarityCheckBox = new IndexCheckBox();
    _reverseMotorPolarityCheckBox->setBitMask(1);
    configLayout->addRow(tr("&Reverse motor polarity:"), _reverseMotorPolarityCheckBox);
    _indexWidgets.append(_reverseMotorPolarityCheckBox);

    _reverseHallPolarityCheckBox = new IndexCheckBox();
    _reverseHallPolarityCheckBox->setBitMask(2);
    configLayout->addRow(tr("&Reverse hall polarity:"), _reverseHallPolarityCheckBox);
    _indexWidgets.append(_reverseHallPolarityCheckBox);

    _motorConfigGroupBox->setLayout(configLayout);

    return _motorConfigGroupBox;
}

QGroupBox *MotorWidget::motorStatusWidgets()
{
    QGroupBox *statusGroupBox = new QGroupBox(tr("Motor status"));
    QFormLayout *statusLayout = new QFormLayout();

    _hallRawValueLabel = new IndexLabel();
    statusLayout->addRow(tr("&Hall raw:"), _hallRawValueLabel);
    _indexWidgets.append(_hallRawValueLabel);

    _hallPhaseLabel = new IndexLabel();
    statusLayout->addRow(tr("&Hall phase:"), _hallPhaseLabel);
    _indexWidgets.append(_hallPhaseLabel);

    _bridgePweredPhaseLabel = new IndexLabel();
    statusLayout->addRow(tr("&Powered phase:"), _bridgePweredPhaseLabel);
    _indexWidgets.append(_bridgePweredPhaseLabel);

    _bridgeCommandLabel = new IndexLabel();
    statusLayout->addRow(tr("&Command:"), _bridgeCommandLabel);
    _indexWidgets.append(_bridgeCommandLabel);

    _bridgeTemp1Label = new IndexLabel();
    statusLayout->addRow(tr("&Temperature bridge 1:"), _bridgeTemp1Label);
    _bridgeTemp1Label->setScale(1.0 / 10.0);
    _bridgeTemp1Label->setUnit(" °C");
    _indexWidgets.append(_bridgeTemp1Label);

    _bridgeTemp2Label = new IndexLabel();
    statusLayout->addRow(tr("&Temperature bridge 2:"), _bridgeTemp2Label);
    _bridgeTemp2Label->setScale(1.0 / 10.0);
    _bridgeTemp2Label->setUnit(" °C");
    _indexWidgets.append(_bridgeTemp2Label);

    statusGroupBox->setLayout(statusLayout);
    return statusGroupBox;
}

void MotorWidget::goEnableButton()
{
    _nodeProfile402->goToState(NodeProfile402::STATE_SwitchedOn);
}
