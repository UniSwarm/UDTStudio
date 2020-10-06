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

#include "indexspinbox.h"

#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>
#include <QRegularExpression>

IndexSpinBox::IndexSpinBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    updateHint();
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
    setValue(value);
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
        requestWriteValue(value());
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

void IndexSpinBox::setValue(const QVariant &value)
{
    switch (_hint)
    {
    case AbstractIndexWidget::DisplayDirectValue:
        lineEdit()->setText(value.toString());
        break;

    case AbstractIndexWidget::DisplayHexa:
        lineEdit()->setText("0x" + QString::number(value.toInt(), 16).toUpper());
        break;

    case AbstractIndexWidget::DisplayQ1_15:
    case AbstractIndexWidget::DisplayQ15_16:
        lineEdit()->setText(QString::number(value.toDouble() / 65536.0, 'g', 6));
        break;

    case AbstractIndexWidget::DisplayFloat:
        lineEdit()->setText(QString::number(value.toDouble(), 'g', 6));
        break;
    }
}

QVariant IndexSpinBox::value() const
{
    bool ok = false;
    QVariant value;
    switch (_hint)
    {
    case AbstractIndexWidget::DisplayDirectValue:
    case AbstractIndexWidget::DisplayHexa:
        value = QVariant(text().toInt(&ok, 0));
        break;

    case AbstractIndexWidget::DisplayQ15_16:
    case AbstractIndexWidget::DisplayQ1_15:
        value = QVariant(static_cast<uint>(text().toDouble(&ok) * 65536.0));
        break;

    case AbstractIndexWidget::DisplayFloat:
        value = QVariant(text().toDouble(&ok));
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
    switch (_hint)
    {
    case AbstractIndexWidget::DisplayDirectValue:
    case AbstractIndexWidget::DisplayHexa:
        setValue(value().toInt() + steps);
        break;

    case AbstractIndexWidget::DisplayQ15_16:
    case AbstractIndexWidget::DisplayQ1_15:
        setValue(value().toDouble() + steps);
        break;

    case AbstractIndexWidget::DisplayFloat:
        setValue(value().toDouble() + steps);
        break;
    }
}

QAbstractSpinBox::StepEnabled IndexSpinBox::stepEnabled() const
{
    return StepUpEnabled| StepDownEnabled;
}

void IndexSpinBox::updateHint()
{
    QRegularExpressionValidator *validator = nullptr;
    switch (_hint)
    {
    case AbstractIndexWidget::DisplayDirectValue:
        validator = new QRegularExpressionValidator(QRegularExpression("[0-9]+"));
        break;

    case AbstractIndexWidget::DisplayHexa:
        validator = new QRegularExpressionValidator(QRegularExpression("(0x)?[0-9A-F]+"));
        break;

    case AbstractIndexWidget::DisplayQ15_16:
    case AbstractIndexWidget::DisplayQ1_15:
    case AbstractIndexWidget::DisplayFloat:
        validator = new QRegularExpressionValidator(QRegularExpression("[0-9]*(\\.[0-9]*)?(e-?[0-9]*)?"));
        break;
    }
    lineEdit()->setValidator(validator);
}
