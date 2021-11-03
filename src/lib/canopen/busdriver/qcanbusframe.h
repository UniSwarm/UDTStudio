/**
 ** File get from Qt5.15.1 from compatibility issues
 **/

#ifndef QCANBUSFRAME_H
#define QCANBUSFRAME_H

#include "canopen_global.h"

#include <QtCore/qmetatype.h>

class QDataStream;

class CANOPEN_EXPORT QCanBusFrame
{
public:
    class TimeStamp
    {
    public:
        Q_DECL_CONSTEXPR TimeStamp(qint64 s = 0, qint64 usec = 0) Q_DECL_NOTHROW : secs(s), usecs(usec)
        {
        }

        Q_DECL_CONSTEXPR static TimeStamp fromMicroSeconds(qint64 usec) Q_DECL_NOTHROW
        {
            return TimeStamp(usec / 1000000, usec % 1000000);
        }

        Q_DECL_CONSTEXPR qint64 seconds() const Q_DECL_NOTHROW
        {
            return secs;
        }

        Q_DECL_CONSTEXPR qint64 microSeconds() const Q_DECL_NOTHROW
        {
            return usecs;
        }

    private:
        qint64 secs;
        qint64 usecs;
    };

    enum FrameType
    {
        UnknownFrame = 0x0,
        DataFrame = 0x1,
        ErrorFrame = 0x2,
        RemoteRequestFrame = 0x3,
        InvalidFrame = 0x4
    };

    explicit QCanBusFrame(FrameType type = DataFrame);

    enum FrameError
    {
        NoError = 0,
        TransmissionTimeoutError = (1 << 0),
        LostArbitrationError = (1 << 1),
        ControllerError = (1 << 2),
        ProtocolViolationError = (1 << 3),
        TransceiverError = (1 << 4),
        MissingAcknowledgmentError = (1 << 5),
        BusOffError = (1 << 6),
        BusError = (1 << 7),
        ControllerRestartError = (1 << 8),
        UnknownError = (1 << 9),
        AnyError = 0x1FFFFFFFU
        // only 29 bits usable
    };
    Q_DECLARE_FLAGS(FrameErrors, FrameError)
    Q_FLAGS(FrameErrors)

    explicit QCanBusFrame(quint32 identifier, const QByteArray &data);

    bool isValid() const;

    FrameType frameType() const;
    void setFrameType(FrameType newFormat);

    bool hasExtendedFrameFormat() const;
    void setExtendedFrameFormat(bool isExtended);

    quint32 frameId() const;
    void setFrameId(quint32 newFrameId);

    QByteArray payload() const;
    void setPayload(const QByteArray &data);

    TimeStamp timeStamp() const;
    void setTimeStamp(TimeStamp ts);

    FrameErrors error() const;
    void setError(FrameErrors e);

    QString toString() const;

    bool hasFlexibleDataRateFormat() const;
    void setFlexibleDataRateFormat(bool isFlexibleData);

    bool hasBitrateSwitch() const;
    void setBitrateSwitch(bool bitrateSwitch);

    bool hasErrorStateIndicator() const;
    void setErrorStateIndicator(bool errorStateIndicator);

    bool hasLocalEcho() const;
    void setLocalEcho(bool localEcho);

private:
    quint32 _canId : 29;  // acts as container for error codes too
    quint8 _format : 3;   // max of 8 frame types

    quint8 _isExtendedFrame : 1;
    quint8 _isValidFrameId : 1;
    quint8 _isFlexibleDataRate : 1;

    quint8 _isBitrateSwitch : 1;
    quint8 _isErrorStateIndicator : 1;
    quint8 _isLocalEcho : 1;
    quint16 _reserved0 : 10;

    QByteArray _load;
    TimeStamp _stamp;
};

#endif  // QCANBUSFRAME_H
