#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QCanBus>

#include "canopen.h"
#include "can/canFrameListView/canframelistview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void explore();
    void sendState();
    void replySdo1000();

private:
    CanOpen *_canOpen;
    QCanBusDevice *_canDev;
    CanFrameListView *_canView;
    bool toggle;
    quint32 _value;
};

#endif // MAINWINDOW_H
