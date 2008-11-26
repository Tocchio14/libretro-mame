/***************************************************************************

    TMS34010: Portable Texas Instruments TMS34010 emulator

    Copyright Alex Pasadyn/Zsolt Vasvari
    Parts based on code by Aaron Giles

***************************************************************************/

#include "debugger.h"
#include "osd_cpu.h"
#include "tms34010.h"


/***************************************************************************
    DEBUG STATE & STRUCTURES
***************************************************************************/

#define VERBOSE 			0
#define LOG_CONTROL_REGS	0
#define LOG_GRAPHICS_OPS	0

#define LOG(x)	do { if (VERBOSE) logerror x; } while (0)



/***************************************************************************
    CORE STATE
***************************************************************************/

/* TMS34010 State */
typedef struct _XY XY;
struct _XY
{
#ifdef LSB_FIRST
	INT16 x;
	INT16 y;
#else
	INT16 y;
	INT16 x;
#endif
};

typedef struct _tms34010_state tms34010_state;
struct _tms34010_state
{
	UINT32				pc;
	UINT32				st;
	void (*pixel_write)(tms34010_state *tms, offs_t offset, UINT32 data);
	UINT32 (*pixel_read)(tms34010_state *tms, offs_t offset);
	UINT32 (*raster_op)(tms34010_state *tms, UINT32 newpix, UINT32 oldpix);
	UINT32 				convsp;
	UINT32 				convdp;
	UINT32 				convmp;
	UINT16 *			shiftreg;
	INT32 				gfxcycles;
	UINT8 				pixelshift;
	UINT8 				is_34020;
	UINT8 				reset_deferred;
	UINT8 				hblank_stable;
	UINT8				external_host_access;
	UINT8				executing;
	cpu_irq_callback 	irq_callback;
	const device_config *device;
	const address_space *program;
	const tms34010_config *config;
	const device_config *screen;
	emu_timer *			scantimer;
	int					icount;

	/* A registers 0-15 map to regs[0]-regs[15] */
	/* B registers 0-15 map to regs[30]-regs[15] */
	union
	{
		INT32 reg;
		XY xy;
	} regs[31];

	/* for the 34010, we only copy 32 of these into the new state */
	UINT16 IOregs[64];
};

#include "34010ops.h"


/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

/* default configuration */
static const tms34010_config default_config =
{
	0
};

static void check_interrupt(tms34010_state *tms);
static TIMER_CALLBACK( scanline_callback );
static STATE_POSTLOAD( tms34010_state_postload );


/***************************************************************************
    MACROS
***************************************************************************/

/* status register definitions */
#define STBIT_N			(1 << 31)
#define STBIT_C			(1 << 30)
#define STBIT_Z			(1 << 29)
#define STBIT_V			(1 << 28)
#define STBIT_P			(1 << 25)
#define STBIT_IE		(1 << 21)
#define STBIT_FE1		(1 << 11)
#define STBITS_F1		(0x1f << 6)
#define STBIT_FE0		(1 << 5)
#define STBITS_F0		(0x1f << 0)

/* register definitions and shortcuts */
#define N_FLAG(T)		((T)->st & STBIT_N)
#define Z_FLAG(T)		((T)->st & STBIT_Z)
#define C_FLAG(T)		((T)->st & STBIT_C)
#define V_FLAG(T)		((T)->st & STBIT_V)
#define P_FLAG(T)		((T)->st & STBIT_P)
#define IE_FLAG(T)		((T)->st & STBIT_IE)
#define FE0_FLAG(T)		((T)->st & STBIT_FE0)
#define FE1_FLAG(T)		((T)->st & STBIT_FE1)

/* register file access */
#define AREG(T,i)		((T)->regs[i].reg)
#define AREG_XY(T,i)	((T)->regs[i].xy)
#define AREG_X(T,i)		((T)->regs[i].xy.x)
#define AREG_Y(T,i)		((T)->regs[i].xy.y)
#define BREG(T,i)		((T)->regs[30 - (i)].reg)
#define BREG_XY(T,i)	((T)->regs[30 - (i)].xy)
#define BREG_X(T,i)		((T)->regs[30 - (i)].xy.x)
#define BREG_Y(T,i)		((T)->regs[30 - (i)].xy.y)
#define SP(T)			AREG(T,15)
#define FW(T,i)			(((T)->st >> (i ? 6 : 0)) & 0x1f)
#define FWEX(T,i)		(((T)->st >> (i ? 6 : 0)) & 0x3f)

/* opcode decode helpers */
#define SRCREG(O)		(((O) >> 5) & 0x0f)
#define DSTREG(O)		((O) & 0x0f)
#define SKIP_WORD(T)	((T)->pc += (2 << 3))
#define SKIP_LONG(T)	((T)->pc += (4 << 3))
#define PARAM_K(O)		(((O) >> 5) & 0x1f)
#define PARAM_N(O)		((O) & 0x1f)
#define PARAM_REL8(O)	((INT8)(O))

/* memory I/O */
#define WFIELD0(T,a,b)	(*tms34010_wfield_functions[FW(T,0)])(T,a,b)
#define WFIELD1(T,a,b)	(*tms34010_wfield_functions[FW(T,1)])(T,a,b)
#define RFIELD0(T,a)	(*tms34010_rfield_functions[FWEX(T,0)])(T,a)
#define RFIELD1(T,a)	(*tms34010_rfield_functions[FWEX(T,1)])(T,a)
#define WPIXEL(T,a,b)	(*(T)->pixel_write)(T,a,b)
#define RPIXEL(T,a)		(*(T)->pixel_read)(T,a)

/* Implied Operands */
#define SADDR(T)		BREG(T,0)
#define SADDR_X(T)		BREG_X(T,0)
#define SADDR_Y(T)		BREG_Y(T,0)
#define SADDR_XY(T)		BREG_XY(T,0)
#define SPTCH(T)		BREG(T,1)
#define DADDR(T)		BREG(T,2)
#define DADDR_X(T)		BREG_X(T,2)
#define DADDR_Y(T)		BREG_Y(T,2)
#define DADDR_XY(T)		BREG_XY(T,2)
#define DPTCH(T)		BREG(T,3)
#define OFFSET(T)		BREG(T,4)
#define WSTART_X(T)		BREG_X(T,5)
#define WSTART_Y(T)		BREG_Y(T,5)
#define WEND_X(T)		BREG_X(T,6)
#define WEND_Y(T)		BREG_Y(T,6)
#define DYDX_X(T)		BREG_X(T,7)
#define DYDX_Y(T)		BREG_Y(T,7)
#define COLOR0(T)		BREG(T,8)
#define COLOR1(T)		BREG(T,9)
#define COUNT(T)		BREG(T,10)
#define INC1_X(T)		BREG_X(T,11)
#define INC1_Y(T)		BREG_Y(T,11)
#define INC2_X(T)		BREG_X(T,12)
#define INC2_Y(T)		BREG_Y(T,12)
#define PATTRN(T)		BREG(T,13)
#define TEMP(T)			BREG(T,14)

/* I/O registers */
#define WINDOW_CHECKING(T)	((IOREG(T, REG_CONTROL) >> 6) & 0x03)



/***************************************************************************
    INLINE SHORTCUTS
***************************************************************************/

/* Combine indiviual flags into the Status Register */
INLINE UINT32 GET_ST(tms34010_state *tms)
{
	return tms->st;
}

/* Break up Status Register into indiviual flags */
INLINE void SET_ST(tms34010_state *tms, UINT32 st)
{
	tms->st = st;
	/* interrupts might have been enabled, check it */
	check_interrupt(tms);
}

/* Intialize Status to 0x0010 */
INLINE void RESET_ST(tms34010_state *tms)
{
	SET_ST(tms, 0x00000010);
}

/* shortcuts for reading opcodes */
INLINE UINT32 ROPCODE(tms34010_state *tms)
{
	UINT32 pc = TOBYTE(tms->pc);
	tms->pc += 2 << 3;
	return memory_decrypted_read_word(tms->program, pc);
}

INLINE INT16 PARAM_WORD(tms34010_state *tms)
{
	UINT32 pc = TOBYTE(tms->pc);
	tms->pc += 2 << 3;
	return memory_raw_read_word(tms->program, pc);
}

INLINE INT32 PARAM_LONG(tms34010_state *tms)
{
	UINT32 pc = TOBYTE(tms->pc);
	tms->pc += 4 << 3;
	return (UINT16)memory_raw_read_word(tms->program, pc) | (memory_raw_read_word(tms->program, pc + 2) << 16);
}

INLINE INT16 PARAM_WORD_NO_INC(tms34010_state *tms)
{
	return memory_raw_read_word(tms->program, TOBYTE(tms->pc));
}

INLINE INT32 PARAM_LONG_NO_INC(tms34010_state *tms)
{
	UINT32 pc = TOBYTE(tms->pc);
	return (UINT16)memory_raw_read_word(tms->program, pc) | (memory_raw_read_word(tms->program, pc + 2) << 16);
}

/* read memory byte */
INLINE UINT32 RBYTE(tms34010_state *tms, offs_t offset)
{
	UINT32 ret;
	RFIELDMAC_8(tms);
	return ret;
}

/* write memory byte */
INLINE void WBYTE(tms34010_state *tms, offs_t offset, UINT32 data)
{
	WFIELDMAC_8(tms);
}

/* read memory long */
INLINE UINT32 RLONG(tms34010_state *tms, offs_t offset)
{
	RFIELDMAC_32(tms);
}

/* write memory long */
INLINE void WLONG(tms34010_state *tms, offs_t offset, UINT32 data)
{
	WFIELDMAC_32(tms);
}

/* pushes/pops a value from the stack */
INLINE void PUSH(tms34010_state *tms, UINT32 data)
{
	SP(tms) -= 0x20;
	WLONG(tms, SP(tms), data);
}

INLINE INT32 POP(tms34010_state *tms)
{
	INT32 ret = RLONG(tms, SP(tms));
	SP(tms) += 0x20;
	return ret;
}



/***************************************************************************
    PIXEL READS
***************************************************************************/

#define RP(T,m1,m2)  											\
	/* TODO: Plane masking */								\
	return (TMS34010_RDMEM_WORD(T, TOBYTE(offset & 0xfffffff0)) >> (offset & m1)) & m2;

static UINT32 read_pixel_1(tms34010_state *tms, offs_t offset) { RP(tms,0x0f,0x01) }
static UINT32 read_pixel_2(tms34010_state *tms, offs_t offset) { RP(tms,0x0e,0x03) }
static UINT32 read_pixel_4(tms34010_state *tms, offs_t offset) { RP(tms,0x0c,0x0f) }
static UINT32 read_pixel_8(tms34010_state *tms, offs_t offset) { RP(tms,0x08,0xff) }
static UINT32 read_pixel_16(tms34010_state *tms, offs_t offset)
{
	/* TODO: Plane masking */
	return TMS34010_RDMEM_WORD(tms, TOBYTE(offset & 0xfffffff0));
}
static UINT32 read_pixel_32(tms34010_state *tms, offs_t offset)
{
	/* TODO: Plane masking */
	return TMS34010_RDMEM_DWORD(tms, TOBYTE(offset & 0xffffffe0));
}

/* Shift register read */
static UINT32 read_pixel_shiftreg(tms34010_state *tms, offs_t offset)
{
	if (tms->config->to_shiftreg)
		tms->config->to_shiftreg(offset, &tms->shiftreg[0]);
	else
		fatalerror("To ShiftReg function not set. PC = %08X\n", tms->pc);
	return tms->shiftreg[0];
}



/***************************************************************************
    PIXEL WRITES
***************************************************************************/

/* No Raster Op + No Transparency */
#define WP(T,m1,m2)  																			\
	UINT32 a = TOBYTE(offset & 0xfffffff0);													\
	UINT32 pix = TMS34010_RDMEM_WORD(T,a);													\
	UINT32 shiftcount = offset & m1;														\
																							\
	/* TODO: plane masking */																\
	data &= m2;																				\
	pix = (pix & ~(m2 << shiftcount)) | (data << shiftcount);								\
	TMS34010_WRMEM_WORD(T, a, pix);															\

/* No Raster Op + Transparency */
#define WP_T(T,m1,m2)  																		\
	/* TODO: plane masking */																\
	data &= m2;																				\
	if (data)																				\
	{																						\
		UINT32 a = TOBYTE(offset & 0xfffffff0);												\
		UINT32 pix = TMS34010_RDMEM_WORD(T,a);												\
		UINT32 shiftcount = offset & m1;													\
																							\
		/* TODO: plane masking */															\
		pix = (pix & ~(m2 << shiftcount)) | (data << shiftcount);							\
		TMS34010_WRMEM_WORD(T, a, pix);														\
	}						  																\

/* Raster Op + No Transparency */
#define WP_R(T,m1,m2)  																		\
	UINT32 a = TOBYTE(offset & 0xfffffff0);													\
	UINT32 pix = TMS34010_RDMEM_WORD(T,a);													\
	UINT32 shiftcount = offset & m1;														\
																							\
	/* TODO: plane masking */																\
	data = (*(T)->raster_op)(tms, data & m2, (pix >> shiftcount) & m2) & m2;				\
	pix = (pix & ~(m2 << shiftcount)) | (data << shiftcount);								\
	TMS34010_WRMEM_WORD(T, a, pix);															\

/* Raster Op + Transparency */
#define WP_R_T(T,m1,m2)  																		\
	UINT32 a = TOBYTE(offset & 0xfffffff0);													\
	UINT32 pix = TMS34010_RDMEM_WORD(T,a);													\
	UINT32 shiftcount = offset & m1;														\
																							\
	/* TODO: plane masking */																\
	data = (*(T)->raster_op)(tms, data & m2, (pix >> shiftcount) & m2) & m2;				\
	if (data)																				\
	{																						\
		pix = (pix & ~(m2 << shiftcount)) | (data << shiftcount);							\
		TMS34010_WRMEM_WORD(T, a, pix);														\
	}						  																\


/* No Raster Op + No Transparency */
static void write_pixel_1(tms34010_state *tms, offs_t offset, UINT32 data) { WP(tms, 0x0f, 0x01); }
static void write_pixel_2(tms34010_state *tms, offs_t offset, UINT32 data) { WP(tms, 0x0e, 0x03); }
static void write_pixel_4(tms34010_state *tms, offs_t offset, UINT32 data) { WP(tms, 0x0c, 0x0f); }
static void write_pixel_8(tms34010_state *tms, offs_t offset, UINT32 data) { WP(tms, 0x08, 0xff); }
static void write_pixel_16(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	TMS34010_WRMEM_WORD(tms, TOBYTE(offset & 0xfffffff0), data);
}
static void write_pixel_32(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	TMS34010_WRMEM_WORD(tms, TOBYTE(offset & 0xffffffe0), data);
}

/* No Raster Op + Transparency */
static void write_pixel_t_1(tms34010_state *tms, offs_t offset, UINT32 data) { WP_T(tms, 0x0f, 0x01); }
static void write_pixel_t_2(tms34010_state *tms, offs_t offset, UINT32 data) { WP_T(tms, 0x0e, 0x03); }
static void write_pixel_t_4(tms34010_state *tms, offs_t offset, UINT32 data) { WP_T(tms, 0x0c, 0x0f); }
static void write_pixel_t_8(tms34010_state *tms, offs_t offset, UINT32 data) { WP_T(tms, 0x08, 0xff); }
static void write_pixel_t_16(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	if (data)
		TMS34010_WRMEM_WORD(tms, TOBYTE(offset & 0xfffffff0), data);
}
static void write_pixel_t_32(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	if (data)
		TMS34010_WRMEM_DWORD(tms, TOBYTE(offset & 0xffffffe0), data);
}

/* Raster Op + No Transparency */
static void write_pixel_r_1(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R(tms, 0x0f, 0x01); }
static void write_pixel_r_2(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R(tms, 0x0e, 0x03); }
static void write_pixel_r_4(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R(tms, 0x0c, 0x0f); }
static void write_pixel_r_8(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R(tms, 0x08, 0xff); }
static void write_pixel_r_16(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	UINT32 a = TOBYTE(offset & 0xfffffff0);
	TMS34010_WRMEM_WORD(tms, a, (*tms->raster_op)(tms, data, TMS34010_RDMEM_WORD(tms, a)));
}
static void write_pixel_r_32(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	UINT32 a = TOBYTE(offset & 0xffffffe0);
	TMS34010_WRMEM_DWORD(tms, a, (*tms->raster_op)(tms, data, TMS34010_RDMEM_DWORD(tms, a)));
}

/* Raster Op + Transparency */
static void write_pixel_r_t_1(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R_T(tms, 0x0f,0x01); }
static void write_pixel_r_t_2(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R_T(tms, 0x0e,0x03); }
static void write_pixel_r_t_4(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R_T(tms, 0x0c,0x0f); }
static void write_pixel_r_t_8(tms34010_state *tms, offs_t offset, UINT32 data) { WP_R_T(tms, 0x08,0xff); }
static void write_pixel_r_t_16(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	UINT32 a = TOBYTE(offset & 0xfffffff0);
	data = (*tms->raster_op)(tms, data, TMS34010_RDMEM_WORD(tms, a));

	if (data)
		TMS34010_WRMEM_WORD(tms, a, data);
}
static void write_pixel_r_t_32(tms34010_state *tms, offs_t offset, UINT32 data)
{
	/* TODO: plane masking */
	UINT32 a = TOBYTE(offset & 0xffffffe0);
	data = (*tms->raster_op)(tms, data, TMS34010_RDMEM_DWORD(tms, a));

	if (data)
		TMS34010_WRMEM_DWORD(tms, a, data);
}

/* Shift register write */
static void write_pixel_shiftreg(tms34010_state *tms, offs_t offset, UINT32 data)
{
	if (tms->config->from_shiftreg)
		tms->config->from_shiftreg(offset, &tms->shiftreg[0]);
	else
		fatalerror("From ShiftReg function not set. PC = %08X\n", tms->pc);
}



/***************************************************************************
    RASTER OPS
***************************************************************************/

/* Raster operations */
static UINT32 raster_op_1(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return newpix & oldpix; }
static UINT32 raster_op_2(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return newpix & ~oldpix; }
static UINT32 raster_op_3(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return 0; }
static UINT32 raster_op_4(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return newpix | ~oldpix; }
static UINT32 raster_op_5(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return ~(newpix ^ oldpix); }
static UINT32 raster_op_6(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return ~oldpix; }
static UINT32 raster_op_7(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return ~(newpix | oldpix); }
static UINT32 raster_op_8(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return newpix | oldpix; }
static UINT32 raster_op_9(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)  { return oldpix; }
static UINT32 raster_op_10(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return newpix ^ oldpix; }
static UINT32 raster_op_11(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return ~newpix & oldpix; }
static UINT32 raster_op_12(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return 0xffff; }
static UINT32 raster_op_13(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return ~newpix | oldpix; }
static UINT32 raster_op_14(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return ~(newpix & oldpix); }
static UINT32 raster_op_15(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return ~newpix; }
static UINT32 raster_op_16(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return newpix + oldpix; }
static UINT32 raster_op_17(tms34010_state *tms, UINT32 newpix, UINT32 oldpix)
{
	UINT32 max = (UINT32)0xffffffff >> (32 - IOREG(tms, REG_PSIZE));
	UINT32 res = newpix + oldpix;
	return (res > max) ? max : res;
}
static UINT32 raster_op_18(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return oldpix - newpix; }
static UINT32 raster_op_19(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return (oldpix > newpix) ? oldpix - newpix : 0; }
static UINT32 raster_op_20(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return (oldpix > newpix) ? oldpix : newpix; }
static UINT32 raster_op_21(tms34010_state *tms, UINT32 newpix, UINT32 oldpix) { return (oldpix > newpix) ? newpix : oldpix; }



/***************************************************************************
    OPCODE TABLE & IMPLEMENTATIONS
***************************************************************************/

#include "34010fld.c"

/* includes the static function prototypes and the master opcode table */
#include "34010tbl.c"

/* includes the actual opcode implementations */
#include "34010ops.c"
#include "34010gfx.c"



/***************************************************************************
    Internal interrupt check
****************************************************************************/

/* Generate pending interrupts. */
static void check_interrupt(tms34010_state *tms)
{
	int vector = 0;
	int irqline = -1;
	int irq;

	/* if we're not actively executing, skip it */
	if (!tms->executing)
		return;

	/* check for NMI first */
	if (IOREG(tms, REG_HSTCTLH) & 0x0100)
	{
		LOG(("TMS34010 '%s' takes NMI\n", tms->device->tag));

		/* ack the NMI */
		IOREG(tms, REG_HSTCTLH) &= ~0x0100;

		/* handle NMI mode bit */
		if (!(IOREG(tms, REG_HSTCTLH) & 0x0200))
		{
			PUSH(tms, tms->pc);
			PUSH(tms, GET_ST(tms));
		}

		/* leap to the vector */
		RESET_ST(tms);
		tms->pc = RLONG(tms, 0xfffffee0);
		COUNT_CYCLES(tms,16);
		return;
	}

	/* early out if everything else is disabled */
	irq = IOREG(tms, REG_INTPEND) & IOREG(tms, REG_INTENB);
	if (!IE_FLAG(tms) || !irq)
		return;

	/* host interrupt */
	if (irq & TMS34010_HI)
	{
		LOG(("TMS34010 '%s' takes HI\n", tms->device->tag));
		vector = 0xfffffec0;
	}

	/* display interrupt */
	else if (irq & TMS34010_DI)
	{
		LOG(("TMS34010 '%s' takes DI\n", tms->device->tag));
		vector = 0xfffffea0;
	}

	/* window violation interrupt */
	else if (irq & TMS34010_WV)
	{
		LOG(("TMS34010 '%s' takes WV\n", tms->device->tag));
		vector = 0xfffffe80;
	}

	/* external 1 interrupt */
	else if (irq & TMS34010_INT1)
	{
		LOG(("TMS34010 '%s' takes INT1\n", tms->device->tag));
		vector = 0xffffffc0;
		irqline = 0;
	}

	/* external 2 interrupt */
	else if (irq & TMS34010_INT2)
	{
		LOG(("TMS34010 '%s' takes INT2\n", tms->device->tag));
		vector = 0xffffffa0;
		irqline = 1;
	}

	/* if we took something, generate it */
	if (vector)
	{
		PUSH(tms, tms->pc);
		PUSH(tms, GET_ST(tms));
		RESET_ST(tms);
		tms->pc = RLONG(tms, vector);
		COUNT_CYCLES(tms,16);

		/* call the callback for externals */
		if (irqline >= 0)
			(void)(*tms->irq_callback)(tms->device, irqline);
	}
}



/***************************************************************************
    Reset the CPU emulation
***************************************************************************/

static CPU_INIT( tms34010 )
{
	const tms34010_config *configdata = device->static_config ? device->static_config : &default_config;
	tms34010_state *tms = device->token;

	tms->external_host_access = FALSE;

	tms->config = configdata;
	tms->irq_callback = irqcallback;
	tms->device = device;
	tms->program = memory_find_address_space(device, ADDRESS_SPACE_PROGRAM);
	tms->screen = device_list_find_by_tag(device->machine->config->devicelist, VIDEO_SCREEN, configdata->screen_tag);

	/* allocate a scanline timer and set it to go off at the start */
	tms->scantimer = timer_alloc(device->machine, scanline_callback, tms);
	timer_adjust_oneshot(tms->scantimer, attotime_zero, index);

	/* allocate the shiftreg */
	tms->shiftreg = auto_malloc(SHIFTREG_SIZE);

	state_save_register_item("tms34010", device->tag, 0, tms->pc);
	state_save_register_item("tms34010", device->tag, 0, tms->st);
	state_save_register_item("tms34010", device->tag, 0, tms->reset_deferred);
	state_save_register_item_pointer("tms34010", device->tag, 0, tms->shiftreg, SHIFTREG_SIZE / 2);
	state_save_register_item_array("tms34010", device->tag, 0, tms->IOregs);
	state_save_register_item("tms34010", device->tag, 0, tms->convsp);
	state_save_register_item("tms34010", device->tag, 0, tms->convdp);
	state_save_register_item("tms34010", device->tag, 0, tms->convmp);
	state_save_register_item("tms34010", device->tag, 0, tms->pixelshift);
	state_save_register_item("tms34010", device->tag, 0, tms->gfxcycles);
	state_save_register_item_pointer("tms34010", device->tag, 0, (&tms->regs[0].reg), ARRAY_LENGTH(tms->regs));
	state_save_register_postload(device->machine, tms34010_state_postload, tms);
}

static CPU_RESET( tms34010 )
{
	/* zap the state and copy in the config pointer */
	tms34010_state *tms = device->token;
	const tms34010_config *config = tms->config;
	const device_config *screen = tms->screen;
	UINT16 *shiftreg = tms->shiftreg;
	cpu_irq_callback save_irqcallback = tms->irq_callback;
	emu_timer *save_scantimer = tms->scantimer;

	memset(tms, 0, sizeof(*tms));

	tms->config = config;
	tms->screen = screen;
	tms->shiftreg = shiftreg;
	tms->irq_callback = save_irqcallback;
	tms->scantimer = save_scantimer;
	tms->device = device;
	tms->program = memory_find_address_space(device, ADDRESS_SPACE_PROGRAM);

	/* fetch the initial PC and reset the state */
	tms->pc = RLONG(tms, 0xffffffe0) & 0xfffffff0;
	RESET_ST(tms);

	/* HALT the CPU if requested, and remember to re-read the starting PC */
	/* the first time we are run */
	tms->reset_deferred = tms->config->halt_on_reset;
	if (tms->config->halt_on_reset)
		tms34010_io_register_w(cpu_get_address_space(device, ADDRESS_SPACE_PROGRAM), REG_HSTCTLH, 0x8000, 0xffff);
}


static CPU_RESET( tms34020 )
{
	tms34010_state *tms = device->token;
	CPU_RESET_CALL(tms34010);
	tms->is_34020 = 1;
}



/***************************************************************************
    Shut down the CPU emulation
***************************************************************************/

static CPU_EXIT( tms34010 )
{
	tms34010_state *tms = device->token;
	tms->shiftreg = NULL;
}



/***************************************************************************
    Get all registers in given buffer
***************************************************************************/

static CPU_GET_CONTEXT( tms34010 )
{
}


static CPU_GET_CONTEXT( tms34020 )
{
}



/***************************************************************************
    Set all registers to given values
***************************************************************************/

static CPU_SET_CONTEXT( tms34010 )
{
}


static CPU_SET_CONTEXT( tms34020 )
{
}



/***************************************************************************
    Set IRQ line state
***************************************************************************/

static void set_irq_line(tms34010_state *tms, int irqline, int linestate)
{
	LOG(("TMS34010 '%s' set irq line %d state %d\n", tms->device->tag, irqline, linestate));

	/* set the pending interrupt */
	switch (irqline)
	{
		case 0:
			if (linestate != CLEAR_LINE)
				IOREG(tms, REG_INTPEND) |= TMS34010_INT1;
			else
				IOREG(tms, REG_INTPEND) &= ~TMS34010_INT1;
			break;

		case 1:
			if (linestate != CLEAR_LINE)
				IOREG(tms, REG_INTPEND) |= TMS34010_INT2;
			else
				IOREG(tms, REG_INTPEND) &= ~TMS34010_INT2;
			break;
	}
}



/***************************************************************************
    Generate internal interrupt
***************************************************************************/

static TIMER_CALLBACK( internal_interrupt_callback )
{
	tms34010_state *tms = ptr;
	int type = param;
	int cpunum;

	/* call through to the CPU to generate the int */
	IOREG(tms, REG_INTPEND) |= type;
	LOG(("TMS34010 '%s' set internal interrupt $%04x\n", tms->device->tag, type));

	/* generate triggers so that spin loops can key off them */
	for (cpunum = 0; cpunum < ARRAY_LENGTH(machine->cpu); cpunum++)
		if (machine->cpu[cpunum] == tms->device)
		{
			cpu_triggerint(machine->cpu[cpunum]);
			break;
		}
}



/***************************************************************************
    Execute
***************************************************************************/

static CPU_EXECUTE( tms34010 )
{
	tms34010_state *tms = device->token;

	/* Get out if CPU is halted. Absolutely no interrupts must be taken!!! */
	if (IOREG(tms, REG_HSTCTLH) & 0x8000)
		return cycles;

	/* if the CPU's reset was deferred, do it now */
	if (tms->reset_deferred)
	{
		tms->reset_deferred = 0;
		tms->pc = RLONG(tms, 0xffffffe0);
	}

	/* execute starting now */
	tms->icount = cycles;

	/* check interrupts first */
	tms->executing = TRUE;
	check_interrupt(tms);
	if ((tms->device->machine->debug_flags & DEBUG_FLAG_ENABLED) == 0)
	{
		do
		{
			UINT16 op = ROPCODE(tms);
			(*opcode_table[op >> 4])(tms, op);
		} while (tms->icount > 0);
	}
	else
	{
		do
		{
			UINT16 op;
			if ((tms->device->machine->debug_flags & DEBUG_FLAG_CALL_HOOK) != 0)
			{
				tms->st = GET_ST(tms);
				debugger_instruction_hook(tms->device, tms->pc);
			}
			op = ROPCODE(tms);
			(*opcode_table[op >> 4])(tms, op);
		} while (tms->icount > 0);
	}
	tms->executing = FALSE;

	return cycles - tms->icount;
}



/***************************************************************************
    PIXEL OPS
***************************************************************************/

static void (*const pixel_write_ops[4][6])(tms34010_state *tms, offs_t offset, UINT32 data) =
{
	{ write_pixel_1,     write_pixel_2,     write_pixel_4,     write_pixel_8,     write_pixel_16,     write_pixel_32     },
	{ write_pixel_r_1,   write_pixel_r_2,   write_pixel_r_4,   write_pixel_r_8,   write_pixel_r_16,   write_pixel_r_32   },
	{ write_pixel_t_1,   write_pixel_t_2,   write_pixel_t_4,   write_pixel_t_8,   write_pixel_t_16,	  write_pixel_t_32   },
	{ write_pixel_r_t_1, write_pixel_r_t_2, write_pixel_r_t_4, write_pixel_r_t_8, write_pixel_r_t_16, write_pixel_r_t_32 }
};

static UINT32 (*const pixel_read_ops[6])(tms34010_state *tms, offs_t offset) =
{
	read_pixel_1,        read_pixel_2,      read_pixel_4,      read_pixel_8,      read_pixel_16,      read_pixel_32
};


static void set_pixel_function(tms34010_state *tms)
{
	UINT32 i1,i2;

	if (IOREG(tms, REG_DPYCTL) & 0x0800)
	{
		/* Shift Register Transfer */
		tms->pixel_write = write_pixel_shiftreg;
		tms->pixel_read  = read_pixel_shiftreg;
		return;
	}

	switch (IOREG(tms, REG_PSIZE))
	{
		default:
		case 0x01: i2 = 0; break;
		case 0x02: i2 = 1; break;
		case 0x04: i2 = 2; break;
		case 0x08: i2 = 3; break;
		case 0x10: i2 = 4; break;
		case 0x20: i2 = 5; break;
	}

	if (IOREG(tms, REG_CONTROL) & 0x20)
		i1 = tms->raster_op ? 3 : 2;
	else
		i1 = tms->raster_op ? 1 : 0;

	tms->pixel_write = pixel_write_ops[i1][i2];
	tms->pixel_read  = pixel_read_ops [i2];
}



/***************************************************************************
    RASTER OPS
***************************************************************************/

static UINT32 (*const raster_ops[32]) (tms34010_state *tms, UINT32 newpix, UINT32 oldpix) =
{
	           0, raster_op_1 , raster_op_2 , raster_op_3,
	raster_op_4 , raster_op_5 , raster_op_6 , raster_op_7,
	raster_op_8 , raster_op_9 , raster_op_10, raster_op_11,
	raster_op_12, raster_op_13, raster_op_14, raster_op_15,
	raster_op_16, raster_op_17, raster_op_18, raster_op_19,
	raster_op_20, raster_op_21,            0,            0,
	           0,            0,            0,            0,
	           0,            0,            0,            0,
};


static void set_raster_op(tms34010_state *tms)
{
	tms->raster_op = raster_ops[(IOREG(tms, REG_CONTROL) >> 10) & 0x1f];
}



/***************************************************************************
    VIDEO TIMING HELPERS
***************************************************************************/

static TIMER_CALLBACK( scanline_callback )
{
	tms34010_state *tms = ptr;
	const rectangle *current_visarea;
	int vsblnk, veblnk, vtotal;
	int vcount = param >> 8;
	int cpunum = param & 0xff;
	int enabled;
	int master;

	/* set the CPU context */
	cpu_push_context(tms->device);

	/* fetch the core timing parameters */
	current_visarea = video_screen_get_visible_area(tms->screen);
	enabled = SMART_IOREG(tms, DPYCTL) & 0x8000;
	master = (tms->is_34020 || (SMART_IOREG(tms, DPYCTL) & 0x2000));
	vsblnk = SMART_IOREG(tms, VSBLNK);
	veblnk = SMART_IOREG(tms, VEBLNK);
	vtotal = SMART_IOREG(tms, VTOTAL);
	if (!master)
	{
		vtotal = MIN(video_screen_get_height(tms->screen) - 1, vtotal);
		vcount = video_screen_get_vpos(tms->screen);
	}

	/* update the VCOUNT */
	SMART_IOREG(tms, VCOUNT) = vcount;

	/* if we match the display interrupt scanline, signal an interrupt */
	if (enabled && vcount == SMART_IOREG(tms, DPYINT))
	{
		/* generate the display interrupt signal */
		internal_interrupt_callback(machine, tms, TMS34010_DI);
	}

	/* at the start of VBLANK, load the starting display address */
	if (vcount == vsblnk)
	{
		/* 34010 loads DPYADR with DPYSTRT, and inverts if the origin is 0 */
		if (!tms->is_34020)
		{
			IOREG(tms, REG_DPYADR) = IOREG(tms, REG_DPYSTRT);
			LOG(("Start of VBLANK, DPYADR = %04X\n", IOREG(tms, REG_DPYADR)));
		}

		/* 34020 loads DPYNXx with DPYSTx */
		else
		{
			IOREG(tms, REG020_DPYNXL) = IOREG(tms, REG020_DPYSTL) & 0xffe0;
			IOREG(tms, REG020_DPYNXH) = IOREG(tms, REG020_DPYSTH);
		}
	}

	/* at the end of the screen, update the display parameters */
	if (vcount == vtotal)
	{
		/* only do this if we have an incoming pixel clock */
		/* also, only do it if the HEBLNK/HSBLNK values are stable */
		if (master && tms->config->scanline_callback != NULL)
		{
			int htotal = SMART_IOREG(tms, HTOTAL);
			if (htotal > 0 && vtotal > 0)
			{
				attoseconds_t refresh = HZ_TO_ATTOSECONDS(tms->config->pixclock) * (htotal + 1) * (vtotal + 1);
				int width = (htotal + 1) * tms->config->pixperclock;
				int height = vtotal + 1;
				rectangle visarea;

				/* extract the visible area */
				visarea.min_x = SMART_IOREG(tms, HEBLNK) * tms->config->pixperclock;
				visarea.max_x = SMART_IOREG(tms, HSBLNK) * tms->config->pixperclock - 1;
				visarea.min_y = veblnk;
				visarea.max_y = vsblnk - 1;

				/* if everything looks good, set the info */
				if (visarea.min_x < visarea.max_x && visarea.max_x <= width && visarea.min_y < visarea.max_y && visarea.max_y <= height)
				{
					/* because many games play with the HEBLNK/HSBLNK for effects, we don't change
                       if they are the only thing that has changed, unless they are stable for a couple
                       of frames */
					int current_width  = video_screen_get_width(tms->screen);
					int current_height = video_screen_get_height(tms->screen);

					if (width != current_width || height != current_height || visarea.min_y != current_visarea->min_y || visarea.max_y != current_visarea->max_y ||
						(tms->hblank_stable > 2 && (visarea.min_x != current_visarea->min_x || visarea.max_x != current_visarea->max_x)))
					{
						video_screen_configure(tms->screen, width, height, &visarea, refresh);
					}
					tms->hblank_stable++;
				}

				LOG(("Configuring screen: HTOTAL=%3d BLANK=%3d-%3d VTOTAL=%3d BLANK=%3d-%3d refresh=%f\n",
						htotal, SMART_IOREG(tms, HEBLNK), SMART_IOREG(tms, HSBLNK), vtotal, veblnk, vsblnk, ATTOSECONDS_TO_HZ(refresh)));

				/* interlaced timing not supported */
				if ((SMART_IOREG(tms, DPYCTL) & 0x4000) == 0)
					fatalerror("Interlaced video configured on the TMS34010 (unsupported)");
			}
		}
	}

	/* force a partial update within the visible area */
	if (vcount >= current_visarea->min_y && vcount <= current_visarea->max_y && tms->config->scanline_callback != NULL)
		video_screen_update_partial(tms->screen, vcount);

	/* if we are in the visible area, increment DPYADR by DUDATE */
	if (vcount >= veblnk && vcount < vsblnk)
	{
		/* 34010 increments by the DUDATE field in DPYCTL */
		if (!tms->is_34020)
		{
			UINT16 dpyadr = IOREG(tms, REG_DPYADR);
			if ((dpyadr & 3) == 0)
				dpyadr = ((dpyadr & 0xfffc) - (IOREG(tms, REG_DPYCTL) & 0x03fc)) | (IOREG(tms, REG_DPYSTRT) & 0x0003);
			else
				dpyadr = (dpyadr & 0xfffc) | ((dpyadr - 1) & 3);
			IOREG(tms, REG_DPYADR) = dpyadr;
		}

		/* 34020 updates based on the DINC register, including zoom */
		else
		{
			UINT32 dpynx = IOREG(tms, REG020_DPYNXL) | (IOREG(tms, REG020_DPYNXH) << 16);
			UINT32 dinc = IOREG(tms, REG020_DINCL) | (IOREG(tms, REG020_DINCH) << 16);
			dpynx = (dpynx & 0xffffffe0) | ((dpynx + dinc) & 0x1f);
			if ((dpynx & 0x1f) == 0)
				dpynx += dinc & 0xffffffe0;
			IOREG(tms, REG020_DPYNXL) = dpynx;
			IOREG(tms, REG020_DPYNXH) = dpynx >> 16;
		}
	}

	/* adjust for the next callback */
	vcount++;
	if (vcount > vtotal)
		vcount = 0;

	/* note that we add !master (0 or 1) as a attoseconds value; this makes no practical difference */
	/* but helps ensure that masters are updated first before slaves */
	timer_adjust_oneshot(tms->scantimer, attotime_add_attoseconds(video_screen_get_time_until_pos(tms->screen, vcount, 0), !master), cpunum | (vcount << 8));

	/* restore the context */
	cpu_pop_context();
}


void tms34010_get_display_params(const device_config *cpu, tms34010_display_params *params)
{
	tms34010_state *tms = cpu->token;

	params->enabled = ((SMART_IOREG(tms, DPYCTL) & 0x8000) != 0);
	params->vcount = SMART_IOREG(tms, VCOUNT);
	params->veblnk = SMART_IOREG(tms, VEBLNK);
	params->vsblnk = SMART_IOREG(tms, VSBLNK);
	params->heblnk = SMART_IOREG(tms, HEBLNK) * tms->config->pixperclock;
	params->hsblnk = SMART_IOREG(tms, HSBLNK) * tms->config->pixperclock;

	/* 34010 gets its address from DPYADR and DPYTAP */
	if (!tms->is_34020)
	{
		UINT16 dpyadr = IOREG(tms, REG_DPYADR);
		if (!(IOREG(tms, REG_DPYCTL) & 0x0400))
			dpyadr ^= 0xfffc;
		params->rowaddr = dpyadr >> 4;
		params->coladdr = ((dpyadr & 0x007c) << 4) | (IOREG(tms, REG_DPYTAP) & 0x3fff);
		params->yoffset = (IOREG(tms, REG_DPYSTRT) - IOREG(tms, REG_DPYADR)) & 3;
	}

	/* 34020 gets its address from DPYNX */
	else
	{
		params->rowaddr = IOREG(tms, REG020_DPYNXH);
		params->coladdr = IOREG(tms, REG020_DPYNXL) & 0xffe0;
		params->yoffset = 0;
		if ((IOREG(tms, REG020_DINCL) & 0x1f) != 0)
			params->yoffset = (IOREG(tms, REG020_DPYNXL) & 0x1f) / (IOREG(tms, REG020_DINCL) & 0x1f);
	}
}


VIDEO_UPDATE( tms340x0 )
{
	pen_t blackpen = get_black_pen(screen->machine);
	tms34010_display_params params;
	tms34010_state *tms = NULL;
	int cpunum = -1;
	int x;

	/* find the owning CPU */
	for (cpunum = 0; cpunum < ARRAY_LENGTH(screen->machine->cpu); cpunum++)
	{
		const device_config *cpudevice = screen->machine->cpu[cpunum];
		if (cpudevice != NULL && (cpudevice->type == (device_type)CPU_TMS34010 || cpudevice->type == (device_type)CPU_TMS34020))
		{
			tms = cpudevice->token;
			if (tms->config != NULL && tms->config->scanline_callback != NULL && tms->screen == screen)
				break;
			tms = NULL;
		}
	}
	if (tms == NULL)
		fatalerror("Unable to locate matching CPU for screen '%s'\n", screen->tag);

	/* get the display parameters for the screen */
	tms34010_get_display_params(tms->device, &params);

	/* if the display is enabled, call the scanline callback */
	if (params.enabled)
	{
		/* call through to the callback */
		LOG(("  Update: scan=%3d ROW=%04X COL=%04X\n", cliprect->min_y, params.rowaddr, params.coladdr));
		(*tms->config->scanline_callback)(screen, bitmap, cliprect->min_y, &params);
	}

	/* otherwise, just blank the current scanline */
	else
		params.heblnk = params.hsblnk = cliprect->max_x + 1;

	/* blank out the blank regions */
	if (bitmap->bpp == 16)
	{
		UINT16 *dest = BITMAP_ADDR16(bitmap, cliprect->min_y, 0);
		for (x = cliprect->min_x; x < params.heblnk; x++)
			dest[x] = blackpen;
		for (x = params.hsblnk; x <= cliprect->max_y; x++)
			dest[x] = blackpen;
	}
	else if (bitmap->bpp == 32)
	{
		UINT32 *dest = BITMAP_ADDR32(bitmap, cliprect->min_y, 0);
		for (x = cliprect->min_x; x < params.heblnk; x++)
			dest[x] = blackpen;
		for (x = params.hsblnk; x <= cliprect->max_y; x++)
			dest[x] = blackpen;
	}
	return 0;
}


/***************************************************************************
    I/O REGISTER WRITES
***************************************************************************/

static const char *const ioreg_name[] =
{
	"HESYNC", "HEBLNK", "HSBLNK", "HTOTAL",
	"VESYNC", "VEBLNK", "VSBLNK", "VTOTAL",
	"DPYCTL", "DPYSTART", "DPYINT", "CONTROL",
	"HSTDATA", "HSTADRL", "HSTADRH", "HSTCTLL",

	"HSTCTLH", "INTENB", "INTPEND", "CONVSP",
	"CONVDP", "PSIZE", "PMASK", "RESERVED",
	"RESERVED", "RESERVED", "RESERVED", "DPYTAP",
	"HCOUNT", "VCOUNT", "DPYADR", "REFCNT"
};

WRITE16_HANDLER( tms34010_io_register_w )
{
	tms34010_state *tms = space->cpu->token;
	int oldreg, newreg;

	/* Set register */
	oldreg = IOREG(tms, offset);
	IOREG(tms, offset) = data;

	switch (offset)
	{
		case REG_CONTROL:
			set_raster_op(tms);
			set_pixel_function(tms);
			break;

		case REG_PSIZE:
			set_pixel_function(tms);

			switch (data)
			{
				default:
				case 0x01: tms->pixelshift = 0; break;
				case 0x02: tms->pixelshift = 1; break;
				case 0x04: tms->pixelshift = 2; break;
				case 0x08: tms->pixelshift = 3; break;
				case 0x10: tms->pixelshift = 4; break;
			}
			break;

		case REG_PMASK:
			if (data) logerror("Plane masking not supported. PC=%08X\n", cpu_get_pc(space->cpu));
			break;

		case REG_DPYCTL:
			set_pixel_function(tms);
			break;

		case REG_HSTCTLH:
			/* if the CPU is halting itself, stop execution right away */
			if ((data & 0x8000) && !tms->external_host_access)
				tms->icount = 0;
			cputag_set_input_line(tms->device->machine, tms->device->tag, INPUT_LINE_HALT, (data & 0x8000) ? ASSERT_LINE : CLEAR_LINE);

			/* NMI issued? */
			if (data & 0x0100)
				timer_call_after_resynch(tms->device->machine, tms, 0, internal_interrupt_callback);
			break;

		case REG_HSTCTLL:
			/* the TMS34010 can change MSGOUT, can set INTOUT, and can clear INTIN */
			if (!tms->external_host_access)
			{
				newreg = (oldreg & 0xff8f) | (data & 0x0070);
				newreg |= data & 0x0080;
				newreg &= data | ~0x0008;
			}

			/* the host can change MSGIN, can set INTIN, and can clear INTOUT */
			else
			{
				newreg = (oldreg & 0xfff8) | (data & 0x0007);
				newreg &= data | ~0x0080;
				newreg |= data & 0x0008;
			}
			IOREG(tms, offset) = newreg;

			/* the TMS34010 can set output interrupt? */
			if (!(oldreg & 0x0080) && (newreg & 0x0080))
			{
				if (tms->config->output_int)
					(*tms->config->output_int)(1);
			}
			else if ((oldreg & 0x0080) && !(newreg & 0x0080))
			{
				if (tms->config->output_int)
					(*tms->config->output_int)(0);
			}

			/* input interrupt? (should really be state-based, but the functions don't exist!) */
			if (!(oldreg & 0x0008) && (newreg & 0x0008))
				timer_call_after_resynch(tms->device->machine, tms, TMS34010_HI, internal_interrupt_callback);
			else if ((oldreg & 0x0008) && !(newreg & 0x0008))
				IOREG(tms, REG_INTPEND) &= ~TMS34010_HI;
			break;

		case REG_CONVSP:
			tms->convsp = 1 << (~data & 0x1f);
			break;

		case REG_CONVDP:
			tms->convdp = 1 << (~data & 0x1f);
			break;

		case REG_INTENB:
			check_interrupt(tms);
			break;

		case REG_INTPEND:
			/* X1P, X2P and HIP are read-only */
			/* WVP and DIP can only have 0's written to them */
			IOREG(tms, REG_INTPEND) = oldreg;
			if (!(data & TMS34010_WV))
				IOREG(tms, REG_INTPEND) &= ~TMS34010_WV;
			if (!(data & TMS34010_DI))
				IOREG(tms, REG_INTPEND) &= ~TMS34010_DI;
			break;

		case REG_HEBLNK:
		case REG_HSBLNK:
			if (oldreg != data)
				tms->hblank_stable = 0;
			break;
	}

//  if (LOG_CONTROL_REGS)
//      logerror("CPU#%d@%08X: %s = %04X (%d)\n", cpunum, cpu_get_pc(space->cpu), ioreg_name[offset], IOREG(tms, offset), video_screen_get_vpos(tms->screen));
}


static const char *const ioreg020_name[] =
{
	"VESYNC", "HESYNC", "VEBLNK", "HEBLNK",
	"VSBLNK", "HSBLNK", "VTOTAL", "HTOTAL",
	"DPYCTL", "DPYSTRT", "DPYINT", "CONTROL",
	"HSTDATA", "HSTADRL", "HSTADRH", "HSTCTLL",

	"HSTCTLH", "INTENB", "INTPEND", "CONVSP",
	"CONVDP", "PSIZE", "PMASKL", "PMASKH",
	"CONVMP", "CONTROL2", "CONFIG", "DPYTAP",
	"VCOUNT", "HCOUNT", "DPYADR", "REFADR",

	"DPYSTL", "DPYSTH", "DPYNXL", "DPYNXH",
	"DINCL", "DINCH", "RES0", "HESERR",
	"RES1", "RES2", "RES3", "RES4",
	"SCOUNT", "BSFLTST", "DPYMSK", "RES5",

	"SETVCNT", "SETHCNT", "BSFLTDL", "BSFLTDH",
	"RES6", "RES7", "RES8", "RES9",
	"IHOST1L", "IHOST1H", "IHOST2L", "IHOST2H",
	"IHOST3L", "IHOST3H", "IHOST4L", "IHOST4H"
};

WRITE16_HANDLER( tms34020_io_register_w )
{
	tms34010_state *tms = space->cpu->token;
	int oldreg, newreg;

	/* Set register */
	oldreg = IOREG(tms, offset);
	IOREG(tms, offset) = data;

//  if (LOG_CONTROL_REGS)
//      logerror("CPU#%d@%08X: %s = %04X (%d)\n", cpunum, cpu_get_pc(space->cpu), ioreg020_name[offset], IOREG(tms, offset), video_screen_get_vpos(tms->screen));

	switch (offset)
	{
		case REG020_CONTROL:
		case REG020_CONTROL2:
			IOREG(tms, REG020_CONTROL) = data;
			IOREG(tms, REG020_CONTROL2) = data;
			set_raster_op(tms);
			set_pixel_function(tms);
			break;

		case REG020_PSIZE:
			set_pixel_function(tms);

			switch (data)
			{
				default:
				case 0x01: tms->pixelshift = 0; break;
				case 0x02: tms->pixelshift = 1; break;
				case 0x04: tms->pixelshift = 2; break;
				case 0x08: tms->pixelshift = 3; break;
				case 0x10: tms->pixelshift = 4; break;
				case 0x20: tms->pixelshift = 5; break;
			}
			break;

		case REG020_PMASKL:
		case REG020_PMASKH:
			if (data) logerror("Plane masking not supported. PC=%08X\n", cpu_get_pc(space->cpu));
			break;

		case REG020_DPYCTL:
			set_pixel_function(tms);
			break;

		case REG020_HSTCTLH:
			/* if the CPU is halting itself, stop execution right away */
			if ((data & 0x8000) && !tms->external_host_access)
				tms->icount = 0;
			cputag_set_input_line(tms->device->machine, tms->device->tag, INPUT_LINE_HALT, (data & 0x8000) ? ASSERT_LINE : CLEAR_LINE);

			/* NMI issued? */
			if (data & 0x0100)
				timer_call_after_resynch(tms->device->machine, tms, 0, internal_interrupt_callback);
			break;

		case REG020_HSTCTLL:
			/* the TMS34010 can change MSGOUT, can set INTOUT, and can clear INTIN */
			if (!tms->external_host_access)
			{
				newreg = (oldreg & 0xff8f) | (data & 0x0070);
				newreg |= data & 0x0080;
				newreg &= data | ~0x0008;
			}

			/* the host can change MSGIN, can set INTIN, and can clear INTOUT */
			else
			{
				newreg = (oldreg & 0xfff8) | (data & 0x0007);
				newreg &= data | ~0x0080;
				newreg |= data & 0x0008;
			}
			IOREG(tms, offset) = newreg;

			/* the TMS34010 can set output interrupt? */
			if (!(oldreg & 0x0080) && (newreg & 0x0080))
			{
				if (tms->config->output_int)
					(*tms->config->output_int)(1);
			}
			else if ((oldreg & 0x0080) && !(newreg & 0x0080))
			{
				if (tms->config->output_int)
					(*tms->config->output_int)(0);
			}

			/* input interrupt? (should really be state-based, but the functions don't exist!) */
			if (!(oldreg & 0x0008) && (newreg & 0x0008))
				timer_call_after_resynch(tms->device->machine, tms, TMS34010_HI, internal_interrupt_callback);
			else if ((oldreg & 0x0008) && !(newreg & 0x0008))
				IOREG(tms, REG020_INTPEND) &= ~TMS34010_HI;
			break;

		case REG020_INTENB:
			check_interrupt(tms);
			break;

		case REG020_INTPEND:
			/* X1P, X2P and HIP are read-only */
			/* WVP and DIP can only have 0's written to them */
			IOREG(tms, REG020_INTPEND) = oldreg;
			if (!(data & TMS34010_WV))
				IOREG(tms, REG020_INTPEND) &= ~TMS34010_WV;
			if (!(data & TMS34010_DI))
				IOREG(tms, REG020_INTPEND) &= ~TMS34010_DI;
			break;

		case REG020_CONVSP:
			if (data & 0x001f)
			{
				if (data & 0x1f00)
					tms->convsp = (1 << (~data & 0x1f)) + (1 << (~(data >> 8) & 0x1f));
				else
					tms->convsp = 1 << (~data & 0x1f);
			}
			else
				tms->convsp = data;
			break;

		case REG020_CONVDP:
			if (data & 0x001f)
			{
				if (data & 0x1f00)
					tms->convdp = (1 << (~data & 0x1f)) + (1 << (~(data >> 8) & 0x1f));
				else
					tms->convdp = 1 << (~data & 0x1f);
			}
			else
				tms->convdp = data;
			break;

		case REG020_CONVMP:
			if (data & 0x001f)
			{
				if (data & 0x1f00)
					tms->convmp = (1 << (~data & 0x1f)) + (1 << (~(data >> 8) & 0x1f));
				else
					tms->convmp = 1 << (~data & 0x1f);
			}
			else
				tms->convmp = data;
			break;

		case REG020_DPYSTRT:
		case REG020_DPYADR:
		case REG020_DPYTAP:
			break;

		case REG020_HEBLNK:
		case REG020_HSBLNK:
			if (oldreg != data)
				tms->hblank_stable = 0;
			break;
	}
}



/***************************************************************************
    I/O REGISTER READS
***************************************************************************/

READ16_HANDLER( tms34010_io_register_r )
{
	tms34010_state *tms = space->cpu->token;
	int result, total;

//  if (LOG_CONTROL_REGS)
//      logerror("CPU#%d@%08X: read %s\n", cpunum, cpu_get_pc(space->cpu), ioreg_name[offset]);

	switch (offset)
	{
		case REG_HCOUNT:
			/* scale the horizontal position from screen width to HTOTAL */
			result = video_screen_get_hpos(tms->screen);
			total = IOREG(tms, REG_HTOTAL) + 1;
			result = result * total / video_screen_get_width(tms->screen);

			/* offset by the HBLANK end */
			result += IOREG(tms, REG_HEBLNK);

			/* wrap around */
			if (result > total)
				result -= total;
			return result;

		case REG_REFCNT:
			return (cpu_get_total_cycles(space->cpu) / 16) & 0xfffc;

		case REG_INTPEND:
			result = IOREG(tms, offset);

			/* Cool Pool loops in mainline code on the appearance of the DI, even though they */
			/* have an IRQ handler. For this reason, we return it signalled a bit early in order */
			/* to make it past these loops. */
			if (SMART_IOREG(tms, VCOUNT) + 1 == SMART_IOREG(tms, DPYINT) &&
				attotime_compare(timer_timeleft(tms->scantimer), ATTOTIME_IN_HZ(40000000/8/3)) < 0)
				result |= TMS34010_DI;
			return result;
	}

	return IOREG(tms, offset);
}


READ16_HANDLER( tms34020_io_register_r )
{
	tms34010_state *tms = space->cpu->token;
	int result, total;

//  if (LOG_CONTROL_REGS)
//      logerror("CPU#%d@%08X: read %s\n", cpunum, cpu_get_pc(space->cpu), ioreg_name[offset]);

	switch (offset)
	{
		case REG020_HCOUNT:
			/* scale the horizontal position from screen width to HTOTAL */
			result = video_screen_get_hpos(tms->screen);
			total = IOREG(tms, REG020_HTOTAL) + 1;
			result = result * total / video_screen_get_width(tms->screen);

			/* offset by the HBLANK end */
			result += IOREG(tms, REG020_HEBLNK);

			/* wrap around */
			if (result > total)
				result -= total;
			return result;

		case REG020_REFADR:
		{
			int refreshrate = (IOREG(tms, REG020_CONFIG) >> 8) & 7;
			if (refreshrate < 6)
				return (cpu_get_total_cycles(space->cpu) / refreshrate) & 0xffff;
			break;
		}
	}

	return IOREG(tms, offset);
}



/***************************************************************************
    SAVE STATE
***************************************************************************/

static STATE_POSTLOAD( tms34010_state_postload )
{
	tms34010_state *tms = param;
	set_raster_op(tms);
	set_pixel_function(tms);
}


/***************************************************************************
    HOST INTERFACE WRITES
***************************************************************************/

void tms34010_host_w(const device_config *cpu, int reg, int data)
{
	const address_space *space;
	tms34010_state *tms = cpu->token;
	unsigned int addr;

	/* swap to the target cpu */
	cpu_push_context(cpu);

	switch (reg)
	{
		/* upper 16 bits of the address */
		case TMS34010_HOST_ADDRESS_H:
			IOREG(tms, REG_HSTADRH) = data;
			break;

		/* lower 16 bits of the address */
		case TMS34010_HOST_ADDRESS_L:
			IOREG(tms, REG_HSTADRL) = data;
			break;

		/* actual data */
		case TMS34010_HOST_DATA:

			/* write to the address */
			addr = (IOREG(tms, REG_HSTADRH) << 16) | IOREG(tms, REG_HSTADRL);
			TMS34010_WRMEM_WORD(tms, TOBYTE(addr & 0xfffffff0), data);

			/* optional postincrement */
			if (IOREG(tms, REG_HSTCTLH) & 0x0800)
			{
				addr += 0x10;
				IOREG(tms, REG_HSTADRH) = addr >> 16;
				IOREG(tms, REG_HSTADRL) = (UINT16)addr;
			}
			break;

		/* control register */
		case TMS34010_HOST_CONTROL:
			tms->external_host_access = TRUE;
			space = cpu_get_address_space(tms->device, ADDRESS_SPACE_PROGRAM);
			tms34010_io_register_w(space, REG_HSTCTLH, data & 0xff00, 0xffff);
			tms34010_io_register_w(space, REG_HSTCTLL, data & 0x00ff, 0xffff);
			tms->external_host_access = FALSE;
			break;

		/* error case */
		default:
			logerror("tms34010_host_control_w called on invalid register %d\n", reg);
			break;
	}

	/* swap back */
	cpu_pop_context();
}



/***************************************************************************
    HOST INTERFACE READS
***************************************************************************/

int tms34010_host_r(const device_config *cpu, int reg)
{
	tms34010_state *tms = cpu->token;
	unsigned int addr;
	int result = 0;

	/* swap to the target cpu */

	switch (reg)
	{
		/* upper 16 bits of the address */
		case TMS34010_HOST_ADDRESS_H:
			result = IOREG(tms, REG_HSTADRH);
			break;

		/* lower 16 bits of the address */
		case TMS34010_HOST_ADDRESS_L:
			result = IOREG(tms, REG_HSTADRL);
			break;

		/* actual data */
		case TMS34010_HOST_DATA:

			/* read from the address */
			addr = (IOREG(tms, REG_HSTADRH) << 16) | IOREG(tms, REG_HSTADRL);
			cpu_push_context(cpu);
			result = TMS34010_RDMEM_WORD(tms, TOBYTE(addr & 0xfffffff0));
			cpu_pop_context();

			/* optional postincrement (it says preincrement, but data is preloaded, so it
               is effectively a postincrement */
			if (IOREG(tms, REG_HSTCTLH) & 0x1000)
			{
				addr += 0x10;
				IOREG(tms, REG_HSTADRH) = addr >> 16;
				IOREG(tms, REG_HSTADRL) = (UINT16)addr;
			}
			break;

		/* control register */
		case TMS34010_HOST_CONTROL:
			result = (IOREG(tms, REG_HSTCTLH) & 0xff00) | (IOREG(tms, REG_HSTCTLL) & 0x00ff);
			break;

		/* error case */
		default:
			logerror("tms34010_host_control_r called on invalid register %d\n", reg);
			break;
	}

	return result;
}



/**************************************************************************
 * Generic set_info
 **************************************************************************/

static CPU_SET_INFO( tms34010 )
{
	tms34010_state *tms = device->token;

	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + 0:				set_irq_line(tms, 0, info->i);				break;
		case CPUINFO_INT_INPUT_STATE + 1:				set_irq_line(tms, 1, info->i);				break;

		case CPUINFO_INT_PC:       						tms->pc = info->i; 							break;
		case CPUINFO_INT_REGISTER + TMS34010_PC:		tms->pc = info->i;							break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + TMS34010_SP:		SP(tms) = info->i;							break;
		case CPUINFO_INT_REGISTER + TMS34010_ST:		tms->st = info->i;							break;
		case CPUINFO_INT_REGISTER + TMS34010_A0:		AREG(tms, 0) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A1:		AREG(tms, 1) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A2:		AREG(tms, 2) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A3:		AREG(tms, 3) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A4:		AREG(tms, 4) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A5:		AREG(tms, 5) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A6:		AREG(tms, 6) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A7:		AREG(tms, 7) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A8:		AREG(tms, 8) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A9:		AREG(tms, 9) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A10:		AREG(tms, 10) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A11:		AREG(tms, 11) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A12:		AREG(tms, 12) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A13:		AREG(tms, 13) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_A14:		AREG(tms, 14) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B0:		BREG(tms, 0) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B1:		BREG(tms, 1) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B2:		BREG(tms, 2) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B3:		BREG(tms, 3) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B4:		BREG(tms, 4) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B5:		BREG(tms, 5) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B6:		BREG(tms, 6) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B7:		BREG(tms, 7) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B8:		BREG(tms, 8) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B9:		BREG(tms, 9) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B10:		BREG(tms, 10) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B11:		BREG(tms, 11) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B12:		BREG(tms, 12) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B13:		BREG(tms, 13) = info->i;						break;
		case CPUINFO_INT_REGISTER + TMS34010_B14:		BREG(tms, 14) = info->i;						break;
	}
}



/**************************************************************************
 * Generic get_info
 **************************************************************************/

CPU_GET_INFO( tms34010 )
{
	tms34010_state *tms = (device != NULL) ? device->token : NULL;

	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = sizeof(tms34010_state);		break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 2;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = 0;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = CPU_IS_LE;					break;
		case CPUINFO_INT_CLOCK_MULTIPLIER:				info->i = 1;							break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 8;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 2;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 10;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 1;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 10000;						break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM:	info->i = 16;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 32;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 3;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO: 		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE + 0:				info->i = (IOREG(tms, REG_INTPEND) & TMS34010_INT1) ? ASSERT_LINE : CLEAR_LINE; break;
		case CPUINFO_INT_INPUT_STATE + 1:				info->i = (IOREG(tms, REG_INTPEND) & TMS34010_INT2) ? ASSERT_LINE : CLEAR_LINE; break;

		case CPUINFO_INT_PREVIOUSPC:					/* not implemented */					break;

		case CPUINFO_INT_PC:
		case CPUINFO_INT_REGISTER + TMS34010_PC:		info->i = tms->pc;							break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + TMS34010_SP:		info->i = SP(tms);							break;
		case CPUINFO_INT_REGISTER + TMS34010_ST:		info->i = tms->st;							break;
		case CPUINFO_INT_REGISTER + TMS34010_A0:		info->i = AREG(tms, 0);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A1:		info->i = AREG(tms, 1);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A2:		info->i = AREG(tms, 2);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A3:		info->i = AREG(tms, 3);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A4:		info->i = AREG(tms, 4);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A5:		info->i = AREG(tms, 5);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A6:		info->i = AREG(tms, 6);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A7:		info->i = AREG(tms, 7);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A8:		info->i = AREG(tms, 8);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A9:		info->i = AREG(tms, 9);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A10:		info->i = AREG(tms, 10);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A11:		info->i = AREG(tms, 11);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A12:		info->i = AREG(tms, 12);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A13:		info->i = AREG(tms, 13);						break;
		case CPUINFO_INT_REGISTER + TMS34010_A14:		info->i = AREG(tms, 14);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B0:		info->i = BREG(tms, 0);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B1:		info->i = BREG(tms, 1);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B2:		info->i = BREG(tms, 2);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B3:		info->i = BREG(tms, 3);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B4:		info->i = BREG(tms, 4);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B5:		info->i = BREG(tms, 5);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B6:		info->i = BREG(tms, 6);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B7:		info->i = BREG(tms, 7);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B8:		info->i = BREG(tms, 8);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B9:		info->i = BREG(tms, 9);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B10:		info->i = BREG(tms, 10);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B11:		info->i = BREG(tms, 11);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B12:		info->i = BREG(tms, 12);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B13:		info->i = BREG(tms, 13);						break;
		case CPUINFO_INT_REGISTER + TMS34010_B14:		info->i = BREG(tms, 14);						break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = CPU_SET_INFO_NAME(tms34010);		break;
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = CPU_GET_CONTEXT_NAME(tms34010); break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = CPU_SET_CONTEXT_NAME(tms34010); break;
		case CPUINFO_PTR_INIT:							info->init = CPU_INIT_NAME(tms34010);				break;
		case CPUINFO_PTR_RESET:							info->reset = CPU_RESET_NAME(tms34010);			break;
		case CPUINFO_PTR_EXIT:							info->exit = CPU_EXIT_NAME(tms34010);				break;
		case CPUINFO_PTR_EXECUTE:						info->execute = CPU_EXECUTE_NAME(tms34010);		break;
		case CPUINFO_PTR_BURN:							info->burn = NULL;						break;
		case CPUINFO_PTR_DISASSEMBLE:					info->disassemble = CPU_DISASSEMBLE_NAME(tms34010);		break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &tms->icount;		break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s, "TMS34010");			break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s, "Texas Instruments 340x0"); break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s, "1.0");					break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s, __FILE__);				break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s, "Copyright Alex Pasadyn/Zsolt Vasvari\nParts based on code by Aaron Giles"); break;

		case CPUINFO_STR_FLAGS:
			sprintf(info->s, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				tms->st & 0x80000000 ? 'N':'.',
				tms->st & 0x40000000 ? 'C':'.',
				tms->st & 0x20000000 ? 'Z':'.',
				tms->st & 0x10000000 ? 'V':'.',
				tms->st & 0x02000000 ? 'P':'.',
				tms->st & 0x00200000 ? 'I':'.',
				tms->st & 0x00000800 ? 'E':'.',
				tms->st & 0x00000400 ? 'F':'.',
				tms->st & 0x00000200 ? 'F':'.',
				tms->st & 0x00000100 ? 'F':'.',
				tms->st & 0x00000080 ? 'F':'.',
				tms->st & 0x00000040 ? 'F':'.',
				tms->st & 0x00000020 ? 'E':'.',
				tms->st & 0x00000010 ? 'F':'.',
				tms->st & 0x00000008 ? 'F':'.',
				tms->st & 0x00000004 ? 'F':'.',
				tms->st & 0x00000002 ? 'F':'.',
				tms->st & 0x00000001 ? 'F':'.');
			break;

		case CPUINFO_STR_REGISTER + TMS34010_PC:		sprintf(info->s, "PC :%08X", tms->pc); break;
		case CPUINFO_STR_REGISTER + TMS34010_SP:		sprintf(info->s, "SP :%08X", AREG(tms, 15)); break;
		case CPUINFO_STR_REGISTER + TMS34010_ST:		sprintf(info->s, "ST :%08X", tms->st); break;
		case CPUINFO_STR_REGISTER + TMS34010_A0:		sprintf(info->s, "A0 :%08X", AREG(tms,  0)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A1:		sprintf(info->s, "A1 :%08X", AREG(tms,  1)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A2:		sprintf(info->s, "A2 :%08X", AREG(tms,  2)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A3:		sprintf(info->s, "A3 :%08X", AREG(tms,  3)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A4:		sprintf(info->s, "A4 :%08X", AREG(tms,  4)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A5:		sprintf(info->s, "A5 :%08X", AREG(tms,  5)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A6:		sprintf(info->s, "A6 :%08X", AREG(tms,  6)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A7:		sprintf(info->s, "A7 :%08X", AREG(tms,  7)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A8:		sprintf(info->s, "A8 :%08X", AREG(tms,  8)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A9:		sprintf(info->s, "A9 :%08X", AREG(tms,  9)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A10:		sprintf(info->s,"A10:%08X", AREG(tms, 10)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A11:		sprintf(info->s,"A11:%08X", AREG(tms, 11)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A12:		sprintf(info->s,"A12:%08X", AREG(tms, 12)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A13:		sprintf(info->s,"A13:%08X", AREG(tms, 13)); break;
		case CPUINFO_STR_REGISTER + TMS34010_A14:		sprintf(info->s,"A14:%08X", AREG(tms, 14)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B0:		sprintf(info->s, "B0 :%08X", BREG(tms,  0)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B1:		sprintf(info->s, "B1 :%08X", BREG(tms,  1)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B2:		sprintf(info->s, "B2 :%08X", BREG(tms,  2)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B3:		sprintf(info->s, "B3 :%08X", BREG(tms,  3)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B4:		sprintf(info->s, "B4 :%08X", BREG(tms,  4)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B5:		sprintf(info->s, "B5 :%08X", BREG(tms,  5)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B6:		sprintf(info->s, "B6 :%08X", BREG(tms,  6)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B7:		sprintf(info->s, "B7 :%08X", BREG(tms,  7)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B8:		sprintf(info->s, "B8 :%08X", BREG(tms,  8)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B9:		sprintf(info->s, "B9 :%08X", BREG(tms,  9)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B10:		sprintf(info->s,"B10:%08X", BREG(tms, 10)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B11:		sprintf(info->s,"B11:%08X", BREG(tms, 11)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B12:		sprintf(info->s,"B12:%08X", BREG(tms, 12)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B13:		sprintf(info->s,"B13:%08X", BREG(tms, 13)); break;
		case CPUINFO_STR_REGISTER + TMS34010_B14:		sprintf(info->s,"B14:%08X", BREG(tms, 14)); break;
	}
}


/**************************************************************************
 * CPU-specific set_info
 **************************************************************************/

CPU_GET_INFO( tms34020 )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = sizeof(tms34010_state);		break;
		case CPUINFO_INT_CLOCK_MULTIPLIER:				info->i = 1;							break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 4;							break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = CPU_GET_CONTEXT_NAME(tms34020); break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = CPU_SET_CONTEXT_NAME(tms34020); break;
		case CPUINFO_PTR_RESET:							info->reset = CPU_RESET_NAME(tms34020);			break;
		case CPUINFO_PTR_DISASSEMBLE:					info->disassemble = CPU_DISASSEMBLE_NAME(tms34020);		break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s, "TMS34020");			break;

		default:										CPU_GET_INFO_CALL(tms34010);		break;
	}
}
