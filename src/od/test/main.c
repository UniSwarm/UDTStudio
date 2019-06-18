
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "od_controller.h"
#include "pdo.h"

//==========test on OD_OBJECT_VAR =============
void testReadObjectVar()
{
    uint16_t *value;

    assert(od_read(0x1017, 0x00, (void**)&value) == 6);
    assert(*value == 0x17);
}

void testReadObjectVarRO()
{
    uint8_t *value;

    assert(od_read(0x1001, 0x00, (void**)&value) == 5);
    assert(*value == 0x1);
}

void testReadObjectVarNoObject()
{
    uint8_t *value;

    assert(od_read(0x10, 0x00, (void**)&value) == -OD_ABORT_CODE_NO_OBJECT);
}

void testReadObjectVarNoSub()
{
    uint8_t *value;

    assert(od_read(0x1001, 0x01, (void**)&value) == -OD_ABORT_CODE_NO_SUBINDEX);
}

//==========test on OD_OBJECT_RECORD ==========
void testReadObjectRecord()
{
    uint32_t *value;

    assert(od_read(0x1018, 0x01, (void**)&value) == 7);
    assert(*value == 0x1);
}

void testReadObjectRecordNonConituous()
{
    uint32_t *value;

    assert(od_read(0x1601, 0x08, (void**)&value) == 7);
    assert(*value == 0x8);
}

void testWriteObjectRecordNonConituous()
{
    uint32_t data = 4;
    uint32_t *value;

    assert(od_write(0x1601, 0x08, (void*)&data, 4) == 7);
    assert(od_read(0x1601, 0x08, (void**)&value) == 7);
    assert(*value == 4);
}

void testWriteObjectVar()
{
    uint32_t data = 4;
    uint16_t *value;

    assert(od_write(0x1017, 0x00, (void*)&data, 2) == 6);
    assert(od_read(0x1017, 0x00, (void**)&value) == 6);
    assert(*value == 4);
}

void testWriteObjectVarRO()
{
    uint32_t data = 4;
    uint8_t *value;

    assert(od_write(0x1001, 0x00, (void*)&data, 1) == -OD_ABORT_CODE_READ_ONLY);
    assert(od_read(0x1001, 0x00, (void**)&value) == 5);
    assert(*value == 0x1);
}

void testWriteObjectVarNoObject()
{
    uint32_t data = 4;

    assert(od_write(0x10, 0x00, (void*)&data, 1) == -OD_ABORT_CODE_NO_OBJECT);
}

void testWriteObjectVarNoSub()
{
    uint32_t data = 4;

    assert(od_write(0x1017, 0x06, (void*)&data, 1) == -OD_ABORT_CODE_NO_SUBINDEX);
    assert(od_write(0x1601, 0x06, (void*)&data, 1) == -OD_ABORT_CODE_NO_SUBINDEX);
}

//============= test on array ==============
void testWriteArray()
{
    uint32_t data = 4;
    uint8_t *value;

    assert(od_write(0x6047, 0x1, (void*)&data, 4) == 7);
    assert(od_read(0x6047, 0x01, (void**)&value) == 7);
    assert(*value == 4);
}

void testWriteArrayRO()
{
    int16_t data = 4;
    uint8_t *value;

    assert(od_write(0x604B, 0x2, (void*)&data, 2) == -OD_ABORT_CODE_READ_ONLY);
    assert(od_read(0x604B, 0x02, (void**)&value) == 3);
    assert(*value == 1);
}

void testWriteArrayNoObject()
{
    int16_t data = 4;
    uint8_t *value;

    assert(od_write(0x604, 0x0, (void*)&data, 2) == -OD_ABORT_CODE_NO_OBJECT);
    assert(od_read(0x604, 0x0, (void**)&value) == -OD_ABORT_CODE_NO_OBJECT);
}

void testWriteArrayNoSub()
{
    int16_t data = 4;
    uint8_t *value;

    assert(od_write(0x6047, 0x7, (void*)&data, 2) == -OD_ABORT_CODE_NO_SUBINDEX);
    assert(od_read(0x6047, 0x7, (void**)&value) == -OD_ABORT_CODE_NO_SUBINDEX);

    assert(od_write(0x1017, 0x7, (void*)&data, 2) == -OD_ABORT_CODE_NO_SUBINDEX);
    assert(od_read(0x1017, 0x7, (void**)&value) == -OD_ABORT_CODE_NO_SUBINDEX);
}

void testWriteBadSize()
{
    int16_t data = 4;

    assert(od_write(0x6047, 0x4, (void*)&data, 2) == -OD_ABORT_CODE_LENGTH_DOESNT_MATCH);
    assert(od_write(0x1017, 0x0, (void*)&data, 4) == -OD_ABORT_CODE_LENGTH_DOESNT_MATCH);
}

//=========test on OD_TYPE_VISIBLE_STRING=========
void testReadVisibleString()
{
    vstring_t* value1;
    ostring_t* value2;

    assert(od_read(0x1008, 0x00, (void**)&value1) == 9);
    assert(strcmp((char*)*value1, "test1") == 0);

    assert(od_read(0x4041, 0x00, (void**)&value2) == 0xA);
    assert(strcmp((char*)*value2, "test2") == 0);
}

void testWriteObjectBool()
{
    uint8_t *value;
    uint8_t data = 1;

    assert(od_write(0x6423, 0x00, (void*)&data, 1) == 1);
    assert(od_read(0x6423, 0x00, (void**)&value) == 1);
    assert(*value == 1);

    data = 2;
    assert(od_write(0x6423, 0x00, (void*)&data, 1) == -OD_ABORT_CODE_LENGTH_DOESNT_MATCH);
}

void testCustomEds()
{  
    testReadObjectVar();
    testReadObjectVarRO();
    testReadObjectVarNoObject();
    testReadObjectVarNoSub();
    testReadObjectRecord();
    testReadObjectRecordNonConituous();
    testWriteObjectRecordNonConituous();
    testWriteObjectVar();
    testWriteObjectVarRO();
    testWriteObjectVarNoObject();
    testWriteObjectVarNoSub();
    testWriteArray();
    testWriteArrayRO();
    testWriteArrayNoObject();
    testWriteArrayNoSub();
    testWriteBadSize();
    testReadVisibleString();
    testWriteObjectBool();
}

void testUmcEds()
{
    vstring_t *value;

    assert(od_read(0x1008, 0x0, (void*)&value) == 9);
    assert(strcmp((char *)*value, "") == 0);

    assert(od_read(0x1009, 0x0, (void*)&value) == 9);
    assert(strcmp((char *)*value, "") == 0);

    assert(od_read(0x100A, 0x0, (void*)&value) == 9);
    assert(strcmp((char *)*value, "3150.69D") == 0);
}

void testTPDO()
{
    TPDO tpdo;

    mapTpdo(&tpdo, 1);

    assert(tpdo.cob_id.can_id == 0x282);
    assert(tpdo.dataSize == 6);
    printf("%s\n", tpdo.dataToTransmit);
}

int main()
{
    od_reset();

    testTPDO();

	return 0;
}
