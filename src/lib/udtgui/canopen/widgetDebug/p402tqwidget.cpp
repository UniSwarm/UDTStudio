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
#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>

P402TqWidget::P402TqWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
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
        if (_nodeProfile402->actualMode() == NodeProfile402::Mode::TQ)
        {
            _node->readObject(_tqTorqueDemandObjectId);
            _node->readObject(_tqTorqueActualValueObjectId);
        }
    }
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
        _tqTargetTorqueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE, axis);
        _tqTorqueActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, axis);
        _tqCurrentActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_CURRENT_ACTUAL_VALUE, axis);
        _tqDCLinkVoltageObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_DC_LINK_CIRCUIT_VOLTAGE, axis);

        _tqTorqueDemandObjectId.setBusId(_node->busId());
        _tqTorqueDemandObjectId.setNodeId(_node->nodeId());
        _tqTargetTorqueObjectId.setBusId(_node->busId());
        _tqTargetTorqueObjectId.setNodeId(_node->nodeId());
        _tqTorqueActualValueObjectId.setBusId(_node->busId());
        _tqTorqueActualValueObjectId.setNodeId(_node->nodeId());
        _tqCurrentActualValueObjectId.setBusId(_node->busId());
        _tqCurrentActualValueObjectId.setNodeId(_node->nodeId());
        _tqDCLinkVoltageObjectId.setBusId(_node->busId());
        _tqDCLinkVoltageObjectId.setNodeId(_node->nodeId());

        createWidgets();

        registerObjId(_tqTargetTorqueObjectId);
        registerObjId(_tqTorqueDemandObjectId);
        registerObjId(_tqTorqueActualValueObjectId);
        registerObjId(_tqCurrentActualValueObjectId);
        registerObjId(_tqDCLinkVoltageObjectId);
        setNodeInterrest(_node);

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
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::Mode::TQ)
        {
            setEnabled(true);
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
    dataLogger->addData(_tqTargetTorqueObjectId);
    dataLogger->addData(_tqTorqueDemandObjectId);
    _dataLoggerWidget->show();
}

void P402TqWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = NodeObjectId(0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(0x6041, 0, QMetaType::Type::UShort);

    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId,
                                            _tqTargetTorqueObjectId};

    _node->rpdos().at(0)->writeMapping(tqRpdoObjectList);
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId,
                                            _tqTorqueDemandObjectId};

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
    QString name;
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box TQ mode
    QGroupBox *tqGroupBox = new QGroupBox(tr("Torque mode"));
    QFormLayout *tqLayout = new QFormLayout();

    _tqTargetTorqueSpinBox = new QSpinBox();
    _tqTargetTorqueSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    name = tr("Target torque (0x%1) :").arg(QString::number(_tqTargetTorqueObjectId.index(), 16).toUpper());
    tqLayout->addRow(name, _tqTargetTorqueSpinBox);

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

    _tqTorqueDemandLabel = new QLabel("-");
    _tqTorqueDemandLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Torque demand ") + QString("(0x%1) :").arg(QString::number(_tqTorqueDemandObjectId.index(), 16).toUpper());
    tqLayout->addRow(name, _tqTorqueDemandLabel);

    _tqTorqueActualValueLabel = new QLabel("-");
    _tqTorqueActualValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Torque actual value ") + QString("(0x%1) :").arg(QString::number(_tqTorqueActualValueObjectId.index(), 16).toUpper());
    tqLayout->addRow(name, _tqTorqueActualValueLabel);

    _tqCurrentActualValueLabel = new QLabel("-");
    _tqCurrentActualValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Current Actual Value ") + QString("(0x%1) :").arg(QString::number(_tqCurrentActualValueObjectId.index(), 16).toUpper());
    tqLayout->addRow(name, _tqCurrentActualValueLabel);

    _tqDCLinkVoltageLabel = new QLabel("-");
    _tqDCLinkVoltageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("DC Link Voltage ") + QString("(0x%1) :").arg(QString::number(_tqDCLinkVoltageObjectId.index(), 16).toUpper());
    tqLayout->addRow(name, _tqDCLinkVoltageLabel);

    _tqTargetSlopeSpinBox = new IndexSpinBox();
    _tqTargetSlopeSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    name = tr("T&arget Slope ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE, _axis).index(), 16).toUpper());
    tqLayout->addRow(name, _tqTargetSlopeSpinBox);

    _tqMaxTorqueSpinBox = new IndexSpinBox();
    _tqMaxTorqueSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    name = tr("Ma&x torque ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE, _axis).index(), 16).toUpper());
    tqLayout->addRow(name, _tqMaxTorqueSpinBox);

    _tqTorqueProfileTypeSpinBox = new IndexSpinBox();
    _tqTorqueProfileTypeSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    name = tr("To&rque Profile Type ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_PROFILE_TYPE, _axis).index(), 16).toUpper());
    tqLayout->addRow(name, _tqTorqueProfileTypeSpinBox);

    _tqMaxCurrentSpinBox = new IndexSpinBox();
    _tqMaxCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    name = tr("Max &Current ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_CURRENT, _axis).index(), 16).toUpper());
    tqLayout->addRow(name, _tqMaxCurrentSpinBox);

    _tqMotorRatedTorqueSpinBox = new IndexSpinBox();
    _tqMotorRatedTorqueSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    name = tr("Mot&or Rated Torque ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_TORQUE, _axis).index(), 16).toUpper());
    tqLayout->addRow(name, _tqMotorRatedTorqueSpinBox);

    _tqMotorRatedCurrentSpinBox = new IndexSpinBox();
    _tqMotorRatedCurrentSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    name = tr("Motor Rate&d Current ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_CURRENT, _axis).index(), 16).toUpper());
    tqLayout->addRow(name, _tqMotorRatedCurrentSpinBox);
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
    tqButtonLayout->addWidget(dataLoggerPushButton);
    tqButtonLayout->addWidget(mappingPdoPushButton);
    tqButtonLayout->addWidget(imgPushButton);

    layout->addWidget(tqGroupBox);
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
