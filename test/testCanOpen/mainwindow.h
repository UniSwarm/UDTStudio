#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "canopenbus.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void pushButton_clicked();

private:
    CanOpenBus *_bus;
    QPushButton *_button;
};

#endif // MAINWINDOW_H
