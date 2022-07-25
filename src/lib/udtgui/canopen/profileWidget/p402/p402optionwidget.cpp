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

#include "indexdb402.h"
#include "node.h"
#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>

P402OptionWidget::P402OptionWidget(QWidget *parent)
    : P402ModeWidget(parent)
{
    createWidgets();
}

void P402OptionWidget::setIProfile(NodeProfile402 *nodeProfile402)
{
    if (nodeProfile402 == nullptr)
    {
        return;
    }

    registerObjId(_nodeProfile402->abortConnectionObjectId());
    registerObjId(_nodeProfile402->quickStopObjectId());
    registerObjId(_nodeProfile402->shutdownObjectId());
    registerObjId(_nodeProfile402->disableObjectId());
    registerObjId(_nodeProfile402->haltObjectId());
    registerObjId(_nodeProfile402->faultReactionObjectId());
}

void P402OptionWidget::readAllObjects()
{
    readObject(_nodeProfile402->abortConnectionObjectId());
    readObject(_nodeProfile402->quickStopObjectId());
    readObject(_nodeProfile402->shutdownObjectId());
    readObject(_nodeProfile402->disableObjectId());
    readObject(_nodeProfile402->haltObjectId());
    readObject(_nodeProfile402->faultReactionObjectId());
}

void P402OptionWidget::abortConnectionOptionClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _nodeProfile402->node()->writeObject(_nodeProfile402->abortConnectionObjectId(), QVariant(value));
}

void P402OptionWidget::quickStopOptionClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _nodeProfile402->node()->writeObject(_nodeProfile402->quickStopObjectId(), QVariant(value));
}

void P402OptionWidget::shutdownOptionClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _nodeProfile402->node()->writeObject(_nodeProfile402->shutdownObjectId(), QVariant(value));
}

void P402OptionWidget::disableOptionClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _nodeProfile402->node()->writeObject(_nodeProfile402->disableObjectId(), QVariant(value));
}

void P402OptionWidget::haltOptionClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _nodeProfile402->node()->writeObject(_nodeProfile402->haltObjectId(), QVariant(value));
}

void P402OptionWidget::faultReactionOptionClicked(int id)
{
    if (_nodeProfile402 == nullptr)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _nodeProfile402->node()->writeObject(_nodeProfile402->faultReactionObjectId(), QVariant(value));
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
    QRadioButton *p0AbortConnectionOptionCheckBox = new QRadioButton(tr("0 No action"));
    layout->addRow(p0AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(p0AbortConnectionOptionCheckBox, 0);
    QRadioButton *p1AbortConnectionOptionCheckBox = new QRadioButton(tr("1 Fault signal"));
    layout->addRow(p1AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(p1AbortConnectionOptionCheckBox, 1);
    QRadioButton *p2AbortConnectionOptionCheckBox = new QRadioButton(tr("2 Disable voltage command"));
    layout->addRow(p2AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(p2AbortConnectionOptionCheckBox, 2);
    QRadioButton *p3AbortConnectionOptionCheckBox = new QRadioButton(tr("3 Quick stop command"));
    layout->addRow(p3AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(p3AbortConnectionOptionCheckBox, 3);
    groupBox->setLayout(layout);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_abortConnectionOptionGroup,
            QOverload<int>::of(&QButtonGroup::buttonClicked),
            this,
            [=](int id)
            {
                abortConnectionOptionClicked(id);
            });
#else
    connect(_abortConnectionOptionGroup,
            QOverload<int>::of(&QButtonGroup::idClicked),
            this,
            [=](int id)
            {
                abortConnectionOptionClicked(id);
            });
#endif

    return groupBox;
}

QGroupBox *P402OptionWidget::quickStopWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Quick stop option (0x6n5A)"));
    QFormLayout *layout = new QFormLayout();
    _quickStopOptionGroup = new QButtonGroup(this);
    _quickStopOptionGroup->setExclusive(true);
    QRadioButton *p0QuickStopOptionCheckBox = new QRadioButton(tr("0 Disable drive function"));
    layout->addRow(p0QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(p0QuickStopOptionCheckBox, 0);
    QRadioButton *p1QuickStopOptionCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp and transit into switch on disabled"));
    layout->addRow(p1QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(p1QuickStopOptionCheckBox, 1);
    QRadioButton *p2QuickStopOptionCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp and transit into switch on disabled"));
    layout->addRow(p2QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(p2QuickStopOptionCheckBox, 2);
    QRadioButton *p5QuickStopOptionCheckBox = new QRadioButton(tr("5 Slow down on slow down ramp and stay in quick stop active"));
    layout->addRow(p5QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(p5QuickStopOptionCheckBox, 5);
    QRadioButton *p6QuickStopOptionCheckBox = new QRadioButton(tr("6 Slow down on quick stop ramp and stay in quick stop active"));
    layout->addRow(p6QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(p6QuickStopOptionCheckBox, 6);
    groupBox->setLayout(layout);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_quickStopOptionGroup,
            QOverload<int>::of(&QButtonGroup::buttonPressed),
            [=](int id)
            {
                quickStopOptionClicked(id);
            });
#else
    connect(_quickStopOptionGroup,
            QOverload<int>::of(&QButtonGroup::idPressed),
            this,
            [=](int id)
            {
                quickStopOptionClicked(id);
            });
#endif

    return groupBox;
}

QGroupBox *P402OptionWidget::shutdownWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Shutdown option code (0x6n5B)"));
    QFormLayout *layout = new QFormLayout();
    _shutdownOptionGroup = new QButtonGroup(this);
    _shutdownOptionGroup->setExclusive(true);
    QRadioButton *p0ShutdownOptionCheckBox = new QRadioButton(tr("0 Disable drive function (switch-off the drive power stage)"));
    layout->addRow(p0ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(p0ShutdownOptionCheckBox, 0);
    QRadioButton *p1ShutdownOptionCheckBox = new QRadioButton(tr("1 Slow down with slow down ramp; disable of the drive function"));
    layout->addRow(p1ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(p1ShutdownOptionCheckBox, 1);
    groupBox->setLayout(layout);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_shutdownOptionGroup,
            QOverload<int>::of(&QButtonGroup::buttonPressed),
            this,
            [=](int id)
            {
                shutdownOptionClicked(id);
            });
#else
    connect(_shutdownOptionGroup,
            QOverload<int>::of(&QButtonGroup::idPressed),
            this,
            [=](int id)
            {
                shutdownOptionClicked(id);
            });
#endif

    return groupBox;
}

QGroupBox *P402OptionWidget::disableWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Disable operation option code (0x6n5C)"));
    QFormLayout *layout = new QFormLayout();
    _disableOptionGroup = new QButtonGroup(this);
    _disableOptionGroup->setExclusive(true);
    QRadioButton *p0DisableOptionCheckBox = new QRadioButton(tr("0 Disable drive function (switch-off the drive power stage)"));
    layout->addRow(p0DisableOptionCheckBox);
    _disableOptionGroup->addButton(p0DisableOptionCheckBox, 0);
    QRadioButton *p1DisableOptionCheckBox = new QRadioButton(tr("1 Slow down with slow down ramp; disable of the drive function"));
    layout->addRow(p1DisableOptionCheckBox);
    _disableOptionGroup->addButton(p1DisableOptionCheckBox, 1);
    groupBox->setLayout(layout);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_disableOptionGroup,
            QOverload<int>::of(&QButtonGroup::buttonPressed),
            this,
            [=](int id)
            {
                disableOptionClicked(id);
            });
#else
    connect(_disableOptionGroup,
            QOverload<int>::of(&QButtonGroup::idPressed),
            this,
            [=](int id)
            {
                disableOptionClicked(id);
            });
#endif

    return groupBox;
}

QGroupBox *P402OptionWidget::haltWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Halt option (0x6n5D)"));
    QFormLayout *layout = new QFormLayout();
    _haltOptionGroup = new QButtonGroup(this);
    _haltOptionGroup->setExclusive(true);
    QRadioButton *slowDownRampCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp and stay in operation enabled"));
    layout->addRow(slowDownRampCheckBox);
    _haltOptionGroup->addButton(slowDownRampCheckBox, 1);
    QRadioButton *quickStopRampCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp and stay in operation enabled"));
    layout->addRow(quickStopRampCheckBox);
    _haltOptionGroup->addButton(quickStopRampCheckBox, 2);
    groupBox->setLayout(layout);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_haltOptionGroup,
            QOverload<int>::of(&QButtonGroup::buttonClicked),
            this,
            [=](int id)
            {
                haltOptionClicked(id);
            });
#else
    connect(_haltOptionGroup,
            QOverload<int>::of(&QButtonGroup::idPressed),
            this,
            [=](int id)
            {
                haltOptionClicked(id);
            });
#endif

    return groupBox;
}

QGroupBox *P402OptionWidget::faultReactionWidgets()
{
    QGroupBox *groupBox = new QGroupBox(tr("Fault reaction option (0x6n5E)"));
    QFormLayout *layout = new QFormLayout();
    _faultReactionOptionGroup = new QButtonGroup(this);
    _faultReactionOptionGroup->setExclusive(true);
    QRadioButton *p0FaultReactionOptionCheckBox = new QRadioButton(tr("0 Disable drive function, motor is free to rotate"));
    layout->addRow(p0FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(p0FaultReactionOptionCheckBox, 0);
    QRadioButton *p1FaultReactionOptionCheckBox = new QRadioButton(tr("1 Slow down on slow down ramp"));
    layout->addRow(p1FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(p1FaultReactionOptionCheckBox, 1);
    QRadioButton *p2FaultReactionOptionCheckBox = new QRadioButton(tr("2 Slow down on quick stop ramp"));
    layout->addRow(p2FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(p2FaultReactionOptionCheckBox, 2);
    groupBox->setLayout(layout);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(_faultReactionOptionGroup,
            QOverload<int>::of(&QButtonGroup::buttonClicked),
            this,
            [=](int id)
            {
                faultReactionOptionClicked(id);
            });
#else
    connect(_faultReactionOptionGroup,
            QOverload<int>::of(&QButtonGroup::idPressed),
            this,
            [=](int id)
            {
                faultReactionOptionClicked(id);
            });
#endif

    return groupBox;
}

void P402OptionWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if (_nodeProfile402->node()->nodeOd()->indexExist(objId.index()))
    {
        int value = _nodeProfile402->node()->nodeOd()->value(objId).toInt();

        if (objId == _nodeProfile402->abortConnectionObjectId())
        {
            QAbstractButton *button = _abortConnectionOptionGroup->button(value);
            if (button != nullptr)
            {
                button->setChecked(true);
            }
        }

        if (objId == _nodeProfile402->quickStopObjectId())
        {
            QAbstractButton *button = _quickStopOptionGroup->button(value);
            if (button != nullptr)
            {
                button->setChecked(true);
            }
        }

        if (objId == _nodeProfile402->shutdownObjectId())
        {
            QAbstractButton *button = _shutdownOptionGroup->button(value);
            if (button != nullptr)
            {
                button->setChecked(true);
            }
        }

        if (objId == _nodeProfile402->disableObjectId())
        {
            QAbstractButton *button = _disableOptionGroup->button(value);
            if (button != nullptr)
            {
                button->setChecked(true);
            }
        }

        if (objId == _nodeProfile402->haltObjectId())
        {
            QAbstractButton *button = _haltOptionGroup->button(value);
            if (button != nullptr)
            {
                button->setChecked(true);
            }
        }

        if (objId == _nodeProfile402->faultReactionObjectId())
        {
            QAbstractButton *button = _faultReactionOptionGroup->button(value);
            if (button != nullptr)
            {
                button->setChecked(true);
            }
        }
    }
}
