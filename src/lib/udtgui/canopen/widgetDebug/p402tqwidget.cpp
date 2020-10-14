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

#include "p402tqwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexspinbox.h"
#include "services/services.h"
#include "indexdb402.h"

#include <QFormLayout>
#include <QPushButton>
#include <QRadioButton>

P402TqWidget::P402TqWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    createWidgets();
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
        _node->readObject(_tqTorqueDemandObjectId);
        _node->readObject(_tqTorqueActualValueObjectId);
    }
}

void P402TqWidget::setNode(Node *node)
{
    _tqTargetSlopeSpinBox->setNode(node);
    _tqTorqueProfileTypeSpinBox->setNode(node);
    _tqMaxTorqueSpinBox->setNode(node);

    _tqMaxCurrentSpinBox->setNode(node);
    _tqMotorRatedTorqueSpinBox->setNode(node);
    _tqMotorRatedCurrentSpinBox->setNode(node);

    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402TqWidget::updateData);
        }
    }

    _node = node;
    if (_node)
    {
        _tqTorqueDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND);
        _tqTargetTorqueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE);
        _tqTorqueActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE);
        _tqCurrentActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_CURRENT_ACTUAL_VALUE);
        _tqDCLinkVoltageObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_DC_LINK_CIRCUIT_VOLTAGE);
        _tqTorqueDemandObjectId.setBusId(_node->busId());
        _tqTorqueDemandObjectId.setNodeId(_node->nodeId());
        _tqTargetTorqueObjectId .setBusId(_node->busId());
        _tqTargetTorqueObjectId.setNodeId(_node->nodeId());
        _tqTorqueActualValueObjectId.setBusId(_node->busId());
        _tqTorqueActualValueObjectId.setNodeId(_node->nodeId());
        _tqCurrentActualValueObjectId.setBusId(_node->busId());
        _tqCurrentActualValueObjectId.setNodeId(_node->nodeId());
        _tqDCLinkVoltageObjectId.setBusId(_node->busId());
        _tqDCLinkVoltageObjectId.setNodeId(_node->nodeId());

        registerObjId({_tqTargetTorqueObjectId});
        registerObjId({_tqTorqueDemandObjectId});
        registerObjId({_tqTorqueActualValueObjectId});
        registerObjId({_tqCurrentActualValueObjectId});
        registerObjId({_tqDCLinkVoltageObjectId});
        setNodeInterrest(_node);

        _tqTargetSlopeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE));
        _tqTorqueProfileTypeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_PROFILE_TYPE));
        _tqMaxTorqueSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE));
        _tqMaxCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_CURRENT));
        _tqMotorRatedTorqueSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_TORQUE));
        _tqMotorRatedCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_CURRENT));

        connect(_node, &Node::statusChanged, this, &P402TqWidget::updateData);
    }
}

void P402TqWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);
            _node->readObject(_tqTargetTorqueObjectId);
            _node->readObject(_tqTorqueDemandObjectId);
            _node->readObject(_tqTorqueActualValueObjectId);
            _node->readObject(_tqCurrentActualValueObjectId);
            _node->readObject(_tqDCLinkVoltageObjectId);
        }
    }
}

void P402TqWidget::tqTargetTorqueSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_tqTargetTorqueSpinBox->value());
    _node->writeObject(_tqTargetTorqueObjectId, QVariant(value));
    _tqTargetTorqueSlider->setValue(value);
}

void P402TqWidget::tqTargetTorqueSliderChanged()
{
    qint16 value = static_cast<qint16>(_tqTargetTorqueSpinBox->value());
    _node->writeObject(_tqTargetTorqueObjectId, QVariant(value));
}

void P402TqWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData({_tqTorqueActualValueObjectId});
    dataLogger->addData({_tqTargetTorqueObjectId});
    dataLogger->addData({_tqTorqueDemandObjectId});
    _dataLoggerWidget->show();
}

void P402TqWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = NodeObjectId(0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(0x6041, 0, QMetaType::Type::UShort);

    QList<NodeObjectId> tqRpdoObjectList = {{controlWordObjectId},
                                            {_tqTargetTorqueObjectId}};

    _node->rpdos().at(0)->writeMapping(tqRpdoObjectList);
    QList<NodeObjectId> tqTpdoObjectList = {{statusWordObjectId},
                                            {_tqTorqueDemandObjectId}};

    _node->tpdos().at(2)->writeMapping(tqTpdoObjectList);
}


void P402TqWidget::refreshData(NodeObjectId object)
{
    int value;
    if (_node->nodeOd()->indexExist(object.index()))
    {
        if (object == _tqTargetTorqueObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            if (!_tqTargetTorqueSpinBox->hasFocus())
            {
                _tqTargetTorqueSpinBox->setValue(value);
            }
            if (!_tqTargetTorqueSlider->isSliderDown())
            {
                //_tqTargetTorqueSlider->setValue(value);
            }
        }

        if (object == _tqTorqueActualValueObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqTorqueActualValueLabel->setNum(value);
        }
        if (object == _tqCurrentActualValueObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqCurrentActualValueLabel->setNum(value);
        }
        if (object == _tqDCLinkVoltageObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqDCLinkVoltageLabel->setNum(value);
        }

        if (object == _tqTorqueDemandObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqTorqueDemandLabel->setNum(value);
        }

    }
}

void P402TqWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box TQ mode
    QGroupBox *tqGroupBox = new QGroupBox(tr("Torque mode"));
    QFormLayout *tqLayout = new QFormLayout();

    _tqTargetTorqueSpinBox = new QSpinBox();
    _tqTargetTorqueSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    tqLayout->addRow("Target torque (0x6071) :", _tqTargetTorqueSpinBox);

    _tqTargetTorqueSlider = new QSlider(Qt::Horizontal);
    _tqTargetTorqueSlider->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    tqLayout->addRow(_tqTargetTorqueSlider);

    connect(_tqTargetTorqueSlider, &QSlider::valueChanged, _tqTargetTorqueSpinBox, &QSpinBox::setValue);
    connect(_tqTargetTorqueSlider, &QSlider::valueChanged, this, &P402TqWidget::tqTargetTorqueSliderChanged);
    connect(_tqTargetTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqTargetTorqueSpinboxFinished);

    _tqTorqueDemandLabel = new QLabel();
    _tqTorqueDemandLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tqLayout->addRow("Torque demand (0x6074) :", _tqTorqueDemandLabel);

    _tqTorqueActualValueLabel = new QLabel();
    _tqTorqueActualValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tqLayout->addRow("Torque actual value (0x6077) :", _tqTorqueActualValueLabel);

    _tqCurrentActualValueLabel = new QLabel();
    _tqCurrentActualValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tqLayout->addRow("Current Actual Value (0x6078) :", _tqCurrentActualValueLabel);

    _tqDCLinkVoltageLabel = new QLabel();
    _tqDCLinkVoltageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    tqLayout->addRow("DC Link Voltage (0x6079) :", _tqDCLinkVoltageLabel);

    _tqTargetSlopeSpinBox = new IndexSpinBox();
    _tqTargetSlopeSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    tqLayout->addRow(tr("T&arget Slope (0x6087) :"), _tqTargetSlopeSpinBox);

    _tqMaxTorqueSpinBox = new IndexSpinBox();
    _tqMaxTorqueSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    tqLayout->addRow(tr("Ma&x torque (0x6072) :"), _tqMaxTorqueSpinBox);

    _tqTorqueProfileTypeSpinBox = new IndexSpinBox();
    _tqTorqueProfileTypeSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    tqLayout->addRow(tr("To&rque Profile Type (0x6088) :"), _tqTorqueProfileTypeSpinBox);

    _tqMaxCurrentSpinBox = new IndexSpinBox();
    _tqMaxCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    tqLayout->addRow(tr("Max &Current (0x6073) :"), _tqMaxCurrentSpinBox);

    _tqMotorRatedTorqueSpinBox = new IndexSpinBox();
    _tqMotorRatedTorqueSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    tqLayout->addRow(tr("Mot&or Rated Torque (0x6076) :"), _tqMotorRatedTorqueSpinBox);

    _tqMotorRatedCurrentSpinBox = new IndexSpinBox();
    _tqMotorRatedCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    tqLayout->addRow(tr("Motor Rate&d Current (0x6075) :"), _tqMotorRatedCurrentSpinBox);

    tqGroupBox->setLayout(tqLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4, bit 5, bit 6, bit 8"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    QLabel *tqHaltLabel = new QLabel(tr("Halt ramp (bit 8) : Motor stopped"));
    modeControlWordLayout->addRow(tqHaltLabel);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402TqWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402TqWidget::pdoMapping);

    QPixmap tqModePixmap;
    QLabel *tqModeLabel;
    tqModeLabel = new QLabel();
    tqModePixmap.load(":/diagram/img/402TQDiagram.png");
    tqModeLabel->setPixmap(tqModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram TQ mode"));
    connect(imgPushButton, SIGNAL(clicked()), tqModeLabel, SLOT(show()));
    QHBoxLayout *tqButtonLayout = new QHBoxLayout();
    tqButtonLayout->addWidget(dataLoggerPushButton);
    tqButtonLayout->addWidget(mappingPdoPushButton);
    tqButtonLayout->addWidget(imgPushButton);

    layout->addWidget(tqGroupBox);
    layout->addWidget(modeControlWordGroupBox);
    layout->addItem(tqButtonLayout);

    setLayout(layout);
}

void P402TqWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if ((objId == _tqTargetTorqueObjectId)
        || (objId == _tqTorqueDemandObjectId)
        || (objId == _tqTorqueActualValueObjectId)
        || (objId == _tqCurrentActualValueObjectId)
        || (objId == _tqDCLinkVoltageObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId);
    }
}
