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
    P402Widget(Node *node = nullptr, uint8_t axis = 0, QWidget *parent = nullptr);
    ~P402Widget() override;

    Node *node() const;

    QString title() const;

public slots:
    void setNode(Node *value, uint8_t axis = 0);
    void gotoStateOEClicked();

protected slots:
    void toggleStartLogger(bool start);
    void setLogTimer(int ms);
    void statusNodeChanged();

private:
    Node *_node;
    uint8_t _axis;

    QMap<NodeProfile402::OperationMode, P402ModeWidget *> _modes;

    enum State
    {
        STATE_NotReadyToSwitchOn = 1,
        STATE_SwitchOnDisabled = 2,
        STATE_ReadyToSwitchOn = 3,
        STATE_SwitchedOn = 4,
        STATE_OperationEnabled = 5,
        STATE_QuickStopActive = 6,
        STATE_FaultReactionActive = 7,
        STATE_Fault = 8,
    };

    NodeObjectId _controlWordObjectId;
    NodeObjectId _statusWordObjectId;
    NodeProfile402 *_nodeProfile402;

    QStackedWidget *_stackedWidget;

    // Logger timer
    QSpinBox *_logTimerSpinBox;

    // Toolbar action
    QAction *_startStopAction;
    QAction *_option402Action;

    QGroupBox *_modeGroupBox;
    QComboBox *_modeComboBox;
    QList<NodeProfile402::OperationMode> _listModeComboBox;
    QLabel *_modeLabel;

    QGroupBox *_stateMachineGroupBox;
    QButtonGroup *_stateMachineGroup;

    QGroupBox *_controlWordGroupBox;
    QPushButton *_haltPushButton;
    IndexLabel *_controlWordLabel;

    QGroupBox *_statusWordGroupBox;
    IndexLabel *_statusWordLabel;
    QLabel *_statusWordStateLabel;
    QLabel *_informationLabel;
    QLabel *_warningLabel;

    void modeChanged(uint8_t axis, NodeProfile402::OperationMode mode);
    void stateChanged();
    void modeIndexChanged(int id);

    void readAllObject();

    void isHalted(bool state);
    void eventHappened(quint8 event);

    void updateModeComboBox();

    void displayOption402();
    void stateMachineClicked(int id);
    void haltClicked();

    void setCheckableStateMachine(int id);

    // Create widgets
    void createWidgets();
    QToolBar *toolBarWidgets();
    QGroupBox *modeWidgets();
    QGroupBox *stateMachineWidgets();
    QGroupBox *controlWordWidgets();
    QGroupBox *statusWordWidgets();
};

#endif  // P402Widget_H
