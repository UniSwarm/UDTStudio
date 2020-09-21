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

#ifndef ABSTRACTINDEXWIDGET_H
#define ABSTRACTINDEXWIDGET_H

#include "../../udtgui_global.h"

#include "nodeodsubscriber.h"
#include <QWidget>

class UDTGUI_EXPORT AbstractIndexWidget : public NodeOdSubscriber
{
public:
    AbstractIndexWidget(const NodeObjectId &objId);

    Node *node() const;
    void setNode(Node *node);

protected:
    enum DisplayAttribute
    {
        Normal,
        PendingValue,
        Error,
        Invalid
    };
    virtual void setDisplayValue(const QVariant &value, DisplayAttribute flags) =0;
    void requestWriteValue(const QVariant &value);
    void requestReadValue();
    virtual bool isEditing() const =0;

protected:
    NodeObjectId _objId;
    QVariant _pendingValue;
    bool _requestRead;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags);
};

#endif // ABSTRACTINDEXWIDGET_H
