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
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"
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
    _node = nullptr;
    _axis = 0;
    _nodeProfile402 = nullptr;

    _created = false;

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
    _node = node;
    if (node == nullptr)
    {
        return;
    }
    if (axis > 8)
    {
        return;
    }

    if (node->profiles().isEmpty())
    {
        return;
    }

    _axis = axis;

    setIMode();
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
        _readStatusTimer.start();
    }
    else
    {
        _readStatusTimer.stop();
    }
}

void PidWidget::setLogTimer(int ms)
{
    if (_dataLogger->isStarted())
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
    NodeObjectId pidTargetStatus_ObjId;

    if (_node == nullptr || !_created)
    {
        return;
    }

    _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[_axis]);
    connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &PidWidget::updateState);
    connect(_node, &Node::statusChanged, this, &PidWidget::statusNodeChanged);

    IndexDb402::OdMode402 odMode402 = IndexDb402::MODE402_TORQUE;

    switch (_modePid)
    {
        case MODE_PID_NONE:
            break;

        case MODE_PID_TORQUE:
            _actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, _axis);
            target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, _axis);
            _pidConfigGroupBox->setTitle(tr("Torque PID configuration"));
            _pidStatusGroupBox->setTitle(tr("Torque PID status"));
            _pidTestGroupBox->setTitle(tr("Torque PID test"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 torque PID").arg(_nodeProfile402->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_TORQUE;
            _directCtrlCheckBox->setEnabled(false);
            _directCtrlLabel->setEnabled(false);
            break;

        case MODE_PID_VELOCITY:
            _actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE, _axis);
            target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND, _axis);
            _pidConfigGroupBox->setTitle(tr("Velocity PID configuration"));
            _pidStatusGroupBox->setTitle(tr("Velocity PID status"));
            _pidTestGroupBox->setTitle(tr("Velocity PID test"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 velocity PID").arg(_nodeProfile402->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_VELOCITY;
            break;

        case MODE_PID_POSITION:
            _actualValue_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, _axis);
            target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, _axis);
            _pidConfigGroupBox->setTitle(tr("Position PID configuration"));
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
    pidTargetStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PID_TARGET, _axis, odMode402);
    _inputLabel->setObjId(pidInputStatus_ObjId);
    _errorLabel->setObjId(pidErrorStatus_ObjId);
    _integratorLabel->setObjId(pidIntegratorStatus_ObjId);
    _outputLabel->setObjId(pidOutputStatus_ObjId);
    _targetLabel->setObjId(pidTargetStatus_ObjId);

    _modeComboBox->clear();
    QList<NodeProfile402::OperationMode> supportedModeList = _nodeProfile402->modesSupportedByType(odMode402);
    for (NodeProfile402::OperationMode mode : supportedModeList)
    {
        _modeComboBox->addItem(_nodeProfile402->modeStr(mode), QVariant(static_cast<int>(mode)));
    }

    // Datalogger
    pidInputStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidErrorStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidIntegratorStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidOutputStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    pidTargetStatus_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    target_ObjId.setBusIdNodeId(node()->busId(), node()->nodeId());
    _dataLogger->removeAllData();
    _dataLogger->addData(pidInputStatus_ObjId);
    _dataLogger->addData(pidErrorStatus_ObjId);
    _dataLogger->addData(target_ObjId);

    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(_node);
    }
}

void PidWidget::changeMode402()
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    connect(_nodeProfile402, &NodeProfile402::modeChanged, this, &PidWidget::updateMode);

    switch (_modePid)
    {
        case MODE_PID_NONE:
            break;

        case MODE_PID_TORQUE:
            if (_nodeProfile402->actualMode() != NodeProfile402::TQ)
            {
                NodeProfile402::OperationMode mode = static_cast<NodeProfile402::OperationMode>(_modeComboBox->currentData().toInt());
                _nodeProfile402->setMode(mode);
            }
            else
            {
                updateMode(_nodeProfile402->actualMode());
            }
            break;

        case MODE_PID_VELOCITY:
            _nodeProfile402->setDefaultValueOfMode();
            if (_nodeProfile402->actualMode() != NodeProfile402::VL)
            {
                NodeProfile402::OperationMode mode = static_cast<NodeProfile402::OperationMode>(_modeComboBox->currentData().toInt());
                _nodeProfile402->setMode(mode);
            }
            else
            {
                updateMode(_nodeProfile402->actualMode());
            }
            break;

        case MODE_PID_POSITION:
            _nodeProfile402->setDefaultValueOfMode();
            if (_nodeProfile402->actualMode() != NodeProfile402::PP || _nodeProfile402->actualMode() != NodeProfile402::IP
                || _nodeProfile402->actualMode() != NodeProfile402::CP)
            {
                NodeProfile402::OperationMode mode = static_cast<NodeProfile402::OperationMode>(_modeComboBox->currentData().toInt());
                _nodeProfile402->setMode(mode);
            }
            else
            {
                updateMode(_nodeProfile402->actualMode());
            }
            break;
    }

    _savePushButton->setEnabled(true);
    _startTargetPushButton->setEnabled(false);
}

void PidWidget::updateMode(NodeProfile402::OperationMode mode)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    switch (mode)
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
        case NodeProfile402::CP:
        {
            _modePid = ModePid::MODE_PID_POSITION;
            int actualPosition = node()->nodeOd()->value(_actualValue_ObjId).toInt();
            _nodeProfile402->setTarget((_firstTargetSpinBox->value() - actualPosition) + actualPosition);
            _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
            break;
        }
    }
    _timerTest.start(_windowFirstTargetSpinBox->value());
}

void PidWidget::manageMeasurement()
{
    switch (_state)
    {
        case PidWidget::NONE:
            _dataLogger->clear();
            _dataLogger->start(10);
            _timerTest.start(10);
            _readStatusTimer.start(10);
            _state = LAUCH_DATALOGGER;
            break;

        case PidWidget::LAUCH_DATALOGGER:
            _timerTest.stop();
            changeMode402();
            _state = LAUCH_FIRST_TARGET;
            break;

        case PidWidget::LAUCH_FIRST_TARGET:
            stopFirstMeasurement();
            _state = LAUCH_SECOND_TARGET;
            break;

        case PidWidget::LAUCH_SECOND_TARGET:
            stopSecondMeasurement();
            _timerTest.start(_stopDataLoggerSpinBox->value());
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
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    switch (_modePid)
    {
        case MODE_PID_NONE:
            break;

        case MODE_PID_TORQUE:
            _nodeProfile402->setTarget(_secondTargetSpinBox->value());
            _timerTest.start(_windowSecondTargetSpinBox->value());
            break;

        case MODE_PID_VELOCITY:
            _nodeProfile402->setTarget(_secondTargetSpinBox->value());
            _timerTest.start(_windowSecondTargetSpinBox->value());
            break;

        case MODE_PID_POSITION:
        {
            int actualPosition = node()->nodeOd()->value(_actualValue_ObjId).toInt();
            _nodeProfile402->setTarget((_secondTargetSpinBox->value() - actualPosition) + actualPosition);
            _timerTest.start(_windowSecondTargetSpinBox->value());
            break;
        }
    }
}

void PidWidget::stopSecondMeasurement()
{
    if (_nodeProfile402 == nullptr)
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
    _timerTest.stop();
    _state = NONE;
    _dataLogger->stop();
    _savePushButton->setEnabled(true);
    _startTargetPushButton->setEnabled(true);
    disconnect(_nodeProfile402, &NodeProfile402::modeChanged, this, &PidWidget::updateMode);
}

void PidWidget::readStatus()
{
    node()->readObject(_actualValue_ObjId);
    _inputLabel->readObject();
    _errorLabel->readObject();
    _integratorLabel->readObject();
    _outputLabel->readObject();
    _targetLabel->readObject();
}

void PidWidget::readAllObject()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void PidWidget::createWidgets()
{
    _dataLogger = new DataLogger();

    QWidget *pidWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(pidWidget);
    actionLayout->setContentsMargins(0, 0, 4, 0);
    actionLayout->setSpacing(0);

    _pidConfigGroupBox = createPIDConfigWidgets();
    actionLayout->addWidget(_pidConfigGroupBox);

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
    layoutLogger->setContentsMargins(4, 4, 0, 0);
    _dataLoggerWidget = new DataLoggerWidget(_dataLogger, Qt::Vertical);
    _dataLoggerWidget->setType(DataLoggerWidget::InternalType);
    _dataLoggerWidget->chartView()->setRollingTimeMs(10000);
    _dataLoggerWidget->chartView()->setRollingEnabled(true);
    layoutLogger->addWidget(_dataLoggerWidget);
    widgetLogger->setLayout(layoutLogger);
    splitter->addWidget(widgetLogger);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(0);
    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2, 2, 2, 0);
    toolBarLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addItem(toolBarLayout);
    vBoxLayout->addWidget(splitter);
    setLayout(vBoxLayout);

    connect(&_timerTest, &QTimer::timeout, this, &PidWidget::manageMeasurement);
    connect(&_readStatusTimer, &QTimer::timeout, this, &PidWidget::readStatus);

    _created = true;
}

QToolBar *PidWidget::createToolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Data logger commands"));
    toolBar->setIconSize(QSize(20, 20));

    // start stop
    toolBar->addAction(_dataLoggerWidget->managerWidget()->startStopAction());
    connect(_dataLogger, &DataLogger::startChanged, this, &PidWidget::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            [=](int i)
            {
                setLogTimer(i);
            });

    // clear
    QAction *action;
    action = toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _dataLogger, &DataLogger::clear);

    toolBar->addSeparator();

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-update.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &PidWidget::readAllObject);

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
    _lockAction->setStatusTip(tr("Editing of config parameters is not possible in OE mode, goto to SO to unlock"));
    connect(_lockAction, &QAction::triggered, this, &PidWidget::lockUnlockConfig);

    return toolBar;
}

QGroupBox *PidWidget::createPIDConfigWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("PID"));
    IndexFormLayout *formLayout = new IndexFormLayout();

    _pSpinBox = new IndexSpinBox();
    formLayout->addRow(tr("&P:"), _pSpinBox);
    _indexWidgets.append(_pSpinBox);

    _iSpinBox = new IndexSpinBox();
    formLayout->addRow(tr("&I:"), _iSpinBox);
    _indexWidgets.append(_iSpinBox);

    _dSpinBox = new IndexSpinBox();
    formLayout->addRow(tr("&D:"), _dSpinBox);
    _indexWidgets.append(_dSpinBox);

    formLayout->addItem(new QSpacerItem(0, 6));

    _minSpinBox = new IndexSpinBox();
    _indexWidgets.append(_minSpinBox);
    _maxSpinBox = new IndexSpinBox();
    _indexWidgets.append(_maxSpinBox);
    formLayout->addDualRow(tr("&Min - max:"), _minSpinBox, _maxSpinBox, tr("-"));

    _thresholdSpinBox = new IndexSpinBox();
    _thresholdSpinBox->setMinValue(0);
    formLayout->addRow(tr("&Threshold:"), _thresholdSpinBox);
    _indexWidgets.append(_thresholdSpinBox);

    _freqDividerSpinBox = new IndexSpinBox();
    _freqDividerSpinBox->setRangeValue(1, 1000);
    formLayout->addRow(tr("&Subsampling:"), _freqDividerSpinBox);
    _indexWidgets.append(_freqDividerSpinBox);

    _antiReverseCheckBox = new IndexCheckBox();
    _antiReverseCheckBox->setBitMask(1);
    formLayout->addRow(tr("&Anti reverse:"), _antiReverseCheckBox);
    _indexWidgets.append(_antiReverseCheckBox);

    _directCtrlCheckBox = new IndexCheckBox();
    _directCtrlCheckBox->setBitMask(1 << 8);
    _directCtrlLabel = new QLabel(tr("Direct &control:"));
    _directCtrlLabel->setBuddy(_directCtrlCheckBox);
    formLayout->addRow(_directCtrlLabel, _directCtrlCheckBox);
    _indexWidgets.append(_directCtrlCheckBox);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *PidWidget::createPIDStatusWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("PID status"));
    IndexFormLayout *formLayout = new IndexFormLayout();

    _targetLabel = new IndexLabel();
    formLayout->addRow(tr("Target:"), _targetLabel);
    _indexWidgets.append(_targetLabel);

    _inputLabel = new IndexLabel();
    formLayout->addRow(tr("Input sensor:"), _inputLabel);
    _indexWidgets.append(_inputLabel);

    _errorLabel = new IndexLabel();
    formLayout->addRow(tr("Error:"), _errorLabel);
    _indexWidgets.append(_errorLabel);

    _integratorLabel = new IndexLabel();
    formLayout->addRow(tr("Integrator:"), _integratorLabel);
    _indexWidgets.append(_integratorLabel);

    _outputLabel = new IndexLabel();
    formLayout->addRow(tr("Output:"), _outputLabel);
    _indexWidgets.append(_outputLabel);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *PidWidget::createPIDTestWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("PID test"));
    IndexFormLayout *formLayout = new IndexFormLayout();

    _modeComboBox = new QComboBox();
    formLayout->addRow(new QLabel(tr("Mode:")), _modeComboBox);

    QHBoxLayout *firstTargetLayout = new QHBoxLayout();
    firstTargetLayout->setSpacing(3);
    _firstTargetSpinBox = new QSpinBox();
    _firstTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _firstTargetSpinBox->setValue(50);
    _windowFirstTargetSpinBox = new QSpinBox();
    _windowFirstTargetSpinBox->setRange(10, 5000);
    _windowFirstTargetSpinBox->setValue(2000);
    _windowFirstTargetSpinBox->setSuffix(" ms");
    firstTargetLayout->addWidget(_firstTargetSpinBox);
    firstTargetLayout->addWidget(_windowFirstTargetSpinBox);
    formLayout->addRow(tr("First Target:"), firstTargetLayout);

    QHBoxLayout *secondTargetLayout = new QHBoxLayout();
    secondTargetLayout->setSpacing(3);
    _secondTargetSpinBox = new QSpinBox();
    _secondTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _secondTargetSpinBox->setValue(50);
    _windowSecondTargetSpinBox = new QSpinBox();
    _windowSecondTargetSpinBox->setRange(10, 5000);
    _windowSecondTargetSpinBox->setValue(2000);
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

void PidWidget::updateState()
{
    _lockAction->blockSignals(true);
    if (_nodeProfile402->currentState() == NodeProfile402::STATE_OperationEnabled)
    {
        _pidConfigGroupBox->setEnabled(false);
        _lockAction->setChecked(true);
    }
    else
    {
        _pidConfigGroupBox->setEnabled(true);
        _lockAction->setChecked(false);
    }
    _lockAction->blockSignals(false);
    _lockAction->setEnabled((_nodeProfile402->status() == NodeProfile402::NODEPROFILE_STARTED));
}

void PidWidget::lockUnlockConfig()
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

void PidWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    if (!_created)
    {
        createWidgets();
        setIMode();
    }
}
