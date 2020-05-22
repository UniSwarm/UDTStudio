#include "nodeoditemdelegate.h"

#include <QDebug>
#include <QEvent>
#include <QLineEdit>

NodeOdItemDelegate::NodeOdItemDelegate(QObject *parent)
    :QItemDelegate(parent)
{
}

bool NodeOdItemDelegate::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        qobject_cast<QLineEdit*>(watched)->setMaxLength(0);
    }
    bool ok = QItemDelegate::eventFilter(watched, event);
    return ok;
}
