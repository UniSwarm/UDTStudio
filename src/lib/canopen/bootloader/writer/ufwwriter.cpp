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

    _ufwByteArray.clear();
    _ufwModel = new UfwModel();

    _ufwModel->setDevice(qToLittleEndian(static_cast<uint16_t>(type.toUInt())));
    qToLittleEndian(_ufwModel->device(), buffer);
    _ufwByteArray.append(buffer, sizeof(_ufwModel->device()));

    _ufwModel->setCountSegment(qToLittleEndian(static_cast<uint8_t>(segment.size())));
    qToLittleEndian(_ufwModel->countSegment(), buffer);
    _ufwByteArray.append(buffer, sizeof(_ufwModel->countSegment()));

    _ufwModel->setVendorId(0);
    qToLittleEndian(_ufwModel->vendorId(), buffer);
    _ufwByteArray.append(buffer, sizeof(_ufwModel->vendorId()));

    _ufwModel->setProductId(0);
    qToLittleEndian(_ufwModel->productId(), buffer);
    _ufwByteArray.append(buffer, sizeof(_ufwModel->productId()));

    _ufwModel->setRevision(0);
    qToLittleEndian(_ufwModel->revision(), buffer);
    _ufwByteArray.append(buffer, sizeof(_ufwModel->revision()));

    _ufwModel->setSerial(0);
    qToLittleEndian(_ufwModel->serial(), buffer);
    _ufwByteArray.append(buffer, sizeof(_ufwModel->serial()));

    bool ok;
    int i = 0;
    QList<UfwModel::Segment *> segmentList;
    for (i = 0; i < segment.size(); i++)
    {
        UfwModel::Segment *seg = new UfwModel::Segment();
        seg->memorySegmentStart = segment.at(i).split(":").at(0).toUInt(&ok, 16);
        seg->memorySegmentEnd = segment.at(i).split(":").at(1).toUInt(&ok, 16);
        segmentList.append(seg);
    }

    _ufwModel->setSegmentList(segmentList);

    for (i = 0; i < _ufwModel->countSegment(); i++)
    {
        uint32_t adrStart = _ufwModel->segmentList().at(i)->memorySegmentStart;
        uint32_t adrEnd = _ufwModel->segmentList().at(i)->memorySegmentEnd;

        qToLittleEndian(adrStart, buffer);
        _ufwByteArray.append(buffer, sizeof(adrStart));

        qToLittleEndian(adrEnd, buffer);
        _ufwByteArray.append(buffer, sizeof(adrEnd));
    }

    _ufwByteArray.append(hex);

    return 0;
}

const QByteArray &UfwWriter::binary() const
{
    return _ufwByteArray;
}

void UfwWriter::setBinary(const QByteArray &newBinary)
{
    _ufwByteArray = newBinary;
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
        _ufwByteArray.append(app.mid(adrStart, adrEnd - adrStart));
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
