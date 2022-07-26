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
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "profile/p402/modecp.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

P402CpWidget::P402CpWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _modeCp = nullptr;

    createWidgets();
    createActions();
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

    _positionDemandValueLabel->setObjId(_modeCp->positionDemandValueObjectId());
    _positionActualValueLabel->setObjId(_modeCp->positionActualValueObjectId());

    _positionRangeLimitMinSpinBox->setObjId(_modeCp->positionRangeLimitMinObjectId());
    _positionRangeLimitMaxSpinBox->setObjId(_modeCp->positionRangeLimitMaxObjectId());
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
    if (_q15Action->isChecked())
    {
        _nodeProfile402->setTarget(_goOneLineEdit->text().toDouble() * 65536);
    }
    else
    {
        _nodeProfile402->setTarget(_goOneLineEdit->text().toInt());
    }
}

void P402CpWidget::twoOneLineEditFinished()
{
    if (_q15Action->isChecked())
    {
        _nodeProfile402->setTarget(_goTwoLineEdit->text().toDouble() * 65536);
    }
    else
    {
        _nodeProfile402->setTarget(_goTwoLineEdit->text().toInt());
    }
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

void P402CpWidget::setQ15Position(bool q15)
{
    _positionDemandValueLabel->setQ1516View(q15);
    _positionActualValueLabel->setQ1516View(q15);
    _positionRangeLimitMinSpinBox->setQ1516View(q15);
    _positionRangeLimitMaxSpinBox->setQ1516View(q15);
    _softwarePositionLimitMinSpinBox->setQ1516View(q15);
    _softwarePositionLimitMaxSpinBox->setQ1516View(q15);
    _profileVelocitySpinBox->setQ1516View(q15);
    _maxProfileVelocitySpinBox->setQ1516View(q15);
    _maxMotorSpeedSpinBox->setQ1516View(q15);
    _profileAccelerationSpinBox->setQ1516View(q15);
    _maxAccelerationSpinBox->setQ1516View(q15);
    _profileDecelerationSpinBox->setQ1516View(q15);
    _maxDecelerationSpinBox->setQ1516View(q15);
    _quickStopDecelerationSpinBox->setQ1516View(q15);
    _homeOffsetSpinBox->setQ1516View(q15);
}

void P402CpWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 CP").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axis()));

    dataLogger->addData(_modeCp->positionDemandValueObjectId());
    dataLogger->addData(_modeCp->targetObjectId());

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

    QList<NodeObjectId> cpRpdoObjectList = {controlWordObjectId, _modeCp->targetObjectId()};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(cpRpdoObjectList);

    QList<NodeObjectId> cpTpdoObjectList = {statusWordObjectId, _modeCp->positionDemandValueObjectId()};
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

void P402CpWidget::createActions()
{
    QAction *action;

    createDefaultActions();

    action = new QAction(this);
    action->setSeparator(true);
    _modeActions.append(action);

    _q15Action = new QAction(tr("Q1516 position"), this);
    _q15Action->setIcon(QIcon(":/icons/img/q15option.png"));
    _q15Action->setCheckable(true);
    _q15Action->setStatusTip(tr("Set all position widgets with Q15.16 attribute"));
    _modeActions.append(_q15Action);
    connect(_q15Action, &QAction::triggered, this, &P402CpWidget::setQ15Position);
}

void P402CpWidget::createWidgets()
{
    // Group Box CP mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Continuous position mode"));
    IndexFormLayout *indexLayout = new IndexFormLayout();

    createTargetWidgets(indexLayout);
    createInformationWidgets(indexLayout);
    createLimitWidgets(indexLayout);
    indexLayout->addLineSeparator();

    createVelocityWidgets(indexLayout);
    indexLayout->addLineSeparator();

    createAccelDeccelWidgets(indexLayout);
    indexLayout->addLineSeparator();

    createHomePolarityWidgets(indexLayout);
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

void P402CpWidget::createTargetWidgets(IndexFormLayout *indexLayout)
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
    indexLayout->addRow(labelPositionTarget, goLayout);
}

void P402CpWidget::createInformationWidgets(IndexFormLayout *indexLayout)
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    indexLayout->addRow(tr("Information:"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    indexLayout->addRow(tr("Position demand value:"), _positionDemandValueLabel);

    _positionActualValueLabel = new IndexLabel();
    indexLayout->addRow(tr("Position actual value:"), _positionActualValueLabel);
}

void P402CpWidget::createLimitWidgets(IndexFormLayout *indexLayout)
{
    // POSITION RANGE LIMIT
    _positionRangeLimitMinSpinBox = new IndexSpinBox();
    _positionRangeLimitMaxSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("&Position range limits:"), _positionRangeLimitMinSpinBox, _positionRangeLimitMaxSpinBox, tr("-"));

    // SOFTWARE RANGE LIMIT
    _softwarePositionLimitMinSpinBox = new IndexSpinBox();
    _softwarePositionLimitMaxSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("Software position limits:"), _softwarePositionLimitMinSpinBox, _softwarePositionLimitMaxSpinBox, tr("-"));
}

void P402CpWidget::createVelocityWidgets(IndexFormLayout *indexLayout)
{
    _profileVelocitySpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Profile velocity:"), _profileVelocitySpinBox);

    _maxProfileVelocitySpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Max profile velocity:"), _maxProfileVelocitySpinBox);

    _maxMotorSpeedSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Max motor speed:"), _maxMotorSpeedSpinBox);
}

void P402CpWidget::createAccelDeccelWidgets(IndexFormLayout *indexLayout)
{
    _profileAccelerationSpinBox = new IndexSpinBox();
    _maxAccelerationSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("Profile acceleration:"), _profileAccelerationSpinBox, _maxAccelerationSpinBox, tr("Max:"));

    _profileDecelerationSpinBox = new IndexSpinBox();
    _maxDecelerationSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("Profile deceleration:"), _profileDecelerationSpinBox, _maxDecelerationSpinBox, tr("Max:"));

    _quickStopDecelerationSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Quick stop deceleration:"), _quickStopDecelerationSpinBox);
}

void P402CpWidget::createHomePolarityWidgets(IndexFormLayout *indexLayout)
{
    _homeOffsetSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    indexLayout->addRow(tr("Polarity:"), _polarityCheckBox);
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

void P402CpWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}
