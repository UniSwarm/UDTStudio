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

#ifndef NODEMANAGERWIDGET_H
#define NODEMANAGERWIDGET_H

#include "../udtgui_global.h"

#include "nodeodsubscriber.h"
#include <QWidget>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolBar>

#include "node.h"
#include "widgetDebug/widgetdebug.h"

class UDTGUI_EXPORT NodeManagerWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    NodeManagerWidget(QWidget *parent = nullptr);
    NodeManagerWidget(Node *node, QWidget *parent = nullptr);

    Node *node() const;

    QAction *actionPreop() const;
    QAction *actionStart() const;
    QAction *actionStop() const;
    QAction *actionResetCom() const;
    QAction *actionReset() const;

signals:

public slots:
    void setNode(Node *value);
    void updateData();

    void preop();
    void start();
    void stop();
    void resetCom();
    void resetNode();

protected:
    void createWidgets();
    QToolBar *_toolBar;
    QActionGroup *_groupNmt;
    QAction *_actionPreop;
    QAction *_actionStart;
    QAction *_actionStop;
    QAction *_actionResetCom;
    QAction *_actionReset;
    QGroupBox *_groupBox;
    QLineEdit *_nodeNameEdit;
    QLabel *_nodeStatusLabel;

    Node *_node;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // NODEMANAGERWIDGET_H
