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

#include "motionsensorwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexcombobox.h"
#include "canopen/widget/indexlabel.h"
#include "canopen/widget/indexspinbox.h"

#include "indexdb402.h"
#include "node.h"
#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSplitter>
#include <QWidget>

MotionSensorWidget::MotionSensorWidget(QWidget *parent)
    : QWidget(parent)
{
    _nodeProfile402 = nullptr;
    createWidgets();
    _state = NONE;
    _mode = MODE_SENSOR_NONE;
}

Node *MotionSensorWidget::node() const
{
    return _node;
}

QString MotionSensorWidget::title() const
{
    switch (_mode)
    {
    case MotionSensorWidget::MODE_SENSOR_NONE:
        return tr("None");
    case MODE_SENSOR_POSITION:
        return tr("Position sensor");
    case MODE_SENSOR_VELOCITY:
        return tr("Velocity sensor");
    case MODE_SENSOR_TORQUE:
        return tr("Torque sensor");
    }
    return QString();
}

void MotionSensorWidget::setNode(Node *node, uint8_t axis)
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

    _dataLogger->removeAllData();

    connect(_node, &Node::statusChanged, this, &MotionSensorWidget::statusNodeChanged);
    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &MotionSensorWidget::stateChanged);
    }
    setIMode();

    _sensorSelectComboBox->setNode(node);
    _thresholdMinSpinBox->setNode(node);
    _thresholdMaxSpinBox->setNode(node);
    _thresholdModeComboBox->setNode(node);
    _preOffsetSpinBox->setNode(node);
    _scaleSpinBox->setNode(node);
    _postOffsetSpinBox->setNode(node);
    _errorMinSpinBox->setNode(node);
    _errorMaxSpinBox->setNode(node);
    _frequencyDividerSpinBox->setNode(node);
    _configBitSpinBox->setNode(node);

    _rawDataValueLabel->setNode(node);
    _flagLabel->setNode(node);
    _valueLabel->setNode(node);

    _filterSelect->setNode(node);
    _param0->setNode(node);
    _param1->setNode(node);
    _param2->setNode(node);
    _param3->setNode(node);
}

void MotionSensorWidget::setMode(MotionSensorWidget::ModeSensor mode)
{
    _mode = mode;
    setIMode();
}

void MotionSensorWidget::toggleStartLogger(bool start)
{
    if (start)
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-stop.png"));
        _dataLogger->start(_logTimerSpinBox->value());
    }
    else
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
        _dataLogger->stop();
    }
}

void MotionSensorWidget::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _dataLogger->start(ms);
    }
}

void MotionSensorWidget::setIMode()
{
    NodeObjectId _sensorSelectSpinBox_ObjId;
    NodeObjectId _thresholdMinSpinBox_ObjId;
    NodeObjectId _thresholdMaxSpinBox_ObjId;
    NodeObjectId _thresholdModeSpinBox_ObjId;
    NodeObjectId _preOffsetSpinBox_ObjId;
    NodeObjectId _scaleSpinBox_ObjId;
    NodeObjectId _postOffsetSpinBox_ObjId;
    NodeObjectId _errorMinSpinBox_ObjId;
    NodeObjectId _errorMaxSpinBox_ObjId;
    NodeObjectId _frequencyDividerSpinBox_ObjId;
    NodeObjectId _configBitSpinBox_ObjId;

    NodeObjectId _rawDataValueLabel_ObjId;
    NodeObjectId _flagLabel_ObjId;
    NodeObjectId _valueLabel_ObjId;

    NodeObjectId _filterSelect_ObjId;
    NodeObjectId _param0_ObjId;
    NodeObjectId _param1_ObjId;
    NodeObjectId _param2_ObjId;
    NodeObjectId _param3_ObjId;

    if (!_nodeProfile402)
    {
        return;
    }

    switch (_mode)
    {
    case MODE_SENSOR_NONE:
        break;

    case MODE_SENSOR_VELOCITY:
        _sensorConfigGroupBox->setTitle(tr("Velocity sensor configuration"));
        _sensorSelectSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_SELECT, _axis);
        _thresholdMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_THRESHOLD_MIN, _axis);
        _thresholdMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_THRESHOLD_MAX, _axis);
        _thresholdModeSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_THRESHOLD_MODE, _axis);
        _preOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_PRE_OFFSET, _axis);
        _scaleSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_SCALE, _axis);
        _postOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_POST_OFFSET, _axis);
        _errorMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_ERROR_MIN, _axis);
        _errorMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_ERROR_MAX, _axis);
        _frequencyDividerSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_FREQUENCY_DIVIDER, _axis);
        _configBitSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_CONFIG_BIT, _axis);

        _rawDataValueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_STATUS_RAWDATA, _axis);
        _flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_STATUS_FLAG, _axis);
        _valueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_STATUS_VALUE, _axis);

        _filterSelect_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_FILTER_SELECT, _axis);
        _param0_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_FILTER_PARAM_0, _axis);
        _param1_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_FILTER_PARAM_1, _axis);
        _param2_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_FILTER_PARAM_2, _axis);
        _param3_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_FILTER_PARAM_3, _axis);

        break;

    case MODE_SENSOR_TORQUE:
        _sensorConfigGroupBox->setTitle(tr("Torque sensor configuration"));
        _sensorSelectSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_SELECT, _axis);
        _thresholdMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_THRESHOLD_MIN, _axis);
        _thresholdMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_THRESHOLD_MAX, _axis);
        _thresholdModeSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_THRESHOLD_MODE, _axis);
        _preOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_PRE_OFFSET, _axis);
        _scaleSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_SCALE, _axis);
        _postOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_POST_OFFSET, _axis);
        _errorMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_ERROR_MIN, _axis);
        _errorMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_ERROR_MAX, _axis);
        _frequencyDividerSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_FREQUENCY_DIVIDER, _axis);
        _configBitSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_CONFIG_BIT, _axis);

        _rawDataValueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_STATUS_RAWDATA, _axis);
        _flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_STATUS_FLAG, _axis);
        _valueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_STATUS_VALUE, _axis);

        _filterSelect_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_FILTER_SELECT, _axis);
        _param0_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_FILTER_PARAM_0, _axis);
        _param1_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_FILTER_PARAM_1, _axis);
        _param2_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_FILTER_PARAM_2, _axis);
        _param3_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_FILTER_PARAM_3, _axis);
        break;

    case MODE_SENSOR_POSITION:
        _sensorConfigGroupBox->setTitle(tr("Position sensor configuration"));
        _sensorSelectSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_SELECT, _axis);
        _thresholdMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_THRESHOLD_MIN, _axis);
        _thresholdMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_THRESHOLD_MAX, _axis);
        _thresholdModeSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_THRESHOLD_MODE, _axis);
        _preOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_PRE_OFFSET, _axis);
        _scaleSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_SCALE, _axis);
        _postOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_POST_OFFSET, _axis);
        _errorMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_ERROR_MIN, _axis);
        _errorMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_ERROR_MAX, _axis);
        _frequencyDividerSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_FREQUENCY_DIVIDER, _axis);
        _configBitSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_CONFIG_BIT, _axis);

        _rawDataValueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_STATUS_RAWDATA, _axis);
        _flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_STATUS_FLAG, _axis);
        _valueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_STATUS_VALUE, _axis);

        _filterSelect_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_FILTER_SELECT, _axis);
        _param0_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_FILTER_PARAM_0, _axis);
        _param1_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_FILTER_PARAM_1, _axis);
        _param2_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_FILTER_PARAM_2, _axis);
        _param3_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_FILTER_PARAM_3, _axis);
        break;
    }

    _sensorSelectComboBox->setObjId(_sensorSelectSpinBox_ObjId);
    _thresholdMinSpinBox->setObjId(_thresholdMinSpinBox_ObjId);
    _thresholdMaxSpinBox->setObjId(_thresholdMaxSpinBox_ObjId);
    _thresholdModeComboBox->setObjId(_thresholdModeSpinBox_ObjId);
    _preOffsetSpinBox->setObjId(_preOffsetSpinBox_ObjId);
    _scaleSpinBox->setObjId(_scaleSpinBox_ObjId);
    _postOffsetSpinBox->setObjId(_postOffsetSpinBox_ObjId);
    _errorMinSpinBox->setObjId(_errorMinSpinBox_ObjId);
    _errorMaxSpinBox->setObjId(_errorMaxSpinBox_ObjId);
    _frequencyDividerSpinBox->setObjId(_frequencyDividerSpinBox_ObjId);
    _configBitSpinBox->setObjId(_configBitSpinBox_ObjId);

    _rawDataValueLabel->setObjId(_rawDataValueLabel_ObjId);
    _flagLabel->setObjId(_flagLabel_ObjId);
    _valueLabel->setObjId(_valueLabel_ObjId);

    _filterSelect->setObjId(_filterSelect_ObjId);
    _param0->setObjId(_param0_ObjId);
    _param1->setObjId(_param1_ObjId);
    _param2->setObjId(_param2_ObjId);
    _param3->setObjId(_param3_ObjId);

    _rawDataValueLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _flagLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _valueLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());

    _dataLogger->removeData(_rawDataValueLabel_ObjId);
    _dataLogger->removeData(_valueLabel_ObjId);
    _dataLogger->addData(_rawDataValueLabel_ObjId);
    _dataLogger->addData(_valueLabel_ObjId);
}

void MotionSensorWidget::createWidgets()
{
    _dataLogger = new DataLogger();
    _dataLoggerChartsWidget = new DataLoggerChartsWidget(_dataLogger);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // toolbar
    _toolBar = new QToolBar(tr("Data logger commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &MotionSensorWidget::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    _toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });

    // clear
    QAction *action;
    action = _toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _dataLogger, &DataLogger::clear);

    _toolBar->addSeparator();

    // read all action
    QAction * readAllAction = _toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotionSensorWidget::readAllObject);

    QWidget *motionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motionSensorWidget);

    _sensorConfigGroupBox = new QGroupBox(tr("Sensor config"));
    QFormLayout *configLayout = new QFormLayout();

    _sensorSelectComboBox = new IndexComboBox();
    _sensorSelectComboBox->addItem("OFF", QVariant(static_cast<uint16_t>(0x0000)));

    _sensorSelectComboBox->addItem("TORQUE_FROM_MOTOR", QVariant(static_cast<uint16_t>(0x1100)));
    _sensorSelectComboBox->addItem("VELOCITY_FROM_MOTOR", QVariant(static_cast<uint16_t>(0x1200)));
    _sensorSelectComboBox->addItem("POSITION_FROM_MOTOR", QVariant(static_cast<uint16_t>(0x1300)));

    _sensorSelectComboBox->addItem("POSITION_FROM_VELOCITY", QVariant(static_cast<uint16_t>(0x2200)));
    _sensorSelectComboBox->addItem("VELOCITY_FROM_POSITION", QVariant(static_cast<uint16_t>(0x2300)));

    _sensorSelectComboBox->addItem("QEI_CH1", QVariant(static_cast<uint16_t>(0x3101)));
    _sensorSelectComboBox->addItem("QEI_CH2", QVariant(static_cast<uint16_t>(0x3102)));

    _sensorSelectComboBox->addItem("SSI_CH1", QVariant(static_cast<uint16_t>(0x3201)));
    _sensorSelectComboBox->addItem("SSI_CH2", QVariant(static_cast<uint16_t>(0x3202)));

    _sensorSelectComboBox->addItem("ANALOG_CH1", QVariant(static_cast<uint16_t>(0x4001)));
    _sensorSelectComboBox->addItem("ANALOG_CH2", QVariant(static_cast<uint16_t>(0x4002)));
    configLayout->addRow(tr("&Sensor select :"), _sensorSelectComboBox);

    _preOffsetSpinBox = new IndexSpinBox();
    _preOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("Pr&e offset :"), _preOffsetSpinBox);

    _scaleSpinBox = new IndexSpinBox();
    _scaleSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("S&cale :"), _scaleSpinBox);

    _postOffsetSpinBox = new IndexSpinBox();
    _postOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("&Post offset :"), _postOffsetSpinBox);

    QLayout *errorRangelayout = new QHBoxLayout();
    errorRangelayout->setSpacing(0);
    _errorMinSpinBox = new IndexSpinBox();
    _errorMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    errorRangelayout->addWidget(_errorMinSpinBox);
    QLabel *errorRangeSepLabel = new QLabel(tr("-"));
    errorRangeSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    errorRangelayout->addWidget(errorRangeSepLabel);
    _errorMaxSpinBox = new IndexSpinBox();
    _errorMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    errorRangelayout->addWidget(_errorMaxSpinBox);
    QLabel *errorRangeLabel = new QLabel(tr("&Error range :"));
    errorRangeLabel->setBuddy(_errorMinSpinBox);
    configLayout->addRow(errorRangeLabel, errorRangelayout);

    _thresholdModeComboBox = new IndexComboBox();
    _thresholdModeComboBox->addItem("No threshold", QVariant(0));
    _thresholdModeComboBox->addItem("Min-max mode", QVariant(1));
    _thresholdModeComboBox->addItem("Old value mode", QVariant(3));
    configLayout->addRow(tr("Th&reshold mode :"), _thresholdModeComboBox);

    QLayout *thresholdlayout = new QHBoxLayout();
    thresholdlayout->setSpacing(0);
    _thresholdMinSpinBox = new IndexSpinBox();
    _thresholdMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    thresholdlayout->addWidget(_thresholdMinSpinBox);
    QLabel *thresholdSepLabel = new QLabel(tr("-"));
    thresholdSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    thresholdlayout->addWidget(thresholdSepLabel);
    _thresholdMaxSpinBox = new IndexSpinBox();
    _thresholdMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    thresholdlayout->addWidget(_thresholdMaxSpinBox);
    QLabel *thresholdLabel = new QLabel(tr("&Threshold :"));
    thresholdLabel->setBuddy(_thresholdMinSpinBox);
    configLayout->addRow(thresholdLabel, thresholdlayout);

    _frequencyDividerSpinBox = new IndexSpinBox();
    _frequencyDividerSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    configLayout->addRow(tr("&Frequency divider :"), _frequencyDividerSpinBox);

    _configBitSpinBox = new IndexSpinBox();
    _configBitSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    configLayout->addRow(tr("&Config bit :"), _configBitSpinBox);

    _sensorConfigGroupBox->setLayout(configLayout);

    QGroupBox *statusGroupBox = new QGroupBox(tr("Sensor status"));
    QFormLayout *statusLayout = new QFormLayout();

    _rawDataValueLabel = new IndexLabel();
    _rawDataValueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    statusLayout->addRow(tr("Ra&w Data :"), _rawDataValueLabel);

    _flagLabel = new IndexLabel();
    statusLayout->addRow(tr("&Flag :"), _flagLabel);

    _valueLabel = new IndexLabel();
    _valueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    statusLayout->addRow(tr("&Value :"), _valueLabel);

    statusGroupBox->setLayout(statusLayout);

    _filterGroupBox = new QGroupBox(tr("Sensor filter"));
    QFormLayout *filterLayout = new QFormLayout();

    _filterSelect = new IndexComboBox();
    _filterSelect->addItem("OFF", QVariant(static_cast<uint16_t>(0x0000)));
    _filterSelect->addItem("LPF", QVariant(static_cast<uint16_t>(0x1000)));
    _filterSelect->addItem("AVEGAGING", QVariant(static_cast<uint16_t>(0x2000)));
    _filterSelect->addItem("INTEGRATOR", QVariant(static_cast<uint16_t>(0x8000)));
    filterLayout->addRow(tr("Filter select:"), _filterSelect);

    _param0 = new IndexSpinBox();
    _param0->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    filterLayout->addRow(tr("Param_&0:"), _param0);

    _param1 = new IndexSpinBox();
    _param1->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    filterLayout->addRow(tr("Param_&1:"), _param1);

    _param2 = new IndexSpinBox();
    _param2->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    filterLayout->addRow(tr("Param_&2:"), _param2);

    _param3 = new IndexSpinBox();
    _param3->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    filterLayout->addRow(tr("Param_&3:"), _param3);

    _filterGroupBox->setLayout(filterLayout);

    QGroupBox *informationGroupBox = new QGroupBox(tr("Information"));
    _informationLabel = new QLabel(tr("Not available in \"Operation Enabled\""));
    _enableButton = new QPushButton(tr("Unlock, Go to \"Switched On\""));
    connect(_enableButton, &QPushButton::clicked, this, &MotionSensorWidget::goEnableButton);

    QVBoxLayout *informationLayout = new QVBoxLayout();
    informationLayout->addWidget(_informationLabel);
    informationLayout->addWidget(_enableButton);
    informationGroupBox->setLayout(informationLayout);

    QGroupBox *targetGroupBox = new QGroupBox(tr("Target"));
    QFormLayout *targetLayout = new QFormLayout();
    targetGroupBox->setLayout(targetLayout);

    actionLayout->addWidget(informationGroupBox);
    actionLayout->addWidget(_sensorConfigGroupBox);
    actionLayout->addWidget(_filterGroupBox);
    actionLayout->addWidget(statusGroupBox);

    QScrollArea *motionSensorScrollArea = new QScrollArea;
    motionSensorScrollArea->setWidget(motionSensorWidget);
    motionSensorScrollArea->setWidgetResizable(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    layout->addWidget(splitter);
    splitter->addWidget(motionSensorScrollArea);
    splitter->addWidget(_dataLoggerChartsWidget);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(_toolBar);
    vBoxLayout->addWidget(splitter);
    vBoxLayout->setMargin(2);
    setLayout(vBoxLayout);
}

void MotionSensorWidget::statusNodeChanged(Node::Status status)
{
    if (status == Node::STOPPED)
    {
        this->setEnabled(false);
    }
    else
    {
        this->setEnabled(true);
    }
}

void MotionSensorWidget::stateChanged()
{
    if (_nodeProfile402->currentState() == NodeProfile402::STATE_OperationEnabled)
    {
        _sensorConfigGroupBox->setEnabled(false);
        _filterGroupBox->setEnabled(false);
        _enableButton->setEnabled(true);
        _informationLabel->show();
    }
    else
    {
        _sensorConfigGroupBox->setEnabled(true);
        _filterGroupBox->setEnabled(true);
        _enableButton->setEnabled(false);
        _informationLabel->hide();
    }
}

void MotionSensorWidget::readAllObject()
{
    _sensorSelectComboBox->readObject();
    _thresholdMinSpinBox->readObject();
    _thresholdMaxSpinBox->readObject();
    _thresholdModeComboBox->readObject();
    _preOffsetSpinBox->readObject();
    _scaleSpinBox->readObject();
    _postOffsetSpinBox->readObject();
    _errorMinSpinBox->readObject();
    _errorMaxSpinBox->readObject();
    _frequencyDividerSpinBox->readObject();
    _configBitSpinBox->readObject();

    _rawDataValueLabel->readObject();
    _flagLabel->readObject();
    _valueLabel->readObject();

    _filterSelect->readObject();
    _param0->readObject();
    _param1->readObject();
    _param2->readObject();
    _param3->readObject();
}

void MotionSensorWidget::goEnableButton()
{
    _nodeProfile402->goToState(NodeProfile402::STATE_SwitchedOn);
}
