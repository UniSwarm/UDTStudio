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
#include "services/services.h"
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

void P402TqWidget::setNode(Node *node)
{
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
        _tqTorqueDemandObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6074, 0);
        _tqTargetTorqueObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6071, 0);
        _tqTargetSlopeObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6087, 0);
        _tqTorqueProfileTypeObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6088, 0);
        _tqMaxTorqueObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6072, 0);

        _tqMaxCurrentObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6073, 0);
        _tqMotorRatedTorqueObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6076, 0);
        _tqMotorRatedCurrentObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6075, 0);

        _tqTorqueActualValueObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6077, 0);
        _tqCurrentActualValueObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6078, 0);
        _tqDCLinkVoltageObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6079, 0);

//        registerObjId({_controlWordObjectId});
        registerObjId({_tqTargetTorqueObjectId});
        registerObjId({_tqTorqueDemandObjectId});
        registerObjId({_tqTargetSlopeObjectId});
        registerObjId({_tqTorqueProfileTypeObjectId});
        registerObjId({_tqMaxTorqueObjectId});
        registerObjId({_tqMaxCurrentObjectId});
        registerObjId({_tqMotorRatedTorqueObjectId});
        registerObjId({_tqMotorRatedCurrentObjectId});
        registerObjId({_tqTorqueActualValueObjectId});
        registerObjId({_tqCurrentActualValueObjectId});
        registerObjId({_tqDCLinkVoltageObjectId});

        setNodeInterrest(_node);

        connect(_node, &Node::statusChanged, this, &P402TqWidget::updateData);
        updateData();
    }
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

void P402TqWidget::updateData()
{
    if (_node)
    {
        this->setEnabled(true);
        _node->readObject(_tqTargetTorqueObjectId);
        _node->readObject(_tqTorqueDemandObjectId);
        _node->readObject(_tqTargetSlopeObjectId);
        _node->readObject(_tqTorqueProfileTypeObjectId);
        _node->readObject(_tqMaxTorqueObjectId);
        _node->readObject(_tqMaxCurrentObjectId);
        _node->readObject(_tqMotorRatedTorqueObjectId);
        _node->readObject(_tqMotorRatedCurrentObjectId);
        _node->readObject(_tqTorqueActualValueObjectId);
        _node->readObject(_tqCurrentActualValueObjectId);
        _node->readObject(_tqDCLinkVoltageObjectId);
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
void P402TqWidget::tqTargetSlopeEditingFinished()
{
    quint32 value = static_cast<quint32>(_tqTargetSlopeSpinBox->value());
    _node->writeObject(_tqTargetSlopeObjectId, QVariant(value));
}
void P402TqWidget::tqMaxTorqueEditingFinished()
{
    quint16 value = static_cast<quint16>(_tqMaxTorqueSpinBox->value());
    _node->writeObject(_tqMaxTorqueObjectId, QVariant(value));
}
void P402TqWidget::tqTorqueProfileTypeEditingFinished()
{
    qint16 value = static_cast<qint16>(_tqTorqueProfileTypeSpinBox->value());
    _node->writeObject(_tqTorqueProfileTypeObjectId, QVariant(value));
}
void P402TqWidget::tqMaxCurrentEditingFinished()
{
    quint16 value = static_cast<quint16>(_tqMaxCurrentSpinBox->value());
    _node->writeObject(_tqMaxCurrentObjectId, QVariant(value));
}
void P402TqWidget::tqMotorRatedTorqueEditingFinished()
{
    quint32 value = static_cast<quint32>(_tqMotorRatedTorqueSpinBox->value());
    _node->writeObject(_tqMotorRatedTorqueObjectId, QVariant(value));
}
void P402TqWidget::tqMotorRatedCurrentEditingFinished()
{
    quint32 value = static_cast<quint32>(_tqMotorRatedCurrentSpinBox->value());
    _node->writeObject(_tqMotorRatedCurrentObjectId, QVariant(value));
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

    _tqTargetSlopeSpinBox = new QSpinBox();
    tqLayout->addRow(tr("Target Slope (0x6087) :"), _tqTargetSlopeSpinBox);
    _tqTargetSlopeSpinBox->setRange(0, std::numeric_limits<int>::max());
    connect(_tqTargetSlopeSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqTargetSlopeEditingFinished);

    _tqMaxTorqueSpinBox = new QSpinBox();
    tqLayout->addRow(tr("Max torque (0x6072) :"), _tqMaxTorqueSpinBox);
    _tqMaxTorqueSpinBox->setRange(0,  std::numeric_limits<qint16>::max());
    connect(_tqMaxTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqMaxTorqueEditingFinished);

    _tqTorqueProfileTypeSpinBox = new QSpinBox();
    tqLayout->addRow(tr("Torque Profile Type (0x6088) :"), _tqTorqueProfileTypeSpinBox);
    //_tqTorqueProfileTypeSpinBox->setRange(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
    _tqTorqueProfileTypeSpinBox->setRange(0, 0);
    connect(_tqTorqueProfileTypeSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqTorqueProfileTypeEditingFinished);

    _tqMaxCurrentSpinBox = new QSpinBox();
    tqLayout->addRow(tr("Max Current (0x6073) :"), _tqMaxCurrentSpinBox);
    _tqMaxCurrentSpinBox->setRange(std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
    connect(_tqMaxCurrentSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqMaxCurrentEditingFinished);

    _tqMotorRatedTorqueSpinBox = new QSpinBox();
    tqLayout->addRow(tr("Motor Rated Torque (0x6076) :"), _tqMotorRatedTorqueSpinBox);
    _tqMotorRatedTorqueSpinBox->setRange(0, std::numeric_limits<int>::max());
    connect(_tqMotorRatedTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqMotorRatedTorqueEditingFinished);

    _tqMotorRatedCurrentSpinBox = new QSpinBox();
    tqLayout->addRow(tr("Motor Rated Current (0x6075) :"), _tqMotorRatedCurrentSpinBox);
    _tqMotorRatedCurrentSpinBox->setRange(0, std::numeric_limits<int>::max());
    connect(_tqMotorRatedCurrentSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::tqMotorRatedCurrentEditingFinished);

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
    NodeObjectId controlWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6041, 0, QMetaType::Type::UShort);

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
    if (_node->nodeOd()->indexExist(object.index))
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
        if (object == _tqTargetSlopeObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqTargetSlopeSpinBox->setValue(value);
        }
        if (object == _tqTorqueProfileTypeObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqTorqueProfileTypeSpinBox->setValue(value);
        }
        if (object == _tqMaxTorqueObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqMaxTorqueSpinBox->setValue(value);
            _tqTargetTorqueSlider->setMinimum(-value);
            _tqTargetTorqueSlider->setMaximum(value);
        }

        if (object == _tqMaxCurrentObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqMaxCurrentSpinBox->setValue(value);
        }

        if (object == _tqMotorRatedTorqueObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqMotorRatedTorqueSpinBox->setValue(value);
        }
        if (object == _tqMotorRatedCurrentObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _tqMotorRatedCurrentSpinBox->setValue(value);
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

void P402TqWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!_node)
    {
        return;
    }

    if ((objId == _tqTargetTorqueObjectId)
        || (objId == _tqTorqueDemandObjectId)
        || (objId == _tqTargetSlopeObjectId)
        || (objId == _tqTorqueProfileTypeObjectId)
        || (objId == _tqMaxTorqueObjectId)
        || (objId == _tqMaxCurrentObjectId)
        || (objId == _tqMotorRatedTorqueObjectId)
        || (objId == _tqMotorRatedCurrentObjectId)
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
