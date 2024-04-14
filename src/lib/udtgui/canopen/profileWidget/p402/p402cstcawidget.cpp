/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "p402cstcawidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexslider.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "profile/p402/modecstca.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

#include <QPushButton>

P402CstcaWidget::P402CstcaWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _modeCstca = nullptr;

    createWidgets();
    createActions();
}

void P402CstcaWidget::reset()
{
    _modeCstca->reset();
}

void P402CstcaWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        _modeCstca = nullptr;
        return;
    }
    _modeCstca = dynamic_cast<ModeCstca *>(_nodeProfile402->mode(NodeProfile402::OperationMode::CSTCA));

    _targetTorqueSpinBox->setObjId(_modeCstca->targetObjectId());
    _targetTorqueSlider->setObjId(_modeCstca->targetObjectId());

    _torqueDemandLabel->setObjId(_modeCstca->torqueDemandObjectId());
    registerObjId(_modeCstca->torqueDemandObjectId());

    _torqueActualValueLabel->setObjId(_modeCstca->torqueActualValueObjectId());

    _maxTorqueSpinBox->setObjId(_modeCstca->maxTorqueObjectId());
    _angleSpinBox->setObjId(_modeCstca->commutationAngleObjectId());

    int max = _nodeProfile402->node()->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
    _targetTorqueSlider->setRange(-max, max);
    _targetTorqueSlider->setTickInterval(max / 10);
    _sliderMinLabel->setNum(-max);
    _sliderMaxLabel->setNum(max);
}

void P402CstcaWidget::updateMaxTorque()
{
    int max = _nodeProfile402->node()->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
    _targetTorqueSlider->setRange(-max, max);
    _targetTorqueSlider->setTickInterval(max / 10);
    _sliderMinLabel->setText(QStringLiteral("[-%1").arg(max));
    _sliderMaxLabel->setText(QStringLiteral("%1]").arg(max));
}

void P402CstcaWidget::setTargetZero()
{
    _nodeProfile402->setTarget(0);
}

void P402CstcaWidget::createActions()
{
    createDefaultActions();
}

void P402CstcaWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 TQ").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axis()));

    dataLogger->addData(_modeCstca->torqueActualValueObjectId());
    dataLogger->addData(_modeCstca->targetObjectId());
    dataLogger->addData(_modeCstca->torqueDemandObjectId());

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();
}

void P402CstcaWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId, _modeCstca->targetObjectId()};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(tqRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId, _modeCstca->torqueDemandObjectId()};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(tqTpdoObjectList);
}

void P402CstcaWidget::createWidgets()
{
    QGroupBox *modeGroupBox = new QGroupBox(tr("Cyclic sync torque with angle mode"));
    IndexFormLayout *indexLayout = new IndexFormLayout();

    createTargetWidgets(indexLayout);
    createInformationWidgets(indexLayout);
    createLimitWidgets(indexLayout);

    indexLayout->addLineSeparator();

    createAngleWidgets(indexLayout);

    modeGroupBox->setLayout(indexLayout);

    // Create interface
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(modeGroupBox);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(scrollArea);
    setLayout(vBoxLayout);
}

void P402CstcaWidget::createTargetWidgets(IndexFormLayout *indexLayout)
{
    _targetTorqueSpinBox = new IndexSpinBox();
    _targetTorqueSpinBox->setRangeValue(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    indexLayout->addRow(tr("&Target:"), _targetTorqueSpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();

    _sliderMinLabel = new QLabel(QStringLiteral("min"));
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel(QStringLiteral("0")));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    _sliderMaxLabel = new QLabel(QStringLiteral("max"));
    labelSliderLayout->addWidget(_sliderMaxLabel);
    indexLayout->addRow(labelSliderLayout);

    _targetTorqueSlider = new IndexSlider(Qt::Horizontal);
    _targetTorqueSlider->setTickPosition(QSlider::TicksBelow);
    indexLayout->addRow(_targetTorqueSlider);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText(tr("Set to 0"));
    connect(setZeroButton, &QPushButton::clicked, this, &P402CstcaWidget::setTargetZero);

    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    indexLayout->addRow(setZeroLayout);
}

void P402CstcaWidget::createAngleWidgets(IndexFormLayout *indexLayout)
{
    _angleSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("&Angle:"), _angleSpinBox);
}

void P402CstcaWidget::createInformationWidgets(IndexFormLayout *indexLayout)
{
    _torqueDemandLabel = new IndexLabel();
    indexLayout->addRow(tr("Torque demand:"), _torqueDemandLabel);

    _torqueActualValueLabel = new IndexLabel();
    indexLayout->addRow(tr("Torque actual value:"), _torqueActualValueLabel);
}

void P402CstcaWidget::createLimitWidgets(IndexFormLayout *indexLayout)
{
    _maxTorqueSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Ma&x torque:"), _maxTorqueSpinBox);
}

void P402CstcaWidget::createInterpolationWidgets(IndexFormLayout *indexLayout)
{
    _timePeriodUnitSpinBox = new IndexSpinBox();
    _timePeriodIndexSpinBox = new IndexSpinBox();
    //_timePeriodIndexSpinBox->setDisplayHint(AbstractIndexWidget::DisplayHint::DisplayDirectValue);
    indexLayout->addDualRow(tr("&Interpolation time period:"), _timePeriodUnitSpinBox, _timePeriodIndexSpinBox);
}

void P402CstcaWidget::createHomePolarityWidgets(IndexFormLayout *indexLayout)
{
    _homeOffsetSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    indexLayout->addRow(tr("Polarity:"), _polarityCheckBox);
}

void P402CstcaWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if (objId == _modeCstca->maxTorqueObjectId())
    {
        updateMaxTorque();
    }
    else if (objId == _modeCstca->torqueDemandObjectId())
    {
        int value = _nodeProfile402->node()->nodeOd()->value(objId).toInt();
        _targetTorqueSlider->setFeedBackValue(value);
    }
}
