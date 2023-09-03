#include "nodewidget.h"

#include "node.h"

NodeWidget::NodeWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
}

Node *NodeWidget::node() const
{
    return _node;
}

void NodeWidget::setNode(Node *node)
{
    _node = node;

    if (node != nullptr)
    {
        connect(node, &Node::statusChanged, this, &NodeWidget::updateObjects);
    }
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }
}

void NodeWidget::readAll()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void NodeWidget::readDynamics()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_dynamicIndexWidgets))
    {
        indexWidget->readObject();
    }
}

void NodeWidget::addIndexWidget(AbstractIndexWidget *indexWidget)
{
    _indexWidgets.append(indexWidget);
}

void NodeWidget::adddynamicIndexWidget(AbstractIndexWidget *indexWidget)
{
    _indexWidgets.append(indexWidget);
    _dynamicIndexWidgets.append(indexWidget);
}

void NodeWidget::updateObjects()
{
    if (_node == nullptr || _indexWidgets.isEmpty())
    {
        return;
    }

    if (_node->nodeOd()->subIndex(_indexWidgets.first()->objId())->lastModification().isNull()
        && (_node->status() == Node::Status::PREOP || _node->status() == Node::Status::STARTED) && isVisible())
    {
        QTimer::singleShot(100, this, &NodeWidget::readAll);
    }
}

void NodeWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    updateObjects();
}
