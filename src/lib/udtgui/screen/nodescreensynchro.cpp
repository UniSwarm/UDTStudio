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

#include "canopen/widget/indexcombobox.h"
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

    _modeSynchroComboBox = new IndexComboBox();
    _modeSynchroComboBox->addItem("OFF", QVariant(static_cast<uint16_t>(0x0000)));
    _modeSynchroComboBox->addItem("Position", QVariant(static_cast<uint16_t>(0x0001)));
    synchroLayout->addRow(tr("&Mode_Synchro :"), _modeSynchroComboBox);

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
}
