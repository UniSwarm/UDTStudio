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
#include "canopen/widget/indexspinbox.h"
#include "services/services.h"

#include "indexdb402.h"
#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modepp.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QtMath>

#include <canopen/widget/indexlabel.h>

P402PpWidget::P402PpWidget(QWidget *parent)
    : QScrollArea(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);

    _node = nullptr;
    _nodeProfile402 = nullptr;
}

P402PpWidget::~P402PpWidget()
{
    unRegisterFullOd();
}

Node *P402PpWidget::node() const
{
    return _node;
}

void P402PpWidget::readData()
{
    if (_node)
    {
        if (_nodeProfile402->actualMode() == NodeProfile402::OperationMode::PP)
        {
            _node->readObject(_positionDemandValueObjectId);
            _node->readObject(_positionActualValueObjectId);
        }
    }
}

void P402PpWidget::setNode(Node *node, uint8_t axis)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402PpWidget::updateData);
        }
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    _node = node;
    if (_node)
    {
        _positionDemandValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, axis);
        _positionActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, axis);
        _targetPositionObjectId = IndexDb402::getObjectId(IndexDb402::OD_PP_TARGET_POSITION, axis);

        createWidgets();

        _positionDemandValueObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        _positionActualValueObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

        registerObjId(_targetPositionObjectId);
        setNodeInterrest(node);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            _modePp = dynamic_cast<ModePp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::PP));

            newSetPointEvent(_modePp->isNewSetPoint());
            changeSetImmediatelyPointEvent(_modePp->isChangeSetImmediately());
            absRelEvent(_modePp->isAbsRel());
            changeOnSetPointEvent(_modePp->isChangeOnSetPoint());
        }

        _positionDemandValueLabel->setObjId(_positionDemandValueObjectId);
        _positionActualValueLabel->setObjId(_positionActualValueObjectId);

        _positionRangelLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, axis));
        _positionRangelLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, axis));
        _softwarePositionLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, axis));
        _softwarePositionLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, axis));
        _homeOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, axis));
        _profileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_VELOCITY, axis));
        _endVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_END_VELOCITY, axis));
        _maxProfileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, axis));
        _maxMotorSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, axis));
        _profileAccelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_ACCELERATION, axis));
        _maxAccelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_ACCELERATION, axis));
        _profileDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_DECELERATION, axis));
        _maxDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_DECELERATION, axis));
         _quickStopDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_QUICK_STOP_DECELERATION, axis));

        _positionDemandValueLabel->setNode(node);
        _positionActualValueLabel->setNode(node);

        _positionRangelLimitMinSpinBox->setNode(node);
        _positionRangelLimitMaxSpinBox->setNode(node);
        _softwarePositionLimitMinSpinBox->setNode(node);
        _softwarePositionLimitMaxSpinBox->setNode(node);
        _homeOffsetSpinBox->setNode(node);
        _profileVelocitySpinBox->setNode(node);
        _endVelocitySpinBox->setNode(node);
        _maxProfileVelocitySpinBox->setNode(node);
        _maxMotorSpeedSpinBox->setNode(node);
        _profileAccelerationSpinBox->setNode(node);
        _maxAccelerationSpinBox->setNode(node);
        _profileDecelerationSpinBox->setNode(node);
        _maxDecelerationSpinBox->setNode(node);
         _quickStopDecelerationSpinBox->setNode(node);

        _bus = _node->bus();
    }
}

void P402PpWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::OperationMode::PP)
        {
            setEnabled(true);
            _positionDemandValueLabel->readObject();
            _positionActualValueLabel->readObject();
        }
        else
        {
            stop();
        }
    }
}

void P402PpWidget::stop()
{
}

void P402PpWidget::targetPositionLineEditFinished()
{
    _listDataRecord = _targetPositionLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
    _iteratorForSendDataRecord = 0;
    sendDataRecord();
}

void P402PpWidget::sendDataRecord()
{
    if (_iteratorForSendDataRecord < _listDataRecord.size())
    {
        qint32 value = _listDataRecord.at(_iteratorForSendDataRecord).toInt();
        _node->writeObject(_targetPositionObjectId, QVariant(value));
        _iteratorForSendDataRecord++;
    }
    else
    {
        _targetPositionLineEdit->clear();
        _listDataRecord.clear();
    }
}

void P402PpWidget::newSetPointClicked(bool ok)
{
    // 0 Disable interpolation
    // 1 Enable interpolation
    if (_nodeProfile402)
    {
        _modePp->newSetPoint(ok);
    }
    updateInformationLabel();
}

void P402PpWidget::newSetPointEvent(bool ok)
{
    _newSetPointCheckBox->setChecked(ok);
    updateInformationLabel();
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
    _changeOnSetPointCheckBox->setChecked(ok);
}

void P402PpWidget::updateInformationLabel()
{
    QString text;
    //    if (!_ppEnableRampCheckBox->isChecked())
    //    {
    //        text = "Enable Interpolation";
    //    }

    _infoLabel->setText(text);
}
void P402PpWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_positionDemandValueObjectId);
    _dataLoggerWidget->show();
}

void P402PpWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    NodeObjectId statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);
    controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _targetPositionObjectId};
    _node->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _positionDemandValueObjectId};
    _node->tpdos().at(2)->writeMapping(ipTpdoObjectList);
}

void P402PpWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box IP mode
    QGroupBox *ipGroupBox = new QGroupBox(tr(" Profile position mode"));
    QFormLayout *ipLayout = new QFormLayout();

    _targetPositionLineEdit = new QLineEdit();
    ipLayout->addRow(tr("Position_Target :"), _targetPositionLineEdit);
    connect(_targetPositionLineEdit, &QLineEdit::editingFinished, this, &P402PpWidget::targetPositionLineEditFinished);

    _infoLabel = new QLabel();
    ipLayout->addRow(tr("Information :"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    ipLayout->addRow(tr("Position demand value :"), _positionDemandValueLabel);

    _positionActualValueLabel = new IndexLabel();
    ipLayout->addRow(tr("Position actual value :"), _positionActualValueLabel);

    QLabel *ipPositionRangelLimitLabel = new QLabel(tr("Position range limit :"));
    ipLayout->addRow(ipPositionRangelLimitLabel);
    QLayout *ipPositionRangelLimitlayout = new QHBoxLayout();
    _positionRangelLimitMinSpinBox = new IndexSpinBox();
    _positionRangelLimitMinSpinBox->setToolTip("min");
    ipPositionRangelLimitlayout->addWidget(_positionRangelLimitMinSpinBox);
    _positionRangelLimitMaxSpinBox = new IndexSpinBox();
    _positionRangelLimitMaxSpinBox->setToolTip("max");
    ipPositionRangelLimitlayout->addWidget(_positionRangelLimitMaxSpinBox);
    ipLayout->addRow(ipPositionRangelLimitlayout);

    QLabel *ipSoftwarePositionLimitLabel = new QLabel(tr("Software position limit :"));
    ipLayout->addRow(ipSoftwarePositionLimitLabel);
    QLayout *ipSoftwarePositionLimitlayout = new QHBoxLayout();
    _softwarePositionLimitMinSpinBox = new IndexSpinBox();
    _softwarePositionLimitMinSpinBox->setToolTip("min");
    ipSoftwarePositionLimitlayout->addWidget(_softwarePositionLimitMinSpinBox);
    _softwarePositionLimitMaxSpinBox = new IndexSpinBox();
    _softwarePositionLimitMaxSpinBox->setToolTip("max");
    ipSoftwarePositionLimitlayout->addWidget(_softwarePositionLimitMaxSpinBox);
    ipLayout->addRow(ipSoftwarePositionLimitlayout);

    // Add Home offset (0x607C) and Polarity (0x607E)
    QLayout *ipHomeOffsetlayout = new QVBoxLayout();
    QLabel *ipHomeOffsetLabel = new QLabel(tr("Home offset :"));
    _homeOffsetSpinBox = new IndexSpinBox();
    _homeOffsetSpinBox->setToolTip("");
    ipHomeOffsetlayout->addWidget(ipHomeOffsetLabel);
    ipHomeOffsetlayout->addWidget(_homeOffsetSpinBox);

    QLayout *ipPolaritylayout = new QVBoxLayout();
    QLabel *ipPolarityLabel = new QLabel(tr("Polarity :"));
    _polaritySpinBox = new QSpinBox();
    _polaritySpinBox->setToolTip("0 = x1, 1 = x(-1)");
    _polaritySpinBox->setRange(0, 1);
    ipPolaritylayout->addWidget(ipPolarityLabel);
    ipPolaritylayout->addWidget(_polaritySpinBox);
    connect(_polaritySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { _nodeProfile402->setPolarityPosition(i); });

    QHBoxLayout *ipHomePolaritylayout = new QHBoxLayout();
    ipHomePolaritylayout->addLayout(ipHomeOffsetlayout);
    ipHomePolaritylayout->addLayout(ipPolaritylayout);
    ipLayout->addRow(ipHomePolaritylayout);

    // Add Profile velocity (0x6081) and Max motor speed (0x6080)
    QLayout *ipProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipProfileVelocityLabel = new QLabel(tr("Profile velocity :"));
    _profileVelocitySpinBox = new IndexSpinBox();
    _profileVelocitySpinBox->setToolTip("");
    ipProfileVelocitylayout->addWidget(ipProfileVelocityLabel);
    ipProfileVelocitylayout->addWidget(_profileVelocitySpinBox);

    QLayout *ipEndVelocitylayout = new QVBoxLayout();
    QLabel *ipEndVelocityLabel = new QLabel(tr("End velocity :"));
    _endVelocitySpinBox = new IndexSpinBox();
    _endVelocitySpinBox->setToolTip("");
    ipEndVelocitylayout->addWidget(ipEndVelocityLabel);
    ipEndVelocitylayout->addWidget(_endVelocitySpinBox);

    QHBoxLayout *ipMaxVelocitylayout = new QHBoxLayout();
    ipMaxVelocitylayout->addLayout(ipProfileVelocitylayout);
    ipMaxVelocitylayout->addLayout(ipEndVelocitylayout);
    ipLayout->addRow(ipMaxVelocitylayout);

    // Add Max profile velocity (0x607F) and Max motor speed (0x6080)
    QLayout *ipMaxProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipMaxProfileVelocityLabel = new QLabel(tr("Max profile velocity :"));
    _maxProfileVelocitySpinBox = new IndexSpinBox();
    _maxProfileVelocitySpinBox->setToolTip("");
    ipMaxProfileVelocitylayout->addWidget(ipMaxProfileVelocityLabel);
    ipMaxProfileVelocitylayout->addWidget(_maxProfileVelocitySpinBox);

    QLayout *ipMaxMotorSpeedlayout = new QVBoxLayout();
    QLabel *ipMaxMotorSpeedLabel = new QLabel(tr("Max motor speedy :"));
    _maxMotorSpeedSpinBox = new IndexSpinBox();
    _maxMotorSpeedSpinBox->setToolTip("");
    ipMaxMotorSpeedlayout->addWidget(ipMaxMotorSpeedLabel);
    ipMaxMotorSpeedlayout->addWidget(_maxMotorSpeedSpinBox);

    QHBoxLayout *ipVelocitylayout = new QHBoxLayout();
    ipVelocitylayout->addLayout(ipMaxProfileVelocitylayout);
    ipVelocitylayout->addLayout(ipMaxMotorSpeedlayout);
    ipLayout->addRow(ipVelocitylayout);

    // Add Profile acceleration (0x6083) and Max acceleration (0x60C5)
    QLayout *ipProfileAccelerationlayout = new QVBoxLayout();
    QLabel *ipProfileAccelerationLabel = new QLabel(tr("Profile acceleration :"));
    _profileAccelerationSpinBox = new IndexSpinBox();
    _profileAccelerationSpinBox->setToolTip("");
    ipProfileAccelerationlayout->addWidget(ipProfileAccelerationLabel);
    ipProfileAccelerationlayout->addWidget(_profileAccelerationSpinBox);

    QLayout *ipMaxAccelerationlayout = new QVBoxLayout();
    QLabel *ipMaxAccelerationLabel = new QLabel(tr("Max acceleration :"));
    _maxAccelerationSpinBox = new IndexSpinBox();
    _maxAccelerationSpinBox->setToolTip("");
    ipMaxAccelerationlayout->addWidget(ipMaxAccelerationLabel);
    ipMaxAccelerationlayout->addWidget(_maxAccelerationSpinBox);

    QHBoxLayout *ipProfileAccelerationHlayout = new QHBoxLayout();
    ipProfileAccelerationHlayout->addLayout(ipProfileAccelerationlayout);
    ipProfileAccelerationHlayout->addLayout(ipMaxAccelerationlayout);
    ipLayout->addRow(ipProfileAccelerationHlayout);

    // Add Profile deceleration (0x6084) and Max deceleration (0x60C6)
    QLayout *ipProfileDecelerationlayout = new QVBoxLayout();
    QLabel *ipProfileDecelerationLabel = new QLabel(tr("Profile deceleration :"));
    _profileDecelerationSpinBox = new IndexSpinBox();
    _profileDecelerationSpinBox->setToolTip("");
    ipProfileDecelerationlayout->addWidget(ipProfileDecelerationLabel);
    ipProfileDecelerationlayout->addWidget(_profileDecelerationSpinBox);

    QLayout *ipMaxDecelerationlayout = new QVBoxLayout();
    QLabel *ipMaxDecelerationLabel = new QLabel(tr("Max deceleration :"));
    _maxDecelerationSpinBox = new IndexSpinBox();
    _maxDecelerationSpinBox->setToolTip("");
    ipMaxDecelerationlayout->addWidget(ipMaxDecelerationLabel);
    ipMaxDecelerationlayout->addWidget(_maxDecelerationSpinBox);

    QHBoxLayout *ipProfileDecelerationHlayout = new QHBoxLayout();
    ipProfileDecelerationHlayout->addLayout(ipProfileDecelerationlayout);
    ipProfileDecelerationHlayout->addLayout(ipMaxDecelerationlayout);
    ipLayout->addRow(ipProfileDecelerationHlayout);

    // Add Quick stop deceleration (0x6085)
    QLayout *ipQuickStopDecelerationlayout = new QVBoxLayout();
    QLabel *ipQuickStopDecelerationLabel = new QLabel(tr("Quick stop deceleration :"));
     _quickStopDecelerationSpinBox = new IndexSpinBox();
     _quickStopDecelerationSpinBox->setToolTip("");
    ipQuickStopDecelerationlayout->addWidget(ipQuickStopDecelerationLabel);
    ipQuickStopDecelerationlayout->addWidget( _quickStopDecelerationSpinBox);
    ipLayout->addRow(ipQuickStopDecelerationlayout);

    ipGroupBox->setLayout(ipLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    _newSetPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("New set-point (bit 4) :"), _newSetPointCheckBox);
    connect(_newSetPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::newSetPointClicked);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    _changeSetImmediatelyPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Change set immediately (bit 5) :"), _changeSetImmediatelyPointCheckBox);
    connect(_changeSetImmediatelyPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::changeSetImmediatelyPointCheckBoxRampClicked);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    _absRelCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Change on set-point (bit 9) :"), _absRelCheckBox);
    connect(_absRelCheckBox, &QCheckBox::clicked, this, &P402PpWidget::absRelCheckBoxRampClicked);

    _changeOnSetPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Abs/Rel (bit 6) :"), _changeOnSetPointCheckBox);
    connect(_changeOnSetPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::changeOnSetPointCheckBoxRampClicked);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

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
    QHBoxLayout *ipButtonLayout = new QHBoxLayout();
    ipButtonLayout->addWidget(dataLoggerPushButton);
    ipButtonLayout->addWidget(mappingPdoPushButton);
    ipButtonLayout->addWidget(imgPushButton);

    layout->addWidget(ipGroupBox);
    layout->addWidget(modeControlWordGroupBox);
    layout->addItem(ipButtonLayout);

    widget->setLayout(layout);
    setWidget(widget);
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

    if ((objId == _targetPositionObjectId))
    {
        if (!_listDataRecord.isEmpty())
        {
            sendDataRecord();
        }
    }
}
