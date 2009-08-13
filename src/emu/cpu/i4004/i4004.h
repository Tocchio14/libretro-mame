#ifndef __I4004_H__
#define __I4004_H__

#include "cpuintrf.h"


/***************************************************************************
    CONSTANTS
***************************************************************************/

enum
{
	I4004_PC,
	I4004_A,
	I4004_R01, I4004_R23, I4004_R45, I4004_R67, I4004_R89, I4004_RAB, I4004_RCD, I4004_REF,
	I4004_ADDR1,I4004_ADDR2,I4004_ADDR3,I4004_ADDR4,I4004_RAM,
	I4004_GENPC = REG_GENPC,
	I4004_GENSP = REG_GENSP,
	I4004_GENPCBASE = REG_GENPCBASE
};

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

CPU_GET_INFO( i4004 );
#define CPU_I4004 CPU_GET_INFO_NAME( i4004 )

CPU_DISASSEMBLE( i4004 );

void i4004_set_test(const device_config *device, UINT8 val);
#endif
