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

#ifndef NODEODFILTERPROXYMODEL_H
#define NODEODFILTERPROXYMODEL_H

#include "../../udtgui_global.h"

#include <QSortFilterProxyModel>

class UDTGUI_EXPORT NodeOdFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    NodeOdFilterProxyModel(QObject *parent = nullptr);
    ~NodeOdFilterProxyModel();

    enum PDOFilter
    {
        PDOFILTER_ALL,
        PDOFILTER_PDO,
        PDOFILTER_RPDO,
        PDOFILTER_TPDO
    };
    PDOFilter pdoFilter() const;
    void setPdoFilter(PDOFilter pdoFilter);

protected:
    PDOFilter _pdoFilter;

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // NODEODFILTERPROXYMODEL_H
