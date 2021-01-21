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

#include "motionsensorwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
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
    }
    setIMode();

    _sensorSelectSpinBox->setNode(node);
    _thresholdMinSpinBox->setNode(node);
    _thresholdMaxSpinBox->setNode(node);
    _thresholdModeSpinBox->setNode(node);
    _preOffsetSpinBox->setNode(node);
    _scaleSpinBox->setNode(node);
    _postOffsetSpinBox->setNode(node);
    _errorMinSpinBox->setNode(node);
    _errorMaxsetSpinBox->setNode(node);

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

void MotionSensorWidget::start()
{
    _dataLogger->start(_logTimerSpinBox->value());
}

void MotionSensorWidget::stop()
{
    _dataLogger->stop();
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
    NodeObjectId _errorMaxsetSpinBox_ObjId;

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
        sensorConfigGroupBox->setTitle(tr("Sensor Velocity"));
        _sensorSelectSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_SELECT, _axis);
        _thresholdMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_THRESHOLD_MIN, _axis);
        _thresholdMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_THRESHOLD_MAX, _axis);
        _thresholdModeSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_THRESHOLD_MODE, _axis);
        _preOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_PRE_OFFSET, _axis);
        _scaleSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_SCALE, _axis);
        _postOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_POST_OFFSET, _axis);
        _errorMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_ERROR_MIN, _axis);
        _errorMaxsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_SENSOR_ERROR_MAX, _axis);

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
        sensorConfigGroupBox->setTitle(tr("Sensor Torque"));
        _sensorSelectSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_SELECT, _axis);
        _thresholdMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_THRESHOLD_MIN, _axis);
        _thresholdMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_THRESHOLD_MAX, _axis);
        _thresholdModeSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_THRESHOLD_MODE, _axis);
        _preOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_PRE_OFFSET, _axis);
        _scaleSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_SCALE, _axis);
        _postOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_POST_OFFSET, _axis);
        _errorMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_ERROR_MIN, _axis);
        _errorMaxsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_SENSOR_ERROR_MAX, _axis);

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
        sensorConfigGroupBox->setTitle(tr("Sensor Position"));
        _sensorSelectSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_SELECT, _axis);
        _thresholdMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_THRESHOLD_MIN, _axis);
        _thresholdMaxSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_THRESHOLD_MAX, _axis);
        _thresholdModeSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_THRESHOLD_MODE, _axis);
        _preOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_PRE_OFFSET, _axis);
        _scaleSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_SCALE, _axis);
        _postOffsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_POST_OFFSET, _axis);
        _errorMinSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_ERROR_MIN, _axis);
        _errorMaxsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_SENSOR_ERROR_MAX, _axis);

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

    _sensorSelectSpinBox->setObjId(_sensorSelectSpinBox_ObjId);
    _thresholdMinSpinBox->setObjId(_thresholdMinSpinBox_ObjId);
    _thresholdMaxSpinBox->setObjId(_thresholdMaxSpinBox_ObjId);
    _thresholdModeSpinBox->setObjId(_thresholdModeSpinBox_ObjId);
    _preOffsetSpinBox->setObjId(_preOffsetSpinBox_ObjId);
    _scaleSpinBox->setObjId(_scaleSpinBox_ObjId);
    _postOffsetSpinBox->setObjId(_postOffsetSpinBox_ObjId);
    _errorMinSpinBox->setObjId(_errorMinSpinBox_ObjId);
    _errorMaxsetSpinBox->setObjId(_errorMaxsetSpinBox_ObjId);

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
    _dataLogger->removeData(_flagLabel_ObjId);
    _dataLogger->removeData(_valueLabel_ObjId);
    _dataLogger->addData(_rawDataValueLabel_ObjId);
    _dataLogger->addData(_flagLabel_ObjId);
    _dataLogger->addData(_valueLabel_ObjId);
}

void MotionSensorWidget::setLogTimer(int ms)
{
    _dataLogger->start(ms);
}

void MotionSensorWidget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    QSplitter *splitter = new QSplitter();
    layout->addWidget(splitter);

    // toolbar
    _sensorToolBar = new QToolBar(tr("Axis commands"));
    QActionGroup *groupNmt = new QActionGroup(this);
    groupNmt->setExclusive(true);
    QAction *action;
    action = groupNmt->addAction(tr("Start object reading of axis"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-play.png"));
    action->setStatusTip(tr("Start object reading of axis"));
    connect(action, &QAction::triggered, this, &MotionSensorWidget::start);

    action = groupNmt->addAction(tr("Stop object reading of axis"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    action->setStatusTip(tr("Stop object reading of axis"));
    connect(action, &QAction::triggered, this, &MotionSensorWidget::stop);

    _sensorToolBar->addActions(groupNmt->actions());
    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(500);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });
    _sensorToolBar->addWidget(_logTimerSpinBox);

    QWidget *MotionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(MotionSensorWidget);

    sensorConfigGroupBox = new QGroupBox(tr("Sensor config"));
    QFormLayout *configLayout = new QFormLayout();

    _sensorSelectSpinBox = new IndexSpinBox();
    _sensorSelectSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    configLayout->addRow(tr("&Sensor select :"), _sensorSelectSpinBox);

    _preOffsetSpinBox = new IndexSpinBox();
    _preOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("Pr&e offset :"), _preOffsetSpinBox);

    _scaleSpinBox = new IndexSpinBox();
    _scaleSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("S&cale :"), _scaleSpinBox);

    _postOffsetSpinBox = new IndexSpinBox();
    _postOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("&Post offset :"), _postOffsetSpinBox);

    _errorMinSpinBox = new IndexSpinBox();
    _errorMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("Err&or min :"), _errorMinSpinBox);

    _errorMaxsetSpinBox = new IndexSpinBox();
    _errorMaxsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("Error m&ax :"), _errorMaxsetSpinBox);

    _thresholdMinSpinBox = new IndexSpinBox();
    _thresholdMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("&Threshold min :"), _thresholdMinSpinBox);

    _thresholdMaxSpinBox = new IndexSpinBox();
    _thresholdMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("T&hreshold max :"), _thresholdMaxSpinBox);

    _thresholdModeSpinBox = new IndexSpinBox();
    _thresholdModeSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    configLayout->addRow(tr("Th&reshold mode :"), _thresholdModeSpinBox);

    sensorConfigGroupBox->setLayout(configLayout);

    QGroupBox *statusGroupBox = new QGroupBox(tr("Sensor status"));
    QFormLayout *statusLayout = new QFormLayout();

    _rawDataValueLabel = new IndexLabel();
    statusLayout->addRow(tr("Ra&w Data :"), _rawDataValueLabel);

    _flagLabel = new IndexLabel();
    statusLayout->addRow(tr("&Flag :"), _flagLabel);

    _valueLabel = new IndexLabel();
    statusLayout->addRow(tr("&Value :"), _valueLabel);

    statusGroupBox->setLayout(statusLayout);

    QGroupBox *filterGroupBox = new QGroupBox(tr("Sensor filter"));
    QFormLayout *filterLayout = new QFormLayout();

    _filterSelect = new IndexSpinBox();
    _filterSelect->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
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

    filterGroupBox->setLayout(filterLayout);

    QGroupBox *targetGroupBox = new QGroupBox(tr("Target"));
    QFormLayout *targetLayout = new QFormLayout();

    targetGroupBox->setLayout(targetLayout);

    actionLayout->addWidget(_sensorToolBar);
    actionLayout->addWidget(sensorConfigGroupBox);
    actionLayout->addWidget(statusGroupBox);
    actionLayout->addWidget(filterGroupBox);
    splitter->addWidget(MotionSensorWidget);

    _dataLogger = new DataLogger();
    _dataLoggerChartsWidget = new DataLoggerChartsWidget(_dataLogger);

    splitter->addWidget(_dataLoggerChartsWidget);
    setLayout(layout);
}

void MotionSensorWidget::statusNodeChanged(Node::Status status)
{
    if (status == Node::STARTED)
    {
        this->setEnabled(true);
    }
    else
    {
        this->setEnabled(false);
    }
}

QString MotionSensorWidget::title() const
{
    switch (_mode)
    {
    case MotionSensorWidget::MODE_SENSOR_NONE:
        return tr("None");
    case MODE_SENSOR_POSITION:
        return tr("Sensor Position");
    case MODE_SENSOR_VELOCITY:
        return tr("Sensor Velocity");
    case MODE_SENSOR_TORQUE:
        return tr("Sensor Torque");
    }
    return QString();
}
