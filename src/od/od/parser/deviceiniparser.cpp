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
DataStorage DeviceIniParser::readData(bool *nodeId) const
{
    uint8_t base = 10;
    QString value = _file->value("DataType").toString();
    if ( value.startsWith("0x"))
        base = 16;

    bool ok;
    uint8_t dataType = (uint8_t)value.toUInt(&ok, base);

    DataStorage data;
    data.setDataType(dataType);

    if (_file->value("DefaultValue").isNull())
        data.setValue(0);

    else if (_file->value("DefaultValue").toString().startsWith("$NODEID+"))
    {
        data.setValue(_file->value("DefaultValue").toString().mid(8));
        *nodeId = true;
    }

    else
    {
        data.setValue(_file->value("DefaultValue"));
    }

    return data;
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
