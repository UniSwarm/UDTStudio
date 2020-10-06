/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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
#include "services/services.h"

#include "profile/p402/nodeprofile402.h"
#include "profile/p402/nodeprofile402ip.h"

#include "canopenbus.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QStringList>
#include <QtMath>
#include <QDebug>

P402IpWidget::P402IpWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    createWidgets();

    _ipPositionDemandValueObjectId = 0x6062;

    _ipDataRecordObjectId = 0x60C1;
    _ipDataConfigurationObjectId = 0x60C4;

    _ipTimePeriodObjectId = 0x60C2;
    _ipPositionRangelLimitObjectId = 0x607B;
    _ipSoftwarePositionLimitObjectId = 0x607D;
    _ipHomeOffsetObjectId = 0x607C;
    _ipPolarityObjectId = 0x607E;

    _ipProfileVelocityObjectId = 0x6081;
    _ipEndVelocityObjectId = 0x6082;

    _ipMaxProfileVelocityObjectId = 0x607F;
    _ipMaxMotorSpeedObjectId = 0x6080;

    _ipProfileAccelerationObjectId = 0x6083;
    _ipMaxAccelerationObjectId = 0x60C5;

    _ipProfileDecelerationObjectId = 0x6084;
    _ipMaxDecelerationObjectId = 0x60C6;

    _ipQuickStopDecelerationObjectId = 0x6085;
}

P402IpWidget::~P402IpWidget()
{
    unRegisterFullOd();
}

void P402IpWidget::setNode(Node *node)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402IpWidget::updateData);
        }
    }

    _node = node;
    if (_node)
    {
        registerObjId({_ipDataRecordObjectId, 0xFF});
        registerObjId({_ipPositionDemandValueObjectId, 0x00});
        registerObjId({_ipDataConfigurationObjectId, 0x06});
        registerObjId({_ipDataConfigurationObjectId, 0x02});
        registerObjId({_ipTimePeriodObjectId, 0xFF});
        registerObjId({_ipPositionRangelLimitObjectId, 0xFF});
        registerObjId({_ipSoftwarePositionLimitObjectId, 0xFF});
        registerObjId({_ipHomeOffsetObjectId, 0xFF});
        registerObjId({_ipPolarityObjectId, 0xFF});
        registerObjId({_ipProfileVelocityObjectId, 0xFF});
        registerObjId({_ipEndVelocityObjectId, 0xFF});
        registerObjId({_ipMaxProfileVelocityObjectId, 0xFF});
        registerObjId({_ipMaxMotorSpeedObjectId, 0xFF});
        registerObjId({_ipProfileAccelerationObjectId, 0xFF});
        registerObjId({_ipMaxAccelerationObjectId, 0xFF});
        registerObjId({_ipProfileDecelerationObjectId, 0xFF});
        registerObjId({_ipMaxDecelerationObjectId, 0xFF});
        registerObjId({_ipQuickStopDecelerationObjectId, 0xFF});

        setNodeInterrest(node);

        _nodeProfile402Ip = static_cast<NodeProfile402 *>(_node->profiles()[0])->p402Ip();
        connect(_nodeProfile402Ip, &NodeProfile402Ip::enableRampEvent, this, &P402IpWidget::enableRampEvent);
        enableRampEvent(_nodeProfile402Ip->isEnableRamp());

        connect(_node, &Node::statusChanged, this, &P402IpWidget::updateData);
        updateData();

        connect(&_sendPointSinusoidalTimer, &QTimer::timeout, this, &P402IpWidget::sendDataRecordTargetWithSdo);

        _bus = _node->bus();
        connect(_bus->sync(), &Sync::signalBeforeSync, this, &P402IpWidget::sendDataRecordTargetWithPdo);
    }
}

Node *P402IpWidget::node() const
{
    return _node;
}

void P402IpWidget::readData()
{
    if (_node)
    {
        _node->readObject(_ipPositionDemandValueObjectId, 0x0);
    }
}

void P402IpWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);

            _node->readObject(_ipPositionDemandValueObjectId, 0x0);

            _node->readObject(_ipTimePeriodObjectId, 1);
            _node->readObject(_ipTimePeriodObjectId, 2);

            _node->readObject(_ipPositionRangelLimitObjectId, 1);
            _node->readObject(_ipPositionRangelLimitObjectId, 2);

            _node->readObject(_ipSoftwarePositionLimitObjectId, 1);
            _node->readObject(_ipSoftwarePositionLimitObjectId, 2);

            _node->readObject(_ipHomeOffsetObjectId, 0);
            _node->readObject(_ipPolarityObjectId, 0);

            _node->readObject(_ipProfileVelocityObjectId, 0);
            _node->readObject(_ipEndVelocityObjectId, 0);

            _node->readObject(_ipMaxProfileVelocityObjectId, 0);
            _node->readObject(_ipMaxMotorSpeedObjectId, 0);

            _node->readObject(_ipProfileAccelerationObjectId, 0);
            _node->readObject(_ipMaxAccelerationObjectId, 0);

            _node->readObject(_ipProfileDecelerationObjectId, 0);
            _node->readObject(_ipMaxDecelerationObjectId, 0);

            _node->readObject(_ipQuickStopDecelerationObjectId, 0);

            quint8 value = 1;
            _node->writeObject(_ipDataConfigurationObjectId, 0x06, QVariant(value));
        }
        else
        {
            // this->setEnabled(false);
        }
    }
}

void P402IpWidget::stop()
{
    stopTargetPosition();
}

void P402IpWidget::ipDataRecordLineEditFinished()
{
    _listDataRecord = _ipDataRecordLineEdit->text().split(QLatin1Char(','), QString::SkipEmptyParts);
    _iteratorForSendDataRecord = 0;
    ipSendDataRecord();
}

void P402IpWidget::ipSendDataRecord()
{
    if (_iteratorForSendDataRecord < _listDataRecord.size())
    {
        qint32 value = _listDataRecord.at(_iteratorForSendDataRecord).toInt();
        _node->writeObject(_ipDataRecordObjectId, 0x01, QVariant(value));
        _iteratorForSendDataRecord++;
    }
    else
    {
        _ipDataRecordLineEdit->clear();
        _listDataRecord.clear();
    }
}

void P402IpWidget::ipTimePeriodUnitEditingFinished()
{
    quint32 value = static_cast<quint32>(_ipTimePeriodUnitSpinBox->value());
    _node->writeObject(_ipTimePeriodObjectId, 0x01, QVariant(value));
}
void P402IpWidget::ipTimePeriodIndexEditingFinished()
{
    quint32 value = static_cast<quint32>(_ipTimePeriodIndexSpinBox->value());
    _node->writeObject(_ipTimePeriodObjectId, 0x02, QVariant(value));
}
void P402IpWidget::ipPositionRangelLimitMinEditingFinished()
{
    quint32 value = static_cast<quint32>(_ipPositionRangelLimitMinSpinBox->value());
    _node->writeObject(_ipPositionRangelLimitObjectId, 0x01, QVariant(value));
}
void P402IpWidget::ipPositionRangelLimitMaxEditingFinished()
{
    quint32 value = static_cast<quint32>(_ipPositionRangelLimitMaxSpinBox->value());
    _node->writeObject(_ipPositionRangelLimitObjectId, 0x02, QVariant(value));
}
void P402IpWidget::ipSoftwarePositionLimitMinEditingFinished()
{
    quint32 value = static_cast<quint32>(_ipSoftwarePositionLimitMinSpinBox->value());
    _node->writeObject(_ipSoftwarePositionLimitObjectId, 0x01, QVariant(value));
}
void P402IpWidget::ipSoftwarePositionLimitMaxFinished()
{
    quint32 value = static_cast<quint32>(_ipSoftwarePositionLimitMaxSpinBox->value());
    _node->writeObject(_ipSoftwarePositionLimitObjectId, 0x02, QVariant(value));
}
void P402IpWidget::ipHomeOffsetEditingFinished()
{
    quint32 value = static_cast<quint32>(_ipHomeOffsetSpinBox->value());
    _node->writeObject(_ipHomeOffsetObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipPolarityEditingFinished()
{
    quint8 value = static_cast<quint8>(_node->nodeOd()->index(_ipPolarityObjectId)->subIndex(0)->value().toInt());

    if (_ipPolaritySpinBox->value() == 0)
    {
        value = value & 0x7F;
    }
    else
    {
        value = value | 0x80;
    }
    _node->writeObject(_ipPolarityObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipProfileVelocityFinished()
{
    quint32 value = static_cast<quint32>(_ipProfileVelocitySpinBox->value());
    _node->writeObject(_ipProfileVelocityObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipEndVelocityFinished()
{
    quint32 value = static_cast<quint32>(_ipEndVelocitySpinBox->value());
    _node->writeObject(_ipEndVelocityObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipMaxProfileVelocityFinished()
{
    quint32 value = static_cast<quint32>(_ipMaxProfileVelocitySpinBox->value());
    _node->writeObject(_ipMaxProfileVelocityObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipMaxMotorSpeedFinished()
{
    quint32 value = static_cast<quint32>(_ipMaxMotorSpeedSpinBox->value());
    _node->writeObject(_ipMaxMotorSpeedObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipProfileAccelerationFinished()
{
    quint32 value = static_cast<quint32>(_ipProfileAccelerationSpinBox->value());
    _node->writeObject(_ipProfileAccelerationObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipMaxAccelerationFinished()
{
    quint32 value = static_cast<quint32>(_ipMaxAccelerationSpinBox->value());
    _node->writeObject(_ipMaxAccelerationObjectId, 0x00, QVariant(value));
}

void P402IpWidget::ipProfileDecelerationFinished()
{
    quint32 value = static_cast<quint32>(_ipProfileDecelerationSpinBox->value());
    _node->writeObject(_ipProfileDecelerationObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipMaxDecelerationFinished()
{
    quint32 value = static_cast<quint32>(_ipMaxDecelerationSpinBox->value());
    _node->writeObject(_ipMaxDecelerationObjectId, 0x00, QVariant(value));
}
void P402IpWidget::ipQuickStopDecelerationFinished()
{
    quint32 value = static_cast<quint32>(_ipQuickStopDecelerationSpinBox->value());
    _node->writeObject(_ipQuickStopDecelerationObjectId, 0x00, QVariant(value));
}

void P402IpWidget::ipClearBufferClicked()
{
    quint8 value = 0;

    if (_clearBufferPushButton->isChecked())
    {
        value = 0;
        _node->writeObject(_ipDataConfigurationObjectId, 0x06, QVariant(value));
        _clearBufferPushButton->setChecked(false);
    }
    else
    {
        value = 1;
        _node->writeObject(_ipDataConfigurationObjectId, 0x06, QVariant(value));
        _clearBufferPushButton->setChecked(true);
    }
}

void P402IpWidget::ipEnableRampClicked(int id)
{
    _nodeProfile402Ip->setEnableRamp(id);
}

void P402IpWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box IP mode
    QGroupBox *ipGroupBox = new QGroupBox(tr(" Interpolated position mode"));
    QFormLayout *ipLayout = new QFormLayout();

    _ipDataRecordLineEdit = new QLineEdit();
    ipLayout->addRow(tr("Data record (0x60C1) :"), _ipDataRecordLineEdit);
    _ipDataRecordLineEdit->setToolTip("Separated by ,");
    connect(_ipDataRecordLineEdit, &QLineEdit::editingFinished, this, &P402IpWidget::ipDataRecordLineEditFinished);

    _ipPositionDemandValueLabel = new QLabel();
    _ipPositionDemandValueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ipLayout->addRow("Position demand value (0x6062) :", _ipPositionDemandValueLabel);

    QLabel *ipTimePeriodLabel = new QLabel(tr("Interpolation time period (0x60C2) unit*10^index:"));
    ipLayout->addRow(ipTimePeriodLabel);
    QLayout *ipTimePeriodlayout = new QHBoxLayout();
    _ipTimePeriodUnitSpinBox = new QSpinBox();
    _ipTimePeriodUnitSpinBox->setToolTip("unit");
    _ipTimePeriodUnitSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    ipTimePeriodlayout->addWidget(_ipTimePeriodUnitSpinBox);
    _ipTimePeriodIndexSpinBox = new QSpinBox();
    _ipTimePeriodIndexSpinBox->setToolTip("index");
    _ipTimePeriodIndexSpinBox->setRange(-3, 63);
    ipTimePeriodlayout->addWidget(_ipTimePeriodIndexSpinBox);
    ipLayout->addRow(ipTimePeriodlayout);
    connect(_ipTimePeriodUnitSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipTimePeriodUnitEditingFinished);
    connect(_ipTimePeriodIndexSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipTimePeriodIndexEditingFinished);

    _clearBufferPushButton = new QPushButton(tr("Clear Buffer"));
    _clearBufferPushButton->setCheckable(true);
    _clearBufferPushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    ipLayout->addRow(_clearBufferPushButton);
    connect(_clearBufferPushButton, &QPushButton::clicked, this, &P402IpWidget::ipClearBufferClicked);

    QLabel *ipPositionRangelLimitLabel = new QLabel(tr("Position range limit (0x607B) :"));
    ipLayout->addRow(ipPositionRangelLimitLabel);
    QLayout *ipPositionRangelLimitlayout = new QHBoxLayout();
    _ipPositionRangelLimitMinSpinBox = new QSpinBox();
    _ipPositionRangelLimitMinSpinBox->setSuffix(" inc");
    _ipPositionRangelLimitMinSpinBox->setToolTip("min");
    _ipPositionRangelLimitMinSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    ipPositionRangelLimitlayout->addWidget(_ipPositionRangelLimitMinSpinBox);
    _ipPositionRangelLimitMaxSpinBox = new QSpinBox();
    _ipPositionRangelLimitMaxSpinBox->setSuffix(" inc");
    _ipPositionRangelLimitMaxSpinBox->setToolTip("max");
    _ipPositionRangelLimitMaxSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    ipPositionRangelLimitlayout->addWidget(_ipPositionRangelLimitMaxSpinBox);
    ipLayout->addRow(ipPositionRangelLimitlayout);
    connect(_ipPositionRangelLimitMinSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipPositionRangelLimitMinEditingFinished);
    connect(_ipPositionRangelLimitMaxSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipPositionRangelLimitMaxEditingFinished);

    QLabel *ipSoftwarePositionLimitLabel = new QLabel(tr("Software position limit (0x607D) :"));
    ipLayout->addRow(ipSoftwarePositionLimitLabel);
    QLayout *ipSoftwarePositionLimitlayout = new QHBoxLayout();
    _ipSoftwarePositionLimitMinSpinBox = new QSpinBox();
    _ipSoftwarePositionLimitMinSpinBox->setSuffix(" inc");
    _ipSoftwarePositionLimitMinSpinBox->setToolTip("min");
    _ipSoftwarePositionLimitMinSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    ipSoftwarePositionLimitlayout->addWidget(_ipSoftwarePositionLimitMinSpinBox);
    _ipSoftwarePositionLimitMaxSpinBox = new QSpinBox();
    _ipSoftwarePositionLimitMaxSpinBox->setSuffix(" inc");
    _ipSoftwarePositionLimitMaxSpinBox->setToolTip("max");
    _ipSoftwarePositionLimitMaxSpinBox->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    ipSoftwarePositionLimitlayout->addWidget(_ipSoftwarePositionLimitMaxSpinBox);
    ipLayout->addRow(ipSoftwarePositionLimitlayout);
    connect(_ipSoftwarePositionLimitMinSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipSoftwarePositionLimitMinEditingFinished);
    connect(_ipSoftwarePositionLimitMaxSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipSoftwarePositionLimitMaxFinished);

    // Add Home offset (0x607C) and Polarity (0x607E)
    QLayout *ipHomeOffsetlayout = new QVBoxLayout();
    QLabel *ipHomeOffsetLabel = new QLabel(tr("Home offset (0x607C) :"));
    _ipHomeOffsetSpinBox = new QSpinBox();
    _ipHomeOffsetSpinBox->setSuffix(" inc");
    _ipHomeOffsetSpinBox->setToolTip("");
    _ipHomeOffsetSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipHomeOffsetlayout->addWidget(ipHomeOffsetLabel);
    ipHomeOffsetlayout->addWidget(_ipHomeOffsetSpinBox);
    connect(_ipHomeOffsetSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipHomeOffsetEditingFinished);

    QLayout *ipPolaritylayout = new QVBoxLayout();
    QLabel *ipPolarityLabel = new QLabel(tr("Polarity (0x607E bit 7) :"));
    _ipPolaritySpinBox = new QSpinBox();
    _ipPolaritySpinBox->setToolTip("0 = x1, 1 = x(-1)");
    _ipPolaritySpinBox->setRange(0, 1);
    ipPolaritylayout->addWidget(ipPolarityLabel);
    ipPolaritylayout->addWidget(_ipPolaritySpinBox);
    connect(_ipPolaritySpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipPolarityEditingFinished);

    QHBoxLayout *ipHomePolaritylayout = new QHBoxLayout();
    ipHomePolaritylayout->addLayout(ipHomeOffsetlayout);
    ipHomePolaritylayout->addLayout(ipPolaritylayout);
    ipLayout->addRow(ipHomePolaritylayout);

    // Add Profile velocity (0x6081) and Max motor speed (0x6080)
    QLayout *ipProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipProfileVelocityLabel = new QLabel(tr("Profile velocity (0x6081) :"));
    _ipProfileVelocitySpinBox = new QSpinBox();
    _ipProfileVelocitySpinBox->setSuffix(" inc/s");
    _ipProfileVelocitySpinBox->setToolTip("");
    _ipProfileVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    ipProfileVelocitylayout->addWidget(ipProfileVelocityLabel);
    ipProfileVelocitylayout->addWidget(_ipProfileVelocitySpinBox);
    connect(_ipProfileVelocitySpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipProfileVelocityFinished);

    QLayout *ipEndVelocitylayout = new QVBoxLayout();
    QLabel *ipEndVelocityLabel = new QLabel(tr("End velocity (0x6082) :"));
    _ipEndVelocitySpinBox = new QSpinBox();
    _ipEndVelocitySpinBox->setSuffix(" inc/s");
    _ipEndVelocitySpinBox->setToolTip("");
    _ipEndVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    ipEndVelocitylayout->addWidget(ipEndVelocityLabel);
    ipEndVelocitylayout->addWidget(_ipEndVelocitySpinBox);
    connect(_ipEndVelocitySpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipEndVelocityFinished);

    QHBoxLayout *ipMaxVelocitylayout = new QHBoxLayout();
    ipMaxVelocitylayout->addLayout(ipProfileVelocitylayout);
    ipMaxVelocitylayout->addLayout(ipEndVelocitylayout);
    ipLayout->addRow(ipMaxVelocitylayout);

    // Add Max profile velocity (0x607F) and Max motor speed (0x6080)
    QLayout *ipMaxProfileVelocitylayout = new QVBoxLayout();
    QLabel *ipMaxProfileVelocityLabel = new QLabel(tr("Max profile velocity (0x607F) :"));
    _ipMaxProfileVelocitySpinBox = new QSpinBox();
    _ipMaxProfileVelocitySpinBox->setSuffix(" inc/period");
    _ipMaxProfileVelocitySpinBox->setToolTip("");
    _ipMaxProfileVelocitySpinBox->setRange(0, std::numeric_limits<int>::max());
    ipMaxProfileVelocitylayout->addWidget(ipMaxProfileVelocityLabel);
    ipMaxProfileVelocitylayout->addWidget(_ipMaxProfileVelocitySpinBox);
    connect(_ipMaxProfileVelocitySpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipMaxProfileVelocityFinished);

    QLayout *ipMaxMotorSpeedlayout = new QVBoxLayout();
    QLabel *ipMaxMotorSpeedLabel = new QLabel(tr("Max motor speed (0x6080) :"));
    _ipMaxMotorSpeedSpinBox = new QSpinBox();
    _ipMaxMotorSpeedSpinBox->setSuffix(" inc/period");
    _ipMaxMotorSpeedSpinBox->setToolTip("");
    _ipMaxMotorSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipMaxMotorSpeedlayout->addWidget(ipMaxMotorSpeedLabel);
    ipMaxMotorSpeedlayout->addWidget(_ipMaxMotorSpeedSpinBox);
    connect(_ipMaxMotorSpeedSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipMaxMotorSpeedFinished);

    QHBoxLayout *ipVelocitylayout = new QHBoxLayout();
    ipVelocitylayout->addLayout(ipMaxProfileVelocitylayout);
    ipVelocitylayout->addLayout(ipMaxMotorSpeedlayout);
    ipLayout->addRow(ipVelocitylayout);

    // Add Profile acceleration (0x6083) and Max acceleration (0x60C5)
    QLayout *ipProfileAccelerationlayout = new QVBoxLayout();
    QLabel *ipProfileAccelerationLabel = new QLabel(tr("Profile acceleration (0x6083) :"));
    _ipProfileAccelerationSpinBox = new QSpinBox();
    _ipProfileAccelerationSpinBox->setSuffix(" inc/s2");
    _ipProfileAccelerationSpinBox->setToolTip("");
    _ipProfileAccelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipProfileAccelerationlayout->addWidget(ipProfileAccelerationLabel);
    ipProfileAccelerationlayout->addWidget(_ipProfileAccelerationSpinBox);
    connect(_ipProfileAccelerationSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipProfileAccelerationFinished);

    QLayout *ipMaxAccelerationlayout = new QVBoxLayout();
    QLabel *ipMaxAccelerationLabel = new QLabel(tr("Max acceleration (0x60C5) :"));
    _ipMaxAccelerationSpinBox = new QSpinBox();
    _ipMaxAccelerationSpinBox->setSuffix(" inc/s2");
    _ipMaxAccelerationSpinBox->setToolTip("");
    _ipMaxAccelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipMaxAccelerationlayout->addWidget(ipMaxAccelerationLabel);
    ipMaxAccelerationlayout->addWidget(_ipMaxAccelerationSpinBox);
    connect(_ipMaxAccelerationSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipMaxAccelerationFinished);

    QHBoxLayout *ipProfileAccelerationHlayout = new QHBoxLayout();
    ipProfileAccelerationHlayout->addLayout(ipProfileAccelerationlayout);
    ipProfileAccelerationHlayout->addLayout(ipMaxAccelerationlayout);
    ipLayout->addRow(ipProfileAccelerationHlayout);

    // Add Profile deceleration (0x6084) and Max deceleration (0x60C6)
    QLayout *ipProfileDecelerationlayout = new QVBoxLayout();
    QLabel *ipProfileDecelerationLabel = new QLabel(tr("Profile deceleration (0x6084) :"));
    _ipProfileDecelerationSpinBox = new QSpinBox();
    _ipProfileDecelerationSpinBox->setSuffix(" inc/s2");
    _ipProfileDecelerationSpinBox->setToolTip("");
    _ipProfileDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipProfileDecelerationlayout->addWidget(ipProfileDecelerationLabel);
    ipProfileDecelerationlayout->addWidget(_ipProfileDecelerationSpinBox);
    connect(_ipProfileDecelerationSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipProfileDecelerationFinished);

    QLayout *ipMaxDecelerationlayout = new QVBoxLayout();
    QLabel *ipMaxDecelerationLabel = new QLabel(tr("Max deceleration (0x60C6) :"));
    _ipMaxDecelerationSpinBox = new QSpinBox();
    _ipMaxDecelerationSpinBox->setSuffix(" inc/s2");
    _ipMaxDecelerationSpinBox->setToolTip("");
    _ipMaxDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipMaxDecelerationlayout->addWidget(ipMaxDecelerationLabel);
    ipMaxDecelerationlayout->addWidget(_ipMaxDecelerationSpinBox);
    connect(_ipMaxDecelerationSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipMaxDecelerationFinished);

    QHBoxLayout *ipProfileDecelerationHlayout = new QHBoxLayout();
    ipProfileDecelerationHlayout->addLayout(ipProfileDecelerationlayout);
    ipProfileDecelerationHlayout->addLayout(ipMaxDecelerationlayout);
    ipLayout->addRow(ipProfileDecelerationHlayout);

    // Add Quick stop deceleration (0x6085)
    QLayout *ipQuickStopDecelerationlayout = new QVBoxLayout();
    QLabel *ipQuickStopDecelerationLabel = new QLabel(tr("Quick stop deceleration (0x6085) :"));
    _ipQuickStopDecelerationSpinBox = new QSpinBox();
    _ipQuickStopDecelerationSpinBox->setSuffix(" inc/s2");
    _ipQuickStopDecelerationSpinBox->setToolTip("");
    _ipQuickStopDecelerationSpinBox->setRange(0, std::numeric_limits<int>::max());
    ipQuickStopDecelerationlayout->addWidget(ipQuickStopDecelerationLabel);
    ipQuickStopDecelerationlayout->addWidget(_ipQuickStopDecelerationSpinBox);
    ipLayout->addRow(ipQuickStopDecelerationlayout);
    connect(_ipQuickStopDecelerationSpinBox, &QSpinBox::editingFinished, this, &P402IpWidget::ipQuickStopDecelerationFinished);

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
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4, bit 8"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    QLabel *ipEnableRampLabel = new QLabel(tr("Enable interpolation (bit 4) :"));
    modeControlWordLayout->addRow(ipEnableRampLabel);
    _ipEnableRampButtonGroup = new QButtonGroup(this);
    _ipEnableRampButtonGroup->setExclusive(true);
    QVBoxLayout *ipEnableRamplayout = new QVBoxLayout();
    QRadioButton *ip0EnableRamp = new QRadioButton(tr("0 Disable interpolation"));
    ipEnableRamplayout->addWidget(ip0EnableRamp);
    QRadioButton *ip1EnableRamp = new QRadioButton(tr("1 Enable interpolation"));
    ipEnableRamplayout->addWidget(ip1EnableRamp);
    _ipEnableRampButtonGroup->addButton(ip0EnableRamp, 0);
    _ipEnableRampButtonGroup->addButton(ip1EnableRamp, 1);
    modeControlWordLayout->addRow(ipEnableRamplayout);
    connect(_ipEnableRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { ipEnableRampClicked(id); });

    QLabel *ipHaltLabel = new QLabel(tr("Halt ramp (bit 8) : Motor stopped"));
    modeControlWordLayout->addRow(ipHaltLabel);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402IpWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402IpWidget::pdoMapping);

    QPixmap ipModePixmap;
    QLabel *ipModeLabel;
    ipModeLabel = new QLabel();
    ipModePixmap.load(":/diagram/img/402IPDiagram.png");
    ipModeLabel->setPixmap(ipModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram IP mode"));
    connect(imgPushButton, SIGNAL(clicked()), ipModeLabel, SLOT(show()));
    QHBoxLayout *ipButtonLayout = new QHBoxLayout();
    ipButtonLayout->addWidget(dataLoggerPushButton);
    ipButtonLayout->addWidget(mappingPdoPushButton);
    ipButtonLayout->addWidget(imgPushButton);

    layout->addWidget(ipGroupBox);
    layout->addWidget(generatorSinusoidalGroupBox);
    layout->addWidget(modeControlWordGroupBox);
    layout->addItem(ipButtonLayout);

    setLayout(layout);
}

// each period * 5 -> we send 5 set-point, for have always value in buffer in device
void P402IpWidget::goTargetPosition()
{
    quint32 unit = 0;
    qint32 index = 0;
    qreal period = 0;

    qint32 initialPosition = _node->nodeOd()->value(_ipPositionDemandValueObjectId).toInt();
    calculatePointSinusoidalMotionProfile(initialPosition);

    quint8 value = 1;
    _node->writeObject(_ipDataConfigurationObjectId, 0x06, QVariant(value));

    unit = static_cast<quint32>(_ipTimePeriodUnitSpinBox->value());
    index = static_cast<qint32>(_ipTimePeriodIndexSpinBox->value());

    period = qPow(10, index);
    period = unit * period * 1000;

    if (_bus->sync()->status() == Sync::Status::STOPPED)
    {
        sendDataRecordTargetWithSdo();
        _sendPointSinusoidalTimer.start(static_cast<int>(period) * 5);
    }
    _goTargetPushButton->setEnabled(false);
    _ipDataRecordLineEdit->setEnabled(false);
}

void P402IpWidget::stopTargetPosition()
{
    _pointSinusoidalVector.clear();
    _sendPointSinusoidalTimer.stop();
    _goTargetPushButton->setEnabled(true);
    _ipDataRecordLineEdit->setEnabled(true);
}

void P402IpWidget::readActualBufferSize()
{
    _node->readObject(_ipDataConfigurationObjectId, 0x2);
}

void P402IpWidget::sendDataRecordTargetWithPdo()
{
    if (_pointSinusoidalVector.isEmpty())
    {
        stopTargetPosition();
        return;
    }
    _node->writeObject(_ipDataRecordObjectId, 0x01, QVariant(_pointSinusoidalVector.at(0)));
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
        _node->writeObject(_ipDataRecordObjectId, 0x01, QVariant(_pointSinusoidalVector.at(i)));
    }
    _pointSinusoidalVector.remove(0, i);
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
    unit = static_cast<quint32>(_ipTimePeriodUnitSpinBox->value());
    index = static_cast<qint32>(_ipTimePeriodIndexSpinBox->value());

    period = qPow(10, index);
    period = unit * period * 1000;
    if (period == 0.0)
    {
        return;
    }
    _pointSinusoidalVector.clear();

    int j = 0;
    for(quint32 i = 0; i <= duration; i++)
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

void P402IpWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData({_node->busId(), _node->nodeId(), _ipPositionDemandValueObjectId, 0x0, QMetaType::Type::Short});
    _dataLoggerWidget->show();
}

void P402IpWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6040, 0, QMetaType::Type::UShort);
    NodeObjectId statusWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6041, 0, QMetaType::Type::UShort);

    QList<NodeObjectId> ipRpdoObjectList = {{controlWordObjectId},
                                            {_node->busId(), _node->nodeId(), _ipDataRecordObjectId, 0x0, QMetaType::Type::Short}};
    _node->rpdos().at(0)->writeMapping(ipRpdoObjectList);

    QList<NodeObjectId> ipTpdoObjectList = {{statusWordObjectId},
                                            {_node->busId(), _node->nodeId(), _ipPositionDemandValueObjectId, 0x0, QMetaType::Type::Short}};

    _node->tpdos().at(2)->writeMapping(ipTpdoObjectList);
}

void P402IpWidget::enableRampEvent(bool ok)
{
    _ipEnableRampButtonGroup->button(ok)->setChecked(ok);
}

void P402IpWidget::refreshData(quint16 object)
{
    int value;
    if (_node->nodeOd()->indexExist(object))
    {
        if (object == _ipPositionDemandValueObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipPositionDemandValueLabel->setNum(value);
        }

        if (object == _ipTimePeriodObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _ipTimePeriodUnitSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _ipTimePeriodIndexSpinBox->setValue(value);
        }
        if (object == _ipPositionRangelLimitObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _ipPositionRangelLimitMinSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _ipPositionRangelLimitMaxSpinBox->setValue(value);
        }
        if (object == _ipSoftwarePositionLimitObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _ipSoftwarePositionLimitMinSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _ipSoftwarePositionLimitMaxSpinBox->setValue(value);
        }

        if (object == _ipHomeOffsetObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipHomeOffsetSpinBox->setValue(value);
        }
        if (object == _ipPolarityObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipPolaritySpinBox->setValue(value);
        }
        if (object == _ipProfileVelocityObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipProfileVelocitySpinBox->setValue(value);
        }
        if (object == _ipEndVelocityObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipEndVelocitySpinBox->setValue(value);
        }
        if (object == _ipMaxProfileVelocityObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipMaxProfileVelocitySpinBox->setValue(value);
        }
        if (object == _ipMaxMotorSpeedObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipMaxMotorSpeedSpinBox->setValue(value);
        }
        if (object == _ipProfileAccelerationObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipProfileAccelerationSpinBox->setValue(value);
        }
        if (object == _ipMaxAccelerationObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipMaxAccelerationSpinBox->setValue(value);
        }
        if (object == _ipProfileDecelerationObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipProfileDecelerationSpinBox->setValue(value);
        }
        if (object == _ipMaxDecelerationObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipMaxDecelerationSpinBox->setValue(value);
        }
        if (object == _ipQuickStopDecelerationObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _ipQuickStopDecelerationSpinBox->setValue(value);
        }
    }
}

void P402IpWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!_node)
    {
        return;
    }

    if ((objId.index == _ipPositionDemandValueObjectId)
        || (objId.index == _ipVelocityActualObjectId)
        || (objId.index == _ipTimePeriodObjectId)
        || (objId.index == _ipPositionRangelLimitObjectId)
        || (objId.index == _ipSoftwarePositionLimitObjectId)
        || (objId.index == _ipHomeOffsetObjectId)
        || (objId.index == _ipPolarityObjectId)
        || (objId.index == _ipProfileVelocityObjectId)
        || (objId.index == _ipEndVelocityObjectId)
        || (objId.index == _ipMaxProfileVelocityObjectId)
        || (objId.index == _ipMaxMotorSpeedObjectId)
        || (objId.index == _ipProfileAccelerationObjectId)
        || (objId.index == _ipMaxAccelerationObjectId)
        || (objId.index == _ipProfileDecelerationObjectId)
        || (objId.index == _ipMaxDecelerationObjectId)
        || (objId.index == _ipQuickStopDecelerationObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId.index);
    }
    if ((objId.index == _ipDataRecordObjectId))
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
    if ((objId.index == _ipDataConfigurationObjectId) && (objId.subIndex == 0x06)) // object BUFFER_CLEAR
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
    }
    if ((objId.index == _ipDataConfigurationObjectId) && (objId.subIndex == 0x02))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
    }
}
