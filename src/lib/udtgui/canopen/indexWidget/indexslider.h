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

#ifndef INDEXSLIDER_H
#define INDEXSLIDER_H

#include "../../udtgui_global.h"

#include "abstractindexwidget.h"
#include <QSlider>
#include <QStyle>

class UDTGUI_EXPORT IndexSlider : public QSlider, public AbstractIndexWidget
{
    Q_OBJECT
public:
    IndexSlider(QWidget *parent);
    IndexSlider(const NodeObjectId &objId = NodeObjectId());
    IndexSlider(Qt::Orientation orientation, QWidget *parent = nullptr, const NodeObjectId &objId = NodeObjectId());

    int feedBackValue() const;
    void setFeedBackValue(int feedBackValue);

protected:
    bool _internalUpdate;
    void applyValue(int value);

    int _feedBackValue;
    bool _hasFeedBack;

    void initStyle();
    int _pressedControl;
    int _hoverControl;

    // AbstractIndexWidget interface
protected:
    void setDisplayValue(const QVariant &value, DisplayAttribute flags) override;
    bool isEditing() const override;
    void updateRange() override;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;

protected:
};

#endif  // INDEXSLIDER_H
