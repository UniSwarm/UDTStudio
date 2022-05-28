#include "busnodesmodel.h"

#include <QIcon>
#include <QPainter>

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
                this,
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
            this,
            [=](int nodeId)
            {
                prepareAddNode(bus, nodeId);
            });
    connect(bus,
            &CanOpenBus::nodeAdded,
            this,
            [=](int nodeId)
            {
                addNode(bus, nodeId);
            });
    connect(bus,
            &CanOpenBus::nodeAboutToBeRemoved,
            this,
            [=](int nodeId)
            {
                removeNode(bus, nodeId);
            });
    connect(bus,
            &CanOpenBus::nodeRemoved,
            this,
            [=]()
            {
                endRemoveRows();
            });
    connect(bus,
            &CanOpenBus::busNameChanged,
            this,
            [=]()
            {
                updateBus(bus, Name);
            });
    connect(bus,
            &CanOpenBus::connectedChanged,
            this,
            [=]()
            {
                updateBus(bus, Status);
                updateBus(bus, NodeId);
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
            this,
            [=]()
            {
                updateNode(node, Name);
            });
    connect(node,
            &Node::statusChanged,
            this,
            [=]()
            {
                updateNode(node, Status);
                updateNode(node, NodeId);
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
                    QPixmap pixmap;

                    pixmap = QPixmap(":/uBoards/usb.png");

                    if (pixmap.isNull())
                    {
                        pixmap = QPixmap(":/uBoards/unknown.png");
                    }

                    QPainter painter(&pixmap);
                    if (bus->isConnected())
                    {
                        painter.setBrush(QColor(0, 255, 0));
                        painter.drawEllipse(pixmap.height() * .6, pixmap.width() * .6, pixmap.height() * .4, pixmap.width() * .4);
                    }
                    else
                    {
                        painter.setBrush(QColor(255, 0, 0));
                        painter.drawEllipse(pixmap.height() * .6, pixmap.width() * .6, pixmap.height() * .4, pixmap.width() * .4);
                    }

                    return QVariant(QIcon(pixmap));
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
                    QPixmap pixmap;
                    if (node->manufacturerId() == 0x04A2)  // UniSwarm
                    {
                        switch (node->profileNumber())
                        {
                            case 401:
                            case 428:
                                pixmap = QPixmap(":/uBoards/uio.png");
                                break;

                            case 402:
                                pixmap = QPixmap(":/uBoards/umc.png");
                                break;
                        }
                    }

                    if (pixmap.isNull())
                    {
                        pixmap = QPixmap(":/uBoards/unknown.png");
                    }

                    QPainter painter(&pixmap);
                    switch (node->status())
                    {
                        case Node::PREOP:
                            painter.setBrush(QColor(255, 140, 0));
                            painter.drawEllipse(pixmap.height() * .6, pixmap.width() * .6, pixmap.height() * .4, pixmap.width() * .4);
                            break;

                        case Node::STARTED:
                            painter.setBrush(QColor(0, 255, 0));
                            painter.drawEllipse(pixmap.height() * .6, pixmap.width() * .6, pixmap.height() * .4, pixmap.width() * .4);
                            break;

                        case Node::STOPPED:
                            painter.setBrush(QColor(255, 0, 0));
                            painter.drawEllipse(pixmap.height() * .6, pixmap.width() * .6, pixmap.height() * .4, pixmap.width() * .4);
                            break;

                        default:
                            break;
                    }

                    return QVariant(QIcon(pixmap));
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
