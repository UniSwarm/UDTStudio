#include "fileinfo.h"

FileInfo::FileInfo()
{

}

QString FileInfo::fileName() const
{
    return _fileName;
}

void FileInfo::setFileName(const QString &fileName)
{
    _fileName = fileName;
}

QString FileInfo::fileVersion() const
{
    return _fileVersion;
}

void FileInfo::setFileVersion(const QString &fileVersion)
{
    _fileVersion = fileVersion;
}

QString FileInfo::fileRevision() const
{
    return _fileRevision;
}

void FileInfo::setFileRevision(const QString &fileRevision)
{
    _fileRevision = fileRevision;
}

QString FileInfo::edsVersion() const
{
    return _edsVersion;
}

void FileInfo::setEdsVersion(const QString &edsVersion)
{
    _edsVersion = edsVersion;
}

QString FileInfo::description() const
{
    return _description;
}

void FileInfo::setDescription(const QString &description)
{
    _description = description;
}

QString FileInfo::creationTime() const
{
    return _creationTime;
}

void FileInfo::setCreationTime(const QString &creationTime)
{
    _creationTime = creationTime;
}

QString FileInfo::creationDate() const
{
    return _creationDate;
}

void FileInfo::setCreationDate(const QString &creationDate)
{
    _creationDate = creationDate;
}

QString FileInfo::createdBy() const
{
    return _createdBy;
}

void FileInfo::setCreatedBy(const QString &createdBy)
{
    _createdBy = createdBy;
}

QString FileInfo::modificationTime() const
{
    return _modificationTime;
}

void FileInfo::setModificationTime(const QString &modificationTime)
{
    _modificationTime = modificationTime;
}

QString FileInfo::modificationDate() const
{
    return _modificationDate;
}

void FileInfo::setModificationDate(const QString &modificationDate)
{
    _modificationDate = modificationDate;
}

QString FileInfo::modifiedBy() const
{
    return _modifiedBy;
}

void FileInfo::setModifiedBy(const QString &modifiedBy)
{
    _modifiedBy = modifiedBy;
}
