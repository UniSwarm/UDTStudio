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

#ifndef NODEPROFILE402_H
#define NODEPROFILE402_H

#include "canopen_global.h"

#include "nodeprofile.h"

class NodeObjectId;
class NodeProfile402Vl;

#define TIMER_FOR_CHANGE_MODE 10 // in ms

class CANOPEN_EXPORT NodeProfile402 : public NodeProfile
{
    Q_OBJECT
public:
    NodeProfile402(Node *node);

    enum Mode
    {
        MS = -1,
        NoMode = 0, //
        PP = 1, // Profile position mode
        VL = 2, // Velocity mode
        PV = 3, // Profile velocity mode
        TQ = 4, // Torque profile mode
        HM = 6, // Homing mode
        IP = 7, // Interpolated position mode
        CSP = 8, // Cyclic sync position mode
        CSV = 9, // Cyclic sync velocity mode
        CST = 10, // Cyclic sync torque mode
        CSTCA = 11, // Cyclic sync torque mode with commutation angle
        Reserved = 12
    };

    Mode actualMode();
    QString actualModeStr();
    QString modeStr(NodeProfile402::Mode mode);
    bool setMode(Mode mode);
    bool isModeSupported(Mode mode);
    QList<Mode> modesSupported();

    enum StateOf402
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

    StateOf402 currentStateOf402() const;
    void changeStateOf402(const StateOf402 stateOf402);
    QString currentStateOf402Str() const;
    QString stateOf402Str(StateOf402 stateOf402) const;
    quint16 rawStatusWord();
    quint16 rawControlWord();

    bool haltToggle();

    enum WarningStatusWord : quint8
    {
        WarningNone = 0x00,
        InternalLimitActive = 0x01,
        Warning = 0x02,
        FollowingError = 0x08
    };

    enum InformationStatusWord : quint8
    {
        InformationNone = 0x0,
        VoltageEnabled = 0x1,
        Remote = 0x2,
        TargetReached = 0x4
    };

    uint8_t msFieldStatusWord();
    uint8_t omsFieldStatusWord();
    uint8_t warningStatusWord();
    uint8_t informationStatusWord();

    enum Error
    {
        NO_ERROR,
        WRONG_MODE,
        MODE_NOT_APPLY
    };

    Error errorOccurred();

signals:
    void modeChanged();
    void stateChanged();
    void isHalted(bool state);

private:
    enum State
    {
        STATE_NONE,
        STATE_CHANGE_MODE
    };
    State _state;
    Error _currentError;

    // Mode
    NodeObjectId _modesOfOperationObjectId;
    NodeObjectId _modesOfOperationDisplayObjectId;
    NodeObjectId _supportedDriveModesObjectId;
    Mode _currentMode;
    QList<Mode> _supportedModes;

//    ControlWord/StatusWord
    NodeObjectId _controlWordObjectId;
    NodeObjectId _statusWordObjectId;
    quint16 _cmdControlWord;
    StateOf402 _stateMachineCurrent;
    quint8 _msFieldStatusWord;
    quint8 _omsFieldStatusWord;


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
        SW_FollowingError = 0x2000,
        SW_InternalLimitActive = 0x800,
        SW_OperationModeSpecific = 0x3000,
        SW_ManufacturerSpecific = 0xC000
    };

    enum StatusWordStateMask
    {
        Mask1 = 0x4F,
        Mask2 = 0x6f
    };

    uint8_t _warningFieldStatusWord;
    uint8_t _informationFieldStatusWord;

public:
    bool status() const override;
    quint16 profileNumber() const override;
    QString profileNumberStr() const override;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // NODEPROFILE402_H
