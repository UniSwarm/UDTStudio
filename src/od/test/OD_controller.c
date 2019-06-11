#include "OD_controller.h"

#include <stdio.h>

/**
 * @brief Reset the RAM memory
 */
void OD_reset()
{
    OD_initRam();
}

/**
 * @brief Find an OD's entry by index
 * @param index eds index number
 * @return adress of OD's entry
 */
OD_entry_t* OD_getIndex(uint16_t index)
{
    int i;
    OD_entry_t *entry;

    for (i = 0; i < OD_NB_ELEMENTS; i++)
    {
        if (OD[i].index == index)
        {
            entry = (OD_entry_t *)&OD[i];
            return entry;
        }
    }

    return NULL;
}

/**
 * @brief Find an OD's entry by index with dichotomy
 * @param index eds index number
 * @return adress of OD's entry
 */
OD_entry_t* OD_getIndexDicho(uint16_t index)
{
    int middle;
    int start = 0;
    int end = OD_NB_ELEMENTS;

    while ((end - start) > 1)
    {
        middle = (start + end) / 2;

        if (OD[middle].index == index)
        {
            return (OD_entry_t *)&OD[middle];
        }

        if (OD[middle].index > index)
        {
            end = middle;
        }
        else
        {
            start = middle;
        }
    }

    if (OD[start].index == index)
    {
        return (OD_entry_t*)&OD[start];
    }

    return NULL;
}

/**
 */
OD_entrySubIndex_t* OD_getSubIndex(OD_entry_t *record, uint8_t subIndex)
{
    int i;
    OD_entrySubIndex_t *subIndexes = (OD_entrySubIndex_t*)record->ptData;

    for (i=0; i<=record->nbSubIndex; i++)
    {
        if (subIndexes[i].subNumber == subIndex)
        {
            return (OD_entrySubIndex_t*)&subIndexes[i];
        }
    }

    return NULL;
}

/**
 * @brief Read an index or sub-index at index number 'index' and sub-index number "subIndex"
 * @param index eds index number
 * @param subIndex eds sub-index number
 * @param **ptData pointer to returned index OD's entry
 * @return negative sdo abort code or data type code;
 */
int32_t OD_read(uint16_t index, uint8_t subIndex, void **ptData)
{
    OD_entry_t *entry;
    OD_entrySubIndex_t *record, *subIndexData;

    entry = OD_getIndexDicho(index);

    if (entry == NULL)
        return -OD_ABORT_CODE_NO_OBJECT;

    if ((entry->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_WRITE_ONLY)
        return -OD_ABORT_CODE_WRITE_ONLY;

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_RECORD)
    {
        record = entry->ptData;

        subIndexData = OD_getSubIndex(entry, subIndex);
        
        if (subIndexData == NULL)
            return -OD_ABORT_CODE_NO_SUBINDEX;

        if ((subIndexData->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_WRITE_ONLY)
            return -OD_ABORT_CODE_WRITE_ONLY;

        *ptData = (void*)subIndexData->ptData;
        return subIndexData->typeObject & OD_TYPE_MASK;
    }

    if (subIndex > entry->nbSubIndex)
        return -OD_ABORT_CODE_NO_SUBINDEX;

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_ARRAY)
    {
        if (subIndex == 0)
        {
            *ptData = (void*)(&entry->nbSubIndex);
            return OD_TYPE_UNSIGNED8;
        }
        else
        {
            *ptData = (void*)entry->ptData + (entry->length * (subIndex - 1));
            return entry->typeObject & OD_TYPE_MASK;
        }
    }

    *ptData = (void *)entry->ptData;
    return entry->typeObject & OD_TYPE_MASK;
}

/**
 * @brief write data in an index or in an sub-index indentified by 'index' and 'subIndex'
 * @param index eds index number
 * @param subIndex eds sub-index number
 * @param *ptData pointer to data
 * @param dataType variable type of data
 * @return negative sdo abort code or data type code
 */
int32_t OD_write(uint16_t index, uint8_t subIndex, void *ptData, uint16_t dataType)
{
    OD_entry_t *entry;
    OD_entrySubIndex_t *record, *sub;
    uint8_t nbSubIndex;

    entry = OD_getIndexDicho(index);

    if (entry == NULL)
        return -OD_ABORT_CODE_NO_OBJECT;

    if ((entry->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_READ_ONLY
     || (entry->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_CONST)
        return -OD_ABORT_CODE_READ_ONLY;

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_RECORD)
    {
        record = entry->ptData;
        sub = OD_getSubIndex(entry, subIndex);

        if ((sub->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_READ_ONLY
         || (sub->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_CONST)
            return -OD_ABORT_CODE_READ_ONLY;

        if ((sub->typeObject & OD_TYPE_MASK) != dataType)
            //return -OD_ABORT_CODE_LENGTH_DOESNT_MATCH;

        dataType = sub->typeObject & OD_TYPE_MASK;
        switch (dataType)
        {
        case OD_TYPE_UNSIGNED8:
            *((uint8_t*)sub->ptData) = *(uint8_t*)ptData;
            break;

        case OD_TYPE_UNSIGNED16:
            *((uint16_t*)sub->ptData) = *(uint16_t*)ptData;
            break;

        case OD_TYPE_UNSIGNED32:
            *((uint32_t*)sub->ptData) = *((uint32_t*)ptData);
            break;

        case OD_TYPE_UNSIGNED64:
            *((uint64_t*)sub->ptData) = *((uint64_t*)ptData);
            break;

        case OD_TYPE_INTEGER8:
            *((int8_t*)sub->ptData) = *(int8_t*)ptData;
            break;

        case OD_TYPE_INTEGER16:
            *((int16_t*)sub->ptData) = *(int16_t*)ptData;
            break;

        case OD_TYPE_INTEGER32:
            *((int32_t*)sub->ptData) = *((int32_t*)ptData);
            break;

        case OD_TYPE_INTEGER64:
            *((int64_t*)sub->ptData) = *((int64_t*)ptData);
            break;

        case OD_TYPE_REAL32:
            *((float32_t*)sub->ptData) = *((float32_t*)ptData);
            break;

        case OD_TYPE_REAL64:
            *((float64_t*)sub->ptData) = *((float64_t*)ptData);
            break;
        }

        return sub->typeObject & OD_TYPE_MASK;
    }

    if (subIndex > entry->nbSubIndex)
        return -OD_ABORT_CODE_NO_SUBINDEX;

    if ((entry->typeObject & OD_TYPE_MASK) != dataType)
        //return -OD_ABORT_CODE_LENGTH_DOESNT_MATCH;

    dataType = entry->typeObject & OD_TYPE_MASK;
    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_ARRAY)
    {
        switch (dataType)
        {
        case OD_TYPE_UNSIGNED8:
            *((uint8_t*)entry->ptData + (entry->length * (subIndex - 1))) = *(uint8_t*)ptData;
            break;

        case OD_TYPE_UNSIGNED16:
            *((uint16_t*)entry->ptData + (entry->length * (subIndex - 1))) = *(uint16_t*)ptData;
            break;

        case OD_TYPE_UNSIGNED32:
            *((uint32_t*)entry->ptData + (entry->length * (subIndex - 1))) = *((uint32_t*)ptData);
            break;

        case OD_TYPE_UNSIGNED64:
            *((uint64_t*)entry->ptData + (entry->length * (subIndex - 1))) = *((uint64_t*)ptData);
            break;

        case OD_TYPE_INTEGER8:
            *((int8_t*)entry->ptData + (entry->length * (subIndex - 1))) = *(int8_t*)ptData;
            break;

        case OD_TYPE_INTEGER16:
            *((int16_t*)entry->ptData + (entry->length * (subIndex - 1))) = *(int16_t*)ptData;
            break;

        case OD_TYPE_INTEGER32:
            *((int32_t*)entry->ptData + (entry->length * (subIndex - 1))) = *((int32_t*)ptData);
            break;

        case OD_TYPE_INTEGER64:
            *((int64_t*)entry->ptData + (entry->length * (subIndex - 1))) = *((int64_t*)ptData);
            break;

        case OD_TYPE_REAL32:
            *((float32_t*)entry->ptData + (entry->length * (subIndex - 1))) = *((float32_t*)ptData);
            break;

        case OD_TYPE_REAL64:
            *((float64_t*)entry->ptData + (entry->length * (subIndex - 1))) = *((float64_t*)ptData);
            break;
        }

        return entry->typeObject & OD_TYPE_MASK;
    }

    switch (dataType)
    {
    case OD_TYPE_UNSIGNED8:
        *((uint8_t*)entry->ptData) = *(uint8_t*)ptData;
        break;

    case OD_TYPE_UNSIGNED16:
        *((uint16_t*)entry->ptData) = *(uint16_t*)ptData;
        break;

    case OD_TYPE_UNSIGNED32:
        *((uint32_t*)entry->ptData) = *(uint32_t*)ptData;
        break;

    case OD_TYPE_UNSIGNED64:
        *((uint64_t*)entry->ptData) = *(uint64_t*)ptData;
        break;

    case OD_TYPE_INTEGER8:
        *((int8_t*)entry->ptData) = *(int8_t*)ptData;
        break;

    case OD_TYPE_INTEGER16:
        *((int16_t*)entry->ptData) = *(int16_t*)ptData;
        break;

    case OD_TYPE_INTEGER32:
        *((int32_t*)entry->ptData) = *(int32_t*)ptData;
        break;

    case OD_TYPE_INTEGER64:
        *((int64_t*)entry->ptData) = *(int64_t*)ptData;
        break;

    case OD_TYPE_REAL32:
        *((float32_t*)entry->ptData) = *(float32_t*)ptData;
        break;

    case OD_TYPE_REAL64:
        *((float64_t*)entry->ptData) = *(float64_t*)ptData;
        break;
    }

    return entry->typeObject & OD_TYPE_MASK;
}

uint8_t OD_sizeFromType(uint16_t dataType)
{
    switch (dataType)
    {
    case OD_TYPE_UNSIGNED8:
    case OD_TYPE_INTEGER8:
        return 1;

    case OD_TYPE_UNSIGNED16:
    case OD_TYPE_INTEGER16:
        return 2;

    case OD_TYPE_UNSIGNED32:
    case OD_TYPE_INTEGER32:
    case OD_TYPE_REAL32:
        return 4;

    case OD_TYPE_UNSIGNED64:
    case OD_TYPE_INTEGER64:
    case OD_TYPE_REAL64:
        return 8;
    }
    return 0;
}
