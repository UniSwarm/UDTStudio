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

#include "indexslider.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QVBoxLayout>
#include <QtGlobal>

IndexSlider::IndexSlider(Qt::Orientation orientation, QWidget *parent, const NodeObjectId &objId)
    : QSlider(orientation, parent)
    , AbstractIndexWidget(objId)
{
    _internalUpdate = false;
    _feedBackValue = 0;
    _hasFeedBack = false;

    _pressedControl = 0;
    _hoverControl = 0;

    initStyle();

    setObjId(objId);
    connect(this, &QSlider::valueChanged, this, &IndexSlider::applyValue);
}

IndexSlider::IndexSlider(QWidget *parent)
    : IndexSlider(Qt::Horizontal, parent, NodeObjectId())
{
}

IndexSlider::IndexSlider(const NodeObjectId &objId)
    : IndexSlider(Qt::Horizontal, nullptr, objId)
{
}

void IndexSlider::initStyle()
{
    setStyleSheet("QSlider{background: none;}"
                  "QSlider::sub-page:horizontal {background: none; border: 0;}");
}

void IndexSlider::applyValue(int value)
{
    if (!_internalUpdate)
    {
        requestWriteValue(value);
    }
}

int IndexSlider::feedBackValue() const
{
    return _feedBackValue;
}

void IndexSlider::setFeedBackValue(int feedBackValue)
{
    _hasFeedBack = true;
    feedBackValue = qBound(minimum(), feedBackValue, maximum());
    if (_feedBackValue != feedBackValue)
    {
        _feedBackValue = feedBackValue;
        update();
    }
}

void IndexSlider::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
{
    Q_UNUSED(flags)

    _internalUpdate = true;
    setValue(value.toInt());
    _internalUpdate = false;
}

bool IndexSlider::isEditing() const
{
    return false;
}

void IndexSlider::updateRange()
{
    setRange(_minValue.toInt(), _maxValue.toInt());
}

void IndexSlider::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

void IndexSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter p(this);
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    if (_pressedControl)
    {
        opt.activeSubControls = static_cast<QStyle::SubControls>(_pressedControl);
        opt.state |= QStyle::State_Sunken;
    }
    else
    {
        opt.activeSubControls = static_cast<QStyle::SubControls>(_hoverControl);
    }

    QRect barRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    p.setBrush(QColor("#32414B"));  // TODO get color from theme
    p.setPen(QPen(Qt::NoPen));
    p.drawRect(barRect);

    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    p.setBrush(QColor("#1464A0"));  // TODO get color from theme
    p.setPen(QPen(Qt::NoPen));

    double feedbackValue = _hasFeedBack ? _feedBackValue : value();

    // TODO add vertical management
    double width = barRect.width() - handleRect.width();
    double range = maximum() - minimum();
    double centerX = (-minimum() / range) * barRect.width();
    double valueX = ((feedbackValue - minimum()) / range) * width + handleRect.width() / 2;
    barRect.adjust(centerX, 0, 0, 0);
    barRect.setRight(valueX);

    p.drawRect(barRect.normalized());

    opt.subControls = QStyle::SC_SliderHandle;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);
}

void IndexSlider::mousePressEvent(QMouseEvent *event)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    if (handleRect.contains(event->pos()))
    {
        _pressedControl = QStyle::SC_SliderHandle;
    }
    QSlider::mousePressEvent(event);
}

void IndexSlider::mouseReleaseEvent(QMouseEvent *event)
{
    _pressedControl = 0;
    QSlider::mouseReleaseEvent(event);
}

bool IndexSlider::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::HoverEnter:
        case QEvent::HoverLeave:
        case QEvent::HoverMove:
            if (const QHoverEvent *he = static_cast<const QHoverEvent *>(event))
            {
                QStyleOptionSlider opt;
                initStyleOption(&opt);
                QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
                int hoverControl = QStyle::SC_None;
                if (handleRect.contains(he->pos()))
                {
                    hoverControl = QStyle::SC_SliderHandle;
                }
                if (hoverControl != _hoverControl)
                {
                    _hoverControl = hoverControl;
                    update(handleRect);
                }
            }
            break;

        default:
            break;
    }
    return QSlider::event(event);
}
