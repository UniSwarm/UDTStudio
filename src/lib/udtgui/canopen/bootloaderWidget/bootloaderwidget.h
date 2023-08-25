/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef BOOTLOADERWIDGET_H
#define BOOTLOADERWIDGET_H

#include "udtgui_global.h"

#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

#include "canopen/indexWidget/abstractindexwidget.h"

// #define DEBUG_BOOT

class Node;
class Bootloader;
class IndexLabel;

class UDTGUI_EXPORT BootloaderWidget : public QDialog
{
    Q_OBJECT
public:
    BootloaderWidget(QWidget *parent = nullptr);
    BootloaderWidget(Node *node, QWidget *parent = nullptr);

    Node *node() const;

public:
    void readAll();

public slots:
    void setNode(Node *node);

private slots:
    void updateProgram();
    void updateStatus();
    void openFile();

    void sendKeyButton();

private:
    Node *_node;

    QLabel *_infoLabel;
    QString _fileName;

#ifdef DEBUG_BOOT
    QPushButton *_stopButton;
    QPushButton *_startButton;
    QPushButton *_resetButton;
    QPushButton *_clearButton;
    QPushButton *_sendKeyButton;
#endif

    // Create widgets
    void createWidgets();

    QList<AbstractIndexWidget *> _indexWidgets;

    QGroupBox *informationDeviceWidget();

    QGroupBox *informationFileWidget();
    QPushButton *_openButton;
    QLabel *_fileUfwLabel;
    QLabel *_deviceTypeUfwLabel;
    QLabel *_versionSoftwareUfwLabel;
    QLabel *_buildDateUfwLabel;

    QLabel *_statusLabel;
    QPushButton *_updateButton;
    QProgressBar *_updateProgressBar;
    QTimer _progressTimer;
};

#endif  // BOOTLOADERWIDGET_H
