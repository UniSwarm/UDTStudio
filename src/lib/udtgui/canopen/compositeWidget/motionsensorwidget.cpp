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

#include "motionsensorwidget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexcheckbox.h"
#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "indexdb402.h"
#include "node.h"
#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QWidget>

const int indentLabel = 18;

MotionSensorWidget::MotionSensorWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    _nodeProfile402 = nullptr;
    _dataLogger = new DataLogger();

    createWidgets();
    _state = NONE;
    _mode = MODE_SENSOR_NONE;
}

Node *MotionSensorWidget::node() const
{
    return _node;
}

QString MotionSensorWidget::title() const
{
    switch (_mode)
    {
        case MODE_SENSOR_NONE:
            return tr("None");

        case MODE_SENSOR_POSITION:
            return tr("Position sensor");

        case MODE_SENSOR_VELOCITY:
            return tr("Velocity sensor");

        case MODE_SENSOR_TORQUE:
            return tr("Torque sensor");
    }
    return QString();
}

void MotionSensorWidget::setNode(Node *node, uint8_t axis)
{
    _node = node;
    if (_node == nullptr)
    {
        return;
    }

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    _dataLogger->removeAllData();

    connect(_node, &Node::statusChanged, this, &MotionSensorWidget::statusNodeChanged);
    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);
        connect(_nodeProfile402, &NodeProfile402::stateChanged, this, &MotionSensorWidget::stateChanged);
    }
    setIMode();

    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }
}

void MotionSensorWidget::setMode(MotionSensorWidget::ModeSensor mode)
{
    _mode = mode;
    setIMode();
}

void MotionSensorWidget::setLogTimer(int ms)
{
    if (_dataLogger->isStarted())
    {
        _dataLogger->start(ms);
    }
}

void MotionSensorWidget::setIMode()
{
    NodeObjectId rawDataValueLabel_ObjId;
    NodeObjectId flagLabel_ObjId;
    NodeObjectId valueLabel_ObjId;

    if (_nodeProfile402 == nullptr)
    {
        return;
    }

    IndexDb402::OdMode402 odMode402 = IndexDb402::MODE402_TORQUE;

    switch (_mode)
    {
        case MODE_SENSOR_NONE:
            break;

        case MODE_SENSOR_TORQUE:
            _sensorConfigGroupBox->setTitle(tr("Torque sensor configuration"));
            _filterGroupBox->setTitle(tr("Torque filter"));
            _conditioningGroupBox->setTitle(tr("Torque conditioning"));
            _statusGroupBox->setTitle(tr("Torque sensor status"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 torque motion sensor").arg(_node->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_TORQUE;
            break;

        case MODE_SENSOR_VELOCITY:
            _sensorConfigGroupBox->setTitle(tr("Velocity sensor configuration"));
            _filterGroupBox->setTitle(tr("Velocity filter"));
            _conditioningGroupBox->setTitle(tr("Velocity conditioning"));
            _statusGroupBox->setTitle(tr("Velocity sensor status"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 velocity motion sensor").arg(_node->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_VELOCITY;
            break;

        case MODE_SENSOR_POSITION:
            _sensorConfigGroupBox->setTitle(tr("Position sensor configuration"));
            _filterGroupBox->setTitle(tr("Position filter"));
            _conditioningGroupBox->setTitle(tr("Position conditioning"));
            _statusGroupBox->setTitle(tr("Position sensor status"));
            _dataLoggerWidget->setTitle(tr("Node %1 axis %2 position motion sensor").arg(_node->nodeId()).arg(_axis));
            odMode402 = IndexDb402::MODE402_POSITION;
            break;
    }
    fillSensorSelect();

    // Config
    _sensorSelectComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_SELECT, _axis, odMode402));
    _frequencyDividerSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FREQUENCY_DIVIDER, _axis, odMode402));
    _configBitCheckBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_CONFIG_BIT, _axis, odMode402));
    _sensorParam0SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_0, _axis, odMode402));
    _sensorParam1SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_1, _axis, odMode402));
    _sensorParam2SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_2, _axis, odMode402));
    _sensorParam3SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PARAM_3, _axis, odMode402));

    // Filter
    _filterSelectComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_SELECT, _axis, odMode402));
    _filterParam0SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_0, _axis, odMode402));
    _filterParam1SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_1, _axis, odMode402));
    _filterParam2SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_2, _axis, odMode402));
    _filterParam3SpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_FILTER_PARAM_3, _axis, odMode402));

    // Conditioning
    _thresholdMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_THRESHOLD_MIN, _axis, odMode402));
    _thresholdMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_THRESHOLD_MAX, _axis, odMode402));
    _thresholdModeComboBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_THRESHOLD_MODE, _axis, odMode402));
    _preOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_PRE_OFFSET, _axis, odMode402));
    _scaleSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_SCALE, _axis, odMode402));
    _postOffsetSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_POST_OFFSET, _axis, odMode402));
    _errorMinSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_ERROR_MIN, _axis, odMode402));
    _errorMaxSpinBox->setObjId(IndexDb402::getObjectId(IndexDb402::OD_SENSOR_ERROR_MAX, _axis, odMode402));

    rawDataValueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_SENSOR_STATUS_RAW_DATA, _axis, odMode402);
    flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_SENSOR_STATUS_FLAGS, _axis, odMode402);
    valueLabel_ObjId = IndexDb402::getObjectId(IndexDb402::OD_SENSOR_STATUS_VALUE, _axis, odMode402);

    // Status
    _rawDataValueLabel->setObjId(rawDataValueLabel_ObjId);
    _flagLabel->setObjId(flagLabel_ObjId);
    _valueLabel->setObjId(valueLabel_ObjId);
    rawDataValueLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    flagLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    valueLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());

    // Datalogger
    _dataLogger->removeAllData();
    _dataLogger->addData(rawDataValueLabel_ObjId);
    _dataLogger->data(0)->setActive(false);
    _dataLogger->addData(valueLabel_ObjId);
}

void MotionSensorWidget::fillSensorSelect()
{
    _sensorSelectComboBox->clear();

    _sensorSelectComboBox->addItem(tr("OFF"), QVariant(static_cast<uint16_t>(0x0000)));
    _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("No source sensor selected"), Qt::StatusTipRole);
    _sensorSelectComboBox->insertSeparator(_sensorSelectComboBox->count());

    _sensorSelectComboBox->addItem(tr("Motor data"));
    dynamic_cast<QStandardItemModel *>(_sensorSelectComboBox->model())->item(_sensorSelectComboBox->count() - 1)->setEnabled(false);
    if (_mode == MODE_SENSOR_TORQUE)
    {
        _sensorSelectComboBox->addItem(tr("Motor torque"), QVariant(static_cast<uint16_t>(0x1100)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Torque computed from motor current and Ki constant"), Qt::StatusTipRole);
    }
    if (_mode == MODE_SENSOR_VELOCITY)
    {
        _sensorSelectComboBox->addItem(tr("Motor velocity"), QVariant(static_cast<uint16_t>(0x1200)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Velocity computed from motor bach EMF or hall sensor"), Qt::StatusTipRole);
    }
    if (_mode == MODE_SENSOR_POSITION)
    {
        _sensorSelectComboBox->addItem(tr("Motor position"), QVariant(static_cast<uint16_t>(0x1300)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Position computed from motor hall"), Qt::StatusTipRole);
    }
    _sensorSelectComboBox->insertSeparator(_sensorSelectComboBox->count());

    if (_mode == MODE_SENSOR_VELOCITY)
    {
        _sensorSelectComboBox->addItem(tr("From another sensor"));
        dynamic_cast<QStandardItemModel *>(_sensorSelectComboBox->model())->item(_sensorSelectComboBox->count() - 1)->setEnabled(false);
        // _sensorSelectComboBox->addItem(tr("Position from velocity"), QVariant(static_cast<uint16_t>(0x2200)));
        // _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Velocity derived from the position sensor"), Qt::StatusTipRole);
        _sensorSelectComboBox->addItem(tr("Velocity from position"), QVariant(static_cast<uint16_t>(0x2300)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Integrated position from the velocity sensor"), Qt::StatusTipRole);
        _sensorSelectComboBox->insertSeparator(_sensorSelectComboBox->count());
    }

    if (_mode == MODE_SENSOR_POSITION)
    {
        _sensorSelectComboBox->addItem(tr("Encoders"));
        dynamic_cast<QStandardItemModel *>(_sensorSelectComboBox->model())->item(_sensorSelectComboBox->count() - 1)->setEnabled(false);
        _sensorSelectComboBox->addItem(tr("QEI CH1"), QVariant(static_cast<uint16_t>(0x3101)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Quadrature incremental encoder channel 1"), Qt::StatusTipRole);
        _sensorSelectComboBox->addItem(tr("QEI CH2"), QVariant(static_cast<uint16_t>(0x3102)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Quadrature incremental encoder channel 2"), Qt::StatusTipRole);
        _sensorSelectComboBox->addItem(tr("SSI CH1"), QVariant(static_cast<uint16_t>(0x3201)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("SSI absolute encoder channel 1"), Qt::StatusTipRole);
        _sensorSelectComboBox->addItem(tr("SSI CH2"), QVariant(static_cast<uint16_t>(0x3202)));
        _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("SSI absolute encoder channel 2"), Qt::StatusTipRole);
        _sensorSelectComboBox->insertSeparator(_sensorSelectComboBox->count());
    }

    _sensorSelectComboBox->addItem(tr("Analog inputs"));
    dynamic_cast<QStandardItemModel *>(_sensorSelectComboBox->model())->item(_sensorSelectComboBox->count() - 1)->setEnabled(false);
    _sensorSelectComboBox->addItem(tr("AN1"), QVariant(static_cast<uint16_t>(0x4001)));
    _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Analog input channel 1"), Qt::StatusTipRole);
    _sensorSelectComboBox->addItem(tr("AN2"), QVariant(static_cast<uint16_t>(0x4002)));
    _sensorSelectComboBox->setItemData(_sensorSelectComboBox->count() - 1, tr("Analog input channel 2"), Qt::StatusTipRole);
}

void MotionSensorWidget::createWidgets()
{
    QWidget *motionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(motionSensorWidget);
    actionLayout->setContentsMargins(0, 0, 4, 0);
    actionLayout->setSpacing(0);

    _sensorConfigGroupBox = createSensorConfigurationWidgets();
    actionLayout->addWidget(_sensorConfigGroupBox);
    _filterGroupBox = createSensorFilterWidgets();
    actionLayout->addWidget(_filterGroupBox);
    _conditioningGroupBox = createSensorConditioningWidgets();
    actionLayout->addWidget(_conditioningGroupBox);
    _statusGroupBox = createSensorStatusWidgets();
    actionLayout->addWidget(_statusGroupBox);

    QScrollArea *motionSensorScrollArea = new QScrollArea;
    motionSensorScrollArea->setWidget(motionSensorWidget);
    motionSensorScrollArea->setWidgetResizable(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter {background: #19232D;}");
    splitter->addWidget(motionSensorScrollArea);

    QWidget *widgetLogger = new QWidget();
    QVBoxLayout *layoutLogger = new QVBoxLayout();
    layoutLogger->setContentsMargins(4, 4, 0, 0);
    _dataLoggerWidget = new DataLoggerWidget(_dataLogger, Qt::Vertical);
    _dataLoggerWidget->setType(DataLoggerWidget::InternalType);
    _dataLoggerWidget->chartView()->setRollingTimeMs(10000);
    _dataLoggerWidget->chartView()->setRollingEnabled(true);
    layoutLogger->addWidget(_dataLoggerWidget);
    widgetLogger->setLayout(layoutLogger);
    splitter->addWidget(widgetLogger);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->setSpacing(0);
    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2, 2, 2, 0);
    toolBarLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addItem(toolBarLayout);
    vBoxLayout->addWidget(splitter);
    setLayout(vBoxLayout);
}

QToolBar *MotionSensorWidget::createToolBarWidgets()
{
    // toolbar
    QToolBar *toolBar = new QToolBar(tr("Data logger commands"));
    toolBar->setIconSize(QSize(20, 20));

    // start stop
    toolBar->addAction(_dataLoggerWidget->managerWidget()->startStopAction());

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int i)
            {
                setLogTimer(i);
            });

    // clear
    QAction *action;
    action = toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _dataLogger, &DataLogger::clear);

    toolBar->addSeparator();

    // lock / unlock action
    _lockAction = toolBar->addAction(tr("Lock/unlock config objects"));
    _lockAction->setEnabled(false);
    _lockAction->setCheckable(true);
    QIcon iconLockUnlock;
    iconLockUnlock.addFile(":/icons/img/icons8-lock.png", QSize(), QIcon::Normal, QIcon::On);
    iconLockUnlock.addFile(":/icons/img/icons8-unlock.png", QSize(), QIcon::Normal, QIcon::Off);
    _lockAction->setIcon(iconLockUnlock);
    _lockAction->setShortcut(QKeySequence("Ctrl+L"));
    _lockAction->setStatusTip(tr("Editing of config parameters is not possible in OE mode, goto to SO to unlock"));
    connect(_lockAction, &QAction::triggered, this, &MotionSensorWidget::lockUnlockConfig);

    toolBar->addSeparator();

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-update.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &MotionSensorWidget::readAllObject);

    QWidget *spacerWidget = new QLabel(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacerWidget->setStyleSheet("QLabel {background: none}");
    spacerWidget->setVisible(true);
    toolBar->addWidget(spacerWidget);

    QSlider *_motionTargetSlidder = new QSlider(Qt::Horizontal);
    toolBar->addWidget(_motionTargetSlidder);

    // revert color style for QSlider in toolbar
    setStyleSheet("QSlider {background-color: #32414B;}\
                   QSlider::groove:horizontal {background-color: #19232D;}");

    return toolBar;
}

QGroupBox *MotionSensorWidget::createSensorConfigurationWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor config"));
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(3);
    formLayout->setHorizontalSpacing(3);

    _sensorSelectComboBox = new IndexComboBox();
    formLayout->addRow(tr("&Sensor select:"), _sensorSelectComboBox);
    connect(_sensorSelectComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MotionSensorWidget::updateSensorParams);
    _indexWidgets.append(_sensorSelectComboBox);

    _sensorParam0SpinBox = new IndexSpinBox();
    _sensorParamLabels.append(new QLabel(tr("param 0:")));
    _sensorParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_sensorParamLabels.last(), _sensorParam0SpinBox);
    _indexWidgets.append(_sensorParam0SpinBox);

    _sensorParam1SpinBox = new IndexSpinBox();
    _sensorParamLabels.append(new QLabel(tr("param 1:")));
    _sensorParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_sensorParamLabels.last(), _sensorParam1SpinBox);
    _indexWidgets.append(_sensorParam1SpinBox);

    _sensorParam2SpinBox = new IndexSpinBox();
    _sensorParamLabels.append(new QLabel(tr("param 2:")));
    _sensorParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_sensorParamLabels.last(), _sensorParam2SpinBox);
    _indexWidgets.append(_sensorParam2SpinBox);

    _sensorParam3SpinBox = new IndexSpinBox();
    _sensorParamLabels.append(new QLabel(tr("param 3:")));
    _sensorParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_sensorParamLabels.last(), _sensorParam3SpinBox);
    _indexWidgets.append(_sensorParam3SpinBox);

    formLayout->addItem(new QSpacerItem(0, 5));

    _frequencyDividerSpinBox = new IndexSpinBox();
    _frequencyDividerSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    _frequencyDividerSpinBox->setRangeValue(1, 1000);
    formLayout->addRow(tr("S&ubsampling:"), _frequencyDividerSpinBox);
    _indexWidgets.append(_frequencyDividerSpinBox);

    _configBitCheckBox = new IndexCheckBox();
    _configBitCheckBox->setBitMask(32);
    formLayout->addRow(tr("&Q15.16 (<<16):"), _configBitCheckBox);
    _indexWidgets.append(_configBitCheckBox);

    updateSensorParams(0);
    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorFilterWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor filter"));
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(3);
    formLayout->setHorizontalSpacing(3);

    _filterSelectComboBox = new IndexComboBox();
    _filterSelectComboBox->addItem(tr("OFF"), QVariant(static_cast<uint16_t>(0x0000)));
    _filterSelectComboBox->setItemData(_filterSelectComboBox->count() - 1, tr("No filter active, direct raw value"), Qt::StatusTipRole);
    _filterSelectComboBox->addItem(tr("Low pass"), QVariant(static_cast<uint16_t>(0x1000)));
    _filterSelectComboBox->setItemData(_filterSelectComboBox->count() - 1, tr("Low pass filter"), Qt::StatusTipRole);
    _filterSelectComboBox->addItem(tr("Averaging"), QVariant(static_cast<uint16_t>(0x2000)));
    _filterSelectComboBox->setItemData(_filterSelectComboBox->count() - 1, tr("Averaging value from 'N' samples"), Qt::StatusTipRole);
    connect(_filterSelectComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MotionSensorWidget::updateFilterParams);
    formLayout->addRow(tr("&Filter select:"), _filterSelectComboBox);
    _indexWidgets.append(_filterSelectComboBox);

    _filterParam0SpinBox = new IndexSpinBox();
    _filterParam0SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _filterParamLabels.append(new QLabel(tr("param 0:")));
    _filterParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_filterParamLabels.last(), _filterParam0SpinBox);
    _indexWidgets.append(_filterParam0SpinBox);

    _filterParam1SpinBox = new IndexSpinBox();
    _filterParam1SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _filterParamLabels.append(new QLabel(tr("param 1:")));
    _filterParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_filterParamLabels.last(), _filterParam1SpinBox);
    _indexWidgets.append(_filterParam1SpinBox);

    _filterParam2SpinBox = new IndexSpinBox();
    _filterParam2SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _filterParamLabels.append(new QLabel(tr("param 2:")));
    _filterParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_filterParamLabels.last(), _filterParam2SpinBox);
    _indexWidgets.append(_filterParam2SpinBox);

    _filterParam3SpinBox = new IndexSpinBox();
    _filterParam3SpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _filterParamLabels.append(new QLabel(tr("param 3:")));
    _filterParamLabels.last()->setIndent(indentLabel);
    formLayout->addRow(_filterParamLabels.last(), _filterParam3SpinBox);
    _indexWidgets.append(_filterParam3SpinBox);

    updateFilterParams(0);
    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorConditioningWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor conditioning"));
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(3);
    formLayout->setHorizontalSpacing(3);

    _preOffsetSpinBox = new IndexSpinBox();
    _preOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Pre &offset:"), _preOffsetSpinBox);
    _indexWidgets.append(_preOffsetSpinBox);

    _scaleSpinBox = new IndexSpinBox();
    _scaleSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("S&cale:"), _scaleSpinBox);
    _indexWidgets.append(_scaleSpinBox);

    _postOffsetSpinBox = new IndexSpinBox();
    _postOffsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("&Post offset:"), _postOffsetSpinBox);
    _indexWidgets.append(_postOffsetSpinBox);

    formLayout->addItem(new QSpacerItem(0, 6));

    QLayout *errorRangelayout = new QHBoxLayout();
    errorRangelayout->setSpacing(0);
    _errorMinSpinBox = new IndexSpinBox();
    _errorMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    errorRangelayout->addWidget(_errorMinSpinBox);
    _indexWidgets.append(_errorMinSpinBox);
    QLabel *errorRangeSepLabel = new QLabel(tr("-"));
    errorRangeSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    errorRangelayout->addWidget(errorRangeSepLabel);
    errorRangeSepLabel->setIndent(2);
    _errorMaxSpinBox = new IndexSpinBox();
    _errorMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    errorRangelayout->addWidget(_errorMaxSpinBox);
    _indexWidgets.append(_errorMaxSpinBox);
    QLabel *errorRangeLabel = new QLabel(tr("&Error range:"));
    errorRangeLabel->setBuddy(_errorMinSpinBox);
    formLayout->addRow(errorRangeLabel, errorRangelayout);

    formLayout->addItem(new QSpacerItem(0, 6));

    _thresholdModeComboBox = new IndexComboBox();
    _thresholdModeComboBox->addItem(tr("No threshold"), QVariant(static_cast<uint16_t>(0x0000)));
    _thresholdModeComboBox->setItemData(_thresholdModeComboBox->count() - 1, tr("No threshold active, direct value"), Qt::StatusTipRole);
    _thresholdModeComboBox->addItem(tr("Min-max"), QVariant(static_cast<uint16_t>(0x0001)));
    _thresholdModeComboBox->setItemData(_thresholdModeComboBox->count() - 1, tr("Limit output range from min to max"), Qt::StatusTipRole);
    _thresholdModeComboBox->addItem(tr("Modulo"), QVariant(static_cast<uint16_t>(0x0002)));
    _thresholdModeComboBox->setItemData(_thresholdModeComboBox->count() - 1, tr("Apply a modulo to keep value in range from min to max"), Qt::StatusTipRole);
    _thresholdModeComboBox->addItem(tr("Last valid value"), QVariant(static_cast<uint16_t>(0x0003)));
    _thresholdModeComboBox->setItemData(_thresholdModeComboBox->count() - 1, tr("Keep the last valid value as output"), Qt::StatusTipRole);
    connect(_thresholdModeComboBox,
            qOverload<int>(&QComboBox::currentIndexChanged),
            [=](int index)
            {
                _thresholdMinSpinBox->setEnabled(index != 0);
                _thresholdMaxSpinBox->setEnabled(index != 0);
            });
    formLayout->addRow(tr("Th&reshold:"), _thresholdModeComboBox);
    _indexWidgets.append(_thresholdModeComboBox);

    QLayout *thresholdlayout = new QHBoxLayout();
    thresholdlayout->setSpacing(0);
    _thresholdMinSpinBox = new IndexSpinBox();
    _thresholdMinSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _thresholdMinSpinBox->setEnabled(false);
    thresholdlayout->addWidget(_thresholdMinSpinBox);
    _indexWidgets.append(_thresholdMinSpinBox);
    QLabel *thresholdSepLabel = new QLabel(tr("-"));
    thresholdSepLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    thresholdSepLabel->setIndent(2);
    thresholdlayout->addWidget(thresholdSepLabel);
    _thresholdMaxSpinBox = new IndexSpinBox();
    _thresholdMaxSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    _thresholdMaxSpinBox->setEnabled(false);
    thresholdlayout->addWidget(_thresholdMaxSpinBox);
    _indexWidgets.append(_thresholdMaxSpinBox);
    QLabel *thresholdLabel = new QLabel();
    thresholdLabel->setBuddy(_thresholdMinSpinBox);
    formLayout->addRow(thresholdLabel, thresholdlayout);
    _indexWidgets.append(_filterParam3SpinBox);

    groupBox->setLayout(formLayout);
    return groupBox;
}

QGroupBox *MotionSensorWidget::createSensorStatusWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Sensor status"));
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(3);
    formLayout->setHorizontalSpacing(3);

    _rawDataValueLabel = new IndexLabel();
    _rawDataValueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Raw data:"), _rawDataValueLabel);
    _indexWidgets.append(_rawDataValueLabel);

    _flagLabel = new IndexLabel();
    formLayout->addRow(tr("Flag:"), _flagLabel);
    _indexWidgets.append(_flagLabel);

    _valueLabel = new IndexLabel();
    _valueLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    formLayout->addRow(tr("Value:"), _valueLabel);
    _indexWidgets.append(_valueLabel);

    groupBox->setLayout(formLayout);

    return groupBox;
}

void MotionSensorWidget::lockUnlockConfig()
{
    if (_lockAction->isChecked())
    {
        _nodeProfile402->goToState(NodeProfile402::STATE_OperationEnabled);
    }
    else
    {
        _nodeProfile402->goToState(NodeProfile402::STATE_SwitchedOn);
    }
    _lockAction->blockSignals(true);
    _lockAction->setChecked(!_lockAction->isChecked());  // Keep action in the previous state
    _lockAction->blockSignals(false);
}

void MotionSensorWidget::statusNodeChanged(Node::Status status)
{
    if (status == Node::STOPPED)
    {
        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }
}

void MotionSensorWidget::stateChanged()
{
    _lockAction->blockSignals(true);
    if (_nodeProfile402->currentState() == NodeProfile402::STATE_OperationEnabled)
    {
        _sensorConfigGroupBox->setEnabled(false);
        _filterGroupBox->setEnabled(false);
        _conditioningGroupBox->setEnabled(false);
        _lockAction->setChecked(true);
    }
    else
    {
        _sensorConfigGroupBox->setEnabled(true);
        _filterGroupBox->setEnabled(true);
        _conditioningGroupBox->setEnabled(true);
        _lockAction->setChecked(false);
    }
    _lockAction->blockSignals(false);
    _lockAction->setEnabled((_nodeProfile402->status() == NodeProfile402::NODEPROFILE_STARTED));
}

void MotionSensorWidget::readAllObject()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void MotionSensorWidget::updateSensorParams(int index)
{
    Q_UNUSED(index)

    _sensorParamLabels.at(0)->setText(tr("param 0:"));
    _sensorParamLabels.at(1)->setText(tr("param 1:"));
    _sensorParamLabels.at(2)->setText(tr("param 2:"));
    _sensorParamLabels.at(3)->setText(tr("param 3:"));
    _sensorParamLabels.at(0)->setEnabled(false);
    _sensorParamLabels.at(1)->setEnabled(false);
    _sensorParamLabels.at(2)->setEnabled(false);
    _sensorParamLabels.at(3)->setEnabled(false);
    _sensorParam0SpinBox->setEnabled(false);
    _sensorParam1SpinBox->setEnabled(false);
    _sensorParam2SpinBox->setEnabled(false);
    _sensorParam3SpinBox->setEnabled(false);

    switch (_sensorSelectComboBox->currentData().toUInt())
    {
        case 0x2300:  // VELOCITY_FROM_POSITION
            _sensorParamLabels.at(0)->setText(tr("Window size:"));
            _sensorParamLabels.at(0)->setEnabled(true);
            _sensorParam0SpinBox->setEnabled(true);
            break;

        case 0x3101:  // QEI_CH1
            _sensorParamLabels.at(0)->setText(tr("Filter divider:"));
            _sensorParamLabels.at(0)->setEnabled(true);
            _sensorParam0SpinBox->setEnabled(true);
            break;
    }
}

void MotionSensorWidget::updateFilterParams(int index)
{
    Q_UNUSED(index)

    _filterParamLabels.at(0)->setText(tr("param 0:"));
    _filterParamLabels.at(1)->setText(tr("param 1:"));
    _filterParamLabels.at(2)->setText(tr("param 2:"));
    _filterParamLabels.at(3)->setText(tr("param 3:"));
    _filterParamLabels.at(0)->setEnabled(false);
    _filterParamLabels.at(1)->setEnabled(false);
    _filterParamLabels.at(2)->setEnabled(false);
    _filterParamLabels.at(3)->setEnabled(false);
    _filterParam0SpinBox->setEnabled(false);
    _filterParam1SpinBox->setEnabled(false);
    _filterParam2SpinBox->setEnabled(false);
    _filterParam3SpinBox->setEnabled(false);

    switch (_filterSelectComboBox->currentData().toUInt())
    {
        case 0x2000:  // AVERAGING
            _filterParamLabels.at(0)->setText(tr("Sample count:"));
            _filterParamLabels.at(0)->setEnabled(true);
            _filterParam0SpinBox->setEnabled(true);
            break;
    }
}
