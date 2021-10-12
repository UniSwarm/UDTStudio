#include "ufwmodel.h"

UfwModel::UfwModel()
{
    _device = 0;
    //        QString version;
    //        QString date;
    _vendorId = 0;
    _productId = 0;
    _revision = 0;
    _serial = 0;
    _countSegment = 0;
}

const QByteArray &UfwModel::prog() const
{
    return _prog;
}

void UfwModel::setProg(const QByteArray &prog)
{
    _prog = prog;
}

uint16_t UfwModel::device() const
{
    return _device;
}

void UfwModel::setDevice(uint16_t device)
{
    _device = device;
}

uint32_t UfwModel::vendorId() const
{
    return _vendorId;
}

void UfwModel::setVendorId(uint32_t vendorId)
{
    _vendorId = vendorId;
}

uint32_t UfwModel::productId() const
{
    return _productId;
}

void UfwModel::setProductId(uint32_t productId)
{
    _productId = productId;
}

uint32_t UfwModel::revision() const
{
    return _revision;
}

void UfwModel::setRevision(uint32_t revision)
{
    _revision = revision;
}

uint32_t UfwModel::serial() const
{
    return _serial;
}

void UfwModel::setSerial(uint32_t serial)
{
    _serial = serial;
}

uint8_t UfwModel::countSegment() const
{
    return _countSegment;
}

void UfwModel::setCountSegment(uint8_t countSegment)
{
    _countSegment = countSegment;
}

const QList<UfwModel::Segment *> &UfwModel::segmentList() const
{
    return _segmentList;
}

void UfwModel::setSegmentList(const QList<Segment *> &newSegmentList)
{
    _segmentList = newSegmentList;
}
