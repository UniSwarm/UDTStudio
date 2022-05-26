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

#ifndef INDEXCHECKBOX_H
#define INDEXCHECKBOX_H

#include "../../udtgui_global.h"

#include "abstractindexwidget.h"
#include <QCheckBox>

class UDTGUI_EXPORT IndexCheckBox : public QCheckBox, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexCheckBox(const NodeObjectId &objId = NodeObjectId());

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    // QAbstractButton interface
protected:
    void nextCheckState() override;

    // AbstractIndexWidget interface
protected:
    void setDisplayValue(const QVariant &value, DisplayAttribute flags) override;
    bool isEditing() const override;
};

#endif  // INDEXCHECKBOX_H
