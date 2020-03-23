#ifndef CANFRAMEVIEW_H
#define CANFRAMEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QCanBus>
#include <QCanBusFrame>

#include "canframemodel.h"

class CanFrameListView : public QTableView
{
  public:
    CanFrameListView(QWidget *parent = 0);
    ~CanFrameListView();
    void appendCanFrame(QCanBusFrame *frame);

  protected:
    CanFrameModel *_canModel;
};

#endif // CANFRAMEVIEW_H
