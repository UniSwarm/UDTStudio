#include "oddb.h"

#include <QDirIterator>
#include <QDebug>
#include "parser/edsparser.h"
#include <QCryptographicHash>

OdDb::OdDb(QString directory)
    : _directory(directory)
{
    _directoryList.append(_directory);

    _fileName = nullptr;
    _path = nullptr;

    searchFile();
}

void OdDb::setDirectory(QString directory)
{
    _directory = directory;
    searchFile();
}

void OdDb::searchFile()
{
    QDir dir(_directory);
    QStringList list = dir.entryList(QStringList() << "*.eds", QDir::Files | QDir::NoSymLinks);

    for (const QString &file : list)
    {
        EdsParser parser;
        DeviceDescription *deviceDescription = parser.parse(_directory + "/" + file);
        QByteArray byte;
        byte.append(deviceDescription->index(0x1000)->subIndex(0)->value().toByteArray());
        byte.append(deviceDescription->index(0x1018)->subIndex(1)->value().toByteArray());
        byte.append(deviceDescription->index(0x1018)->subIndex(2)->value().toByteArray());
        byte.append(deviceDescription->index(0x1018)->subIndex(3)->value().toByteArray());
        QByteArray hash = QCryptographicHash::hash(byte, QCryptographicHash::Md4);
        _mapFile.insert(hash, _directory + "/" + file);
        byte.clear();
    }
}

QString OdDb::file(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber)
{
    QByteArray byte;
    byte.append(QVariant(deviceType).toByteArray());
    byte.append(QVariant(vendorID).toByteArray());
    byte.append(QVariant(productCode).toByteArray());
    byte.append(QVariant(revisionNumber).toByteArray());
    QByteArray hash = QCryptographicHash::hash(byte, QCryptographicHash::Md4);
    _mapFile.contains(hash);

    if (_mapFile.contains(hash))
    {
        return _mapFile.value(hash);
    }
    return QString();
}

void OdDb::refreshFile()
{
    _mapFile.clear();
    searchFile();
}
