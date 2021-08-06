#include "createbinary.h"

#include <QDataStream>
#include <QIODevice>

CreateBinary::CreateBinary()
{
}

int CreateBinary::create(const QByteArray &hex, QString type, QStringList segment)
{
    QByteArray head;

    _head.deviceModel = type.toUInt();

    bool ok;
    _head.memoryBlockStart1 = segment.at(0).split(":").at(0).toUInt(&ok, 16);
    _head.memoryBlockend1 = segment.at(0).split(":").at(1).toUInt(&ok, 16);
    _head.memoryBlockStart2 = segment.at(1).split(":").at(0).toUInt(&ok, 16);
    _head.memoryBlockend2 = segment.at(1).split(":").at(1).toUInt(&ok, 16);

    append(hex, segment);

    head.append(reinterpret_cast<char *>(&_head), sizeof(_head));
    _binary.prepend(head);

    return 0;
}

const QByteArray &CreateBinary::binary() const
{
    return _binary;
}

int CreateBinary::append(const QByteArray &app, QStringList addresses)
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
        _binary.append(app.mid(adrStart, adrEnd - adrStart));
    }
    return 0;
}

int CreateBinary::checkAddresses(QStringList addresses)
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
