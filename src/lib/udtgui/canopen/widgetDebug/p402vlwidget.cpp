#include "p402vlwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "services/services.h"
#include <QFormLayout>
#include <QPushButton>
#include <QRadioButton>

P402VlWidget::P402VlWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    createWidgets();
    _controlWordObjectId = 0x6040;
    _statusWordObjectId = 0x6041;
    _vlVelocityDemandObjectId = 0x6043;
    _vlVelocityActualObjectId = 0x6044;
    _vlTargetVelocityObjectId = 0x6042;
    _vlVelocityMinMaxAmountObjectId = 0x6046;
    _vlAccelerationObjectId = 0x6048;
    _vlDecelerationObjectId = 0x6049;
    _vlQuickStopObjectId = 0x604A;
    _vlSetPointFactorObjectId = 0x604B;
    _vlDimensionFactorObjectId = 0x604C;
}

P402VlWidget::~P402VlWidget()
{
    unRegisterFullOd();
}

void P402VlWidget::setNode(Node *node)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402VlWidget::updateData);
        }
    }

    registerObjId({_controlWordObjectId, 0x00});
    registerObjId({_vlTargetVelocityObjectId, 0xFF});
    registerObjId({_vlVelocityDemandObjectId, 0xFF});
    registerObjId({_vlVelocityActualObjectId, 0xFF});
    registerObjId({_vlVelocityMinMaxAmountObjectId, 0xFF});
    registerObjId({_vlAccelerationObjectId, 0xFF});
    registerObjId({_vlDecelerationObjectId, 0xFF});
    registerObjId({_vlQuickStopObjectId, 0xFF});
    registerObjId({_vlSetPointFactorObjectId, 0xFF});
    registerObjId({_vlDimensionFactorObjectId, 0xFF});

    setNodeInterrest(node);

    _node = node;
    if (_node)
    {
        connect(_node, &Node::statusChanged, this, &P402VlWidget::updateData);
        updateData();
    }
}

Node *P402VlWidget::node() const
{
    return _node;
}

void P402VlWidget::readData()
{
    if (_node)
    {
        _node->readObject(_vlVelocityDemandObjectId, 0x0);
        _node->readObject(_vlVelocityActualObjectId, 0x0);
    }
}

void P402VlWidget::updateData()
{
    if (_node)
    {
        this->setEnabled(true);
        _node->readObject(_controlWordObjectId, 0x00);
        _node->readObject(_vlTargetVelocityObjectId, 0);
        _node->readObject(_vlVelocityDemandObjectId, 0x0);
        _node->readObject(_vlVelocityActualObjectId, 0x0);
        _node->readObject(_vlVelocityMinMaxAmountObjectId, 1);
        _node->readObject(_vlVelocityMinMaxAmountObjectId, 2);
        _node->readObject(_vlAccelerationObjectId, 1);
        _node->readObject(_vlAccelerationObjectId, 2);
        _node->readObject(_vlDecelerationObjectId, 1);
        _node->readObject(_vlDecelerationObjectId, 2);
        _node->readObject(_vlQuickStopObjectId, 1);
        _node->readObject(_vlQuickStopObjectId, 2);
        _node->readObject(_vlSetPointFactorObjectId, 1);
        _node->readObject(_vlSetPointFactorObjectId, 2);
        _node->readObject(_vlDimensionFactorObjectId, 1);
        _node->readObject(_vlDimensionFactorObjectId, 2);

        _cmdControlWord |= CW_VL_EnableRamp;
        _cmdControlWord |= CW_VL_UnlockRamp;
        _cmdControlWord |= CW_VL_ReferenceRamp;
        _node->writeObject(_controlWordObjectId, 0x00, QVariant(_cmdControlWord));
    }
}

void P402VlWidget::vlTargetVelocitySpinboxFinished()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, 0x00, QVariant(value));
    _vlTargetVelocitySlider->setValue(value);
}
void P402VlWidget::vlTargetVelocitySliderChanged()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, 0x00, QVariant(value));
}
void P402VlWidget::vlMinAmountEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlMinVelocityMinMaxAmountSpinBox->value());
    _node->writeObject(0x6046, 0x01, QVariant(value));
}
void P402VlWidget::vlMaxAmountEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlMaxVelocityMinMaxAmountSpinBox->value());
    _node->writeObject(0x6046, 0x02, QVariant(value));
}
void P402VlWidget::vlAccelerationDeltaSpeedEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlAccelerationDeltaSpeedSpinBox->value());
    _node->writeObject(0x6048, 0x01, QVariant(value));
}
void P402VlWidget::vlAccelerationDeltaTimeEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlAccelerationDeltaTimeSpinBox->value());
    _node->writeObject(0x6048, 0x02, QVariant(value));
}
void P402VlWidget::vlDecelerationDeltaSpeedEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDecelerationDeltaSpeedSpinBox->value());
    _node->writeObject(0x6049, 0x01, QVariant(value));
}
void P402VlWidget::vlDecelerationDeltaTimeEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDecelerationDeltaTimeSpinBox->value());
    _node->writeObject(0x6049, 0x02, QVariant(value));
}
void P402VlWidget::vlQuickStopDeltaSpeedEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlQuickStopDeltaSpeedSpinBox->value());
    _node->writeObject(0x604A, 0x01, QVariant(value));
}
void P402VlWidget::vlQuickStopDeltaTimeEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlQuickStopDeltaTimeSpinBox->value());
    _node->writeObject(0x604A, 0x02, QVariant(value));
}
void P402VlWidget::vlSetPointFactorNumeratorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlSetPointFactorNumeratorSpinBox->value());
    _node->writeObject(0x604B, 0x01, QVariant(value));
}
void P402VlWidget::vlSetPointFactorDenominatorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlSetPointFactorDenominatorSpinBox->value());
    _node->writeObject(0x604B, 0x02, QVariant(value));
}

void P402VlWidget::vlDimensionFactorNumeratorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDimensionFactorNumeratorSpinBox->value());
    _node->writeObject(0x604C, 0x01, QVariant(value));
}
void P402VlWidget::vlDimensionFactorDenominatorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDimensionFactorDenominatorSpinBox->value());
    _node->writeObject(0x604C, 0x02, QVariant(value));
}

void P402VlWidget::vlEnableRampClicked(int id)
{
    if (id == 0)
    {
        _cmdControlWord = (_cmdControlWord & ~ControlWordVL::CW_VL_EnableRamp);
    }
    else if (id == 1)
    {
        _cmdControlWord |= CW_VL_EnableRamp;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(_cmdControlWord));
}

void P402VlWidget::vlUnlockRampClicked(int id)
{
    if (id == 0)
    {
        _cmdControlWord = (_cmdControlWord & ~ControlWordVL::CW_VL_UnlockRamp);
    }
    else if (id == 1)
    {
        _cmdControlWord |= CW_VL_UnlockRamp;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(_cmdControlWord));
}

void P402VlWidget::vlReferenceRampClicked(int id)
{
    if (id == 0)
    {
        _cmdControlWord = (_cmdControlWord & ~ControlWordVL::CW_VL_ReferenceRamp);
    }
    else if (id == 1)
    {
        _cmdControlWord |= CW_VL_ReferenceRamp;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(_cmdControlWord));
}

void P402VlWidget::vlHaltClicked(int id)
{
    if (id == 0)
    {
        _cmdControlWord = (_cmdControlWord & ~CW_Halt);
    }
    else if (id == 1)
    {
        _cmdControlWord |= CW_Halt;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(_cmdControlWord));
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
    _vlMinVelocityMinMaxAmountSpinBox = new QSpinBox();
    _vlMinVelocityMinMaxAmountSpinBox->setToolTip("min ");
    _vlMinVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMinVelocityMinMaxAmountSpinBox);
    _vlMaxVelocityMinMaxAmountSpinBox = new QSpinBox();
    _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("max ");
    _vlMaxVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMaxVelocityMinMaxAmountSpinBox);
    vlLayout->addRow(vlVelocityMinMaxAmountlayout);
    connect(_vlMinVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlMinAmountEditingFinished);
    connect(_vlMaxVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlMaxAmountEditingFinished);

    QLabel *vlVelocityAccelerationLabel = new QLabel(tr("Velocity acceleration (0x6048) :"));
    vlLayout->addRow(vlVelocityAccelerationLabel);
    QLayout *vlVelocityAccelerationlayout = new QHBoxLayout();
    _vlAccelerationDeltaSpeedSpinBox = new QSpinBox();
    _vlAccelerationDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlAccelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlAccelerationDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaSpeedSpinBox);
    _vlAccelerationDeltaTimeSpinBox = new QSpinBox();
    _vlAccelerationDeltaTimeSpinBox->setSuffix(" ms");
    _vlAccelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlAccelerationDeltaTimeSpinBox->setRange(1, std::numeric_limits<quint16>::max());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityAccelerationlayout);
    connect(_vlAccelerationDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlAccelerationDeltaSpeedEditingFinished);
    connect(_vlAccelerationDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlAccelerationDeltaTimeEditingFinished);

    QLabel *vlVelocityDecelerationLabel = new QLabel(tr("Velocity deceleration (0x6049) :"));
    vlLayout->addRow(vlVelocityDecelerationLabel);
    QLayout *vlVelocityDecelerationlayout = new QHBoxLayout();
    _vlDecelerationDeltaSpeedSpinBox = new QSpinBox();
    _vlDecelerationDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlDecelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlDecelerationDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaSpeedSpinBox);
    _vlDecelerationDeltaTimeSpinBox = new QSpinBox();
    _vlDecelerationDeltaTimeSpinBox->setSuffix(" ms");
    _vlDecelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlDecelerationDeltaTimeSpinBox->setRange(1, std::numeric_limits<quint16>::max());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityDecelerationlayout);
    connect(_vlDecelerationDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlDecelerationDeltaSpeedEditingFinished);
    connect(_vlDecelerationDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlDecelerationDeltaTimeEditingFinished);

    QLabel *vlVelocityQuickStopLabel = new QLabel(tr("Velocity quick stop (0x604A) :"));
    vlLayout->addRow(vlVelocityQuickStopLabel);
    QLayout *vlVelocityQuickStoplayout = new QHBoxLayout();
    _vlQuickStopDeltaSpeedSpinBox = new QSpinBox();
    _vlQuickStopDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlQuickStopDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlQuickStopDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaSpeedSpinBox);
    _vlQuickStopDeltaTimeSpinBox = new QSpinBox();
    _vlQuickStopDeltaTimeSpinBox->setSuffix(" ms");
    _vlQuickStopDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlQuickStopDeltaTimeSpinBox->setRange(1, std::numeric_limits<quint16>::max());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityQuickStoplayout);
    connect(_vlQuickStopDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlQuickStopDeltaSpeedEditingFinished);
    connect(_vlQuickStopDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlQuickStopDeltaTimeEditingFinished);

    QLabel *vlSetPointFactorLabel = new QLabel(tr("Set-point factor (0x604B) :"));
    vlLayout->addRow(vlSetPointFactorLabel);
    QLayout *vlSetPointFactorlayout = new QHBoxLayout();
    _vlSetPointFactorNumeratorSpinBox = new QSpinBox();
    _vlSetPointFactorNumeratorSpinBox->setToolTip("Numerator");
    _vlSetPointFactorNumeratorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorNumeratorSpinBox);
    _vlSetPointFactorDenominatorSpinBox = new QSpinBox();
    _vlSetPointFactorDenominatorSpinBox->setToolTip("Denominator");
    _vlSetPointFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorDenominatorSpinBox);
    vlLayout->addRow(vlSetPointFactorlayout);
    connect(_vlSetPointFactorNumeratorSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlSetPointFactorNumeratorEditingFinished);
    connect(_vlSetPointFactorDenominatorSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlSetPointFactorDenominatorEditingFinished);

    QLabel *vlDimensionFactorLabel = new QLabel(tr("Dimension factor (0x604C) :"));
    vlLayout->addRow(vlDimensionFactorLabel);
    QLayout *vlDimensionFactorlayout = new QHBoxLayout();
    _vlDimensionFactorNumeratorSpinBox = new QSpinBox();
    _vlDimensionFactorNumeratorSpinBox->setToolTip("Numerator");
    _vlDimensionFactorNumeratorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorNumeratorSpinBox);
    _vlDimensionFactorDenominatorSpinBox = new QSpinBox();
    _vlDimensionFactorDenominatorSpinBox->setToolTip("Denominator");
    _vlDimensionFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorDenominatorSpinBox);
    vlLayout->addRow(vlDimensionFactorlayout);
    connect(_vlDimensionFactorNumeratorSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlDimensionFactorNumeratorEditingFinished);
    connect(_vlDimensionFactorDenominatorSpinBox, &QSpinBox::editingFinished, this, &P402VlWidget::vlDimensionFactorDenominatorEditingFinished);

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
    connect(_vlEnableRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { vlEnableRampClicked(id); });

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
    connect(_vlUnlockRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { vlUnlockRampClicked(id); });

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
    connect(_vlReferenceRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { vlReferenceRampClicked(id); });

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

void P402VlWidget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData({_node->busId(), _node->nodeId(), _vlVelocityActualObjectId, 0x0, QMetaType::Type::Short});
    dataLogger->addData({_node->busId(), _node->nodeId(), _vlTargetVelocityObjectId, 0x0, QMetaType::Type::Short});
    dataLogger->addData({_node->busId(), _node->nodeId(), _vlVelocityDemandObjectId, 0x0, QMetaType::Type::Short});
    _dataLoggerWidget->show();
}

void P402VlWidget::pdoMapping()
{
    QList<NodeObjectId> vlRpdoObjectList = {{_node->busId(), _node->nodeId(), _controlWordObjectId, 0x0, QMetaType::Type::UShort},
                                            {_node->busId(), _node->nodeId(), _vlTargetVelocityObjectId, 0x0, QMetaType::Type::Short}};

    _node->rpdos().at(0)->writeMapping(vlRpdoObjectList);
    QList<NodeObjectId> vlTpdoObjectList = {{_node->busId(), _node->nodeId(), _statusWordObjectId, 0x0, QMetaType::Type::UShort},
                                            {_node->busId(), _node->nodeId(), _vlVelocityDemandObjectId, 0x0, QMetaType::Type::Short}};

    _node->tpdos().at(2)->writeMapping(vlTpdoObjectList);
}

void P402VlWidget::manageNotificationControlWordObject(SDO::FlagsRequest flags)
{
    if (flags == SDO::FlagsRequest::Error)
    {
    }
    quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toInt());
    _cmdControlWord = controlWord;

    if (_vlEnableRampButtonGroup->button((controlWord & CW_VL_EnableRamp) > 4))
    {
        _vlEnableRampButtonGroup->button((controlWord & CW_VL_EnableRamp) > 4)->setChecked(true);
    }
    if (_vlUnlockRampButtonGroup->button((controlWord & CW_VL_UnlockRamp) > 5))
    {
        _vlUnlockRampButtonGroup->button((controlWord & CW_VL_UnlockRamp) > 5)->setChecked(true);
    }
    if (_vlReferenceRampButtonGroup->button((controlWord & CW_VL_ReferenceRamp) > 6))
    {
        _vlReferenceRampButtonGroup->button((controlWord & CW_VL_ReferenceRamp) > 6)->setChecked(true);
    }

    if ((controlWord & CW_VL_EnableRamp) &&  (controlWord & CW_VL_UnlockRamp) && (controlWord & CW_VL_ReferenceRamp))
    {
        _vlMinVelocityMinMaxAmountSpinBox->setEnabled(true);
        _vlMinVelocityMinMaxAmountSpinBox->setToolTip("min ");

        _vlMaxVelocityMinMaxAmountSpinBox->setEnabled(true);
        _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("max ");

        _vlAccelerationDeltaSpeedSpinBox->setEnabled(true);
        _vlAccelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");

        _vlAccelerationDeltaTimeSpinBox->setEnabled(true);
        _vlAccelerationDeltaTimeSpinBox->setToolTip("Delta Time");

        _vlDecelerationDeltaSpeedSpinBox->setEnabled(true);
        _vlDecelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
        _vlDecelerationDeltaTimeSpinBox->setEnabled(true);
        _vlDecelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    }
    else
    {
        _vlMinVelocityMinMaxAmountSpinBox->setEnabled(false);
        _vlMinVelocityMinMaxAmountSpinBox->setToolTip("Deactive because one bit (4,5,6) of ControlWord is deactivate");
        _vlMaxVelocityMinMaxAmountSpinBox->setEnabled(false);
        _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("Deactive because one bit (4,5,6) of ControlWord is deactivate");
        _vlAccelerationDeltaSpeedSpinBox->setEnabled(false);
        _vlAccelerationDeltaSpeedSpinBox->setToolTip("Deactive because one bit (4,5,6) of ControlWord is deactivate");
        _vlAccelerationDeltaTimeSpinBox->setEnabled(false);
        _vlAccelerationDeltaTimeSpinBox->setToolTip("Deactive because one bit (4,5,6) of ControlWord is deactivate");
        _vlDecelerationDeltaSpeedSpinBox->setEnabled(false);
        _vlDecelerationDeltaSpeedSpinBox->setToolTip("Deactive because one bit (4,5,6) of ControlWord is deactivate");
        _vlDecelerationDeltaTimeSpinBox->setEnabled(false);
        _vlDecelerationDeltaTimeSpinBox->setToolTip("Deactive because one bit (4,5,6) of ControlWord is deactivate");
    }
}

void P402VlWidget::refreshData(quint16 object)
{
    int value;
    if (_node->nodeOd()->indexExist(object))
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
        if (object == _vlVelocityMinMaxAmountObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _vlMinVelocityMinMaxAmountSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _vlMaxVelocityMinMaxAmountSpinBox->setValue(value);
            _vlTargetVelocitySlider->setMinimum(-value);
            _vlTargetVelocitySlider->setMaximum(value);
        }
        if (object == _vlAccelerationObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _vlAccelerationDeltaSpeedSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _vlAccelerationDeltaTimeSpinBox->setValue(value);
        }
        if (object == _vlDecelerationObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _vlDecelerationDeltaSpeedSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _vlDecelerationDeltaTimeSpinBox->setValue(value);
        }
        if (object == _vlQuickStopObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _vlQuickStopDeltaSpeedSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _vlQuickStopDeltaTimeSpinBox->setValue(value);
        }

        if (object == _vlSetPointFactorObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _vlSetPointFactorNumeratorSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _vlSetPointFactorDenominatorSpinBox->setValue(value);
        }
        if (object == _vlDimensionFactorObjectId)
        {
            value = _node->nodeOd()->value(object, 1).toInt();
            _vlDimensionFactorNumeratorSpinBox->setValue(value);
            value = _node->nodeOd()->value(object, 2).toInt();
            _vlDimensionFactorDenominatorSpinBox->setValue(value);
        }
    }
}

void P402VlWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!_node)
    {
        return;
    }

    if (objId.index == _controlWordObjectId && objId.subIndex == 0x00)
    {
        manageNotificationControlWordObject(flags);
    }

    if ((objId.index == _vlTargetVelocityObjectId)
        || (objId.index == _vlVelocityDemandObjectId)
        || (objId.index == _vlVelocityActualObjectId)
        || (objId.index == _vlVelocityMinMaxAmountObjectId)
        || (objId.index == _vlAccelerationObjectId)
        || (objId.index == _vlDecelerationObjectId)
        || (objId.index == _vlQuickStopObjectId)
        || (objId.index == _vlSetPointFactorObjectId)
        || (objId.index == _vlDimensionFactorObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId.index);
    }
}
