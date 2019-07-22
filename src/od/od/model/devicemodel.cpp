#include "devicemodel.h"

DeviceModel::DeviceModel()
{

}

DeviceModel::~DeviceModel()
{
    qDeleteAll(_fileInfos);
    _fileInfos.clear();

    qDeleteAll(_dummyUsages);
    _dummyUsages.clear();

    qDeleteAll(_indexes);
    _indexes.clear();
}

QMap<QString, QString *> DeviceModel::fileInfos() const
{
    return _fileInfos;
}

void DeviceModel::setFileInfos(const QMap<QString, QString *> &fileInfos)
{
    _fileInfos = fileInfos;
}

QMap<QString, QString *> DeviceModel::dummyUsages() const
{
    return _dummyUsages;
}

void DeviceModel::setDummyUsages(const QMap<QString, QString *> &dummyUsages)
{
    _dummyUsages = dummyUsages;
}

QMap<uint16_t, Index *> DeviceModel::indexes() const
{
    return _indexes;
}

void DeviceModel::setIndexes(const QMap<uint16_t, Index *> &indexes)
{
    _indexes = indexes;
}
