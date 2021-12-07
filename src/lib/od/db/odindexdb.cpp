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

#include "odindexdb.h"

bool ODIndexDb::isQ1516(quint16 index, quint8 subIndex, quint16 profileNumber)
{
    if (index < 0x2000 || index >= 0x6000)
    {
        return false;
    }
    if (profileNumber == 402)
    {
        if (index == 0x2A00)
        {
            if (subIndex == 2 || subIndex == 3)
            {
                return true;
            }
        }
        if (index == 0x2A01)
        {
            if (subIndex >= 2 && subIndex <= 5)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4006)
        {
            if (subIndex == 3 || subIndex == 4 || subIndex == 5)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4007)
        {
            if (subIndex == 6 || subIndex == 8)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4020 || (index & (quint16)0xF1FF) == 0x4040 || (index & (quint16)0xF1FF) == 0x4060)
        {
            if (subIndex >= 1 && subIndex <= 4)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4021 || (index & (quint16)0xF1FF) == 0x4041 || (index & (quint16)0xF1FF) == 0x4061)
        {
            if (subIndex >= 1 && subIndex <= 6)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4022 || (index & (quint16)0xF1FF) == 0x4042 || (index & (quint16)0xF1FF) == 0x4062)
        {
            if (subIndex == 1 || subIndex == 3)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4024 || (index & (quint16)0xF1FF) == 0x4044 || (index & (quint16)0xF1FF) == 0x4064)
        {
            if (subIndex >= 2 && subIndex <= 5)
            {
                return true;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4025 || (index & (quint16)0xF1FF) == 0x4045 || (index & (quint16)0xF1FF) == 0x4065)
        {
            if (subIndex >= 1 && subIndex <= 7)
            {
                return true;
            }
        }
    }
    return false;
}

double ODIndexDb::scale(quint16 index, quint8 subIndex, quint16 profileNumber)
{
    if (index < 0x2000 || index >= 0x6000)
    {
        return 1.0;
    }

    if (index == 0x2000) // board voltages
    {
        return 1/100.0;
    }
    if (index == 0x2020) // cpu temps
    {
        return 1/10.0;
    }

    if (profileNumber == 402)
    {
        if (index == 0x2801) // bridge temps
        {
            return 1/10.0;
        }
        if (index == 0x2802 || index == 0x2803) // bridge currents
        {
            return 1/100.0;
        }
        if (index == 0x2805) // bridge bemf voltages
        {
            return 1/10.0;
        }
        if ((index & (quint16)0xF1FF) == 0x4006) // Motor status
        {
            if (subIndex == 2) // motor current
            {
                return 1/100.0;
            }
        }
    }
    return 1.0;
}
