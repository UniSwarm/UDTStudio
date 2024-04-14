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

#include "nodeodwidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QStandardItemModel>

#include "node.h"

NodeOdWidget::NodeOdWidget(QWidget *parent)
    : NodeOdWidget(nullptr, parent)
{
}

NodeOdWidget::NodeOdWidget(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _oldProfile = 0;
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

NodeOdWidget::Filter NodeOdWidget::filter() const
{
    return _filter;
}

void NodeOdWidget::setFilter(NodeOdWidget::Filter filter)
{
    _filter = filter;
    if (_filter == FilterPDO)
    {
        QSignalBlocker block(_filterLineEdit);
        _filterLineEdit->setText(QStringLiteral("pdo:pdo "));
        _nodeOdTreeView->setFilter(QStringLiteral("pdo:pdo "));
    }
}

void NodeOdWidget::setNode(Node *node)
{
    _node = node;
    _nodeOdTreeView->setNode(node);
    if (node != nullptr)
    {
        createDefaultFilters(node->profileNumber());
    }
}

void NodeOdWidget::selectFilter(int index)
{
    QSignalBlocker blockLine(_filterLineEdit);
    QSignalBlocker blockCombo(_filterCombobox);
    QString filterString = _filterCombobox->itemData(index).toString();
    if (_filter == FilterPDO)
    {
        filterString.prepend(QStringLiteral("pdo:pdo "));
    }
    _filterLineEdit->setText(filterString);
    _nodeOdTreeView->setFilter(filterString);
    _filterCombobox->setCurrentIndex(index);
}

void NodeOdWidget::applyFilterCustom(const QString &filterText)
{
    QSignalBlocker block(_filterCombobox);
    _nodeOdTreeView->setFilter(filterText);
    _filterCombobox->setCurrentIndex(0);
}

void NodeOdWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // >> toolbar creation
    _toolBar = new QToolBar(tr("Node commands"));

    // filter preset combo
    QLabel *filterLabel = new QLabel(tr("Filter:"));
    filterLabel->setStyleSheet(QStringLiteral("QLabel {background: none}"));
    _toolBar->addWidget(filterLabel);
    _filterCombobox = new QComboBox();
    _toolBar->addWidget(_filterCombobox);
    _filterLineEdit = new QLineEdit();
    _filterLineEdit->setPlaceholderText(tr("Regexp filter..."));
    _filterLineEdit->setClearButtonEnabled(true);
    _filterLineEdit->setToolTip(tr("Start filter with '0x' to match index, otherwise, it will match object name column."));
    _toolBar->addWidget(_filterLineEdit);
    layout->addWidget(_toolBar);

    // base filters
    _filterCombobox->addItem(tr("All"), QVariant(".*"));
    _filterCombobox->addItem(tr("Com. objects"), QVariant("0x1[0-9A-F]{3}"));
    _filterCombobox->addItem(tr("Man. objects"), QVariant("0x[2-4][0-9A-F]{3}"));
    _filterCombobox->addItem(tr("Profile objects"), QVariant("0x[6-9][0-9A-F]{3}"));
    _filterCombobox->insertSeparator(1);
    connect(_filterCombobox,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [=](int index)
            {
                this->selectFilter(index);
            });
    connect(_filterLineEdit, &QLineEdit::textChanged, this, &NodeOdWidget::applyFilterCustom);

    _nodeOdTreeView = new NodeOdTreeView();
    layout->addWidget(_nodeOdTreeView);

    setLayout(layout);
}

NodeOdTreeView *NodeOdWidget::nodeOdTreeView() const
{
    return _nodeOdTreeView;
}

void NodeOdWidget::selectNodeObjectId(const NodeObjectId &objId)
{
    selectFilter(0);
    _nodeOdTreeView->selectNodeObjectId(objId);
}

void NodeOdWidget::createDefaultFilters(uint profile)
{
    if (profile == _oldProfile)
    {
        return;
    }
    int oldIndex = _filterCombobox->currentIndex();

    while (_filterCombobox->count() > 5)
    {
        _filterCombobox->removeItem(_filterCombobox->count() - 1);
    }

    // profile dependant filters
    switch (profile)
    {
        case 401:
            _filterCombobox->addItem(tr("401 DI"), QVariant("0x60[0-9A-F]{2}"));
            _filterCombobox->addItem(tr("401 DO"), QVariant("0x62[0-9A-F]{2}"));
            _filterCombobox->addItem(tr("401 AI"), QVariant("0x64[023][0-9A-F]"));
            _filterCombobox->addItem(tr("401 AO"), QVariant("0x64[14-5][0-9A-F]"));
            break;

        case 402:
            _filterCombobox->addItem(tr("402"));
            qobject_cast<QStandardItemModel *>(_filterCombobox->model())->item(_filterCombobox->count() - 1)->setEnabled(false);
            _filterCombobox->addItem(tr("402 FSA"), QVariant("0x[6-A][058D](0[27]|3F|4[01]|5[A-E]|6[01])"));
            _filterCombobox->addItem(tr("402 tp (Touch probe)"), QVariant("0x[6-A][08](B[8-D]|D0)"));
            _filterCombobox->addItem(tr("402 hm (Homming)"), QVariant("0x[6-A][08](9[89A]|7C|E3)"));
            _filterCombobox->addItem(tr("402 fg factors"), QVariant("0x[6-A][08](8F|9[0-267]|A[8-B])"));

            _filterCombobox->addItem(tr("OTHER MODE"));
            qobject_cast<QStandardItemModel *>(_filterCombobox->model())->item(_filterCombobox->count() - 1)->setEnabled(false);
            _filterCombobox->addItem(tr("402 dty (Duty cycle)"), QVariant("0x4[13579BDE][A-F]"));

            _filterCombobox->addItem(tr("TORQUE MODE"));
            qobject_cast<QStandardItemModel *>(_filterCombobox->model())->item(_filterCombobox->count() - 1)->setEnabled(false);
            _filterCombobox->addItem(tr("402 tq (Torque)"), QVariant("0x[6-A][08](7[1-9]|8[7-8])"));
            _filterCombobox->addItem(tr("402 cst (Cyclic Synchronous Torque)"), QVariant("0x[6-A][08](7[17]|B2)"));
            _filterCombobox->addItem(tr("402 cstca (CST with Commutation Angle)"), QVariant("0x[6-A][08](7[17]|B2|EA)"));

            _filterCombobox->addItem(tr("VELOCITY MODE"));
            qobject_cast<QStandardItemModel *>(_filterCombobox->model())->item(_filterCombobox->count() - 1)->setEnabled(false);
            _filterCombobox->addItem(tr("402 vl (Velocity)"), QVariant("0x[6-A][08]4[2-F]"));
            _filterCombobox->addItem(tr("402 pv (Profile Velocity)"), QVariant("0x[6-A][08](6[9-F]|70|F[8F])"));

            _filterCombobox->addItem(tr("POSITION MODE"));
            qobject_cast<QStandardItemModel *>(_filterCombobox->model())->item(_filterCombobox->count() - 1)->setEnabled(false);
            _filterCombobox->addItem(tr("402 pc (Position Control)"), QVariant("0x[6-A][08](6[2-8]|F[24AC])"));
            _filterCombobox->addItem(tr("402 pp (Profile Position)"), QVariant("0x[6-A][08](7[ABDEF]|8[0-6]|A[34]|C[56]|6[2-4])"));
            _filterCombobox->addItem(tr("402 ip (Interpolated Position)"), QVariant("0x[6-A][08](C[0-4]|7[B-F]|8[0-5]|C[5-6]|6[2-4])"));
            _filterCombobox->addItem(tr("402 cp (Continuous Position)"), QVariant("0x[6-A][08](7[BDEF]|8[01345]|C[56]|6[2-4])|0x4[1-8]F0"));
            break;
    }
    _oldProfile = profile;
    if (oldIndex > 3)
    {
        _filterCombobox->setCurrentIndex(0);
    }
}
