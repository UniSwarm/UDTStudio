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

#include "p402tqwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexspinbox.h"
#include "canopen/widget/indexlabel.h"
#include "services/services.h"

#include "indexdb402.h"
#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modetq.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>

P402TqWidget::P402TqWidget(QWidget *parent)
    : QScrollArea(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);

    _node = nullptr;
    _nodeProfile402 = nullptr;
}

P402TqWidget::~P402TqWidget()
{
    unRegisterFullOd();
}

Node *P402TqWidget::node() const
{
    return _node;
}

void P402TqWidget::readData()
{
    if (_node)
    {
        if (_nodeProfile402->actualMode() == NodeProfile402::OperationMode::TQ)
        {
            _tqTorqueDemandLabel->readObject();
            _tqTorqueActualValueLabel->readObject();
        }
    }
}

void P402TqWidget::reset()
{
    _node->readObject(_tqTorqueTargetObjectId);
}

void P402TqWidget::setNode(Node *node, uint8_t axis)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402TqWidget::updateData);
        }
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    _node = node;
    if (_node)
    {
        _tqTorqueDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, axis);
        _tqTorqueTargetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE, axis);
        _tqTorqueActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, axis);
        _tqCurrentActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_CURRENT_ACTUAL_VALUE, axis);
        _tqDCLinkVoltageObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_DC_LINK_CIRCUIT_VOLTAGE, axis);

        _tqTorqueDemandObjectId.setBusId(_node->busId());
        _tqTorqueDemandObjectId.setNodeId(_node->nodeId());
        _tqTorqueTargetObjectId.setBusId(_node->busId());
        _tqTorqueTargetObjectId.setNodeId(_node->nodeId());
        _tqTorqueActualValueObjectId.setBusId(_node->busId());
        _tqTorqueActualValueObjectId.setNodeId(_node->nodeId());
        _tqCurrentActualValueObjectId.setBusId(_node->busId());
        _tqCurrentActualValueObjectId.setNodeId(_node->nodeId());
        _tqDCLinkVoltageObjectId.setBusId(_node->busId());
        _tqDCLinkVoltageObjectId.setNodeId(_node->nodeId());

        createWidgets();

        registerObjId(_tqTorqueTargetObjectId);
        setNodeInterrest(_node);

        _tqTorqueDemandLabel->setObjId(_tqTorqueDemandObjectId);
        _tqTorqueActualValueLabel->setObjId(_tqTorqueActualValueObjectId);
        _tqCurrentActualValueLabel->setObjId(_tqCurrentActualValueObjectId);
        _tqDCLinkVoltageLabel->setObjId(_tqDCLinkVoltageObjectId);

        _tqTargetSlopeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE, axis));
        _tqTorqueProfileTypeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_PROFILE_TYPE, axis));
        _tqMaxTorqueSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE, axis));
        _tqMaxCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_CURRENT, axis));
        _tqMotorRatedTorqueSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_TORQUE, axis));
        _tqMotorRatedCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_CURRENT, axis));
        _tqTargetSlopeSpinBox->setNode(node);
        _tqTorqueProfileTypeSpinBox->setNode(node);
        _tqMaxTorqueSpinBox->setNode(node);
        _tqMaxCurrentSpinBox->setNode(node);
        _tqMotorRatedTorqueSpinBox->setNode(node);
        _tqMotorRatedCurrentSpinBox->setNode(node);

        int max = _node->nodeOd()->value(_tqMaxTorqueSpinBox->objId()).toInt();
        //        _tqTargetTorqueSlider->setValue(_node->nodeOd()->value(_tqTargetTorqueObjectId).toInt());
        _tqTargetTorqueSlider->setRange(-max, max);
        _tqTargetTorqueSlider->setTickInterval(max / 10);
        _tqSliderMinLabel->setNum(-max);
        _tqSliderMaxLabel->setNum(max);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        }

        connect(_tqMaxTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqMaxTorqueSpinboxFinished);
    }
}

void P402TqWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::OperationMode::TQ)
        {
            setEnabled(true);
            _node->readObject(_tqTorqueTargetObjectId);
            _tqTorqueDemandLabel->readObject();
            _tqTorqueActualValueLabel->readObject();
        }
    }
}

void P402TqWidget::tqTargetTorqueSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_tqTargetTorqueSpinBox->value());
    _node->writeObject(_tqTorqueTargetObjectId, QVariant(value));
    _tqTargetTorqueSlider->setValue(value);
}

void P402TqWidget::tqTargetTorqueSliderChanged()
{
    qint16 value = static_cast<qint16>(_tqTargetTorqueSpinBox->value());
    _node->writeObject(_tqTorqueTargetObjectId, QVariant(value));
}

void P402TqWidget::tqMaxTorqueSpinboxFinished()
{
    int max = _node->nodeOd()->value(_tqMaxTorqueSpinBox->objId()).toInt();
    _tqTargetTorqueSlider->setRange(-max, max);
    _tqSliderMinLabel->setNum(-max);
    _tqSliderMaxLabel->setNum(max);
}

void P402TqWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_tqTorqueActualValueObjectId);
    dataLogger->addData(_tqTorqueTargetObjectId);
    dataLogger->addData(_tqTorqueDemandObjectId);
    _dataLoggerWidget->show();
}

void P402TqWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = NodeObjectId(0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(0x6041, 0, QMetaType::Type::UShort);

    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId, _tqTorqueTargetObjectId};

    _node->rpdos().at(0)->writeMapping(tqRpdoObjectList);
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId, _tqTorqueDemandObjectId};

    _node->tpdos().at(2)->writeMapping(tqTpdoObjectList);
}

void P402TqWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box TQ mode
    QGroupBox *tqGroupBox = new QGroupBox(tr("Torque mode"));
    QFormLayout *tqLayout = new QFormLayout();

    _tqTargetTorqueSpinBox = new QSpinBox();
    _tqTargetTorqueSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    tqLayout->addRow(tr("Target torque"), _tqTargetTorqueSpinBox);

    _tqTargetTorqueSlider = new QSlider(Qt::Horizontal);
    _tqTargetTorqueSlider->setTickPosition(QSlider::TicksBelow);
    tqLayout->addRow(_tqTargetTorqueSlider);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _tqSliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_tqSliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _tqSliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_tqSliderMaxLabel);
    tqLayout->addRow(labelSliderLayout);

    connect(_tqTargetTorqueSlider, &QSlider::valueChanged, _tqTargetTorqueSpinBox, &QSpinBox::setValue);
    connect(_tqTargetTorqueSlider, &QSlider::valueChanged, this, &P402TqWidget::tqTargetTorqueSliderChanged);
    connect(_tqTargetTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqTargetTorqueSpinboxFinished);

    _tqTorqueDemandLabel = new IndexLabel();
    tqLayout->addRow(tr("Torque demand :"), _tqTorqueDemandLabel);

    _tqTorqueActualValueLabel = new IndexLabel();
    tqLayout->addRow(tr("Torque actual value :"), _tqTorqueActualValueLabel);

    _tqCurrentActualValueLabel = new IndexLabel();
    //tqLayout->addRow(name, _tqCurrentActualValueLabel);

    _tqDCLinkVoltageLabel = new IndexLabel();
    //    tqLayout->addRow(tr("DC Link Voltage "), _tqDCLinkVoltageLabel);

    _tqTargetSlopeSpinBox = new IndexSpinBox();
    tqLayout->addRow(tr("T&arget Slope "), _tqTargetSlopeSpinBox);

    _tqMaxTorqueSpinBox = new IndexSpinBox();
    tqLayout->addRow(tr("Ma&x torque "), _tqMaxTorqueSpinBox);

    _tqTorqueProfileTypeSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("To&rque Profile Type "), _tqTorqueProfileTypeSpinBox);

    _tqMaxCurrentSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("Max &Current "), _tqMaxCurrentSpinBox);

    _tqMotorRatedTorqueSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("Mot&or Rated Torque "), _tqMotorRatedTorqueSpinBox);

    _tqMotorRatedCurrentSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("Motor Rate&d Current "), _tqMotorRatedCurrentSpinBox);
    tqGroupBox->setLayout(tqLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402TqWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402TqWidget::pdoMapping);

    QPixmap tqModePixmap;
    QLabel *tqModeLabel;
    tqModeLabel = new QLabel();
    tqModePixmap.load(":/diagram/img/diagrams/402TQDiagram.png");
    tqModeLabel->setPixmap(tqModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram TQ mode"));
    connect(imgPushButton, SIGNAL(clicked()), tqModeLabel, SLOT(show()));
    QHBoxLayout *tqButtonLayout = new QHBoxLayout();
    tqButtonLayout->setSpacing(5);
    tqButtonLayout->addWidget(dataLoggerPushButton);
    tqButtonLayout->addWidget(mappingPdoPushButton);
    tqButtonLayout->addWidget(imgPushButton);

    layout->addWidget(tqGroupBox);
    layout->addItem(tqButtonLayout);

    widget->setLayout(layout);
    setWidget(widget);
}

void P402TqWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if (objId == _tqTorqueTargetObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            int value = _node->nodeOd()->value(objId).toInt();
            if (!_tqTargetTorqueSpinBox->hasFocus())
            {
                _tqTargetTorqueSpinBox->setValue(value);
            }
            if (!_tqTargetTorqueSlider->isSliderDown())
            {
                //_tqTargetTorqueSlider->setValue(value);
            }
        }
    }
}
