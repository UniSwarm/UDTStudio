#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTreeView>
#include <QSortFilterProxyModel>

#include "od/odtreeview.h"
#include "canopenbus.h"

#include "can/canFrameListView/canframelistview.h"
#include "canopen/busnodestreeview.h"
#include "can/canSettingsDialog/cansettingsdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createActions();
    void createMenus();

    CanSettingsDialog *_connectDialog;

    CanOpen *_canOpen;
    CanOpenBus *_bus;
    QCanBusDevice *_canDevice;

    CanFrameListView *_canFrameListView;
    BusNodesTreeView *_busNodeTreeView;
    ODTreeView *_odView;

    QWidget *widget;
    QMenu *_fileMenu;
    QMenu *_connectMenu;
    QAction *_quitAction;

    QAction *_connectAction;
    void connectDevice();

    QAction *_disconnectAction;
    void disconnectDevice();

    QAction *_canSettingsAction;

    QAction *_exploreBusAction;
    void exploreBus();

};

#endif // MAINWINDOW_H
