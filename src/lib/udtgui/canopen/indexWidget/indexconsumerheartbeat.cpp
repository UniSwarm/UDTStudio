/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "indexconsumerheartbeat.h"

#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QMenu>

IndexConsumerHeartBeat::IndexConsumerHeartBeat(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    createWidgets();
    setObjId(objId);
}

void IndexConsumerHeartBeat::createWidgets()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _nodeSpinBox = new QSpinBox();
    _nodeSpinBox->setRange(0, 125);
    connect(_nodeSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &IndexConsumerHeartBeat::sendValue);
    layout->addWidget(_nodeSpinBox);

    _timeSpinBox = new QSpinBox();
    _timeSpinBox->setRange(0, 0xFFFF);
    _timeSpinBox->setSuffix(tr(" ms"));
    connect(_timeSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &IndexConsumerHeartBeat::sendValue);
    layout->addWidget(_timeSpinBox);

    setLayout(layout);
}

void IndexConsumerHeartBeat::sendValue()
{
    uint32_t val32 = (_nodeSpinBox->value() << 16) + _timeSpinBox->value();
    requestWriteValue(val32);
}

void IndexConsumerHeartBeat::setDisplayValue(const QVariant &value, DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        _nodeSpinBox->setFont(mfont);
        _timeSpinBox->setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        _nodeSpinBox->setFont(mfont);
        _timeSpinBox->setFont(mfont);
    }

    uint32_t val32 = value.toUInt();

    _nodeSpinBox->blockSignals(true);
    _nodeSpinBox->setValue((val32 >> 16) & 0xFF);
    _nodeSpinBox->blockSignals(false);

    _timeSpinBox->blockSignals(true);
    _timeSpinBox->setValue((val32 >> 0) & 0xFFFF);
    _timeSpinBox->blockSignals(false);
}

bool IndexConsumerHeartBeat::isEditing() const
{
    return _nodeSpinBox->hasFocus() || _timeSpinBox->hasFocus();
}

void IndexConsumerHeartBeat::updateHint()
{
}

void IndexConsumerHeartBeat::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}
