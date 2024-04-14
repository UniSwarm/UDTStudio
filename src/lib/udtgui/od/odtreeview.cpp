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

#include "odtreeview.h"

ODTreeView::ODTreeView(QWidget *parent)
    : QTreeView(parent)
{
    _odModel = new ODItemModel();
    _odModelSorter = new QSortFilterProxyModel();
    _odModelSorter->setSourceModel(_odModel);
    setModel(_odModelSorter);

    /*
    _odModelSorter->setFilterRegularExpression(QRegularExpression("0x1[0-9]{3}"));
    _odModelSorter->setFilterKeyColumn(ODItemModel::OdIndex);
*/

    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

ODTreeView::~ODTreeView()
{
    delete _odModelSorter;
    delete _odModel;
}

void ODTreeView::setDeviceModel(DeviceModel *deviceModel)
{
    _odModel->setDeviceModel(deviceModel);
    _odModelSorter->setSourceModel(_odModel);

    expandToDepth(0);
    expandToDepth(1);
    expandToDepth(2);
    collapseAll();
}

DeviceModel *ODTreeView::deviceModel() const
{
    return _odModel->deviceModel();
}

bool ODTreeView::editable() const
{
    return _odModel->editable();
}

void ODTreeView::setEditable(bool editable)
{
    _odModel->setEditable(editable);
}
