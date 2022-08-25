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
        if ((index & (quint16)0xF1FF) == 0x4081)
        {
            if (subIndex == 2)
            {
                return true;
            }
        }
    }
    return false;
}

double ODIndexDb::scale(quint16 index, quint8 subIndex, quint16 profileNumber)
{
    if ((index >= 0x1400 && index <= 0x15FF) || (index >= 0x1800 && index <= 0x19FF))  // RPDO TPDO
    {
        if (subIndex == 3)  // Inhibit Time
        {
            return 0.1;
        }
    }

    if (index < 0x2000 || index >= 0x6000)
    {
        return 1.0;
    }

    if (index == 0x2000)  // board voltages
    {
        if (subIndex > 0)
        {
            return 1 / 100.0;
        }
    }
    if (index == 0x2020)  // cpu temperatures
    {
        if (subIndex > 0)
        {
            return 1 / 10.0;
        }
    }

    if (profileNumber == 402)
    {
        if (index == 0x2041)  // Board under/overvoltage
        {
            if (subIndex > 0)
            {
                return 1 / 100.0;
            }
        }
        if (index == 0x2801)  // bridge temperatures
        {
            if (subIndex > 0)
            {
                return 1 / 10.0;
            }
        }
        if (index == 0x2802 || index == 0x2803)  // bridge currents
        {
            if (subIndex > 0)
            {
                return 1 / 100.0;
            }
        }
        if (index == 0x2805)  // bridge bemf voltages
        {
            if (subIndex > 0)
            {
                return 1 / 10.0;
            }
        }
        if (index == 0x2810)  // driver temperature protection
        {
            if (subIndex == 1 || subIndex == 2)
            {
                return 1 / 10.0;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4006)  // Motor status
        {
            if (subIndex == 2)  // motor current
            {
                return 1 / 100.0;
            }
            if (subIndex == 7)  // motor temperature
            {
                return 1 / 10.0;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4007)  // Motor config
        {
            if (subIndex == 2 || subIndex == 3 || subIndex == 5)  // motor current limits
            {
                return 1 / 100.0;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4081)
        {
            if (subIndex == 3 || subIndex == 4)  // motor temperature protection ST
            {
                return 1 / 10.0;
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4082)
        {
            if (subIndex == 3 || subIndex == 4)  // Brake_duty
            {
                return 1 / 655.36;
            }
        }
    }
    return 1.0;
}

QString ODIndexDb::unit(quint16 index, quint8 subIndex, quint16 profileNumber)
{
    if (index == 0x100C)  // Guard time
    {
        if (subIndex == 0)
        {
            return QString(" ms");
        }
    }
    if (index == 0x1017)  // Producer heartbeat time
    {
        if (subIndex == 0)
        {
            return QString(" ms");
        }
    }
    if ((index >= 0x1400 && index <= 0x15FF) || (index >= 0x1800 && index <= 0x19FF))  // RPDO TPDO
    {
        if (subIndex == 3 || subIndex == 5)  // Inhibit Time and Event Timer
        {
            return QString(" ms");
        }
    }
    if (index == 0x2000)  // board voltages
    {
        if (subIndex > 0)
        {
            return QString(" V");
        }
    }
    if (index == 0x2020)  // cpu temps
    {
        if (subIndex > 0)
        {
            return QString(" °C");
        }
    }
    if (index == 0x2023 || index == 0x2024 || index == 0x2025)  // cpu stats us
    {
        if (subIndex > 0)
        {
            return QString(" µs");
        }
    }
    if (index == 0x2041)  // Board under/overvoltage
    {
        if (subIndex > 0)
        {
            return QString(" V");
        }
    }

    if (profileNumber == 402)
    {
        if (index == 0x2801)  // bridge temps
        {
            if (subIndex > 0)
            {
                return QString(" °C");
            }
        }
        if (index == 0x2802 || index == 0x2803)  // bridge currents
        {
            if (subIndex > 0)
            {
                return QString(" A");
            }
        }
        if (index == 0x2805)  // bridge bemf voltages
        {
            if (subIndex > 0)
            {
                return QString(" V");
            }
        }
        if (index == 0x2810)  // driver temperature protection
        {
            if (subIndex == 1 || subIndex == 2)
            {
                return QString(" °C");
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4006)  // Motor status
        {
            if (subIndex == 2)  // motor current
            {
                return QString(" A");
            }
            if (subIndex == 7)  // motor temperature
            {
                return QString(" °C");
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4007)  // Motor config
        {
            if (subIndex == 2 || subIndex == 3 || subIndex == 5)  // motor current limits
            {
                return QString(" A");
            }
            if (subIndex == 4)  // motor current limits
            {
                return QString(" ms");
            }
            if (subIndex == 6)  // current constant
            {
                return QString(" Nm/A");
            }
            if (subIndex == 7)  // maximum velocity
            {
                return QString(" rpm");
            }
            if (subIndex == 8)  // velocity constant
            {
                return QString(" rpm/V");
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4008)  // BLDC status
        {
            if (subIndex == 3)  // electrical angle
            {
                return QString(" °");
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4081)
        {
            if (subIndex == 3 || subIndex == 4)  // motor temperature protection ST
            {
                return QString(" °C");
            }
        }
        if ((index & (quint16)0xF1FF) == 0x4082)
        {
            if (subIndex == 2 || (subIndex >= 5 && subIndex <= 8))  // Brake_excitation_time_ms and delays
            {
                return QString(" ms");
            }
            if (subIndex == 3 || subIndex == 4)  // Brake_duty
            {
                return QString("%");
            }
        }

        if (index > 0x6000 && index < 0xB000)
        {
            uint16_t indexAxis = (index & 0x00FF) + 0x6000;
            if (indexAxis == 0x6048 || indexAxis == 0x6049 || indexAxis == 0x604A)  // Vl acc, dec, qudec
            {
                if (subIndex == 2)  // Delta_time
                {
                    return QString(" ms");
                }
            }
        }
    }
    return QString();
}
