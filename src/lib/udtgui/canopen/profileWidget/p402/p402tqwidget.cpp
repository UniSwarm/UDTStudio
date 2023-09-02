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
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexslider.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "profile/p402/modetq.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

#include <QPushButton>

P402TqWidget::P402TqWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _modeTq = nullptr;

    createWidgets();
    createActions();
}

void P402TqWidget::reset()
{
    _modeTq->reset();
}

void P402TqWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        _modeTq = nullptr;
        return;
    }
    _modeTq = dynamic_cast<ModeTq *>(_nodeProfile402->mode(NodeProfile402::OperationMode::TQ));

    _targetTorqueSpinBox->setObjId(_modeTq->targetObjectId());
    _targetTorqueSlider->setObjId(_modeTq->targetObjectId());
    _torqueDemandLabel->setObjId(_modeTq->torqueDemandObjectId());
    _torqueActualValueLabel->setObjId(_modeTq->torqueActualValueObjectId());

    _targetSlopeSpinBox->setObjId(_modeTq->targetSlopeObjectId());
    _maxTorqueSpinBox->setObjId(_modeTq->maxTorqueObjectId());

    registerObjId(_modeTq->maxTorqueObjectId());
    registerObjId(_modeTq->torqueDemandObjectId());

    updateMaxTorque();
}

void P402TqWidget::updateMaxTorque()
{
    int max = _nodeProfile402->node()->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
    _targetTorqueSlider->setRange(-max, max);
    _targetTorqueSlider->setTickInterval(max / 10);
    _sliderMinLabel->setText(QString("[-%1").arg(max));
    _sliderMaxLabel->setText(QString("%1]").arg(max));
}

void P402TqWidget::setTargetZero()
{
    _nodeProfile402->setTarget(0);
}

void P402TqWidget::createActions()
{
    createDefaultActions();
}

void P402TqWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 TQ").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axis()));

    dataLogger->addData(_modeTq->torqueActualValueObjectId());
    dataLogger->addData(_modeTq->targetObjectId());
    dataLogger->addData(_modeTq->torqueDemandObjectId());

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();
}

void P402TqWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId, _modeTq->targetObjectId()};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(tqRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId, _modeTq->torqueDemandObjectId()};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(tqTpdoObjectList);
}

void P402TqWidget::showDiagram()
{
    QPixmap tqModePixmap;
    QLabel *tqModeLabel;
    tqModeLabel = new QLabel();
    tqModeLabel->setAttribute(Qt::WA_DeleteOnClose);
    tqModePixmap.load(":/diagram/img/diagrams/402TQDiagram.png");
    tqModeLabel->setPixmap(tqModePixmap);
    tqModeLabel->setWindowTitle("402 TQ Diagram");
    tqModeLabel->show();
}

void P402TqWidget::createWidgets()
{
    // Group Box TQ mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Torque mode"));
    IndexFormLayout *indexLayout = new IndexFormLayout();

    createTargetWidgets(indexLayout);
    createInformationWidgets(indexLayout);
    createLimitWidgets(indexLayout);

    indexLayout->addLineSeparator();

    createSlopeWidgets(indexLayout);

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

void P402TqWidget::createTargetWidgets(IndexFormLayout *indexLayout)
{
    _targetTorqueSpinBox = new IndexSpinBox();
    _targetTorqueSpinBox->setRangeValue(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    indexLayout->addRow(tr("&Target torque"), _targetTorqueSpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();

    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    indexLayout->addRow(labelSliderLayout);

    _targetTorqueSlider = new IndexSlider(Qt::Horizontal);
    _targetTorqueSlider->setTickPosition(QSlider::TicksBelow);
    indexLayout->addRow(_targetTorqueSlider);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText(tr("Set to 0"));
    connect(setZeroButton, &QPushButton::clicked, this, &P402TqWidget::setTargetZero);

    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    indexLayout->addRow(setZeroLayout);
}

void P402TqWidget::createInformationWidgets(IndexFormLayout *indexLayout)
{
    _torqueDemandLabel = new IndexLabel();
    indexLayout->addRow(tr("Torque demand:"), _torqueDemandLabel);

    _torqueActualValueLabel = new IndexLabel();
    indexLayout->addRow(tr("Torque actual value:"), _torqueActualValueLabel);
}

void P402TqWidget::createLimitWidgets(IndexFormLayout *indexLayout)
{
    _maxTorqueSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Ma&x torque "), _maxTorqueSpinBox);
}

void P402TqWidget::createSlopeWidgets(IndexFormLayout *indexLayout)
{
    _targetSlopeSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Target &slope "), _targetSlopeSpinBox);
}

void P402TqWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if (objId == _modeTq->maxTorqueObjectId())
    {
        updateMaxTorque();
    }
    else if (objId == _modeTq->torqueDemandObjectId())
    {
        int value = _nodeProfile402->node()->nodeOd()->value(objId).toInt();
        _targetTorqueSlider->setFeedBackValue(value);
    }
}
