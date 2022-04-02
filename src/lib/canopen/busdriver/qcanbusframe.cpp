/**
 ** File get from Qt5.15.1 from compatibility issues
 **/

#include "busdriver/qcanbusframe.h"

#include <QString>

bool QCanBusFrame::isValid() const
{
    if (_format == InvalidFrame)
    {
        return false;
    }

    // long id used, but extended flag not set
    if (!_isExtendedFrame && ((_canId & 0x1FFFF800U) != 0U))
    {
        return false;
    }

    if (!_isValidFrameId)
    {
        return false;
    }

    // maximum permitted payload size in CAN or CAN FD
    const int length = _load.length();
    if (_isFlexibleDataRate)
    {
        if (_format == RemoteRequestFrame)
        {
            return false;
        }

        return length <= 8 || length == 12 || length == 16 || length == 20 || length == 24 || length == 32 || length == 48 || length == 64;
    }

    return length <= 8;
}

QCanBusFrame::FrameType QCanBusFrame::frameType() const
{
    switch (_format)
    {
        case 0x1:
            return DataFrame;
        case 0x2:
            return ErrorFrame;
        case 0x3:
            return RemoteRequestFrame;
        case 0x4:
            return InvalidFrame;
            // no default to trigger warning
    }

    return UnknownFrame;
}

void QCanBusFrame::setFrameType(FrameType newFormat)
{
    switch (newFormat)
    {
        case DataFrame:
            _format = 0x1;
            return;
        case ErrorFrame:
            _format = 0x2;
            return;
        case RemoteRequestFrame:
            _format = 0x3;
            return;
        case UnknownFrame:
            _format = 0x0;
            return;
        case InvalidFrame:
            _format = 0x4;
            return;
    }
}

bool QCanBusFrame::hasExtendedFrameFormat() const
{
    return (_isExtendedFrame & 0x1) != 0;
}

void QCanBusFrame::setExtendedFrameFormat(bool isExtended)
{
    _isExtendedFrame = (static_cast<int>(isExtended) & 0x1);
}

quint32 QCanBusFrame::frameId() const
{
    if (Q_UNLIKELY(_format == ErrorFrame))
    {
        return 0;
    }
    return (_canId & 0x1FFFFFFFU);
}

QCanBusFrame::QCanBusFrame(FrameType type)
    : _isExtendedFrame(0x0)
    , _isFlexibleDataRate(0x0)
    , _isBitrateSwitch(0x0)
    , _isErrorStateIndicator(0x0)
    , _isLocalEcho(0x0)
    , _reserved0(0x0)
{
    setFrameId(0x0);
    setFrameType(type);
}

QCanBusFrame::QCanBusFrame(quint32 identifier, const QByteArray &data)
    : _format(DataFrame)
    , _isExtendedFrame(0x0)
    , _isFlexibleDataRate(data.length() > 8 ? 0x1 : 0x0)
    , _isBitrateSwitch(0x0)
    , _isErrorStateIndicator(0x0)
    , _isLocalEcho(0x0)
    , _reserved0(0x0)
    , _load(data)
{
    setFrameId(identifier);
}

void QCanBusFrame::setFrameId(quint32 newFrameId)
{
    if (Q_LIKELY(newFrameId < 0x20000000U))
    {
        _isValidFrameId = true;
        _canId = newFrameId;
        setExtendedFrameFormat(_isExtendedFrame || ((newFrameId & 0x1FFFF800U) != 0U));
    }
    else
    {
        _isValidFrameId = false;
        _canId = 0;
    }
}

void QCanBusFrame::setPayload(const QByteArray &data)
{
    _load = data;
    if (data.length() > 8)
    {
        _isFlexibleDataRate = 0x1;
    }
}

void QCanBusFrame::setTimeStamp(TimeStamp ts)
{
    _stamp = ts;
}

QByteArray QCanBusFrame::payload() const
{
    return _load;
}

void QCanBusFrame::setError(FrameErrors e)
{
    if (_format != ErrorFrame)
    {
        return;
    }
    _canId = (e & AnyError);
}

QCanBusFrame::FrameErrors QCanBusFrame::error() const
{
    if (_format != ErrorFrame)
    {
        return NoError;
    }

    return FrameErrors(_canId & 0x1FFFFFFFU);
}

QCanBusFrame::TimeStamp QCanBusFrame::timeStamp() const
{
    return _stamp;
}

QString QCanBusFrame::toString() const
{
    const FrameType type = frameType();

    switch (type)
    {
        case InvalidFrame:
            return QStringLiteral("(Invalid)");
        case ErrorFrame:
            return QStringLiteral("(Error)");
        case UnknownFrame:
            return QStringLiteral("(Unknown)");
        default:
            break;
    }

    const char *const idFormat = hasExtendedFrameFormat() ? "%08X" : "     %03X";
    const char *const dlcFormat = hasFlexibleDataRateFormat() ? "  [%02d]" : "   [%d]";
    QString result;
    result.append(QString::asprintf(idFormat, static_cast<uint>(frameId())));
    result.append(QString::asprintf(dlcFormat, payload().size()));

    if (type == RemoteRequestFrame)
    {
        result.append(QLatin1String("  Remote Request"));
    }
    else if (!payload().isEmpty())
    {
        const QByteArray data = payload().toHex(' ').toUpper();
        result.append(QLatin1String("  "));
        result.append(QLatin1String(data));
    }

    return result;
}

void QCanBusFrame::setLocalEcho(bool localEcho)
{
    _isLocalEcho = (static_cast<int>(localEcho) & 0x1);
}

bool QCanBusFrame::hasLocalEcho() const
{
    return (_isLocalEcho & 0x1) != 0;
}

void QCanBusFrame::setErrorStateIndicator(bool errorStateIndicator)
{
    _isErrorStateIndicator = (static_cast<int>(errorStateIndicator) & 0x1);
    if (errorStateIndicator)
    {
        _isFlexibleDataRate = 0x1;
    }
}

bool QCanBusFrame::hasErrorStateIndicator() const
{
    return (_isErrorStateIndicator & 0x1) != 0;
}

void QCanBusFrame::setBitrateSwitch(bool bitrateSwitch)
{
    _isBitrateSwitch = (static_cast<int>(bitrateSwitch) & 0x1);
    if (bitrateSwitch)
    {
        _isFlexibleDataRate = 0x1;
    }
}

bool QCanBusFrame::hasBitrateSwitch() const
{
    return (_isBitrateSwitch & 0x1) != 0;
}

void QCanBusFrame::setFlexibleDataRateFormat(bool isFlexibleData)
{
    _isFlexibleDataRate = (static_cast<int>(isFlexibleData) & 0x1);
    if (!isFlexibleData)
    {
        _isBitrateSwitch = 0x0;
        _isErrorStateIndicator = 0x0;
    }
}

bool QCanBusFrame::hasFlexibleDataRateFormat() const
{
    return (_isFlexibleDataRate & 0x1) != 0;
}
