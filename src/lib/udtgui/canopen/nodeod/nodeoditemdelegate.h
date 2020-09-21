#ifndef NODEODITEMDELEGATE_H
#define NODEODITEMDELEGATE_H

#include "../../udtgui_global.h"

#include <QItemDelegate>

class UDTGUI_EXPORT NodeOdItemDelegate : public QItemDelegate
{
public:
    NodeOdItemDelegate(QObject *parent = nullptr);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;

    // QAbstractItemDelegate interface
public:
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
};

#endif // NODEODITEMDELEGATE_H
