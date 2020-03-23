#include "hexfile.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

HexFile::HexFile(const QString &fileName)
{
    _fileName = fileName;
}

bool HexFile::read()
{
    int lineCount=1;
    int offsetAddr=0;
    int dataCount, addr, type, checkSum;
    bool ok;

    QFile file(_fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"cannot open";
        return false;
    }

    QTextStream stream(&file);
    _prog.clear();
    //_prog.fill(0xFF, 0x10000);
    _prog.resize(file.size());
    //_prog.fill(static_cast<char>(0xFF), file.size());
    while(!stream.atEnd())
    {
        unsigned index = 0;
        QString line=stream.readLine();

        if(line.size()<11) return false;
        if(line.at(index)!=':') return false;
        index++;

        dataCount = line.mid(index,2).toInt(&ok, 16);
        if(!ok) return false;
        index+=2;

        addr = line.mid(index,4).toInt(&ok, 16);
        if(!ok) return false;
        index+=4;

        type = line.mid(index,2).toInt(&ok, 16);
        if(!ok) return false;
        index+=2;

        if(type==0)
        {
            if(offsetAddr+addr>_prog.size())
            {
                //qDebug()<<"outData"<<QString::number(offsetAddr+addr,16)<<line.mid(index,2*dataCount);
            }
            else
            {
                for(int i=0; i<dataCount; i++)
                {
                    int data = line.mid(index,2).toInt(&ok, 16);
                    if(!ok) return false;
                    index+=2;
                    _prog[addr+i+offsetAddr] = (char)data;
                }
            }
        }
        else if(type==4)
        {
            offsetAddr = line.mid(index,4).toInt(&ok, 16)*0x10000;
            if(!ok) return false;
            index+=4;
            //qDebug()<<"offset"<<QString::number(offsetAddr,16);
        }
        else if(type==1)
        {
            break;
        }
        else
        {
            //qDebug()<<"unknow type"<<type<<"at line"<<lineCount;
        }

        checkSum = line.mid(index,2).toInt(&ok, 16);
        if(!ok) return false;
        index+=2;

        lineCount++;
    }

    _checksum = 0;
    for(int i = 0; i < _prog.size(); i++)
    {
        _checksum += (unsigned char)_prog[i];
    }

    return true;
}

const QByteArray &HexFile::prog() const
{
    return _prog;
}

const unsigned short &HexFile::checksum() const
{
    return _checksum;
}
