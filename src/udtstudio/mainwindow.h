#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTreeView>
#include <QSortFilterProxyModel>

#include "od/odtreeview.h"
#include "canopenbus.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ODTreeView *_odView;
    CanOpenBus *_bus;
};

#endif // MAINWINDOW_H
