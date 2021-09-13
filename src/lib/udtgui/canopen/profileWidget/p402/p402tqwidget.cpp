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
#include "canopen/indexWidget/indexspinbox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "services/services.h"

#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modetq.h"

#include <QPushButton>

P402TqWidget::P402TqWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    createWidgets();
    _nodeProfile402 = nullptr;
}

P402TqWidget::~P402TqWidget()
{
    unRegisterFullOd();
}

void P402TqWidget::readRealTimeObjects()
{
    if (_node)
    {
        _modeTq->readRealTimeObjects();
    }
}

void P402TqWidget::readAllObjects()
{
    if (_node)
    {
        _modeTq->readAllObjects();
    }
}

void P402TqWidget::reset()
{
    _modeTq->reset();
}

void P402TqWidget::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    _node = node;

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    if (_node)
    {
        setNodeInterrest(_node);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            _modeTq = dynamic_cast<ModeTq *>(_nodeProfile402->mode(NodeProfile402::OperationMode::TQ));

            _torqueTargetObjectId = _modeTq->targetObjectId();
            registerObjId(_torqueTargetObjectId);

            _torqueDemandObjectId = _modeTq->torqueDemandObjectId();
            _torqueDemandLabel->setObjId(_torqueDemandObjectId);

            _torqueActualValueObjectId = _modeTq->torqueActualValueObjectId();
            _torqueActualValueLabel->setObjId(_torqueActualValueObjectId);

            _targetSlopeSpinBox->setObjId(_modeTq->targetSlopeObjectId());
            _maxTorqueSpinBox->setObjId(_modeTq->maxTorqueObjectId());

            int max = _node->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
            _targetTorqueSlider->setRange(-max, max);
            _targetTorqueSlider->setTickInterval(max / 10);
            _sliderMinLabel->setNum(-max);
            _sliderMaxLabel->setNum(max);
        }

        connect(_maxTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::maxTorqueSpinboxFinished);
    }
}

void P402TqWidget::targetTorqueSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_targetTorqueSpinBox->value());
    _nodeProfile402->setTarget(value);
}

void P402TqWidget::targetTorqueSliderChanged()
{
    qint16 value = static_cast<qint16>(_targetTorqueSlider->value());
    _nodeProfile402->setTarget(value);
}

void P402TqWidget::maxTorqueSpinboxFinished()
{
    int max = _node->nodeOd()->value(_maxTorqueSpinBox->objId()).toInt();
    _targetTorqueSlider->setRange(-max, max);
    _sliderMinLabel->setNum(-max);
    _sliderMaxLabel->setNum(max);
}

void P402TqWidget::setZeroButton()
{
    _nodeProfile402->setTarget(0);
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
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();

    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId, _torqueTargetObjectId};

    _node->rpdos().at(0)->writeMapping(tqRpdoObjectList);
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId, _torqueDemandObjectId};

    _node->tpdos().at(2)->writeMapping(tqTpdoObjectList);
}

void P402TqWidget::createWidgets()
{
    // Group Box TQ mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Torque mode"));
    _modeLayout = new QFormLayout();

    targetWidgets();
    informationWidgets();
    limitWidgets();

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    slopeWidgets();

    modeGroupBox->setLayout(_modeLayout);

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
    vBoxLayout->addWidget(scrollArea);
    vBoxLayout->addLayout(buttonWidgets());
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vBoxLayout);
}

void P402TqWidget::targetWidgets()
{
    _targetTorqueSpinBox = new QSpinBox();
    _targetTorqueSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _modeLayout->addRow(tr("Target torque"), _targetTorqueSpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();

    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    _modeLayout->addRow(labelSliderLayout);

    _targetTorqueSlider = new QSlider(Qt::Horizontal);
    _targetTorqueSlider->setTickPosition(QSlider::TicksBelow);
    _modeLayout->addRow(_targetTorqueSlider);

    connect(_targetTorqueSlider, &QSlider::valueChanged, this, &P402TqWidget::targetTorqueSliderChanged);
    connect(_targetTorqueSpinBox, &QSpinBox::editingFinished, this, &P402TqWidget::targetTorqueSpinboxFinished);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText("Set to 0");
    connect(setZeroButton, &QPushButton::clicked, this, &P402TqWidget::setZeroButton);

    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _modeLayout->addRow(setZeroLayout);
}

void P402TqWidget::informationWidgets()
{
    _torqueDemandLabel = new IndexLabel();
    _modeLayout->addRow(tr("Torque demand:"), _torqueDemandLabel);

    _torqueActualValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Torque actual value:"), _torqueActualValueLabel);
}

void P402TqWidget::limitWidgets()
{
    _maxTorqueSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Ma&x torque "), _maxTorqueSpinBox);
}

void P402TqWidget::slopeWidgets()
{
    _targetSlopeSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("T&arget Slope "), _targetSlopeSpinBox);
}

QHBoxLayout *P402TqWidget::buttonWidgets()
{
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

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(5);
    layout->addWidget(dataLoggerPushButton);
    layout->addWidget(mappingPdoPushButton);
    layout->addWidget(imgPushButton);

    return layout;
}

void P402TqWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if (objId == _torqueTargetObjectId)
    {
        int value = _node->nodeOd()->value(objId).toInt();
        if (!_targetTorqueSpinBox->hasFocus())
        {
            _targetTorqueSpinBox->blockSignals(true);
            _targetTorqueSpinBox->setValue(value);
            _targetTorqueSpinBox->blockSignals(false);
        }
        if (!_targetTorqueSlider->isSliderDown())
        {
            _targetTorqueSlider->blockSignals(true);
            _targetTorqueSlider->setValue(value);
            _targetTorqueSlider->blockSignals(false);
        }
    }
}
