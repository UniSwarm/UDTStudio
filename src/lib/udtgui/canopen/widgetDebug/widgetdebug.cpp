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

#include "widgetdebug.h"

#include "indexdb402.h"
#include "services/services.h"
#include "canopen/widget/indexlabel.h"

#include <QApplication>
#include <QButtonGroup>
#include <QDebug>
#include <QFormLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>

#include "canopen/datalogger/dataloggerwidget.h"

WidgetDebug::WidgetDebug(Node *node, uint8_t axis, QWidget *parent)
    : QWidget(parent)
    , _node(node)
    , _axis(axis)
{
    if (!_node)
    {
        return;
    }

    if ((node->profileNumber()) != 0x192)
    {
        return;
    }

    setNode(node, axis);
}

WidgetDebug::~WidgetDebug()
{
    unRegisterFullOd();
}

Node *WidgetDebug::node() const
{
    return _node;
}

QString WidgetDebug::title() const
{
    return QString("Motion Control");
}

void WidgetDebug::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    _node = node;

    if ((node->profileNumber() != 0x192) || node->profiles().isEmpty())
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    setNodeInterrest(node);

    if (_node)
    {
        _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, axis);
        _statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD, axis);

        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);

        createWidgets();
        setCheckableStateMachine(2);

        updateModeComboBox();

        connect(_modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int id) { modeIndexChanged(id); });
        connect(_nodeProfile402, &NodeProfile402::modeChanged, this, &WidgetDebug::modeChanged);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &WidgetDebug::stateChanged);
        connect(_nodeProfile402, &NodeProfile402::isHalted, this, &WidgetDebug::isHalted);
        connect(_nodeProfile402, &NodeProfile402::eventHappened, this, &WidgetDebug::eventHappened);
        connect(_node, &Node::statusChanged, this, &WidgetDebug::statusNodeChanged);

        _p402Option->setNode(_node, axis);
        _p402vl->setNode(_node, axis);
        _p402ip->setNode(_node, axis);
        _p402tq->setNode(_node, axis);
        _p402pp->setNode(_node, axis);

        statusNodeChanged();
        modeChanged(_axis, _nodeProfile402->actualMode());
        stateChanged();

        _controlWordLabel->setNode(_node);
        _statusWordLabel->setNode(_node);
        _controlWordLabel->setObjId(_controlWordObjectId);
        _statusWordLabel->setObjId(_statusWordObjectId);
    }
}

void WidgetDebug::statusNodeChanged()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            modeChanged(_axis, _nodeProfile402->actualMode());
            _stackedWidget->setEnabled(false);
            _modeGroupBox->setEnabled(true);
            _stateMachineGroupBox->setEnabled(false);
            _statusWordGroupBox->setEnabled(false);
        }
        else
        {
            toggleStartLogger(false);
        }
    }
}

void WidgetDebug::gotoStateOEClicked()
{
    toggleStartLogger(true);
    _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
}

void WidgetDebug::toggleStartLogger(bool start)
{
    if (!_node)
    {
        return;
    }

    if (start)
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-stop.png"));

        if (_node->status() != Node::STARTED)
        {
            _node->sendStart();
        }
        _timer.start(_logTimerSpinBox->value());

        _stackedWidget->setEnabled(true);
        _modeGroupBox->setEnabled(true);
        _stateMachineGroupBox->setEnabled(true);
        _statusWordGroupBox->setEnabled(true);
    }
    else
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));

        _timer.stop();

        _stackedWidget->setEnabled(false);
        _modeGroupBox->setEnabled(true);
        _stateMachineGroupBox->setEnabled(false);
        _statusWordGroupBox->setEnabled(false);
    }
}

void WidgetDebug::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _timer.start(ms);
    }
}

void WidgetDebug::readDataTimer()
{
    if (_node)
    {
        _node->readObject(_statusWordObjectId);
        _p402ip->readData();
        _p402vl->readData();
        _p402tq->readData();
        _p402pp->readData();
    }
}

void WidgetDebug::modeChanged(uint8_t axis, NodeProfile402::OperationMode modeNew)
{
    if (_axis != axis)
    {
        return;
    }
    // Patch because the widget Tarqet torque and velocity are not an IndexSpinbox so not read automaticaly
    _p402vl->reset();
    _p402tq->reset();

    if (modeNew == NodeProfile402::IP)
    {
        _stackedWidget->setCurrentWidget(_p402ip);
    }
    else if (modeNew == NodeProfile402::VL)
    {
        _p402ip->stop();
        _stackedWidget->setCurrentWidget(_p402vl);
    }
    else if (modeNew == NodeProfile402::TQ)
    {
        _p402ip->stop();
        _stackedWidget->setCurrentWidget(_p402tq);
    }
    else if (modeNew == NodeProfile402::PP)
    {
        _p402ip->stop();
        _stackedWidget->setCurrentWidget(_p402pp);
    }
    else
    {
        _p402ip->stop();
        _stackedWidget->setCurrentWidget(_p402Option);
    }
    int m = _listModeComboBox.indexOf(modeNew);
    _modeComboBox->setCurrentIndex(m);
    _modeComboBox->setEnabled(true);
    _modeLabel->clear();
}

void WidgetDebug::stateChanged()
{
    NodeProfile402::State402 state = _nodeProfile402->currentState();

    _controlWordLabel->setText(QLatin1String("0x") + QString::number(_node->nodeOd()->value(_controlWordObjectId).toUInt(), 16).toUpper().rightJustified(4, '0'));
    _statusWordLabel->setText(QLatin1String("0x") + QString::number(_node->nodeOd()->value(_statusWordObjectId).toUInt(), 16).toUpper().rightJustified(4, '0'));

    if (state == NodeProfile402::STATE_NotReadyToSwitchOn)
    {
        _statusWordStateLabel->setText(tr("NotReadyToSwitchOn"));
        setCheckableStateMachine(STATE_NotReadyToSwitchOn);
    }
    if (state == NodeProfile402::STATE_SwitchOnDisabled)
    {
        _statusWordStateLabel->setText(tr("SwitchOnDisabled"));
        setCheckableStateMachine(STATE_SwitchOnDisabled);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state == NodeProfile402::STATE_ReadyToSwitchOn)
    {
        _statusWordStateLabel->setText(tr("ReadyToSwitchOn"));
        setCheckableStateMachine(STATE_ReadyToSwitchOn);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state == NodeProfile402::STATE_SwitchedOn)
    {
        _statusWordStateLabel->setText(tr("SwitchedOn"));
        setCheckableStateMachine(STATE_SwitchedOn);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state == NodeProfile402::STATE_OperationEnabled)
    {
        _statusWordStateLabel->setText(tr("OperationEnabled"));
        setCheckableStateMachine(STATE_OperationEnabled);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
        _haltPushButton->setEnabled(true);
        _haltPushButton->setCheckable(true);
    }
    if (state == NodeProfile402::STATE_QuickStopActive)
    {
        _statusWordStateLabel->setText(tr("QuickStopActive"));
        setCheckableStateMachine(STATE_QuickStopActive);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state == NodeProfile402::STATE_FaultReactionActive)
    {
        _statusWordStateLabel->setText(tr("FaultReactionActive"));
        setCheckableStateMachine(STATE_FaultReactionActive);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
        _haltPushButton->setCheckable(false);
        _haltPushButton->setEnabled(false);
    }
    if (state == NodeProfile402::STATE_Fault)
    {
        _statusWordStateLabel->setText(tr("Fault"));
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

void WidgetDebug::updateModeComboBox()
{
    QList<NodeProfile402::OperationMode> modeList = _nodeProfile402->modesSupported();
    _modeComboBox->clear();

    for (quint8 i = 0; i < modeList.size(); i++)
    {
        _listModeComboBox.append(modeList.at(i));
        _modeComboBox->insertItem(i, _nodeProfile402->modeStr(modeList.at(i)));
    }
}

void WidgetDebug::displayOption402()
{
    if (_stackedWidget->currentWidget() == _p402Option)
    {
        modeChanged(_axis, _nodeProfile402->actualMode());
    }
    else
    {
        _p402Option->updateData();
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
    _modeComboBox->setEnabled(false);
    _modeLabel->setText("Mode change in progress");
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
    QString name;

    // toolbar
    _toolBar = new QToolBar(tr("Axis commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &WidgetDebug::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(500);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });
    connect(&_timer, &QTimer::timeout, this, &WidgetDebug::readDataTimer);

    QAction *option402 = new QAction();
    option402->setCheckable(true);
    option402->setIcon(QIcon(":/icons/img/icons8-settings.png"));
    option402->setToolTip(tr("Option code"));
    connect(option402, &QAction::triggered, this, &WidgetDebug::displayOption402);

    _toolBar->addWidget(_logTimerSpinBox);
    _toolBar->addSeparator();
    _toolBar->addAction(option402);

    QWidget *p402Widget = new QWidget();
    QLayout *p402layout = new QVBoxLayout(p402Widget);
    p402layout->setMargin(0);

    // Group Box Mode
    _modeGroupBox = new QGroupBox(tr("Mode"));
    QFormLayout *modeLayout = new QFormLayout();
    _modeComboBox = new QComboBox();
    name = tr("Modes of operation ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_MODES_OF_OPERATION, _axis).index(), 16));
    modeLayout->addRow(new QLabel(name));
    modeLayout->addRow(_modeComboBox);

    _modeLabel = new QLabel();
    modeLayout->addWidget(_modeLabel);
    _modeGroupBox->setLayout(modeLayout);
    p402layout->addWidget(_modeGroupBox);

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

    _controlWordGroupBox = new QGroupBox(tr("Control Word ") + QString("(0x%1)").arg(QString::number(_controlWordObjectId.index(), 16)));
    QFormLayout *controlWordLayout = new QFormLayout();

    _haltPushButton = new QPushButton(tr("Halt"));
    _haltPushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    _haltPushButton->setEnabled(false);
    controlWordLayout->addRow(_haltPushButton);
    _controlWordGroupBox->setLayout(controlWordLayout);
    connect(_haltPushButton, &QPushButton::clicked, this, &WidgetDebug::haltClicked);

    _controlWordLabel = new IndexLabel();
    _controlWordLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    controlWordLayout->addRow(tr("ControlWord sended:"), _controlWordLabel);

    QPushButton *_gotoOEPushButton = new QPushButton(tr("Operation enabled quickly"));
    controlWordLayout->addRow(_gotoOEPushButton);
    connect(_gotoOEPushButton, &QPushButton::clicked, this, &WidgetDebug::gotoStateOEClicked);

    _controlWordGroupBox->setLayout(controlWordLayout);
    // END Group Box Control Word

    // Group Box Status Word
    name = tr("Status Word ") + QString("(0x%1)").arg(QString::number(_statusWordObjectId.index(), 16));
    _statusWordGroupBox = new QGroupBox(name);
    QFormLayout *statusWordLayout = new QFormLayout();

    _statusWordLabel = new IndexLabel();
    _statusWordLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    statusWordLayout->addRow(tr("StatusWord raw:"), _statusWordLabel);
    _statusWordStateLabel = new QLabel();
    statusWordLayout->addRow(tr("StatusWord State:"), _statusWordStateLabel);

    _informationLabel = new QLabel();
    statusWordLayout->addRow(tr("Information :"), _informationLabel);
    _warningLabel = new QLabel();
    _warningLabel->setStyleSheet("QLabel { color : red; }");
    statusWordLayout->addRow(tr("Warning :"), _warningLabel);
    _statusWordGroupBox->setLayout(statusWordLayout);
    // END Group Box Status Word

    p402layout->addWidget(_stateMachineGroupBox);
    p402layout->addWidget(_controlWordGroupBox);
    p402layout->addWidget(_statusWordGroupBox);

    QScrollArea *p402ScrollArea = new QScrollArea();
    p402ScrollArea->setWidget(p402Widget);
    p402ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    p402ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p402ScrollArea->setWidgetResizable(true);
    p402ScrollArea->setMaximumWidth(370);
    p402ScrollArea->setMinimumWidth(370);

    _p402Option = new P402OptionWidget();
    _p402vl = new P402VlWidget();
    _p402ip = new P402IpWidget();
    _p402tq = new P402TqWidget();
    _p402pp = new P402PpWidget();

    _stackedWidget = new QStackedWidget;
    _stackedWidget->addWidget(_p402Option);
    _stackedWidget->addWidget(_p402vl);
    _stackedWidget->addWidget(_p402ip);
    _stackedWidget->addWidget(_p402tq);
    _stackedWidget->addWidget(_p402pp);
    _stackedWidget->setMinimumWidth(550);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(0);
    hBoxLayout->addWidget(p402ScrollArea);
    hBoxLayout->addWidget(_stackedWidget);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setMargin(2);
    vBoxLayout->addWidget(_toolBar);
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
    }
}

void WidgetDebug::showEvent(QShowEvent *event)
{
    if (event->type() == QEvent::Show)
    {
    }
}
