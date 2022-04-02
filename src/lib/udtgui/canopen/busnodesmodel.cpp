#include "busnodesmodel.h"

#include <QIcon>

BusNodesModel::BusNodesModel(QObject *parent)
    : BusNodesModel(nullptr, parent)
{
}

BusNodesModel::BusNodesModel(CanOpen *canOpen, QObject *parent)
    : QAbstractItemModel(parent)
    , _canOpen(canOpen)
{
}

BusNodesModel::~BusNodesModel()
{
}

CanOpen *BusNodesModel::canOpen() const
{
    return _canOpen;
}

void BusNodesModel::setCanOpen(CanOpen *canOpen)
{
    emit layoutAboutToBeChanged();
    if (_canOpen != nullptr)
    {
        disconnect(_canOpen, nullptr, this, nullptr);
    }

    _canOpen = canOpen;
    if (_canOpen != nullptr)
    {
        connect(_canOpen, &CanOpen::busAboutToBeAdded, this, &BusNodesModel::prepareAddBus);
        connect(_canOpen, &CanOpen::busAdded, this, &BusNodesModel::addBus);
        connect(_canOpen, &CanOpen::busAboutToBeRemoved, this, &BusNodesModel::removeBus);
        connect(_canOpen,
                &CanOpen::busRemoved,
                [=]()
                {
                    endRemoveRows();
                });
    }
    emit layoutChanged();
}

CanOpenBus *BusNodesModel::bus(const QModelIndex &index) const
{
    CanOpenBus *bus = qobject_cast<CanOpenBus *>(static_cast<QObject *>(index.internalPointer()));
    if (bus != nullptr)
    {
        return bus;
    }
    Node *nodeptr = node(index);
    if (nodeptr != nullptr)
    {
        return nodeptr->bus();
    }
    return nullptr;
}

Node *BusNodesModel::node(const QModelIndex &index) const
{
    Node *node = qobject_cast<Node *>(static_cast<QObject *>(index.internalPointer()));
    if (node != nullptr)
    {
        return node;
    }
    return nullptr;
}

void BusNodesModel::prepareAddBus(quint8 busId)
{
    Q_UNUSED(busId);
    int indexBus = CanOpen::buses().count();
    beginInsertRows(QModelIndex(), indexBus, indexBus);
}

void BusNodesModel::addBus(quint8 busId)
{
    endInsertRows();

    CanOpenBus *bus = CanOpen::bus(busId);
    if (bus == nullptr)
    {
        return;
    }
    connect(bus,
            &CanOpenBus::nodeAboutToBeAdded,
            [=](int nodeId)
            {
                prepareAddNode(bus, nodeId);
            });
    connect(bus,
            &CanOpenBus::nodeAdded,
            [=](int nodeId)
            {
                addNode(bus, nodeId);
            });
    connect(bus,
            &CanOpenBus::nodeAboutToBeRemoved,
            [=](int nodeId)
            {
                removeNode(bus, nodeId);
            });
    connect(bus,
            &CanOpenBus::nodeRemoved,
            [=]()
            {
                endRemoveRows();
            });
    connect(bus,
            &CanOpenBus::busNameChanged,
            [=]()
            {
                updateBus(bus, Name);
            });
    connect(bus,
            &CanOpenBus::connectedChanged,
            [=]()
            {
                updateBus(bus, Status);
            });
}

void BusNodesModel::removeBus(quint8 busId)
{
    CanOpenBus *bus = CanOpen::bus(busId);
    if (bus == nullptr)
    {
        return;
    }
    disconnect(bus, nullptr, this, nullptr);

    int indexBus = CanOpen::buses().indexOf(bus);
    beginRemoveRows(QModelIndex(), indexBus, indexBus);
    endRemoveRows();
}

void BusNodesModel::updateBus(CanOpenBus *bus, quint8 column)
{
    int indexBus = CanOpen::buses().indexOf(bus);
    QModelIndex modelIndex = index(indexBus, column, QModelIndex());
    emit dataChanged(modelIndex, modelIndex);
}

void BusNodesModel::prepareAddNode(CanOpenBus *bus, quint8 nodeId)
{
    Q_UNUSED(nodeId)
    int indexBus = CanOpen::buses().indexOf(bus);
    int indexNode = bus->nodes().count();
    beginInsertRows(index(indexBus, 0, QModelIndex()), indexNode, indexNode);
}

void BusNodesModel::addNode(CanOpenBus *bus, quint8 nodeId)
{
    endInsertRows();

    Node *node = bus->node(nodeId);
    if (node == nullptr)
    {
        return;
    }

    connect(node,
            &Node::nameChanged,
            [=]()
            {
                updateNode(node, Name);
            });
    connect(node,
            &Node::statusChanged,
            [=]()
            {
                updateNode(node, Status);
            });
}

void BusNodesModel::removeNode(CanOpenBus *bus, quint8 nodeId)
{
    Node *node = bus->node(nodeId);
    if (node == nullptr)
    {
        return;
    }
    int indexBus = CanOpen::buses().indexOf(bus);
    int indexNode = bus->nodes().indexOf(node);
    beginRemoveRows(index(indexBus, 0, QModelIndex()), indexNode, indexNode);
}

void BusNodesModel::updateNode(Node *node, quint8 column)
{
    int indexBus = CanOpen::buses().indexOf(node->bus());
    int indexNode = node->bus()->nodes().indexOf(node);

    QModelIndex modelIndexBus = index(indexBus, 0, QModelIndex());
    QModelIndex modelIndex = index(indexNode, column, modelIndexBus);
    emit dataChanged(modelIndex, modelIndex);
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
                    return QVariant(tr("Id"));
                case Name:
                    return QVariant(tr("Name"));
                case Status:
                    return QVariant(tr("Status"));
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
                        return QVariant(index.row());
                    case Name:
                        return QVariant(bus->busName());
                    case Status:
                        return QVariant(bus->isConnected() ? tr("connected") : tr("unconnected"));
                    default:
                        return QVariant();
                }

            case Qt::DecorationRole:
                if (index.column() == NodeId)
                {
                    return QVariant(QIcon(":/uBoards/connexion-usb.png"));
                }
                return QVariant();

            case Qt::TextAlignmentRole:
                if (index.column() == NodeId)
                {
                    return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                }
                return QVariant();
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
                    case Status:
                        return QVariant(node->statusStr());
                    default:
                        return QVariant();
                }

            case Qt::DecorationRole:
                if (index.column() == NodeId)
                {
                    if (node->manufacturerId() == 0x04A2)  // UniSwarm
                    {
                        switch (node->profileNumber())
                        {
                            case 401:
                            case 428:
                                return QVariant(QIcon(":/uBoards/uio.png"));
                            case 402:
                                return QVariant(QIcon(":/uBoards/umc.png"));
                        }
                    }
                }
                return QVariant();

            case Qt::TextAlignmentRole:
                if (index.column() == NodeId)
                {
                    return QVariant(Qt::AlignRight | Qt::AlignVCenter);
                }
                return QVariant();
        }
    }
    return QVariant();
}

QModelIndex BusNodesModel::index(int row, int column, const QModelIndex &parent) const
{
    if (_canOpen == nullptr)
    {
        return QModelIndex();
    }

    if (parent.internalPointer() == nullptr)
    {
        if (row >= CanOpen::buses().count())
        {
            return QModelIndex();
        }
        return createIndex(row, column, CanOpen::buses().at(row));
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
    if (_canOpen == nullptr)
    {
        return 0;
    }
    if (!parent.isValid())
    {
        return CanOpen::buses().count();
    }

    CanOpenBus *bus = qobject_cast<CanOpenBus *>(static_cast<QObject *>(parent.internalPointer()));
    if (bus != nullptr)
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
