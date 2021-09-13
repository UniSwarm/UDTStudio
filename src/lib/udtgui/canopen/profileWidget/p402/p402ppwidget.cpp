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

#include "p402ppwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexspinbox.h"
#include "canopen/indexWidget/indexcheckbox.h"
#include "services/services.h"

#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modepp.h"

#include <QString>
#include <QStringList>
#include <QtMath>

#include <canopen/indexWidget/indexlabel.h>

P402PpWidget::P402PpWidget(QWidget *parent)
    : P402Mode(parent)
{
    createWidgets();
    _nodeProfile402 = nullptr;
}

P402PpWidget::~P402PpWidget()
{
    unRegisterFullOd();
}

void P402PpWidget::readRealTimeObjects()
{
    if (_node)
    {
        _nodeProfile402->readRealTimeObjects();
    }
}

void P402PpWidget::readAllObjects()
{
    if (_node)
    {
        _nodeProfile402->readAllObjects();
    }
}

void P402PpWidget::setNode(Node *node, uint8_t axis)
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
            _modePp = dynamic_cast<ModePp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::PP));

            connect(_modePp, &ModePp::changeSetImmediatelyEvent, this, &P402PpWidget::changeSetImmediatelyPointEvent);
            connect(_modePp, &ModePp::absRelEvent, this, &P402PpWidget::absRelEvent);
            connect(_modePp, &ModePp::changeOnSetPointEvent, this, &P402PpWidget::changeOnSetPointEvent);

            changeSetImmediatelyPointEvent(_modePp->isChangeSetImmediately());
            absRelEvent(_modePp->isAbsRel());
            changeOnSetPointEvent(_modePp->isChangeOnSetPoint());

            _positionTargetObjectId = _modePp->targetObjectId();
            registerObjId(_positionTargetObjectId);

            _positionDemandValueObjectId = _modePp->positionDemandValueObjectId();
            _positionDemandValueLabel->setObjId(_positionDemandValueObjectId);

            _positionActualValueObjectId = _modePp->positionActualValueObjectId();
            _positionActualValueLabel->setObjId(_positionActualValueObjectId);

            _positionRangeLimitMinSpinBox->setObjId(_modePp->positionRangeLimitMinObjectId());
            _positionRangeLimitMaxSpinBox->setObjId(_modePp->positionRangeLimitMaxObjectId());
            registerObjId(_positionRangeLimitMaxSpinBox->objId());
            _softwarePositionLimitMinSpinBox->setObjId(_modePp->softwarePositionLimitMinObjectId());
            _softwarePositionLimitMaxSpinBox->setObjId(_modePp->softwarePositionLimitMaxObjectId());

            _profileVelocitySpinBox->setObjId(_modePp->profileVelocityObjectId());
            _maxProfileVelocitySpinBox->setObjId(_modePp->maxProfileVelocityObjectId());
            _maxMotorSpeedSpinBox->setObjId(_modePp->maxMotorSpeedObjectId());

            _profileAccelerationSpinBox->setObjId(_modePp->profileAccelerationObjectId());
            _maxAccelerationSpinBox->setObjId(_modePp->maxAccelerationObjectId());
            _profileDecelerationSpinBox->setObjId(_modePp->profileDecelerationObjectId());
            _maxDecelerationSpinBox->setObjId(_modePp->maxDecelerationObjectId());
            _quickStopDecelerationSpinBox->setObjId(_modePp->quickStopDecelerationObjectId());

            _homeOffsetSpinBox->setObjId(_modePp->homeOffsetObjectId());

            _polarityCheckBox->setObjId(_nodeProfile402->fgPolaritybjectId());
        }
    }
}

void P402PpWidget::targetPositionLineEditFinished()
{
    _listDataRecord = _targetPositionLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
    _iteratorForSendDataRecord = 0;
    sendDataRecord();
}

void P402PpWidget::goOneLineEditFinished()
{
    _modePp->newSetPoint(false);
    _nodeProfile402->setTarget(_goOneLineEdit->text().toInt());
    _modePp->newSetPoint(true);
}

void P402PpWidget::twoOneLineEditFinished()
{
    _modePp->newSetPoint(false);
    _nodeProfile402->setTarget(_goTwoLineEdit->text().toInt());
    _modePp->newSetPoint(true);
}

void P402PpWidget::sendDataRecord()
{
    if (_iteratorForSendDataRecord < _listDataRecord.size())
    {
        qint32 value = _listDataRecord.at(_iteratorForSendDataRecord).toInt();
        _node->writeObject(_positionTargetObjectId, QVariant(value));
        _iteratorForSendDataRecord++;
    }
    else
    {
        _targetPositionLineEdit->clear();
        _listDataRecord.clear();
    }
}

void P402PpWidget::changeSetImmediatelyPointCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modePp->setChangeSetImmediately(ok);
    }
    updateInformationLabel();
}

void P402PpWidget::changeSetImmediatelyPointEvent(bool ok)
{
    _changeSetImmediatelyPointCheckBox->setChecked(ok);
    updateInformationLabel();
}

void P402PpWidget::absRelCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modePp->setAbsRel(ok);
    }
}

void P402PpWidget::absRelEvent(bool ok)
{
    _absRelCheckBox->setChecked(ok);
}

void P402PpWidget::changeOnSetPointCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modePp->setChangeOnSetPoint(ok);
    }
    updateInformationLabel();
}

void P402PpWidget::changeOnSetPointEvent(bool ok)
{
    //_changeOnSetPointCheckBox->setChecked(ok);
}

void P402PpWidget::updateInformationLabel()
{
//    QString text;
//    if (!_changeSetImmediatelyPointCheckBox->isChecked())
//    {
//        text = "Change set Immediately disabled";
//    }
//    _infoLabel->setText(text);
}
void P402PpWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_positionDemandValueObjectId);
    dataLogger->addData(_positionTargetObjectId);
    _dataLoggerWidget->show();
}

void P402PpWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();

    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _positionTargetObjectId};
    _node->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _positionDemandValueObjectId};
    _node->tpdos().at(2)->writeMapping(ipTpdoObjectList);
}

void P402PpWidget::createWidgets()
{
    // Group Box PP mode
    QGroupBox *modeGroupBox = new QGroupBox(tr(" Profile position mode"));
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

void P402PpWidget::targetWidgets()
{
    _targetPositionLineEdit = new QLineEdit();

    QHBoxLayout *goOneLayout = new QHBoxLayout();
    _goOneLineEdit = new QLineEdit();
    _goOneLineEdit->setPlaceholderText(tr("Target one"));
    _goOnePushButton = new QPushButton(tr("Go one"));
    goOneLayout->addWidget(_goOneLineEdit);
    goOneLayout->addWidget(_goOnePushButton);

    QHBoxLayout *goTwoLayout = new QHBoxLayout();
    _goTwoLineEdit = new QLineEdit();
    _goTwoLineEdit->setPlaceholderText(tr("Target two"));
    _goTwoPushButton = new QPushButton(tr("Go two"));
    goTwoLayout->addWidget(_goTwoLineEdit);
    goTwoLayout->addWidget(_goTwoPushButton);

    QHBoxLayout *goLayout = new QHBoxLayout();
    goLayout->addLayout(goOneLayout);
    goLayout->addLayout(goTwoLayout);
    _modeLayout->addRow(tr("Position_Target:"), goLayout);

    connect(_goOneLineEdit, &QLineEdit::returnPressed, this, &P402PpWidget::goOneLineEditFinished);
    connect(_goTwoLineEdit, &QLineEdit::returnPressed, this, &P402PpWidget::twoOneLineEditFinished);
    connect(_goOnePushButton, &QPushButton::clicked, this, &P402PpWidget::goOneLineEditFinished);
    connect(_goTwoPushButton, &QPushButton::clicked, this, &P402PpWidget::twoOneLineEditFinished);
}

void P402PpWidget::informationWidgets()
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    _modeLayout->addRow(tr("Information:"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position demand value:"), _positionDemandValueLabel);

    _positionActualValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position actual value:"), _positionActualValueLabel);
}

void P402PpWidget::limitWidgets()
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

void P402PpWidget::velocityWidgets()
{
    // Add Profile velocity (0x6081) and Max motor speed (0x6080)
    _profileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Profile velocity:"), _profileVelocitySpinBox);

    _endVelocitySpinBox = new IndexSpinBox();
    //qfLayout->addRow(tr("End velocity:"), _endVelocitySpinBox);

    // Add Max profile velocity (0x607F) and Max motor speed (0x6080)
    _maxProfileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max profile velocity:"), _maxProfileVelocitySpinBox);

    _maxMotorSpeedSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max motor speed:"), _maxMotorSpeedSpinBox);
}

void P402PpWidget::accelDeccelWidgets()
{
    QLabel *label;

    // Add Profile acceleration (0x6083) and Max acceleration (0x60C5)
    QLayout *accelerationlayout = new QHBoxLayout();

    _profileAccelerationSpinBox = new IndexSpinBox();
    accelerationlayout->addWidget(_profileAccelerationSpinBox);

    _maxAccelerationSpinBox = new IndexSpinBox();

    label = new QLabel(tr("Max:"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    accelerationlayout->addWidget(label);
    accelerationlayout->addWidget(_maxAccelerationSpinBox);
    _modeLayout->addRow(tr("Profile acceleration:"), accelerationlayout);

    // Add Profile deceleration (0x6084) and Max deceleration (0x60C6)
    QLayout *decelerationlayout = new QHBoxLayout();

    _profileDecelerationSpinBox = new IndexSpinBox();
    decelerationlayout->addWidget(_profileDecelerationSpinBox);

    _maxDecelerationSpinBox = new IndexSpinBox();

    label = new QLabel(tr("Max:"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    decelerationlayout->addWidget(label);
    decelerationlayout->addWidget(_maxDecelerationSpinBox);
    _modeLayout->addRow(tr("Profile deceleration:"), decelerationlayout);

    // Add Quick stop deceleration (0x6085)
    _quickStopDecelerationSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Quick stop deceleration:"), _quickStopDecelerationSpinBox);
}

void P402PpWidget::homePolarityWidgets()
{
    // Add Home offset (0x607C)
    _homeOffsetSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    // Polarity (0x607E)
    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    _modeLayout->addRow(tr("Polarity:"), _polarityCheckBox);
}

QGroupBox *P402PpWidget::controlWordWidgets()
{
    // Group Box Control Word
    QGroupBox *groupBox = new QGroupBox(tr("Control Word"));
    QFormLayout *layout = new QFormLayout();

    _changeSetImmediatelyPointCheckBox = new QCheckBox();
    layout->addRow(tr("Change set immediately (bit 5):"), _changeSetImmediatelyPointCheckBox);
    connect(_changeSetImmediatelyPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::changeSetImmediatelyPointCheckBoxRampClicked);
    groupBox->setLayout(layout);

    _absRelCheckBox = new QCheckBox();
    layout->addRow(tr("Abs/Rel (bit 6):"), _absRelCheckBox);
    connect(_absRelCheckBox, &QCheckBox::clicked, this, &P402PpWidget::absRelCheckBoxRampClicked);

    //_changeOnSetPointCheckBox = new QCheckBox();
    //    modeControlWordLayout->addRow(tr("Change on set-point (bit 9):"), _changeOnSetPointCheckBox);
    //    connect(_changeOnSetPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::changeOnSetPointCheckBoxRampClicked);
    //    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    return groupBox;
}

QHBoxLayout *P402PpWidget::buttonWidgets()
{
    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402PpWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402PpWidget::pdoMapping);

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

void P402PpWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if ((objId == _positionTargetObjectId))
    {
        if (!_listDataRecord.isEmpty())
        {
            sendDataRecord();
        }
    }
}
