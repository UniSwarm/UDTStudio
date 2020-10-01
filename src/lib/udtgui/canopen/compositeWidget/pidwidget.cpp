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

#include <QLayout>
#include <QSplitter>

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox>
#include <QFormLayout>
#include <QWidget>

PidWidget::PidWidget(QWidget *parent) : QWidget(parent)
{
    createWidgets();
}

Node *PidWidget::node() const
{
    return _node;
}

void PidWidget::setNode(Node *node)
{
    _node = node;
    if (!_node)
    {
        return;
    }

    _dSpinBox->setNode(node);
    _pSpinBox->setNode(node);
    _iSpinBox->setNode(node);
    _dSpinBox->setNode(node);
    _periodSpinBox->setNode(node);
    _targetSpinBox->setNode(node);

    switch (_mode)
    {
    case NodeProfile402::Mode::NoMode:
        break;
    case NodeProfile402::Mode::PP:
    {
        _pidInputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 1);
        _pidErrorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 2);
        _pidIntegratorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 3);
        _pidOutputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 4);
        _dataLogger->addData(_pidInputStatus_ObjId);
        _dataLogger->addData(_pidErrorStatus_ObjId);
        _dataLogger->addData(_pidIntegratorStatus_ObjId);
        _dataLogger->addData(_pidOutputStatus_ObjId);
        break;
    }
    case NodeProfile402::Mode::VL:
    {
        _pidInputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 1);
        _pidErrorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 2);
        _pidIntegratorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 3);
        _pidOutputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 4);
        _dataLogger->addData(_pidInputStatus_ObjId);
        _dataLogger->addData(_pidErrorStatus_ObjId);
        _dataLogger->addData(_pidIntegratorStatus_ObjId);
        _dataLogger->addData(_pidOutputStatus_ObjId);
        break;
    }
    case NodeProfile402::Mode::PV:
    case NodeProfile402::Mode::TQ:
    {
        _pidInputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 1);
        _pidErrorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 2);
        _pidIntegratorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 3);
        _pidOutputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 4);
        _dataLogger->addData(_pidInputStatus_ObjId);
        _dataLogger->addData(_pidErrorStatus_ObjId);
        _dataLogger->addData(_pidIntegratorStatus_ObjId);
        _dataLogger->addData(_pidOutputStatus_ObjId);
        break;
    }
    case NodeProfile402::Mode::HM:
    case NodeProfile402::Mode::IP:
    case NodeProfile402::Mode::CSP:
    case NodeProfile402::Mode::CSV:
    case NodeProfile402::Mode::CST:
    case NodeProfile402::Mode::CSTCA:
    default:
        break;
    }
}
void PidWidget::setMode(NodeProfile402::Mode mode)
{
    _mode = mode;
    switch (_mode)
    {
    case NodeProfile402::Mode::NoMode:
        break;
    case NodeProfile402::Mode::PP:
    {
        setPObjectId(NodeObjectId(0x4041, 1));
        setIObjectId(NodeObjectId(0x4041, 2));
        setDObjectId(NodeObjectId(0x4041, 3));
        setPeriodObjectId(NodeObjectId(0x4041, 4));
        setTargetObjectId(NodeObjectId(0x607A, 0));
        break;
    }
    case NodeProfile402::Mode::VL:
    {
        setPObjectId(NodeObjectId(0x4021, 1));
        setIObjectId(NodeObjectId(0x4021, 2));
        setDObjectId(NodeObjectId(0x4021, 3));
        setPeriodObjectId(NodeObjectId(0x4021, 4));
        setTargetObjectId(NodeObjectId(0x6042, 0));
        break;
    }
    case NodeProfile402::Mode::PV:
    case NodeProfile402::Mode::TQ:
    {
        setPObjectId(NodeObjectId(0x4061, 1));
        setIObjectId(NodeObjectId(0x4061, 2));
        setDObjectId(NodeObjectId(0x4061, 3));
        setPeriodObjectId(NodeObjectId(0x4061, 4));
        setTargetObjectId(NodeObjectId(0x6071, 0));
        break;
    }
    case NodeProfile402::Mode::HM:
    case NodeProfile402::Mode::IP:
    case NodeProfile402::Mode::CSP:
    case NodeProfile402::Mode::CSV:
    case NodeProfile402::Mode::CST:
    case NodeProfile402::Mode::CSTCA:
    default:
        break;
    }
}

void PidWidget::setPObjectId(const NodeObjectId &objId)
{
    _pidP_ObjId = objId;
    _pSpinBox->setObjId(_pidP_ObjId);
}
void PidWidget::setIObjectId(const NodeObjectId &objId)
{
    _pidI_ObjId = objId;
    _iSpinBox->setObjId(_pidI_ObjId);
}
void PidWidget::setDObjectId(const NodeObjectId &objId)
{
    _pidD_ObjId = objId;
    _dSpinBox->setObjId(_pidD_ObjId);
}
void PidWidget::setPeriodObjectId(const NodeObjectId &objId)
{
    _period_ObjId = objId;
    _periodSpinBox->setObjId(_period_ObjId);
}
void PidWidget::setTargetObjectId(const NodeObjectId &objId)
{
    _target_ObjId = objId;
    _targetSpinBox->setObjId(_target_ObjId);
}

void PidWidget::savePosition()
{

}

void PidWidget::goTargetPosition()
{
    _dataLogger->start(10);
    switch (_mode)
    {
    case NodeProfile402::Mode::PP:
    {

        break;
    }
    case NodeProfile402::Mode::VL:
    {

        break;
    }
    case NodeProfile402::Mode::PV:
    case NodeProfile402::Mode::TQ:
    {

        break;
    }
    case NodeProfile402::Mode::NoMode:
    case NodeProfile402::Mode::HM:
    case NodeProfile402::Mode::IP:
    case NodeProfile402::Mode::CSP:
    case NodeProfile402::Mode::CSV:
    case NodeProfile402::Mode::CST:
    case NodeProfile402::Mode::CSTCA:
    default:
        break;
    }

    _measurementTimer.singleShot(_windowSpinBox->value(), this, SLOT(stopMeasurementTimer()));
}

void PidWidget::stopMeasurementTimer()
{
    _dataLogger->stop();
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

    _pSpinBox = new IndexSpinBox(_pidP_ObjId);
    _pSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("P :"), _pSpinBox);

    _iSpinBox = new IndexSpinBox(_pidI_ObjId);
    _iSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("I :"), _iSpinBox);

    _dSpinBox = new IndexSpinBox(_pidD_ObjId);
    _dSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("D :"), _dSpinBox);

    _periodSpinBox = new IndexSpinBox(_period_ObjId);
    pidLayout->addRow(tr("Period :"), _periodSpinBox);

    pidGroupBox->setLayout(pidLayout);

    QGroupBox *targetGroupBox = new QGroupBox(tr("Target"));
    QFormLayout *targetLayout = new QFormLayout(pidWidget);

    _targetSpinBox = new IndexSpinBox(_target_ObjId);
    targetLayout->addRow(tr("Target :"), _targetSpinBox);

    _windowSpinBox = new QSpinBox();
    _windowSpinBox->setRange(10, 5000);
    _windowSpinBox->setValue(500);
    _windowSpinBox->setSuffix(" ms");
    targetLayout->addRow(tr("Measurement window :"), _windowSpinBox);

    _savePushButton = new QPushButton(tr("Save"));
    connect(_savePushButton, &QPushButton::clicked, this, &PidWidget::savePosition);

    _goTargetPushButton = new QPushButton(tr("GO"));
    connect(_goTargetPushButton, &QPushButton::clicked, this, &PidWidget::goTargetPosition);

    targetGroupBox->setLayout(targetLayout);

    actionLayout->addWidget(pidGroupBox);
    actionLayout->addWidget(targetGroupBox);
    actionLayout->addWidget(_savePushButton);
    actionLayout->addWidget(_goTargetPushButton);
    splitter->addWidget(pidWidget);

    _dataLogger = new DataLogger();
    DataLoggerChartsWidget *_dataLoggerChartsWidget = new DataLoggerChartsWidget(_dataLogger);

    splitter->addWidget(_dataLoggerChartsWidget);
    setLayout(layout);
}

QString PidWidget::title() const
{
    switch (_mode)
    {
    case NodeProfile402::Mode::NoMode:
        return tr("No mode");
    case NodeProfile402::Mode::PP:
        return tr("PID Profile position");
    case NodeProfile402::Mode::VL:
        return tr("PID Velocity");
    case NodeProfile402::Mode::PV:
        return tr("PID Profile velocity");
    case NodeProfile402::Mode::TQ:
        return tr("PID Torque profile");
//    case NodeProfile402::Mode::HM:
//        return tr("PID Homing");
    case NodeProfile402::Mode::IP:
        return tr("PID Interpolated position");
    case NodeProfile402::Mode::CSP:
        return tr("PID Cyclic sync position");
    case NodeProfile402::Mode::CSV:
        return tr("PID Cyclic sync velocity");
    case NodeProfile402::Mode::CST:
        return tr("PID Cyclic sync torque");
    case NodeProfile402::Mode::CSTCA:
        return tr("PID Cyclic sync torque mode with commutation angle");
    default:
        if (_mode < 0)
        {
            return tr("Manufacturer-specific");
        }
        else
        {
            return tr("Reserved");
        }
    }
}


