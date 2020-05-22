#ifndef NODEODITEMDELEGATE_H
#define NODEODITEMDELEGATE_H

#include <QItemDelegate>

class NodeOdItemDelegate : public QItemDelegate
{
public:
    NodeOdItemDelegate(QObject *parent = nullptr);

    // QObject interface
public:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // NODEODITEMDELEGATE_H
