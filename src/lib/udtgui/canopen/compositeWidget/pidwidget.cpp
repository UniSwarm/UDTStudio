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

    _nodeProfile402 = static_cast<NodeProfile402 *>(_node->profiles()[0]);
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
            break;
        }
        case NodeProfile402::Mode::VL:
        {
            _pidP_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4021, 1);
            _pidI_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4021, 2);
            _pidD_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4021, 3);
            _period_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4021, 4);
            _target_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6042, 0);
            _pidInputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 1);
            _pidErrorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 2);
            _pidIntegratorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 3);
            _pidOutputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4020, 4);

            _nodeProfile402->setMode(NodeProfile402::VL);
            break;
        }
        case NodeProfile402::Mode::PV:
        case NodeProfile402::Mode::TQ:
        {
            _pidP_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4061, 1);
            _pidI_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4061, 2);
            _pidD_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4061, 3);
            _period_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4061, 4);
            _target_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6071, 0);
            _pidInputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 1);
            _pidErrorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 2);
            _pidIntegratorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 3);
            _pidOutputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4060, 4);

            _nodeProfile402->setMode(NodeProfile402::TQ);
            break;
        }
        case NodeProfile402::Mode::HM:
        case NodeProfile402::Mode::IP:
            _pidP_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4041, 1);
            _pidI_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4041, 2);
            _pidD_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4041, 3);
            _period_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4041, 4);
            _target_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x607A, 0);
            _pidInputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 1);
            _pidErrorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 2);
            _pidIntegratorStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 3);
            _pidOutputStatus_ObjId = NodeObjectId(_node->busId(), _node->nodeId(), 0x4040, 4);

            _nodeProfile402->setMode(NodeProfile402::PP);
            break;
        case NodeProfile402::Mode::CSP:
        case NodeProfile402::Mode::CSV:
        case NodeProfile402::Mode::CST:
        case NodeProfile402::Mode::CSTCA:
        default:
            break;
    }

    _pSpinBox->setObjId(_pidP_ObjId);
    _iSpinBox->setObjId(_pidI_ObjId);
    _dSpinBox->setObjId(_pidD_ObjId);
    _periodSpinBox->setObjId(_period_ObjId);
    _dataLogger->addData(_pidInputStatus_ObjId);
    _dataLogger->addData(_pidErrorStatus_ObjId);
    _dataLogger->addData(_pidIntegratorStatus_ObjId);
    _dataLogger->addData(_pidOutputStatus_ObjId);
}

void PidWidget::savePosition()
{
    QPixmap pixmap = _dataLoggerChartsWidget->grab();
    QString file =
        QString("%1_%2.png").arg(title()).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss.zzz"));
    pixmap.save(file, "PNG");
}

void PidWidget::goTargetPosition()
{
    _savePushButton->setEnabled(false);
    _goTargetPushButton->setEnabled(false);
    _dataLogger->clear();
    _dataLogger->start(10);

    _nodeProfile402->setTarget(_targetSpinBox->value());
    _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);

    _measurementTimer.singleShot(_windowSpinBox->value(), this, SLOT(stopMeasurementTimer()));
}

void PidWidget::stopMeasurementTimer()
{
    _nodeProfile402->setTarget(0);
    _dataLogger->stop();
    _savePushButton->setEnabled(true);
    _goTargetPushButton->setEnabled(true);
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
    pidLayout->addRow(tr("&P :"), _pSpinBox);

    _iSpinBox = new IndexSpinBox(_pidI_ObjId);
    _iSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("&I :"), _iSpinBox);

    _dSpinBox = new IndexSpinBox(_pidD_ObjId);
    _dSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    pidLayout->addRow(tr("&D :"), _dSpinBox);

    _periodSpinBox = new IndexSpinBox(_period_ObjId);
    pidLayout->addRow(tr("P&eriod :"), _periodSpinBox);

    pidGroupBox->setLayout(pidLayout);

    QGroupBox *targetGroupBox = new QGroupBox(tr("Target"));
    QFormLayout *targetLayout = new QFormLayout();

    _targetSpinBox = new QSpinBox();
    _targetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    targetLayout->addRow(tr("&Target :"), _targetSpinBox);

    _windowSpinBox = new QSpinBox();
    _windowSpinBox->setRange(10, 5000);
    _windowSpinBox->setValue(500);
    _windowSpinBox->setSuffix(" ms");
    targetLayout->addRow(tr("&Measurement window :"), _windowSpinBox);

    _savePushButton = new QPushButton(tr("&Save"));
    connect(_savePushButton, &QPushButton::clicked, this, &PidWidget::savePosition);

    _goTargetPushButton = new QPushButton(tr("G&O"));
    connect(_goTargetPushButton, &QPushButton::clicked, this, &PidWidget::goTargetPosition);

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
        case NodeProfile402::Mode::HM:
            return tr("PID Homing");
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
