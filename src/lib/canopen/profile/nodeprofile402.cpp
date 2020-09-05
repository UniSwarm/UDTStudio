#include "nodeprofile402.h"
#include "node.h"
#include "nodeodsubscriber.h"

NodeProfile402::NodeProfile402(Node *node)
    : NodeProfile(node)
{
    _modesOfOperationObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6060, 0);
    _modesOfOperationDisplayObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6061, 0);
    _supportedDriveModesObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6502, 0);

    registerObjId({_modesOfOperationObjectId});
    registerObjId({_modesOfOperationDisplayObjectId});
    setNodeInterrest(node);

    _node->readObject(_supportedDriveModesObjectId.index, _supportedDriveModesObjectId.subIndex);

    connect(&_changeModeTimer, &QTimer::timeout, this, &NodeProfile402::checkModeOperationDisplay);
}

NodeProfile402::Mode NodeProfile402::actualMode()
{
    if ((_currentMode == NoMode) || (_currentMode == PP) || (_currentMode == VL) || (_currentMode == PV) || (_currentMode == TQ) || (_currentMode == HM) || (_currentMode == IP) ||
        (_currentMode == CSP) || (_currentMode == CSV) || (_currentMode == CST) || (_currentMode == CSTCA))
    {
        return static_cast<Mode>(_currentMode);
    }
    else if (_currentMode < 0)
    {
        return Mode::MS;
    }
    else
    {
        return Mode::Reserved;
    }
}

QString NodeProfile402::actualModeStr()
{
    switch (_currentMode)
    {
    case Mode::NoMode:
        return tr("No mode");
    case Mode::PP:
        return tr("Profile position");
    case Mode::VL:
        return tr("Velocity");
    case Mode::PV:
        return tr("Profile velocity");
    case Mode::TQ:
        return tr("Torque profile");
    case Mode::HM:
        return tr("Homing");
    case Mode::IP:
        return tr("Interpolated position");
    case Mode::CSP:
        return tr("Cyclic sync position");
    case Mode::CSV:
        return tr("Cyclic sync velocity");
    case Mode::CST:
        return tr("Cyclic sync torque");
    case Mode::CSTCA:
        return tr("Cyclic sync torque mode with commutation angle");
    default:
        if (_currentMode < 0)
        {
            return tr("Manufacturer-specific");
        }
        else
        {
            return tr("Reserved");
        }
    }
}

bool NodeProfile402::setMode(Mode mode)
{
    if ((mode == NoMode) || (mode == PP) || (mode == VL) || (mode == PV) || (mode == TQ) || (mode == HM) || (mode == IP) || (mode == CSP) || (mode == CSV) || (mode == CST) || (mode == CSTCA))
    {
        _node->writeObject(_modesOfOperationObjectId.index, _modesOfOperationObjectId.subIndex, QVariant(mode));
        _state = STATE_CHANGE_MODE;
        _currentError = NO_ERROR;
        _changeModeTimer.start(TIMER_FOR_CHANGE_MODE);
        return true;
    }
    else
    {
        _currentError = WRONG_MODE;
        return false;
    }
}

void NodeProfile402::checkModeOperationDisplay()
{
    _node->readObject(_modesOfOperationDisplayObjectId.index, _modesOfOperationDisplayObjectId.subIndex);
}

bool NodeProfile402::status() const
{
    return true;
}

void NodeProfile402::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index == _modesOfOperationObjectId.index)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            _currentError = MODE_NOT_APPLY;
        }
        else
        {
            _node->readObject(_modesOfOperationDisplayObjectId.index, _modesOfOperationDisplayObjectId.subIndex);
        }
    }

    if (objId.index == _modesOfOperationDisplayObjectId.index)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            //_currentError = MODE_NOT_APPLY;
        }
        else
        {
            qint8 mode = static_cast<qint8>(_node->nodeOd()->value(_modesOfOperationDisplayObjectId).toInt());
            if (_currentMode != mode)
            {
                _currentMode = mode;
                _changeModeTimer.stop();
                emit modeChanged();
            }
        }
    }

    if (objId.index == _supportedDriveModesObjectId.index)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            //_currentError = MODE_NOT_APPLY;
        }
        else
        {
            quint32 modes = static_cast<quint32>(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());
            quint32 i;
            quint32 mask = 1;
            for (i = 0; i <= 10; i++)
            {
                if ((modes & (mask < i)) == (mask < i))
                {
                    if (i == 0)
                    {
                        _supportedModes.append(Mode::PP);
                    }
                    if (i == 1)
                    {
                        _supportedModes.append(Mode::VL);
                    }
                    if (i == 2)
                    {
                        _supportedModes.append(Mode::PV);
                    }
                    if (i == 3)
                    {
                        _supportedModes.append(Mode::TQ);
                    }
                    if (i == 4)
                    {
                        _supportedModes.append(Mode::HM);
                    }
                    if (i == 6)
                    {
                        _supportedModes.append(Mode::IP);
                    }
                    if (i == 7)
                    {
                        _supportedModes.append(Mode::CSP);
                    }
                    if (i == 8)
                    {
                        _supportedModes.append(Mode::CSV);
                    }
                    if (i == 9)
                    {
                        _supportedModes.append(Mode::CST);
                    }
                    if (i == 10)
                    {
                        _supportedModes.append(Mode::CSTCA);
                    }
                }
            }
        }
    }
}
