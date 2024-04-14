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

#ifndef INDEXCOMBOBOX_H
#define INDEXCOMBOBOX_H

#include "../../udtgui_global.h"

#include "abstractindexwidget.h"
#include <QComboBox>

class UDTGUI_EXPORT IndexComboBox : public QComboBox, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexComboBox(const NodeObjectId &objId = NodeObjectId());

protected:
    void setInternalIndex(int index);
    bool _internalUpdate;

    // QWidget interface
protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    // AbstractIndexWidget interface
protected:
    void setDisplayValue(const QVariant &value, DisplayAttribute flags) override;
    bool isEditing() const override;
};

#endif  // INDEXCOMBOBOX_H
