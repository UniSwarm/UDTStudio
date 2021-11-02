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
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"
#include "services/services.h"

#include "profile/p402/modevl.h"
#include "profile/p402/nodeprofile402.h"

#include <QPushButton>

P402VlWidget::P402VlWidget(QWidget *parent) : P402ModeWidget(parent)
{
    createWidgets();
    _nodeProfile402 = nullptr;
}

P402VlWidget::~P402VlWidget()
{
    unRegisterFullOd();
}

void P402VlWidget::readRealTimeObjects()
{
    if (_nodeProfile402)
    {
        _nodeProfile402->readRealTimeObjects();
    }
}

void P402VlWidget::readAllObjects()
{
    if (_nodeProfile402)
    {
        _nodeProfile402->readAllObjects();
    }
}

void P402VlWidget::reset()
{
    _modeVl->reset();
}

void P402VlWidget::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }

    if (node)
    {
        setNodeInterrest(node);

        if (!node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(node->profiles()[axis]);
            _modeVl = dynamic_cast<ModeVl *>(_nodeProfile402->mode(NodeProfile402::OperationMode::VL));

            connect(_modeVl, &ModeVl::enableRampEvent, this, &P402VlWidget::enableRampEvent);
            connect(_modeVl, &ModeVl::unlockRampEvent, this, &P402VlWidget::unlockRampEvent);
            connect(_modeVl, &ModeVl::referenceRampEvent, this, &P402VlWidget::referenceRamp);

            enableRampEvent(_modeVl->isEnableRamp());
            unlockRampEvent(_modeVl->isUnlockRamp());
            referenceRamp(_modeVl->isReferenceRamp());

            _velocityTargetObjectId = _modeVl->targetObjectId();
            _velocityTargetObjectId.setDataType(QMetaType::Type::Short);
            registerObjId(_velocityTargetObjectId);

            _velocityDemandObjectId = _modeVl->velocityDemandObjectId();
            _velocityDemandObjectId.setDataType(QMetaType::Type::Short);
            _velocityDemandLabel->setObjId(_velocityDemandObjectId);

            _velocityActualObjectId = _modeVl->velocityActualObjectId();
            _velocityActualObjectId.setDataType(QMetaType::Type::Short);
            _velocityActualLabel->setObjId(_velocityActualObjectId);

            _minVelocityMinMaxAmountSpinBox->setObjId(_modeVl->minVelocityMinMaxAmountObjectId());

            _maxVelocityMinMaxAmountObjectId = _modeVl->maxVelocityMinMaxAmountObjectId();
            _maxVelocityMinMaxAmountSpinBox->setObjId(_maxVelocityMinMaxAmountObjectId);
            registerObjId(_maxVelocityMinMaxAmountObjectId);

            _accelerationDeltaSpeedSpinBox->setObjId(_modeVl->accelerationDeltaSpeedObjectId());
            _accelerationDeltaTimeSpinBox->setObjId(_modeVl->accelerationDeltaTimeObjectId());
            _decelerationDeltaSpeedSpinBox->setObjId(_modeVl->decelerationDeltaSpeedObjectId());
            _decelerationDeltaTimeSpinBox->setObjId(_modeVl->decelerationDeltaTimeObjectId());
            _quickStopDeltaSpeedSpinBox->setObjId(_modeVl->quickStopDeltaSpeedObjectId());
            _quickStopDeltaTimeSpinBox->setObjId(_modeVl->quickStopDeltaTimeObjectId());
            _setPointFactorNumeratorSpinBox->setObjId(_modeVl->setPointFactorNumeratorObjectId());
            _setPointFactorDenominatorSpinBox->setObjId(_modeVl->setPointFactorDenominatorObjectId());
            _dimensionFactorNumeratorSpinBox->setObjId(_modeVl->dimensionFactorNumeratorObjectId());
            _dimensionFactorDenominatorSpinBox->setObjId(_modeVl->dimensionFactorDenominatorObjectId());

            int max = _nodeProfile402->node()->nodeOd()->value(_maxVelocityMinMaxAmountSpinBox->objId()).toInt();
            _targetVelocitySlider->setValue(_nodeProfile402->node()->nodeOd()->value(_velocityTargetObjectId).toInt());
            _targetVelocitySlider->setRange(-max, max);
        }
    }
}

void P402VlWidget::targetVelocitySpinboxFinished()
{
    qint16 value = static_cast<qint16>(_targetVelocitySpinBox->value());
    _modeVl->setTarget(value);
}

void P402VlWidget::targetVelocitySliderChanged()
{
    qint16 value = static_cast<qint16>(_targetVelocitySlider->value());
    _modeVl->setTarget(value);
}

void P402VlWidget::updateMaxVelocityMinMaxAmount()
{
    int max = _nodeProfile402->node()->nodeOd()->value(_maxVelocityMinMaxAmountSpinBox->objId()).toInt();
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

void P402VlWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);

    dataLogger->addData(_velocityActualObjectId);
    dataLogger->addData(_velocityTargetObjectId);
    dataLogger->addData(_velocityDemandObjectId);

    _dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(_dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    _dataLoggerWidget->show();
    _dataLoggerWidget->raise();
    _dataLoggerWidget->activateWindow();
}

void P402VlWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> vlRpdoObjectList = {controlWordObjectId, _velocityTargetObjectId};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(vlRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> vlTpdoObjectList = {statusWordObjectId, _velocityDemandObjectId};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(vlTpdoObjectList);
}

void P402VlWidget::createWidgets()
{
    // Group Box Velocity mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Velocity mode"));
    _modeLayout = new QFormLayout();

    targetWidgets();
    informationWidgets();
    limitWidgets();

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    accelDeccelWidgets();
    factorWidgets();

    modeGroupBox->setLayout(_modeLayout);

    // Create interface
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(modeGroupBox);
    layout->addWidget(controlWordWidgets());

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(scrollArea);
    vBoxLayout->addLayout(buttonWidgets());
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vBoxLayout);
}

void P402VlWidget::targetWidgets()
{
    _targetVelocitySpinBox = new QSpinBox();
    _targetVelocitySpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _modeLayout->addRow(tr("Target velocity:"), _targetVelocitySpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    _modeLayout->addRow(labelSliderLayout);

    _targetVelocitySlider = new QSlider(Qt::Horizontal);
    _targetVelocitySlider->setTickPosition(QSlider::TicksBothSides);
    _modeLayout->addRow(_targetVelocitySlider);

    connect(_targetVelocitySlider, &QSlider::valueChanged, this, &P402VlWidget::targetVelocitySliderChanged);
    connect(_targetVelocitySpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::targetVelocitySpinboxFinished);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText("Set to 0");
    connect(setZeroButton, &QPushButton::clicked, this, &P402VlWidget::setZeroButton);
    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _modeLayout->addRow(setZeroLayout);
}

void P402VlWidget::informationWidgets()
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    _modeLayout->addRow(tr("Information:"), _infoLabel);

    _velocityDemandLabel = new IndexLabel();
    _modeLayout->addRow(tr("Velocity demand "), _velocityDemandLabel);

    _velocityActualLabel = new IndexLabel();
    _modeLayout->addRow(tr("Velocity actual value "), _velocityActualLabel);
}

void P402VlWidget::limitWidgets()
{
    QLabel *label;
    QLayout *minMaxAmountlayout = new QHBoxLayout();
    minMaxAmountlayout->setSpacing(0);

    _minVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    minMaxAmountlayout->addWidget(_minVelocityMinMaxAmountSpinBox);

    label = new QLabel(tr("-"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    minMaxAmountlayout->addWidget(label);

    _maxVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    minMaxAmountlayout->addWidget(_maxVelocityMinMaxAmountSpinBox);
    label = new QLabel(tr("Min/Max amo&unt:"));
    label->setToolTip("Min, Max");
    label->setBuddy(_minVelocityMinMaxAmountSpinBox);

    _modeLayout->addRow(label, minMaxAmountlayout);
}

void P402VlWidget::accelDeccelWidgets()
{
    QLabel *label;

    // ACCELERATION
    QHBoxLayout *accelerationlayout = new QHBoxLayout();
    accelerationlayout->setSpacing(0);

    _accelerationDeltaSpeedSpinBox = new IndexSpinBox();
    accelerationlayout->addWidget(_accelerationDeltaSpeedSpinBox);
    label = new QLabel(tr("/"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    accelerationlayout->addWidget(label);

    _accelerationDeltaTimeSpinBox = new IndexSpinBox();
    accelerationlayout->addWidget(_accelerationDeltaTimeSpinBox);
    label = new QLabel(tr("&Acceleration:"));
    label->setToolTip(QString(QChar(0x0394)) + "speed, " + QString(QChar(0x0394)) + "time");
    label->setBuddy(_accelerationDeltaSpeedSpinBox);
    _modeLayout->addRow(label, accelerationlayout);

    // DECELERATION
    QHBoxLayout *decelerationlayout = new QHBoxLayout();
    decelerationlayout->setSpacing(0);

    _decelerationDeltaSpeedSpinBox = new IndexSpinBox();
    decelerationlayout->addWidget(_decelerationDeltaSpeedSpinBox);
    label = new QLabel(tr("/"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    decelerationlayout->addWidget(label);

    _decelerationDeltaTimeSpinBox = new IndexSpinBox();
    decelerationlayout->addWidget(_decelerationDeltaTimeSpinBox);
    label = new QLabel(tr("&Deceleration:"));
    label->setToolTip(QString(QChar(0x0394)) + "speed, " + QString(QChar(0x0394)) + "time");
    label->setBuddy(_decelerationDeltaSpeedSpinBox);
    _modeLayout->addRow(label, decelerationlayout);

    // DECELERATION QUICKSTOP
    QHBoxLayout *quickStoplayout = new QHBoxLayout();
    quickStoplayout->setSpacing(0);

    _quickStopDeltaSpeedSpinBox = new IndexSpinBox();
    quickStoplayout->addWidget(_quickStopDeltaSpeedSpinBox);
    label = new QLabel(tr("/"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    quickStoplayout->addWidget(label);

    _quickStopDeltaTimeSpinBox = new IndexSpinBox();
    quickStoplayout->addWidget(_quickStopDeltaTimeSpinBox);
    label = new QLabel(tr("&Quick stop deceleration:"));
    label->setToolTip(QString(QChar(0x0394)) + "speed, " + QString(QChar(0x0394)) + "time");
    label->setBuddy(_quickStopDeltaSpeedSpinBox);
    _modeLayout->addRow(label, quickStoplayout);
}

void P402VlWidget::factorWidgets()
{
    QLabel *label;

    // SET-POINT FACTOR
    QLayout *stPointFactorlayout = new QHBoxLayout();
    stPointFactorlayout->setSpacing(0);

    _setPointFactorNumeratorSpinBox = new IndexSpinBox();
    stPointFactorlayout->addWidget(_setPointFactorNumeratorSpinBox);
    label = new QLabel(tr("/"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    stPointFactorlayout->addWidget(label);

    _setPointFactorDenominatorSpinBox = new IndexSpinBox();
    stPointFactorlayout->addWidget(_setPointFactorDenominatorSpinBox);
    label = new QLabel(tr("&Set-point factor:"));
    label->setToolTip("&Numerator, Denominator");
    label->setBuddy(_setPointFactorNumeratorSpinBox);
    _modeLayout->addRow(label, stPointFactorlayout);

    // DIMENSION FACTOR
    QLayout *dimensionFactorlayout = new QHBoxLayout();
    dimensionFactorlayout->setSpacing(0);

    _dimensionFactorNumeratorSpinBox = new IndexSpinBox();
    dimensionFactorlayout->addWidget(_dimensionFactorNumeratorSpinBox);
    label = new QLabel(tr("/"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dimensionFactorlayout->addWidget(label);

    _dimensionFactorDenominatorSpinBox = new IndexSpinBox();
    dimensionFactorlayout->addWidget(_dimensionFactorDenominatorSpinBox);
    label = new QLabel(tr("D&imension factor:"));
    label->setToolTip("Numerator, Denominator");
    label->setBuddy(_dimensionFactorNumeratorSpinBox);
    _modeLayout->addRow(label, dimensionFactorlayout);
}

QGroupBox *P402VlWidget::controlWordWidgets()
{
    // Group Box CONTROL WORD
    QGroupBox *groupBox = new QGroupBox(tr("Control Word:"));
    QFormLayout *layout = new QFormLayout();

    _enableRampCheckBox = new QCheckBox();
    layout->addRow(tr("Enable Ramp (bit 4):"), _enableRampCheckBox);
    connect(_enableRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::enableRampClicked);

    _unlockRampCheckBox = new QCheckBox();
    layout->addRow(tr("Unlock Ramp (bit 5):"), _unlockRampCheckBox);
    connect(_unlockRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::unlockRampClicked);

    _referenceRampCheckBox = new QCheckBox();
    layout->addRow(tr("Not set ramp to zero (bit 6):"), _referenceRampCheckBox);
    connect(_referenceRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::referenceRampClicked);
    groupBox->setLayout(layout);

    return groupBox;
}

QHBoxLayout *P402VlWidget::buttonWidgets()
{
    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402VlWidget::createDataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Map VL to PDOs"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402VlWidget::mapDefaultObjects);

    QPixmap modePixmap;
    QLabel *vlModeLabel;
    vlModeLabel = new QLabel();
    modePixmap.load(":/diagram/img/diagrams/402VLDiagram.png");
    vlModeLabel->setPixmap(modePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram VL mode"));
    connect(imgPushButton, SIGNAL(clicked()), vlModeLabel, SLOT(show()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(5);
    layout->addWidget(dataLoggerPushButton);
    layout->addWidget(mappingPdoPushButton);
    layout->addWidget(imgPushButton);

    return layout;
}

void P402VlWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if (objId == _velocityTargetObjectId)
    {
        int value = _nodeProfile402->node()->nodeOd()->value(objId).toInt();
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
    else if (objId == _maxVelocityMinMaxAmountObjectId)
    {
        updateMaxVelocityMinMaxAmount();
    }
}
