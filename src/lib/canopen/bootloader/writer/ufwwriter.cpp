#include "ufwwriter.h"

#include <QDataStream>
#include <QIODevice>
#include <QList>
#include <QtEndian>

enum
{
    VERSION_CHAR_MAX = 6,
    DATE_CHAR_MAX = 30
};

UfwWriter::UfwWriter()
{
}

int UfwWriter::create(uint16_t type, const QString &version, const QString &date, const QStringList &segment, const QByteArray &hex)
{
    char buffer[DATE_CHAR_MAX];

    _ufwByteArray.clear();
    _ufwModel = new UfwModel();

    qToLittleEndian(type, buffer);
    _ufwByteArray.append(buffer, sizeof(uint16_t));

    uint8_t size = static_cast<uint8_t>(version.size());
    if (size > VERSION_CHAR_MAX)
    {
        size = VERSION_CHAR_MAX;
    }
    qToLittleEndian(size, buffer);
    _ufwByteArray.append(buffer, sizeof(uint8_t));
    _ufwByteArray.append(version.toUtf8(), size);

    size = static_cast<uint8_t>(date.size());
    if (size >= DATE_CHAR_MAX)
    {
        size = DATE_CHAR_MAX;
    }
    qToLittleEndian(size, buffer);
    _ufwByteArray.append(buffer, sizeof(uint8_t));
    _ufwByteArray.append(date.toUtf8(), size);

    qToLittleEndian(segment.size(), buffer);
    _ufwByteArray.append(buffer, sizeof(uint8_t));

    bool ok;
    for (int i = 0; i < segment.size(); i++)
    {
        uint32_t start = segment.at(i).split(":").at(0).toUInt(&ok, 16);
        uint32_t end = segment.at(i).split(":").at(1).toUInt(&ok, 16);

        qToLittleEndian(start, buffer);
        _ufwByteArray.append(buffer, sizeof(start));

        qToLittleEndian(end, buffer);
        _ufwByteArray.append(buffer, sizeof(end));
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

int UfwWriter::checkAddresses(const QStringList &addresses)
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
