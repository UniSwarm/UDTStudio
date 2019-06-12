#ifndef SDO_H
#define SDO_H

#include <stdint.h>

// ===== Datatype definitions =================
typedef float       float32_t;
typedef double      float64_t;
typedef char*       vstring_t;

// =============== Data type ==================
#define OD_TYPE_BOOLEAN         0x0001
#define OD_TYPE_INTEGER8        0x0002
#define OD_TYPE_INTEGER16       0x0003
#define OD_TYPE_INTEGER32       0x0004
#define OD_TYPE_UNSIGNED8       0x0005
#define OD_TYPE_UNSIGNED16      0x0006
#define OD_TYPE_UNSIGNED32      0x0007
#define OD_TYPE_REAL32          0x0008
#define OD_TYPE_VISIBLE_STRING  0x0009
#define OD_TYPE_OCTET_STRING    0x000A
#define OD_TYPE_UNICODE_STRING  0x000B
#define OD_TYPE_TIME_OF_DAY     0x000C
#define OD_TYPE_TIME_DIFFERENCE 0x000D
#define OD_TYPE_DOMAIN          0x000F
#define OD_TYPE_INTEGER24       0x0010
#define OD_TYPE_REAL64          0x0011
#define OD_TYPE_INTEGER40       0x0012
#define OD_TYPE_INTEGER48       0x0013
#define OD_TYPE_INTEGER56       0x0014
#define OD_TYPE_INTEGER64       0x0015
#define OD_TYPE_UNSIGNED24      0x0016
#define OD_TYPE_UNSIGNED40      0x0018
#define OD_TYPE_UNSIGNED48      0x0019
#define OD_TYPE_UNSIGNED56      0x001A
#define OD_TYPE_UNSIGNED64      0x001B

// =========== Object type ====================
#define OD_OBJECT_NULL          0x0000
#define OD_OBJECT_DOMAIN        0x2000
#define OD_OBJECT_DEFTYPE       0x5000
#define OD_OBJECT_DEFSTRUCT     0x6000
#define OD_OBJECT_VAR           0x7000
#define OD_OBJECT_ARRAY         0x8000
#define OD_OBJECT_RECORD        0x9000

// ============== Access type =================
#define OD_ACCESS_READ_WRITE    0x0001
#define OD_ACCESS_WRITE_ONLY    0x0002
#define OD_ACCESS_READ_ONLY     0x0003
#define OD_ACCESS_CONST         0x0004

// ============= masks ========================
#define OD_TYPE_MASK            0x0FFF
#define OD_OBJECT_MASK          0xF000
#define OD_ACCESS_MASK          0x000F
#define OD_PDOMAPPING_MASK      0X00F0

// ====    struct definitons for OD entries =====
// Sub-index entry for OD
typedef struct {
    void*       ptData;
    uint8_t     length;
    uint16_t    typeObject;
    uint8_t     accessPDOmapping;
    uint8_t     subNumber;
} OD_entrySubIndex_t;

// OD entry
// !!! length at 0 when object is a record
typedef struct {
    uint16_t   index;
    uint8_t    nbSubIndex;
    void*      ptData;
    uint8_t    length;
    uint16_t   typeObject;
    uint8_t    accessPDOmapping;
} OD_entry_t;

#endif // SDO_H
