/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef P402WIDGET_H
#define P402WIDGET_H

#include "../../../udtgui_global.h"

#include <QWidget>

#include "p402modewidget.h"
#include "profile/p402/nodeprofile402.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QToolBar>

class IndexLabel;

class UDTGUI_EXPORT P402Widget : public QWidget
{
    Q_OBJECT
public:
    P402Widget(NodeProfile402 *profile = nullptr, QWidget *parent = nullptr);

    Node *node() const;

    QString title() const;

public slots:
    void setProfile(NodeProfile402 *profile);
    void gotoStateOEClicked();

protected slots:
    void setStartLogger(bool start);
    void setLogTimer(int ms);

    void updateNodeStatus();
    void updateState();
    void setEvent(quint8 event);
    void updateMode(NodeProfile402::OperationMode mode);
    void updateModeComboBox();
    void setModeIndex(int id);

    void setCurrentWidget(NodeProfile402::OperationMode mode);

    void displayOption402();
    void haltClicked();

    void setCheckableStateMachine(int id);
    void stateMachineClicked(int id);

    void readAllObjects();

private:
    NodeProfile402 *_nodeProfile402;

    QMap<NodeProfile402::OperationMode, P402ModeWidget *> _modeWidgets;

    // Create widgets
    void createWidgets();
    QStackedWidget *_stackedWidget;

    QToolBar *_toolBar;
    QToolBar *createToolBarWidgets();
    QSpinBox *_logTimerSpinBox;
    QAction *_startStopAction;
    QAction *_option402Action;

    QGroupBox *createModeWidgets();
    QGroupBox *_modeGroupBox;
    QComboBox *_modeComboBox;
    QLabel *_modeLabel;

    QGroupBox *createStateMachineWidgets();
    QGroupBox *_stateMachineGroupBox;
    QButtonGroup *_stateMachineButtonGroup;

    QGroupBox *createControlWordWidgets();
    QGroupBox *_controlWordGroupBox;
    QPushButton *_haltPushButton;
    IndexLabel *_controlWordLabel;

    QGroupBox *createStatusWordWidgets();
    QGroupBox *_statusWordGroupBox;
    IndexLabel *_statusWordLabel;
    QLabel *_statusWordStateLabel;
    QLabel *_informationLabel;
    QLabel *_warningLabel;

    QList<QAction *> _modeActions;
};

#endif  // P402WIDGET_H
