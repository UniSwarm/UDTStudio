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
    : QWidget(parent)
{
    _node = nullptr;
    _axis = 0;
    _nodeProfile402 = nullptr;

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

    // motor config
    _motorTypeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_TYPE, _axis));
    _peakCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_PEAK_CURRENT, _axis));
    _burstCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_BURST_CURRENT, _axis));
    _burstDurationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_BURST_DURATION, _axis));
    _sustainedCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_SUSTAINED_CURRENT, _axis));
    _currentConstantSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_CURRENT_CONSTANT, _axis));
    _maxVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_MAX_VELOCITY, _axis));
    _velocityConstantSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_VELOCITY_CONSTANT, _axis));
    _brakeBypassCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_DIGITAL_OUTPUTS_PHYSICAL_OUTPUTS, _axis));
    _reverseMotorPolarityCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_FLAGS, _axis));
    _motorTypeComboBox->setNode(node);
    _peakCurrentSpinBox->setNode(node);
    _burstCurrentSpinBox->setNode(node);
    _burstDurationSpinBox->setNode(node);
    _sustainedCurrentSpinBox->setNode(node);
    _currentConstantSpinBox->setNode(node);
    _maxVelocitySpinBox->setNode(node);
    _velocityConstantSpinBox->setNode(node);
    _brakeBypassCheckBox->setNode(node);
    _reverseMotorPolarityCheckBox->setNode(node);

    // motor status
    _bridgeTemp1Label->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DRIVER_TEMPERATURE, _axis, 0));
    _bridgeTemp2Label->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DRIVER_TEMPERATURE, _axis, 1));
    _bridgeTemp1Label->setNode(node);
    _bridgeTemp2Label->setNode(node);
    _bridgeCommandBar->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_COMMAND, _axis));
    _motorCurrentLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_CURRENT, _axis));
    _motorTorqueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_TORQUE, _axis));
    _motorVelocityLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_VELOCITY, _axis));
    _motorPositionLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_POSITION, _axis));
    _bridgeCommandBar->setNode(node);
    _motorCurrentLabel->setNode(node);
    _motorTorqueLabel->setNode(node);
    _motorVelocityLabel->setNode(node);
    _motorPositionLabel->setNode(node);

    // BLDC config
    _polePairSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_CONFIG_POLE_PAIR, _axis));
    _reverseHallPolarityCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONFIG_FLAGS, _axis));
    _polePairSpinBox->setNode(node);
    _reverseHallPolarityCheckBox->setNode(node);

    // BLDC status
    _hallRawValueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_STATUS_HALL_RAW, _axis));
    _hallPhaseLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_STATUS_HALL_PHASE, _axis));
    _electricalAngleLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_BLDC_STATUS_ELECTRICAL_ANGLE, _axis));
    _hallRawValueLabel->setNode(node);
    _hallPhaseLabel->setNode(node);
    _electricalAngleLabel->setNode(node);
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
        _bldcConfigGroupBox->setEnabled(false);
        _enableButton->setEnabled(true);
        _informationLabel->setText(tr("Not available in \"Operation Enabled\""));
    }
    else
    {
        _motorConfigGroupBox->setEnabled(true);
        _bldcConfigGroupBox->setEnabled(true);
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
    QWidget *motorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motorWidget);
    actionLayout->setContentsMargins(0, 0, 0, 0);
    actionLayout->setSpacing(0);

    actionLayout->addWidget(createInformationWidgets());

    _motorConfigGroupBox = createMotorConfigWidgets();
    actionLayout->addWidget(_motorConfigGroupBox);
    actionLayout->addWidget(createMotorStatusWidgets());
    _bldcConfigGroupBox = createBldcConfigWidgets();
    actionLayout->addWidget(_bldcConfigGroupBox);
    actionLayout->addWidget(createBldcStatusWidgets());

    QScrollArea *motorScrollArea = new QScrollArea;
    motorScrollArea->setWidget(motorWidget);
    motorScrollArea->setWidgetResizable(true);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2, 2);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addWidget(motorScrollArea);
    setLayout(vBoxLayout);
}

QToolBar *MotorWidget::createToolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Motor commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotorWidget::readAllObject);

    // currents actions
    QAction *mapCurrentsAction = toolBar->addAction(tr("Map currents"));
    mapCurrentsAction->setShortcut(QKeySequence("Ctrl+M"));
    connect(mapCurrentsAction, &QAction::triggered, this, &MotorWidget::mapCurrents);

    QAction *monitorCurrentsAction = toolBar->addAction(tr("Logger currents"));
    monitorCurrentsAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(monitorCurrentsAction, &QAction::triggered, this, &MotorWidget::monitorCurrents);

    return toolBar;
}

QGroupBox *MotorWidget::createInformationWidgets()
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

QGroupBox *MotorWidget::createMotorConfigWidgets()
{
    QGroupBox *motorConfigGroupBox = new QGroupBox(tr("Motor config"));
    QFormLayout *configLayout = new QFormLayout();
    configLayout->setVerticalSpacing(3);
    configLayout->setHorizontalSpacing(3);

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
    _indexWidgets.append(_motorTypeComboBox);

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    configLayout->addRow(frame);

    _peakCurrentSpinBox = new IndexSpinBox();
    _peakCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    _peakCurrentSpinBox->setUnit(" A");
    _peakCurrentSpinBox->setScale(1.0 / 100);
    configLayout->addRow(tr("P&eak current:"), _peakCurrentSpinBox);
    _indexWidgets.append(_peakCurrentSpinBox);

    QHBoxLayout *burstCurrentLayout = new QHBoxLayout();
    burstCurrentLayout->setSpacing(0);

    _burstCurrentSpinBox = new IndexSpinBox();
    _burstCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    _burstCurrentSpinBox->setUnit(" A");
    _burstCurrentSpinBox->setScale(1.0 / 100);
    burstCurrentLayout->addWidget(_burstCurrentSpinBox);
    _indexWidgets.append(_burstCurrentSpinBox);

    QLabel *label = new QLabel(tr("/"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    burstCurrentLayout->addWidget(label);

    _burstDurationSpinBox = new IndexSpinBox();
    _burstDurationSpinBox->setUnit(" ms");
    burstCurrentLayout->addWidget(_burstDurationSpinBox);
    label = new QLabel(tr("Burst &current:"));
    label->setBuddy(_burstDurationSpinBox);
    configLayout->addRow(label, burstCurrentLayout);
    _indexWidgets.append(_burstDurationSpinBox);

    _sustainedCurrentSpinBox = new IndexSpinBox();
    _sustainedCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    _sustainedCurrentSpinBox->setUnit(" A");
    _sustainedCurrentSpinBox->setScale(1.0 / 100);
    configLayout->addRow(tr("&Sustained current:"), _sustainedCurrentSpinBox);
    _indexWidgets.append(_sustainedCurrentSpinBox);

    _currentConstantSpinBox = new IndexSpinBox();
    _currentConstantSpinBox->setUnit(" Nm/A");
    _currentConstantSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("C&urrent constant:"), _currentConstantSpinBox);
    _indexWidgets.append(_currentConstantSpinBox);

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    configLayout->addRow(frame);

    _maxVelocitySpinBox = new IndexSpinBox();
    _maxVelocitySpinBox->setUnit(" rpm");
    configLayout->addRow(tr("M&ax velocity:"), _maxVelocitySpinBox);
    _indexWidgets.append(_maxVelocitySpinBox);

    _velocityConstantSpinBox = new IndexSpinBox();
    _velocityConstantSpinBox->setUnit(" V/rpm");
    _velocityConstantSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("Ve&locity constant:"), _velocityConstantSpinBox);
    _indexWidgets.append(_velocityConstantSpinBox);

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    configLayout->addRow(frame);

    _brakeBypassCheckBox = new IndexCheckBox();
    _brakeBypassCheckBox->setBitMask(1);
    configLayout->addRow(tr("&Brake override:"), _brakeBypassCheckBox);
    _indexWidgets.append(_brakeBypassCheckBox);

    _reverseMotorPolarityCheckBox = new IndexCheckBox();
    _reverseMotorPolarityCheckBox->setBitMask(1);
    configLayout->addRow(tr("&Invert motor direction:"), _reverseMotorPolarityCheckBox);
    _indexWidgets.append(_reverseMotorPolarityCheckBox);

    motorConfigGroupBox->setLayout(configLayout);
    return motorConfigGroupBox;
}

QGroupBox *MotorWidget::createMotorStatusWidgets()
{
    QGroupBox *statusGroupBox = new QGroupBox(tr("Motor status"));
    QFormLayout *statusLayout = new QFormLayout();
    statusLayout->setVerticalSpacing(3);
    statusLayout->setHorizontalSpacing(3);

    _bridgeCommandBar = new IndexBar();
    statusLayout->addRow(tr("Command duty cycle:"), _bridgeCommandBar);
    _bridgeCommandBar->setUnit("%");
    _bridgeCommandBar->setScale(100.0/32768.0);
    _bridgeCommandBar->setRange(-100, 100);
    _indexWidgets.append(_bridgeCommandBar);

    _motorCurrentLabel = new IndexLabel();
    _motorCurrentLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    _motorCurrentLabel->setScale(1.0 / 100.0);
    _motorCurrentLabel->setUnit(" A");
    statusLayout->addRow(tr("Current"), _motorCurrentLabel);
    _indexWidgets.append(_motorCurrentLabel);

    _motorTorqueLabel = new IndexLabel();
    _motorTorqueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _motorTorqueLabel->setUnit(" Nm");
    statusLayout->addRow(tr("Torque"), _motorTorqueLabel);
    _indexWidgets.append(_motorTorqueLabel);

    _motorVelocityLabel = new IndexLabel();
    _motorVelocityLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _motorVelocityLabel->setUnit(" rpm");
    statusLayout->addRow(tr("Velocity"), _motorVelocityLabel);
    _indexWidgets.append(_motorVelocityLabel);

    _motorPositionLabel = new IndexLabel();
    _motorPositionLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _motorPositionLabel->setUnit(" tr");
    statusLayout->addRow(tr("Position"), _motorPositionLabel);
    _indexWidgets.append(_motorPositionLabel);

    _bridgeTemp1Label = new IndexLabel();
    statusLayout->addRow(tr("Temperature bridge 1:"), _bridgeTemp1Label);
    _bridgeTemp1Label->setScale(1.0 / 10.0);
    _bridgeTemp1Label->setUnit(" °C");
    _indexWidgets.append(_bridgeTemp1Label);

    _bridgeTemp2Label = new IndexLabel();
    statusLayout->addRow(tr("Temperature bridge 2:"), _bridgeTemp2Label);
    _bridgeTemp2Label->setScale(1.0 / 10.0);
    _bridgeTemp2Label->setUnit(" °C");
    _indexWidgets.append(_bridgeTemp2Label);

    statusGroupBox->setLayout(statusLayout);
    return statusGroupBox;
}

QGroupBox *MotorWidget::createBldcConfigWidgets()
{
    QGroupBox *configGroupBox = new QGroupBox(tr("BLDC config"));
    QFormLayout *configLayout = new QFormLayout();
    configLayout->setVerticalSpacing(3);
    configLayout->setHorizontalSpacing(3);

    _polePairSpinBox = new IndexSpinBox();
    configLayout->addRow(tr("P&ole pair:"), _polePairSpinBox);
    _indexWidgets.append(_polePairSpinBox);

    _reverseHallPolarityCheckBox = new IndexCheckBox();
    _reverseHallPolarityCheckBox->setBitMask(2);
    configLayout->addRow(tr("Invert &hall polarity:"), _reverseHallPolarityCheckBox);
    _indexWidgets.append(_reverseHallPolarityCheckBox);

    configGroupBox->setLayout(configLayout);
    return configGroupBox;
}

QGroupBox *MotorWidget::createBldcStatusWidgets()
{
    QGroupBox *statusGroupBox = new QGroupBox(tr("BLDC status"));
    QFormLayout *statusLayout = new QFormLayout();
    statusLayout->setVerticalSpacing(3);
    statusLayout->setHorizontalSpacing(3);

    _hallRawValueLabel = new IndexLabel();
    statusLayout->addRow(tr("Hall raw:"), _hallRawValueLabel);
    _indexWidgets.append(_hallRawValueLabel);

    _hallPhaseLabel = new IndexLabel();
    statusLayout->addRow(tr("Hall phase:"), _hallPhaseLabel);
    _indexWidgets.append(_hallPhaseLabel);

    _electricalAngleLabel = new IndexLabel();
    statusLayout->addRow(tr("Electrical angle:"), _electricalAngleLabel);
    _electricalAngleLabel->setUnit("°");
    _indexWidgets.append(_electricalAngleLabel);

    statusGroupBox->setLayout(statusLayout);
    return statusGroupBox;
}

void MotorWidget::goEnableButton()
{
    _nodeProfile402->goToState(NodeProfile402::STATE_SwitchedOn);
}

void MotorWidget::mapCurrents()
{
    QList<NodeObjectId> currentsHLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsHLObjectsList.append(NodeObjectId(_node->busId(), _node->nodeId(), 0x2802, i));
    }
    QList<NodeObjectId> currentsLLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsLLObjectsList.append(NodeObjectId(_node->busId(), _node->nodeId(), 0x2803, i));
    }

    _nodeProfile402->node()->tpdos().at(2)->writeMapping(currentsHLObjectsList);
    _nodeProfile402->node()->tpdos().at(3)->writeMapping(currentsLLObjectsList);
}

void MotorWidget::monitorCurrents()
{
    QList<NodeObjectId> currentsHLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsHLObjectsList.append(NodeObjectId(_node->busId(), _node->nodeId(), 0x2802, i));
    }
    QList<NodeObjectId> currentsLLObjectsList;
    for (int i = 1; i <= 4; i++)
    {
        currentsLLObjectsList.append(NodeObjectId(_node->busId(), _node->nodeId(), 0x2803, i));
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
