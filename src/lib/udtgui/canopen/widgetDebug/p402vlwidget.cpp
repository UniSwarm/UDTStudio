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

#include "p402vlwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexspinbox.h"
#include "services/services.h"

#include "indexdb402.h"
#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modevl.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>

P402VlWidget::P402VlWidget(QWidget *parent)
    : QScrollArea(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);

    _node = nullptr;
    _nodeProfile402 = nullptr;
}

P402VlWidget::~P402VlWidget()
{
    unRegisterFullOd();
}

Node *P402VlWidget::node() const
{
    return _node;
}

void P402VlWidget::readData()
{
    if (_node)
    {
        if (_nodeProfile402->actualMode() == NodeProfile402::Mode::VL)
        {
            _node->readObject(_vlVelocityDemandObjectId);
            _node->readObject(_vlVelocityActualObjectId);
        }
    }
}

void P402VlWidget::reset()
{
    _node->readObject(_vlTargetVelocityObjectId);
}

void P402VlWidget::setNode(Node *node, uint8_t axis)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402VlWidget::updateData);
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
        _vlVelocityDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND, axis);
        _vlVelocityActualObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE, axis);
        _vlTargetVelocityObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET, axis);

        createWidgets();

        _vlTargetVelocityObjectId.setBusId(_node->busId());
        _vlVelocityDemandObjectId.setBusId(_node->busId());
        _vlVelocityActualObjectId.setBusId(_node->busId());
        _vlTargetVelocityObjectId.setNodeId(_node->nodeId());
        _vlVelocityDemandObjectId.setNodeId(_node->nodeId());
        _vlVelocityActualObjectId.setNodeId(_node->nodeId());
        _vlTargetVelocityObjectId.setDataType(QMetaType::Type::Short);
        _vlVelocityDemandObjectId.setDataType(QMetaType::Type::Short);
        _vlVelocityActualObjectId.setDataType(QMetaType::Type::Short);

        registerObjId(_vlTargetVelocityObjectId);
        registerObjId(_vlVelocityDemandObjectId);
        registerObjId(_vlVelocityActualObjectId);

        setNodeInterrest(node);

        _vlMinVelocityMinMaxAmountSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_MIN, axis));
        _vlMaxVelocityMinMaxAmountSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_MAX, axis));
        _vlAccelerationDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_SPEED, axis));
        _vlAccelerationDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_TIME, axis));
        _vlDecelerationDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_SPEED, axis));
        _vlDecelerationDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_TIME, axis));
        _vlQuickStopDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_SPEED, axis));
        _vlQuickStopDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_TIME, axis));
        _vlSetPointFactorNumeratorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_NUMERATOR, axis));
        _vlSetPointFactorDenominatorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_DENOMINATOR, axis));
        _vlDimensionFactorNumeratorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_NUMERATOR, axis));
        _vlDimensionFactorDenominatorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_DENOMINATOR, axis));

        _vlMinVelocityMinMaxAmountSpinBox->setNode(node);
        _vlMaxVelocityMinMaxAmountSpinBox->setNode(node);
        _vlAccelerationDeltaSpeedSpinBox->setNode(node);
        _vlAccelerationDeltaTimeSpinBox->setNode(node);
        _vlDecelerationDeltaSpeedSpinBox->setNode(node);
        _vlDecelerationDeltaTimeSpinBox->setNode(node);
        _vlQuickStopDeltaSpeedSpinBox->setNode(node);
        _vlQuickStopDeltaTimeSpinBox->setNode(node);
        _vlSetPointFactorNumeratorSpinBox->setNode(node);
        _vlSetPointFactorDenominatorSpinBox->setNode(node);
        _vlDimensionFactorNumeratorSpinBox->setNode(node);
        _vlDimensionFactorDenominatorSpinBox->setNode(node);

        int max = _node->nodeOd()->value(_vlMaxVelocityMinMaxAmountSpinBox->objId()).toInt();
        _vlTargetVelocitySlider->setValue(_node->nodeOd()->value(_vlTargetVelocityObjectId).toInt());
        _vlTargetVelocitySlider->setRange(-max, max);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            connect(_nodeProfile402, &NodeProfile402::enableRampEvent, this, &P402VlWidget::vlEnableRampEvent);
            connect(_nodeProfile402, &NodeProfile402::unlockRampEvent, this, &P402VlWidget::vlUnlockRampEvent);
            connect(_nodeProfile402, &NodeProfile402::referenceRampEvent, this, &P402VlWidget::vlReferenceRamp);

            vlEnableRampEvent(_nodeProfile402->isEnableRamp());
            vlUnlockRampEvent(_nodeProfile402->isUnlockRamp());
            vlReferenceRamp(_nodeProfile402->isReferenceRamp());
        }

        connect(_vlMinVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlMinVelocityMinMaxAmountSpinboxFinished);
        connect(_vlMaxVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vllMaxVelocityMinMaxAmountSpinboxFinished);
    }
}

void P402VlWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::Mode::VL)
        {
            this->setEnabled(true);
            _node->readObject(_vlTargetVelocityObjectId);
            _node->readObject(_vlVelocityDemandObjectId);
            _node->readObject(_vlVelocityActualObjectId);
        }
    }
}

void P402VlWidget::vlTargetVelocitySpinboxFinished()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, QVariant(value));
    _vlTargetVelocitySlider->setValue(value);
}
void P402VlWidget::vlTargetVelocitySliderChanged()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, QVariant(value));
}

void P402VlWidget::vlMinVelocityMinMaxAmountSpinboxFinished()
{
    //    int min = _node->nodeOd()->value(_vlMinVelocityMinMaxAmountSpinBox->objId()).toInt();
    //    _vlTargetVelocitySlider->setMinimum(min);
}

void P402VlWidget::vllMaxVelocityMinMaxAmountSpinboxFinished()
{
    int max = _node->nodeOd()->value(_vlMaxVelocityMinMaxAmountSpinBox->objId()).toInt();
    _vlTargetVelocitySlider->setRange(-max, max);
    _vlSliderMinLabel->setNum(-max);
    _vlSliderMaxLabel->setNum(max);
}

void P402VlWidget::vlEnableRampClicked(bool ok)
{
    // 0 -> Velocity demand value shall be controlled in any other
    // 1 -> Velocity demand value shall accord with ramp output value

    if (_nodeProfile402)
    {
        _nodeProfile402->setEnableRamp(ok);
    }
}

void P402VlWidget::vlUnlockRampClicked(bool ok)
{
    // 0 -> Ramp output value shall be locked to current output value
    // 1 -> Ramp output value shall follow ramp input value

    if (_nodeProfile402)
    {
        _nodeProfile402->setUnlockRamp(ok);
    }
}

void P402VlWidget::vlReferenceRampClicked(bool ok)
{
    // 0 -> Ramp input value shall be set to zero
    // 1 -> Ramp input value shall accord with ramp reference
    if (_nodeProfile402)
    {
        _nodeProfile402->setReferenceRamp(ok);
    }
}

void P402VlWidget::vlEnableRampEvent(bool ok)
{
    _vlEnableRampCheckBox->setChecked(ok);
    updateVelocityDemandLabel();
}

void P402VlWidget::vlUnlockRampEvent(bool ok)
{
    _vlUnlockRampCheckBox->setChecked(ok);
    updateVelocityDemandLabel();
}

void P402VlWidget::vlReferenceRamp(bool ok)
{
    _vlReferenceRampCheckBox->setChecked(ok);
    updateVelocityDemandLabel();
}

void P402VlWidget::updateVelocityDemandLabel()
{
    QString text;
    if (!_vlEnableRampCheckBox->isChecked())
    {
        text = "Enable Ramp";
    }
    if (!_vlUnlockRampCheckBox->isChecked())
    {
        if (!text.isEmpty())
        {
            text.append(", ");
        }
        text += "Unlock Ramp";
    }
    if (!_vlReferenceRampCheckBox->isChecked())
    {
        if (!text.isEmpty())
        {
            text.append(", ");
        }
        text += "Not set ramp to zero";
    }
    if (!text.isEmpty())
    {
        text = "(" + text + "  : Not Activated)";
    }

    int value = _node->nodeOd()->value(_vlVelocityDemandObjectId).toInt();
    _vlVelocityDemandLabel->setText(QString("%1 ").arg(QString::number(value, 10)) + text);
}

void P402VlWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);

    dataLogger->addData(_vlVelocityActualObjectId);
    dataLogger->addData(_vlTargetVelocityObjectId);
    dataLogger->addData(_vlVelocityDemandObjectId);

    _dataLoggerWidget->show();
}

void P402VlWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    NodeObjectId statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);
    controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    QList<NodeObjectId> vlRpdoObjectList = {controlWordObjectId, _vlTargetVelocityObjectId};
    _node->rpdos().at(0)->writeMapping(vlRpdoObjectList);

    QList<NodeObjectId> vlTpdoObjectList = {statusWordObjectId, _vlVelocityDemandObjectId};
    _node->tpdos().at(2)->writeMapping(vlTpdoObjectList);
}

void P402VlWidget::refreshData(NodeObjectId object)
{
    int value;
    if (_node->nodeOd()->indexExist(object.index()))
    {
        if (object == _vlTargetVelocityObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            if (!_vlTargetVelocitySpinBox->hasFocus())
            {
                _vlTargetVelocitySpinBox->setValue(value);
            }
            if (!_vlTargetVelocitySlider->isSliderDown())
            {
                //_vlTargetVelocitySlider->setValue(value);
            }
        }
        if (object == _vlVelocityDemandObjectId)
        {
            updateVelocityDemandLabel();
        }
        if (object == _vlVelocityActualObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _vlVelocityActualLabel->setNum(value);
        }
    }
}

void P402VlWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QString name;
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box VL mode
    QGroupBox *vlGroupBox = new QGroupBox(tr("Velocity mode"));
    QFormLayout *vlLayout = new QFormLayout();

    _vlTargetVelocitySpinBox = new QSpinBox();
    _vlTargetVelocitySpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    name = tr("Target velocity ") + QString("(0x%1) :").arg(QString::number(_vlTargetVelocityObjectId.index(), 16).toUpper());
    vlLayout->addRow(name, _vlTargetVelocitySpinBox);

    _vlTargetVelocitySlider = new QSlider(Qt::Horizontal);
    _vlTargetVelocitySlider->setTickPosition(QSlider::TicksBothSides);
    vlLayout->addRow(_vlTargetVelocitySlider);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _vlSliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_vlSliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _vlSliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_vlSliderMaxLabel);
    vlLayout->addRow(labelSliderLayout);

    connect(_vlTargetVelocitySlider, &QSlider::valueChanged, _vlTargetVelocitySpinBox, &QSpinBox::setValue);
    connect(_vlTargetVelocitySlider, &QSlider::valueChanged, this, &P402VlWidget::vlTargetVelocitySliderChanged);
    connect(_vlTargetVelocitySpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlTargetVelocitySpinboxFinished);

    _vlVelocityDemandLabel = new QLabel("-");
    _vlVelocityDemandLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Velocity_demand ") + QString("(0x%1) :").arg(QString::number(_vlVelocityDemandObjectId.index(), 16).toUpper());
    vlLayout->addRow(name, _vlVelocityDemandLabel);

    _vlVelocityActualLabel = new QLabel("-");
    _vlVelocityActualLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Velocity_actual_value ") + QString("(0x%1) :").arg(QString::number(_vlVelocityActualObjectId.index(), 16));
    vlLayout->addRow(name, _vlVelocityActualLabel);

    name = tr("Velocity min max amount ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_VL_MAX, _axis).index(), 16).toUpper());
    QLabel *vlVelocityMinMaxAmountLabel = new QLabel(name);
    vlLayout->addRow(vlVelocityMinMaxAmountLabel);
    QLayout *vlVelocityMinMaxAmountlayout = new QHBoxLayout();

    _vlMinVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _vlMinVelocityMinMaxAmountSpinBox->setToolTip("min ");
    vlVelocityMinMaxAmountlayout->addWidget(_vlMinVelocityMinMaxAmountSpinBox);
    _vlMaxVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("max ");
    vlVelocityMinMaxAmountlayout->addWidget(_vlMaxVelocityMinMaxAmountSpinBox);
    vlLayout->addRow(vlVelocityMinMaxAmountlayout);

    name = tr("Velocity acceleration ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_TIME, _axis).index(), 16).toUpper());
    QLabel *vlVelocityAccelerationLabel = new QLabel(name);
    vlLayout->addRow(vlVelocityAccelerationLabel);
    QLayout *vlVelocityAccelerationlayout = new QHBoxLayout();
    _vlAccelerationDeltaSpeedSpinBox = new IndexSpinBox();
    _vlAccelerationDeltaSpeedSpinBox->setToolTip("Delta Speed (inc/ms)");
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaSpeedSpinBox);
    _vlAccelerationDeltaTimeSpinBox = new IndexSpinBox();
    _vlAccelerationDeltaTimeSpinBox->setToolTip("Delta Time (ms)");
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityAccelerationlayout);

    name = tr("Velocity deceleration ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_TIME, _axis).index(), 16).toUpper());
    QLabel *vlVelocityDecelerationLabel = new QLabel(name);
    vlLayout->addRow(vlVelocityDecelerationLabel);
    QLayout *vlVelocityDecelerationlayout = new QHBoxLayout();
    _vlDecelerationDeltaSpeedSpinBox = new IndexSpinBox();
    _vlDecelerationDeltaSpeedSpinBox->setToolTip("Delta Speed (inc/ms)");
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaSpeedSpinBox);
    _vlDecelerationDeltaTimeSpinBox = new IndexSpinBox();
    _vlDecelerationDeltaTimeSpinBox->setToolTip("Delta Time (ms)");
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityDecelerationlayout);

    name = tr("Velocity quick stop ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_TIME, _axis).index(), 16).toUpper());
    QLabel *vlVelocityQuickStopLabel = new QLabel(name);
    vlLayout->addRow(vlVelocityQuickStopLabel);
    QLayout *vlVelocityQuickStoplayout = new QHBoxLayout();
    _vlQuickStopDeltaSpeedSpinBox = new IndexSpinBox();
    _vlQuickStopDeltaSpeedSpinBox->setToolTip("Delta Speed (inc/ms)");
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaSpeedSpinBox);
    _vlQuickStopDeltaTimeSpinBox = new IndexSpinBox();
    _vlQuickStopDeltaTimeSpinBox->setToolTip("Delta Time (ms)");
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityQuickStoplayout);

    name = tr("Set-point factor ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_NUMERATOR, _axis).index(), 16).toUpper());
    QLabel *vlSetPointFactorLabel = new QLabel(name);
    vlLayout->addRow(vlSetPointFactorLabel);
    QLayout *vlSetPointFactorlayout = new QHBoxLayout();
    _vlSetPointFactorNumeratorSpinBox = new IndexSpinBox();
    _vlSetPointFactorNumeratorSpinBox->setToolTip("Numerator");
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorNumeratorSpinBox);
    _vlSetPointFactorDenominatorSpinBox = new IndexSpinBox();
    _vlSetPointFactorDenominatorSpinBox->setToolTip("Denominator");
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorDenominatorSpinBox);
    vlLayout->addRow(vlSetPointFactorlayout);

    name = tr("Dimension factor ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_NUMERATOR, _axis).index(), 16).toUpper());
    QLabel *vlDimensionFactorLabel = new QLabel(name);
    vlLayout->addRow(vlDimensionFactorLabel);
    QLayout *vlDimensionFactorlayout = new QHBoxLayout();
    _vlDimensionFactorNumeratorSpinBox = new IndexSpinBox();
    _vlDimensionFactorNumeratorSpinBox->setToolTip("Numerator");
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorNumeratorSpinBox);
    _vlDimensionFactorDenominatorSpinBox = new IndexSpinBox();
    _vlDimensionFactorDenominatorSpinBox->setToolTip("Denominator");
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorDenominatorSpinBox);
    vlLayout->addRow(vlDimensionFactorlayout);

    vlGroupBox->setLayout(vlLayout);

    // Group Box Control Word
    name = tr("Control Word ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, _axis).index(), 16).toUpper());
    QGroupBox *modeControlWordGroupBox = new QGroupBox(name);
    QFormLayout *modeControlWordLayout = new QFormLayout();

    _vlEnableRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Enable Ramp (bit 4) :"), _vlEnableRampCheckBox);
    connect(_vlEnableRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::vlEnableRampClicked);

    _vlUnlockRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Unlock Ramp (bit 5) :"), _vlUnlockRampCheckBox);
    connect(_vlUnlockRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::vlUnlockRampClicked);

    _vlReferenceRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Not set ramp to zero (bit 6) :"), _vlReferenceRampCheckBox);
    connect(_vlReferenceRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::vlReferenceRampClicked);

    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402VlWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402VlWidget::pdoMapping);

    QPixmap vlModePixmap;
    QLabel *vlModeLabel;
    vlModeLabel = new QLabel();
    vlModePixmap.load(":/diagram/img/diagrams/402VLDiagram.png");
    vlModeLabel->setPixmap(vlModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram VL mode"));
    connect(imgPushButton, SIGNAL(clicked()), vlModeLabel, SLOT(show()));
    QHBoxLayout *vlButtonLayout = new QHBoxLayout();
    vlButtonLayout->setSpacing(5);
    vlButtonLayout->addWidget(dataLoggerPushButton);
    vlButtonLayout->addWidget(mappingPdoPushButton);
    vlButtonLayout->addWidget(imgPushButton);

    layout->addWidget(vlGroupBox);
    layout->addWidget(modeControlWordGroupBox);
    layout->addItem(vlButtonLayout);

    widget->setLayout(layout);
    setWidget(widget);
}

void P402VlWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if ((objId == _vlTargetVelocityObjectId) || (objId == _vlVelocityDemandObjectId) || (objId == _vlVelocityActualObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId);
    }
}
