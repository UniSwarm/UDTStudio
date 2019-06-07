
#include <stdlib.h>
#include <stdio.h>
#include "OD_controller.h"

//==========test on OD_OBJECT_VAR =============
int testReadObjectVar()
{
	void *ptData;
    int32_t code;
    uint16_t *value;

    code = OD_read(0x1017, 0x00, &ptData);
	
    value = (uint16_t*)ptData;

	if (code >= 0)
	{
        printf("1017 value: %x\n", *value);
        return 0;
	}
	else
	{
		printf("code retour: %x\n", -code);
		exit(-1);
	}
}

int testReadObjectVarRO()
{
	void *ptData;
    int32_t code;
    uint32_t *value;

	code = OD_read(0x1001, 0x00, &ptData);
	
    value = (uint32_t*)ptData;

	if (code >= 0)
	{
        printf("1001 value: %x\n", *value);
        return 0;
	}
	else
	{
		printf("code retour: %x\n", -code);
		exit(-1);
	}
}

int testReadObjectVarNoObject()
{
	void *ptData;
    int32_t code;
    uint32_t *value;

	code = OD_read(0x1002, 0x00, &ptData);
	
    value = (uint32_t*)ptData;

	if (code >= 0)
	{
        printf("1001 value: %x\n", *value);
		exit(-1);
	}
	else
	{
		printf("code retour: %x\n", -code);
		return 0;
	}
}

int testReadObjectVarNoSub()
{
	void *ptData;
    int32_t code;
    uint32_t *value;

	code = OD_read(0x1001, 0x01, &ptData);
	
    value = (uint32_t*)ptData;

	if (code >= 0)
	{
        printf("1001 value: %x\n", *value);
		exit(-1);
	}
	else
	{
		printf("code retour: %x\n", -code);
		return 0;
	}
}

//==========test on OD_OBJECT_RECORD ==========
int testReadObjectRecord()
{
	void *ptData;
    int32_t code;
    uint8_t *value;

	code = OD_read(0x1018, 0x00, &ptData);
	
    value = (uint8_t*)ptData;

	if (code >= 0)
	{
        printf("1018sub0 value: %x\n", *value);
        return 0;
	}
	else
	{
		printf("code retour: %x\n", -code);
		exit(-1);
	}
}



int main()
{
	OD_reset();
	printf("==========test on OD_OBJECT_VAR =============\n");
	testReadObjectVar();
    testReadObjectVarRO();
    testReadObjectVarNoObject();
    testReadObjectVarNoSub();
    printf("==========test on OD_OBJECT_RECORD ==========\n");
    testReadObjectRecord();

	return 0;
}
