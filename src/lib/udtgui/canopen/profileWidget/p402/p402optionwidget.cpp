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

#include "p402optionwidget.h"

#include "profile/p402/nodeprofile402.h"
#include "indexdb402.h"
#include "node.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>

P402OptionWidget::P402OptionWidget(QWidget *parent)
    : P402Mode(parent)
{
    createWidgets();
}

P402OptionWidget::~P402OptionWidget()
{
    unRegisterFullOd();
}

void P402OptionWidget::readAllObjects()
{
    if (_node)
    {
        _node->readObject(_abortConnectionObjectId);
        _node->readObject(_quickStopObjectId);
        _node->readObject(_shutdownObjectId);
        _node->readObject(_disableObjectId);
        _node->readObject(_haltObjectId);
        _node->readObject(_faultReactionObjectId);
    }
}

void P402OptionWidget::setNode(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }
    _node = node;

    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    setNodeInterrest(node);

    if (!_node->profiles().isEmpty())
    {
        _nodeProfile402 = dynamic_cast<NodeProfile402 *>(_node->profiles()[axis]);

        _abortConnectionObjectId = _nodeProfile402->abortConnectionObjectId();
        registerObjId({_abortConnectionObjectId});

        _quickStopObjectId = _nodeProfile402->quickStopObjectId();
        registerObjId({_quickStopObjectId});

        _shutdownObjectId = _nodeProfile402->shutdownObjectId();
        registerObjId({_shutdownObjectId});

        _disableObjectId = _nodeProfile402->disableObjectId();
        registerObjId({_disableObjectId});

        _haltObjectId = _nodeProfile402->haltObjectId();
        registerObjId({_haltObjectId});

        _faultReactionObjectId = _nodeProfile402->faultReactionObjectId();
        registerObjId({_faultReactionObjectId});
    }
}

void P402OptionWidget::abortConnectionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_abortConnectionObjectId, QVariant(value));
}

void P402OptionWidget::quickStopOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_quickStopObjectId, QVariant(value));
}

void P402OptionWidget::shutdownOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_shutdownObjectId, QVariant(value));
}

void P402OptionWidget::disableOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_disableObjectId, QVariant(value));
}

void P402OptionWidget::haltOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_haltObjectId, QVariant(value));
}

void P402OptionWidget::faultReactionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_faultReactionObjectId, QVariant(value));
}

void P402OptionWidget::createWidgets()
{
    // Create interface
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(abortConnectionWidgets());
    layout->addWidget(quickStopWidgets());
    layout->addWidget(shutdownWidgets());
    layout->addWidget(disableWidgets());
    layout->addWidget(haltWidgets());
    layout->addWidget(faultReactionWidgets());

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->addWidget(scrollArea);
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(vBoxLayout);
}

QGroupBox *P402OptionWidget::abortConnectionWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Abort connection option (0x6n07)"));
    QFormLayout *layout = new QFormLayout();
    _abortConnectionOptionGroup = new QButtonGroup(this);
    _abortConnectionOptionGroup->setExclusive(true);
    QRadioButton *_0AbortConnectionOptionCheckBox = new QRadioButton(tr("0 No action"));
    layout->addRow(_0AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_0AbortConnectionOptionCheckBox, 0);
    QRadioButton *_1AbortConnectionOptionCheckBox = new QRadioButton(tr("1 Fault signal"));
    layout->addRow(_1AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_1AbortConnectionOptionCheckBox, 1);
    QRadioButton *_2AbortConnectionOptionCheckBox = new QRadioButton(tr("2 Disable voltage command"));
    layout->addRow(_2AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_2AbortConnectionOptionCheckBox, 2);
    QRadioButton *_3AbortConnectionOptionCheckBox = new QRadioButton(tr("3 Quick stop command"));
    layout->addRow(_3AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_3AbortConnectionOptionCheckBox, 3);
    groupBox->setLayout(layout);
    connect(_abortConnectionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { abortConnectionOptionClicked(id); });

    return groupBox;
}

 QGroupBox *P402OptionWidget::quickStopWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Quick stop option (0x6n5A)"));
    QFormLayout *layout = new QFormLayout();
    _quickStopOptionGroup = new QButtonGroup(this);
    _quickStopOptionGroup->setExclusive(true);
    QRadioButton *_0QuickStopOptionCheckBox = new QRadioButton(tr("0 Disable drive function"));
    layout->addRow(_0QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_0QuickStopOptionCheckBox, 0);
    QRadioButton *_1QuickStopOptionCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp and transit into switch on disabled"));
    layout->addRow(_1QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_1QuickStopOptionCheckBox, 1);
    QRadioButton *_2QuickStopOptionCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp and transit into switch on disabled"));
    layout->addRow(_2QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_2QuickStopOptionCheckBox, 2);
    QRadioButton *_5QuickStopOptionCheckBox = new QRadioButton(tr("5 Slow down on slow down ramp and stay in quick stop active"));
    layout->addRow(_5QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_5QuickStopOptionCheckBox, 5);
    QRadioButton *_6QuickStopOptionCheckBox = new QRadioButton(tr("6 Slow down on quick stop ramp and stay in quick stop active"));
    layout->addRow(_6QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_6QuickStopOptionCheckBox, 6);
    groupBox->setLayout(layout);
    connect(_quickStopOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { quickStopOptionClicked(id); });

    return groupBox;
}

QGroupBox *P402OptionWidget::shutdownWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Shutdown option code (0x6n5B)"));
    QFormLayout *layout = new QFormLayout();
    _shutdownOptionGroup = new QButtonGroup(this);
    _shutdownOptionGroup->setExclusive(true);
    QRadioButton *_0ShutdownOptionCheckBox = new QRadioButton(tr("0 Disable drive function (switch-off the drive power stage)"));
    layout->addRow(_0ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(_0ShutdownOptionCheckBox, 0);
    QRadioButton *_1ShutdownOptionCheckBox = new QRadioButton(tr("1 Slow down with slow down ramp; disable of the drive function"));
    layout->addRow(_1ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(_1ShutdownOptionCheckBox, 1);
    groupBox->setLayout(layout);
    connect(_shutdownOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { shutdownOptionClicked(id); });

    return groupBox;
}

 QGroupBox *P402OptionWidget::disableWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Disable operation option code (0x6n5C)"));
    QFormLayout *layout = new QFormLayout();
    _disableOptionGroup = new QButtonGroup(this);
    _disableOptionGroup->setExclusive(true);
    QRadioButton *_0DisableOptionCheckBox = new QRadioButton(tr("0 Disable drive function (switch-off the drive power stage)"));
    layout->addRow(_0DisableOptionCheckBox);
    _disableOptionGroup->addButton(_0DisableOptionCheckBox, 0);
    QRadioButton *_1DisableOptionCheckBox = new QRadioButton(tr("1 Slow down with slow down ramp; disable of the drive function"));
    layout->addRow(_1DisableOptionCheckBox);
    _disableOptionGroup->addButton(_1DisableOptionCheckBox, 1);
    groupBox->setLayout(layout);
    connect(_disableOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { disableOptionClicked(id); });

    return groupBox;
}

QGroupBox *P402OptionWidget::haltWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Halt option (0x6n5D)"));
    QFormLayout *layout = new QFormLayout();
    _haltOptionGroup = new QButtonGroup(this);
    _haltOptionGroup->setExclusive(true);
    QRadioButton *_slowDownRampCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp and stay in operation enabled"));
    layout->addRow(_slowDownRampCheckBox);
    _haltOptionGroup->addButton(_slowDownRampCheckBox, 1);
    QRadioButton *_quickStopRampCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp and stay in operation enabled"));
    layout->addRow(_quickStopRampCheckBox);
    _haltOptionGroup->addButton(_quickStopRampCheckBox, 2);
    groupBox->setLayout(layout);
    connect(_haltOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { haltOptionClicked(id); });

    return groupBox;
}

QGroupBox *P402OptionWidget::faultReactionWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Fault reaction option (0x6n5E)"));
    QFormLayout *layout = new QFormLayout();
    _faultReactionOptionGroup = new QButtonGroup(this);
    _faultReactionOptionGroup->setExclusive(true);
    QRadioButton *_0FaultReactionOptionCheckBox = new QRadioButton(tr("0 Disable drive function, motor is free to rotate"));
    layout->addRow(_0FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_0FaultReactionOptionCheckBox, 0);
    QRadioButton *_1FaultReactionOptionCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp"));
    layout->addRow(_1FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_1FaultReactionOptionCheckBox, 1);
    QRadioButton *_2FaultReactionOptionCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp"));
    layout->addRow(_2FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_2FaultReactionOptionCheckBox, 2);
    groupBox->setLayout(layout);
    connect(_faultReactionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { faultReactionOptionClicked(id); });

    return groupBox;
}

void P402OptionWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((!_node) || (_node->status() != Node::STARTED))
    {
        return;
    }

    if ((objId == _abortConnectionObjectId)
        || (objId == _quickStopObjectId)
        || (objId == _shutdownObjectId)
        || (objId == _disableObjectId)
        || (objId == _haltObjectId)
        || (objId == _faultReactionObjectId))
    {
        if (_node->nodeOd()->indexExist(objId.index()))
        {
            int value = _node->nodeOd()->value(objId).toInt();

            if (objId == _abortConnectionObjectId)
            {
                _abortConnectionOptionGroup->button(value)->setChecked(true);
            }

            if (objId == _quickStopObjectId)
            {
                _quickStopOptionGroup->button(value)->setChecked(true);
            }

            if (objId == _shutdownObjectId)
            {
                _shutdownOptionGroup->button(value)->setChecked(true);
            }

            if (objId == _disableObjectId)
            {
                _disableOptionGroup->button(value)->setChecked(true);
            }

            if (objId == _haltObjectId)
            {
                _haltOptionGroup->button(value)->setChecked(true);
            }

            if (objId == _faultReactionObjectId)
            {
                _faultReactionOptionGroup->button(value)->setChecked(true);
            }
        }
    }
}
