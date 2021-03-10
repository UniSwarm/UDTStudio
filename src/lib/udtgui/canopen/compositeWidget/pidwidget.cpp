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

#include "pidwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexlabel.h"
#include "canopen/widget/indexspinbox.h"
#include "indexdb402.h"
#include "node.h"
#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QWidget>

PidWidget::PidWidget(QWidget *parent)
    : QWidget(parent)
{
    _nodeProfile402 = nullptr;
    createWidgets();
    connect(&_timer, &QTimer::timeout, this, &PidWidget::manageMeasurement);
    connect(&_readStatusTimer, &QTimer::timeout, this, &PidWidget::readStatus);
    _state = NONE;
    _modePid = MODE_PID_NONE;
}

Node *PidWidget::node() const
{
    return _node;
}

QString PidWidget::title() const
{
    switch (_modePid)
    {
    case PidWidget::MODE_PID_NONE:
        return tr("None");
    case MODE_PID_POSITION:
        return tr("Position PID");
    case MODE_PID_VELOCITY:
        return tr("Velocity PID");
    case MODE_PID_TORQUE:
        return tr("Torque PID");
    }
    return QString();
}

void PidWidget::setNode(Node *node, uint8_t axis)
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

    connect(_node, &Node::statusChanged, this, &PidWidget::statusNodeChanged);
    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
    }
    setIMode();

    _dSpinBox->setNode(node);
    _pSpinBox->setNode(node);
    _iSpinBox->setNode(node);
    _actualValueLabel->setNode(node);
    _inputLabel->setNode(node);
    _errorLabel->setNode(node);
    _integratorLabel->setNode(node);
    _outputLabel->setNode(node);
    _minSpinBox->setNode(node);
    _maxSpinBox->setNode(node);
    _thresholdSpinBox->setNode(node);
    _freqDividerSpinBox->setNode(node);
}

void PidWidget::setMode(PidWidget::ModePid mode)
{
    _modePid = mode;
    setIMode();
}

void PidWidget::toggleStartLogger(bool start)
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

void PidWidget::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _dataLogger->start(ms);
    }
}

void PidWidget::setIMode()
{
    NodeObjectId pidP_ObjId;
    NodeObjectId pidI_ObjId;
    NodeObjectId pidD_ObjId;
    NodeObjectId pidMin_ObjId;
    NodeObjectId pidMax_ObjId;
    NodeObjectId pidThreshold_ObjId;
    NodeObjectId pidFreqDivider_ObjId;
    NodeObjectId target_ObjId;
    NodeObjectId pidInputStatus_ObjId;
    NodeObjectId pidErrorStatus_ObjId;
    NodeObjectId pidIntegratorStatus_ObjId;
    NodeObjectId pidOutputStatus_ObjId;
    NodeObjectId actualValue_ObjId;

    if (!_nodeProfile402)
    {
        return;
    }

    switch (_modePid)
    {
    case MODE_PID_NONE:
        break;

    case MODE_PID_VELOCITY:
        _pidGroupBox->setTitle(tr("PID Velocity configuration"));
        actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE, _axis);
        pidP_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_P, _axis);
        pidI_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_I, _axis);
        pidD_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_D, _axis);
        pidMin_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_MIN, _axis);
        pidMax_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_MAX, _axis);
        pidThreshold_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_THRESHOLD, _axis);
        pidFreqDivider_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_FREQDIVIDER, _axis);
        pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_INPUT, _axis);
        pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_ERROR, _axis);
        pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_INTEGRATOR, _axis);
        pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_VELOCITY_PID_OUTPUT, _axis);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND, _axis);
        break;

    case MODE_PID_TORQUE:
        _pidGroupBox->setTitle(tr("PID Torque configuration"));
        actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, _axis);
        pidP_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_P, _axis);
        pidI_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_I, _axis);
        pidD_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_D, _axis);
        pidMin_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_MIN, _axis);
        pidMax_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_MAX, _axis);
        pidThreshold_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_THRESHOLD, _axis);
        pidFreqDivider_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_FREQDIVIDER, _axis);
        pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_INPUT, _axis);
        pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_ERROR, _axis);
        pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_INTEGRATOR, _axis);
        pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PID_OUTPUT, _axis);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, _axis);
        _node->readObject(target_ObjId);
        break;

    case MODE_PID_POSITION:
        _pidGroupBox->setTitle(tr("PID Position configuration"));
        actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, _axis);
        pidP_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_P, _axis);
        pidI_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_I, _axis);
        pidD_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_D, _axis);
        pidMin_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_MIN, _axis);
        pidMax_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_MAX, _axis);
        pidThreshold_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_THRESHOLD, _axis);
        pidFreqDivider_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_FREQDIVIDER, _axis);
        pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_INPUT, _axis);
        pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_ERROR, _axis);
        pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_INTEGRATOR, _axis);
        pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PID_OUTPUT, _axis);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, _axis);
        break;
    }

    _pSpinBox->setObjId(pidP_ObjId);
    _iSpinBox->setObjId(pidI_ObjId);
    _dSpinBox->setObjId(pidD_ObjId);

    _actualValueLabel->setObjId(actualValue_ObjId);

    _inputLabel->setObjId(pidInputStatus_ObjId);
    _errorLabel->setObjId(pidErrorStatus_ObjId);
    _integratorLabel->setObjId(pidIntegratorStatus_ObjId);
    _outputLabel->setObjId(pidOutputStatus_ObjId);

    _minSpinBox->setObjId(pidMin_ObjId);
    _maxSpinBox->setObjId(pidMax_ObjId);
    _thresholdSpinBox->setObjId(pidThreshold_ObjId);
    _freqDividerSpinBox->setObjId(pidFreqDivider_ObjId);

    pidInputStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    pidErrorStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    pidIntegratorStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    pidOutputStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    target_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _dataLogger->removeData(pidInputStatus_ObjId);
    _dataLogger->removeData(pidErrorStatus_ObjId);
    _dataLogger->removeData(target_ObjId);
    _dataLogger->addData(pidInputStatus_ObjId);
    _dataLogger->addData(pidErrorStatus_ObjId);
    _dataLogger->addData(target_ObjId);
}

void PidWidget::screenshotSave()
{
    QPixmap pixmap = _dataLoggerChartsWidget->grab();
    QString file = QString("%1_%2.png").arg(title()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss.zzz"));
    pixmap.save(file, "PNG");
}

void PidWidget::changeMode402()
{
    if (!_nodeProfile402)
    {
        return;
    }

    connect(_nodeProfile402, &NodeProfile402::modeChanged, this, &PidWidget::mode402Changed);

    switch (_modePid)
    {
    case MODE_PID_NONE:
        break;

    case MODE_PID_POSITION:
        _nodeProfile402->setDefaultModeValue();
        if (_nodeProfile402->actualMode() != NodeProfile402::IP)
        {
            _nodeProfile402->setMode(NodeProfile402::IP);
        }
        else
        {
            mode402Changed(_axis, NodeProfile402::IP);
        }
        break;

    case MODE_PID_VELOCITY:
        _nodeProfile402->setDefaultModeValue();
        if (_nodeProfile402->actualMode() != NodeProfile402::VL)
        {
            _nodeProfile402->setMode(NodeProfile402::VL);
        }
        else
        {
            mode402Changed(_axis, NodeProfile402::VL);
        }
        break;

    case MODE_PID_TORQUE:
        if (_nodeProfile402->actualMode() != NodeProfile402::TQ)
        {
            _nodeProfile402->setMode(NodeProfile402::TQ);
        }
        else
        {
            mode402Changed(_axis, NodeProfile402::TQ);
        }
        break;
    }

    _savePushButton->setEnabled(true);
    _startTargetPushButton->setEnabled(false);
}

void PidWidget::mode402Changed(uint8_t axis, NodeProfile402::OperationMode modeNew)
{
    if (!_nodeProfile402)
    {
        return;
    }

    if (_axis != axis)
    {
        return;
    }

    switch (modeNew)
    {
    case NodeProfile402::HM:
    case NodeProfile402::Reserved:
    case NodeProfile402::MS:
    case NodeProfile402::NoMode:
        _modePid = ModePid::MODE_PID_NONE;
        break;

    case NodeProfile402::PP:
    case NodeProfile402::IP:
    case NodeProfile402::CSP:
    {
        _modePid = ModePid::MODE_PID_POSITION;
        int actualPosition = _node->nodeOd()->value(_actualValueLabel->objId()).toInt();
        _nodeProfile402->setTarget((_firstTargetSpinBox->value() - actualPosition) + actualPosition);
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        break;
    }

    case NodeProfile402::CSV:
    case NodeProfile402::VL:
    case NodeProfile402::PV:
        _modePid = ModePid::MODE_PID_VELOCITY;
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        _nodeProfile402->setTarget(_firstTargetSpinBox->value());
        break;

    case NodeProfile402::CST:
    case NodeProfile402::CSTCA:
    case NodeProfile402::TQ:
        _modePid = ModePid::MODE_PID_TORQUE;
        _nodeProfile402->setTarget(_firstTargetSpinBox->value());
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        break;
    }
    _timer.start(_windowFirstTargetSpinBox->value());
}

void PidWidget::manageMeasurement()
{
    switch (_state)
    {
    case PidWidget::NONE:
        _dataLogger->clear();
        _dataLogger->start(10);
        _timer.start(10);
        _readStatusTimer.start(10);
        _state = LAUCH_DATALOGGER;
        break;

    case PidWidget::LAUCH_DATALOGGER:
        _timer.stop();
        changeMode402();
        _state = LAUCH_FIRST_TARGET;
        break;

    case PidWidget::LAUCH_FIRST_TARGET:
        stopFirstMeasurement();
        _state = LAUCH_SECOND_TARGET;
        break;

    case PidWidget::LAUCH_SECOND_TARGET:
        stopSecondMeasurement();
        _timer.start(_stopDataLoggerSpinBox->value());
        _state = STOP_DATALOGGER;
        break;

    case PidWidget::STOP_DATALOGGER:
        stopDataLogger();
        _readStatusTimer.stop();
        _state = NONE;
        break;
    }
}

void PidWidget::stopFirstMeasurement()
{
    if (!_nodeProfile402)
    {
        return;
    }
    switch (_modePid)
    {
    case MODE_PID_NONE:
        break;

    case MODE_PID_POSITION:
    {
        int actualPosition = _node->nodeOd()->value(_actualValueLabel->objId()).toInt();
        _nodeProfile402->setTarget((_secondTargetSpinBox->value() - actualPosition) + actualPosition);
        _timer.start(_windowSecondTargetSpinBox->value());
        break;
    }
    case MODE_PID_VELOCITY:
        _nodeProfile402->setTarget(_secondTargetSpinBox->value());
        _timer.start(_windowSecondTargetSpinBox->value());
        break;

    case MODE_PID_TORQUE:
        _nodeProfile402->setTarget(_secondTargetSpinBox->value());
        _timer.start(_windowSecondTargetSpinBox->value());
        break;
    }
}

void PidWidget::stopSecondMeasurement()
{
    if (!_nodeProfile402)
    {
        return;
    }
    switch (_modePid)
    {
    case MODE_PID_NONE:
        break;

    case MODE_PID_POSITION:
        break;

    case MODE_PID_VELOCITY:
        _nodeProfile402->setTarget(0);
        break;

    case MODE_PID_TORQUE:
        _nodeProfile402->setTarget(0);
        break;
    }
}

void PidWidget::stopMeasurement()
{
    _state = PidWidget::STOP_DATALOGGER;
    manageMeasurement();
}

void PidWidget::stopDataLogger()
{
    _timer.stop();
    _state = NONE;
    _dataLogger->stop();
    _savePushButton->setEnabled(true);
    _startTargetPushButton->setEnabled(true);
    disconnect(_nodeProfile402, &NodeProfile402::modeChanged, this, &PidWidget::mode402Changed);
}

void PidWidget::readStatus()
{
    _pSpinBox->readObject();
    _iSpinBox->readObject();
    _dSpinBox->readObject();
    _actualValueLabel->readObject();
    _inputLabel->readObject();
    _errorLabel->readObject();
    _integratorLabel->readObject();
    _outputLabel->readObject();
}

void PidWidget::readAllObject()
{
    _pSpinBox->readObject();
    _iSpinBox->readObject();
    _dSpinBox->readObject();
    _minSpinBox->readObject();
    _maxSpinBox->readObject();
    _thresholdSpinBox->readObject();
    _freqDividerSpinBox->readObject();
    readStatus();
}

void PidWidget::createWidgets()
{
    _dataLogger = new DataLogger();
    _dataLoggerChartsWidget = new DataLoggerChartsWidget(_dataLogger);

    QVBoxLayout *glayout = new QVBoxLayout();
    glayout->setMargin(0);

    // toolbar
    _toolBar = new QToolBar(tr("Data logger commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &PidWidget::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(500);
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
    connect(readAllAction, &QAction::triggered, this, &PidWidget::readAllObject);

    QWidget *pidWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(pidWidget);

    _pidGroupBox = new QGroupBox(tr("PID"));
    QFormLayout *pidLayout = new QFormLayout();

    _pSpinBox = new IndexSpinBox();
    _pSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("&P :"), _pSpinBox);

    _iSpinBox = new IndexSpinBox();
    _iSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("&I :"), _iSpinBox);

    _dSpinBox = new IndexSpinBox();
    _dSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("&D :"), _dSpinBox);

    QLayout *minMaxlayout = new QHBoxLayout();
    minMaxlayout->setSpacing(0);
    _minSpinBox = new IndexSpinBox();
    _minSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    minMaxlayout->addWidget(_minSpinBox);
    QLabel *errorRangeSepLabel = new QLabel(tr("-"));
    errorRangeSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    minMaxlayout->addWidget(errorRangeSepLabel);
    _maxSpinBox = new IndexSpinBox();
    _maxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    minMaxlayout->addWidget(_maxSpinBox);
    QLabel *errorRangeLabel = new QLabel(tr("&Min max :"));
    errorRangeLabel->setBuddy(errorRangeLabel);
    pidLayout->addRow(errorRangeLabel, minMaxlayout);

    _thresholdSpinBox = new IndexSpinBox();
    _thresholdSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("&Threshold :"), _thresholdSpinBox);

    _freqDividerSpinBox = new IndexSpinBox();
    pidLayout->addRow(tr("&Frequency divider :"), _freqDividerSpinBox);

    _pidGroupBox->setLayout(pidLayout);

    // Status
    QGroupBox *statusGroupBox = new QGroupBox(tr("PID status"));
    QFormLayout *statusLayout = new QFormLayout();

    _actualValueLabel = new IndexLabel();
    statusLayout->addRow(tr("&Actual Value :"), _actualValueLabel);

    _inputLabel = new IndexLabel();
    _inputLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    statusLayout->addRow(tr("&Input :"), _inputLabel);

    _errorLabel = new IndexLabel();
    _errorLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    statusLayout->addRow(tr("&Error :"), _errorLabel);

    _integratorLabel = new IndexLabel();
    _integratorLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    statusLayout->addRow(tr("&Integrator :"), _integratorLabel);

    _outputLabel = new IndexLabel();
    _outputLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    statusLayout->addRow(tr("&Output :"), _outputLabel);

    statusGroupBox->setLayout(statusLayout);

    // Target
    QGroupBox *targetGroupBox = new QGroupBox(tr("PID test"));
    QFormLayout *targetLayout = new QFormLayout();

    QHBoxLayout *firstTargetLayout = new QHBoxLayout();
    _firstTargetSpinBox = new QSpinBox();
    _firstTargetSpinBox->setValue(200);
    _firstTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _windowFirstTargetSpinBox = new QSpinBox();
    _windowFirstTargetSpinBox->setRange(10, 5000);
    _windowFirstTargetSpinBox->setValue(3000);
    _windowFirstTargetSpinBox->setSuffix(" ms");
    firstTargetLayout->addWidget(_firstTargetSpinBox);
    firstTargetLayout->addWidget(_windowFirstTargetSpinBox);
    targetLayout->addRow(tr("Fisrt Target :"), firstTargetLayout);

    QHBoxLayout *secondTargetLayout = new QHBoxLayout();
    _secondTargetSpinBox = new QSpinBox();
    _secondTargetSpinBox->setValue(100);
    _secondTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _windowSecondTargetSpinBox = new QSpinBox();
    _windowSecondTargetSpinBox->setRange(10, 5000);
    _windowSecondTargetSpinBox->setValue(3000);
    _windowSecondTargetSpinBox->setSuffix(" ms");
    secondTargetLayout->addWidget(_secondTargetSpinBox);
    secondTargetLayout->addWidget(_windowSecondTargetSpinBox);
    targetLayout->addRow(tr("Second Target :"), secondTargetLayout);

    _stopDataLoggerSpinBox = new QSpinBox();
    _stopDataLoggerSpinBox->setRange(10, 5000);
    _stopDataLoggerSpinBox->setValue(100);
    _stopDataLoggerSpinBox->setSuffix(" ms");
    targetLayout->addRow(tr("&End time :"), _stopDataLoggerSpinBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    _savePushButton = new QPushButton(tr("&Screenshot logger"));
    targetLayout->addRow(_savePushButton);
    connect(_savePushButton, &QPushButton::clicked, this, &PidWidget::screenshotSave);

    _stopTargetPushButton = new QPushButton(tr("Stop"));
    buttonLayout->addWidget(_stopTargetPushButton);
    connect(_stopTargetPushButton, &QPushButton::clicked, this, &PidWidget::stopMeasurement);

    _startTargetPushButton = new QPushButton(tr("Start"));
    buttonLayout->addWidget(_startTargetPushButton);
    connect(_startTargetPushButton, &QPushButton::clicked, this, &PidWidget::manageMeasurement);

    targetLayout->addRow(buttonLayout);
    targetGroupBox->setLayout(targetLayout);

    actionLayout->addWidget(_pidGroupBox);
    actionLayout->addWidget(statusGroupBox);
    actionLayout->addWidget(targetGroupBox);

    QScrollArea *pidScrollArea = new QScrollArea;
    pidScrollArea->setWidget(pidWidget);
    pidScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    pidScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pidScrollArea->setWidgetResizable(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    glayout->addWidget(splitter);
    splitter->addWidget(pidScrollArea);
    splitter->addWidget(_dataLoggerChartsWidget);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(_toolBar);
    vBoxLayout->addWidget(splitter);
    vBoxLayout->setMargin(2);
    setLayout(vBoxLayout);
}

void PidWidget::statusNodeChanged(Node::Status status)
{
    if (status != Node::STOPPED)
    {
        this->setEnabled(true);
    }
    else
    {
        this->setEnabled(false);
    }
}
