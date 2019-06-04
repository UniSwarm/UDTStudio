
#include <stdio.h>
#include "OD_controller.h"

/**
 */
void testReadIndex()
{
	void *ptData;
	OD_entry_t *entry;
    int32_t code;
    uint8_t *value;

	code = OD_read(0x1001, 0x00, &ptData);
	
    value = (uint8_t*)ptData;

	if (code >= 0)
	{
		printf("main code retour: %x\n", code);
        printf("main value: %x\n", *value);
	}
	else
		printf("main code retour: %x\n", -code);
}
/**
 */
void testReadRecord()
{
	void *ptData;
	OD_entrySubIndex_t* subIndex;
    int32_t code;
	uint32_t *value;

    code = OD_read(0x1018, 0x03, &ptData);
    value = (uint32_t*)ptData;
	
	if (code >= 0)
	{
		printf("main code retour: %x\n", code);
        printf("main value: %x\n", *value);
	}
	else
		printf("main code retour: %x\n", -code);
}
/*
*/
void testWriteIndex()
{
	uint8_t value;
    int32_t code;

	value = 0x03;

	code = OD_write(0x1001, 0x00, (void*)&value, OD_TYPE_UNSIGNED8);

	if (code >= 0)
		printf("main code retour: %x\n", code);
	else 
		printf("main code retour: %x\n", -code);		
}
/**
 */
void testWriteSubIndex()
{
	uint32_t value;
    int32_t code;

	value = 0x00000005;
	code = OD_write(0x1018, 0x03, (void*)&value, OD_TYPE_UNSIGNED16);
	
	if (code >= 0)
		printf("main code retour: %x\n", code);
	else 
		printf("main code retour: %x\n", -code);	
}
/**
 */
int main(){
	OD_reset();

    printf("Test lecture d'index:\n");
    testReadIndex();
    printf("\n====================================\n");

    printf("Test lecture de record:\n");
    testReadRecord();
    printf("\n====================================\n");
	
    printf("Test écriture d'index:\n");
    testReadIndex();
    printf("============================\n");
    testWriteIndex();
    printf("============================\n");
    testReadIndex();
    printf("\n====================================\n");

    printf("Test ecriture record:\n");
    testReadRecord();
    printf("============================\n");
    testWriteSubIndex();
    printf("============================\n");
    testReadRecord();
    printf("\n====================================\n");

	return 0;
}