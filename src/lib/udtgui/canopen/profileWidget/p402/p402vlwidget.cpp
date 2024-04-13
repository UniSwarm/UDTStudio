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
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexslider.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "profile/p402/modevl.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

#include <QPushButton>

P402VlWidget::P402VlWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _modeVl = nullptr;

    createWidgets();
    createActions();
}

void P402VlWidget::reset()
{
    _modeVl->reset();
}

void P402VlWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        _modeVl = nullptr;
        return;
    }
    _modeVl = dynamic_cast<ModeVl *>(_nodeProfile402->mode(NodeProfile402::OperationMode::VL));

    connect(_modeVl, &ModeVl::enableRampEvent, this, &P402VlWidget::enableRampEvent);
    connect(_modeVl, &ModeVl::unlockRampEvent, this, &P402VlWidget::unlockRampEvent);
    connect(_modeVl, &ModeVl::referenceRampEvent, this, &P402VlWidget::referenceRamp);

    enableRampEvent(_modeVl->isEnableRamp());
    unlockRampEvent(_modeVl->isUnlockRamp());
    referenceRamp(_modeVl->isReferenceRamp());

    _targetVelocitySlider->setObjId(_modeVl->targetObjectId());
    _targetVelocitySpinBox->setObjId(_modeVl->targetObjectId());

    _velocityDemandLabel->setObjId(_modeVl->velocityDemandObjectId());
    registerObjId(_modeVl->velocityDemandObjectId());

    _velocityActualLabel->setObjId(_modeVl->velocityActualObjectId());

    _minVelocityMinMaxAmountSpinBox->setObjId(_modeVl->minVelocityMinMaxAmountObjectId());
    _maxVelocityMinMaxAmountSpinBox->setObjId(_modeVl->maxVelocityMinMaxAmountObjectId());
    registerObjId(_modeVl->minVelocityMinMaxAmountObjectId());
    registerObjId(_modeVl->maxVelocityMinMaxAmountObjectId());

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

    updateMinMaxVelocity();
}

void P402VlWidget::updateMinMaxVelocity()
{
    int min = _nodeProfile402->node()->nodeOd()->value(_modeVl->minVelocityMinMaxAmountObjectId()).toInt();
    int max = _nodeProfile402->node()->nodeOd()->value(_modeVl->maxVelocityMinMaxAmountObjectId()).toInt();
    _targetVelocitySlider->setRange(-max, max);
    _sliderMinLabel->setText(QStringLiteral("[-%1").arg(max));
    _sliderMaxLabel->setText(QStringLiteral("%1]").arg(max));
    if (min == 0)
    {
        _sliderCenterLabel->setText(QStringLiteral("0"));
    }
    else
    {
        _sliderCenterLabel->setText(QStringLiteral("]-%1 ; %1[").arg(min));
    }
}

void P402VlWidget::setTargetZero()
{
    _nodeProfile402->setTarget(0);
}

void P402VlWidget::enableRampClicked(bool ok)
{
    // 0 -> Velocity demand value shall be controlled in any other
    // 1 -> Velocity demand value shall accord with ramp output value

    if (_nodeProfile402 != nullptr)
    {
        _modeVl->setEnableRamp(ok);
    }
}

void P402VlWidget::unlockRampClicked(bool ok)
{
    // 0 -> Ramp output value shall be locked to current output value
    // 1 -> Ramp output value shall follow ramp input value

    if (_nodeProfile402 != nullptr)
    {
        _modeVl->setUnlockRamp(ok);
    }
}

void P402VlWidget::referenceRampClicked(bool ok)
{
    // 0 -> Ramp input value shall be set to zero
    // 1 -> Ramp input value shall accord with ramp reference
    if (_nodeProfile402 != nullptr)
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
        text = QStringLiteral("Ramp is disabled");
    }
    if (!_unlockRampCheckBox->isChecked())
    {
        if (!text.isEmpty())
        {
            text.append(QStringLiteral(" & "));
        }
        text += QStringLiteral("Ramp is locked");
    }
    if (!_referenceRampCheckBox->isChecked())
    {
        if (!text.isEmpty())
        {
            text.append(QStringLiteral(" & "));
        }
        text += QStringLiteral("Ramp set to 0");
    }
    _infoLabel->setText(text);
}

void P402VlWidget::createActions()
{
    createDefaultActions();
}

void P402VlWidget::showDiagram()
{
    QPixmap vlModePixmap;
    QLabel *vlModeLabel;
    vlModeLabel = new QLabel();
    vlModeLabel->setAttribute(Qt::WA_DeleteOnClose);
    vlModePixmap.load(QStringLiteral(":/diagram/img/diagrams/402VLDiagram.png"));
    vlModeLabel->setPixmap(vlModePixmap);
    vlModeLabel->setWindowTitle(tr("402 VL Diagram"));
    vlModeLabel->show();
}

void P402VlWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 VL").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axis()));

    dataLogger->addData(_modeVl->velocityActualObjectId());
    dataLogger->addData(_modeVl->targetObjectId());
    dataLogger->addData(_modeVl->velocityDemandObjectId());

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();
}

void P402VlWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> vlRpdoObjectList = {controlWordObjectId, _modeVl->targetObjectId()};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(vlRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> vlTpdoObjectList = {statusWordObjectId, _modeVl->velocityDemandObjectId()};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(vlTpdoObjectList);
}

void P402VlWidget::createWidgets()
{
    // Group Box Velocity mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Velocity mode"));
    IndexFormLayout *indexLayout = new IndexFormLayout();

    createTargetWidgets(indexLayout);
    createInformationWidgets(indexLayout);
    createLimitWidgets(indexLayout);

    indexLayout->addLineSeparator();

    createAccelDeccelWidgets(indexLayout);

    indexLayout->addLineSeparator();

    createFactorWidgets(indexLayout);

    modeGroupBox->setLayout(indexLayout);

    // Create interface
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(modeGroupBox);
    layout->addWidget(createControlWordWidgets());

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(scrollArea);
    setLayout(vBoxLayout);
}

void P402VlWidget::createTargetWidgets(IndexFormLayout *indexLayout)
{
    _targetVelocitySpinBox = new IndexSpinBox();
    _targetVelocitySpinBox->setRangeValue(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    indexLayout->addRow(tr("&Target velocity:"), _targetVelocitySpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _sliderMinLabel = new QLabel(QStringLiteral("min"));
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _sliderCenterLabel = new QLabel(QStringLiteral("0"));
    labelSliderLayout->addWidget(_sliderCenterLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _sliderMaxLabel = new QLabel(QStringLiteral("max"));
    labelSliderLayout->addWidget(_sliderMaxLabel);
    indexLayout->addRow(labelSliderLayout);

    _targetVelocitySlider = new IndexSlider(Qt::Horizontal);
    _targetVelocitySlider->setTickPosition(QSlider::TicksBothSides);
    indexLayout->addRow(_targetVelocitySlider);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText(tr("Set to 0"));
    connect(setZeroButton, &QPushButton::clicked, this, &P402VlWidget::setTargetZero);
    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    indexLayout->addRow(setZeroLayout);
}

void P402VlWidget::createInformationWidgets(IndexFormLayout *indexLayout)
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet(QStringLiteral("QLabel { color : red; }"));
    indexLayout->addRow(tr("Information:"), _infoLabel);

    _velocityDemandLabel = new IndexLabel();
    indexLayout->addRow(tr("Velocity demand:"), _velocityDemandLabel);

    _velocityActualLabel = new IndexLabel();
    indexLayout->addRow(tr("Velocity actual value:"), _velocityActualLabel);
}

void P402VlWidget::createLimitWidgets(IndexFormLayout *indexLayout)
{
    _minVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _maxVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("Min / max amo&unt:"), _minVelocityMinMaxAmountSpinBox, _maxVelocityMinMaxAmountSpinBox, tr("-"));
}

void P402VlWidget::createAccelDeccelWidgets(IndexFormLayout *indexLayout)
{
    // ACCELERATION
    _accelerationDeltaSpeedSpinBox = new IndexSpinBox();
    _accelerationDeltaTimeSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("&Acceleration:"), _accelerationDeltaSpeedSpinBox, _accelerationDeltaTimeSpinBox, tr("/"));

    // DECELERATION
    _decelerationDeltaSpeedSpinBox = new IndexSpinBox();
    _decelerationDeltaTimeSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("&Deceleration:"), _decelerationDeltaSpeedSpinBox, _decelerationDeltaTimeSpinBox, tr("/"));

    // DECELERATION QUICKSTOP
    _quickStopDeltaSpeedSpinBox = new IndexSpinBox();
    _quickStopDeltaTimeSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("&Quick stop deceleration:"), _quickStopDeltaSpeedSpinBox, _quickStopDeltaTimeSpinBox, tr("/"));
}

void P402VlWidget::createFactorWidgets(IndexFormLayout *indexLayout)
{
    // SET-POINT FACTOR
    _setPointFactorNumeratorSpinBox = new IndexSpinBox();
    _setPointFactorDenominatorSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("&Set-point factor:"), _setPointFactorNumeratorSpinBox, _setPointFactorDenominatorSpinBox, tr("/"));

    // DIMENSION FACTOR
    _dimensionFactorNumeratorSpinBox = new IndexSpinBox();
    _dimensionFactorDenominatorSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("D&imension factor:"), _dimensionFactorNumeratorSpinBox, _dimensionFactorDenominatorSpinBox, tr("/"));
}

QGroupBox *P402VlWidget::createControlWordWidgets()
{
    // Group Box CONTROL WORD
    QGroupBox *groupBox = new QGroupBox(tr("Control word:"));
    QFormLayout *layout = new QFormLayout();

    _enableRampCheckBox = new QCheckBox();
    layout->addRow(tr("Enable ramp (bit 4):"), _enableRampCheckBox);
    connect(_enableRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::enableRampClicked);

    _unlockRampCheckBox = new QCheckBox();
    layout->addRow(tr("Unlock ramp (bit 5):"), _unlockRampCheckBox);
    connect(_unlockRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::unlockRampClicked);

    _referenceRampCheckBox = new QCheckBox();
    layout->addRow(tr("Not set ramp to zero (bit 6):"), _referenceRampCheckBox);
    connect(_referenceRampCheckBox, &QCheckBox::clicked, this, &P402VlWidget::referenceRampClicked);
    groupBox->setLayout(layout);

    return groupBox;
}

void P402VlWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if (objId == _modeVl->maxVelocityMinMaxAmountObjectId() || objId == _modeVl->minVelocityMinMaxAmountObjectId())
    {
        updateMinMaxVelocity();
    }
    else if (objId == _modeVl->velocityDemandObjectId())
    {
        int value = _nodeProfile402->node()->nodeOd()->value(objId).toInt();
        _targetVelocitySlider->setFeedBackValue(value);
    }
}
