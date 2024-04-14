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

#include "indexformlayout.h"

#include <QLabel>

IndexFormLayout::IndexFormLayout()
{
    setVerticalSpacing(3);
    setHorizontalSpacing(3);
}

void IndexFormLayout::addDualRow(const QString &labelText, QWidget *field, QWidget *subField, const QString &separator)
{
    QHBoxLayout *vLayout = new QHBoxLayout();
    vLayout->setSpacing(0);

    vLayout->addWidget(field);

    if (!separator.isEmpty())
    {
        QLabel *separatorLabel = new QLabel(separator);
        separatorLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        vLayout->addWidget(separatorLabel);
    }

    vLayout->addWidget(subField);

    QLabel *label = new QLabel(labelText);
    label->setBuddy(field);

    addRow(label, vLayout);
}

void IndexFormLayout::addLineSeparator()
{
    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    addRow(frame);
}
