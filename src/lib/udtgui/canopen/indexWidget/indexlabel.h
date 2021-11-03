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

#ifndef INDEXLABEL_H
#define INDEXLABEL_H

#include "../../udtgui_global.h"

#include "abstractindexwidget.h"
#include <QLabel>

class UDTGUI_EXPORT IndexLabel : public QLabel, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexLabel(const NodeObjectId &objId = NodeObjectId());

    // AbstractIndexWidget interface
protected:
    void setDisplayValue(const QVariant &value, DisplayAttribute flags) override;
    bool isEditing() const override;

    // AbstractIndexWidget interface
protected:
    void updateObjId() override;

    // QWidget interface
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif  // INDEXLABEL_H
