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

    const NodeObjectId &objId() const;
    void setObjId(const NodeObjectId &objId);

    enum DisplayHint
    {
        DisplayDirectValue = 0x00,
        DisplayHexa = 0x01,
        DisplayQ15_16 = 0x02,
        DisplayQ1_15 = 0x04,
        DisplayFloat = 0x08,
    };
    DisplayHint hint() const;
    void setDisplayHint(const DisplayHint hint);

    QVariant value() const;
    QString stringValue() const;

    // min max
    // tooltip
    // setunit

public slots:
    void readObject();

signals:
    void valueChanged(QVariant value);

protected:
    enum DisplayAttribute
    {
        Normal,
        PendingValue,
        Error,
        Invalid
    };
    virtual void setDisplayValue(const QVariant &value, DisplayAttribute flags) = 0;
    void requestWriteValue(const QVariant &value);
    void requestReadValue();
    virtual bool isEditing() const = 0;
    void cancelEdit();

    virtual void updateHint();
    virtual void updateObjId();

    static QVariant pValue(const QVariant &value, const DisplayHint hint = DisplayHint::DisplayDirectValue);
    static QString pstringValue(const QVariant &value, const DisplayHint hint = DisplayHint::DisplayDirectValue);

protected:
    NodeObjectId _objId;
    QVariant _lastValue;
    QVariant _pendingValue;
    bool _requestRead;

    DisplayHint _hint;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags);
};

#endif // ABSTRACTINDEXWIDGET_H
