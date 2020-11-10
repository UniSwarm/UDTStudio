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

#include "abstractindexwidget.h"

#include "node.h"

#include <QDebug>

AbstractIndexWidget::AbstractIndexWidget(const NodeObjectId &objId)
    :_objId(objId)
{
    setObjId(objId);
    _hint = DisplayDirectValue;
    _requestRead = false;
}

Node *AbstractIndexWidget::node() const
{
    return nodeInterrest();
}

void AbstractIndexWidget::setNode(Node *node)
{
    setNodeInterrest(node);
}

void AbstractIndexWidget::requestWriteValue(const QVariant &value)
{
    if (!nodeInterrest())
    {
        return;
    }
    _pendingValue = value;
    nodeInterrest()->writeObject(_objId, value);
    setDisplayValue(_pendingValue, DisplayAttribute::PendingValue);
}

void AbstractIndexWidget::requestReadValue()
{
    if (!nodeInterrest())
    {
        return;
    }
    _requestRead = true;
    nodeInterrest()->readObject(_objId);
}

void AbstractIndexWidget::cancelEdit()
{
    if (!nodeInterrest())
    {
        setDisplayValue(QVariant(), DisplayAttribute::Error);
        return;
    }
    setDisplayValue(nodeInterrest()->nodeOd()->value(_objId), DisplayAttribute::Normal);
}

void AbstractIndexWidget::updateHint()
{
}

AbstractIndexWidget::DisplayHint AbstractIndexWidget::hint() const
{
    return _hint;
}

void AbstractIndexWidget::setDisplayHint(const AbstractIndexWidget::DisplayHint hint)
{
    if (_hint != hint)
    {
        _hint = hint;
        updateHint();
        if (!nodeInterrest())
        {
            return;
        }
        setDisplayValue(nodeInterrest()->nodeOd()->value(_objId), DisplayAttribute::Normal);
    }
}

void AbstractIndexWidget::readObject()
{
    if (!nodeInterrest())
    {
        return;
    }
    nodeInterrest()->readObject(_objId);
}

NodeObjectId AbstractIndexWidget::objId() const
{
    return _objId;
}

void AbstractIndexWidget::setObjId(const NodeObjectId &objId)
{
    if (_objId.isValid())
    {
        unRegisterObjId(_objId);
    }
    _objId = objId;

    if (objId.isASubIndex())
    {
        registerObjId(_objId);
    }
    Node *node = _objId.node();
    if (node)
    {
        setNode(node);
    }
}

void AbstractIndexWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!nodeInterrest())
    {
        return;
    }

    QVariant newValue = nodeInterrest()->nodeOd()->value(objId);
    if (flags & SDO::Error)
    {
        if (_pendingValue.isValid() && (flags & SDO::Write)) // we request a write value that cause an error
        {
            setDisplayValue(_pendingValue, DisplayAttribute::Error);
            _pendingValue = QVariant();
            return;
        }
        else if (flags & SDO::Read) // any read cause an error
        {
            if (isEditing() && !_requestRead)
            {
                return;
            }
            setDisplayValue(_pendingValue, DisplayAttribute::Error);
            _requestRead = false;
            return;
        }
        else // any other write request failed
        {
            return;
        }
    }
    if (flags & SDO::Read && this->isEditing() && !_requestRead)
    {
        return;
    }
    setDisplayValue(newValue, DisplayAttribute::Normal);
    _requestRead = false;
    _pendingValue = QVariant();
}
