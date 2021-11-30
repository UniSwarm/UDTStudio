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
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "canopenbus.h"
#include "services/services.h"

#include "profile/p402/modeip.h"
#include "profile/p402/nodeprofile402.h"

#include <QString>
#include <QStringList>
#include <QtMath>

#include <canopen/indexWidget/indexcheckbox.h>

P402IpWidget::P402IpWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _nodeProfile402 = nullptr;
    createWidgets();
}

P402IpWidget::~P402IpWidget()
{
    unRegisterFullOd();
}

void P402IpWidget::readRealTimeObjects()
{
    if (_nodeProfile402)
    {
        _modeIp->readRealTimeObjects();
    }
}

void P402IpWidget::readAllObjects()
{
    if (_nodeProfile402)
    {
        _modeIp->readAllObjects();
    }
}

void P402IpWidget::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }

    if (node)
    {
        setNodeInterrest(node);

        if (!node->profiles().isEmpty())
        {
            _nodeProfile402 = dynamic_cast<NodeProfile402 *>(node->profiles()[axis]);
            _modeIp = dynamic_cast<ModeIp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::IP));
            connect(_modeIp, &ModeIp::enableRampEvent, this, &P402IpWidget::enableRampEvent);
            enableRampEvent(_modeIp->isEnableRamp());

            _positionTargetObjectId = _modeIp->targetObjectId();
            registerObjId(_positionTargetObjectId);
            _positionDemandValueObjectId = _modeIp->positionDemandValueObjectId();
            _positionDemandValueLabel->setObjId(_positionDemandValueObjectId);

            _positionActualValueObjectId = _modeIp->positionActualValueObjectId();
            _positionAcualValueLabel->setObjId(_positionActualValueObjectId);

            _timePeriodIndexObjectId = _modeIp->timePeriodIndexObjectId();
            _timePeriodIndexSpinBox->setObjId(_timePeriodIndexObjectId);

            _timePeriodUnitsObjectId = _modeIp->timePeriodUnitsObjectId();
            _timePeriodUnitSpinBox->setObjId(_timePeriodUnitsObjectId);

            _positionRangeLimitMinSpinBox->setObjId(_modeIp->positionRangeLimitMinObjectId());
            _positionRangeLimitMaxSpinBox->setObjId(_modeIp->positionRangeLimitMaxObjectId());
            _softwarePositionLimitMinSpinBox->setObjId(_modeIp->softwarePositionLimitMinObjectId());
            _softwarePositionLimitMaxSpinBox->setObjId(_modeIp->softwarePositionLimitMaxObjectId());

            _maxProfileVelocitySpinBox->setObjId(_modeIp->maxProfileVelocityObjectId());
            _maxMotorSpeedSpinBox->setObjId(_modeIp->maxMotorSpeedObjectId());

            _homeOffsetSpinBox->setObjId(_modeIp->homeOffsetObjectId());

            _polarityCheckBox->setObjId(_nodeProfile402->fgPolaritybjectId());
        }

        connect(&_sendPointSinusoidalTimer, &QTimer::timeout, this, &P402IpWidget::sendDataRecordTargetWithSdo);
        connect(_nodeProfile402->node()->bus()->sync(), &Sync::signalBeforeSync, this, &P402IpWidget::sendDataRecordTargetWithPdo);
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
        _nodeProfile402->node()->writeObject(_positionTargetObjectId, QVariant(value));
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

    qint32 initialPosition = _nodeProfile402->node()->nodeOd()->value(_positionDemandValueObjectId).toInt();

    unit = static_cast<quint8>(_nodeProfile402->node()->nodeOd()->value(_timePeriodUnitsObjectId).toUInt());
    index = static_cast<qint8>(_nodeProfile402->node()->nodeOd()->value(_timePeriodIndexObjectId).toUInt());
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

    if (_nodeProfile402->node()->bus()->sync()->status() == Sync::Status::STOPPED)
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
    _nodeProfile402->node()->writeObject(_positionTargetObjectId, QVariant(_pointSinusoidalVector.at(0)));
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
        _nodeProfile402->node()->writeObject(_positionTargetObjectId, QVariant(_pointSinusoidalVector.at(i)));
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

void P402IpWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    _dataLoggerWidget->setTitle(tr("Node %1 axis %2 IP").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axisId()));

    dataLogger->addData(_positionDemandValueObjectId);
    dataLogger->addData(_positionActualValueObjectId);

    _dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(_dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    _dataLoggerWidget->show();
    _dataLoggerWidget->raise();
    _dataLoggerWidget->activateWindow();
}

void P402IpWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _positionTargetObjectId};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _positionDemandValueObjectId};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(ipTpdoObjectList);
}

void P402IpWidget::createWidgets()
{
    // Group Box IP mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Interpolated position mode"));
    _modeLayout = new QFormLayout();

    targetWidgets();
    informationWidgets();
    limitWidgets();

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    _modeLayout->addRow(frame);

    slopeWidgets();

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
    layout->addWidget(sinusoidalMotionProfileWidgets());
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

void P402IpWidget::targetWidgets()
{
    QLayout *dataRecordlayout = new QHBoxLayout();
    _dataRecordLineEdit = new QLineEdit();
    _dataRecordLineEdit->setToolTip("Separated by ,");
    connect(_dataRecordLineEdit, &QLineEdit::editingFinished, this, &P402IpWidget::dataRecordLineEditFinished);

    _clearBufferPushButton = new QPushButton(tr("Clear Fifo"));
    connect(_clearBufferPushButton, &QPushButton::clicked, this, &P402IpWidget::bufferClearClicked);

    dataRecordlayout->addWidget(_dataRecordLineEdit);
    dataRecordlayout->addWidget(_clearBufferPushButton);
    _modeLayout->addRow(tr("Data record "), dataRecordlayout);
}

void P402IpWidget::informationWidgets()
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    _modeLayout->addRow(tr("Information:"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position demand value:"), _positionDemandValueLabel);

    _positionAcualValueLabel = new IndexLabel();
    _modeLayout->addRow(tr("Position actual value:"), _positionAcualValueLabel);
}

void P402IpWidget::limitWidgets()
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

    // Max profile velocity (0x607F)
    _maxProfileVelocitySpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max profile velocity:"), _maxProfileVelocitySpinBox);

    // Max motor speed (0x6080)
    _maxMotorSpeedSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Max motor speed:"), _maxMotorSpeedSpinBox);
}

void P402IpWidget::slopeWidgets()
{
    QLayout *ipTimePeriodlayout = new QHBoxLayout();
    ipTimePeriodlayout->setSpacing(0);

    _timePeriodUnitSpinBox = new IndexSpinBox();
    ipTimePeriodlayout->addWidget(_timePeriodUnitSpinBox);

    QLabel *label = new QLabel(tr("unit.10<sup>index</sup>"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ipTimePeriodlayout->addWidget(label);

    _timePeriodIndexSpinBox = new IndexSpinBox();
    _timePeriodIndexSpinBox->setDisplayHint(AbstractIndexWidget::DisplayHint::DisplayDirectValue);
    ipTimePeriodlayout->addWidget(_timePeriodIndexSpinBox);

    label = new QLabel(tr("&Interpolation time period:"));
    label->setToolTip(tr("unit, index (unit*10^index)"));
    label->setBuddy(_timePeriodUnitSpinBox);

    _modeLayout->addRow(label, ipTimePeriodlayout);
}

void P402IpWidget::homePolarityWidgets()
{
    // Home offset (0x607C)
    _homeOffsetSpinBox = new IndexSpinBox();
    _modeLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    // Polarity (0x607E)
    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    _modeLayout->addRow(tr("Polarity:"), _polarityCheckBox);
}

QGroupBox *P402IpWidget::sinusoidalMotionProfileWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sinusoidal motion Profile"));
    QFormLayout *layout = new QFormLayout();

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
    layout->addRow(tr("Target position:"), buttonGoStoplayout);

    _relativeTargetpositionSpinBox = new QCheckBox();
    layout->addRow(tr("Relative target position:"), _relativeTargetpositionSpinBox);

    _durationSpinBox = new QSpinBox();
    layout->addRow(tr("Duration:"), _durationSpinBox);
    _durationSpinBox->setSuffix(" ms");
    _durationSpinBox->setRange(0, std::numeric_limits<int>::max());

    groupBox->setLayout(layout);

    return groupBox;
}

QGroupBox *P402IpWidget::controlWordWidgets()
{
    // Group Box Control Word
    QGroupBox *groupBox = new QGroupBox(tr("Control word"));
    QFormLayout *layout = new QFormLayout();

    _enableRampCheckBox = new QCheckBox();
    layout->addRow(tr("Enable interpolation (bit 4):"), _enableRampCheckBox);
    connect(_enableRampCheckBox, &QCheckBox::clicked, this, &P402IpWidget::enableRampClicked);
    groupBox->setLayout(layout);

    return groupBox;
}

QHBoxLayout *P402IpWidget::buttonWidgets()
{
    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402IpWidget::createDataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Map IP to PDOs"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402IpWidget::mapDefaultObjects);

    QPixmap ipModePixmap;
    QLabel *ipModeLabel;
    ipModeLabel = new QLabel();
    ipModePixmap.load(":/diagram/img/diagrams/402IPDiagram.png");
    ipModeLabel->setPixmap(ipModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram IP mode"));
    connect(imgPushButton, SIGNAL(clicked()), ipModeLabel, SLOT(show()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 0, 2, 0);
    layout->setSpacing(5);
    layout->addWidget(dataLoggerPushButton);
    layout->addWidget(mappingPdoPushButton);
    layout->addWidget(imgPushButton);

    return layout;
}

void P402IpWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}
