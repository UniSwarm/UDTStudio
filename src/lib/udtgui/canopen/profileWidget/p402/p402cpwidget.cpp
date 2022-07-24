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
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "profile/p402/modecp.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

P402CpWidget::P402CpWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    createWidgets();
    _nodeProfile402 = nullptr;
}

void P402CpWidget::readRealTimeObjects()
{
    _nodeProfile402->readRealTimeObjects();
}

void P402CpWidget::readAllObjects()
{
    _nodeProfile402->readAllObjects();
}

void P402CpWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        _modeCp = nullptr;
        return;
    }
    _modeCp = dynamic_cast<ModeCp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::CP));

    connect(_modeCp, &ModeCp::absRelEvent, this, &P402CpWidget::absRelEvent);
    absRelEvent(_modeCp->isAbsRel());

    _positionTargetObjectId = _modeCp->targetObjectId();

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
}

void P402CpWidget::goOneLineEditFinished()
{
    _nodeProfile402->setTarget(_goOneLineEdit->text().toInt());
}

void P402CpWidget::twoOneLineEditFinished()
{
    _nodeProfile402->setTarget(_goTwoLineEdit->text().toInt());
}

void P402CpWidget::sendAbsRel(bool ok)
{
    if (_nodeProfile402 != nullptr)
    {
        _modeCp->setAbsRel(ok);
    }
}

void P402CpWidget::absRelEvent(bool ok)
{
    _absRelCheckBox->setChecked(ok);
}

void P402CpWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 DTY").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axisId()));

    dataLogger->addData(_positionDemandValueObjectId);
    dataLogger->addData(_positionTargetObjectId);

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();
}

void P402CpWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();

    QList<NodeObjectId> cpRpdoObjectList = {controlWordObjectId, _positionTargetObjectId};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(cpRpdoObjectList);

    QList<NodeObjectId> cpTpdoObjectList = {statusWordObjectId, _positionDemandValueObjectId};
    _nodeProfile402->node()->tpdos().at(2)->writeMapping(cpTpdoObjectList);
}

void P402CpWidget::showDiagram()
{
    QPixmap cpModePixmap;
    QLabel *cpModeLabel;
    cpModeLabel = new QLabel();
    cpModeLabel->setAttribute(Qt::WA_DeleteOnClose);
    cpModePixmap.load(":/diagram/img/diagrams/402CPDiagram.png");
    cpModeLabel->setPixmap(cpModePixmap);
    cpModeLabel->setWindowTitle("402 CP Diagram");
    cpModeLabel->show();
}

void P402CpWidget::createWidgets()
{
    // Group Box CP mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Continuous position mode"));
    _modeLayout = new QFormLayout();

    createTargetWidgets();
    createInformationWidgets();
    createLimitWidgets();

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    createVelocityWidgets();

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    createAccelDeccelWidgets();

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    createHomePolarityWidgets();
    modeGroupBox->setLayout(_modeLayout);

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
    vBoxLayout->addWidget(scrollArea);
    vBoxLayout->addLayout(createButtonWidgets());
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vBoxLayout);
}

void P402CpWidget::createTargetWidgets()
{
    _targetPositionLineEdit = new QLineEdit();

    QHBoxLayout *goLayout = new QHBoxLayout();
    _goOneLineEdit = new QLineEdit();
    _goOneLineEdit->setPlaceholderText(tr("Target one"));
    goLayout->addWidget(_goOneLineEdit);
    _goOnePushButton = new QPushButton(tr("Go one"));
    connect(_goOneLineEdit, &QLineEdit::returnPressed, this, &P402CpWidget::goOneLineEditFinished);
    connect(_goOnePushButton, &QPushButton::clicked, this, &P402CpWidget::goOneLineEditFinished);
    goLayout->addWidget(_goOnePushButton);

    _goTwoLineEdit = new QLineEdit();
    _goTwoLineEdit->setPlaceholderText(tr("Target two"));
    goLayout->addWidget(_goTwoLineEdit);
    _goTwoPushButton = new QPushButton(tr("Go two"));
    connect(_goTwoLineEdit, &QLineEdit::returnPressed, this, &P402CpWidget::twoOneLineEditFinished);
    connect(_goTwoPushButton, &QPushButton::clicked, this, &P402CpWidget::twoOneLineEditFinished);
    goLayout->addWidget(_goTwoPushButton);

    QLabel *labelPositionTarget = new QLabel(tr("Position &target:"));
    labelPositionTarget->setBuddy(_goOneLineEdit);
    _modeLayout->addRow(labelPositionTarget, goLayout);
}

void P402CpWidget::createInformationWidgets()
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    _modeLayout->addRow(tr("Information:"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position demand value:"), _positionDemandValueLabel);

    _positionActualValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position actual value:"), _positionActualValueLabel);
}

void P402CpWidget::createLimitWidgets()
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
    label->setToolTip(tr("Min, max"));
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
    label->setToolTip(tr("Min, max"));
    label->setBuddy(_softwarePositionLimitMinSpinBox);

    _modeLayout->addRow(label, softwareLayout);
}

void P402CpWidget::createVelocityWidgets()
{
    _profileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Profile velocity:"), _profileVelocitySpinBox);

    _maxProfileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max profile velocity:"), _maxProfileVelocitySpinBox);

    _maxMotorSpeedSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max motor speed:"), _maxMotorSpeedSpinBox);
}

void P402CpWidget::createAccelDeccelWidgets()
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

void P402CpWidget::createHomePolarityWidgets()
{
    _homeOffsetSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    _modeLayout->addRow(tr("Polarity:"), _polarityCheckBox);
}

QGroupBox *P402CpWidget::createControlWordWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Control word"));
    QFormLayout *layout = new QFormLayout();

    _absRelCheckBox = new QCheckBox();
    layout->addRow(tr("Abs / rel (bit 6):"), _absRelCheckBox);
    connect(_absRelCheckBox, &QCheckBox::clicked, this, &P402CpWidget::sendAbsRel);

    groupBox->setLayout(layout);
    return groupBox;
}

QHBoxLayout *P402CpWidget::createButtonWidgets() const
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(5);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402CpWidget::createDataLogger);
    layout->addWidget(dataLoggerPushButton);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Map CP to PDOs"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402CpWidget::mapDefaultObjects);
    layout->addWidget(mappingPdoPushButton);

    QPushButton *imgPushButton = new QPushButton(tr("Diagram CP mode"));
    connect(imgPushButton, &QPushButton::clicked, this, &P402CpWidget::showDiagram);
    layout->addWidget(imgPushButton);

    return layout;
}

void P402CpWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}
