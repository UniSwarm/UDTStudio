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

#include "p402dtywidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexslider.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "profile/p402/modedty.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

#include <QPushButton>

P402DtyWidget::P402DtyWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _modeDty = nullptr;

    createWidgets();
    createActions();
}

void P402DtyWidget::reset()
{
    _modeDty->reset();
}

void P402DtyWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        _modeDty = nullptr;
        return;
    }
    _modeDty = dynamic_cast<ModeDty *>(_nodeProfile402->mode(NodeProfile402::OperationMode::DTY));
    connect(_enableRampCheckBox, &QCheckBox::clicked, _modeDty, &ModeDty::setEnableRamp);

    _targetSpinBox->setObjId(_modeDty->targetObjectId());
    _targetSlider->setObjId(_modeDty->targetObjectId());

    _demandLabel->setObjId(_modeDty->demandObjectId());
    registerObjId(_modeDty->demandObjectId());

    _slopeSpinBox->setObjId(_modeDty->slopeObjectId());
    _maxSpinBox->setObjId(_modeDty->maxObjectId());
    registerObjId(_modeDty->maxObjectId());

    updateMaxDty();
}

void P402DtyWidget::updateMaxDty()
{
    int max = _nodeProfile402->node()->nodeOd()->value(_maxSpinBox->objId()).toInt();
    _targetSlider->setRange(-max, max);
    _targetSpinBox->setRangeValue(-max, max);
    _targetSlider->setTickInterval(max / 10);
    _sliderMinLabel->setText(QStringLiteral("[-%1").arg(max));
    _sliderMaxLabel->setText(QStringLiteral("%1]").arg(max));
}

void P402DtyWidget::setTargetZero()
{
    _nodeProfile402->setTarget(0);
}

void P402DtyWidget::createActions()
{
    createDefaultActions();
}

void P402DtyWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 DTY").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axis()));

    dataLogger->addData(_modeDty->targetObjectId());
    dataLogger->addData(_modeDty->demandObjectId());

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();
}

void P402DtyWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> dtyRpdoObjectList = {controlWordObjectId, _modeDty->targetObjectId()};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(dtyRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> dtyTpdoObjectList = {statusWordObjectId, _modeDty->demandObjectId()};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(dtyTpdoObjectList);
}

void P402DtyWidget::showDiagram()
{
    QPixmap dtyModePixmap;
    QLabel *dtyModeLabel;
    dtyModeLabel = new QLabel();
    dtyModeLabel->setAttribute(Qt::WA_DeleteOnClose);
    dtyModePixmap.load(QStringLiteral(":/diagram/img/diagrams/402DTYDiagram.png"));
    dtyModeLabel->setPixmap(dtyModePixmap);
    dtyModeLabel->setWindowTitle(tr("402 DTY Diagram"));
    dtyModeLabel->show();
}

void P402DtyWidget::createWidgets()
{
    QGroupBox *modeGroupBox = new QGroupBox(tr("Duty cycle mode"));
    IndexFormLayout *indexLayout = new IndexFormLayout();

    createTargetWidgets(indexLayout);

    modeGroupBox->setLayout(indexLayout);

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

void P402DtyWidget::createTargetWidgets(IndexFormLayout *indexLayout)
{
    _targetSpinBox = new IndexSpinBox();
    _targetSpinBox->setRangeValue(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    indexLayout->addRow(tr("&Target:"), _targetSpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();

    _sliderMinLabel = new QLabel(QStringLiteral("min"));
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel(QStringLiteral("0")));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    _sliderMaxLabel = new QLabel(QStringLiteral("max"));
    labelSliderLayout->addWidget(_sliderMaxLabel);
    indexLayout->addRow(labelSliderLayout);

    _targetSlider = new IndexSlider(Qt::Horizontal);
    _targetSlider->setTickPosition(QSlider::TicksBelow);
    indexLayout->addRow(_targetSlider);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText(tr("Set to 0"));
    connect(setZeroButton, &QPushButton::clicked, this, &P402DtyWidget::setTargetZero);

    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    indexLayout->addRow(setZeroLayout);

    _demandLabel = new IndexLabel();
    indexLayout->addRow(tr("Demand:"), _demandLabel);

    _maxSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Ma&x:"), _maxSpinBox);

    indexLayout->addLineSeparator();

    _slopeSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Target &slope:"), _slopeSpinBox);
}

QGroupBox *P402DtyWidget::createControlWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Control word:"));
    QFormLayout *layout = new QFormLayout();

    _enableRampCheckBox = new QCheckBox();
    _enableRampCheckBox->setChecked(true);
    layout->addRow(tr("Enable ramp (bit 4):"), _enableRampCheckBox);
    groupBox->setLayout(layout);

    return groupBox;
}

void P402DtyWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if (objId == _modeDty->maxObjectId())
    {
        updateMaxDty();
    }
    else if (objId == _modeDty->demandObjectId())
    {
        int value = _nodeProfile402->node()->nodeOd()->value(objId).toInt();
        _targetSlider->setFeedBackValue(value);
    }
}
