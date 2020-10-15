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

#include "p402vlwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/widget/indexspinbox.h"
#include "services/services.h"

#include "indexdb402.h"
#include "profile/p402/nodeprofile402.h"
#include "profile/p402/nodeprofile402vl.h"
#include <QFormLayout>
#include <QPushButton>
#include <QRadioButton>

P402VlWidget::P402VlWidget(QWidget *parent) : QWidget(parent)
{
    _node = nullptr;
    _nodeProfile402 = nullptr;
    createWidgets();

    _vlVelocityDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_DEMAND);
    _vlVelocityActualObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_ACTUAL_VALUE);
    _vlTargetVelocityObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET);
}

P402VlWidget::~P402VlWidget()
{
    unRegisterFullOd();
}

Node *P402VlWidget::node() const
{
    return _node;
}

void P402VlWidget::readData()
{
    if (_node)
    {
        _node->readObject(_vlVelocityDemandObjectId);
        _node->readObject(_vlVelocityActualObjectId);
    }
}

void P402VlWidget::setNode(Node *node)
{
    _vlMinVelocityMinMaxAmountSpinBox->setNode(node);
    _vlMaxVelocityMinMaxAmountSpinBox->setNode(node);
    _vlAccelerationDeltaSpeedSpinBox->setNode(node);
    _vlAccelerationDeltaTimeSpinBox->setNode(node);
    _vlDecelerationDeltaSpeedSpinBox->setNode(node);
    _vlDecelerationDeltaTimeSpinBox->setNode(node);
    _vlQuickStopDeltaSpeedSpinBox->setNode(node);
    _vlQuickStopDeltaTimeSpinBox->setNode(node);
    _vlSetPointFactorNumeratorSpinBox->setNode(node);
    _vlSetPointFactorDenominatorSpinBox->setNode(node);
    _vlDimensionFactorNumeratorSpinBox->setNode(node);
    _vlDimensionFactorDenominatorSpinBox->setNode(node);

    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402VlWidget::updateData);
        }
    }

    _node = node;
    if (_node)
    {
        _vlTargetVelocityObjectId.setBusId(_node->busId());
        _vlVelocityDemandObjectId.setBusId(_node->busId());
        _vlVelocityActualObjectId.setBusId(_node->busId());
        _vlTargetVelocityObjectId.setNodeId(_node->nodeId());
        _vlVelocityDemandObjectId.setNodeId(_node->nodeId());
        _vlVelocityActualObjectId.setNodeId(_node->nodeId());
        _vlTargetVelocityObjectId.setDataType(QMetaType::Type::Short);
        _vlVelocityDemandObjectId.setDataType(QMetaType::Type::Short);
        _vlVelocityActualObjectId.setDataType(QMetaType::Type::Short);

        registerObjId({_vlTargetVelocityObjectId});
        registerObjId({_vlVelocityDemandObjectId});
        registerObjId({_vlVelocityActualObjectId});

        setNodeInterrest(node);

        _vlMinVelocityMinMaxAmountSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_MIN));
        _vlMaxVelocityMinMaxAmountSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_MAX));
        _vlAccelerationDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_SPEED));
        _vlAccelerationDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_ACCELERATION_DELTA_TIME));
        _vlDecelerationDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_SPEED));
        _vlDecelerationDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DECELERATION_DELTA_TIME));
        _vlQuickStopDeltaSpeedSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_SPEED));
        _vlQuickStopDeltaTimeSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_QUICK_STOP_DELTA_TIME));
        _vlSetPointFactorNumeratorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_NUMERATOR));
        _vlSetPointFactorDenominatorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_SET_POINT_FACTOR_DENOMINATOR));
        _vlDimensionFactorNumeratorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_NUMERATOR));
        _vlDimensionFactorDenominatorSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_VL_DIMENSION_FACTOR_DENOMINATOR));

        if (!_node->profiles().isEmpty())
        {
            _nodeProfile402 = static_cast<NodeProfile402 *>(_node->profiles()[0]);
            connect(_nodeProfile402, &NodeProfile402::enableRampEvent, this, &P402VlWidget::vlEnableRampEvent);
            connect(_nodeProfile402, &NodeProfile402::unlockRampEvent, this, &P402VlWidget::vlUnlockRampEvent);
            connect(_nodeProfile402, &NodeProfile402::referenceRampEvent, this, &P402VlWidget::vlReferenceRamp);

            vlEnableRampEvent(_nodeProfile402->isEnableRamp());
            vlUnlockRampEvent(_nodeProfile402->isUnlockRamp());
            vlReferenceRamp(_nodeProfile402->isReferenceRamp());
        }

        connect(_node, &Node::statusChanged, this, &P402VlWidget::updateData);
    }
}

void P402VlWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);
            _node->readObject(_vlTargetVelocityObjectId);
            _node->readObject(_vlVelocityDemandObjectId);
            _node->readObject(_vlVelocityActualObjectId);

            if (_nodeProfile402)
            {
                _nodeProfile402->setEnableRamp(true);
                _nodeProfile402->setReferenceRamp(true);
                _nodeProfile402->setUnlockRamp(true);
            }
        }
    }
}

void P402VlWidget::vlTargetVelocitySpinboxFinished()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, QVariant(value));
    _vlTargetVelocitySlider->setValue(value);
}
void P402VlWidget::vlTargetVelocitySliderChanged()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, QVariant(value));
}

void P402VlWidget::vlEnableRampClicked(int id)
{
    if (_nodeProfile402)
    {
        _nodeProfile402->setEnableRamp(id);
    }
}

void P402VlWidget::vlUnlockRampClicked(int id)
{
    if (_nodeProfile402)
    {
        _nodeProfile402->setUnlockRamp(id);
    }
}

void P402VlWidget::vlReferenceRampClicked(int id)
{
    if (_nodeProfile402)
    {
        _nodeProfile402->setReferenceRamp(id);
    }
}

void P402VlWidget::vlEnableRampEvent(bool ok)
{
    _vlEnableRampButtonGroup->button(ok)->setChecked(ok);
}

void P402VlWidget::vlUnlockRampEvent(bool ok)
{
    _vlUnlockRampButtonGroup->button(ok)->setChecked(ok);
}

void P402VlWidget::vlReferenceRamp(bool ok)
{
    _vlReferenceRampButtonGroup->button(ok)->setChecked(ok);
}

void P402VlWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);

    dataLogger->addData({_vlVelocityActualObjectId});
    dataLogger->addData({_vlTargetVelocityObjectId});
    dataLogger->addData({_vlVelocityDemandObjectId});

    _dataLoggerWidget->show();
}

void P402VlWidget::pdoMapping()
{
    NodeObjectId controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    NodeObjectId statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD);
    controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    QList<NodeObjectId> vlRpdoObjectList = {{controlWordObjectId}, {_vlTargetVelocityObjectId}};
    _node->rpdos().at(0)->writeMapping(vlRpdoObjectList);

    QList<NodeObjectId> vlTpdoObjectList = {{statusWordObjectId}, {_vlVelocityDemandObjectId}};
    _node->tpdos().at(2)->writeMapping(vlTpdoObjectList);
}

void P402VlWidget::refreshData(NodeObjectId object)
{
    int value;
    if (_node->nodeOd()->indexExist(object.index()))
    {
        if (object == _vlTargetVelocityObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            if (!_vlTargetVelocitySpinBox->hasFocus())
            {
                _vlTargetVelocitySpinBox->setValue(value);
            }
            if (!_vlTargetVelocitySlider->isSliderDown())
            {
                //_vlTargetVelocitySlider->setValue(value);
            }
        }
        if (object == _vlVelocityDemandObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _vlVelocityDemandLabel->setNum(value);
        }
        if (object == _vlVelocityActualObjectId)
        {
            value = _node->nodeOd()->value(object).toInt();
            _vlVelocityActualLabel->setNum(value);
        }
    }
}

void P402VlWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box VL mode
    QGroupBox *vlGroupBox = new QGroupBox(tr("Velocity mode"));
    QFormLayout *vlLayout = new QFormLayout();

    _vlTargetVelocitySpinBox = new QSpinBox();
    _vlTargetVelocitySpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow("Target velocity (0x6042) :", _vlTargetVelocitySpinBox);

    _vlTargetVelocitySlider = new QSlider(Qt::Horizontal);
    _vlTargetVelocitySlider->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow(_vlTargetVelocitySlider);

    connect(_vlTargetVelocitySlider, &QSlider::valueChanged, _vlTargetVelocitySpinBox, &QSpinBox::setValue);
    connect(_vlTargetVelocitySlider, &QSlider::valueChanged, this, &P402VlWidget::vlTargetVelocitySliderChanged);
    connect(_vlTargetVelocitySpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlTargetVelocitySpinboxFinished);

    _vlVelocityDemandLabel = new QLabel();
    _vlVelocityDemandLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vlLayout->addRow("Velocity_demand (0x6043) :", _vlVelocityDemandLabel);

    _vlVelocityActualLabel = new QLabel();
    _vlVelocityActualLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vlLayout->addRow("Velocity_actual_value (0x6044) :", _vlVelocityActualLabel);

    QLabel *vlVelocityMinMaxAmountLabel = new QLabel(tr("Velocity min max amount (0x6046) :"));
    vlLayout->addRow(vlVelocityMinMaxAmountLabel);
    QLayout *vlVelocityMinMaxAmountlayout = new QHBoxLayout();

    _vlMinVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _vlMinVelocityMinMaxAmountSpinBox->setToolTip("min ");
    //    _vlMinVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(),
    //    std::numeric_limits<int>::max());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMinVelocityMinMaxAmountSpinBox);
    _vlMaxVelocityMinMaxAmountSpinBox = new IndexSpinBox();
    _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("max ");
    //    _vlMaxVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(),
    //    std::numeric_limits<int>::max());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMaxVelocityMinMaxAmountSpinBox);
    vlLayout->addRow(vlVelocityMinMaxAmountlayout);

    QLabel *vlVelocityAccelerationLabel = new QLabel(tr("Velocity acceleration (0x6048) :"));
    vlLayout->addRow(vlVelocityAccelerationLabel);
    QLayout *vlVelocityAccelerationlayout = new QHBoxLayout();
    _vlAccelerationDeltaSpeedSpinBox = new IndexSpinBox();
    //    _vlAccelerationDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlAccelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
    //    _vlAccelerationDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaSpeedSpinBox);
    _vlAccelerationDeltaTimeSpinBox = new IndexSpinBox();
    //    _vlAccelerationDeltaTimeSpinBox->setSuffix(" ms");
    _vlAccelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    //    _vlAccelerationDeltaTimeSpinBox->setRange(1, std::numeric_limits<quint16>::max());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityAccelerationlayout);

    QLabel *vlVelocityDecelerationLabel = new QLabel(tr("Velocity deceleration (0x6049) :"));
    vlLayout->addRow(vlVelocityDecelerationLabel);
    QLayout *vlVelocityDecelerationlayout = new QHBoxLayout();
    _vlDecelerationDeltaSpeedSpinBox = new IndexSpinBox();
    //    _vlDecelerationDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlDecelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
    //    _vlDecelerationDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaSpeedSpinBox);
    _vlDecelerationDeltaTimeSpinBox = new IndexSpinBox();
    //    _vlDecelerationDeltaTimeSpinBox->setSuffix(" ms");
    _vlDecelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    //    _vlDecelerationDeltaTimeSpinBox->setRange(1, std::numeric_limits<quint16>::max());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityDecelerationlayout);

    QLabel *vlVelocityQuickStopLabel = new QLabel(tr("Velocity quick stop (0x604A) :"));
    vlLayout->addRow(vlVelocityQuickStopLabel);
    QLayout *vlVelocityQuickStoplayout = new QHBoxLayout();
    _vlQuickStopDeltaSpeedSpinBox = new IndexSpinBox();
    //    _vlQuickStopDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlQuickStopDeltaSpeedSpinBox->setToolTip("Delta Speed");
    //    _vlQuickStopDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaSpeedSpinBox);
    _vlQuickStopDeltaTimeSpinBox = new IndexSpinBox();
    //    _vlQuickStopDeltaTimeSpinBox->setSuffix(" ms");
    _vlQuickStopDeltaTimeSpinBox->setToolTip("Delta Time");
    //    _vlQuickStopDeltaTimeSpinBox->setRange(1, std::numeric_limits<quint16>::max());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityQuickStoplayout);

    QLabel *vlSetPointFactorLabel = new QLabel(tr("Set-point factor (0x604B) :"));
    vlLayout->addRow(vlSetPointFactorLabel);
    QLayout *vlSetPointFactorlayout = new QHBoxLayout();
    _vlSetPointFactorNumeratorSpinBox = new IndexSpinBox();
    _vlSetPointFactorNumeratorSpinBox->setToolTip("Numerator");
    //    _vlSetPointFactorNumeratorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorNumeratorSpinBox);
    _vlSetPointFactorDenominatorSpinBox = new IndexSpinBox();
    _vlSetPointFactorDenominatorSpinBox->setToolTip("Denominator");
    //    _vlSetPointFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorDenominatorSpinBox);
    vlLayout->addRow(vlSetPointFactorlayout);

    QLabel *vlDimensionFactorLabel = new QLabel(tr("Dimension factor (0x604C) :"));
    vlLayout->addRow(vlDimensionFactorLabel);
    QLayout *vlDimensionFactorlayout = new QHBoxLayout();
    _vlDimensionFactorNumeratorSpinBox = new IndexSpinBox();
    _vlDimensionFactorNumeratorSpinBox->setToolTip("Numerator");
    //    _vlDimensionFactorNumeratorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorNumeratorSpinBox);
    _vlDimensionFactorDenominatorSpinBox = new IndexSpinBox();
    _vlDimensionFactorDenominatorSpinBox->setToolTip("Denominator");
    //    _vlDimensionFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorDenominatorSpinBox);
    vlLayout->addRow(vlDimensionFactorlayout);

    vlGroupBox->setLayout(vlLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4, bit 5, bit 6, bit 8"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    QLabel *vlEnableRampLabel = new QLabel(tr("Enable ramp (bit 4) :"));
    modeControlWordLayout->addRow(vlEnableRampLabel);
    _vlEnableRampButtonGroup = new QButtonGroup(this);
    _vlEnableRampButtonGroup->setExclusive(true);
    QVBoxLayout *vlEnableRamplayout = new QVBoxLayout();
    QRadioButton *vl0EnableRamp = new QRadioButton(tr("Velocity demand value shall be controlled in any other"));
    vlEnableRamplayout->addWidget(vl0EnableRamp);
    QRadioButton *vl1EnableRamp = new QRadioButton(tr("Velocity demand value shall accord with ramp output value"));
    vlEnableRamplayout->addWidget(vl1EnableRamp);
    _vlEnableRampButtonGroup->addButton(vl0EnableRamp, 0);
    _vlEnableRampButtonGroup->addButton(vl1EnableRamp, 1);
    modeControlWordLayout->addRow(vlEnableRamplayout);
    connect(_vlEnableRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {vlEnableRampClicked(id);});

    QLabel *vlUnlockRampLabel = new QLabel(tr("Unlock ramp (bit 5) :"));
    modeControlWordLayout->addRow(vlUnlockRampLabel);
    _vlUnlockRampButtonGroup = new QButtonGroup(this);
    _vlUnlockRampButtonGroup->setExclusive(true);
    QVBoxLayout *vlUnlockRamplayout = new QVBoxLayout();
    _vlUnlockRampButtonGroup = new QButtonGroup(this);
    QRadioButton *vl0UnlockRamp = new QRadioButton(tr("Ramp output value shall be locked to current output value"));
    vlUnlockRamplayout->addWidget(vl0UnlockRamp);
    QRadioButton *vl1UnlockRamp = new QRadioButton(tr("Ramp output value shall follow ramp input value"));
    vlUnlockRamplayout->addWidget(vl1UnlockRamp);
    _vlUnlockRampButtonGroup->addButton(vl0UnlockRamp, 0);
    _vlUnlockRampButtonGroup->addButton(vl1UnlockRamp, 1);
    modeControlWordLayout->addRow(vlUnlockRamplayout);
    connect(_vlUnlockRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {vlUnlockRampClicked(id);});

    QLabel *_vlReferenceRampLabel = new QLabel(tr("Reference ramp (bit 6) :"));
    modeControlWordLayout->addRow(_vlReferenceRampLabel);
    _vlReferenceRampButtonGroup = new QButtonGroup(this);
    _vlReferenceRampButtonGroup->setExclusive(true);
    QVBoxLayout *_vlReferenceRamplayout = new QVBoxLayout();
    QRadioButton *vl0ReferenceRamp = new QRadioButton(tr("Ramp input value shall be set to zero"));
    _vlReferenceRamplayout->addWidget(vl0ReferenceRamp);
    QRadioButton *vl1ReferenceRamp = new QRadioButton(tr("Ramp input value shall accord with ramp reference"));
    _vlReferenceRamplayout->addWidget(vl1ReferenceRamp);
    _vlReferenceRampButtonGroup->addButton(vl0ReferenceRamp, 0);
    _vlReferenceRampButtonGroup->addButton(vl1ReferenceRamp, 1);
    modeControlWordLayout->addRow(_vlReferenceRamplayout);
    connect(_vlReferenceRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) {vlReferenceRampClicked(id);});

    QLabel *vlHaltLabel = new QLabel(tr("Halt ramp (bit 8) : Motor stopped"));
    modeControlWordLayout->addRow(vlHaltLabel);
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &P402VlWidget::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &P402VlWidget::pdoMapping);

    QPixmap vlModePixmap;
    QLabel *vlModeLabel;
    vlModeLabel = new QLabel();
    vlModePixmap.load(":/diagram/img/402VLDiagram.png");
    vlModeLabel->setPixmap(vlModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram VL mode"));
    connect(imgPushButton, SIGNAL(clicked()), vlModeLabel, SLOT(show()));
    QHBoxLayout *vlButtonLayout = new QHBoxLayout();
    vlButtonLayout->addWidget(dataLoggerPushButton);
    vlButtonLayout->addWidget(mappingPdoPushButton);
    vlButtonLayout->addWidget(imgPushButton);

    layout->addWidget(vlGroupBox);
    layout->addWidget(modeControlWordGroupBox);
    layout->addItem(vlButtonLayout);

    setLayout(layout);
}

void P402VlWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if ((objId == _vlTargetVelocityObjectId) || (objId == _vlVelocityDemandObjectId)
        || (objId == _vlVelocityActualObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId);
    }
}
