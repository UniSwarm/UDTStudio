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

#include "p402cpwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexspinbox.h"
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexlabel.h"

#include "services/services.h"
#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modecp.h"

P402CpWidget::P402CpWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    createWidgets();
    _nodeProfile402 = nullptr;
}

P402CpWidget::~P402CpWidget()
{
    unRegisterFullOd();
}

void P402CpWidget::targetPositionSpinboxFinished()
{
    qint32 value = static_cast<qint16>(_positionTargetSpinBox->value());
    _nodeProfile402->setTarget(value);
}

void P402CpWidget::targetPositionSliderChanged()
{
    qint32 value = static_cast<qint16>(_positionTargetSlider->value());
    _nodeProfile402->setTarget(value);
}

void P402CpWidget::maxPositionSpinboxFinished()
{
    int max = _node->nodeOd()->value(_positionRangeLimitMaxSpinBox->objId()).toInt();
    _positionTargetSlider->setRange(-max, max);
    _sliderMinLabel->setNum(-max);
    _sliderMaxLabel->setNum(max);
}

void P402CpWidget::setZeroButton()
{
    _nodeProfile402->setTarget(0);
}

void P402CpWidget::absRelCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modeCp->setAbsRel(ok);
    }
}

void P402CpWidget::absRelEvent(bool ok)
{
    _absRelCheckBox->setChecked(ok);
}

void P402CpWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_positionDemandValueObjectId);
    dataLogger->addData(_positionTargetObjectId);
    _dataLoggerWidget->show();
}

void P402CpWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();

    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _positionTargetObjectId};
    _node->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _positionDemandValueObjectId};
    _node->tpdos().at(2)->writeMapping(ipTpdoObjectList);
}

void P402CpWidget::createWidgets()
{
    // Group Box CP mode
    QGroupBox *modeGroupBox = new QGroupBox(tr(" Continuous position mode"));
    _modeLayout = new QFormLayout();

    targetWidgets();
    informationWidgets();
    limitWidgets();

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    velocityWidgets();

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    accelDeccelWidgets();

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    homePolarityWidgets();
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

void P402CpWidget::targetWidgets()
{
    _positionTargetSpinBox = new QSpinBox();
    _positionTargetSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    _modeLayout->addRow(tr("Position Target:"), _positionTargetSpinBox);

    QLayout *labelSliderLayout = new QHBoxLayout();
    _sliderMinLabel = new QLabel("min");
    labelSliderLayout->addWidget(_sliderMinLabel);
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    labelSliderLayout->addWidget(new QLabel("0"));
    labelSliderLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _sliderMaxLabel = new QLabel("max");
    labelSliderLayout->addWidget(_sliderMaxLabel);
    _modeLayout->addRow(labelSliderLayout);

    _positionTargetSlider = new QSlider(Qt::Horizontal);
    _positionTargetSlider->setTickPosition(QSlider::TicksBothSides);
    _modeLayout->addRow(_positionTargetSlider);

    connect(_positionTargetSlider, &QSlider::valueChanged, this, &P402CpWidget::targetPositionSliderChanged);
    connect(_positionTargetSpinBox, &QSpinBox::editingFinished, this, &P402CpWidget::targetPositionSpinboxFinished);

    QPushButton *setZeroButton = new QPushButton();
    setZeroButton->setText("Set to 0");
    connect(setZeroButton, &QPushButton::clicked, this, &P402CpWidget::setZeroButton);
    QLayout *setZeroLayout = new QHBoxLayout();
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    setZeroLayout->addWidget(setZeroButton);
    setZeroLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    _modeLayout->addRow(setZeroLayout);
}

void P402CpWidget::informationWidgets()
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    _modeLayout->addRow(tr("Information:"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position demand value:"), _positionDemandValueLabel);

    _positionActualValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position actual value:"), _positionActualValueLabel);
}

void P402CpWidget::limitWidgets()
{
    // POSITION RANGE LIMIT
    QLayout *positionLayout = new QHBoxLayout();
    positionLayout->setSpacing(0);

    _positionRangeLimitMinSpinBox = new IndexSpinBox();
    positionLayout->addWidget(_positionRangeLimitMinSpinBox);
    QLabel *label = new QLabel(tr(":"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    positionLayout->addWidget(label);

    _positionRangeLimitMaxSpinBox = new IndexSpinBox();
    positionLayout->addWidget(_positionRangeLimitMaxSpinBox);
    label = new QLabel(tr("&Position range limit:"));
    label->setToolTip(tr("Min, Max"));
    label->setBuddy(_positionRangeLimitMinSpinBox);
    _modeLayout->addRow(label, positionLayout);

    // SOFTWARE RANGE LIMIT
    QLayout *softwareLayout = new QHBoxLayout();
    softwareLayout->setSpacing(0);

    _softwarePositionLimitMinSpinBox = new IndexSpinBox();
    softwareLayout->addWidget(_softwarePositionLimitMinSpinBox);
    label = new QLabel(tr(":"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    softwareLayout->addWidget(label);

    _softwarePositionLimitMaxSpinBox = new IndexSpinBox();
    softwareLayout->addWidget(_softwarePositionLimitMaxSpinBox);
    label = new QLabel(tr("&Software position limit:"));
    label->setToolTip(tr("Min, Max"));
    label->setBuddy(_softwarePositionLimitMinSpinBox);

    _modeLayout->addRow(label, softwareLayout);
}

void P402CpWidget::velocityWidgets()
{
    _profileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Profile velocity:"), _profileVelocitySpinBox);

    _maxProfileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max profile velocity:"), _maxProfileVelocitySpinBox);

    _maxMotorSpeedSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max motor speed:"), _maxMotorSpeedSpinBox);
}

void P402CpWidget::accelDeccelWidgets()
{
    QLabel *label;
    QLayout *accelerationlayout = new QHBoxLayout();

    _profileAccelerationSpinBox = new IndexSpinBox();
    accelerationlayout->addWidget(_profileAccelerationSpinBox);

    _maxAccelerationSpinBox = new IndexSpinBox();
    label = new QLabel(tr("Max:"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    accelerationlayout->addWidget(label);
    accelerationlayout->addWidget(_maxAccelerationSpinBox);
    _modeLayout->addRow(tr("Profile acceleration:"), accelerationlayout);

    QLayout *decelerationlayout = new QHBoxLayout();
    _profileDecelerationSpinBox = new IndexSpinBox();
    decelerationlayout->addWidget(_profileDecelerationSpinBox);

    _maxDecelerationSpinBox = new IndexSpinBox();
    label = new QLabel(tr("Max:"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    decelerationlayout->addWidget(label);
    decelerationlayout->addWidget(_maxDecelerationSpinBox);
    _modeLayout->addRow(tr("Profile deceleration:"), decelerationlayout);

    _quickStopDecelerationSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Quick stop deceleration:"), _quickStopDecelerationSpinBox);
}

void P402CpWidget::homePolarityWidgets()
{
    _homeOffsetSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    _modeLayout->addRow(tr("Polarity:"), _polarityCheckBox);
}

QGroupBox *P402CpWidget::controlWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Control Word"));
    QFormLayout *layout = new QFormLayout();

    _absRelCheckBox = new QCheckBox();
    layout->addRow(tr("Abs/Rel (bit 6):"), _absRelCheckBox);
    connect(_absRelCheckBox, &QCheckBox::clicked, this, &P402CpWidget::absRelCheckBoxRampClicked);

    groupBox->setLayout(layout);
    return groupBox;
}

QHBoxLayout *P402CpWidget::buttonWidgets()
{
    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402CpWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402CpWidget::pdoMapping);

    QPixmap ipModePixmap;
    QLabel *ipModeLabel;
    ipModeLabel = new QLabel();
    ipModePixmap.load(":/diagram/img/diagrams/402IPDiagram.png");
    ipModeLabel->setPixmap(ipModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram PP mode"));
    connect(imgPushButton, SIGNAL(clicked()), ipModeLabel, SLOT(show()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(5);
    layout->addWidget(dataLoggerPushButton);
    layout->addWidget(mappingPdoPushButton);
    layout->addWidget(imgPushButton);

    return layout;
}

void P402CpWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if (objId == _positionTargetObjectId)
    {
        int value = _node->nodeOd()->value(objId).toInt();
        if (!_positionTargetSpinBox->hasFocus())
        {
            _positionTargetSpinBox->blockSignals(true);
            _positionTargetSpinBox->setValue(value);
            _positionTargetSpinBox->blockSignals(false);
        }
        if (!_positionTargetSlider->isSliderDown())
        {
            _positionTargetSlider->blockSignals(true);
            _positionTargetSlider->setValue(value);
            _positionTargetSlider->blockSignals(false);
        }
    }
    else if (objId == _positionRangeLimitMaxSpinBox->objId())
    {
        maxPositionSpinboxFinished();
    }
}

void P402CpWidget::readRealTimeObjects()
{
    _nodeProfile402->readRealTimeObjects();
}

void P402CpWidget::readAllObjects()
{
    _nodeProfile402->readAllObjects();
}

void P402CpWidget::setNode(Node *node, uint8_t axis)
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
        setNodeInterrest(node);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            _modeCp = dynamic_cast<ModeCp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::CP));

            connect(_modeCp, &ModeCp::absRelEvent, this, &P402CpWidget::absRelEvent);
            absRelEvent(_modeCp->isAbsRel());

            _positionTargetObjectId = _modeCp->targetObjectId();
            registerObjId(_positionTargetObjectId);

            _positionDemandValueObjectId = _modeCp->positionDemandValueObjectId();
            _positionDemandValueLabel->setObjId(_positionDemandValueObjectId);

            _positionActualValueObjectId = _modeCp->positionActualValueObjectId();
            _positionActualValueLabel->setObjId(_positionActualValueObjectId);

            _positionRangeLimitMinSpinBox->setObjId(_modeCp->positionRangeLimitMinObjectId());
            _positionRangeLimitMaxSpinBox->setObjId(_modeCp->positionRangeLimitMaxObjectId());
            registerObjId(_positionRangeLimitMaxSpinBox->objId());
            _softwarePositionLimitMinSpinBox->setObjId(_modeCp->softwarePositionLimitMinObjectId());
            _softwarePositionLimitMaxSpinBox->setObjId(_modeCp->softwarePositionLimitMaxObjectId());

            _profileVelocitySpinBox->setObjId(_modeCp->profileVelocityObjectId());
            _maxProfileVelocitySpinBox->setObjId(_modeCp->maxProfileVelocityObjectId());
            _maxMotorSpeedSpinBox->setObjId(_modeCp->maxMotorSpeedObjectId());

            _profileAccelerationSpinBox->setObjId(_modeCp->profileAccelerationObjectId());
            _maxAccelerationSpinBox->setObjId(_modeCp->maxAccelerationObjectId());
            _profileDecelerationSpinBox->setObjId(_modeCp->profileDecelerationObjectId());
            _maxDecelerationSpinBox->setObjId(_modeCp->maxDecelerationObjectId());
            _quickStopDecelerationSpinBox->setObjId(_modeCp->quickStopDecelerationObjectId());

            _homeOffsetSpinBox->setObjId(_modeCp->homeOffsetObjectId());
            _polarityCheckBox->setObjId(_nodeProfile402->fgPolaritybjectId());

            int max = _node->nodeOd()->value(_positionRangeLimitMaxSpinBox->objId()).toInt();
            _positionTargetSlider->setValue(_node->nodeOd()->value(_positionTargetObjectId).toInt());
            _positionTargetSlider->setRange(-max, max);
        }
    }
}
