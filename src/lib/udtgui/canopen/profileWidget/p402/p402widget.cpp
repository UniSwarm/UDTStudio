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

#include "p402widget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexlabel.h"

#include "p402cpwidget.h"
#include "p402cstcawidget.h"
#include "p402dtywidget.h"
#include "p402ipwidget.h"
#include "p402optionwidget.h"
#include "p402ppwidget.h"
#include "p402tqwidget.h"
#include "p402vlwidget.h"

#include <QButtonGroup>
#include <QFormLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QStandardItemModel>

P402Widget::P402Widget(NodeProfile402 *profile, QWidget *parent)
    : QWidget(parent)
    , _nodeProfile402(nullptr)
{
    createWidgets();

    setProfile(profile);
}

Node *P402Widget::node() const
{
    return _nodeProfile402->node();
}

QString P402Widget::title() const
{
    return QString("Motion control");
}

void P402Widget::setProfile(NodeProfile402 *profile)
{
    if (profile == nullptr)
    {
        return;
    }
    _nodeProfile402 = profile;
    _nodeProfile402->init();

    _controlWordLabel->setObjId(_nodeProfile402->controlWordObjectId());
    _statusWordLabel->setObjId(_nodeProfile402->statusWordObjectId());

    setCheckableStateMachine(2);
    updateModeComboBox();

    connect(node(), &Node::statusChanged, this, &P402Widget::updateNodeStatus);
    connect(_nodeProfile402, &NodeProfile402::modeChanged, this, &P402Widget::updateMode);
    connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &P402Widget::updateState);
    connect(_nodeProfile402, &NodeProfile402::isHalted, _haltPushButton, &QPushButton::setChecked);
    connect(_nodeProfile402, &NodeProfile402::eventHappened, this, &P402Widget::setEvent);
    connect(_nodeProfile402, &NodeProfile402::supportedDriveModesUdpdated, this, &P402Widget::updateModeComboBox);
    connect(_modeComboBox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [=](int id)
            {
                setModeIndex(id);
            });

    for (P402ModeWidget *mode : qAsConst(_modes))
    {
        mode->setNode(node(), _nodeProfile402->axisId()); // TODO
    }

    updateNodeStatus();
    updateMode(_nodeProfile402->actualMode());
    updateState();

    _modeOfOperationLabel->setText(tr("Modes of operation (0x%1):").arg(QString::number(_nodeProfile402->modesOfOperationObjectId().index(), 16)));
    _controlWordGroupBox->setTitle(tr("Control word (0x%1)").arg(QString::number(_nodeProfile402->controlWordObjectId().index(), 16)));
    _statusWordGroupBox->setTitle(tr("Status word (0x%1)").arg(QString::number(_nodeProfile402->statusWordObjectId().index(), 16)));
}

void P402Widget::updateNodeStatus()
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    if (node()->status() == Node::STARTED)
    {
        updateMode(_nodeProfile402->actualMode());
        _stackedWidget->setEnabled(false);
        _modeGroupBox->setEnabled(true);
    }
    else
    {
        setStartLogger(false);
    }
}

void P402Widget::updateMode(NodeProfile402::OperationMode mode)
{
    /*if ((mode == NodeProfile402::IP) || (mode == NodeProfile402::VL) || (mode == NodeProfile402::TQ) || (mode == NodeProfile402::PP) || (mode == NodeProfile402::DTY)
        || (mode == NodeProfile402::CP) || (mode == NodeProfile402::CSTCA))*/
    {
        P402ModeWidget *mode402 = dynamic_cast<P402ModeWidget *>(_stackedWidget->currentWidget());
        // reset : Patch because the widget Tarqet torque and velocity are not an IndexSpinbox so not read automaticaly
        mode402->reset();

        //_option402Action->setChecked(false);
        _stackedWidget->setCurrentWidget(_modes[mode]);
        _modeComboBox->setCurrentText(_nodeProfile402->modeStr(mode));
    }
    /*else
    {
        _option402Action->setChecked(true);
        _stackedWidget->setCurrentWidget(_modes[NodeProfile402::NoMode]);
    }*/

    _modeComboBox->setEnabled(true);
    _modeLabel->clear();
}

void P402Widget::updateState()
{
    NodeProfile402::State402 state = _nodeProfile402->currentState();

    for (QAbstractButton *button : _stateMachineButtonGroup->buttons())
    {
        button->setEnabled(false);
    }

    switch (state)
    {
        case NodeProfile402::STATE_NotReadyToSwitchOn:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_NotReadyToSwitchOn));
            setCheckableStateMachine(NodeProfile402::STATE_NotReadyToSwitchOn);
            break;

        case NodeProfile402::STATE_SwitchOnDisabled:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_SwitchOnDisabled));
            setCheckableStateMachine(NodeProfile402::STATE_SwitchOnDisabled);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_ReadyToSwitchOn)->setEnabled(true);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_ReadyToSwitchOn:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_ReadyToSwitchOn));
            setCheckableStateMachine(NodeProfile402::STATE_ReadyToSwitchOn);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchedOn)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_OperationEnabled)->setEnabled(true);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_SwitchedOn:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_SwitchedOn));
            setCheckableStateMachine(NodeProfile402::STATE_SwitchedOn);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_OperationEnabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchedOn)->setEnabled(true);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_OperationEnabled:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_OperationEnabled));
            setCheckableStateMachine(NodeProfile402::STATE_OperationEnabled);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_ReadyToSwitchOn)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchedOn)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_OperationEnabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_QuickStopActive)->setEnabled(true);
            _haltPushButton->setEnabled(true);
            _haltPushButton->setCheckable(true);
            break;

        case NodeProfile402::STATE_QuickStopActive:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_QuickStopActive));
            setCheckableStateMachine(NodeProfile402::STATE_QuickStopActive);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchOnDisabled)->setEnabled(true);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_QuickStopActive)->setEnabled(true);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_FaultReactionActive:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_FaultReactionActive));
            setCheckableStateMachine(NodeProfile402::STATE_FaultReactionActive);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;

        case NodeProfile402::STATE_Fault:
            _statusWordStateLabel->setText(_nodeProfile402->stateStr(NodeProfile402::STATE_Fault));
            setCheckableStateMachine(NodeProfile402::STATE_Fault);
            _stateMachineButtonGroup->button(NodeProfile402::STATE_SwitchOnDisabled)->setEnabled(true);
            _haltPushButton->setCheckable(false);
            _haltPushButton->setEnabled(false);
            break;
    }

    update();
}

void P402Widget::setModeIndex(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    if (id == -1)
    {
        return;
    }
    _modeComboBox->setEnabled(false);
    _modeLabel->setText("Mode change in progress");
    NodeProfile402::OperationMode mode = static_cast<NodeProfile402::OperationMode>(_modeComboBox->currentData().toInt());
    _nodeProfile402->setMode(mode);
}

void P402Widget::gotoStateOEClicked()
{
    setStartLogger(true);
    _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
}

void P402Widget::setStartLogger(bool start)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    if (start)
    {
        if (_nodeProfile402->state() == NodeProfile402::NODEPROFILE_STARTED)
        {
            return;
        }

        if (node()->status() != Node::STARTED)
        {
            node()->sendStart();
        }

        _nodeProfile402->start(_logTimerSpinBox->value());

        _stackedWidget->setEnabled(true);
        _modeGroupBox->setEnabled(true);
    }
    else
    {
        _nodeProfile402->stop();

        _stackedWidget->setEnabled(false);
        _modeGroupBox->setEnabled(true);
    }

    if (_startStopAction->isChecked() != start)
    {
        _startStopAction->blockSignals(true);
        _startStopAction->setChecked(start);
        _startStopAction->blockSignals(false);
    }
}

void P402Widget::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _nodeProfile402->start(ms);
    }
}

void P402Widget::readAllObjects()
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    if (_stackedWidget->currentWidget() == _modes[NodeProfile402::NoMode])
    {
        _modes[NodeProfile402::NoMode]->readAllObjects();
    }
    else
    {
        _nodeProfile402->readAllObjects();
    }
}

void P402Widget::setEvent(quint8 event)
{
    QString informationText;
    if ((event & NodeProfile402::VoltageEnabled) != 0)
    {
        informationText = _nodeProfile402->event402Str(NodeProfile402::VoltageEnabled);
    }
    if ((event & NodeProfile402::Remote) != 0)
    {
        if (!informationText.isEmpty())
        {
            informationText.append(",\n");
        }
        informationText.append(_nodeProfile402->event402Str(NodeProfile402::Remote));
    }
    if ((event & NodeProfile402::TargetReached) != 0)
    {
        if (!informationText.isEmpty())
        {
            informationText.append(",\n");
        }
        informationText.append(_nodeProfile402->event402Str(NodeProfile402::TargetReached));
    }
    if ((event & NodeProfile402::ModeSpecific) != 0)
    {
        if (!informationText.isEmpty())
        {
            informationText.append(",\n");
        }
        informationText.append(_nodeProfile402->event402Str(NodeProfile402::ModeSpecific));
    }
    _informationLabel->setText(informationText);

    QString warningText;
    if ((event & NodeProfile402::InternalLimitActive) != 0)
    {
        warningText.append(_nodeProfile402->event402Str(NodeProfile402::InternalLimitActive));
    }
    if ((event & NodeProfile402::Warning) != 0)
    {
        if (!warningText.isEmpty())
        {
            warningText.append(",\n");
        }
        warningText.append(_nodeProfile402->event402Str(NodeProfile402::Warning));
    }
    quint16 mode = static_cast<quint16>(_nodeProfile402->actualMode());
    if (mode == 7)
    {
        if ((event & NodeProfile402::FollowingError) != 0)
        {
            if (!warningText.isEmpty())
            {
                warningText.append(",\n");
            }
            warningText.append(_nodeProfile402->event402Str(NodeProfile402::FollowingError));
        }
    }
    _warningLabel->setText(warningText);
}

void P402Widget::updateModeComboBox()
{
    _modeComboBox->blockSignals(true);
    _modeComboBox->clear();

    QList<NodeProfile402::OperationMode> otherSupportedModes = _nodeProfile402->modesSupportedByType(IndexDb402::MODE402_OTHER);
    if (!otherSupportedModes.isEmpty())
    {
        _modeComboBox->addItem(tr("OTHER"));
        dynamic_cast<QStandardItemModel *>(_modeComboBox->model())->item(_modeComboBox->count() - 1)->setEnabled(false);
        for (NodeProfile402::OperationMode mode : otherSupportedModes)
        {
            _modeComboBox->addItem(_nodeProfile402->modeStr(mode), QVariant(static_cast<int>(mode)));
        }
        _modeComboBox->insertSeparator(_modeComboBox->count());
    }

    QList<NodeProfile402::OperationMode> torqueSupportedModes = _nodeProfile402->modesSupportedByType(IndexDb402::MODE402_TORQUE);
    if (!torqueSupportedModes.isEmpty())
    {
        _modeComboBox->addItem(tr("TORQUE"));
        dynamic_cast<QStandardItemModel *>(_modeComboBox->model())->item(_modeComboBox->count() - 1)->setEnabled(false);
        for (NodeProfile402::OperationMode mode : torqueSupportedModes)
        {
            _modeComboBox->addItem(_nodeProfile402->modeStr(mode), QVariant(static_cast<int>(mode)));
        }
        _modeComboBox->insertSeparator(_modeComboBox->count());
    }

    QList<NodeProfile402::OperationMode> velocitySupportedModes = _nodeProfile402->modesSupportedByType(IndexDb402::MODE402_VELOCITY);
    if (!velocitySupportedModes.isEmpty())
    {
        _modeComboBox->addItem(tr("VELOCITY"));
        dynamic_cast<QStandardItemModel *>(_modeComboBox->model())->item(_modeComboBox->count() - 1)->setEnabled(false);
        for (NodeProfile402::OperationMode mode : velocitySupportedModes)
        {
            _modeComboBox->addItem(_nodeProfile402->modeStr(mode), QVariant(static_cast<int>(mode)));
        }
        _modeComboBox->insertSeparator(_modeComboBox->count());
    }

    QList<NodeProfile402::OperationMode> positionSupportedModes = _nodeProfile402->modesSupportedByType(IndexDb402::MODE402_POSITION);
    if (!positionSupportedModes.isEmpty())
    {
        _modeComboBox->addItem(tr("POSITION"));
        dynamic_cast<QStandardItemModel *>(_modeComboBox->model())->item(_modeComboBox->count() - 1)->setEnabled(false);
        for (NodeProfile402::OperationMode mode : positionSupportedModes)
        {
            _modeComboBox->addItem(_nodeProfile402->modeStr(mode), QVariant(static_cast<int>(mode)));
        }
    }
    updateMode(_nodeProfile402->actualMode());
    _modeComboBox->blockSignals(false);
}

void P402Widget::displayOption402()
{
    if (_stackedWidget->currentWidget() == _modes[NodeProfile402::NoMode])
    {
        updateMode(_nodeProfile402->actualMode());
    }
    else
    {
        _modes[NodeProfile402::NoMode]->readAllObjects();
        _stackedWidget->setCurrentWidget(_modes[NodeProfile402::NoMode]);
    }
}

void P402Widget::stateMachineClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    setStartLogger(true);
    _nodeProfile402->goToState(static_cast<NodeProfile402::State402>(id));
}

void P402Widget::haltClicked()
{
    _nodeProfile402->toggleHalt();
}

void P402Widget::setCheckableStateMachine(int id)
{
    for (int i = 1; i <= _stateMachineButtonGroup->buttons().count(); i++)
    {
        _stateMachineButtonGroup->button(i)->setCheckable(false);
    }
    _stateMachineButtonGroup->button(id)->setCheckable(true);
    _stateMachineButtonGroup->button(id)->setChecked(true);
}

void P402Widget::createWidgets()
{
    // Widget P402
    _modes.insert(NodeProfile402::VL, new P402VlWidget());
    _modes.insert(NodeProfile402::IP, new P402IpWidget());
    _modes.insert(NodeProfile402::TQ, new P402TqWidget());
    _modes.insert(NodeProfile402::PP, new P402PpWidget());
    _modes.insert(NodeProfile402::DTY, new P402DtyWidget());
    _modes.insert(NodeProfile402::CP, new P402CpWidget());
    _modes.insert(NodeProfile402::CSTCA, new P402CstcaWidget());
    _modes.insert(NodeProfile402::NoMode, new P402OptionWidget());

    // Stacked Widget
    _stackedWidget = new QStackedWidget;
    for (P402ModeWidget *mode : qAsConst(_modes))
    {
        _stackedWidget->addWidget(mode);
    }
    _stackedWidget->setStyleSheet("QStackedWidget {padding: 0 0 0 3px;}");
    _stackedWidget->setMinimumWidth(450);

    // Create interface
    QWidget *controlWidget = new QWidget();
    QLayout *controlLayout = new QVBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 4, 0);
    controlLayout->setSpacing(0);

    _modeGroupBox = createModeWidgets();
    controlLayout->addWidget(_modeGroupBox);
    _stateMachineGroupBox = createStateMachineWidgets();
    controlLayout->addWidget(_stateMachineGroupBox);
    _controlWordGroupBox = createControlWordWidgets();
    controlLayout->addWidget(_controlWordGroupBox);
    _statusWordGroupBox = createStatusWordWidgets();
    controlLayout->addWidget(_statusWordGroupBox);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(controlWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setMaximumWidth(370);
    scrollArea->setMinimumWidth(200);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->setSpacing(0);
    hBoxLayout->addWidget(scrollArea);
    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);
    hBoxLayout->addWidget(frame);
    hBoxLayout->addWidget(_stackedWidget);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(2);
    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2, 2, 2, 0);
    toolBarLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addItem(toolBarLayout);
    vBoxLayout->addLayout(hBoxLayout);
    setLayout(vBoxLayout);
}

QToolBar *P402Widget::createToolBarWidgets()
{
    QToolBar *toolBar = new QToolBar(tr("Axis commands"));
    toolBar->setIconSize(QSize(20, 20));

    _startStopAction = toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    QIcon iconStartStop;
    iconStartStop.addFile(":/icons/img/icons8-stop.png", QSize(), QIcon::Normal, QIcon::On);
    iconStartStop.addFile(":/icons/img/icons8-play.png", QSize(), QIcon::Normal, QIcon::Off);
    _startStopAction->setIcon(iconStartStop);
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &P402Widget::setStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            [=](int i)
            {
                setLogTimer(i);
            });

    toolBar->addSeparator();

    _option402Action = toolBar->addAction(tr("Options code"));
    _option402Action->setCheckable(true);
    _option402Action->setIcon(QIcon(":/icons/img/icons8-settings.png"));
    _option402Action->setStatusTip(tr("Show options code settings"));
    connect(_option402Action, &QAction::triggered, this, &P402Widget::displayOption402);

    toolBar->addSeparator();

    // read all action
    QAction *readAllObjectAction = toolBar->addAction(tr("Read all objects"));
    readAllObjectAction->setIcon(QIcon(":/icons/img/icons8-update.png"));
    readAllObjectAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllObjectAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllObjectAction, &QAction::triggered, this, &P402Widget::readAllObjects);

    return toolBar;
}

QGroupBox *P402Widget::createModeWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Mode"));
    QFormLayout *layout = new QFormLayout();

    _modeComboBox = new QComboBox();
    _modeOfOperationLabel = new QLabel();
    layout->addRow(_modeOfOperationLabel);
    layout->addRow(_modeComboBox);

    _modeLabel = new QLabel();
    layout->addWidget(_modeLabel);
    groupBox->setLayout(layout);

    return groupBox;
}

QGroupBox *P402Widget::createStateMachineWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("State machine"));
    QFormLayout *layout = new QFormLayout();

    _stateMachineButtonGroup = new QButtonGroup(this);
    _stateMachineButtonGroup->setExclusive(true);

    QPushButton *stateNotReadyToSwitchOnPushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_NotReadyToSwitchOn));
    stateNotReadyToSwitchOnPushButton->setEnabled(false);
    layout->addRow(stateNotReadyToSwitchOnPushButton);
    _stateMachineButtonGroup->addButton(stateNotReadyToSwitchOnPushButton, NodeProfile402::STATE_NotReadyToSwitchOn);

    QPushButton *stateSwitchOnDisabledPushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_SwitchOnDisabled));
    stateSwitchOnDisabledPushButton->setStyleSheet("QPushButton:checked { background-color : #ffa500; }"
                                                   "QPushButton:checked:hover { border: 1px solid #7f5200; }");
    layout->addRow(stateSwitchOnDisabledPushButton);
    _stateMachineButtonGroup->addButton(stateSwitchOnDisabledPushButton, NodeProfile402::STATE_SwitchOnDisabled);

    QPushButton *stateReadyToSwitchOnPushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_ReadyToSwitchOn));
    stateReadyToSwitchOnPushButton->setStyleSheet("QPushButton:checked { background-color : #ffa500; }"
                                                  "QPushButton:checked:hover { border: 1px solid #7f5200; }");
    layout->addRow(stateReadyToSwitchOnPushButton);
    _stateMachineButtonGroup->addButton(stateReadyToSwitchOnPushButton, NodeProfile402::STATE_ReadyToSwitchOn);

    QPushButton *stateSwitchedOnPushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_SwitchedOn));
    stateSwitchedOnPushButton->setStyleSheet("QPushButton:checked { background-color : #ffa500; }"
                                             "QPushButton:checked:hover { border: 1px solid #7f5200; }");
    layout->addRow(stateSwitchedOnPushButton);
    _stateMachineButtonGroup->addButton(stateSwitchedOnPushButton, NodeProfile402::STATE_SwitchedOn);

    QPushButton *stateOperationEnabledPushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_OperationEnabled));
    stateOperationEnabledPushButton->setStyleSheet("QPushButton:checked { background-color : #008000; }"
                                                   "QPushButton:checked:hover { border: 1px solid #004000; }");
    layout->addRow(stateOperationEnabledPushButton);
    _stateMachineButtonGroup->addButton(stateOperationEnabledPushButton, NodeProfile402::STATE_OperationEnabled);

    QPushButton *stateQuickStopActivePushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_QuickStopActive));
    stateQuickStopActivePushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }"
                                                  "QPushButton:checked:hover { border: 1px solid #094669; }");
    layout->addRow(stateQuickStopActivePushButton);
    _stateMachineButtonGroup->addButton(stateQuickStopActivePushButton, NodeProfile402::STATE_QuickStopActive);

    QPushButton *stateFaultReactionActivePushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_FaultReactionActive));
    stateFaultReactionActivePushButton->setStyleSheet("QPushButton:checked { background-color : #ff0000; }"
                                                      "QPushButton:checked:hover { border: 1px solid #7f0000; }");
    stateFaultReactionActivePushButton->setEnabled(false);
    layout->addRow(stateFaultReactionActivePushButton);
    _stateMachineButtonGroup->addButton(stateFaultReactionActivePushButton, NodeProfile402::STATE_FaultReactionActive);

    QPushButton *stateFaultPushButton = new QPushButton(_nodeProfile402->stateStr(NodeProfile402::STATE_Fault));
    stateFaultPushButton->setStyleSheet("QPushButton:checked { background-color : #ff0000; }"
                                        "QPushButton:checked:hover { border: 1px solid #7f0000; }");
    stateFaultPushButton->setEnabled(false);
    layout->addRow(stateFaultPushButton);
    _stateMachineButtonGroup->addButton(stateFaultPushButton, NodeProfile402::STATE_Fault);

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_stateMachineButtonGroup,
            QOverload<int>::of(&QButtonGroup::buttonClicked),
            this,
            [=](int id)
            {
                stateMachineClicked(id);
            });
#else
    connect(_stateMachineButtonGroup,
            QOverload<int>::of(&QButtonGroup::idClicked),
            this,
            [=](int id)
            {
                stateMachineClicked(id);
            });
#endif

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox *P402Widget::createControlWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox();
    QFormLayout *layout = new QFormLayout();

    _haltPushButton = new QPushButton(tr("Halt"));
    _haltPushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    _haltPushButton->setEnabled(false);
    layout->addRow(_haltPushButton);
    groupBox->setLayout(layout);
    connect(_haltPushButton, &QPushButton::clicked, this, &P402Widget::haltClicked);

    _controlWordLabel = new IndexLabel();
    _controlWordLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    layout->addRow(tr("Control word sent:"), _controlWordLabel);

    QPushButton *_gotoOEPushButton = new QPushButton(tr("Operation enabled quickly"));
    layout->addRow(_gotoOEPushButton);
    connect(_gotoOEPushButton, &QPushButton::clicked, this, &P402Widget::gotoStateOEClicked);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox *P402Widget::createStatusWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox();
    QFormLayout *layout = new QFormLayout();

    _statusWordLabel = new IndexLabel();
    _statusWordLabel->setDisplayHint(AbstractIndexWidget::DisplayHexa);
    layout->addRow(tr("Raw:"), _statusWordLabel);

    _statusWordStateLabel = new QLabel();
    layout->addRow(tr("State:"), _statusWordStateLabel);

    _informationLabel = new QLabel();
    layout->addRow(tr("Information:"), _informationLabel);

    _warningLabel = new QLabel();
    _warningLabel->setStyleSheet("QLabel { color : red; }");
    layout->addRow(tr("Warning:"), _warningLabel);

    groupBox->setLayout(layout);

    return groupBox;
}
