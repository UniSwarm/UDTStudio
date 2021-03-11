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

void P402IpWidget::readAllObject()
{
    if (_node)
    {
        _node->readObject(_positionDemandValueObjectId);
        _node->readObject(_positionActualValueObjectId);

        _positionRangelLimitMinSpinBox->readObject();
        _positionRangelLimitMaxSpinBox->readObject();
        _softwarePositionLimitMinSpinBox->readObject();
        _softwarePositionLimitMaxSpinBox->readObject();
        _maxProfileVelocitySpinBox->readObject();
        _maxMotorSpeedSpinBox->readObject();
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
        _dataRecordObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_DATA_RECORD_SET_POINT, axis);
        _timePeriodIndexObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_INDEX, axis);
        _timePeriodUnitsObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_UNITS, axis);

        createWidgets();

        _positionDemandValueObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
        _positionActualValueObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

        registerObjId(_dataRecordObjectId);
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

        _timePeriodUnitSpinBox->setObjId(_timePeriodUnitsObjectId);
        _timePeriodIndexSpinBox->setObjId(_timePeriodIndexObjectId);
        _positionRangelLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, axis));
        _positionRangelLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, axis));
        _softwarePositionLimitMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, axis));
        _softwarePositionLimitMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, axis));
        _homeOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, axis));
        _maxProfileVelocitySpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, axis));
        _maxMotorSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, axis));

        _positionDemandValueLabel->setNode(node);
        _positionAcualValueLabel->setNode(node);

        _timePeriodUnitSpinBox->setNode(node);
        _timePeriodIndexSpinBox->setNode(node);
        _positionRangelLimitMinSpinBox->setNode(node);
        _positionRangelLimitMaxSpinBox->setNode(node);
        _softwarePositionLimitMinSpinBox->setNode(node);
        _softwarePositionLimitMaxSpinBox->setNode(node);
        _homeOffsetSpinBox->setNode(node);
        _maxProfileVelocitySpinBox->setNode(node);
        _maxMotorSpeedSpinBox->setNode(node);

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
    while (!_listDataRecord.isEmpty())
    {
        qint32 value = _listDataRecord.first().toInt();
        _node->writeObject(_dataRecordObjectId, QVariant(value));
        _listDataRecord.removeFirst();
    }

    _dataRecordLineEdit->clear();
    _listDataRecord.clear();
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
}

void P402IpWidget::enableRampEvent(bool ok)
{
    _enableRampCheckBox->setChecked(ok);
    updateInformationLabel();
}

// each period * 5 -> we send 5 set-point, for have always value in buffer in device
void P402IpWidget::startTargetPosition()
{
    quint8 unit = 0;
    qint8 index = 0;
    qreal period = 0;
    qint32 target = 0;

    qint32 initialPosition = _node->nodeOd()->value(_positionDemandValueObjectId).toInt();

    unit = static_cast<quint8>(_node->nodeOd()->value(_timePeriodUnitsObjectId).toUInt());
    index = static_cast<qint8>(_node->nodeOd()->value(_timePeriodIndexObjectId).toUInt());
    period = qPow(10, index);
    period = unit * period * 1000;

    if (_relativeTargetpositionSpinBox->isChecked())
    {
        target = _targetPositionSpinBox->value();
    }
    else
    {
        target = _targetPositionSpinBox->value() - initialPosition;
    }

    calculatePointSinusoidalMotionProfile(target, initialPosition, period);

    bufferClearClicked();

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

void P402IpWidget::calculatePointSinusoidalMotionProfile(qint32 targetPosition, qint32 initialPosition, qreal periodMs)
{
    quint32 duration = 0;

    duration = static_cast<quint32>(_durationSpinBox->value());

    if (periodMs == 0.0)
    {
        return;
    }
    _pointSinusoidalVector.clear();

    int j = 0;
    for (quint32 i = 0; i <= duration; i++)
    {
        qreal pos = ((M_PI * i) / duration) + M_PI;
        pos = (((targetPosition / 2) * qCos(pos)) + (targetPosition / 2)) + initialPosition;
        if (((i % static_cast<quint32>(periodMs)) == 0) && (i != 0))
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

void P402IpWidget::updateInformationLabel()
{
    QString text;
    if (!_enableRampCheckBox->isChecked())
    {
        text = "Interpolation is disabled";
    }
    _infoLabel->setText(text);
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
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box IP mode
    QGroupBox *ipGroupBox = new QGroupBox(tr(" Interpolated position mode"));
    QFormLayout *qfLayout = new QFormLayout();

    QLayout *dataRecordlayout = new QHBoxLayout();
    _dataRecordLineEdit = new QLineEdit();
    _dataRecordLineEdit->setToolTip("Separated by ,");
    connect(_dataRecordLineEdit, &QLineEdit::editingFinished, this, &P402IpWidget::dataRecordLineEditFinished);

    _clearBufferPushButton = new QPushButton(tr("Clear Fifo"));
    connect(_clearBufferPushButton, &QPushButton::clicked, this, &P402IpWidget::bufferClearClicked);

    dataRecordlayout->addWidget(_dataRecordLineEdit);
    dataRecordlayout->addWidget(_clearBufferPushButton);
    qfLayout->addRow(tr("Data record "), dataRecordlayout);

    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    qfLayout->addRow(tr("Information :"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    qfLayout->addRow(tr("Position demand value :"), _positionDemandValueLabel);

    _positionAcualValueLabel = new IndexLabel();
    qfLayout->addRow(tr("Position actual value :"), _positionAcualValueLabel);

    QLayout *positionRangelLimitlayout = new QHBoxLayout();
    positionRangelLimitlayout->setSpacing(0);
    _positionRangelLimitMinSpinBox = new IndexSpinBox();
    positionRangelLimitlayout->addWidget(_positionRangelLimitMinSpinBox);
    QLabel *label = new QLabel(tr(":"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    positionRangelLimitlayout->addWidget(label);
    _positionRangelLimitMaxSpinBox = new IndexSpinBox();
    positionRangelLimitlayout->addWidget(_positionRangelLimitMaxSpinBox);
    label = new QLabel(tr("&Position range limit :"));
    label->setToolTip(tr("Min, Max"));
    label->setBuddy(_positionRangelLimitMinSpinBox);
    qfLayout->addRow(label, positionRangelLimitlayout);

    QLayout *softwarePositionLimitlayout = new QHBoxLayout();
    softwarePositionLimitlayout->setSpacing(0);
    _softwarePositionLimitMinSpinBox = new IndexSpinBox();
    softwarePositionLimitlayout->addWidget(_softwarePositionLimitMinSpinBox);
    label = new QLabel(tr(":"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    softwarePositionLimitlayout->addWidget(label);
    _softwarePositionLimitMaxSpinBox = new IndexSpinBox();
    softwarePositionLimitlayout->addWidget(_softwarePositionLimitMaxSpinBox);
    label = new QLabel(tr("&Software position limit :"));
    label->setToolTip(tr("Min, Max"));
    label->setBuddy(_softwarePositionLimitMinSpinBox);
    qfLayout->addRow(label, softwarePositionLimitlayout);

    // Add Max profile velocity (0x607F)
    _maxProfileVelocitySpinBox = new IndexSpinBox();
    qfLayout->addRow(tr("Max profile velocity :"), _maxProfileVelocitySpinBox);

    // Add Max motor speed (0x6080)
    _maxMotorSpeedSpinBox = new IndexSpinBox();
    qfLayout->addRow(tr("Max motor speed :"), _maxMotorSpeedSpinBox);

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    qfLayout->addRow(frame);

    QLayout *ipTimePeriodlayout = new QHBoxLayout();
    ipTimePeriodlayout->setSpacing(0);
    _timePeriodUnitSpinBox = new IndexSpinBox();
    ipTimePeriodlayout->addWidget(_timePeriodUnitSpinBox);
    label = new QLabel(tr("unit.10<sup>index</sup>"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ipTimePeriodlayout->addWidget(label);
    _timePeriodIndexSpinBox = new IndexSpinBox();
    _timePeriodIndexSpinBox->setDisplayHint(AbstractIndexWidget::DisplayHint::DisplayDirectValue);
    ipTimePeriodlayout->addWidget(_timePeriodIndexSpinBox);
    label = new QLabel(tr("&Interpolation time period :"));
    label->setToolTip(tr("unit, index (unit*10^index)"));
    label->setBuddy(_timePeriodUnitSpinBox);
    qfLayout->addRow(label, ipTimePeriodlayout);

    frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    qfLayout->addRow(frame);

    // Add Home offset (0x607C)
    _homeOffsetSpinBox = new IndexSpinBox();
    qfLayout->addRow(tr("Home offset :"), _homeOffsetSpinBox);

    // Polarity (0x607E)
    _polaritySpinBox = new QSpinBox();
    _polaritySpinBox->setToolTip("0 = x1, 1 = x(-1)");
    _polaritySpinBox->setRange(0, 1);
    qfLayout->addRow(tr("Polarity :"), _polaritySpinBox);
    connect(_polaritySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { _nodeProfile402->setPolarityPosition(i); });

    ipGroupBox->setLayout(qfLayout);

    // Group Box GeneratorSinusoidal
    QGroupBox *generatorSinusoidalGroupBox = new QGroupBox(tr("Sinusoidal Motion Profile"));
    QFormLayout *generatorSinusoidalLayout = new QFormLayout();

    QHBoxLayout *buttonGoStoplayout = new QHBoxLayout();

    _targetPositionSpinBox = new QSpinBox();
    _targetPositionSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    buttonGoStoplayout->addWidget(_targetPositionSpinBox);

    _stopTargetPushButton = new QPushButton(tr("Stop"));
    _stopTargetPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(_stopTargetPushButton, &QPushButton::clicked, this, &P402IpWidget::stopTargetPosition);
    buttonGoStoplayout->addWidget(_stopTargetPushButton);

    _goTargetPushButton = new QPushButton(tr("Start"));
    _goTargetPushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(_goTargetPushButton, &QPushButton::clicked, this, &P402IpWidget::startTargetPosition);
    buttonGoStoplayout->addWidget(_goTargetPushButton);

    buttonGoStoplayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));
    generatorSinusoidalLayout->addRow(tr("Target position :"), buttonGoStoplayout);

    _relativeTargetpositionSpinBox = new QCheckBox();
    generatorSinusoidalLayout->addRow(tr("Relative target position :"), _relativeTargetpositionSpinBox);

    _durationSpinBox = new QSpinBox();
    generatorSinusoidalLayout->addRow(tr("Duration :"), _durationSpinBox);
    _durationSpinBox->setSuffix(" ms");
    _durationSpinBox->setRange(0, std::numeric_limits<int>::max());

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
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }
}
