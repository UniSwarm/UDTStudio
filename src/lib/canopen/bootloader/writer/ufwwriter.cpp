#include "ufwwriter.h"

#include <QDataStream>
#include <QIODevice>
#include <QList>
#include <QtEndian>

UfwWriter::UfwWriter()
{
}

int UfwWriter::create(const QByteArray &hex, QString type, QStringList segment)
{
    char buffer[4];

    _ufw.clear();
    _head = new UfwParser::Head();

    _head->device = static_cast<uint16_t>(type.toUInt());
    qToLittleEndian(_head->device, buffer);
    _ufw.append(buffer, sizeof(_head->device));

    _head->countSegment = static_cast<uint8_t>(segment.size());
    qToLittleEndian(_head->countSegment, buffer);
    _ufw.append(buffer, sizeof(_head->countSegment));

    bool ok;
    int i = 0;
    for (i = 0; i < segment.size(); i++)
    {
        UfwParser::Segment *seg = new UfwParser::Segment();
        seg->memorySegmentStart = segment.at(i).split(":").at(0).toUInt(&ok, 16);
        seg->memorySegmentEnd = segment.at(i).split(":").at(1).toUInt(&ok, 16);
        _head->segmentList.append(seg);
    }

    for (i = 0; i < _head->countSegment; i++)
    {
        uint32_t adrStart = _head->segmentList.at(i)->memorySegmentStart;
        uint32_t adrEnd = _head->segmentList.at(i)->memorySegmentEnd;

        qToLittleEndian(adrStart, buffer);
        _ufw.append(buffer, sizeof(adrStart));

        qToLittleEndian(adrEnd, buffer);
        _ufw.append(buffer, sizeof(adrEnd));
    }

    _ufw.append(hex);

    return 0;
}

const QByteArray &UfwWriter::binary() const
{
    return _ufw;
}

void UfwWriter::setBinary(const QByteArray &newBinary)
{
    _ufw = newBinary;
}

int UfwWriter::append(const QByteArray &app, QStringList addresses)
{
    bool ok;
    int i = 0;

    addresses.sort();
    int error = checkAddresses(addresses);
    if (error < 0)
    {
        // qDebug() << "HexMerger:append : Error addresses";
        return error;
    }

    for (i = 0; i < addresses.size(); i++)
    {
        int adrStart = addresses.at(i).split(QLatin1Char(':')).at(0).toInt(&ok, 16);
        int adrEnd = addresses.at(i).split(QLatin1Char(':')).at(1).toInt(&ok, 16);
        _ufw.append(app.mid(adrStart, adrEnd - adrStart));
    }
    return 0;
}

int UfwWriter::checkAddresses(QStringList addresses)
{
    int i = 0;
    for (i = 0; i < addresses.size(); i++)
    {
        if (!addresses.at(i).contains(QLatin1Char(':')))
        {
            return -1;
        }
    }
    return 0;
}
