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

#ifndef HEADERVIEW_H
#define HEADERVIEW_H

#include "udtgui_global.h"

#include <QHeaderView>

class UDTGUI_EXPORT HeaderView : public QHeaderView
{
    Q_OBJECT
public:
    HeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

    void addMandatorySection(int section);

private:
    QSet<int> _mandatorySection;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif  // HEADERVIEW_H
