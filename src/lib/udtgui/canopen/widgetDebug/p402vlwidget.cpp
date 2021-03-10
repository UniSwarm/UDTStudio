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
#include "canopen/widget/indexlabel.h"
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
        if (_nodeProfile402->actualMode() == NodeProfile402::OperationMode::VL)
        {
            _velocityDemandLabel->readObject();
            _velocityActualLabel->readObject();
        }
    }
}

void P402VlWidget::readAllObject()
{
    if (_node)
    {
        _velocityDemandLabel->readObject();
        _velocityActualLabel->readObject();
        _minVelocityMinMaxAmountSpinBox->readObject();
        _maxVelocityMinMaxAmountSpinBox->readObject();
        _accelerationDeltaSpeedSpinBox->readObject();
        _accelerationDeltaTimeSpinBox->readObject();
        _decelerationDeltaSpeedSpinBox->readObject();
        _decelerationDeltaTimeSpinBox->readObject();
        _quickStopDeltaSpeedSpinBox->readObject();
        _quickStopDeltaTimeSpinBox->readObject();
        _setPointFactorNumeratorSpinBox->readObject();
        _setPointFactorDenominatorSpinBox->readObject();
        _dimensionFactorNumeratorSpinBox->readObject();
        _dimensionFactorDenominatorSpinBox->readObject();
    }
}

void P402VlWidget::reset()
{
    _node->readObject(_velocityTargetObjectId);
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
        _velocityDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND, _axis);
        _velocityActualObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE, _axis);
        _velocityTargetObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET, _axis);

        createWidgets();

        _velocityTargetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        _velocityDemandObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        _velocityActualObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        _velocityTargetObjectId.setDataType(QMetaType::Type::Short);
        _velocityDemandObjectId.setDataType(QMetaType::Type::Short);
        _velocityActualObjectId.setDataType(QMetaType::Type::Short);

        registerObjId(_velocityTargetObjectId);
        setNodeInterrest(node);

        _velocityDemandLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND, _axis));
        _velocityActualLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE, _axis));

        _minVelocityMinMaxAmountSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_MIN, axis));
        _maxVelocityMinMaxAmountSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_MAX, axis));
        _accelerationDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_SPEED, axis));
        _accelerationDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_TIME, axis));
        _decelerationDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_SPEED, axis));
        _decelerationDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_TIME, axis));
        _quickStopDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_SPEED, axis));
        _quickStopDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_TIME, axis));
        _setPointFactorNumeratorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_NUMERATOR, axis));
        _setPointFactorDenominatorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_DENOMINATOR, axis));
        _dimensionFactorNumeratorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_NUMERATOR, axis));
        _dimensionFactorDenominatorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_DENOMINATOR, axis));

        _velocityDemandLabel->setNode(node);
        _velocityActualLabel->setNode(node);

        _minVelocityMinMaxAmountSpinBox->setNode(node);
        _maxVelocityMinMaxAmountSpinBox->setNode(node);
        _accelerationDeltaSpeedSpinBox->setNode(node);
        _accelerationDeltaTimeSpinBox->setNode(node);
        _decelerationDeltaSpeedSpinBox->setNode(node);
        _decelerationDeltaTimeSpinBox->setNode(node);
        _quickStopDeltaSpeedSpinBox->setNode(node);
        _quickStopDeltaTimeSpinBox->setNode(node);
        _setPointFactorNumeratorSpinBox->setNode(node);
        _setPointFactorDenominatorSpinBox->setNode(node);
        _dimensionFactorNumeratorSpinBox->setNode(node);
        _dimensionFactorDenominatorSpinBox->setNode(node);

        int max = _node->nodeOd()->value(_maxVelocityMinMaxAmountSpinBox->objId()).toInt();
        _targetVelocitySlider->setValue(_node->nodeOd()->value(_velocityTargetObjectId).toInt());
        _targetVelocitySlider->setRange(-max, max);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            _modeVl = dynamic_cast<ModeVl *>(_nodeProfile402->mode(NodeProfile402::OperationMode::VL));

            connect(_modeVl, &ModeVl::enableRampEvent, this, &P402VlWidget::enableRampEvent);
            connect(_modeVl, &ModeVl::unlockRampEvent, this, &P402VlWidget::unlockRampEvent);
            connect(_modeVl, &ModeVl::referenceRampEvent, this, &P402VlWidget::referenceRamp);

            enableRampEvent(_modeVl->isEnableRamp());
            unlockRampEvent(_modeVl->isUnlockRamp());
            referenceRamp(_modeVl->isReferenceRamp());
        }

        connect(_maxVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::maxVelocityMinMaxAmountSpinboxFinished);
    }
}

void P402VlWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::OperationMode::VL)
        {
            this->setEnabled(true);
            _node->readObject(_velocityTargetObjectId);
            _velocityDemandLabel->readObject();
            _velocityActualLabel->readObject();
        }
    }
}

void P402VlWidget::targetVelocitySpinboxFinished()
{
    qint16 value = static_cast<qint16>(_targetVelocitySpinBox->value());
    _node->writeObject(_velocityTargetObjectId, QVariant(value));
}

void P402VlWidget::targetVelocitySliderChanged()
{
    qint16 value = static_cast<qint16>(_targetVelocitySlider->value());
    _node->writeObject(_velocityTargetObjectId, QVariant(value));
}

void P402VlWidget::maxVelocityMinMaxAmountSpinboxFinished()
{
    int max = _node->nodeOd()->value(_maxVelocityMinMaxAmountSpinBox->objId()).toInt();
    _targetVelocitySlider->setRange(-max, max);
    _sliderMinLabel->setNum(-max);
    _sliderMaxLabel->setNum(max);
}

void P402VlWidget::setZeroButton()
{
    _nodeProfile402->setTarget(0);
}

void P402VlWidget::enableRampClicked(bool ok)
{
    // 0 -> Velocity demand value shall be controlled in any other
    // 1 -> Velocity demand value shall accord with ramp output value

    if (_nodeProfile402)
    {
        _modeVl->setEnableRamp(ok);
    }
}

void P402VlWidget::unlockRampClicked(bool ok)
{
    // 0 -> Ramp output value shall be locked to current output value
    // 1 -> Ramp output value shall follow ramp input value

    if (_nodeProfile402)
    {
        _modeVl->setUnlockRamp(ok);
    }
}

void P402VlWidget::referenceRampClicked(bool ok)
{
    // 0 -> Ramp input value shall be set to zero
    // 1 -> Ramp input value shall accord with ramp reference
    if (_nodeProfile402)
    {
        _modeVl->setReferenceRamp(ok);
    }
}

void P402VlWidget::enableRampEvent(bool ok)
{
    _enableRampCheckBox->setChecked(ok);
    updateInformationLabel();
}

void P402VlWidget::unlockRampEvent(bool ok)
{
    _unlockRampCheckBox->setChecked(ok);
    updateInformationLabel();
}

void P402VlWidget::referenceRamp(bool ok)
{
    _referenceRampCheckBox->setChecked(ok);
    updateInformationLabel();
}

void P402VlWidget::updateInformationLabel()
{
    QString text;
    if (!_enableRampCheckBox->isChecked())
    {
        text = "Ramp is disabled";
    }
    if (!_unlockRampCheckBox->isChecked())
    {
        if (!text.isEmpty())
        {
            text.append(" & ");
        }
        text += "Ramp is locked";
    }
    if (!_referenceRampCheckBox->isChecked())
    {
        if (!text.isEmpty())
        {
            text.append(" & ");
        }
        text += "Ramp set to 0";
    }
    _infoLabel->setText(text);
}

void P402VlWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);

    dataLogger->addData(_velocityActualObjectId);
    dataLogger->addData(_velocityTargetObjectId);
    dataLogger->addData(_velocityDemandObjectId);

    _dataLoggerWidget->show();
}

void P402VlWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    NodeObjectId statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);
    controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    QList<NodeObjectId> vlRpdoObjectList = {controlWordObjectId, _velocityTargetObjectId};
    _node->rpdos().at(0)->writeMapping(vlRpdoObjectList);

    QList<NodeObjectId> vlTpdoObjectList = {statusWordObjectId, _velocityDemandObjectId};
    _node->tpdos().at(2)->writeMapping(vlTpdoObjectList);
}

void P402VlWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box VL mode
    QGroupBox *vlGroupBox = new QGroupBox(tr("Velocity mode"));
    QFormLayout *vlLayout = new QFormLayout();

    _targetVelocitySpinBox = new QSpinBox();
    _targetVelocitySpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow(tr("Target velocity :"), _targetVelocitySpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    vlLayout->addRow(labelSliderLayout);

    _targetVelocitySlider = new QSlider(Qt::Horizontal);
    _targetVelocitySlider->setTickPosition(QSlider::TicksBothSides);
    vlLayout->addRow(_targetVelocitySlider);

    connect(_targetVelocitySlider, &QSlider::valueChanged, this, &P402VlWidget::targetVelocitySliderChanged);
    connect(_targetVelocitySpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::targetVelocitySpinboxFinished);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText("Set to 0");
    connect(setZeroButton, &QPushButton::clicked, this, &P402VlWidget::setZeroButton);
    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    vlLayout->addRow(setZeroLayout);

    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    vlLayout->addRow(tr("Information :"), _infoLabel);

    _velocityDemandLabel = new IndexLabel();
    vlLayout->addRow(tr("Velocity demand "), _velocityDemandLabel);

    _velocityActualLabel = new IndexLabel();
    vlLayout->addRow(tr("Velocity actual value "), _velocityActualLabel);

    QLabel *vlVelocityMinMaxAmountLabel = new QLabel(tr("Velocity min max amount :"));
    vlLayout->addRow(vlVelocityMinMaxAmountLabel);
    QLayout *vlVelocityMinMaxAmountlayout = new QHBoxLayout();

    _minVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _minVelocityMinMaxAmountSpinBox->setToolTip("min ");
    vlVelocityMinMaxAmountlayout->addWidget(_minVelocityMinMaxAmountSpinBox);
    _maxVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _maxVelocityMinMaxAmountSpinBox->setToolTip("max ");
    vlVelocityMinMaxAmountlayout->addWidget(_maxVelocityMinMaxAmountSpinBox);
    vlLayout->addRow(vlVelocityMinMaxAmountlayout);

    QLabel *vlVelocityAccelerationLabel = new QLabel(tr("Velocity acceleration :"));
    vlLayout->addRow(vlVelocityAccelerationLabel);
    QLayout *vlVelocityAccelerationlayout = new QHBoxLayout();
    _accelerationDeltaSpeedSpinBox = new IndexSpinBox();
    _accelerationDeltaSpeedSpinBox->setToolTip("Delta Speed (inc/ms)");
    vlVelocityAccelerationlayout->addWidget(_accelerationDeltaSpeedSpinBox);
    _accelerationDeltaTimeSpinBox = new IndexSpinBox();
    _accelerationDeltaTimeSpinBox->setToolTip("Delta Time (ms)");
    vlVelocityAccelerationlayout->addWidget(_accelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityAccelerationlayout);

    QLabel *vlVelocityDecelerationLabel = new QLabel(tr("Velocity deceleration :"));
    vlLayout->addRow(vlVelocityDecelerationLabel);
    QLayout *vlVelocityDecelerationlayout = new QHBoxLayout();
    _decelerationDeltaSpeedSpinBox = new IndexSpinBox();
    _decelerationDeltaSpeedSpinBox->setToolTip("Delta Speed (inc/ms)");
    vlVelocityDecelerationlayout->addWidget(_decelerationDeltaSpeedSpinBox);
    _decelerationDeltaTimeSpinBox = new IndexSpinBox();
    _decelerationDeltaTimeSpinBox->setToolTip("Delta Time (ms)");
    vlVelocityDecelerationlayout->addWidget(_decelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityDecelerationlayout);

    QLabel *vlVelocityQuickStopLabel = new QLabel(tr("Velocity quick stop :"));
    vlLayout->addRow(vlVelocityQuickStopLabel);
    QLayout *vlVelocityQuickStoplayout = new QHBoxLayout();
    _quickStopDeltaSpeedSpinBox = new IndexSpinBox();
    _quickStopDeltaSpeedSpinBox->setToolTip("Delta Speed (inc/ms)");
    vlVelocityQuickStoplayout->addWidget(_quickStopDeltaSpeedSpinBox);
    _quickStopDeltaTimeSpinBox = new IndexSpinBox();
    _quickStopDeltaTimeSpinBox->setToolTip("Delta Time (ms)");
    vlVelocityQuickStoplayout->addWidget(_quickStopDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityQuickStoplayout);

    QLabel *vlSetPointFactorLabel = new QLabel(tr("Set-point factor :"));
    vlLayout->addRow(vlSetPointFactorLabel);
    QLayout *vlSetPointFactorlayout = new QHBoxLayout();
    _setPointFactorNumeratorSpinBox = new IndexSpinBox();
    _setPointFactorNumeratorSpinBox->setToolTip("Numerator");
    vlSetPointFactorlayout->addWidget(_setPointFactorNumeratorSpinBox);
    _setPointFactorDenominatorSpinBox = new IndexSpinBox();
    _setPointFactorDenominatorSpinBox->setToolTip("Denominator");
    vlSetPointFactorlayout->addWidget(_setPointFactorDenominatorSpinBox);
    vlLayout->addRow(vlSetPointFactorlayout);

    QLabel *vlDimensionFactorLabel = new QLabel(tr("Dimension factor :"));
    vlLayout->addRow(vlDimensionFactorLabel);
    QLayout *vlDimensionFactorlayout = new QHBoxLayout();
    _dimensionFactorNumeratorSpinBox = new IndexSpinBox();
    _dimensionFactorNumeratorSpinBox->setToolTip("Numerator");
    vlDimensionFactorlayout->addWidget(_dimensionFactorNumeratorSpinBox);
    _dimensionFactorDenominatorSpinBox = new IndexSpinBox();
    _dimensionFactorDenominatorSpinBox->setToolTip("Denominator");
    vlDimensionFactorlayout->addWidget(_dimensionFactorDenominatorSpinBox);
    vlLayout->addRow(vlDimensionFactorlayout);

    vlGroupBox->setLayout(vlLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word :"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    _enableRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Enable Ramp (bit 4) :"), _enableRampCheckBox);
    connect(_enableRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::enableRampClicked);

    _unlockRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Unlock Ramp (bit 5) :"), _unlockRampCheckBox);
    connect(_unlockRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::unlockRampClicked);

    _referenceRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Not set ramp to zero (bit 6) :"), _referenceRampCheckBox);
    connect(_referenceRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::referenceRampClicked);

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
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if (objId == _velocityTargetObjectId)
    {
        int value = _node->nodeOd()->value(objId).toInt();
        if (!_targetVelocitySpinBox->hasFocus())
        {
            _targetVelocitySpinBox->blockSignals(true);
            _targetVelocitySpinBox->setValue(value);
            _targetVelocitySpinBox->blockSignals(false);
        }
        if (!_targetVelocitySlider->isSliderDown())
        {
            _targetVelocitySlider->blockSignals(true);
            _targetVelocitySlider->setValue(value);
            _targetVelocitySlider->blockSignals(false);
        }
    }
}
