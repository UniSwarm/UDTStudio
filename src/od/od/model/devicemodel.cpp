#include "devicemodel.h"

DeviceModel::DeviceModel()
{

}

DeviceModel::~DeviceModel()
{
    qDeleteAll(_indexes);
    _indexes.clear();
}

DeviceModel::Type DeviceModel::type()
{
    return Invalid;
}

QMap<QString, QString> DeviceModel::fileInfos() const
{
    return _fileInfos;
}

void DeviceModel::setFileInfos(const QMap<QString, QString> &fileInfos)
{
    _fileInfos = fileInfos;
}

void DeviceModel::setFileInfo(const QString &key, const QString &value)
{
    _fileInfos.insert(key, value);
}

QMap<QString, QString> DeviceModel::dummyUsages() const
{
    return _dummyUsages;
}

void DeviceModel::setDummyUsages(const QMap<QString, QString> &dummyUsages)
{
    _dummyUsages = dummyUsages;
}

QMap<uint16_t, Index *> DeviceModel::indexes() const
{
    return _indexes;
}

Index *DeviceModel::index(const uint16_t &index) const
{
    return _indexes.value(index);
}

void DeviceModel::addIndex(Index *index)
{
    _indexes.insert(index->index(), index);
}

int DeviceModel::indexCount() const
{
    return _indexes.count();
}

void DeviceModel::setIndexes(const QMap<uint16_t, Index *> &indexes)
{
    _indexes = indexes;
}
