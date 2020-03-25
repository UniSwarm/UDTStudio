#include "busnodesmodel.h"

#include <QDebug>

BusNodesModel::BusNodesModel(QObject *parent, CanOpen *canOpen)
    : QAbstractItemModel(parent), _canOpen(canOpen)
{
}

CanOpen *BusNodesModel::canOpen() const
{
    return _canOpen;
}

void BusNodesModel::setCanOpen(CanOpen *canOpen)
{
    emit layoutAboutToBeChanged();
    _canOpen = canOpen;
    emit layoutChanged();
}

CanOpenBus *BusNodesModel::bus(const QModelIndex &index) const
{
    CanOpenBus *bus = qobject_cast<CanOpenBus *>(static_cast<QObject *>(index.internalPointer()));
    if (bus)
    {
        return bus;
    }
    return nullptr;
}

BusNodesModel::~BusNodesModel()
{
}

int BusNodesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant BusNodesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        return QVariant();
    }
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case NodeId:
            return QVariant(tr("NodeId"));
        case Name:
            return QVariant(tr("Name"));
        }
        break;
    }
    return QVariant();
}

QVariant BusNodesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    CanOpenBus *bus = qobject_cast<CanOpenBus *>(static_cast<QObject *>(index.internalPointer()));
    if (bus != nullptr)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            switch (index.column())
            {
            case NodeId:
                return QVariant(bus->busName());
            case Name:
                return QVariant(bus->busName());
            default:
                return QVariant();
            }
        }
    }

    Node *node = qobject_cast<Node *>(static_cast<QObject *>(index.internalPointer()));
    if (node != nullptr)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            switch (index.column())
            {
            case NodeId:
                return QVariant(node->nodeId());
            case Name:
                return QVariant(node->name());
            default:
                return QVariant();
            }
        }
    }
    return QVariant();
}

QModelIndex BusNodesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!_canOpen)
    {
        return QModelIndex();
    }

    if (parent.internalPointer() == nullptr)
    {
        if (row >= _canOpen->buses().count())
        {
            return QModelIndex();
        }
        return createIndex(row, column, _canOpen->buses().at(row));
    }

    CanOpenBus *bus = qobject_cast<CanOpenBus *>(static_cast<QObject *>(parent.internalPointer()));
    if (bus != nullptr)
    {
        if (row < bus->nodes().count())
        {
            return createIndex(row, column, bus->nodes().at(row));
        }
        return QModelIndex();
    }
    return QModelIndex();
}

QModelIndex BusNodesModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalPointer() == nullptr)
    {
        return QModelIndex();
    }

    Node *node = qobject_cast<Node *>(static_cast<QObject *>(child.internalPointer()));
    if (node != nullptr)
    {
        QModelIndex index = createIndex(node->bus()->nodes().indexOf(node), 0, node->bus());
        return index;
    }

    return QModelIndex();
}

int BusNodesModel::rowCount(const QModelIndex &parent) const
{
    if (!_canOpen)
    {
        return 0;
    }
    if (!parent.isValid())
    {
        return _canOpen->buses().count();
    }

    CanOpenBus *bus = qobject_cast<CanOpenBus *>(static_cast<QObject *>(parent.internalPointer()));
    if (bus)
    {
        return bus->nodes().count();
    }
    return 0;
}

Qt::ItemFlags BusNodesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags;
    flags.setFlag(Qt::ItemIsEditable, false);
    flags.setFlag(Qt::ItemIsSelectable, true);
    flags.setFlag(Qt::ItemIsEnabled, true);

    return flags;
}
