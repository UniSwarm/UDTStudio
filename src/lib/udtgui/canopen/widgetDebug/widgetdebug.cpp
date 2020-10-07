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

#include "widgetdebug.h"

#include "services/services.h"
#include "indexdb402.h"

#include <QApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QFormLayout>
#include <QPushButton>

#include "canopen/datalogger/dataloggerwidget.h"

WidgetDebug::WidgetDebug(QWidget *parent)
    : QWidget(parent)
{
}

WidgetDebug::WidgetDebug(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    if ((node->profileNumber()) != 0x192)
    {
        return;
    }

    createWidgets();
    setCheckableStateMachine(2);
    setNode(node);
}

WidgetDebug::~WidgetDebug()
{
    unRegisterFullOd();
}

Node *WidgetDebug::node() const
{
    return _node;
}

void WidgetDebug::setNode(Node *node)
{
    if ((node->profileNumber()) != 0x192)
    {
        return;
    }

    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &WidgetDebug::updateData);
        }
    }

    setNodeInterrest(node);
    _node = node;

    if (_node)
    {
        setWindowTitle("402 : " + _node->name() + ", Status :" + _node->statusStr());

        _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
        _statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);

        _nodeProfile402 = static_cast<NodeProfile402 *>(_node->profiles()[0]);
        QList<NodeProfile402::Mode> modeList  = _nodeProfile402->modesSupported();
        _modeComboBox->clear();
        for (quint8 i = 0; i < modeList.size(); i++)
        {
            _listModeComboBox.append(modeList.at(i));
            _modeComboBox->insertItem(i, _nodeProfile402->modeStr(modeList.at(i)));
        }

        connect(_nodeProfile402, &NodeProfile402::modeChanged, this, &WidgetDebug::modeChanged);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &WidgetDebug::stateChanged);
        connect(_nodeProfile402, &NodeProfile402::isHalted, this, &WidgetDebug::isHalted);
        connect(_nodeProfile402, &NodeProfile402::eventHappened, this, &WidgetDebug::eventHappened);

        connect(_node, &Node::statusChanged, this, &WidgetDebug::updateData);

        _p402Option->setNode(_node);
        _p402vl->setNode(_node);
        _p402ip->setNode(_node);
        _p402tq->setNode(_node);
    }
}

void WidgetDebug::updateData()
{
    if (_node)
    {
        this->setWindowTitle("402 : " + _node->name() + ", Status :" + _node->statusStr());
        if (_node->status() == Node::STARTED)
        {
            _stackedWidget->setEnabled(true);
            _modeGroupBox->setEnabled(true);
            _stateMachineGroupBox->setEnabled(true);
            _controlWordGroupBox->setEnabled(true);
            _statusWordGroupBox->setEnabled(true);
        }
        else if ((_node->status() == Node::STOPPED) || (_node->status() == Node::PREOP))
        {
            _timer.stop();
            _stackedWidget->setEnabled(false);
            _nmtToolBar->setEnabled(true);
            _modeGroupBox->setEnabled(false);
            _stateMachineGroupBox->setEnabled(false);
            _controlWordGroupBox->setEnabled(false);
            _statusWordGroupBox->setEnabled(false);
        }
        modeChanged();
    }
}

void WidgetDebug::start()
{
    if (_node)
    {
        _node->sendStart();
        _stackedWidget->setEnabled(true);
        _timer.start(_logTimerSpinBox->value());
        _modeGroupBox->setEnabled(true);
        _stateMachineGroupBox->setEnabled(true);
        _controlWordGroupBox->setEnabled(true);
        _statusWordGroupBox->setEnabled(true);
    }
}

void WidgetDebug::stop()
{
    if (_node)
    {
        _haltPushButton->setChecked(false);
        _stackedWidget->setEnabled(false);
        _nmtToolBar->setEnabled(true);
        _modeGroupBox->setEnabled(false);
        _stateMachineGroupBox->setEnabled(false);
        _controlWordGroupBox->setEnabled(false);
        _statusWordGroupBox->setEnabled(false);

        _timer.stop();
    }
}

void WidgetDebug::gotoStateOEClicked()
{
    _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
}

void WidgetDebug::setTimer(int ms)
{
    _timer.start(ms);
}

void WidgetDebug::readData()
{
    if (_node)
    {
        _node->readObject(_controlWordObjectId);
        _node->readObject(_statusWordObjectId);

        NodeProfile402::Mode mode = _nodeProfile402->actualMode();
        if (mode == NodeProfile402::IP)
        {
            _p402ip->readData();
        }
        else if (mode == NodeProfile402::VL)
        {
            _p402vl->readData();
        }
        else if (mode == NodeProfile402::TQ)
        {
            _p402tq->readData();
        }
    }
}

void WidgetDebug::modeChanged()
{
    NodeProfile402::Mode mode = _nodeProfile402->actualMode();
    if (mode == NodeProfile402::IP)
    {
        _p402ip->updateData();
        _stackedWidget->setCurrentWidget(_p402ip);
    }
    else if (mode == NodeProfile402::VL)
    {
        _p402ip->stop();
        _p402vl->updateData();
        _stackedWidget->setCurrentWidget(_p402vl);
    }
    else if (mode == NodeProfile402::TQ)
    {
        _p402tq->updateData();
        _stackedWidget->setCurrentWidget(_p402tq);

    }
    int m = _listModeComboBox.indexOf(mode);
    _modeComboBox->setCurrentIndex(m);
}

void WidgetDebug::stateChanged()
{
    NodeProfile402::State402 state = _nodeProfile402->currentState();

    _controlWordLabel->setText(QLatin1String("0x") + QString::number(_node->nodeOd()->value(_controlWordObjectId).toUInt(), 16).toUpper().rightJustified(4, '0'));
    _statusWordRawLabel->setText(QLatin1String("0x") + QString::number(_node->nodeOd()->value(_statusWordObjectId).toUInt(), 16).toUpper().rightJustified(4, '0'));

    if (state == NodeProfile402::STATE_NotReadyToSwitchOn)
    {
        _statusWordLabel->setText(tr("NotReadyToSwitchOn"));
        setCheckableStateMachine(STATE_NotReadyToSwitchOn);
    }
    if (state ==  NodeProfile402::STATE_SwitchOnDisabled)
    {
        _statusWordLabel->setText(tr("SwitchOnDisabled"));
        setCheckableStateMachine(STATE_SwitchOnDisabled);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state ==  NodeProfile402::STATE_ReadyToSwitchOn)
    {
        _statusWordLabel->setText(tr("ReadyToSwitchOn"));
        setCheckableStateMachine(STATE_ReadyToSwitchOn);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state ==  NodeProfile402::STATE_SwitchedOn)
    {
        _statusWordLabel->setText(tr("SwitchedOn"));
        setCheckableStateMachine(STATE_SwitchedOn);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state ==  NodeProfile402::STATE_OperationEnabled)
    {
        _statusWordLabel->setText(tr("OperationEnabled"));
        setCheckableStateMachine(STATE_OperationEnabled);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
        _haltPushButton->setEnabled(true);
        _haltPushButton->setCheckable(true);
    }
    if (state ==  NodeProfile402::STATE_QuickStopActive)
    {
        _statusWordLabel->setText(tr("QuickStopActive"));
        setCheckableStateMachine(STATE_QuickStopActive);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state ==  NodeProfile402::STATE_FaultReactionActive)
    {
        _statusWordLabel->setText(tr("FaultReactionActive"));
        setCheckableStateMachine(STATE_FaultReactionActive);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state ==  NodeProfile402::STATE_Fault)
    {
        _statusWordLabel->setText(tr("Fault"));
        setCheckableStateMachine(STATE_Fault);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    update();
}

void WidgetDebug::isHalted(bool state)
{
    _haltPushButton->setChecked(state);
}

void WidgetDebug::eventHappened(quint8 event)
{
    _informationLabel->setText(tr("False"));
    _warningLabel->setText(tr("False"));

    QString text;
    if (event & NodeProfile402::VoltageEnabled)
    {
        text = _nodeProfile402->event402Str(NodeProfile402::VoltageEnabled);
    }

    if (event & NodeProfile402::Remote)
    {
        if (!text.isEmpty())
        {
            text.append(", ");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::Remote));
    }
    if (event & NodeProfile402::TargetReached)
    {
        if (!text.isEmpty())
        {
            text.append(", ");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::TargetReached));
    }

    _informationLabel->clear();
    _informationLabel->setText(text);

    text.clear();
    if (event & NodeProfile402::InternalLimitActive)
    {
        text.append(_nodeProfile402->event402Str(NodeProfile402::InternalLimitActive));
    }

    if (event & NodeProfile402::Warning)
    {
        if (!text.isEmpty())
        {
            text.append(", ");
        }
        text.append(_nodeProfile402->event402Str(NodeProfile402::Warning));
    }
    quint16 mode = static_cast<quint16>(_nodeProfile402->actualMode());
    if (mode == 7)
    {
        if (event & NodeProfile402::FollowingError)
        {
            if (!text.isEmpty())
            {
                text.append(", ");
            }
            text.append(_nodeProfile402->event402Str(NodeProfile402::FollowingError));
        }
    }
    _warningLabel->clear();
    _warningLabel->setText(text);
}

void WidgetDebug::displayOption402()
{
    if (_stackedWidget->currentWidget() == _p402Option)
    {
        modeChanged();
    }
    else
    {
        _stackedWidget->setCurrentWidget(_p402Option);
    }
}
void WidgetDebug::modeIndexChanged(int id)
{
    if (!_node)
    {
        return;
    }

    if (id == -1)
    {
        return;
    }
    _nodeProfile402->setMode(_listModeComboBox.at(id));
}

void WidgetDebug::stateMachineClicked(int id)
{
    if (!_node)
    {
        return;
    }
    _nodeProfile402->goToState(static_cast<NodeProfile402::State402>(id));
}

void WidgetDebug::haltClicked()
{
    _nodeProfile402->toggleHalt();
}

void WidgetDebug::setCheckableStateMachine(int id)
{
    for (int i = 1; i <= 8; i++)
    {
        _stateMachineGroup->button(i)->setCheckable(false);
    }
    _stateMachineGroup->button(id)->setCheckable(true);
    _stateMachineGroup->button(id)->setChecked(true);
}

void WidgetDebug::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // toolbar nmt
    _nmtToolBar = new QToolBar(tr("Node commands"));
    QActionGroup *groupNmt = new QActionGroup(this);
    groupNmt->setExclusive(true);
    QAction *action;
    action = groupNmt->addAction(tr("Start"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-play.png"));
    action->setStatusTip(tr("Request node to go in started mode"));
    connect(action, &QAction::triggered, this, &WidgetDebug::start);

    action = groupNmt->addAction(tr("Stop"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    action->setStatusTip(tr("Request node to go in stop mode"));
    connect(action, &QAction::triggered, this, &WidgetDebug::stop);

    _nmtToolBar->addActions(groupNmt->actions());
    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(500);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setTimer(i); });
    connect(&_timer, &QTimer::timeout, this, &WidgetDebug::readData);
    _nmtToolBar->addWidget(_logTimerSpinBox);

    _nmtToolBar->addSeparator();

    QAction *option402 = new QAction();
    option402->setCheckable(true);
    option402->setIcon(QIcon(":/icons/img/icons8-settings.png"));
    option402->setToolTip(tr("Option code"));
    connect(option402, &QAction::triggered, this, &WidgetDebug::displayOption402);
    _nmtToolBar->addAction(option402);

    // Group Box Mode
    _modeGroupBox = new QGroupBox(tr("Mode"));
    QFormLayout *modeLayout = new QFormLayout();
    _modeComboBox = new QComboBox();
    modeLayout->addRow(tr("Modes of operation (0x6060) :"), _modeComboBox);
    _modeGroupBox->setLayout(modeLayout);
    layout->addWidget(_modeGroupBox);
    connect(_modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int id) { modeIndexChanged(id); });
    // End Group Box State Machine

    // Group Box State Machine
    _stateMachineGroupBox = new QGroupBox(tr("State Machine"));
    _stateMachineGroupBox->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    QFormLayout *stateMachineLayoutGroupBox = new QFormLayout();

    _stateMachineGroup = new QButtonGroup(this);
    _stateMachineGroup->setExclusive(true);

    QPushButton *stateNotReadyToSwitchOnPushButton = new QPushButton(tr("1_Not ready to switch on"));
    stateNotReadyToSwitchOnPushButton->setEnabled(false);
    stateMachineLayoutGroupBox->addRow(stateNotReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateNotReadyToSwitchOnPushButton, STATE_NotReadyToSwitchOn);
    QPushButton *stateSwitchOnDisabledPushButton = new QPushButton(tr("2_Switch on disabled"));
    stateMachineLayoutGroupBox->addRow(stateSwitchOnDisabledPushButton);
    _stateMachineGroup->addButton(stateSwitchOnDisabledPushButton, STATE_SwitchOnDisabled);
    QPushButton *stateReadyToSwitchOnPushButton = new QPushButton(tr("3_Ready to switch on"));
    stateMachineLayoutGroupBox->addRow(stateReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateReadyToSwitchOnPushButton, STATE_ReadyToSwitchOn);
    QPushButton *stateSwitchedOnPushButton = new QPushButton(tr("4_Switched on"));
    stateMachineLayoutGroupBox->addRow(stateSwitchedOnPushButton);
    _stateMachineGroup->addButton(stateSwitchedOnPushButton, STATE_SwitchedOn);
    QPushButton *stateOperationEnabledPushButton = new QPushButton(tr("5_Operation enabled"));
    stateMachineLayoutGroupBox->addRow(stateOperationEnabledPushButton);
    _stateMachineGroup->addButton(stateOperationEnabledPushButton, STATE_OperationEnabled);
    QPushButton *stateQuickStopActivePushButton = new QPushButton(tr("6_Quick stop active"));
    stateMachineLayoutGroupBox->addRow(stateQuickStopActivePushButton);
    _stateMachineGroup->addButton(stateQuickStopActivePushButton, STATE_QuickStopActive);
    QPushButton *stateFaultReactionActivePushButton = new QPushButton(tr("7_Fault reaction active"));
    stateFaultReactionActivePushButton->setEnabled(false);
    stateMachineLayoutGroupBox->addRow(stateFaultReactionActivePushButton);
    _stateMachineGroup->addButton(stateFaultReactionActivePushButton, STATE_FaultReactionActive);
    QPushButton *stateFaultPushButton = new QPushButton(tr("8_Fault"));
    stateFaultPushButton->setEnabled(false);
    stateMachineLayoutGroupBox->addRow(stateFaultPushButton);
    _stateMachineGroup->addButton(stateFaultPushButton, STATE_Fault);

    connect(_stateMachineGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { stateMachineClicked(id); });
    _stateMachineGroupBox->setLayout(stateMachineLayoutGroupBox);
    // END Group Box State Machine

    // Group Box Control Word
    _controlWordGroupBox = new QGroupBox(tr("Control Word (0x6040)"));
    QFormLayout *controlWordLayout = new QFormLayout();

    _haltPushButton = new QPushButton(tr("Halt"));
    _haltPushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    _haltPushButton->setEnabled(false);
    controlWordLayout->addRow(_haltPushButton);
    _controlWordGroupBox->setLayout(controlWordLayout);
    connect(_haltPushButton, &QPushButton::clicked, this, &WidgetDebug::haltClicked);

    _controlWordLabel = new QLabel("0x0000");
    controlWordLayout->addRow(tr("ControlWord sended:"), _controlWordLabel);

    QPushButton *_gotoOEPushButton = new QPushButton(tr("Operation enabled quickly"));
    controlWordLayout->addRow(_gotoOEPushButton);
    connect(_gotoOEPushButton, &QPushButton::clicked, this, &WidgetDebug::gotoStateOEClicked);

    _controlWordGroupBox->setLayout(controlWordLayout);
    // END Group Box Control Word

    // Group Box Status Word
    _statusWordGroupBox = new QGroupBox(tr("Status Word (0x6041)"));
    QFormLayout *statusWordLayout = new QFormLayout();

    _statusWordRawLabel = new QLabel();
    statusWordLayout->addRow(tr("StatusWord raw:"), _statusWordRawLabel);
    _statusWordLabel = new QLabel();
    statusWordLayout->addRow(tr("StatusWord State:"), _statusWordLabel);

    _informationLabel = new QLabel();
    statusWordLayout->addRow(tr("Information :"), _informationLabel);
    _warningLabel = new QLabel();
    _warningLabel->setStyleSheet("QLabel { color : red; }");
    statusWordLayout->addRow(tr("Warning :"), _warningLabel);
    _statusWordGroupBox->setLayout(statusWordLayout);
    // END Group Box Status Word

    layout->addWidget(_stateMachineGroupBox);
    layout->addWidget(_controlWordGroupBox);
    layout->addWidget(_statusWordGroupBox);

    _p402Option = new P402OptionWidget();
    _p402vl = new P402VlWidget();
    _p402ip = new P402IpWidget();
    _p402tq = new P402TqWidget();
    _stackedWidget = new QStackedWidget;
    _stackedWidget->addWidget(_p402Option);
    _stackedWidget->addWidget(_p402vl);
    _stackedWidget->addWidget(_p402ip);
    _stackedWidget->addWidget(_p402tq);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(0);
    hBoxLayout->addLayout(layout);
    hBoxLayout->addWidget(_stackedWidget);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(_nmtToolBar);
    vBoxLayout->addLayout(hBoxLayout);
    setLayout(vBoxLayout);
}

void WidgetDebug::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)

    if (!_node)
    {
        return;
    }
}

void WidgetDebug::closeEvent(QCloseEvent *event)
{
    if (event->type() == QEvent::Close)
    {
        _timer.stop();
    }
}

void WidgetDebug::showEvent(QShowEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        if (_node->status() ==  Node::STARTED)
        {
            _timer.start(_logTimerSpinBox->value());
        }

        NodeProfile402::Mode mode = _nodeProfile402->actualMode();
        if (mode == NodeProfile402::IP)
        {
            _p402ip->updateData();
        }
        else if (mode == NodeProfile402::VL)
        {
            _p402vl->updateData();
        }
        else if (mode == NodeProfile402::TQ)
        {
            _p402tq->updateData();
        }
        _p402Option->updateData();
        updateData();
    }
}
