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
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"
#include "canopen/indexWidget/indexcheckbox.h"
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
    return _nodeProfile402->node();
}

QString PidWidget::title() const
{
    switch (_modePid)
    {
    case PidWidget::MODE_PID_NONE:
        return tr("None");

    case PidWidget::MODE_PID_TORQUE:
        return tr("Torque PID");

    case PidWidget::MODE_PID_VELOCITY:
        return tr("Velocity PID");

    case PidWidget::MODE_PID_POSITION:
        return tr("Position PID");
    }
    return QString();
}

void PidWidget::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    if (axis > 8)
    {
        return;
    }
    _dataLogger->removeAllData();

    if (node->profiles().isEmpty())
    {
        return;
    }

    _axis = axis;
    _nodeProfile402 = dynamic_cast<NodeProfile402 *>(node->profiles()[axis]);

    connect(node, &Node::statusChanged, this, &PidWidget::statusNodeChanged);
    setIMode();

    _dSpinBox->setNode(node);
    _pSpinBox->setNode(node);
    _iSpinBox->setNode(node);
    _inputLabel->setNode(node);
    _errorLabel->setNode(node);
    _integratorLabel->setNode(node);
    _outputLabel->setNode(node);
    _minSpinBox->setNode(node);
    _maxSpinBox->setNode(node);
    _thresholdSpinBox->setNode(node);
    _freqDividerSpinBox->setNode(node);
    _antiReverseCheckBox->setNode(node);
    _directCtrlCheckBox->setNode(node);
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
        _readStatusTimer.start();
    }
    else
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
        _dataLogger->stop();
        _readStatusTimer.stop();
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
    NodeObjectId target_ObjId;
    NodeObjectId pidInputStatus_ObjId;
    NodeObjectId pidErrorStatus_ObjId;
    NodeObjectId pidIntegratorStatus_ObjId;
    NodeObjectId pidOutputStatus_ObjId;

    if (!_nodeProfile402)
    {
        return;
    }

    IndexDb402::OdMode402 odMode402 = IndexDb402::MODE402_TORQUE;

    switch (_modePid)
    {
    case MODE_PID_NONE:
        break;

    case MODE_PID_TORQUE:
        _actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, _axis);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, _axis);
        _pidGroupBox->setTitle(tr("Torque PID configuration"));
        _pidStatusGroupBox->setTitle(tr("Torque PID status"));
        _pidTestGroupBox->setTitle(tr("Torque PID test"));
        _dataLoggerWidget->setTitle(tr("Node %1 axis %2 torque PID").arg(_nodeProfile402->nodeId()).arg(_axis));
        odMode402 = IndexDb402::MODE402_TORQUE;
        break;

    case MODE_PID_VELOCITY:
        _actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE, _axis);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND, _axis);
        _pidGroupBox->setTitle(tr("Velocity PID configuration"));
        _pidStatusGroupBox->setTitle(tr("Velocity PID status"));
        _pidTestGroupBox->setTitle(tr("Velocity PID test"));
        _dataLoggerWidget->setTitle(tr("Node %1 axis %2 velocity PID").arg(_nodeProfile402->nodeId()).arg(_axis));
        odMode402 = IndexDb402::MODE402_VELOCITY;
        break;

    case MODE_PID_POSITION:
        _actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, _axis);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, _axis);
        _pidGroupBox->setTitle(tr("Position PID configuration"));
        _pidStatusGroupBox->setTitle(tr("Position PID status"));
        _pidTestGroupBox->setTitle(tr("Position PID test"));
        _dataLoggerWidget->setTitle(tr("Node %1 axis %2 position PID").arg(_nodeProfile402->nodeId()).arg(_axis));
        odMode402 = IndexDb402::MODE402_POSITION;
        break;
    }

    _pSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_P, _axis, odMode402));
    _iSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_I, _axis, odMode402));
    _dSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_D, _axis, odMode402));
    _minSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_MIN, _axis, odMode402));
    _maxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_MAX, _axis, odMode402));
    _thresholdSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_THRESHOLD, _axis, odMode402));
    _freqDividerSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_FREQDIVIDER, _axis, odMode402));
    _antiReverseCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_CONFIGBIT, _axis, odMode402));
    _directCtrlCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PID_CONFIGBIT, _axis, odMode402));

    pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PID_INPUT, _axis, odMode402);
    pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PID_ERROR, _axis, odMode402);
    pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PID_INTEGRATOR, _axis, odMode402);
    pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PID_OUTPUT, _axis, odMode402);
    _inputLabel->setObjId(pidInputStatus_ObjId);
    _errorLabel->setObjId(pidErrorStatus_ObjId);
    _integratorLabel->setObjId(pidIntegratorStatus_ObjId);
    _outputLabel->setObjId(pidOutputStatus_ObjId);

    // Datalogger
    pidInputStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidErrorStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidIntegratorStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidOutputStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    target_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    _dataLogger->removeAllData();
    _dataLogger->addData(pidInputStatus_ObjId);
    _dataLogger->addData(pidErrorStatus_ObjId);
    _dataLogger->addData(target_ObjId);
}

void PidWidget::screenshotSave()
{
    QPixmap pixmap = _dataLoggerWidget->grab();
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
    case NodeProfile402::DTY:
        _modePid = ModePid::MODE_PID_NONE;
        break;

    case NodeProfile402::CST:
    case NodeProfile402::CSTCA:
    case NodeProfile402::TQ:
        _modePid = ModePid::MODE_PID_TORQUE;
        _nodeProfile402->setTarget(_firstTargetSpinBox->value());
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        break;

    case NodeProfile402::CSV:
    case NodeProfile402::VL:
    case NodeProfile402::PV:
        _modePid = ModePid::MODE_PID_VELOCITY;
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        _nodeProfile402->setTarget(_firstTargetSpinBox->value());
        break;

    case NodeProfile402::PP:
    case NodeProfile402::IP:
    case NodeProfile402::CSP:
    {
        _modePid = ModePid::MODE_PID_POSITION;
        int actualPosition = node()->nodeOd()->value(_actualValue_ObjId).toInt();
        _nodeProfile402->setTarget((_firstTargetSpinBox->value() - actualPosition) + actualPosition);
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        break;
    }
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

    case MODE_PID_TORQUE:
        _nodeProfile402->setTarget(_secondTargetSpinBox->value());
        _timer.start(_windowSecondTargetSpinBox->value());
        break;

    case MODE_PID_VELOCITY:
        _nodeProfile402->setTarget(_secondTargetSpinBox->value());
        _timer.start(_windowSecondTargetSpinBox->value());
        break;

    case MODE_PID_POSITION:
    {
        int actualPosition = node()->nodeOd()->value(_actualValue_ObjId).toInt();
        _nodeProfile402->setTarget((_secondTargetSpinBox->value() - actualPosition) + actualPosition);
        _timer.start(_windowSecondTargetSpinBox->value());
        break;
    }
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

    case MODE_PID_TORQUE:
        _nodeProfile402->setTarget(0);
        break;

    case MODE_PID_VELOCITY:
        _nodeProfile402->setTarget(0);
        break;

    case MODE_PID_POSITION:
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
    node()->readObject(_actualValue_ObjId);
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
    _antiReverseCheckBox->readObject();
    readStatus();
}

void PidWidget::createWidgets()
{
    _dataLogger = new DataLogger();

    QWidget *pidWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(pidWidget);
    actionLayout->setContentsMargins(0, 0, 4, 0);
    actionLayout->setSpacing(0);

    _pidGroupBox = createPIDConfigWidgets();
    actionLayout->addWidget(_pidGroupBox);

    _pidStatusGroupBox = createPIDStatusWidgets();
    actionLayout->addWidget(_pidStatusGroupBox);

    _pidTestGroupBox = createPIDTestWidgets();
    actionLayout->addWidget(_pidTestGroupBox);

    QScrollArea *pidScrollArea = new QScrollArea;
    pidScrollArea->setWidget(pidWidget);
    pidScrollArea->setWidgetResizable(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter {background: #19232D;}");
    splitter->addWidget(pidScrollArea);

    QWidget *widgetLogger = new QWidget();
    QVBoxLayout *layoutLogger = new QVBoxLayout();
    layoutLogger->setContentsMargins(5, 4, 0, 3);
    _dataLoggerWidget = new DataLoggerWidget(_dataLogger, Qt::Vertical);
    _dataLoggerWidget->setType(DataLoggerWidget::InternalType);
    layoutLogger->addWidget(_dataLoggerWidget);
    widgetLogger->setLayout(layoutLogger);
    splitter->addWidget(widgetLogger);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2 ,2);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addWidget(splitter);
    setLayout(vBoxLayout);
}

QToolBar *PidWidget::createToolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Data logger commands"));
    toolBar->setIconSize(QSize(20, 20));

    // start stop
    _startStopAction = toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &PidWidget::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });

    // clear
    QAction *action;
    action = toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _dataLogger, &DataLogger::clear);

    toolBar->addSeparator();

    // read all action
    QAction * readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &PidWidget::readAllObject);

    return toolBar;
}

QGroupBox *PidWidget::createPIDConfigWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("PID"));
    QFormLayout *formLayout = new QFormLayout();

    _pSpinBox = new IndexSpinBox();
    _pSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&P:"), _pSpinBox);

    _iSpinBox = new IndexSpinBox();
    _iSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&I:"), _iSpinBox);

    _dSpinBox = new IndexSpinBox();
    _dSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&D:"), _dSpinBox);

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
    QLabel *errorRangeLabel = new QLabel(tr("&Min max:"));
    errorRangeLabel->setBuddy(errorRangeLabel);
    formLayout->addRow(errorRangeLabel, minMaxlayout);

    _thresholdSpinBox = new IndexSpinBox();
    _thresholdSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Threshold:"), _thresholdSpinBox);

    _freqDividerSpinBox = new IndexSpinBox();
    formLayout->addRow(tr("&Frequency divider:"), _freqDividerSpinBox);

    _antiReverseCheckBox = new IndexCheckBox();
    _antiReverseCheckBox->setBitMask(1);
    formLayout->addRow(tr("&Anti reverse:"), _antiReverseCheckBox);

    _directCtrlCheckBox = new IndexCheckBox();
    _directCtrlCheckBox->setBitMask(1 << 8);
    formLayout->addRow(tr("&Direct control:"), _directCtrlCheckBox);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *PidWidget::createPIDStatusWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("PID status"));
    QFormLayout *formLayout = new QFormLayout();

    _inputLabel = new IndexLabel();
    _inputLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Input:"), _inputLabel);

    _errorLabel = new IndexLabel();
    _errorLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Error:"), _errorLabel);

    _integratorLabel = new IndexLabel();
    _integratorLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Integrator:"), _integratorLabel);

    _outputLabel = new IndexLabel();
    _outputLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Output:"), _outputLabel);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *PidWidget::createPIDTestWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("PID test"));
    QFormLayout *formLayout = new QFormLayout();

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
    formLayout->addRow(tr("First Target:"), firstTargetLayout);

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
    formLayout->addRow(tr("Second Target:"), secondTargetLayout);

    _stopDataLoggerSpinBox = new QSpinBox();
    _stopDataLoggerSpinBox->setRange(10, 5000);
    _stopDataLoggerSpinBox->setValue(100);
    _stopDataLoggerSpinBox->setSuffix(" ms");
    formLayout->addRow(tr("&End time:"), _stopDataLoggerSpinBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    _savePushButton = new QPushButton(tr("&Screenshot logger"));
    formLayout->addRow(_savePushButton);
    connect(_savePushButton, &QPushButton::clicked, this, &PidWidget::screenshotSave);

    _stopTargetPushButton = new QPushButton(tr("Stop"));
    buttonLayout->addWidget(_stopTargetPushButton);
    connect(_stopTargetPushButton, &QPushButton::clicked, this, &PidWidget::stopMeasurement);

    _startTargetPushButton = new QPushButton(tr("Start"));
    buttonLayout->addWidget(_startTargetPushButton);
    connect(_startTargetPushButton, &QPushButton::clicked, this, &PidWidget::manageMeasurement);

    formLayout->addRow(buttonLayout);
    groupBox->setLayout(formLayout);
    return groupBox;
}

void PidWidget::statusNodeChanged(Node::Status status)
{
    if (status != Node::STOPPED)
    {
        setEnabled(true);
    }
    else
    {
        setEnabled(false);
    }
}
