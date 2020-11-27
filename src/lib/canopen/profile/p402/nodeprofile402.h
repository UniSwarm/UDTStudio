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

#include "../nodeprofile.h"
#include "node.h"

class NodeObjectId;
class NodeProfile402Vl;
class NodeProfile402Ip;
class NodeProfile402Tq;

class CANOPEN_EXPORT NodeProfile402 : public NodeProfile
{
    Q_OBJECT
public:
    NodeProfile402(Node *node, uint8_t axis);

    enum Mode
    {
        MS = -1,
        NoMode = 0,  //
        PP = 1,      // Profile position mode
        VL = 2,      // Velocity mode
        PV = 3,      // Profile velocity mode
        TQ = 4,      // Torque profile mode
        HM = 6,      // Homing mode
        IP = 7,      // Interpolated position mode
        CSP = 8,     // Cyclic sync position mode
        CSV = 9,     // Cyclic sync velocity mode
        CST = 10,    // Cyclic sync torque mode
        CSTCA = 11,  // Cyclic sync torque mode with commutation angle
        Reserved = 12
    };

    Mode actualMode();
    bool setMode(Mode mode);
    QString modeStr(NodeProfile402::Mode mode);
    bool isModeSupported(Mode mode);
    QList<Mode> modesSupported();

    enum State402
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

    State402 currentState() const;
    void goToState(const State402 state);
    QString stateStr(State402 state) const;

    bool toggleHalt();

    void setTarget(qint32 setTarget);

    enum Event402 : quint8
    {
        None = 0x00,
        InternalLimitActive = 0x01,
        Warning = 0x02,
        FollowingError = 0x04,
        VoltageEnabled = 0x8,
        Remote = 0x10,
        TargetReached = 0x20
    };

    QString event402Str(quint8 Event402);

    void setEnableRamp(bool ok);
    bool isEnableRamp(void);

    void setUnlockRamp(bool ok);
    bool isUnlockRamp(void);

    void setReferenceRamp(bool ok);
    bool isReferenceRamp(void);

signals:
    void modeChanged(uint8_t axis, Mode modeNew);
    void stateChanged();
    void isHalted(bool state);
    void eventHappened(quint8 Event402);

    void enableRampEvent(bool ok);
    void referenceRampEvent(bool ok);
    void unlockRampEvent(bool ok);

    void supportedDriveModesUdpdated();

private:
    uint8_t _axis;

    enum State
    {
        NONE,
        STATE_CHANGE,
        MODE_CHANGE,
    };
    State _state;

    NodeObjectId _modesOfOperationObjectId;
    NodeObjectId _modesOfOperationDisplayObjectId;
    NodeObjectId _supportedDriveModesObjectId;
    NodeObjectId _controlWordObjectId;
    NodeObjectId _statusWordObjectId;

    Mode _currentMode;
    Mode _requestedChangeMode;
    QList<Mode> _supportedModes;

    quint16 _cmdControlWord;
    State402 _stateMachineCurrent;
    State402 _requestedStateMachine;

    uint8_t _eventStatusWord;

    NodeProfile402Ip *_p402Ip;
    NodeProfile402Tq *_p402Tq;
    NodeProfile402Vl *_p402Vl;

    QTimer _modeTimer;

    void statusNodeChanged(Node::Status status);

    void enableRamp(quint16 cmdControlWord);
    void manageState(const State402 state);

    void manageEventStatusWord(quint16 statusWord);
    void manageStateStatusWord(quint16 statusWord);
    void manageSupportedDriveModes(quint32 supportedDriveModes);

private slots:
    void readModeOfOperationDisplay(void);

public:
    bool status() const override;
    quint16 profileNumber() const override;
    QString profileNumberStr() const override;
    virtual void reset() override;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // NODEPROFILE402_H
