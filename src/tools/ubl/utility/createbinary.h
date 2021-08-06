#ifndef CREATEBINARY_H
#define CREATEBINARY_H

#include <QByteArray>
#include <QStringList>

class CreateBinary
{
public:
    CreateBinary();

    int create(const QByteArray &hex, QString type, QStringList segment);

    const QByteArray &binary() const;

private:
    struct Head
    {
        uint32_t deviceModel;
        uint32_t memoryBlockStart1;
        uint32_t memoryBlockend1;
        uint32_t memoryBlockStart2;
        uint32_t memoryBlockend2;
    };
    Head _head;

    QByteArray _binary;

    int append(const QByteArray &app, QStringList addresses);
    int checkAddresses(QStringList addresses);
};

#endif // CREATEBINARY_H
