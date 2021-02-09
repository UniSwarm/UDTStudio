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
#include "canopenbus.h"
#include "services/services.h"

#include "canopen/widget/indexspinbox.h"
#include "indexdb402.h"
#include "profile/p402/nodeprofile402.h"
#include "profile/p402/modeip.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QtMath>

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
        if (_nodeProfile402->actualMode() == NodeProfile402::Mode::PP)
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
        registerObjId(_ppPositionDemandValueObjectId);
        registerObjId(_ppPositionActualValueObjectId);
        registerObjId(_ppPolarityObjectId);
        setNodeInterrest(node);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            connect(_nodeProfile402, &NodeProfile402::enableRampEvent, this, &P402PpWidget::newSetPointEvent);
            newSetPointEvent(_nodeProfile402->isEnableRamp());
        }

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
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::Mode::PP)
        {
            setEnabled(true);
            _node->readObject(_ppPositionDemandValueObjectId);
            _node->readObject(_ppPositionActualValueObjectId);
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

void P402PpWidget::ppNewSetPointClicked(int id)
{
    // 0 Disable interpolation
    // 1 Enable interpolation
    if (_nodeProfile402)
    {
        _nodeProfile402->setEnableRamp(id);
    }
    updatePositionDemandLabel();
}

void P402PpWidget::newSetPointEvent(bool ok)
{
    _ppNewSetPointCheckBox->setChecked(ok);
    updatePositionDemandLabel();
}

void P402PpWidget::ppChangeSetImmediatelyPointCheckBoxRampClicked(int id)
{
    if (_nodeProfile402)
    {
        //        _nodeProfile402->setEnableRamp(id);
    }
    updatePositionDemandLabel();
}

void P402PpWidget::changeSetImmediatelyPointEvent(bool ok)
{
    _ppChangeSetImmediatelyPointCheckBox->setChecked(ok);
    updatePositionDemandLabel();
}

void P402PpWidget::ppChangeOnSetPointCheckBoxRampClicked(int id)
{
    if (_nodeProfile402)
    {
        //        _nodeProfile402->setEnableRamp(id);
    }
    updatePositionDemandLabel();
}

void P402PpWidget::changeOnSetPointEvent(bool ok)
{
    _ppChangeOnSetPointCheckBox->setChecked(ok);
    updatePositionDemandLabel();
}

void P402PpWidget::updatePositionDemandLabel()
{
    QString text;
    //    if (!_ppNewSetPointCheckBox->isChecked())
    //    {
    //        text = "Enable Interpolation";
    //    }

    //    if (!text.isEmpty())
    //    {
    //        text = "(" + text + "  : Not Activated)";
    //    }

    int value = _node->nodeOd()->value(_ppPositionDemandValueObjectId).toInt();
    _ppPositionDemandValueLabel->setText(QString("%1 ").arg(QString::number(value, 10)) + text);
}

void P402PpWidget::updatePositionActualLabel()
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

    int value = _node->nodeOd()->value(_ppPositionActualValueObjectId).toInt();
    _ppPositionActualValueLabel->setText(QString("%1 ").arg(QString::number(value, 10)) + text);
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

void P402PpWidget::refreshData(NodeObjectId object)
{
    if (_node->nodeOd()->indexExist(object.index()))
    {
        if (object == _ppPositionDemandValueObjectId)
        {
            updatePositionDemandLabel();
        }
        if (object == _ppPositionActualValueObjectId)
        {
            updatePositionActualLabel();
        }
    }
}

void P402PpWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QString name;
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box IP mode
    QGroupBox *ipGroupBox = new QGroupBox(tr(" Profile position mode"));
    QFormLayout *ipLayout = new QFormLayout();

    _ppTargetPositionLineEdit = new QLineEdit();
    name = tr("Position_Target ") + QString("(0x%1) :").arg(QString::number(_ppTargetPositionObjectId.index(), 16).toUpper());
    ipLayout->addRow(name, _ppTargetPositionLineEdit);
    connect(_ppTargetPositionLineEdit, &QLineEdit::editingFinished, this, &P402PpWidget::ppTargetPositionLineEditFinished);

    _ppPositionDemandValueLabel = new QLabel("-");
    _ppPositionDemandValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Position demand value ") + QString("(0x%1) :").arg(QString::number(_ppPositionDemandValueObjectId.index(), 16).toUpper());
    ipLayout->addRow(name, _ppPositionDemandValueLabel);

    _ppPositionActualValueLabel = new QLabel("-");
    _ppPositionActualValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    name = tr("Position actual value ") + QString("(0x%1) :").arg(QString::number(_ppPositionActualValueObjectId.index(), 16).toUpper());
    ipLayout->addRow(name, _ppPositionActualValueLabel);

    name = tr("Position range limit ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, _axis).index(), 16).toUpper());
    QLabel *ipPositionRangelLimitLabel = new QLabel(name);
    ipLayout->addRow(ipPositionRangelLimitLabel);
    QLayout *ipPositionRangelLimitlayout = new QHBoxLayout();
    _ppPositionRangelLimitMinSpinBox = new IndexSpinBox();
    //    _ppPositionRangelLimitMinSpinBox->setSuffix(" inc");
    _ppPositionRangelLimitMinSpinBox->setToolTip("min");
    ipPositionRangelLimitlayout->addWidget(_ppPositionRangelLimitMinSpinBox);
    _ppPositionRangelLimitMaxSpinBox = new IndexSpinBox();
    //    _ppPositionRangelLimitMaxSpinBox->setSuffix(" inc");
    _ppPositionRangelLimitMaxSpinBox->setToolTip("max");
    ipPositionRangelLimitlayout->addWidget(_ppPositionRangelLimitMaxSpinBox);
    ipLayout->addRow(ipPositionRangelLimitlayout);

    name = tr("Software position limit ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, _axis).index(), 16).toUpper());
    QLabel *ipSoftwarePositionLimitLabel = new QLabel(name);
    ipLayout->addRow(ipSoftwarePositionLimitLabel);
    QLayout *ipSoftwarePositionLimitlayout = new QHBoxLayout();
    _ppSoftwarePositionLimitMinSpinBox = new IndexSpinBox();
    //    _ppSoftwarePositionLimitMinSpinBox->setSuffix(" inc");
    _ppSoftwarePositionLimitMinSpinBox->setToolTip("min");
    //    std::numeric_limits<int>::max());
    ipSoftwarePositionLimitlayout->addWidget(_ppSoftwarePositionLimitMinSpinBox);
    _ppSoftwarePositionLimitMaxSpinBox = new IndexSpinBox();
    //    _ppSoftwarePositionLimitMaxSpinBox->setSuffix(" inc");
    _ppSoftwarePositionLimitMaxSpinBox->setToolTip("max");
    //    std::numeric_limits<int>::max());
    ipSoftwarePositionLimitlayout->addWidget(_ppSoftwarePositionLimitMaxSpinBox);
    ipLayout->addRow(ipSoftwarePositionLimitlayout);

    name = tr("Home offset ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, _axis).index(), 16).toUpper());
    // Add Home offset (0x607C) and Polarity (0x607E)
    QLayout *ipHomeOffsetlayout = new QVBoxLayout();
    QLabel *ipHomeOffsetLabel = new QLabel(name);
    _ppHomeOffsetSpinBox = new IndexSpinBox();
    //    _ppHomeOffsetSpinBox->setSuffix(" inc");
    _ppHomeOffsetSpinBox->setToolTip("");
    ipHomeOffsetlayout->addWidget(ipHomeOffsetLabel);
    ipHomeOffsetlayout->addWidget(_ppHomeOffsetSpinBox);

    name = tr("Polarity ") + QString("(0x%1) ").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_FG_POLARITY, _axis).index(), 16).toUpper()) + "bit 7)";
    QLayout *ipPolaritylayout = new QVBoxLayout();
    QLabel *ipPolarityLabel = new QLabel(name);
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
    QLabel *ipProfileVelocityLabel = new QLabel(tr("Profile velocity (0x6081) :"));
    _ppProfileVelocitySpinBox = new IndexSpinBox();
    //    _ppProfileVelocitySpinBox->setSuffix(" inc/s");
    _ppProfileVelocitySpinBox->setToolTip("");
    //    _ppProfileVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    ipProfileVelocitylayout->addWidget(ipProfileVelocityLabel);
    ipProfileVelocitylayout->addWidget(_ppProfileVelocitySpinBox);

    QLayout *ipEndVelocitylayout = new QVBoxLayout();
    QLabel *ipEndVelocityLabel = new QLabel(tr("End velocity (0x6082) :"));
    _ppEndVelocitySpinBox = new IndexSpinBox();
    //    _ppEndVelocitySpinBox->setSuffix(" inc/s");
    _ppEndVelocitySpinBox->setToolTip("");
    //    _ppEndVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    ipEndVelocitylayout->addWidget(ipEndVelocityLabel);
    ipEndVelocitylayout->addWidget(_ppEndVelocitySpinBox);

    QHBoxLayout *ipMaxVelocitylayout = new QHBoxLayout();
    ipMaxVelocitylayout->addLayout(ipProfileVelocitylayout);
    ipMaxVelocitylayout->addLayout(ipEndVelocitylayout);
    ipLayout->addRow(ipMaxVelocitylayout);

    // Add Max profile velocity (0x607F) and Max motor speed (0x6080)
    QLayout *ipMaxProfileVelocitylayout = new QVBoxLayout();
    name = tr("Max profile velocity ") + QString("(0x%1) :").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, _axis).index(), 16).toUpper());
    QLabel *ipMaxProfileVelocityLabel = new QLabel(name);
    _ppMaxProfileVelocitySpinBox = new IndexSpinBox();
    //    _ppMaxProfileVelocitySpinBox->setSuffix(" inc/period");
    _ppMaxProfileVelocitySpinBox->setToolTip("");
    ipMaxProfileVelocitylayout->addWidget(ipMaxProfileVelocityLabel);
    ipMaxProfileVelocitylayout->addWidget(_ppMaxProfileVelocitySpinBox);

    QLayout *ipMaxMotorSpeedlayout = new QVBoxLayout();
    name = tr("Max motor speedy ") + QString("(0x%1) : ").arg(QString::number(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, _axis).index(), 16).toUpper());
    QLabel *ipMaxMotorSpeedLabel = new QLabel(name);
    _ppMaxMotorSpeedSpinBox = new IndexSpinBox();
    //    _ppMaxMotorSpeedSpinBox->setSuffix(" inc/period");
    _ppMaxMotorSpeedSpinBox->setToolTip("");
    ipMaxMotorSpeedlayout->addWidget(ipMaxMotorSpeedLabel);
    ipMaxMotorSpeedlayout->addWidget(_ppMaxMotorSpeedSpinBox);

    QHBoxLayout *ipVelocitylayout = new QHBoxLayout();
    ipVelocitylayout->addLayout(ipMaxProfileVelocitylayout);
    ipVelocitylayout->addLayout(ipMaxMotorSpeedlayout);
    ipLayout->addRow(ipVelocitylayout);

    // Add Profile acceleration (0x6083) and Max acceleration (0x60C5)
    QLayout *ipProfileAccelerationlayout = new QVBoxLayout();
    QLabel *ipProfileAccelerationLabel = new QLabel(tr("Profile acceleration (0x6083) :"));
    _ppProfileAccelerationSpinBox = new IndexSpinBox();
    //    _ppProfileAccelerationSpinBox->setSuffix(" inc/s2");
    _ppProfileAccelerationSpinBox->setToolTip("");
    //    _ppProfileAccelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipProfileAccelerationlayout->addWidget(ipProfileAccelerationLabel);
    ipProfileAccelerationlayout->addWidget(_ppProfileAccelerationSpinBox);

    QLayout *ipMaxAccelerationlayout = new QVBoxLayout();
    QLabel *ipMaxAccelerationLabel = new QLabel(tr("Max acceleration (0x60C5) :"));
    _ppMaxAccelerationSpinBox = new IndexSpinBox();
    //    _ppMaxAccelerationSpinBox->setSuffix(" inc/s2");
    _ppMaxAccelerationSpinBox->setToolTip("");
    //    _ppMaxAccelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipMaxAccelerationlayout->addWidget(ipMaxAccelerationLabel);
    ipMaxAccelerationlayout->addWidget(_ppMaxAccelerationSpinBox);

    QHBoxLayout *ipProfileAccelerationHlayout = new QHBoxLayout();
    ipProfileAccelerationHlayout->addLayout(ipProfileAccelerationlayout);
    ipProfileAccelerationHlayout->addLayout(ipMaxAccelerationlayout);
    ipLayout->addRow(ipProfileAccelerationHlayout);

    // Add Profile deceleration (0x6084) and Max deceleration (0x60C6)
    QLayout *ipProfileDecelerationlayout = new QVBoxLayout();
    QLabel *ipProfileDecelerationLabel = new QLabel(tr("Profile deceleration (0x6084) :"));
    _ppProfileDecelerationSpinBox = new IndexSpinBox();
    //    _ppProfileDecelerationSpinBox->setSuffix(" inc/s2");
    _ppProfileDecelerationSpinBox->setToolTip("");
    //    _ppProfileDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipProfileDecelerationlayout->addWidget(ipProfileDecelerationLabel);
    ipProfileDecelerationlayout->addWidget(_ppProfileDecelerationSpinBox);

    QLayout *ipMaxDecelerationlayout = new QVBoxLayout();
    QLabel *ipMaxDecelerationLabel = new QLabel(tr("Max deceleration (0x60C6) :"));
    _ppMaxDecelerationSpinBox = new IndexSpinBox();
    //    _ppMaxDecelerationSpinBox->setSuffix(" inc/s2");
    _ppMaxDecelerationSpinBox->setToolTip("");
    //    _ppMaxDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipMaxDecelerationlayout->addWidget(ipMaxDecelerationLabel);
    ipMaxDecelerationlayout->addWidget(_ppMaxDecelerationSpinBox);

    QHBoxLayout *ipProfileDecelerationHlayout = new QHBoxLayout();
    ipProfileDecelerationHlayout->addLayout(ipProfileDecelerationlayout);
    ipProfileDecelerationHlayout->addLayout(ipMaxDecelerationlayout);
    ipLayout->addRow(ipProfileDecelerationHlayout);

    // Add Quick stop deceleration (0x6085)
    QLayout *ipQuickStopDecelerationlayout = new QVBoxLayout();
    QLabel *ipQuickStopDecelerationLabel = new QLabel(tr("Quick stop deceleration (0x6085) :"));
    _ppQuickStopDecelerationSpinBox = new IndexSpinBox();
    //    _ppQuickStopDecelerationSpinBox->setSuffix(" inc/s2");
    _ppQuickStopDecelerationSpinBox->setToolTip("");
    //    _ppQuickStopDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
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

    _ppChangeOnSetPointCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Change on set-point (bit 9) :"), _ppChangeOnSetPointCheckBox);
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

    if ((objId == _ppPositionDemandValueObjectId) || (objId == _ppVelocityActualObjectId) || (objId == _ppPolarityObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId);
    }
    if ((objId == _ppTargetPositionObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (!_listDataRecord.isEmpty())
        {
            ipSendDataRecord();
        }
    }
}
