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

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "canopen/indexWidget/abstractindexwidget.h"

class Node;
class Bootloader;
class IndexLabel;

class UDTGUI_EXPORT BootloaderWidget : public QWidget
{
    Q_OBJECT
public:
    BootloaderWidget(QWidget *parent = nullptr);

    Node *node() const;

public:
    void readAll();

public slots:
    void setNode(Node *node);

private slots:
    void sendKeyButton();
    void updateProgram();
    void openFile();

private:
    Node *_node;
    Bootloader *_bootloader;

    QPushButton *_stopButton;
    QPushButton *_startButton;
    QPushButton *_resetButton;
    QPushButton *_clearButton;
    QPushButton *_updateButton;
    QPushButton *_sendKeyButton;

    QString _fileName;

    // Create widgets
    void createWidgets();

    QLabel *_fileUfwLabel;
    QLabel *_deviceTypeUfwLabel;
    QLabel *_vendorIdUfwLabel;
    QLabel *_productCodeUfwLabel;
    QLabel *_revisionNumberUfwLabel;
    QLabel *_versionSoftwareUfwLabel;
    QLabel *_buildDateUfwLabel;

    QList<AbstractIndexWidget *> _indexWidgets;

    QWidget *informationDeviceWidget();
    QWidget *informationFileWidget();
};

#endif // BOOTLOADERWIDGET_H
