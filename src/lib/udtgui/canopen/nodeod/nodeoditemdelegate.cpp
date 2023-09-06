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

#include "nodeoditemdelegate.h"

#include <QDebug>
#include <QEvent>
#include <QLineEdit>

#include "nodeodfilterproxymodel.h"
#include "nodeoditemmodel.h"

NodeOdItemDelegate::NodeOdItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

bool NodeOdItemDelegate::eventFilter(QObject *editor, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        qobject_cast<QLineEdit *>(editor)->setMaxLength(0);
    }
    if (event->type() == QEvent::FocusIn)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
        lineEdit->setSelection(0, lineEdit->text().size() - _currentEditUnit.size());
        return false;
    }
    return QItemDelegate::eventFilter(editor, event);
}

void NodeOdItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!editor->hasFocus())
    {
        QVariant v = index.data(Qt::EditRole);
        qobject_cast<QLineEdit *>(editor)->setText(v.toString() + _currentEditUnit);
    }
}

QWidget *NodeOdItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = QItemDelegate::createEditor(parent, option, index);
    if (index.column() == NodeOdItemModel::Value)
    {
        const NodeOdFilterProxyModel *itemModel = dynamic_cast<const NodeOdFilterProxyModel *>(index.model());
        NodeSubIndex *subIndex = itemModel->nodeSubIndex(index);
        _currentEditUnit = subIndex->unit();

        QString regExpStr = QStringLiteral("^(?:0x|\\-)?(?:(?:[0-9]+\\.?)(?:[0-9]+)?|\\.[0-9]+)");
        if (!_currentEditUnit.isEmpty())
        {
            regExpStr.append("(?:" + QRegularExpression::escape(_currentEditUnit) + ")?");
        }
        regExpStr.append('$');

        QRegularExpressionValidator *validator = new QRegularExpressionValidator(QRegularExpression(regExpStr));
        qobject_cast<QLineEdit *>(editor)->setValidator(validator);
    }
    else
    {
        _currentEditUnit.clear();
    }
    return editor;
}
