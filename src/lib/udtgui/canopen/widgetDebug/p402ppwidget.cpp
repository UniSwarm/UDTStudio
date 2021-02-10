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
            _node->readObject(_ppPositionDemandValueObjectId);
            _node->readObject(_ppPositionActualValueObjectId);
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
        _ppPositionDemandValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, axis);
        _ppPositionActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, axis);
        _ppTargetPositionObjectId = IndexDb402::getObjectId(IndexDb402::OD_PP_TARGET_POSITION, axis);
        _ppPolarityObjectId = IndexDb402::getObjectId(IndexDb402::OD_FG_POLARITY, axis);

        createWidgets();

        _ppPositionDemandValueObjectId.setBusId(_node->busId());
        _ppPositionDemandValueObjectId.setNodeId(_node->nodeId());
        _ppPositionActualValueObjectId.setBusId(_node->busId());
        _ppPositionActualValueObjectId.setNodeId(_node->nodeId());
        _ppPolarityObjectId.setBusId(_node->busId());
        _ppPolarityObjectId.setNodeId(_node->nodeId());

        registerObjId(_ppTargetPositionObjectId);
        registerObjId(_ppPolarityObjectId);
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

        _ppPositionDemandValueLabel->setObjId(_ppPositionDemandValueObjectId);
        _ppPositionActualValueLabel->setObjId(_ppPositionActualValueObjectId);

        _ppPositionRangelLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, axis));
        _ppPositionRangelLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, axis));
        _ppSoftwarePositionLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, axis));
        _ppSoftwarePositionLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, axis));
        _ppHomeOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, axis));
        _ppProfileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_VELOCITY, axis));
        _ppEndVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_END_VELOCITY, axis));
        _ppMaxProfileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, axis));
        _ppMaxMotorSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, axis));
        _ppProfileAccelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_ACCELERATION, axis));
        _ppMaxAccelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_ACCELERATION, axis));
        _ppProfileDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_DECELERATION, axis));
        _ppMaxDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_DECELERATION, axis));
        _ppQuickStopDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_QUICK_STOP_DECELERATION, axis));

        _ppPositionDemandValueLabel->setNode(node);
        _ppPositionActualValueLabel->setNode(node);

        _ppPositionRangelLimitMinSpinBox->setNode(node);
        _ppPositionRangelLimitMaxSpinBox->setNode(node);
        _ppSoftwarePositionLimitMinSpinBox->setNode(node);
        _ppSoftwarePositionLimitMaxSpinBox->setNode(node);
        _ppHomeOffsetSpinBox->setNode(node);
        _ppProfileVelocitySpinBox->setNode(node);
        _ppEndVelocitySpinBox->setNode(node);
        _ppMaxProfileVelocitySpinBox->setNode(node);
        _ppMaxMotorSpeedSpinBox->setNode(node);
        _ppProfileAccelerationSpinBox->setNode(node);
        _ppMaxAccelerationSpinBox->setNode(node);
        _ppProfileDecelerationSpinBox->setNode(node);
        _ppMaxDecelerationSpinBox->setNode(node);
        _ppQuickStopDecelerationSpinBox->setNode(node);

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
            _ppPositionDemandValueLabel->readObject();
            _ppPositionActualValueLabel->readObject();
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

void P402PpWidget::ppTargetPositionLineEditFinished()
{
    _listDataRecord = _ppTargetPositionLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
    _iteratorForSendDataRecord = 0;
    ipSendDataRecord();
}

void P402PpWidget::ipSendDataRecord()
{
    if (_iteratorForSendDataRecord < _listDataRecord.size())
    {
        qint32 value = _listDataRecord.at(_iteratorForSendDataRecord).toInt();
        _node->writeObject(_ppTargetPositionObjectId, QVariant(value));
        _iteratorForSendDataRecord++;
    }
    else
    {
        _ppTargetPositionLineEdit->clear();
        _listDataRecord.clear();
    }
}

void P402PpWidget::ipPolarityEditingFinished()
{
    quint8 value = static_cast<quint8>(_node->nodeOd()->value(_ppPolarityObjectId).toInt());

    if (_ppPolaritySpinBox->value() == 0)
    {
        value = value & 0x7F;
    }
    else
    {
        value = value | 0x80;
    }
    _node->writeObject(_ppPolarityObjectId, QVariant(value));
}

void P402PpWidget::ppNewSetPointClicked(bool ok)
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
    _ppNewSetPointCheckBox->setChecked(ok);
    updateInformationLabel();
}

void P402PpWidget::ppChangeSetImmediatelyPointCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modePp->setChangeSetImmediately(ok);
    }
    updateInformationLabel();
}

void P402PpWidget::changeSetImmediatelyPointEvent(bool ok)
{
    _ppChangeSetImmediatelyPointCheckBox->setChecked(ok);
    updateInformationLabel();
}

void P402PpWidget::ppAbsRelCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modePp->setAbsRel(ok);
    }
}

void P402PpWidget::absRelEvent(bool ok)
{
    _ppAbsRelCheckBox->setChecked(ok);
}

void P402PpWidget::ppChangeOnSetPointCheckBoxRampClicked(bool ok)
{
    if (_nodeProfile402)
    {
        _modePp->setChangeOnSetPoint(ok);
    }
    updateInformationLabel();
}

void P402PpWidget::changeOnSetPointEvent(bool ok)
{
    _ppChangeOnSetPointCheckBox->setChecked(ok);
}

void P402PpWidget::updateInformationLabel()
{
    QString text;
    //    if (!_ppEnableRampCheckBox->isChecked())
    //    {
    //        text = "Enable Interpolation";
    //    }

    //    if (!text.isEmpty())
    //    {
    //        text = "(" + text + "  : Not Activated)";
    //    }

    _ppInfoLabel->setText(text);
}
void P402PpWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_ppPositionDemandValueObjectId);
    _dataLoggerWidget->show();
}

void P402PpWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    NodeObjectId statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);
    controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _ppTargetPositionObjectId};
    _node->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _ppPositionDemandValueObjectId};
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

    _ppTargetPositionLineEdit = new QLineEdit();
    ipLayout->addRow(tr("Position_Target :"), _ppTargetPositionLineEdit);
    connect(_ppTargetPositionLineEdit, &QLineEdit::editingFinished, this, &P402PpWidget::ppTargetPositionLineEditFinished);

    _ppInfoLabel = new QLabel();
    ipLayout->addRow(tr("Information :"), _ppInfoLabel);

    _ppPositionDemandValueLabel = new IndexLabel();
    ipLayout->addRow(tr("Position demand value :"), _ppPositionDemandValueLabel);

    _ppPositionActualValueLabel = new IndexLabel();
    ipLayout->addRow(tr("Position actual value :"), _ppPositionActualValueLabel);

    QLabel *ipPositionRangelLimitLabel = new QLabel(tr("Position range limit :"));
    ipLayout->addRow(ipPositionRangelLimitLabel);
    QLayout *ipPositionRangelLimitlayout = new QHBoxLayout();
    _ppPositionRangelLimitMinSpinBox = new IndexSpinBox();
    _ppPositionRangelLimitMinSpinBox->setToolTip("min");
    ipPositionRangelLimitlayout->addWidget(_ppPositionRangelLimitMinSpinBox);
    _ppPositionRangelLimitMaxSpinBox = new IndexSpinBox();
    _ppPositionRangelLimitMaxSpinBox->setToolTip("max");
    ipPositionRangelLimitlayout->addWidget(_ppPositionRangelLimitMaxSpinBox);
    ipLayout->addRow(ipPositionRangelLimitlayout);

    QLabel *ipSoftwarePositionLimitLabel = new QLabel(tr("Software position limit :"));
    ipLayout->addRow(ipSoftwarePositionLimitLabel);
    QLayout *ipSoftwarePositionLimitlayout = new QHBoxLayout();
    _ppSoftwarePositionLimitMinSpinBox = new IndexSpinBox();
    _ppSoftwarePositionLimitMinSpinBox->setToolTip("min");
    ipSoftwarePositionLimitlayout->addWidget(_ppSoftwarePositionLimitMinSpinBox);
    _ppSoftwarePositionLimitMaxSpinBox = new IndexSpinBox();
    _ppSoftwarePositionLimitMaxSpinBox->setToolTip("max");
    ipSoftwarePositionLimitlayout->addWidget(_ppSoftwarePositionLimitMaxSpinBox);
    ipLayout->addRow(ipSoftwarePositionLimitlayout);

    // Add Home offset (0x607C) and Polarity (0x607E)
    QLayout *ipHomeOffsetlayout = new QVBoxLayout();
    QLabel *ipHomeOffsetLabel = new QLabel(tr("Home offset :"));
    _ppHomeOffsetSpinBox = new IndexSpinBox();
    _ppHomeOffsetSpinBox->setToolTip("");
    ipHomeOffsetlayout->addWidget(ipHomeOffsetLabel);
    ipHomeOffsetlayout->addWidget(_ppHomeOffsetSpinBox);

    QLayout *ipPolaritylayout = new QVBoxLayout();
    QLabel *ipPolarityLabel = new QLabel(tr("Polarity :"));
    _ppPolaritySpinBox = new QSpinBox();
    _ppPolaritySpinBox->setToolTip("0 = x1, 1 = x(-1)");
    _ppPolaritySpinBox->setRange(0, 1);
    ipPolaritylayout->addWidget(ipPolarityLabel);
    ipPolaritylayout->addWidget(_ppPolaritySpinBox);

    QHBoxLayout *ipHomePolaritylayout = new QHBoxLayout();
    ipHomePolaritylayout->addLayout(ipHomeOffsetlayout);
    ipHomePolaritylayout->addLayout(ipPolaritylayout);
    ipLayout->addRow(ipHomePolaritylayout);

    // Add Profile velocity (0x6081) and Max motor speed (0x6080)
    QLayout *ipProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipProfileVelocityLabel = new QLabel(tr("Profile velocity :"));
    _ppProfileVelocitySpinBox = new IndexSpinBox();
    _ppProfileVelocitySpinBox->setToolTip("");
    ipProfileVelocitylayout->addWidget(ipProfileVelocityLabel);
    ipProfileVelocitylayout->addWidget(_ppProfileVelocitySpinBox);

    QLayout *ipEndVelocitylayout = new QVBoxLayout();
    QLabel *ipEndVelocityLabel = new QLabel(tr("End velocity :"));
    _ppEndVelocitySpinBox = new IndexSpinBox();
    _ppEndVelocitySpinBox->setToolTip("");
    ipEndVelocitylayout->addWidget(ipEndVelocityLabel);
    ipEndVelocitylayout->addWidget(_ppEndVelocitySpinBox);

    QHBoxLayout *ipMaxVelocitylayout = new QHBoxLayout();
    ipMaxVelocitylayout->addLayout(ipProfileVelocitylayout);
    ipMaxVelocitylayout->addLayout(ipEndVelocitylayout);
    ipLayout->addRow(ipMaxVelocitylayout);

    // Add Max profile velocity (0x607F) and Max motor speed (0x6080)
    QLayout *ipMaxProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipMaxProfileVelocityLabel = new QLabel(tr("Max profile velocity :"));
    _ppMaxProfileVelocitySpinBox = new IndexSpinBox();
    _ppMaxProfileVelocitySpinBox->setToolTip("");
    ipMaxProfileVelocitylayout->addWidget(ipMaxProfileVelocityLabel);
    ipMaxProfileVelocitylayout->addWidget(_ppMaxProfileVelocitySpinBox);

    QLayout *ipMaxMotorSpeedlayout = new QVBoxLayout();
    QLabel *ipMaxMotorSpeedLabel = new QLabel(tr("Max motor speedy :"));
    _ppMaxMotorSpeedSpinBox = new IndexSpinBox();
    _ppMaxMotorSpeedSpinBox->setToolTip("");
    ipMaxMotorSpeedlayout->addWidget(ipMaxMotorSpeedLabel);
    ipMaxMotorSpeedlayout->addWidget(_ppMaxMotorSpeedSpinBox);

    QHBoxLayout *ipVelocitylayout = new QHBoxLayout();
    ipVelocitylayout->addLayout(ipMaxProfileVelocitylayout);
    ipVelocitylayout->addLayout(ipMaxMotorSpeedlayout);
    ipLayout->addRow(ipVelocitylayout);

    // Add Profile acceleration (0x6083) and Max acceleration (0x60C5)
    QLayout *ipProfileAccelerationlayout = new QVBoxLayout();
    QLabel *ipProfileAccelerationLabel = new QLabel(tr("Profile acceleration :"));
    _ppProfileAccelerationSpinBox = new IndexSpinBox();
    _ppProfileAccelerationSpinBox->setToolTip("");
    ipProfileAccelerationlayout->addWidget(ipProfileAccelerationLabel);
    ipProfileAccelerationlayout->addWidget(_ppProfileAccelerationSpinBox);

    QLayout *ipMaxAccelerationlayout = new QVBoxLayout();
    QLabel *ipMaxAccelerationLabel = new QLabel(tr("Max acceleration :"));
    _ppMaxAccelerationSpinBox = new IndexSpinBox();
    _ppMaxAccelerationSpinBox->setToolTip("");
    ipMaxAccelerationlayout->addWidget(ipMaxAccelerationLabel);
    ipMaxAccelerationlayout->addWidget(_ppMaxAccelerationSpinBox);

    QHBoxLayout *ipProfileAccelerationHlayout = new QHBoxLayout();
    ipProfileAccelerationHlayout->addLayout(ipProfileAccelerationlayout);
    ipProfileAccelerationHlayout->addLayout(ipMaxAccelerationlayout);
    ipLayout->addRow(ipProfileAccelerationHlayout);

    // Add Profile deceleration (0x6084) and Max deceleration (0x60C6)
    QLayout *ipProfileDecelerationlayout = new QVBoxLayout();
    QLabel *ipProfileDecelerationLabel = new QLabel(tr("Profile deceleration :"));
    _ppProfileDecelerationSpinBox = new IndexSpinBox();
    _ppProfileDecelerationSpinBox->setToolTip("");
    ipProfileDecelerationlayout->addWidget(ipProfileDecelerationLabel);
    ipProfileDecelerationlayout->addWidget(_ppProfileDecelerationSpinBox);

    QLayout *ipMaxDecelerationlayout = new QVBoxLayout();
    QLabel *ipMaxDecelerationLabel = new QLabel(tr("Max deceleration :"));
    _ppMaxDecelerationSpinBox = new IndexSpinBox();
    _ppMaxDecelerationSpinBox->setToolTip("");
    ipMaxDecelerationlayout->addWidget(ipMaxDecelerationLabel);
    ipMaxDecelerationlayout->addWidget(_ppMaxDecelerationSpinBox);

    QHBoxLayout *ipProfileDecelerationHlayout = new QHBoxLayout();
    ipProfileDecelerationHlayout->addLayout(ipProfileDecelerationlayout);
    ipProfileDecelerationHlayout->addLayout(ipMaxDecelerationlayout);
    ipLayout->addRow(ipProfileDecelerationHlayout);

    // Add Quick stop deceleration (0x6085)
    QLayout *ipQuickStopDecelerationlayout = new QVBoxLayout();
    QLabel *ipQuickStopDecelerationLabel = new QLabel(tr("Quick stop deceleration :"));
    _ppQuickStopDecelerationSpinBox = new IndexSpinBox();
    _ppQuickStopDecelerationSpinBox->setToolTip("");
    ipQuickStopDecelerationlayout->addWidget(ipQuickStopDecelerationLabel);
    ipQuickStopDecelerationlayout->addWidget(_ppQuickStopDecelerationSpinBox);
    ipLayout->addRow(ipQuickStopDecelerationlayout);

    ipGroupBox->setLayout(ipLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    _ppNewSetPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("New set-point (bit 4) :"), _ppNewSetPointCheckBox);
    connect(_ppNewSetPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::ppNewSetPointClicked);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    _ppChangeSetImmediatelyPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Change set immediately (bit 5) :"), _ppChangeSetImmediatelyPointCheckBox);
    connect(_ppChangeSetImmediatelyPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::ppChangeSetImmediatelyPointCheckBoxRampClicked);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    _ppAbsRelCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Change on set-point (bit 9) :"), _ppAbsRelCheckBox);
    connect(_ppAbsRelCheckBox, &QCheckBox::clicked, this, &P402PpWidget::ppAbsRelCheckBoxRampClicked);

    _ppChangeOnSetPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Abs/Rel (bit 6) :"), _ppChangeOnSetPointCheckBox);
    connect(_ppChangeOnSetPointCheckBox, &QCheckBox::clicked, this, &P402PpWidget::ppChangeOnSetPointCheckBoxRampClicked);
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
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if (objId == _ppPolarityObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            ipPolarityEditingFinished();
        }
    }

    if ((objId == _ppTargetPositionObjectId))
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            if (!_listDataRecord.isEmpty())
            {
                ipSendDataRecord();
            }
        }        
    }
}
