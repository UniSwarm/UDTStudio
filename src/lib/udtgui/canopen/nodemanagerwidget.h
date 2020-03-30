/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#ifndef NODEMANAGERWIDGET_H
#define NODEMANAGERWIDGET_H

#include "../udtgui_global.h"

#include <QWidget>

#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolBar>

#include "node.h"

class UDTGUI_EXPORT NodeManagerWidget : public QWidget
{
    Q_OBJECT
public:
    NodeManagerWidget(QWidget *parent = nullptr);
    NodeManagerWidget(Node *node, QWidget *parent = nullptr);

    Node *node() const;

    static void snotify(void* object, quint16 index, quint8 subindexDevice, const QByteArray &data);
    void notify(quint16 index, quint8 subindexDevice, const QByteArray &data);

signals:

public slots:
    void setNode(Node *value);
    void updateData();

    void start();
    void stop();
    void resetCom();
    void resetNode();
    void test();

protected:
    void createWidgets();
    QToolBar *_toolBar;
    QGroupBox *_groupBox;
    QLineEdit *_nodeNameEdit;
    QLabel *_index1000Label;
    QLabel *_nodeStatusLabel;

    Node *_node;
};

#endif // NODEMANAGERWIDGET_H
