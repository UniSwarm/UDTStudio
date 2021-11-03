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

#include "nodescreensynchro.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexlabel.h"
#include "canopen/indexWidget/indexspinbox.h"

#include "indexdb402.h"
#include "node.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>

NodeScreenSynchro::NodeScreenSynchro(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenSynchro::setLogTimer(int ms)
{
    if (_dataLogger->isStarted())
    {
        _dataLogger->start(ms);
    }
}

void NodeScreenSynchro::readAllObject()
{
    _modeSynchroComboBox->readObject();
    _maxDiffSpinBox->readObject();
    _coeffSpinBox->readObject();
    _windowSpinBox->readObject();
    _offsetSpinBox->readObject();

    _flagLabel->readObject();
    _erorLabel->readObject();
    _correctorLabel->readObject();
}

void NodeScreenSynchro::createWidgets()
{
    _dataLogger = new DataLogger();

    QWidget *widget = new QWidget(this);
    QVBoxLayout *syncroBoxesLayout = new QVBoxLayout(widget);
    syncroBoxesLayout->setContentsMargins(0, 0, 4, 0);
    syncroBoxesLayout->setSpacing(0);

    _synchroConfigGroupBox = createSynchroConfigurationWidgets();
    syncroBoxesLayout->addWidget(_synchroConfigGroupBox);
    _synchroStatusGroupBox = createSynchroStatusWidgets();
    syncroBoxesLayout->addWidget(_synchroStatusGroupBox);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(scrollArea);

    QWidget *widgetLogger = new QWidget();
    QVBoxLayout *layoutLogger = new QVBoxLayout();
    layoutLogger->setContentsMargins(4, 4, 0, 0);
    _dataLoggerWidget = new DataLoggerWidget(_dataLogger, Qt::Vertical);
    _dataLoggerWidget->setType(DataLoggerWidget::InternalType);
    layoutLogger->addWidget(_dataLoggerWidget);
    widgetLogger->setLayout(layoutLogger);
    splitter->addWidget(widgetLogger);
    splitter->setSizes(QList<int>() << 100 << 300);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2, 2);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(createToolBarWidgets());
    vBoxLayout->addWidget(splitter);
    setLayout(vBoxLayout);
}

QToolBar *NodeScreenSynchro::createToolBarWidgets()
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

    // read all action
    QAction *readAllAction = toolBar->addAction(tr("Read all objects"));
    readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllAction, &QAction::triggered, this, &NodeScreenSynchro::readAllObject);

    return toolBar;
}

QGroupBox *NodeScreenSynchro::createSynchroConfigurationWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Synchro config"));
    QFormLayout *layout = new QFormLayout();

    _modeSynchroComboBox = new IndexComboBox();
    _modeSynchroComboBox->addItem("OFF", QVariant(static_cast<uint16_t>(0x0000)));
    _modeSynchroComboBox->addItem("Position", QVariant(static_cast<uint16_t>(0x0001)));
    layout->addRow(tr("&Mode_Synchro:"), _modeSynchroComboBox);

    _maxDiffSpinBox = new IndexSpinBox();
    _maxDiffSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    layout->addRow(tr("Ma&x diff:"), _maxDiffSpinBox);

    _coeffSpinBox = new IndexSpinBox();
    _coeffSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    layout->addRow(tr("&Coeff:"), _coeffSpinBox);

    _windowSpinBox = new IndexSpinBox();
    _windowSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    layout->addRow(tr("&Window:"), _windowSpinBox);

    _offsetSpinBox = new IndexSpinBox();
    _offsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    layout->addRow(tr("&Offset:"), _offsetSpinBox);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox *NodeScreenSynchro::createSynchroStatusWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Synchro status"));
    QFormLayout *layout = new QFormLayout();

    _flagLabel = new IndexLabel();
    layout->addRow(tr("&Flag:"), _flagLabel);

    _erorLabel = new IndexLabel();
    _erorLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    layout->addRow(tr("&Error:"), _erorLabel);

    _correctorLabel = new IndexLabel();
    _correctorLabel->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    layout->addRow(tr("&Corrector:"), _correctorLabel);

    groupBox->setLayout(layout);
    return groupBox;
}

QString NodeScreenSynchro::title() const
{
    return QString(tr("Synchro %1").arg(_axis));
}

void NodeScreenSynchro::setNodeInternal(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    if ((node->profileNumber() != 0x192) || node->profiles().isEmpty())
    {
        return;
    }

    _axis = axis;

    NodeObjectId modeSynchroSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_MODE_SYNCHRO);
    NodeObjectId maxDiffSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_MAX_DIFF);
    NodeObjectId coeffSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_COEFF);
    NodeObjectId windowSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_WINDOW);
    NodeObjectId offsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_OFFSET);

    NodeObjectId flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_STATUS_FLAG);
    NodeObjectId erorLabel_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_STATUS_ERROR);
    NodeObjectId correctorLabel_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_STATUS_CORRECTOR);

    _modeSynchroComboBox->setObjId(modeSynchroSpinBox_ObjId);
    _maxDiffSpinBox->setObjId(maxDiffSpinBox_ObjId);
    _coeffSpinBox->setObjId(coeffSpinBox_ObjId);
    _windowSpinBox->setObjId(windowSpinBox_ObjId);
    _offsetSpinBox->setObjId(offsetSpinBox_ObjId);

    _flagLabel->setObjId(flagLabel_ObjId);
    _erorLabel->setObjId(erorLabel_ObjId);
    _correctorLabel->setObjId(correctorLabel_ObjId);

    _modeSynchroComboBox->setNode(node);
    _maxDiffSpinBox->setNode(node);
    _coeffSpinBox->setNode(node);
    _windowSpinBox->setNode(node);
    _offsetSpinBox->setNode(node);

    _flagLabel->setNode(node);
    _erorLabel->setNode(node);
    _correctorLabel->setNode(node);

    // Datalogger
    erorLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    correctorLabel_ObjId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _dataLogger->removeData(erorLabel_ObjId);
    _dataLogger->removeData(correctorLabel_ObjId);
    _dataLogger->addData(erorLabel_ObjId);
    _dataLogger->addData(correctorLabel_ObjId);
}
