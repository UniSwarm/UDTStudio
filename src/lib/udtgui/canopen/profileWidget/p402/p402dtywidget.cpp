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
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"
#include "indexdb402.h"
#include "node.h"
#include "profile/p402/nodeprofile402.h"
#include "services/services.h"

#include <QPushButton>

P402DtyWidget::P402DtyWidget(QWidget *parent)
    : P402Mode(parent)
{
    createWidgets();
    _nodeProfile402 = nullptr;
}

P402DtyWidget::~P402DtyWidget()
{
    unRegisterFullOd();
}

void P402DtyWidget::readRealTimeObjects()
{
    if (_node)
    {
        _demandLabel->readObject();
    }
}

void P402DtyWidget::readAllObjects()
{
    if (_node)
    {
        _demandLabel->readObject();
        _slopeSpinBox->readObject();
        _maxSpinBox->readObject();
    }
}

void P402DtyWidget::reset()
{
    _node->readObject(_targetObjectId);
}

void P402DtyWidget::setNode(Node *node, uint8_t axis)
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
        _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_TARGET, axis);
        _demandObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_DEMAND, axis);
        _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        _demandObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

        registerObjId(_targetObjectId);
        setNodeInterrest(_node);

        _demandLabel->setObjId(_demandObjectId);

        _slopeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_SLOPE, axis));
        _maxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_MAX, axis));
        _slopeSpinBox->setNode(node);
        _maxSpinBox->setNode(node);

        int max = _node->nodeOd()->value(_maxSpinBox->objId()).toInt();
        _targetSlider->setRange(-max, max);
        _targetSlider->setTickInterval(max / 10);
        _sliderMinLabel->setNum(-max);
        _sliderMaxLabel->setNum(max);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        }

        connect(_maxSpinBox, &QSpinBox::editingFinished, this, &P402DtyWidget::maxSpinboxFinished);
    }
}

void P402DtyWidget::targetSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_targetSpinBox->value());
    _node->writeObject(_targetObjectId, QVariant(value));
}

void P402DtyWidget::targetSliderChanged()
{
    qint16 value = static_cast<qint16>(_targetSlider->value());
    _node->writeObject(_targetObjectId, QVariant(value));
}

void P402DtyWidget::maxSpinboxFinished()
{
    int max = _node->nodeOd()->value(_maxSpinBox->objId()).toInt();
    _targetSlider->setRange(-max, max);
    _sliderMinLabel->setNum(-max);
    _sliderMaxLabel->setNum(max);
}

void P402DtyWidget::setZeroButton()
{
    _nodeProfile402->setTarget(0);
}

void P402DtyWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_targetObjectId);
    dataLogger->addData(_demandObjectId);
    _dataLoggerWidget->show();
}

void P402DtyWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = NodeObjectId(0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(0x6041, 0, QMetaType::Type::UShort);

    QList<NodeObjectId> tqRpdoObjectList = {controlWordObjectId, _targetObjectId};

    _node->rpdos().at(0)->writeMapping(tqRpdoObjectList);
    QList<NodeObjectId> tqTpdoObjectList = {statusWordObjectId, _demandObjectId};

    _node->tpdos().at(2)->writeMapping(tqTpdoObjectList);
}

void P402DtyWidget::createWidgets()
{
    QGroupBox *modeGroupBox = new QGroupBox(tr("Duty Cycle mode"));
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

void P402DtyWidget::targetWidgets()
{
    _targetSpinBox = new QSpinBox();
    _targetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _modeLayout->addRow(tr("Target"), _targetSpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();

    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    _modeLayout->addRow(labelSliderLayout);

    _targetSlider = new QSlider(Qt::Horizontal);
    _targetSlider->setTickPosition(QSlider::TicksBelow);
    _modeLayout->addRow(_targetSlider);

    connect(_targetSlider, &QSlider::valueChanged, this, &P402DtyWidget::targetSliderChanged);
    connect(_targetSpinBox, &QSpinBox::editingFinished, this, &P402DtyWidget::targetSpinboxFinished);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText("Set to 0");
    connect(setZeroButton, &QPushButton::clicked, this, &P402DtyWidget::setZeroButton);

    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _modeLayout->addRow(setZeroLayout);
}

void P402DtyWidget::informationWidgets()
{
    _demandLabel = new IndexLabel();
    _modeLayout->addRow(tr("Demand:"), _demandLabel);
}

void P402DtyWidget::limitWidgets()
{
    _maxSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Ma&x "), _maxSpinBox);
}

void P402DtyWidget::slopeWidgets()
{
    _slopeSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("T&arget Slope "), _slopeSpinBox);
}

QHBoxLayout *P402DtyWidget::buttonWidgets()
{
    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402DtyWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402DtyWidget::pdoMapping);

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

void P402DtyWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if (objId == _targetObjectId)
    {
        int value = _node->nodeOd()->value(objId).toInt();
        if (!_targetSpinBox->hasFocus())
        {
            _targetSpinBox->blockSignals(true);
            _targetSpinBox->setValue(value);
            _targetSpinBox->blockSignals(false);
        }
        if (!_targetSlider->isSliderDown())
        {
            _targetSlider->blockSignals(true);
            _targetSlider->setValue(value);
            _targetSlider->blockSignals(false);
        }
    }
}
