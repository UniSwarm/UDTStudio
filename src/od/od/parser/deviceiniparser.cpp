#include "deviceiniparser.h"

DeviceIniParser::DeviceIniParser(QSettings *file)
{
    _file = file;
}

/**
 * @brief read data to correct format from dcf or eds file
 * @param dcf or eds file
 * @return data
 */
QVariant DeviceIniParser::readData(bool *nodeId) const
{
    QVariant value;

    if (_file->value("DefaultValue").isNull())
        value = QVariant(0);

    else if (_file->value("DefaultValue").toString().startsWith("$NODEID+"))
    {
        value = QVariant(_file->value("DefaultValue").toString().mid(8));
        *nodeId = true;
    }

    else
    {
        value = QVariant(_file->value("DefaultValue"));
    }

    return value;
}

void DeviceIniParser::readFileInfo(DeviceModel *od) const
{
    foreach (const QString &key, _file->allKeys())
    {
       od->setFileInfo(key, _file->value(key).toString());
    }
}

void DeviceIniParser::readDummyUsage(DeviceModel *od) const
{
    foreach (const QString &key, _file->allKeys())
    {
       od->setDummyUsage(key, _file->value(key).toString());
    }
}

void DeviceIniParser::readDeviceInfo(DeviceDescription *od) const
{
    foreach (const QString &key, _file->allKeys())
    {
       od->setDeviceInfo(key, _file->value(key).toString());
    }
}

void DeviceIniParser::readDeviceComissioning(DeviceConfiguration *od) const
{
    foreach (const QString &key, _file->allKeys())
    {
       od->setDeviceComissioning(key, _file->value(key).toString());
    }
}

uint8_t DeviceIniParser::readPdoMapping() const
{
    if (_file->value("PDOMapping") == 0)
        return 0;

    QString accessString = _file->value("AccessType").toString();

    if (accessString == "rwr" || accessString == "ro" || accessString == "const")
        return SubIndex::Access::TPDO;

    if (accessString == "rww" || accessString == "wo")
        return SubIndex::Access::RPDO;

    return SubIndex::Access::TPDO + SubIndex::Access::RPDO;
}

QVariant DeviceIniParser::readLowLimit() const
{
    return QVariant(_file->value("LowLimit"));
}

QVariant DeviceIniParser::readHighLimit() const
{
    return QVariant(_file->value("HighLimit"));
}

uint16_t DeviceIniParser::readDataType() const
{
    QString dataType = _file->value("DataType").toString();

    int base = 10;
    if (dataType.startsWith("0x"))
        base = 16;

    bool ok;
    return static_cast<uint16_t>(dataType.toInt(&ok, base));
}
