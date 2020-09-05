#ifndef NODEPROFILE402_H
#define NODEPROFILE402_H

#include "canopen_global.h"

#include "../services/service.h"
#include "nodeobjectid.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"
#include "nodeprofile.h"

#define TIMER_FOR_CHANGE_MODE 10 // in ms

class NodeProfile402 : public NodeProfile
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
    bool setMode(Mode mode);
    bool isModeSupported(Mode mode);
    QList<Mode> modesSupported();

    enum Error
    {
        NO_ERROR,
        WRONG_MODE,
        MODE_NOT_APPLY

    };

    Error errorOccurred();

signals:
    void modeChanged();

private:
    enum State
    {
        STATE_NONE,
        STATE_CHANGE_MODE
    };
    State _state;

    QTimer _changeModeTimer;

    NodeObjectId _modesOfOperationObjectId;
    NodeObjectId _modesOfOperationDisplayObjectId;
    NodeObjectId _supportedDriveModesObjectId;

    Error _currentError;

    qint8 _currentMode;
    QList<Mode> _supportedModes;

    void checkModeOperationDisplay();

public:
    bool status() const override;

    // NodeOdSubscriber interface
public:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // NODEPROFILE402_H
