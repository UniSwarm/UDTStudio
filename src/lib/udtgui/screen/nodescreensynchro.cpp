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

#include "nodescreensynchro.h"

#include "canopen/widget/indexlabel.h"
#include "canopen/widget/indexspinbox.h"
#include "indexdb402.h"
#include "node.h"

#include <QFormLayout>
#include <QGroupBox>

NodeScreenSynchro::NodeScreenSynchro(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenSynchro::createWidgets()
{
    QWidget *MotionSensorWidget = new QWidget(this);
    QVBoxLayout *actionLayout = new QVBoxLayout(MotionSensorWidget);

    QGroupBox *synchroConfigGroupBox = new QGroupBox(tr("Synchro config"));
    QFormLayout *synchroLayout = new QFormLayout();

    _modeSynchroSpinBox = new IndexSpinBox();
    _modeSynchroSpinBox->setDisplayHint(AbstractIndexWidget::DisplayDirectValue);
    synchroLayout->addRow(tr("&Mode_Synchro :"), _modeSynchroSpinBox);

    _maxDiffSpinBox = new IndexSpinBox();
    _maxDiffSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    synchroLayout->addRow(tr("Ma&x diff :"), _maxDiffSpinBox);

    _coeffSpinBox = new IndexSpinBox();
    _coeffSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    synchroLayout->addRow(tr("&Coeff :"), _coeffSpinBox);

    _windowSpinBox = new IndexSpinBox();
    _windowSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    synchroLayout->addRow(tr("&Window :"), _windowSpinBox);

    _offsetSpinBox = new IndexSpinBox();
    _offsetSpinBox->setDisplayHint(AbstractIndexWidget::DisplayQ15_16);
    synchroLayout->addRow(tr("&Offset :"), _offsetSpinBox);

    synchroConfigGroupBox->setLayout(synchroLayout);

    QGroupBox *statusGroupBox = new QGroupBox(tr("Synchro status"));
    QFormLayout *statusLayout = new QFormLayout();

    _flagLabel = new IndexLabel();
    statusLayout->addRow(tr("&Flag :"), _flagLabel);

    _erorLabel = new IndexLabel();
    statusLayout->addRow(tr("&Error :"), _erorLabel);

    _correctorLabel = new IndexLabel();
    statusLayout->addRow(tr("&Corrector :"), _correctorLabel);

    statusGroupBox->setLayout(statusLayout);

    actionLayout->addWidget(synchroConfigGroupBox);
    actionLayout->addWidget(statusGroupBox);

    setLayout(actionLayout);
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

    _modeSynchroSpinBox->setNode(node);
    _maxDiffSpinBox->setNode(node);
    _coeffSpinBox->setNode(node);
    _windowSpinBox->setNode(node);
    _offsetSpinBox->setNode(node);

    _flagLabel->setNode(node);
    _erorLabel->setNode(node);
    _correctorLabel->setNode(node);

    NodeObjectId _modeSynchroSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_MODE_SYNCHRO);
    NodeObjectId _maxDiffSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_MAX_DIFF);
    NodeObjectId _coeffSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_COEFF);
    NodeObjectId _windowSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_WINDOW);
    NodeObjectId _offsetSpinBox_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_CONFIG_OFFSET);

    NodeObjectId _flagLabel_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_STATUS_FLAG);
    NodeObjectId _erorLabel_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_STATUS_ERROR);
    NodeObjectId _correctorLabel_ObjId = IndexDb402::getObjectId(IndexDb402::S12_SYNCHRO_STATUS_CORRECTOR);

    _modeSynchroSpinBox->setObjId(_modeSynchroSpinBox_ObjId);
    _maxDiffSpinBox->setObjId(_maxDiffSpinBox_ObjId);
    _coeffSpinBox->setObjId(_coeffSpinBox_ObjId);
    _windowSpinBox->setObjId(_windowSpinBox_ObjId);
    _offsetSpinBox->setObjId(_offsetSpinBox_ObjId);

    _flagLabel->setObjId(_flagLabel_ObjId);
    _erorLabel->setObjId(_erorLabel_ObjId);
    _correctorLabel->setObjId(_correctorLabel_ObjId);
}
