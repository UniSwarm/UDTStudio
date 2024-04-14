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

#include "indexspinbox.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QRegularExpression>

IndexSpinBox::IndexSpinBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;
}

double IndexSpinBox::value()
{
    return textEditValue().toDouble();
}

void IndexSpinBox::setDisplayValue(const QVariant &value, DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        lineEdit()->setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        lineEdit()->setFont(mfont);
    }
    setTextEditValue(value);
    emit valueChanged(value.toDouble());
}

bool IndexSpinBox::isEditing() const
{
    return lineEdit()->hasFocus();
}

void IndexSpinBox::keyPressEvent(QKeyEvent *event)
{
    QAbstractSpinBox::keyPressEvent(event);
    switch (event->key())
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            requestWriteValue(textEditValue());
            break;

        case Qt::Key_F5:
            requestReadValue();
            break;

        case Qt::Key_Escape:
            cancelEdit();
            break;
    }
}

void IndexSpinBox::focusOutEvent(QFocusEvent *event)
{
    cancelEdit();
    QAbstractSpinBox::focusOutEvent(event);
}

void IndexSpinBox::setTextEditValue(const QVariant &value)
{
    lineEdit()->setText(pstringValue(value, _hint));
}

QVariant IndexSpinBox::textEditValue() const
{
    bool ok = false;
    QVariant value;

    QString textValue = text();
    if (textValue.endsWith(_unit))
    {
        textValue.chop(_unit.length());
    }

    switch (_hint)
    {
        case AbstractIndexWidget::DisplayHexa:
            value = QVariant(textValue.toInt(&ok, 0));
            break;

        case AbstractIndexWidget::DisplayDirectValue:
        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayQ1_15:
        case AbstractIndexWidget::DisplayFloat:
            value = QVariant(textValue.toDouble(&ok));
            break;
    }

    if (ok)
    {
        return value;
    }
    return QVariant();
}

void IndexSpinBox::stepBy(int steps)
{
    QVariant newValue;

    switch (_hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
        case AbstractIndexWidget::DisplayHexa:
            newValue = textEditValue().toInt() + steps;
            break;

        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayQ1_15:
        case AbstractIndexWidget::DisplayFloat:
            newValue = textEditValue().toDouble() + steps;
            break;
    }

    switch (inBound(newValue))
    {
        case AbstractIndexWidget::BoundTooLow:
            setTextEditValue(_minValue.toDouble());
            break;
        case AbstractIndexWidget::BoundOK:
            setTextEditValue(newValue);
            break;
        case AbstractIndexWidget::BoundTooHigh:
            setTextEditValue(_maxValue.toDouble());
            break;
    }
}

QAbstractSpinBox::StepEnabled IndexSpinBox::stepEnabled() const
{
    if (isReadOnly())
    {
        return StepNone;
    }

    StepEnabled step = StepNone;
    if (_maxValue.isNull())
    {
        step |= StepUpEnabled;
    }
    else
    {
        if (textEditValue().toDouble() < _maxValue.toDouble())
        {
            step |= StepUpEnabled;
        }
    }

    if (_minValue.isNull())
    {
        step |= StepDownEnabled;
    }
    else
    {
        if (textEditValue().toDouble() > _minValue.toDouble())
        {
            step |= StepDownEnabled;
        }
    }
    return step;
}

void IndexSpinBox::updateHint()
{
    QRegularExpressionValidator *validator = nullptr;
    switch (_hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
            validator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("\\-?[0-9]+")), this);
            break;

        case AbstractIndexWidget::DisplayHexa:
            validator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("\\-?(0x)?[0-9A-F]+")), this);
            break;

        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayQ1_15:
        case AbstractIndexWidget::DisplayFloat:
            validator = new QRegularExpressionValidator(QRegularExpression(QStringLiteral("\\-?[0-9]*(\\.[0-9]*)?(e-?[0-9]*)?")), this);
            break;
    }
    lineEdit()->setValidator(validator);
}

void IndexSpinBox::mousePressEvent(QMouseEvent *event)
{
    QAbstractSpinBox::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexSpinBox::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractSpinBox::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = lineEdit()->createStandardContextMenu();
    menu = createStandardContextMenu(menu);

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}
