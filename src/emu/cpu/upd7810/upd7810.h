#pragma once

#ifndef __UPD7810_H__
#define __UPD7810_H__

#include "cpuintrf.h"

/*
  all types have internal ram at 0xff00-0xffff
  7810
  7811 (4kbyte),7812(8),7814(16) have internal rom at 0x0000
*/

// unfortunatly memory configuration differs with internal rom size
typedef enum
{
	TYPE_7801,
	TYPE_78C05,
	TYPE_78C06,
	TYPE_7810,
	TYPE_7810_GAMEMASTER, // a few modifications until internal rom dumped
	TYPE_7807
//  TYPE_78C10, // stop instruction added
//  TYPE_78IV,
//  TYPE_78K0,
//  TYPE_78K0S
//  millions of subtypes
} UPD7810_TYPE;

/* Supply an instance of this function in your driver code:
 * It will be called whenever an output signal changes or a new
 * input line state is to be sampled.
 */
typedef int (*upd7810_io_callback)(const device_config *device, int ioline, int state);

// use it as reset parameter in the Machine struct
typedef struct {
    UPD7810_TYPE type;
    upd7810_io_callback io_callback;
} UPD7810_CONFIG;

enum
{
	UPD7810_PC=1, UPD7810_SP, UPD7810_PSW,
	UPD7810_EA, UPD7810_V, UPD7810_A, UPD7810_VA,
	UPD7810_BC, UPD7810_B, UPD7810_C, UPD7810_DE, UPD7810_D, UPD7810_E, UPD7810_HL, UPD7810_H, UPD7810_L,
	UPD7810_EA2, UPD7810_V2, UPD7810_A2, UPD7810_VA2,
	UPD7810_BC2, UPD7810_B2, UPD7810_C2, UPD7810_DE2, UPD7810_D2, UPD7810_E2, UPD7810_HL2, UPD7810_H2, UPD7810_L2,
	UPD7810_CNT0, UPD7810_CNT1, UPD7810_TM0, UPD7810_TM1, UPD7810_ECNT, UPD7810_ECPT, UPD7810_ETM0, UPD7810_ETM1,
	UPD7810_MA, UPD7810_MB, UPD7810_MCC, UPD7810_MC, UPD7810_MM, UPD7810_MF,
	UPD7810_TMM, UPD7810_ETMM, UPD7810_EOM, UPD7810_SML, UPD7810_SMH,
	UPD7810_ANM, UPD7810_MKL, UPD7810_MKH, UPD7810_ZCM,
	UPD7810_TXB, UPD7810_RXB, UPD7810_CR0, UPD7810_CR1, UPD7810_CR2, UPD7810_CR3,
	UPD7810_TXD, UPD7810_RXD, UPD7810_SCK, UPD7810_TI, UPD7810_TO, UPD7810_CI, UPD7810_CO0, UPD7810_CO1
};

/* port numbers for PA,PB,PC,PD and PF */
enum
{
	UPD7810_PORTA, UPD7810_PORTB, UPD7810_PORTC, UPD7810_PORTD, UPD7810_PORTF
};

enum
{
	UPD7807_PORTA, UPD7807_PORTB, UPD7807_PORTC, UPD7807_PORTD, UPD7807_PORTF,
	UPD7807_PORTT
};

/* IRQ lines */
#define UPD7810_INTF1		0
#define UPD7810_INTF2		1
#define UPD7810_INTF0		2
#define UPD7810_INTFE1      4

CPU_GET_INFO( upd7810 );
CPU_GET_INFO( upd7807 );
CPU_GET_INFO( upd7801 );
CPU_GET_INFO( upd78c05 );
CPU_GET_INFO( upd78c06 );

#define CPU_UPD7810 CPU_GET_INFO_NAME( upd7810 )
#define CPU_UPD7807 CPU_GET_INFO_NAME( upd7807 )
#define CPU_UPD7801 CPU_GET_INFO_NAME( upd7801 )
#define CPU_UPD78C05 CPU_GET_INFO_NAME( upd78c05 )
#define CPU_UPD78C06 CPU_GET_INFO_NAME( upd78c06 )

CPU_DISASSEMBLE( upd7810 );
CPU_DISASSEMBLE( upd7807 );
CPU_DISASSEMBLE( upd7801 );
CPU_DISASSEMBLE( upd78c05 );

#endif /* __UPD7810_H__ */
