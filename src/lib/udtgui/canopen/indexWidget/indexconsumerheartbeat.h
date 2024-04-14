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

#ifndef INDEXCONSUMERHEARTBEAT_H
#define INDEXCONSUMERHEARTBEAT_H

#include "../../udtgui_global.h"

#include "abstractindexwidget.h"
#include <QWidget>

#include <QSpinBox>

class UDTGUI_EXPORT IndexConsumerHeartBeat : public QWidget, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexConsumerHeartBeat(const NodeObjectId &objId = NodeObjectId());

protected:
    void createWidgets();
    QSpinBox *_nodeSpinBox;
    QSpinBox *_timeSpinBox;

    void sendValue();

    // AbstractIndexWidget interface
protected:
    void setDisplayValue(const QVariant &value, DisplayAttribute flags) override;
    bool isEditing() const override;
    void updateHint() override;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif  // INDEXCONSUMERHEARTBEAT_H
