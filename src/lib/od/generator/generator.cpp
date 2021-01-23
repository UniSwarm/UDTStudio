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

#include "generator.h"

#include "cgenerator.h"

/**
 * @brief default constructor
 */
Generator::Generator()
{
}

/**
 * @brief virtual default destructor
 */
Generator::~Generator()
{
}

/**
 * @brief returns the generator wich corresponds with the type passed in parameter
 * @param type of the generator
 * @return a generator
 */
Generator *Generator::getGenerator(const QString &type)
{
    if (type == "c")
    {
        return new CGenerator();
    }
    return nullptr;
}
