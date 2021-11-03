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

#ifndef PDOMAPPINGWIDGET_H
#define PDOMAPPINGWIDGET_H

#include "../../udtgui_global.h"

#include <QWidget>

#include "pdomappingview.h"

#include <QLabel>
#include <QToolBar>

class UDTGUI_EXPORT PDOMappingWidget : public QWidget
{
    Q_OBJECT
public:
    PDOMappingWidget(QWidget *parent = nullptr);

    PDO *pdo() const;

public slots:
    void setPdo(PDO *pdo);

    void clearMapping();
    void setEnabled(bool enabled);

protected slots:
    void updateEnabled(bool enabled);

protected:
    PDO *_pdo;

    void createWidget();
    QLabel *_pdoNameLabel;
    QToolBar *_toolBar;
    QAction *_enableAction;
    QLabel *_pdoIndexLabel;
    PDOMappingView *_pdoMappingView;
};

#endif  // PDOMAPPINGWIDGET_H
