#include "canframemodel.h"
#include "canframelistview.h"

CanFrameModel::CanFrameModel()
{

}

CanFrameModel::~CanFrameModel()
{
}


int CanFrameModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant CanFrameModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case Time:
            return QVariant("Time");
        case CanId:
            return QVariant("CanId");
        case Type:
            return QVariant("Type");
        case DLC:
            return QVariant("DLC");
        case DataByte:
            return QVariant("DataByte");
        }
        break;
    }
    return QVariant();
}

void appendCanFrame(QCanBusFrame *frame)
{
//    frame->frameId();

//    const QString time = QString::fromLatin1("%1.%2  ")
//                             .arg(frame->timeStamp().seconds(), 10, 10, QLatin1Char(' '))
//                             .arg(frame->timeStamp().microSeconds() / 100, 4, 10, QLatin1Char('0'));

}


QVariant CanFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
    {
        return QVariant();
    }
    return QVariant();
}

bool CanFrameModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return false;
}

QModelIndex CanFrameModel::index(int row, int column, const QModelIndex &parent) const
{
        Q_UNUSED(row)
        Q_UNUSED(column)
        Q_UNUSED(parent)
        return QModelIndex();
}

QModelIndex CanFrameModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int CanFrameModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

Qt::ItemFlags CanFrameModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    Qt::ItemFlags flags;
    flags.setFlag(Qt::ItemIsEditable, false);

    return flags;
}
