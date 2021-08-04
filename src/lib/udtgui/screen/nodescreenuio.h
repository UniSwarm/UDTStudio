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

#ifndef NODESCREENUIO_H
#define NODESCREENUIO_H

#include "udtgui_global.h"

#include "nodescreen.h"

#include <QSpinBox>
#include <QTabWidget>

class P401Widget;

class UDTGUI_EXPORT NodeScreenUio: public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenUio(QWidget *parent = nullptr);

public slots:
    void readAll();

protected slots:
    void toggleStartLogger(bool start);
    void setLogTimer(int ms);

protected:
    P401Widget *_p401Widget;

    // Logger timer
    QSpinBox *_logTimerSpinBox;

    // Toolbar action
    QAction *_startStopAction;
    QAction *_option402Action;

    void createWidgets();    

    // NodeScreen interface
public:
    QString title() const override;
    QIcon icon() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;
};

#endif // NODESCREENUIO_H
