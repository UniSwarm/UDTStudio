#include "ufwmodel.h"

UfwModel::UfwModel()
{
    _head = new Head();
}

UfwModel::Head *UfwModel::head() const
{
    return _head;
}

const QByteArray &UfwModel::prog() const
{
    return _prog;
}

void UfwModel::setProg(const QByteArray &prog)
{
    _prog = prog;
}
