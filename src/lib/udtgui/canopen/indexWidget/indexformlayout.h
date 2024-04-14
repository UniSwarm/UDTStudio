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

#ifndef INDEXFORMLAYOUT_H
#define INDEXFORMLAYOUT_H

#include "../../udtgui_global.h"

#include <QFormLayout>

class UDTGUI_EXPORT IndexFormLayout : public QFormLayout
{
    Q_OBJECT
public:
    IndexFormLayout();

    void addDualRow(const QString &labelText, QWidget *field, QWidget *subField, const QString &separator = QString());

    void addLineSeparator();
};

#endif  // INDEXFORMLAYOUT_H
