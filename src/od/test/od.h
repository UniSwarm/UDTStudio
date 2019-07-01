#ifndef OD_H
#define OD_H

#include "od_data.h"

// ============ sdo abort codes =============
#define OD_ABORT_CODE_WRITE_ONLY            0x06010001
#define OD_ABORT_CODE_READ_ONLY             0x06010002
#define OD_ABORT_CODE_NO_OBJECT             0x06020000
#define OD_ABORT_CODE_NO_SUBINDEX           0x06090011
#define OD_ABORT_CODE_LENGTH_DOESNT_MATCH   0x06070010

// =============== functions ================
void od_reset(void);

int32_t od_read(uint16_t index, uint8_t subIndex, void **ptData);
int32_t od_write(uint16_t index, uint8_t subIndex, void *ptData, uint8_t size);

uint8_t od_sizeFromType(uint16_t dataType);

#endif // OD_H
