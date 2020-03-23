#include "canframelistview.h"

CanFrameListView::CanFrameListView(QWidget *parent)
{
    _canModel = new CanFrameModel();
    setModel(_canModel);
}

void CanFrameListView::appendCanFrame(QCanBusFrame *frame)
{
    //_canModel->appendCanFrame(frame);
}

CanFrameListView::~CanFrameListView()
{
    delete _canModel;
}
