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

#include "indexcombobox.h"

#include <QApplication>
#include <QMainWindow>
#include <QStatusBar>

IndexComboBox::IndexComboBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    connect(this,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index)
            {
                setInternalIndex(index);
            });

    connect(this,
            QOverload<int>::of(&QComboBox::highlighted),
            [=](int index)
            {
                displayStatus(itemData(index, Qt::StatusTipRole).toString());
            });
}

void IndexComboBox::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        setFont(mfont);
    }

    int index = findData(value);
    if (index == -1)
    {
        QString text = "(" + QString::number(value.toInt()) + ")";
        setEditable(true);
        setCurrentText(text);
    }
    else
    {
        _internalUpdate = true;
        setCurrentIndex(index);
        _internalUpdate = false;
    }
}

bool IndexComboBox::isEditing() const
{
    return false;
}

void IndexComboBox::updateObjId()
{
    setToolTip(QString("0x%1.%2").arg(QString::number(objId().index(), 16).toUpper().rightJustified(4, '0'), QString::number(objId().subIndex()).toUpper().rightJustified(2, '0')));
}

void IndexComboBox::setInternalIndex(int index)
{
    if (!_internalUpdate)
    {
        requestWriteValue(itemData(index));
    }
}

QMainWindow *IndexComboBox::getMainWindow() const
{
    for (QWidget *w : QApplication::topLevelWidgets())
    {
        if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(w))
        {
            return mainWindow;
        }
    }
    return nullptr;
}

void IndexComboBox::displayStatus(const QString &message)
{
    QMainWindow *mainWindow = getMainWindow();
    if (!mainWindow)
    {
        return;
    }
    mainWindow->statusBar()->showMessage(message);
}

void IndexComboBox::clearStatus()
{
    QMainWindow *mainWindow = getMainWindow();
    if (!mainWindow)
    {
        return;
    }
    mainWindow->statusBar()->clearMessage();
}

void IndexComboBox::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    displayStatus(itemData(currentIndex(), Qt::StatusTipRole).toString());
}

void IndexComboBox::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    clearStatus();
}
