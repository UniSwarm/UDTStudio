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
            _torqueDemandLabel->readObject();
            _torqueActualValueLabel->readObject();
        }
    }
}

void P402TqWidget::reset()
{
    _node->readObject(_torqueTargetObjectId);
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
        _torqueDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, axis);
        _torqueTargetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE, axis);
        _torqueActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, axis);
        _currentActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_CURRENT_ACTUAL_VALUE, axis);
        _dcLinkVoltageObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_DC_LINK_CIRCUIT_VOLTAGE, axis);

        _torqueDemandObjectId.setBusId(_node->busId());
        _torqueDemandObjectId.setNodeId(_node->nodeId());
        _torqueTargetObjectId.setBusId(_node->busId());
        _torqueTargetObjectId.setNodeId(_node->nodeId());
        _torqueActualValueObjectId.setBusId(_node->busId());
        _torqueActualValueObjectId.setNodeId(_node->nodeId());
        _currentActualValueObjectId.setBusId(_node->busId());
        _currentActualValueObjectId.setNodeId(_node->nodeId());
        _dcLinkVoltageObjectId.setBusId(_node->busId());
        _dcLinkVoltageObjectId.setNodeId(_node->nodeId());

        createWidgets();

        registerObjId(_torqueTargetObjectId);
        setNodeInterrest(_node);

        _torqueDemandLabel->setObjId(_torqueDemandObjectId);
        _torqueActualValueLabel->setObjId(_torqueActualValueObjectId);
        _currentActualValueLabel->setObjId(_currentActualValueObjectId);
        _dcLinkVoltageLabel->setObjId(_dcLinkVoltageObjectId);

        _targetSlopeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE, axis));
        _torqueProfileTypeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_PROFILE_TYPE, axis));
        _maxTorqueSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE, axis));
        _maxCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_CURRENT, axis));
        _motorRatedTorqueSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_TORQUE, axis));
        _motorRatedCurrentSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_CURRENT, axis));
        _targetSlopeSpinBox->setNode(node);
        _torqueProfileTypeSpinBox->setNode(node);
        _maxTorqueSpinBox->setNode(node);
        _maxCurrentSpinBox->setNode(node);
        _motorRatedTorqueSpinBox->setNode(node);
        _motorRatedCurrentSpinBox->setNode(node);

        int max = _node->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
        //        _tqTargetTorqueSlider->setValue(_node->nodeOd()->value(_tqTargetTorqueObjectId).toInt());
        _targetTorqueSlider->setRange(-max, max);
        _targetTorqueSlider->setTickInterval(max / 10);
        _sliderMinLabel->setNum(-max);
        _sliderMaxLabel->setNum(max);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        }

        connect(_maxTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::maxTorqueSpinboxFinished);
    }
}

void P402TqWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::OperationMode::TQ)
        {
            setEnabled(true);
            _node->readObject(_torqueTargetObjectId);
            _torqueDemandLabel->readObject();
            _torqueActualValueLabel->readObject();
        }
    }
}

void P402TqWidget::targetTorqueSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_targetTorqueSpinBox->value());
    _node->writeObject(_torqueTargetObjectId, QVariant(value));
    _targetTorqueSlider->setValue(value);
}

void P402TqWidget::targetTorqueSliderChanged()
{
    qint16 value = static_cast<qint16>(_targetTorqueSpinBox->value());
    _node->writeObject(_torqueTargetObjectId, QVariant(value));
}

void P402TqWidget::maxTorqueSpinboxFinished()
{
    int max = _node->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
    _targetTorqueSlider->setRange(-max, max);
    _sliderMinLabel->setNum(-max);
    _sliderMaxLabel->setNum(max);
}

void P402TqWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_torqueActualValueObjectId);
    dataLogger->addData(_torqueTargetObjectId);
    dataLogger->addData(_torqueDemandObjectId);
    _dataLoggerWidget->show();
}

void P402TqWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = NodeObjectId(0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(0x6041, 0, QMetaType::Type::UShort);

    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId, _torqueTargetObjectId};

    _node->rpdos().at(0)->writeMapping(tqRpdoObjectList);
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId, _torqueDemandObjectId};

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

    _targetTorqueSpinBox = new QSpinBox();
    _targetTorqueSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    tqLayout->addRow(tr("Target torque"), _targetTorqueSpinBox);

    _targetTorqueSlider = new QSlider(Qt::Horizontal);
    _targetTorqueSlider->setTickPosition(QSlider::TicksBelow);
    tqLayout->addRow(_targetTorqueSlider);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    tqLayout->addRow(labelSliderLayout);

    connect(_targetTorqueSlider, &QSlider::valueChanged, _targetTorqueSpinBox, &QSpinBox::setValue);
    connect(_targetTorqueSlider, &QSlider::valueChanged, this, &P402TqWidget::targetTorqueSliderChanged);
    connect(_targetTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::targetTorqueSpinboxFinished);

    _torqueDemandLabel = new IndexLabel();
    tqLayout->addRow(tr("Torque demand :"), _torqueDemandLabel);

    _torqueActualValueLabel = new IndexLabel();
    tqLayout->addRow(tr("Torque actual value :"), _torqueActualValueLabel);

    _currentActualValueLabel = new IndexLabel();
    //tqLayout->addRow(name, _tqCurrentActualValueLabel);

    _dcLinkVoltageLabel = new IndexLabel();
    //    tqLayout->addRow(tr("DC Link Voltage "), _tqDCLinkVoltageLabel);

    _targetSlopeSpinBox = new IndexSpinBox();
    tqLayout->addRow(tr("T&arget Slope "), _targetSlopeSpinBox);

    _maxTorqueSpinBox = new IndexSpinBox();
    tqLayout->addRow(tr("Ma&x torque "), _maxTorqueSpinBox);

    _torqueProfileTypeSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("To&rque Profile Type "), _tqTorqueProfileTypeSpinBox);

    _maxCurrentSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("Max &Current "), _tqMaxCurrentSpinBox);

    _motorRatedTorqueSpinBox = new IndexSpinBox();
    //    tqLayout->addRow(tr("Mot&or Rated Torque "), _tqMotorRatedTorqueSpinBox);

    _motorRatedCurrentSpinBox = new IndexSpinBox();
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

    if (objId == _torqueTargetObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            int value = _node->nodeOd()->value(objId).toInt();
            if (!_targetTorqueSpinBox->hasFocus())
            {
                _targetTorqueSpinBox->setValue(value);
            }
            if (!_targetTorqueSlider->isSliderDown())
            {
                //_tqTargetTorqueSlider->setValue(value);
            }
        }
    }
}
