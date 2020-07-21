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

#ifndef WIDGETDEBUG_H
#define WIDGETDEBUG_H

#include "../../udtgui_global.h"

#include "node.h"
#include "nodeodsubscriber.h"
#include "p402ipwidget.h"
#include "p402optionwidget.h"
#include "p402vlwidget.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QToolBar>
#include <QWidget>

class WidgetDebug : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    WidgetDebug(QWidget *parent = nullptr);
    WidgetDebug(Node *node, QWidget *parent = nullptr);
    ~WidgetDebug() override;

    Node *node() const;

public slots:
    void setNode(Node *value);
    void updateData();

    void preop();
    void start();
    void stop();
    void resetCom();
    void resetNode();
    void gotoStateOEClicked();

private:
    void createWidgets();
    void setTimer(int ms);
    void readData();

    QTimer _timer;
    QTimer _operationEnabledTimer;

    Node *_node;

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

    State stateMachineCurrent;

    quint16 cmdControlWord;
    quint16 _controlWordObjectId;
    quint16 _statusWordObjectId;
    quint16 _modesOfOperationObjectId;
    quint16 _modesOfOperationDisplayObjectId;

    QStackedWidget *_stackedWidget;
    P402OptionWidget *_p402Option;
    P402VlWidget *_p402vl;
    P402IpWidget *_p402ip;

    QToolBar *_nmtToolBar;
    QSpinBox *_logTimerSpinBox;

    QGroupBox *_modeGroupBox;
    QGroupBox *_stateMachineGroupBox;
    QGroupBox *_controlWordGroupBox;
    QGroupBox *_statusWordGroupBox;

    QComboBox *_modeComboBox;

    QLabel *_controlWordLabel;
    QPushButton *_haltPushButton;

    QLabel *_statusWordRawLabel;
    QLabel *_statusWordLabel;
    QLabel *_informationLabel;
    QLabel *_warningLabel;
    QLabel *_operationModeSpecificLabel;
    QLabel *_manufacturerSpecificLabel;
    QButtonGroup *_stateMachineGroup;

    void displayOption402();
    void modeIndexChanged(int id);
    void stateMachineClicked(int id);

    void manageNotificationControlWordObject(SDO::FlagsRequest flags);
    void manageNotificationStatusWordobject(SDO::FlagsRequest flags);
    void manageStatusWordInformation();
    void manageModeOfOperationObject(SDO::FlagsRequest flags);

    void setCheckableStateMachine(int id);
    void controlWordHaltClicked();

    enum ControlWord : quint16
    {
        CW_SwitchOn = 0x01,
        CW_EnableVoltage = 0x02,
        CW_QuickStop = 0x04,
        CW_EnableOperation = 0x08,
        CW_FaultReset = 0x80,
        CW_Halt = 0x100,
        CW_OperationModeSpecific = 0x70,
        CW_ManufacturerSpecific = 0xF800,

        CW_Mask = 0x8F
    };

    enum StatusWord : quint16
    {
        SW_StateNotReadyToSwitchOn = 0x00,
        SW_StateSwitchOnDisabled = 0x40,
        SW_StateReadyToSwitchOn = 0x21,
        SW_StateSwitchedOn = 0x23,
        SW_StateOperationEnabled = 0x27,
        SW_StateQuickStopActive = 0x07,
        SW_StateFaultReactionActive = 0x0F,
        SW_StateFault = 0x08,

        SW_VoltageEnabled = 0x10,
        SW_Warning = 0x80,
        SW_Remote = 0x200,
        SW_TargetReached = 0x400,
        SW_InternalLimitActive = 0x800,
        SW_OperationModeSpecific = 0x3000,
        SW_ManufacturerSpecific = 0xC000
    };

    enum StatusWordStateMask
    {
        Mask1 = 0x4F,
        Mask2 = 0x6f
    };

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
    // QObject interface
public:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
};

#endif // WIDGETDEBUG_H
