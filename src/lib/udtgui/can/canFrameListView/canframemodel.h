#ifndef CANFRAMEMODEL_H
#define CANFRAMEMODEL_H

#include <QAbstractItemModel>
#include <QCanBus>
#include <QCanBusFrame>

class CanFrameModel : public QAbstractItemModel
{
  public:
    CanFrameModel();
    ~CanFrameModel();

    enum Column {
        Time,
        CanId,
        Type,
        DLC,
        DataByte,
        ColumnCount
    };
    // QAbstractItemModel interface
  public:

    void appendCanFrame(QCanBusFrame *frame);

    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

  private:


};

#endif // CANFRAMEMODEL_H
