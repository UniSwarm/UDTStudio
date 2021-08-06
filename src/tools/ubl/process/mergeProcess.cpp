/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "mergeProcess.h"

#include "parser/hexparser.h"
#include "utility/hexmerger.h"

MergeProcess::MergeProcess(const QString fileA, QStringList segmentA, const QString fileB, QStringList segmentB)
    : _fileA(fileA)
    , _segmentA(segmentA)
    , _fileB(fileB)
    , _segmentB(segmentB)

{
}

void MergeProcess::setFileA(const QString file, QStringList segment)
{
    _fileA = file;
    _segmentA = segment;
}

void MergeProcess::setFileB(const QString file, QStringList segment)
{
    _fileB = file;
    _segmentB = segment;
}

int MergeProcess::merge()
{
    HexParser *hexAFile = new HexParser(_fileA);
    hexAFile->read();
    HexParser *hexBFile = new HexParser(_fileB);
    hexBFile->read();

    HexMerger *merger = new HexMerger();

    int ret = merger->merge(hexAFile->prog(), hexBFile->prog(), _segmentA, _segmentB);
    if (ret < 0)
    {
        return -1;
    }

    _prog = merger->prog();

    return 0;
}

const QByteArray &MergeProcess::prog() const
{
    return _prog;
}
