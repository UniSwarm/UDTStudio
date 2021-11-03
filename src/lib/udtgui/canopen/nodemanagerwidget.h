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

class UDTGUI_EXPORT NodeManagerWidget : public QWidget
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

    QAction *actionLoadEds() const;
    QAction *actionReLoadEds() const;

    QAction *actionRemoveNode() const;

    QAction *actionUpdateFirmware() const;

public slots:
    void setNode(Node *value);
    void updateData();

    void preop();
    void start();
    void stop();
    void resetCom();
    void resetNode();

    void removeNode();

    void updateNodeFirmware();

    void loadEds(const QString &edsFileName = QString());
    void reloadEds();

protected slots:
    void setNodeName();

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
    QLabel *_edsFileNameLabel;

    QAction *_actionLoadEds;
    QAction *_actionReLoadEds;

    QAction *_actionRemoveNode;

    QAction *_actionUpdateFirmware;

    Node *_node;
};

#endif  // NODEMANAGERWIDGET_H
