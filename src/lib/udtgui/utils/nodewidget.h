#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include <QWidget>

#include "canopen/indexWidget/abstractindexwidget.h"

class NodeWidget : public QWidget
{
    Q_OBJECT
public:
    NodeWidget(QWidget *parent = nullptr);

    Node *node() const;
    virtual void setNode(Node *node);

    void readAll();
    void readDynamics();

protected:
    void addIndexWidget(AbstractIndexWidget *indexWidget);
    void adddynamicIndexWidget(AbstractIndexWidget *indexWidget);

    void updateObjects();

protected:
    Node *_node;
    QList<AbstractIndexWidget *> _indexWidgets;
    QList<AbstractIndexWidget *> _dynamicIndexWidgets;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif  // NODEWIDGET_H
