#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

class FileInfo
{
public:
    FileInfo();

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString fileVersion() const;
    void setFileVersion(const QString &fileVersion);

    QString fileRevision() const;
    void setFileRevision(const QString &fileRevision);

    QString edsVersion() const;
    void setEdsVersion(const QString &edsVersion);

    QString description() const;
    void setDescription(const QString &description);

    QString creationTime() const;
    void setCreationTime(const QString &creationTime);

    QString creationDate() const;
    void setCreationDate(const QString &creationDate);

    QString createdBy() const;
    void setCreatedBy(const QString &createdBy);

    QString modificationTime() const;
    void setModificationTime(const QString &modificationTime);

    QString modificationDate() const;
    void setModificationDate(const QString &modificationDate);

    QString modifiedBy() const;
    void setModifiedBy(const QString &modifiedBy);

private:
    QString _fileName;
    QString _fileVersion;
    QString _fileRevision;
    QString _edsVersion;
    QString _description;
    QString _creationTime;
    QString _creationDate;
    QString _createdBy;
    QString _modificationTime;
    QString _modificationDate;
    QString _modifiedBy;
};

#endif // FILEINFO_H
