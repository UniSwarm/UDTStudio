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

#include "motorwidget.h"

#include "canopen/widget/indexcombobox.h"
#include "canopen/widget/indexlabel.h"
#include "canopen/widget/indexspinbox.h"

#include "indexdb402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QWidget>

MotorWidget::MotorWidget(QWidget *parent)
    : QWidget(parent)
{
    createWidgets();
}

Node *MotorWidget::node() const
{
        return _node;
}

QString MotorWidget::title() const
{
    return tr("Motor config");
}

void MotorWidget::setNode(Node *node)
{
     _node = node;
    if (!_node)
    {
        return;
    }

    connect(_node, &Node::statusChanged, this, &MotorWidget::statusNodeChanged);

    _motorTypeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_TYPE));
    _peakCurrent->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_PEAK_CURRENT));
    _polePair->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_POLE_PAIR));
    _maxVelocity->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_MAX_VELOCITY));
    _velocityConstant->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_VELOCITY_CONSTANT));
    _currentConstant->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_CONF_CURRENT_CONSTANT));
    _motorTypeComboBox->setNode(node);
    _peakCurrent->setNode(node);
    _polePair->setNode(node);
    _maxVelocity->setNode(node);
    _velocityConstant->setNode(node);
    _currentConstant->setNode(node);

    _coderLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_CODER));
    _timeCoderLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_TIME_CODER));
    _phaseLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_PHASE));
    _bridgePwmLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_MOTOR_STATUS_BRIDGE_PWM));
    _coderLabel->setNode(node);
    _timeCoderLabel->setNode(node);
    _phaseLabel->setNode(node);
    _bridgePwmLabel->setNode(node);
}

void MotorWidget::statusNodeChanged(Node::Status status)
{
    if (status == Node::STOPPED)
    {
        this->setEnabled(false);
    }
    else
    {
        this->setEnabled(true);
    }
}

void MotorWidget::readAllObject()
{
    _motorTypeComboBox->readObject();
    _peakCurrent->readObject();
    _polePair->readObject();
    _maxVelocity->readObject();
    _velocityConstant->readObject();
    _currentConstant->readObject();

    _coderLabel->readObject();
    _timeCoderLabel->readObject();
    _phaseLabel->readObject();
    _bridgePwmLabel->readObject();
}

void MotorWidget::createWidgets()
{
    QWidget *motionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motionSensorWidget);

    actionLayout->addWidget(motorConfigWidgets());
    actionLayout->addWidget(motorStatusWidgets());

    QScrollArea *motionSensorScrollArea = new QScrollArea;
    motionSensorScrollArea->setWidget(motionSensorWidget);
    motionSensorScrollArea->setWidgetResizable(true);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(toolBarWidgets());
    vBoxLayout->addWidget(motionSensorScrollArea);
    vBoxLayout->setMargin(0);
    setLayout(vBoxLayout);
}

QToolBar *MotorWidget::toolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Data logger commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction * readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotorWidget::readAllObject);
    return toolBar;
}

QGroupBox *MotorWidget::motorConfigWidgets()
{
    QGroupBox *_motorConfigGroupBox = new QGroupBox(tr("Motor config"));
    QFormLayout *configLayout = new QFormLayout();

    _motorTypeComboBox = new IndexComboBox();
    _motorTypeComboBox->addItem("DC motor", QVariant(static_cast<uint16_t>(0x0101)));
    _motorTypeComboBox->addItem("BLDC trapezoidal", QVariant(static_cast<uint16_t>(0x0201)));
    _motorTypeComboBox->addItem("BLDC sinusoidal", QVariant(static_cast<uint16_t>(0x0202)));
    configLayout->addRow(tr("&Motor type :"), _motorTypeComboBox);

    _peakCurrent = new IndexSpinBox();
    configLayout->addRow(tr("P&eak current :"), _peakCurrent);

    _polePair = new IndexSpinBox();
    configLayout->addRow(tr("P&ole pair :"), _polePair);

    _maxVelocity = new IndexSpinBox();
    configLayout->addRow(tr("M&ax velocity :"), _maxVelocity);

    _velocityConstant = new IndexSpinBox();
    configLayout->addRow(tr("Ve&locity constant :"), _velocityConstant);

    _currentConstant = new IndexSpinBox();
    configLayout->addRow(tr("C&urrent constant :"), _currentConstant);

    _motorConfigGroupBox->setLayout(configLayout);

    return _motorConfigGroupBox;
}

QGroupBox *MotorWidget::motorStatusWidgets()
{
    QGroupBox *statusGroupBox = new QGroupBox(tr("Motor status"));
    QFormLayout *statusLayout = new QFormLayout();

    _coderLabel = new IndexLabel();
    statusLayout->addRow(tr("&Coder :"), _coderLabel);

    _timeCoderLabel = new IndexLabel();
    statusLayout->addRow(tr("&Time coder :"), _timeCoderLabel);

    _phaseLabel = new IndexLabel();
    statusLayout->addRow(tr("&Phase :"), _phaseLabel);

    _bridgePwmLabel = new IndexLabel();
    statusLayout->addRow(tr("&Bridge PWM :"), _bridgePwmLabel);

    statusGroupBox->setLayout(statusLayout);
    return statusGroupBox;
}
