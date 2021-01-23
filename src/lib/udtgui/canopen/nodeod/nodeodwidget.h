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

#ifndef NODEODWIDGET_H
#define NODEODWIDGET_H

#include "../../udtgui_global.h"

#include <QWidget>

#include "nodeodtreeview.h"
#include <QToolBar>
#include <QComboBox>
#include <QLineEdit>

class UDTGUI_EXPORT NodeOdWidget : public QWidget
{
    Q_OBJECT
public:
    NodeOdWidget(QWidget *parent = nullptr);
    NodeOdWidget(Node *node, QWidget *parent = nullptr);
    ~NodeOdWidget();

    Node *node() const;

    enum Filter
    {
        FilterNone,
        FilterPDO
    };
    Filter filter() const;
    void setFilter(Filter filter);

public slots:
    void setNode(Node *node);

protected slots:
    void selectFilter(int index);
    void applyFilterCustom(const QString &filterText);

protected:
    void createWidgets();
    QToolBar *_toolBar;
    QComboBox *_filterCombobox;
    QLineEdit *_filterLineEdit;
    NodeOdTreeView *_nodeOdTreeView;

    void createDefaultFilters(uint profile = 0);
    uint _oldProfile;

    Node *_node;
    Filter _filter;
};

#endif // NODEODWIDGET_H
