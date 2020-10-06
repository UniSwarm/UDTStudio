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

#ifndef INDEXSPINBOX_H
#define INDEXSPINBOX_H

#include "../../udtgui_global.h"

#include "abstractindexwidget.h"
#include <QAbstractSpinBox>

class UDTGUI_EXPORT IndexSpinBox : public QAbstractSpinBox, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexSpinBox(const NodeObjectId &objId = NodeObjectId());

    // AbstractIndexWidget interface
public:
    void setDisplayValue(const QVariant &value, DisplayAttribute flags) override;
    bool isEditing() const override;
    void updateHint() override;

protected:
    void setValue(const QVariant &value);
    QVariant value() const;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    // QAbstractSpinBox interface
public:
    void stepBy(int steps) override;
    StepEnabled stepEnabled() const override;
};

#endif // INDEXSPINBOX_H
