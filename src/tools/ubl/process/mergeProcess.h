#ifndef MERGEPROCESS_H
#define MERGEPROCESS_H

#include <QString>
#include <QStringList>

class MergeProcess
{
public:
    MergeProcess(const QString fileA, QStringList segmentA, const QString fileB, QStringList segmentB);

    void setFileA(const QString file, QStringList segment);
    void setFileB(const QString file, QStringList segment);

    int merge();

    const QByteArray &prog() const;

private:
    QString _fileA;
    QStringList _segmentA;

    QString _fileB;
    QStringList _segmentB;

    QByteArray _prog;
};

#endif // MERGEPROCESS_H
