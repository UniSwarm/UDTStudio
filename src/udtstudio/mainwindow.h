#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTreeView>
#include <QSortFilterProxyModel>

#include "od/odtreeview.h"
#include "canopenbus.h"

#include "can/canFrameListView/canframelistview.h"
#include "canopen/busnodesmanagerview.h"
#include "can/canSettingsDialog/cansettingsdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    CanSettingsDialog *_connectDialog;

    CanOpen *_canOpen;
    CanOpenBus *_bus;
    QCanBusDevice *_canDevice;

    void createDocks();
    QDockWidget *_busNodesManagerDock;
    BusNodesManagerView *_busNodesManagerView;

    void createWidgets();
    CanFrameListView *_canFrameListView;
    ODTreeView *_odView;

    void createActions();
    void createMenus();
    QWidget *widget;
    QMenu *_fileMenu;
    QMenu *_connectMenu;
    QAction *_quitAction;

    QAction *_connectAction;
    void connectDevice();

    QAction *_disconnectAction;
    void disconnectDevice();

    QAction *_canSettingsAction;
};

#endif // MAINWINDOW_H
