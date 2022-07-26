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
#include "canopen/indexWidget/indexformlayout.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "canopenbus.h"
#include "profile/p402/modeip.h"
#include "profile/p402/nodeprofile402.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

#include <QString>
#include <QStringList>
#include <QtMath>

P402IpWidget::P402IpWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    _modeIp = nullptr;

    createWidgets();
    createActions();
}

void P402IpWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        _modeIp = nullptr;
        return;
    }
    _modeIp = dynamic_cast<ModeIp *>(_nodeProfile402->mode(NodeProfile402::OperationMode::IP));
    connect(_modeIp, &ModeIp::enableRampEvent, this, &P402IpWidget::enableRampEvent);
    enableRampEvent(_modeIp->isEnableRamp());

    _positionDemandValueLabel->setObjId(_modeIp->positionDemandValueObjectId());
    _positionAcualValueLabel->setObjId(_modeIp->positionActualValueObjectId());

    _timePeriodIndexSpinBox->setObjId(_modeIp->timePeriodIndexObjectId());
    _timePeriodUnitSpinBox->setObjId(_modeIp->timePeriodUnitsObjectId());

    _positionRangeLimitMinSpinBox->setObjId(_modeIp->positionRangeLimitMinObjectId());
    _positionRangeLimitMaxSpinBox->setObjId(_modeIp->positionRangeLimitMaxObjectId());
    _softwarePositionLimitMinSpinBox->setObjId(_modeIp->softwarePositionLimitMinObjectId());
    _softwarePositionLimitMaxSpinBox->setObjId(_modeIp->softwarePositionLimitMaxObjectId());

    _maxProfileVelocitySpinBox->setObjId(_modeIp->maxProfileVelocityObjectId());
    _maxMotorSpeedSpinBox->setObjId(_modeIp->maxMotorSpeedObjectId());

    _homeOffsetSpinBox->setObjId(_modeIp->homeOffsetObjectId());

    _polarityCheckBox->setObjId(_nodeProfile402->fgPolaritybjectId());

    connect(&_sendPointSinusoidalTimer, &QTimer::timeout, this, &P402IpWidget::sendDataRecordTargetWithSdo);
    connect(_nodeProfile402->node()->bus()->sync(), &Sync::signalBeforeSync, this, &P402IpWidget::sendDataRecordTargetWithPdo);
}

void P402IpWidget::stop()
{
    stopTargetPosition();
}

void P402IpWidget::dataRecordLineEditFinished()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    _listDataRecord = _dataRecordLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
#else
    _listDataRecord = _dataRecordLineEdit->text().split(QLatin1Char(','), Qt::SkipEmptyParts);
#endif
    _iteratorForSendDataRecord = 0;
    sendDataRecord();
}

void P402IpWidget::sendDataRecord()
{
    while (!_listDataRecord.isEmpty())
    {
        qint32 value = _listDataRecord.first().toInt();
        _nodeProfile402->node()->writeObject(_modeIp->targetObjectId(), QVariant(value));
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
    if (_nodeProfile402 != nullptr)
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

    qint32 initialPosition = _nodeProfile402->node()->nodeOd()->value(_modeIp->positionDemandValueObjectId()).toInt();

    unit = static_cast<quint8>(_nodeProfile402->node()->nodeOd()->value(_modeIp->timePeriodUnitsObjectId()).toUInt());
    index = static_cast<qint8>(_nodeProfile402->node()->nodeOd()->value(_modeIp->timePeriodIndexObjectId()).toUInt());
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
    _nodeProfile402->node()->writeObject(_modeIp->targetObjectId(), QVariant(_pointSinusoidalVector.at(0)));
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
        _nodeProfile402->node()->writeObject(_modeIp->targetObjectId(), QVariant(_pointSinusoidalVector.at(i)));
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

void P402IpWidget::createActions()
{
    createDefaultActions();
}

void P402IpWidget::createDataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->setTitle(tr("Node %1 axis %2 IP").arg(_nodeProfile402->nodeId()).arg(_nodeProfile402->axis()));

    dataLogger->addData(_modeIp->positionDemandValueObjectId());
    dataLogger->addData(_modeIp->positionActualValueObjectId());

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();
}

void P402IpWidget::mapDefaultObjects()
{
    NodeObjectId controlWordObjectId = _nodeProfile402->controlWordObjectId();
    QList<NodeObjectId> ipRpdoObjectList = {controlWordObjectId, _modeIp->targetObjectId()};
    _nodeProfile402->node()->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    NodeObjectId statusWordObjectId = _nodeProfile402->statusWordObjectId();
    QList<NodeObjectId> ipTpdoObjectList = {statusWordObjectId, _modeIp->positionDemandValueObjectId()};
    _nodeProfile402->node()->tpdos().at(0)->writeMapping(ipTpdoObjectList);
}

void P402IpWidget::showDiagram()
{
    QPixmap ipModePixmap;
    QLabel *ipModeLabel;
    ipModeLabel = new QLabel();
    ipModeLabel->setAttribute(Qt::WA_DeleteOnClose);
    ipModePixmap.load(":/diagram/img/diagrams/402IPDiagram.png");
    ipModeLabel->setPixmap(ipModePixmap);
    ipModeLabel->setWindowTitle("402 IP Diagram");
    ipModeLabel->show();
}

void P402IpWidget::createWidgets()
{
    // Group Box IP mode
    QGroupBox *modeGroupBox = new QGroupBox(tr("Interpolated position mode"));
    IndexFormLayout *indexLayout = new IndexFormLayout();

    createTargetWidgets(indexLayout);
    createInformationWidgets(indexLayout);
    createLimitWidgets(indexLayout);

    indexLayout->addLineSeparator();

    createSlopeWidgets(indexLayout);

    indexLayout->addLineSeparator();

    createHomePolarityWidgets(indexLayout);

    modeGroupBox->setLayout(indexLayout);

    // Create interface
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(modeGroupBox);
    layout->addWidget(createSinusoidalMotionProfileWidgets());
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

void P402IpWidget::createTargetWidgets(IndexFormLayout *indexLayout)
{
    QLayout *dataRecordlayout = new QHBoxLayout();
    _dataRecordLineEdit = new QLineEdit();
    _dataRecordLineEdit->setToolTip("Separated by ,");
    connect(_dataRecordLineEdit, &QLineEdit::editingFinished, this, &P402IpWidget::dataRecordLineEditFinished);

    _clearBufferPushButton = new QPushButton(tr("Clear Fifo"));
    connect(_clearBufferPushButton, &QPushButton::clicked, this, &P402IpWidget::bufferClearClicked);

    dataRecordlayout->addWidget(_dataRecordLineEdit);
    dataRecordlayout->addWidget(_clearBufferPushButton);
    indexLayout->addRow(tr("Data record "), dataRecordlayout);
}

void P402IpWidget::createInformationWidgets(IndexFormLayout *indexLayout)
{
    _infoLabel = new QLabel();
    _infoLabel->setStyleSheet("QLabel { color : red; }");
    indexLayout->addRow(tr("Information:"), _infoLabel);

    _positionDemandValueLabel = new IndexLabel();
    indexLayout->addRow(tr("Position demand value:"), _positionDemandValueLabel);

    _positionAcualValueLabel = new IndexLabel();
    indexLayout->addRow(tr("Position actual value:"), _positionAcualValueLabel);
}

void P402IpWidget::createLimitWidgets(IndexFormLayout *indexLayout)
{
    // POSITION RANGE LIMIT
    _positionRangeLimitMinSpinBox = new IndexSpinBox();
    _positionRangeLimitMaxSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("&Position range limits:"), _positionRangeLimitMinSpinBox, _positionRangeLimitMaxSpinBox, tr("-"));

    // SOFTWARE RANGE LIMIT
    _softwarePositionLimitMinSpinBox = new IndexSpinBox();
    _softwarePositionLimitMaxSpinBox = new IndexSpinBox();
    indexLayout->addDualRow(tr("Software position limits:"), _softwarePositionLimitMinSpinBox, _softwarePositionLimitMaxSpinBox, tr("-"));

    // Max profile velocity (0x607F)
    _maxProfileVelocitySpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Max profile velocity:"), _maxProfileVelocitySpinBox);

    // Max motor speed (0x6080)
    _maxMotorSpeedSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Max motor speed:"), _maxMotorSpeedSpinBox);
}

void P402IpWidget::createSlopeWidgets(IndexFormLayout *indexLayout)
{
    _timePeriodUnitSpinBox = new IndexSpinBox();
    _timePeriodIndexSpinBox = new IndexSpinBox();
    _timePeriodIndexSpinBox->setDisplayHint(AbstractIndexWidget::DisplayHint::DisplayDirectValue);
    indexLayout->addDualRow(tr("&Interpolation time period:"), _timePeriodUnitSpinBox, _timePeriodIndexSpinBox, tr("unit.10<sup>index</sup>"));
}

void P402IpWidget::createHomePolarityWidgets(IndexFormLayout *indexLayout)
{
    // Home offset (0x607C)
    _homeOffsetSpinBox = new IndexSpinBox();
    indexLayout->addRow(tr("Home offset:"), _homeOffsetSpinBox);

    // Polarity (0x607E)
    _polarityCheckBox = new IndexCheckBox();
    _polarityCheckBox->setBitMask(NodeProfile402::FgPolarity::MASK_POLARITY_POSITION);
    indexLayout->addRow(tr("Polarity:"), _polarityCheckBox);
}

QGroupBox *P402IpWidget::createSinusoidalMotionProfileWidgets()
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

QGroupBox *P402IpWidget::createControlWordWidgets()
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

void P402IpWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}
