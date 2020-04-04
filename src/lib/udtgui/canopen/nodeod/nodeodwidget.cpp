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

#include "nodeodwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

#include "node.h"

NodeOdWidget::NodeOdWidget(QWidget *parent)
    : NodeOdWidget(nullptr, parent)
{
    _oldProfile = 0;
}

NodeOdWidget::NodeOdWidget(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    createWidgets();
    setNode(node);
}

NodeOdWidget::~NodeOdWidget()
{
}

Node *NodeOdWidget::node() const
{
    return _node;
}

void NodeOdWidget::setNode(Node *node)
{
    _node = node;
    _nodeOdTreeView->setNode(node);
    if (node)
    {
        createDefaultFilters(node->nodeOd()->value(0x1000).toUInt() & 0xFFFF);
    }
}

void NodeOdWidget::selectFilter(int index)
{
    _filterLineEdit->setText(_filterCombobox->itemData(index).toString());
}

void NodeOdWidget::applyFilter(const QString &filterText)
{
    _nodeOdTreeView->setFilter(filterText);
}

void NodeOdWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();

    // >> toolbar creation
    _toolBar = new QToolBar(tr("Node commands"));

    // filter preset combo
    _toolBar->addWidget(new QLabel(tr("Filter ")));
    _filterCombobox = new QComboBox();
    _toolBar->addWidget(_filterCombobox);
    _filterLineEdit = new QLineEdit();
    _toolBar->addWidget(_filterLineEdit);
    layout->addWidget(_toolBar);

    // base filters
    _filterCombobox->addItem(tr("All"), QVariant(".*"));
    _filterCombobox->addItem(tr("Com. objects"), QVariant("1[0-9A-F]{3}"));
    _filterCombobox->addItem(tr("Man. objects"), QVariant("[2-4][0-9A-F]{3}"));
    _filterCombobox->addItem(tr("Profile objects"), QVariant("[6-9][0-9A-F]{3}"));
    _filterCombobox->insertSeparator(20);
    connect(_filterCombobox, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index){this->selectFilter(index);});
    connect(_filterLineEdit, &QLineEdit::textChanged, this, &NodeOdWidget::applyFilter);

    _nodeOdTreeView = new NodeOdTreeView();
    layout->addWidget(_nodeOdTreeView);

    setLayout(layout);
}

void NodeOdWidget::createDefaultFilters(uint profile)
{
    if (profile == _oldProfile)
    {
        return;
    }

    while (_filterCombobox->count() > 5)
    {
        _filterCombobox->removeItem(_filterCombobox->count() - 1);
    }

    // profile dependant filters
    switch (profile)
    {
    case 401:
        _filterCombobox->addItem(tr("401 DI"), QVariant("60[0-9A-F]{2}"));
        _filterCombobox->addItem(tr("401 DO"), QVariant("62[0-9A-F]{2}"));
        _filterCombobox->addItem(tr("401 AI"), QVariant("64[023][0-9A-F]"));
        _filterCombobox->addItem(tr("401 AO"), QVariant("64[14-5][0-9A-F]"));
        break;

    case 402:
        _filterCombobox->addItem(tr("402 FSA"), QVariant("60(0[27]|3F|4[01]|5[A-E]|6[01])"));
        _filterCombobox->addItem(tr("402 pp"), QVariant("60(7[ABD]|8[0-6]|A[34]|C[56])"));
        _filterCombobox->addItem(tr("402 hm"), QVariant("60(9[89A]|7C|E3)"));
        _filterCombobox->addItem(tr("402 tp"), QVariant("60(B[8-D]|D0)"));
        _filterCombobox->addItem(tr("402 pc"), QVariant("60(6[2-8]|F[24AC])"));
        _filterCombobox->addItem(tr("402 ip"), QVariant("60C[0-4]"));
        _filterCombobox->addItem(tr("402 pv"), QVariant("60(6[9-F]|70|F[8F])"));
        _filterCombobox->addItem(tr("402 pt"), QVariant("60(7[1-9]|E[01])"));
        _filterCombobox->addItem(tr("402 vl"), QVariant("604[2-F]"));
        _filterCombobox->addItem(tr("402 factors"), QVariant("60(8F|9[0-267]|A[8-B])"));
        break;
    }
    _oldProfile = profile;
}
