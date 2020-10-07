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
    createWidgets();
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
    _nodeProfile402 = static_cast<NodeProfile402 *>(_node->profiles()[0]);
}
void PidWidget::setMode(PidWidget::ModePid mode)
{
    NodeObjectId pidP_ObjId;
    NodeObjectId pidI_ObjId;
    NodeObjectId pidD_ObjId;
    NodeObjectId period_ObjId;
    NodeObjectId target_ObjId;
    NodeObjectId pidInputStatus_ObjId;
    NodeObjectId pidErrorStatus_ObjId;
    NodeObjectId pidIntegratorStatus_ObjId;
    NodeObjectId pidOutputStatus_ObjId;

    _mode = mode;
    switch (_mode)
    {
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

            _nodeProfile402->setMode(NodeProfile402::VL);
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

            _nodeProfile402->setMode(NodeProfile402::TQ);
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

            _nodeProfile402->setMode(NodeProfile402::PP);
            break;
        }
    }

    _pSpinBox->setObjId(pidP_ObjId);
    _iSpinBox->setObjId(pidI_ObjId);
    _dSpinBox->setObjId(pidD_ObjId);
    _periodSpinBox->setObjId(period_ObjId);

    _dataLogger->addData(pidInputStatus_ObjId);
    _dataLogger->addData(pidErrorStatus_ObjId);
    _dataLogger->addData(pidIntegratorStatus_ObjId);
    _dataLogger->addData(pidOutputStatus_ObjId);
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
        case MODE_PID_POSITION:
            return tr("PID Position");
        case MODE_PID_VELOCITY:
            return tr("PID Velocity");
        case MODE_PID_TORQUE:
            return tr("PID Torque");
    }
    return QString();
}
