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

#include "p402ipwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexspinbox.h"
#include "canopen/widget/indexlabel.h"
#include "services/services.h"
#include "canopenbus.h"

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

P402IpWidget::P402IpWidget(QWidget *parent)
    : QScrollArea(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);

    _node = nullptr;
    _nodeProfile402 = nullptr;
}

P402IpWidget::~P402IpWidget()
{
    unRegisterFullOd();
}

Node *P402IpWidget::node() const
{
    return _node;
}

void P402IpWidget::readData()
{
    if (_node)
    {
        if (_nodeProfile402->actualMode() == NodeProfile402::OperationMode::IP)
        {
            _positionDemandValueLabel->readObject();
            _positionAcualValueLabel->readObject();
        }
    }
}

void P402IpWidget::setNode(Node *node, uint8_t axis)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402IpWidget::updateData);
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
        _dataRecordObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_SET_POINT, axis);
        _bufferClearObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_BUFFER_CLEAR, axis);
        _polarityObjectId = IndexDb402::getObjectId(IndexDb402::OD_FG_POLARITY, axis);

        createWidgets();

        _positionDemandValueObjectId.setBusId(_node->busId());
        _positionActualValueObjectId.setNodeId(_node->nodeId());
        _bufferClearObjectId.setBusId(_node->busId());
        _polarityObjectId.setBusId(_node->busId());

        registerObjId(_dataRecordObjectId);
        registerObjId(_polarityObjectId);
        setNodeInterrest(node);

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
            _modeIp = dynamic_cast<ModeIp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::IP));
            connect(_modeIp, &ModeIp::enableRampEvent, this, &P402IpWidget::enableRampEvent);
            enableRampEvent(_modeIp->isEnableRamp());
        }

        connect(&_sendPointSinusoidalTimer, &QTimer::timeout, this, &P402IpWidget::sendDataRecordTargetWithSdo);

        _positionDemandValueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, axis));
        _positionAcualValueLabel->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, axis));

        _timePeriodUnitSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_UNITS, axis));
        _timePeriodIndexSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_INDEX, axis));
        _positionRangelLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, axis));
        _positionRangelLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, axis));
        _softwarePositionLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, axis));
        _softwarePositionLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, axis));
        _homeOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, axis));
        //        _ipProfileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_VELOCITY, axis));
        //        _ipEndVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_END_VELOCITY, axis));
        _maxProfileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, axis));
        _maxMotorSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, axis));
        //        _ipProfileAccelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_ACCELERATION, axis));
        //        _ipMaxAccelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_ACCELERATION, axis));
        //        _ipProfileDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_DECELERATION, axis));
        //        _ipMaxDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_DECELERATION, axis));
        //        _ipQuickStopDecelerationSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_QUICK_STOP_DECELERATION, axis));

        _positionDemandValueLabel->setNode(node);
        _positionAcualValueLabel->setNode(node);

        _timePeriodUnitSpinBox->setNode(node);
        _timePeriodIndexSpinBox->setNode(node);
        _positionRangelLimitMinSpinBox->setNode(node);
        _positionRangelLimitMaxSpinBox->setNode(node);
        _softwarePositionLimitMinSpinBox->setNode(node);
        _softwarePositionLimitMaxSpinBox->setNode(node);
        _homeOffsetSpinBox->setNode(node);
        //    _ipProfileVelocitySpinBox->setNode(node);
        //    _ipEndVelocitySpinBox->setNode(node);
        _maxProfileVelocitySpinBox->setNode(node);
        _maxMotorSpeedSpinBox->setNode(node);
        //    _ipProfileAccelerationSpinBox->setNode(node);
        //    _ipMaxAccelerationSpinBox->setNode(node);
        //    _ipProfileDecelerationSpinBox->setNode(node);
        //    _ipMaxDecelerationSpinBox->setNode(node);
        //    _ipQuickStopDecelerationSpinBox->setNode(node);

        _bus = _node->bus();
        connect(_bus->sync(), &Sync::signalBeforeSync, this, &P402IpWidget::sendDataRecordTargetWithPdo);
    }
}

void P402IpWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED && _nodeProfile402->actualMode() == NodeProfile402::OperationMode::IP)
        {
            setEnabled(true);
            _positionDemandValueLabel->readObject();
            _positionAcualValueLabel->readObject();
        }
        else
        {
            stop();
        }
    }
}

void P402IpWidget::stop()
{
    stopTargetPosition();
}

void P402IpWidget::dataRecordLineEditFinished()
{
    _listDataRecord = _dataRecordLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
    _iteratorForSendDataRecord = 0;
    sendDataRecord();
}

void P402IpWidget::sendDataRecord()
{
    if (_iteratorForSendDataRecord < _listDataRecord.size())
    {
        qint32 value = _listDataRecord.at(_iteratorForSendDataRecord).toInt();
        _node->writeObject(_dataRecordObjectId, QVariant(value));
        _iteratorForSendDataRecord++;
    }
    else
    {
        _dataRecordLineEdit->clear();
        _listDataRecord.clear();
    }
}

void P402IpWidget::polarityEditingFinished()
{
    quint8 value = static_cast<quint8>(_node->nodeOd()->value(_polarityObjectId).toInt());

    if (_polaritySpinBox->value() == 0)
    {
        value = value & 0x7F;
    }
    else
    {
        value = value | 0x80;
    }
    _node->writeObject(_polarityObjectId, QVariant(value));
}

void P402IpWidget::bufferClearClicked()
{
    _modeIp->bufferClear();
}

void P402IpWidget::enableRampClicked(bool ok)
{
    // 0 Disable interpolation
    // 1 Enable interpolation
    if (_nodeProfile402)
    {
        _modeIp->setEnableRamp(ok);
    }
    updatePositionDemandLabel();
}

void P402IpWidget::enableRampEvent(bool ok)
{
    _enableRampCheckBox->setChecked(ok);
    updatePositionDemandLabel();
}

void P402IpWidget::updatePositionDemandLabel()
{
    QString text;
    if (!_enableRampCheckBox->isChecked())
    {
        text = "Enable Interpolation";
    }

    if (!text.isEmpty())
    {
        text = "(" + text + "  : Not Activated)";
    }

    int value = _node->nodeOd()->value(_positionDemandValueObjectId).toInt();
    _positionDemandValueLabel->setText(QString("%1 ").arg(QString::number(value, 10)) + text);
}

// each period * 5 -> we send 5 set-point, for have always value in buffer in device
void P402IpWidget::goTargetPosition()
{
    quint32 unit = 0;
    qint32 index = 0;
    qreal period = 0;

    qint32 initialPosition = _node->nodeOd()->value(_positionDemandValueObjectId).toInt();
    calculatePointSinusoidalMotionProfile(initialPosition);

    quint8 value = 1;
    _node->writeObject(_bufferClearObjectId, QVariant(value));

    unit = static_cast<quint32>(_node->nodeOd()->value(_timePeriodIndexObjectId).toUInt());
    index = static_cast<qint32>(_node->nodeOd()->value(_timePeriodUnitsObjectId).toUInt());

    period = qPow(10, index);
    period = unit * period * 1000;

    if (_bus->sync()->status() == Sync::Status::STOPPED)
    {
        sendDataRecordTargetWithSdo();
        _sendPointSinusoidalTimer.start(static_cast<int>(period) * 5);
    }
    _goTargetPushButton->setEnabled(false);
    _dataRecordLineEdit->setEnabled(false);
}

void P402IpWidget::stopTargetPosition()
{
    _pointSinusoidalVector.clear();
    _sendPointSinusoidalTimer.stop();
    _goTargetPushButton->setEnabled(true);
    _dataRecordLineEdit->setEnabled(true);
}

void P402IpWidget::calculatePointSinusoidalMotionProfile(qint32 initialPosition)
{
    qint32 target = 0;
    quint32 duration = 0;
    quint32 unit = 0;
    qint32 index = 0;
    qreal period = 0;

    if (_relativeTargetpositionSpinBox->isChecked())
    {
        target = _targetPositionSpinBox->value();
    }
    else
    {
        target = _targetPositionSpinBox->value() - initialPosition;
    }

    duration = static_cast<quint32>(_durationSpinBox->value());
    unit = static_cast<quint32>(_node->nodeOd()->value(_timePeriodIndexObjectId).toUInt());
    index = static_cast<qint32>(_node->nodeOd()->value(_timePeriodUnitsObjectId).toUInt());

    period = qPow(10, index);
    period = unit * period * 1000;
    if (period == 0.0)
    {
        return;
    }
    _pointSinusoidalVector.clear();

    int j = 0;
    for (quint32 i = 0; i <= duration; i++)
    {
        qreal pos = ((M_PI * i) / duration) + M_PI;
        pos = (((target / 2) * qCos(pos)) + (target / 2)) + initialPosition;
        if (((i % static_cast<quint32>(period)) == 0) && (i != 0))
        {
            _pointSinusoidalVector.insert(static_cast<int>(j), static_cast<qint32>(pos));
            j++;
        }
    }
}

void P402IpWidget::sendDataRecordTargetWithPdo()
{
    if (_pointSinusoidalVector.isEmpty())
    {
        stopTargetPosition();
        return;
    }
    _node->writeObject(_dataRecordObjectId, QVariant(_pointSinusoidalVector.at(0)));
    _pointSinusoidalVector.remove(0);
}

void P402IpWidget::sendDataRecordTargetWithSdo()
{
    int i = 0;
    if (_pointSinusoidalVector.isEmpty())
    {
        stopTargetPosition();
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (i > (_pointSinusoidalVector.size() - 1))
        {
            break;
        }
        _node->writeObject(_dataRecordObjectId, QVariant(_pointSinusoidalVector.at(i)));
    }
    _pointSinusoidalVector.remove(0, i);
}

void P402IpWidget::readActualBufferSize()
{
    _node->readObject(_bufferClearObjectId);
}

void P402IpWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData(_positionDemandValueObjectId);
    _dataLoggerWidget->show();
}

void P402IpWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    NodeObjectId statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);
    controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _dataRecordObjectId};
    _node->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _positionDemandValueObjectId};
    _node->tpdos().at(2)->writeMapping(ipTpdoObjectList);
}

void P402IpWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box IP mode
    QGroupBox *ipGroupBox = new QGroupBox(tr(" Interpolated position mode"));
    QFormLayout *ipLayout = new QFormLayout();

    _dataRecordLineEdit = new QLineEdit();
    ipLayout->addRow(tr("Data record "), _dataRecordLineEdit);
    _dataRecordLineEdit->setToolTip("Separated by ,");
    connect(_dataRecordLineEdit, &QLineEdit::editingFinished, this, &P402IpWidget::dataRecordLineEditFinished);

    _positionDemandValueLabel = new IndexLabel();
    ipLayout->addRow(tr("Position demand value :"), _positionDemandValueLabel);

    _positionAcualValueLabel = new IndexLabel();
    ipLayout->addRow(tr("Position actual value :"), _positionAcualValueLabel);

    QLabel *ipTimePeriodLabel = new QLabel(tr("Interpolation time period ") + "unit*10^index :");
    ipLayout->addRow(ipTimePeriodLabel);
    QLayout *ipTimePeriodlayout = new QHBoxLayout();
    _timePeriodUnitSpinBox = new IndexSpinBox();
    _timePeriodUnitSpinBox->setToolTip("unit");
    ipTimePeriodlayout->addWidget(_timePeriodUnitSpinBox);
    _timePeriodIndexSpinBox = new IndexSpinBox();
    _timePeriodIndexSpinBox->setToolTip("index");
    _timePeriodIndexSpinBox->setDisplayHint(AbstractIndexWidget::DisplayHint::DisplayDirectValue);
    ipTimePeriodlayout->addWidget(_timePeriodIndexSpinBox);
    ipLayout->addRow(ipTimePeriodlayout);

    _clearBufferPushButton = new QPushButton(tr("Clear Buffer"));
    ipLayout->addRow(_clearBufferPushButton);
    connect(_clearBufferPushButton, &QPushButton::clicked, this, &P402IpWidget::bufferClearClicked);

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

    QHBoxLayout *ipHomePolaritylayout = new QHBoxLayout();
    ipHomePolaritylayout->addLayout(ipHomeOffsetlayout);
    ipHomePolaritylayout->addLayout(ipPolaritylayout);
    ipLayout->addRow(ipHomePolaritylayout);

    //    // Add Profile velocity (0x6081) and Max motor speed (0x6080)
    //    QLayout *ipProfileVelocitylayout = new QVBoxLayout();
    //    QLabel *ipProfileVelocityLabel = new QLabel(tr("Profile velocity (0x6081) :"));
    //    _ipProfileVelocitySpinBox = new IndexSpinBox();
    //    //    _ipProfileVelocitySpinBox->setSuffix(" inc/s");
    //    _ipProfileVelocitySpinBox->setToolTip("");
    //    //    _ipProfileVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipProfileVelocitylayout->addWidget(ipProfileVelocityLabel);
    //    ipProfileVelocitylayout->addWidget(_ipProfileVelocitySpinBox);

    //    QLayout *ipEndVelocitylayout = new QVBoxLayout();
    //    QLabel *ipEndVelocityLabel = new QLabel(tr("End velocity (0x6082) :"));
    //    _ipEndVelocitySpinBox = new IndexSpinBox();
    //    //    _ipEndVelocitySpinBox->setSuffix(" inc/s");
    //    _ipEndVelocitySpinBox->setToolTip("");
    //    //    _ipEndVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipEndVelocitylayout->addWidget(ipEndVelocityLabel);
    //    ipEndVelocitylayout->addWidget(_ipEndVelocitySpinBox);

    //    QHBoxLayout *ipMaxVelocitylayout = new QHBoxLayout();
    //    ipMaxVelocitylayout->addLayout(ipProfileVelocitylayout);
    //    ipMaxVelocitylayout->addLayout(ipEndVelocitylayout);
    //    ipLayout->addRow(ipMaxVelocitylayout);

    // Add Max profile velocity (0x607F) and Max motor speed (0x6080)
    QLayout *ipMaxProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipMaxProfileVelocityLabel = new QLabel(tr("Max profile velocity :"));
    _maxProfileVelocitySpinBox = new IndexSpinBox();
    //    _ipMaxProfileVelocitySpinBox->setSuffix(" inc/period");
    _maxProfileVelocitySpinBox->setToolTip("");
    ipMaxProfileVelocitylayout->addWidget(ipMaxProfileVelocityLabel);
    ipMaxProfileVelocitylayout->addWidget(_maxProfileVelocitySpinBox);

    QLayout *ipMaxMotorSpeedlayout = new QVBoxLayout();
    QLabel *ipMaxMotorSpeedLabel = new QLabel(tr("Max motor speed :"));
    _maxMotorSpeedSpinBox = new IndexSpinBox();
    //    _ipMaxMotorSpeedSpinBox->setSuffix(" inc/period");
    _maxMotorSpeedSpinBox->setToolTip("");
    ipMaxMotorSpeedlayout->addWidget(ipMaxMotorSpeedLabel);
    ipMaxMotorSpeedlayout->addWidget(_maxMotorSpeedSpinBox);

    QHBoxLayout *ipVelocitylayout = new QHBoxLayout();
    ipVelocitylayout->addLayout(ipMaxProfileVelocitylayout);
    ipVelocitylayout->addLayout(ipMaxMotorSpeedlayout);
    ipLayout->addRow(ipVelocitylayout);

    //    // Add Profile acceleration (0x6083) and Max acceleration (0x60C5)
    //    QLayout *ipProfileAccelerationlayout = new QVBoxLayout();
    //    QLabel *ipProfileAccelerationLabel = new QLabel(tr("Profile acceleration (0x6083) :"));
    //    _ipProfileAccelerationSpinBox = new IndexSpinBox();
    //    //    _ipProfileAccelerationSpinBox->setSuffix(" inc/s2");
    //    _ipProfileAccelerationSpinBox->setToolTip("");
    //    //    _ipProfileAccelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipProfileAccelerationlayout->addWidget(ipProfileAccelerationLabel);
    //    ipProfileAccelerationlayout->addWidget(_ipProfileAccelerationSpinBox);

    //    QLayout *ipMaxAccelerationlayout = new QVBoxLayout();
    //    QLabel *ipMaxAccelerationLabel = new QLabel(tr("Max acceleration (0x60C5) :"));
    //    _ipMaxAccelerationSpinBox = new IndexSpinBox();
    //    //    _ipMaxAccelerationSpinBox->setSuffix(" inc/s2");
    //    _ipMaxAccelerationSpinBox->setToolTip("");
    //    //    _ipMaxAccelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipMaxAccelerationlayout->addWidget(ipMaxAccelerationLabel);
    //    ipMaxAccelerationlayout->addWidget(_ipMaxAccelerationSpinBox);

    //    QHBoxLayout *ipProfileAccelerationHlayout = new QHBoxLayout();
    //    ipProfileAccelerationHlayout->addLayout(ipProfileAccelerationlayout);
    //    ipProfileAccelerationHlayout->addLayout(ipMaxAccelerationlayout);
    //    ipLayout->addRow(ipProfileAccelerationHlayout);

    //    // Add Profile deceleration (0x6084) and Max deceleration (0x60C6)
    //    QLayout *ipProfileDecelerationlayout = new QVBoxLayout();
    //    QLabel *ipProfileDecelerationLabel = new QLabel(tr("Profile deceleration (0x6084) :"));
    //    _ipProfileDecelerationSpinBox = new IndexSpinBox();
    //    //    _ipProfileDecelerationSpinBox->setSuffix(" inc/s2");
    //    _ipProfileDecelerationSpinBox->setToolTip("");
    //    //    _ipProfileDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipProfileDecelerationlayout->addWidget(ipProfileDecelerationLabel);
    //    ipProfileDecelerationlayout->addWidget(_ipProfileDecelerationSpinBox);

    //    QLayout *ipMaxDecelerationlayout = new QVBoxLayout();
    //    QLabel *ipMaxDecelerationLabel = new QLabel(tr("Max deceleration (0x60C6) :"));
    //    _ipMaxDecelerationSpinBox = new IndexSpinBox();
    //    //    _ipMaxDecelerationSpinBox->setSuffix(" inc/s2");
    //    _ipMaxDecelerationSpinBox->setToolTip("");
    //    //    _ipMaxDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipMaxDecelerationlayout->addWidget(ipMaxDecelerationLabel);
    //    ipMaxDecelerationlayout->addWidget(_ipMaxDecelerationSpinBox);

    //    QHBoxLayout *ipProfileDecelerationHlayout = new QHBoxLayout();
    //    ipProfileDecelerationHlayout->addLayout(ipProfileDecelerationlayout);
    //    ipProfileDecelerationHlayout->addLayout(ipMaxDecelerationlayout);
    //    ipLayout->addRow(ipProfileDecelerationHlayout);

    //    // Add Quick stop deceleration (0x6085)
    //    QLayout *ipQuickStopDecelerationlayout = new QVBoxLayout();
    //    QLabel *ipQuickStopDecelerationLabel = new QLabel(tr("Quick stop deceleration (0x6085) :"));
    //    _ipQuickStopDecelerationSpinBox = new IndexSpinBox();
    //    //    _ipQuickStopDecelerationSpinBox->setSuffix(" inc/s2");
    //    _ipQuickStopDecelerationSpinBox->setToolTip("");
    //    //    _ipQuickStopDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    //    ipQuickStopDecelerationlayout->addWidget(ipQuickStopDecelerationLabel);
    //    ipQuickStopDecelerationlayout->addWidget(_ipQuickStopDecelerationSpinBox);
    //    ipLayout->addRow(ipQuickStopDecelerationlayout);

    ipGroupBox->setLayout(ipLayout);

    // Group Box GeneratorSinusoidal
    QGroupBox *generatorSinusoidalGroupBox = new QGroupBox(tr("Sinusoidal Motion Profile"));
    QFormLayout *generatorSinusoidalLayout = new QFormLayout();

    _targetPositionSpinBox = new QSpinBox();
    generatorSinusoidalLayout->addRow(tr("Target position :"), _targetPositionSpinBox);
    _targetPositionSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    _relativeTargetpositionSpinBox = new QCheckBox();
    generatorSinusoidalLayout->addRow(tr("Relative target position :"), _relativeTargetpositionSpinBox);

    _durationSpinBox = new QSpinBox();
    generatorSinusoidalLayout->addRow(tr("Duration :"), _durationSpinBox);
    _durationSpinBox->setSuffix(" ms");
    _durationSpinBox->setRange(0, std::numeric_limits<int>::max());

    QHBoxLayout *buttonGoStoplayout = new QHBoxLayout();

    _goTargetPushButton = new QPushButton(tr("GO"));
    connect(_goTargetPushButton, &QPushButton::clicked, this, &P402IpWidget::goTargetPosition);
    _stopTargetPushButton = new QPushButton(tr("STOP"));
    connect(_stopTargetPushButton, &QPushButton::clicked, this, &P402IpWidget::stopTargetPosition);
    buttonGoStoplayout->addWidget(_stopTargetPushButton);
    buttonGoStoplayout->addWidget(_goTargetPushButton);

    generatorSinusoidalLayout->addRow(buttonGoStoplayout);
    generatorSinusoidalGroupBox->setLayout(generatorSinusoidalLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    _enableRampCheckBox = new QCheckBox();
    modeControlWordLayout->addRow(tr("Enable interpolation (bit 4) :"), _enableRampCheckBox);
    connect(_enableRampCheckBox, &QCheckBox::clicked, this, &P402IpWidget::enableRampClicked);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402IpWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402IpWidget::pdoMapping);

    QPixmap ipModePixmap;
    QLabel *ipModeLabel;
    ipModeLabel = new QLabel();
    ipModePixmap.load(":/diagram/img/diagrams/402IPDiagram.png");
    ipModeLabel->setPixmap(ipModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram IP mode"));
    connect(imgPushButton, SIGNAL(clicked()), ipModeLabel, SLOT(show()));
    QHBoxLayout *ipButtonLayout = new QHBoxLayout();
    ipButtonLayout->setSpacing(5);
    ipButtonLayout->addWidget(dataLoggerPushButton);
    ipButtonLayout->addWidget(mappingPdoPushButton);
    ipButtonLayout->addWidget(imgPushButton);

    layout->addWidget(ipGroupBox);
    layout->addWidget(generatorSinusoidalGroupBox);
    layout->addWidget(modeControlWordGroupBox);
    layout->addItem(ipButtonLayout);

    widget->setLayout(layout);
    setWidget(widget);
}

void P402IpWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }
    if (objId == _polarityObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            polarityEditingFinished();
        }
    }
}
