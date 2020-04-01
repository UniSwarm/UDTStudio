#ifndef ODDB_H
#define ODDB_H

#include "od_global.h"
#include <QString>
#include <QMap>

class OD_EXPORT OdDb
{
  public:
    OdDb(QString directory = ".");

    void setDirectory(QString directory);
    QString fileEds(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber);
    void refreshFile();

  private:
    quint32 _deviceType;
    quint32 _vendorID;
    quint32 _productCode;
    quint32 _revisionNumber;

    QMap<QByteArray, QString> _mapFile;
    QString _directory;
    QString _fileName;
    QString _path;
    QList<QString> _directoryList;

    void searchEds();

};

#endif // ODDB_H
