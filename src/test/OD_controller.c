
#include <stdio.h>
#include "OD_controller.h"

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

    while((end-start) > 1)
	{
		middle = (start + end)/2;

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
 * @brief Read an index or sub-index at index number 'index' and sub-index number "subIndex" 
 * @param index eds index number
 * @param subIndex eds sub-index number
 * @param **ptData pointer to returned index OD's entry
 * @return negative sdo abort code or data type code;
 */
int32_t OD_read(uint16_t index, uint8_t subIndex, void **ptData)
{
    OD_entry_t *entry;
    OD_entrySubIndex_t *record;

    entry = OD_getIndexDicho(index);

    if (entry == NULL)
    {
        return -OD_ABORT_CODE_NO_OBJECT;
    }

    if ((entry->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_WRITE_ONLY) 
    {
        return -OD_ABORT_CODE_WRITE_ONLY;
    }

    if (subIndex > entry->nbSubIndex)
    {
        return -OD_ABORT_CODE_NO_SUBINDEX;
    }

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_RECORD)
    {
        record = entry->ptData;

        if ((record[subIndex].accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_WRITE_ONLY)
        {
            return -OD_ABORT_CODE_WRITE_ONLY;
        }

        record = &record[subIndex];
        *ptData = (void*)record->ptData;
        return record->typeObject & OD_TYPE_MASK;
    }

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_ARRAY)
    {
        *ptData = (void*)entry->ptData+((entry->length)*(subIndex-1));
        return entry->typeObject & OD_TYPE_MASK;
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
    int8_t *i8;
    int16_t *i16;
    int32_t *i32;
    int64_t *i64;
    uint8_t *ui8;
    uint16_t *ui16;
    uint32_t *ui32;
    uint64_t *ui64;
    float32_t *f32;
    float64_t *f64;
    OD_entry_t *entry;
    OD_entrySubIndex_t *record, *sub;

    entry = OD_getIndexDicho(index);

    if (entry == NULL)
    {
        return -OD_ABORT_CODE_NO_OBJECT;
    }

    if ((entry->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_READ_ONLY || (entry->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_CONST) 
    {
        return -OD_ABORT_CODE_READ_ONLY;
    }

    if (subIndex > entry->nbSubIndex)
    {
        return -OD_ABORT_CODE_NO_SUBINDEX;
    }

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_RECORD)
    {
        record = entry->ptData;
        sub = &record[subIndex];

        if ((sub->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_READ_ONLY || (sub->accessPDOmapping & OD_ACCESS_MASK) == OD_ACCESS_CONST)
        {
            return -OD_ABORT_CODE_READ_ONLY;
        }

        if ((sub->typeObject & OD_TYPE_MASK) != dataType)
        {
           return -OD_ABORT_CODE_LENGTH_DOESNT_MATCH;
        }

        switch (dataType)
        {
        case OD_TYPE_UNSIGNED8:
           ui8 = (uint8_t*)sub->ptData;
           *ui8 = *(uint8_t*)ptData;
           break;
        
        case OD_TYPE_UNSIGNED16:
           ui16 = (uint16_t*)sub->ptData;
           *ui16 = *(uint16_t*)ptData;
           break;
        
        case OD_TYPE_UNSIGNED32:
            ui32 = (uint32_t*)sub->ptData;
            *ui32 = *((uint32_t*)ptData);
           break;

        case OD_TYPE_UNSIGNED64:
            ui64 = (uint64_t*)sub->ptData;
            *ui64 = *((uint64_t*)ptData);
           break;           

        case OD_TYPE_INTEGER8:
           i8 = (int8_t*)sub->ptData;
           *i8 = *(int8_t*)ptData;
           break;
        
        case OD_TYPE_INTEGER16:
           i16 = (int16_t*)sub->ptData;
           *i16 = *(int16_t*)ptData;
           break;
        
        case OD_TYPE_INTEGER32:
            i32 = (int32_t*)sub->ptData;
            *i32 = *((int32_t*)ptData);
           break;

        case OD_TYPE_INTEGER64:
            i64 = (int64_t*)sub->ptData;
            *i64 = *((int64_t*)ptData);
           break;    

        case OD_TYPE_REAL32:
            f32 = (float32_t*)sub->ptData;
            *f32 = *((float32_t*)ptData);
           break;

        case OD_TYPE_REAL64:
            f64 = (float64_t*)sub->ptData;
            *f64 = *((float64_t*)ptData);
           break;                    
        }

        return sub->typeObject & OD_TYPE_MASK;
    }

    if ((entry->typeObject & OD_TYPE_MASK) != dataType)
    {
       return -OD_ABORT_CODE_LENGTH_DOESNT_MATCH;
    }

    if ((entry->typeObject & OD_OBJECT_MASK) == OD_OBJECT_ARRAY)
    {
        switch (dataType)
        {
        case OD_TYPE_UNSIGNED8:
           ui8 = (uint8_t*)entry->ptData+((entry->length)*(subIndex-1));
           *ui8 = *(uint8_t*)ptData;
           break;
        
        case OD_TYPE_UNSIGNED16:
           ui16 = (uint16_t*)entry->ptData+((entry->length)*(subIndex-1));
           *ui16 = *(uint16_t*)ptData;
           break;
        
        case OD_TYPE_UNSIGNED32:
            ui32 = (uint32_t*)entry->ptData+((entry->length)*(subIndex-1));
            *ui32 = *((uint32_t*)ptData);
           break;

        case OD_TYPE_UNSIGNED64:
            ui64 = (uint64_t*)entry->ptData+((entry->length)*(subIndex-1));
            *ui64 = *((uint64_t*)ptData);
           break;           

        case OD_TYPE_INTEGER8:
           i8 = (int8_t*)entry->ptData+((entry->length)*(subIndex-1));
           *i8 = *(int8_t*)ptData;
           break;
        
        case OD_TYPE_INTEGER16:
           i16 = (int16_t*)entry->ptData+((entry->length)*(subIndex-1));
           *i16 = *(int16_t*)ptData;
           break;
        
        case OD_TYPE_INTEGER32:
            i32 = (int32_t*)entry->ptData+((entry->length)*(subIndex-1));
            *i32 = *((int32_t*)ptData);
           break;

        case OD_TYPE_INTEGER64:
            i64 = (int64_t*)entry->ptData+((entry->length)*(subIndex-1));
            *i64 = *((int64_t*)ptData);
           break;     

        case OD_TYPE_REAL32:
            f32 = (float32_t*)entry->ptData+((entry->length)*(subIndex-1));
            *f32 = *((float32_t*)ptData);
           break;

        case OD_TYPE_REAL64:
            f64 = (float64_t*)entry->ptData+((entry->length)*(subIndex-1));
            *f64 = *((float64_t*)ptData);
           break;                  
        }

        return entry->typeObject & OD_TYPE_MASK;
    }    

    switch (dataType)
    {
    case OD_TYPE_UNSIGNED8:
       ui8 = (uint8_t*)entry->ptData;
       *ui8 = *(uint8_t*)ptData;
       break;
    
    case OD_TYPE_UNSIGNED16:
       ui16 = (uint16_t*)entry->ptData;
       *ui16 = *(uint16_t*)ptData;
       break;
    
    case OD_TYPE_UNSIGNED32:
       ui32 = (uint32_t*)entry->ptData;
       *ui32 = *(uint32_t*)ptData;
       break;

    case OD_TYPE_UNSIGNED64:
       ui64 = (uint64_t*)entry->ptData;
       *ui64 = *(uint64_t*)ptData;
       break;       

    case OD_TYPE_INTEGER8:
       i8 = (int8_t*)entry->ptData;
       *i8 = *(int8_t*)ptData;
       break;
    
    case OD_TYPE_INTEGER16:
       i16 = (int16_t*)entry->ptData;
       *i16 = *(int16_t*)ptData;
       break;
    
    case OD_TYPE_INTEGER32:
       i32 = (int32_t*)entry->ptData;
       *i32 = *(int32_t*)ptData;
       break; 

    case OD_TYPE_INTEGER64:
       i64 = (int64_t*)entry->ptData;
       *i64 = *(int64_t*)ptData;
       break;  

    case OD_TYPE_REAL32:
       f32 = (float32_t*)entry->ptData;
       *f32 = *(float32_t*)ptData;
       break; 

    case OD_TYPE_REAL64:
       f64 = (float64_t*)entry->ptData;
       *f64 = *(float64_t*)ptData;
       break;   
    }

    return entry->typeObject & OD_TYPE_MASK;
}