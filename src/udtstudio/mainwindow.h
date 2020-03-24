#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTreeView>
#include <QSortFilterProxyModel>

#include "od/odtreeview.h"
#include "canopenbus.h"

#include "can/canFrameListView/canframelistview.h"
#include "canopen/busnodestreeview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    CanFrameListView *_canFrameListView;
    BusNodesTreeView *_busNodeTreeView;
    ODTreeView *_odView;

    CanOpen *_canOpen;
    CanOpenBus *_bus;
};

#endif // MAINWINDOW_H
