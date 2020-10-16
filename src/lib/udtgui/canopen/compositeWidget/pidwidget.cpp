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

#include "pidwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
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

PidWidget::PidWidget(QWidget *parent) : QWidget(parent)
{
    _nodeProfile402 = nullptr;
    createWidgets();
    connect(&_timer, &QTimer::timeout, this, &PidWidget::manageMeasurement);
    _state = NONE;
}

Node *PidWidget::node() const
{
    return _node;
}

void PidWidget::setNode(Node *node)
{
    _dSpinBox->setNode(node);
    _pSpinBox->setNode(node);
    _iSpinBox->setNode(node);
    _dSpinBox->setNode(node);
    _periodSpinBox->setNode(node);

    _node = node;
    if (!_node)
    {
        return;
    }
    connect(_node, &Node::statusChanged, this, &PidWidget::statusNodeChanged);
    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = static_cast<NodeProfile402 *>(_node->profiles()[0]);
    }

    _modePid = MODE_PID_NONE;
}
void PidWidget::setMode(PidWidget::ModePid mode)
{
    if (!_nodeProfile402)
    {
        return;
    }

    NodeObjectId pidP_ObjId;
    NodeObjectId pidI_ObjId;
    NodeObjectId pidD_ObjId;
    NodeObjectId period_ObjId;
    NodeObjectId target_ObjId;
    NodeObjectId pidInputStatus_ObjId;
    NodeObjectId pidErrorStatus_ObjId;
    NodeObjectId pidIntegratorStatus_ObjId;
    NodeObjectId pidOutputStatus_ObjId;

    _modePid = mode;
    switch (_modePid)
    {
    case PidWidget::MODE_PID_NONE:
        break;
    case MODE_PID_VELOCITY:
    {
        pidP_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_P);
        pidI_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_I);
        pidD_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_D);
        period_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_PERIOD_US);
        pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_INPUT);
        pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_ERROR);
        pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_INTEGRATOR);
        pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_SPEED_OUTPUT);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET);
        _secondTargetSpinBox->setEnabled(true);
        break;
    }
    case MODE_PID_TORQUE:
    {
        pidP_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_P);
        pidI_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_I);
        pidD_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_D);
        period_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_PERIOD_US);
        pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_INPUT);
        pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_ERROR);
        pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_INTEGRATOR);
        pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_TORQUE_OUTPUT);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE);
        _secondTargetSpinBox->setEnabled(true);
        break;
    }
    case MODE_PID_POSITION:
    {
        pidP_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_P);
        pidI_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_I);
        pidD_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_D);
        period_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_PERIOD_US);
        pidInputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_INPUT);
        pidErrorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_ERROR);
        pidIntegratorStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_INTEGRATOR);
        pidOutputStatus_ObjId = IndexDb402::getObjectId(IndexDb402::OD_MS_POSITION_OUTPUT);
        target_ObjId = IndexDb402::getObjectId(IndexDb402::OD_PP_TARGET_POSITION);
        _secondTargetSpinBox->setEnabled(false);
        break;
    }
    }

    _pSpinBox->setObjId(pidP_ObjId);
    _iSpinBox->setObjId(pidI_ObjId);
    _dSpinBox->setObjId(pidD_ObjId);
    _periodSpinBox->setObjId(period_ObjId);

    pidInputStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    pidErrorStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    pidIntegratorStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    pidOutputStatus_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());

    _dataLogger->addData(pidInputStatus_ObjId);
    _dataLogger->addData(pidErrorStatus_ObjId);
    _dataLogger->addData(pidIntegratorStatus_ObjId);
    _dataLogger->addData(pidOutputStatus_ObjId);
}

void PidWidget::savePosition()
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
        _nodeProfile402->setEnableRamp(true);
        if (_nodeProfile402->actualMode() != NodeProfile402::IP)
        {
            _nodeProfile402->setMode(NodeProfile402::IP);
        }
        else
        {
            mode402Changed(NodeProfile402::IP);
        }
        break;
    case MODE_PID_VELOCITY:
        _nodeProfile402->setEnableRamp(false);
        if (_nodeProfile402->actualMode() != NodeProfile402::VL)
        {
            _nodeProfile402->setMode(NodeProfile402::VL);
        }
        else
        {
            mode402Changed(NodeProfile402::VL);
        }
        break;
    case MODE_PID_TORQUE:
        if (_nodeProfile402->actualMode() != NodeProfile402::TQ)
        {
            _nodeProfile402->setMode(NodeProfile402::TQ);
        }
        else
        {
            mode402Changed(NodeProfile402::TQ);
        }
        break;
    }

    _savePushButton->setEnabled(false);
    _goTargetPushButton->setEnabled(false);

}

void PidWidget::mode402Changed(NodeProfile402::Mode modeNew)
{
    if (!_nodeProfile402)
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
    case NodeProfile402::IP :
    case NodeProfile402::CSP:
        _modePid = ModePid::MODE_PID_POSITION;
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
    case NodeProfile402::CST:
    case NodeProfile402::CSTCA:
    case NodeProfile402::TQ:
        _modePid = ModePid::MODE_PID_TORQUE;
        _nodeProfile402->setTarget(_firstTargetSpinBox->value());
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
        break;
    }
    _timer.start(_windowSpinBox->value());
}

void PidWidget::manageMeasurement()
{
    switch (_state)
    {
    case PidWidget::NONE:
        _dataLogger->start(10);
        _dataLogger->clear();
        _state = LAUCH_DATALOGGER;
        _timer.start(100);
        break;
    case PidWidget::LAUCH_DATALOGGER:
        _state = LAUCH_FIRST_TARGET;
        _timer.stop();
        changeMode402();
        break;
    case PidWidget::LAUCH_FIRST_TARGET:
        stopFirstMeasurement();
        break;
    case PidWidget::LAUCH_SECOND_TARGET:
        stopSecondMeasurement();
        _timer.start(_windowSpinBox->value());
        _state = STOP_DATALOGGER;
        break;
    case PidWidget::STOP_DATALOGGER:
        stopDataLogger();
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
        _nodeProfile402->setTarget(0);
        _timer.start(_stopDataLoggerSpinBox->value());
        _state = STOP_DATALOGGER;
        break;
    case MODE_PID_VELOCITY:
        _nodeProfile402->setTarget(_secondTargetSpinBox->value());
        _timer.start(_windowSpinBox->value());
        _state = LAUCH_SECOND_TARGET;
        break;
    case MODE_PID_TORQUE:
        _nodeProfile402->setTarget(_secondTargetSpinBox->value());
        _timer.start(_windowSpinBox->value());
        _state = LAUCH_SECOND_TARGET;
        break;
    }
}

void PidWidget::stopSecondMeasurement()
{
    if (!_nodeProfile402)
    {
        return;
    }

    _nodeProfile402->setTarget(0);
}

void PidWidget::stopDataLogger()
{
    _timer.stop();
    _state = NONE;
    _dataLogger->stop();
    _savePushButton->setEnabled(true);
    _goTargetPushButton->setEnabled(true);
    disconnect(_nodeProfile402, &NodeProfile402::modeChanged, this, &PidWidget::mode402Changed);
}

void PidWidget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    QSplitter *splitter = new QSplitter();
    layout->addWidget(splitter);

    QWidget *pidWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(pidWidget);

    QGroupBox *pidGroupBox = new QGroupBox(tr("PID"));
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

    _periodSpinBox = new IndexSpinBox();
    pidLayout->addRow(tr("P&eriod :"), _periodSpinBox);

    pidGroupBox->setLayout(pidLayout);

    QGroupBox *targetGroupBox = new QGroupBox(tr("Target"));
    QFormLayout *targetLayout = new QFormLayout();

    _firstTargetSpinBox = new QSpinBox();
    _firstTargetSpinBox->setValue(200);
    _firstTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    targetLayout->addRow(tr("&Fisrt Target :"), _firstTargetSpinBox);

    _secondTargetSpinBox = new QSpinBox();
    _secondTargetSpinBox->setValue(100);
    _secondTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    targetLayout->addRow(tr("&Second Target :"), _secondTargetSpinBox);

    _windowSpinBox = new QSpinBox();
    _windowSpinBox->setRange(10, 5000);
    _windowSpinBox->setValue(500);
    _windowSpinBox->setSuffix(" ms");

    _stopDataLoggerSpinBox = new QSpinBox();
    _stopDataLoggerSpinBox->setRange(10, 5000);
    _stopDataLoggerSpinBox->setValue(100);
    _stopDataLoggerSpinBox->setSuffix(" ms");

    targetLayout->addRow(tr("&Measurement window :"), _windowSpinBox);
    targetLayout->addRow(tr("&Time limit after the end of the last target :"), _stopDataLoggerSpinBox);

    _savePushButton = new QPushButton(tr("&Save"));
    connect(_savePushButton, &QPushButton::clicked, this, &PidWidget::savePosition);

    _goTargetPushButton = new QPushButton(tr("G&O"));
    connect(_goTargetPushButton, &QPushButton::clicked, this, &PidWidget::manageMeasurement);

    targetGroupBox->setLayout(targetLayout);

    actionLayout->addWidget(pidGroupBox);
    actionLayout->addWidget(targetGroupBox);
    actionLayout->addWidget(_savePushButton);
    actionLayout->addWidget(_goTargetPushButton);
    splitter->addWidget(pidWidget);

    _dataLogger = new DataLogger();
    _dataLoggerChartsWidget = new DataLoggerChartsWidget(_dataLogger);

    splitter->addWidget(_dataLoggerChartsWidget);
    setLayout(layout);
}

void PidWidget::statusNodeChanged(Node::Status status)
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

QString PidWidget::title() const
{
    switch (_modePid)
    {
    case PidWidget::MODE_PID_NONE:
        return tr("None");
    case MODE_PID_POSITION:
        return tr("PID Position");
    case MODE_PID_VELOCITY:
        return tr("PID Velocity");
    case MODE_PID_TORQUE:
        return tr("PID Torque");
    }
    return QString();
}
