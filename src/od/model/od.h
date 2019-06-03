#ifndef OD_H
#define OD_H

#include <QMap>

#include "index.h"

#include <stdint.h>

// =============== Data type ==================
#define OD_TYPE_BOOLEAN       	0x0001
#define OD_TYPE_INTEGER8      	0x0002
#define OD_TYPE_INTEGER16     	0x0003
#define OD_TYPE_INTEGER32     	0x0004
#define OD_TYPE_UNSIGNED8     	0x0005
#define OD_TYPE_UNSIGNED16    	0x0006
#define OD_TYPE_UNSIGNED32    	0x0007
#define OD_TYPE_REAL32        	0x0008
#define OD_TYPE_VISIBLE_STRING	0x0009
#define OD_TYPE_OCTET_STRING  	0x000A
#define OD_TYPE_UNICODE_STRING	0x000B
#define OD_TYPE_TIME_OF_DAY   	0x000C
#define OD_TIME_DIFFERENCE    	0x000D
#define OD_TYPE_DOMAIN        	0x000F
#define OD_TYPE_INTEGER24     	0x0010
#define OD_TYPE_REAL64        	0x0011
#define OD_TYPE_INTEGER40     	0x0012
#define OD_TYPE_INTEGER48     	0x0013
#define OD_TYPE_INTEGER56     	0x0014
#define OD_TYPE_INTEGER64     	0x0015
#define OD_TYPE_UNSIGNED24    	0x0016
#define OD_TYPE_UNSIGNED40    	0x0018
#define OD_TYPE_UNSIGNED48    	0x0019
#define OD_TYPE_UNSIGNED56    	0x001A
#define OD_TYPE_UNSIGNED64    	0x001B

// =========== Object type ====================
#define OD_OBJECT_NULL 			0x00
#define OD_OBJECT_DOMAIN      	0x02
#define OD_OBJECT_DEFTYPE     	0x05
#define OD_OBJECT_DEFSTRUCT   	0x06
#define OD_OBJECT_VAR         	0x07
#define OD_OBJECT_ARRAY       	0x08
#define OD_OBJECT_RECORD      	0x09

// ============== Access type =================
#define OD_ACCESS_READ_WRITE	0x01
#define OD_ACCESS_WRITE_ONLY	0x02
#define OD_ACCESS_READ_ONLY		0x03
#define OD_ACCESS_CONST			0x04

class OD
{
public:
    OD();
    ~OD();

    Index* index(const uint16_t &indexKey) const;
    void addIndex(Index *index);
    int indexCount() const;

    QMap<uint16_t, Index *> &indexes();

private:
    QMap<uint16_t, Index*> _indexes;
};

#endif // OD_H
