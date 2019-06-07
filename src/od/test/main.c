
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "OD_controller.h"

//==========test on OD_OBJECT_VAR =============
void testReadObjectVar()
{
    int32_t code;
    uint16_t *value;

    assert(OD_read(0x1017, 0x00, &value) == 6);
    assert(*value == 0x17);
}

void testReadObjectVarRO()
{
    int32_t code;
    uint8_t *value;

    assert(OD_read(0x1001, 0x00, &value) == 5);
    assert(*value == 0x1);
}

void testReadObjectVarNoObject()
{
	int32_t code;
    uint8_t *value;

    assert(OD_read(0x10, 0x00, &value) == -OD_ABORT_CODE_NO_OBJECT);
}

void testReadObjectVarNoSub()
{
	int32_t code;
    uint8_t *value;

    assert(OD_read(0x1001, 0x01, &value) == -OD_ABORT_CODE_NO_SUBINDEX);
}

//==========test on OD_OBJECT_RECORD ==========
void testReadObjectRecord()
{
    int32_t code;
    uint32_t *value;

    assert(OD_read(0x1018, 0x01, &value) == 7);
    assert(*value == 0x1);
}

//=========test on OD_TYPE_VISIBLE_STRING=========
void testVisibleString()
{
	int32_t 	code;
	vstring_t*	value;

	assert(OD_read(0x1008, 0x00, &value) == 9);
	assert(strcmp(*value, "test") == 0);
}

int main()
{
	OD_reset();

	testReadObjectVar();
    testReadObjectVarRO();
    testReadObjectVarNoObject();
    testReadObjectVarNoSub();
    testReadObjectRecord();
	testVisibleString();

	return 0;
}
