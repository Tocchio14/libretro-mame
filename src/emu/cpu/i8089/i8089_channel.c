/***************************************************************************

    Intel 8089 I/O Processor

	license: MAME, GPL-2.0+
	copyright-holders: Dirk Best

    I/O channel

***************************************************************************/

#include "emu.h"
#include "debugger.h"
#include "i8089_channel.h"


//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define VERBOSE      1
#define VERBOSE_DMA  1

// channel control register fields
#define CC_TMC     ((m_r[CC].w >>  0) & 0x07)   // terminate on masked compare
#define CC_TBC     ((m_r[CC].w >>  3) & 0x03)   // terminate on byte count
#define CC_TX      ((m_r[CC].w >>  5) & 0x03)   // terminate on external signal
#define CC_TS      ((m_r[CC].w >>  7) & 0x01)   // terminate on single transfer
#define CC_CHAIN   ((m_r[CC].w >>  8) & 0x01)   // chaining
#define CC_LOCK    ((m_r[CC].w >>  9) & 0x01)   // actuate lock
#define CC_SOURCE  ((m_r[CC].w >> 10) & 0x01)   // source register
#define CC_SYNC    ((m_r[CC].w >> 11) & 0x03)   // synchronization
#define CC_TRANS   ((m_r[CC].w >> 13) & 0x01)   // translation
#define CC_FUNC    ((m_r[CC].w >> 14) & 0x03)   // function


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type I8089_CHANNEL = &device_creator<i8089_channel>;


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  i8089_channel - constructor
//-------------------------------------------------

i8089_channel::i8089_channel(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, I8089_CHANNEL, "Intel 8089 I/O Channel", tag, owner, clock, "i8089_channel", __FILE__),
	m_write_sintr(*this),
	m_icount(0),
	m_xfer_pending(false),
	m_dma_value(0),
	m_dma_state(DMA_IDLE)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void i8089_channel::device_start()
{
	// get parent device
	m_iop = downcast<i8089_device *>(owner());

	// resolve callbacks
	m_write_sintr.resolve_safe();

	// register for save states
	save_item(NAME(m_xfer_pending));
	save_item(NAME(m_dma_value));
	save_item(NAME(m_dma_state));

	for (int i = 0; i < ARRAY_LENGTH(m_r); i++)
	{
		save_item(NAME(m_r[i].w), i);
		save_item(NAME(m_r[i].t), i);
	}
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void i8089_channel::device_reset()
{
	m_xfer_pending = false;

	// initialize registers
	for (int i = 0; i < ARRAY_LENGTH(m_r); i++)
	{
		m_r[i].w = 0;
		m_r[i].t = 0;
	}
}


//**************************************************************************
//  IMPLEMENTATION
//**************************************************************************

void i8089_channel::set_reg(int reg, int value, int tag)
{
	m_r[reg].w = value;

	if (tag != -1)
		m_r[reg].t = tag;

	if (reg == TP)
		m_iop->m_current_tp = value;
}

// channel status
bool i8089_channel::executing()    { return BIT(m_r[PSW].w, 2); }
bool i8089_channel::transferring() { return BIT(m_r[PSW].w, 6); }
bool i8089_channel::priority()     { return BIT(m_r[PSW].w, 7); }
bool i8089_channel::chained()      { return CC_CHAIN; }
bool i8089_channel::lock()         { return CC_LOCK; }

UINT16 i8089_channel::displacement(int wb)
{
	UINT16 displacement = 0;

	if (wb == 1)
	{
		displacement = m_iop->read_byte(m_r[TP].w);
		set_reg(TP, m_r[TP].w + 1);
	}
	else if (wb == 2)
	{
		displacement = m_iop->read_word(m_r[TP].w);
		set_reg(TP, m_r[TP].w + 2);
	}

	return displacement;
}

UINT8 i8089_channel::offset(int aa)
{
	UINT8 offset = 0;

	if (aa == 1)
	{
		offset = m_iop->read_byte(m_r[TP].w);
		set_reg(TP, m_r[TP].w + 1);
	}

	return offset;
}

UINT8 i8089_channel::imm8()
{
	UINT8 imm8 = m_iop->read_byte(m_r[TP].w);
	set_reg(TP, m_r[TP].w + 1);
	return imm8;
}

UINT16 i8089_channel::imm16()
{
	UINT16 imm16 = m_iop->read_word(m_r[TP].w);
	set_reg(TP, m_r[TP].w + 2);
	return imm16;
}

// adjust task pointer and continue execution
void i8089_channel::terminate_dma(int offset)
{
	if (VERBOSE)
		logerror("%s('%s'): terminating dma transfer\n", shortname(), tag());

	set_reg(TP, m_r[TP].w + offset);
	m_r[PSW].w |= 1 << 2;
	m_r[PSW].w &= ~(1 << 6);
	m_dma_state = DMA_IDLE;
}

int i8089_channel::execute_run()
{
	m_icount = 0;

	// active transfer?
	if (transferring())
	{
		// new transfer?
		if (executing())
		{
			// we are no longer executing task blocks
			m_r[PSW].w &= ~(1 << 2);
			m_xfer_pending = false;

			if (VERBOSE)
			{
				logerror("%s('%s'): ---- starting dma transfer ----\n", shortname(), tag());
				logerror("%s('%s'): ga = %06x, gb = %06x, gc = %06x\n", shortname(), tag(), m_r[GA].w, m_r[GB].w, m_r[GC].w);
				logerror("%s('%s'): bc = %04x, cc = %04x, mc = %04x\n", shortname(), tag(), m_r[BC].w, m_r[CC].w, m_r[MC].w);
			}
		}

		// todo: port transfers
		if (CC_FUNC != 0x03)
			fatalerror("%s('%s'): port transfer\n", shortname(), tag());

		switch (m_dma_state)
		{
		case DMA_IDLE:
			if (VERBOSE_DMA)
				logerror("%s('%s'): entering state: DMA_IDLE (bc = %04x)\n", shortname(), tag(), m_r[BC].w);

			// synchronize on source?
			if (CC_SYNC == 0x01)
				m_dma_state = DMA_WAIT_FOR_SOURCE_DRQ;
			else
				m_dma_state = DMA_FETCH;
			break;

		case DMA_WAIT_FOR_SOURCE_DRQ:
			fatalerror("%s('%s'): wait for source drq not supported\n", shortname(), tag());
			break;

		case DMA_FETCH:
			if (VERBOSE_DMA)
				logerror("%s('%s'): entering state: DMA_FETCH", shortname(), tag());

			// source is 16-bit?
			if (BIT(m_r[PSW].w, 1))
			{
				m_dma_value = m_iop->read_word(m_r[GA + CC_SOURCE].w);
				m_r[GA + CC_SOURCE].w += 2;
				m_r[BC].w -= 2;
			}
			// destination is 16-bit, byte count is even
			else if (BIT(m_r[PSW].w, 0) && !(m_r[BC].w & 1))
			{
				m_dma_value = m_iop->read_byte(m_r[GA + CC_SOURCE].w);
				m_r[GA + CC_SOURCE].w++;
				m_r[BC].w--;
			}
			// destination is 16-bit, byte count is odd
			else if (BIT(m_r[PSW].w, 0) && (m_r[BC].w & 1))
			{
				m_dma_value |= m_iop->read_byte(m_r[GA + CC_SOURCE].w) << 8;
				m_r[GA + CC_SOURCE].w++;
				m_r[BC].w--;
			}
			// 8-bit transfer
			else
			{
				m_dma_value = m_iop->read_byte(m_r[GA + CC_SOURCE].w);
				m_r[GA + CC_SOURCE].w++;
				m_r[BC].w--;
			}

			if (VERBOSE_DMA)
				logerror("[ %04x ]\n", m_dma_value);

			if (BIT(m_r[PSW].w, 0) && (m_r[BC].w & 1))
				m_dma_state = DMA_FETCH;
			else if (CC_TRANS)
				m_dma_state = DMA_TRANSLATE;
			else if (CC_SYNC == 0x02)
				m_dma_state = DMA_WAIT_FOR_DEST_DRQ;
			else
				m_dma_state = DMA_STORE;

			break;

		case DMA_TRANSLATE:
			fatalerror("%s('%s'): dma translate requested\n", shortname(), tag());
			break;

		case DMA_WAIT_FOR_DEST_DRQ:
			fatalerror("%s('%s'): wait for destination drq not supported\n", shortname(), tag());
			break;

		case DMA_STORE:
			if (VERBOSE_DMA)
				logerror("%s('%s'): entering state: DMA_STORE", shortname(), tag());

			// destination is 16-bit?
			if (BIT(m_r[PSW].w, 0))
			{
				m_iop->write_word(m_r[GB - CC_SOURCE].w, m_dma_value);
				m_r[GB - CC_SOURCE].w += 2;

				if (VERBOSE_DMA)
					logerror("[ %04x ]\n", m_dma_value);
			}
			// destination is 8-bit
			else
			{
				m_iop->write_byte(m_r[GB - CC_SOURCE].w, m_dma_value & 0xff);
				m_r[GB - CC_SOURCE].w++;

				if (VERBOSE_DMA)
					logerror("[ %02x ]\n", m_dma_value & 0xff);
			}

			if (CC_TMC & 0x03)
				m_dma_state = DMA_COMPARE;
			else
				m_dma_state = DMA_TERMINATE;

			break;

		case DMA_COMPARE:
			fatalerror("%s('%s'): dma compare requested\n", shortname(), tag());
			break;

		case DMA_TERMINATE:
			if (VERBOSE_DMA)
				logerror("%s('%s'): entering state: DMA_TERMINATE\n", shortname(), tag());

			// terminate on masked compare?
			if (CC_TMC & 0x03)
				fatalerror("%s('%s'): terminate on masked compare not supported\n", shortname(), tag());

			// terminate on byte count?
			else if (CC_TBC && m_r[BC].w == 0)
				terminate_dma((CC_TBC - 1) * 4);

			// terminate on external signal
			else if (CC_TX)
				fatalerror("%s('%s'): terminate on external signal not supported\n", shortname(), tag());

			// terminate on single transfer
			else if (CC_TS)
				fatalerror("%s('%s'): terminate on single transfer not supported\n", shortname(), tag());

			// not terminated, continue transfer
			else
				// do we need to read another byte?
				if (BIT(m_r[PSW].w, 1) && !BIT(m_r[PSW].w, 0))
					if (CC_SYNC == 0x02)
						m_dma_state = DMA_WAIT_FOR_DEST_DRQ;
					else
						m_dma_state = DMA_STORE_BYTE_HIGH;

				// transfer done
				else
					m_dma_state = DMA_IDLE;

			break;

		case DMA_STORE_BYTE_HIGH:
			if (VERBOSE_DMA)
				logerror("%s('%s'): entering state: DMA_STORE_BYTE_HIGH[ %02x ]\n", shortname(), tag(), (m_dma_value >> 8) & 0xff);

			m_iop->write_byte(m_r[GB - CC_SOURCE].w, (m_dma_value >> 8) & 0xff);
			m_r[GB - CC_SOURCE].w++;
			m_dma_state = DMA_TERMINATE;

			break;
		}

		m_icount++;
	}

	// executing task block instructions?
	else if (executing())
	{
		// call debugger
		debugger_instruction_hook(m_iop, m_iop->m_current_tp);

		// dma transfer pending?
		if (m_xfer_pending)
			m_r[PSW].w |= 1 << 6;

		// fetch first two instruction bytes
		UINT16 op = m_iop->read_word(m_r[TP].w);
		set_reg(TP, m_r[TP].w + 2);

		// extract parameters
		UINT8 params = op & 0xff;
		UINT8 opcode = (op >> 8) & 0xff;

		int brp = (params >> 5) & 0x07;
		int wb  = (params >> 3) & 0x03;
		int aa  = (params >> 1) & 0x03;
		int w   = (params >> 0) & 0x01;
		int opc = (opcode >> 2) & 0x3f;
		int mm  = (opcode >> 0) & 0x03;

		// fix-up so we can use our register array
		if (mm == BC) mm = PP;

		UINT8 o;
		UINT16 off, seg;

		switch (opc)
		{
		case 0x00: // control
			switch (brp)
			{
			case 0: nop(); break;
			case 1: invalid(opc); break;
			case 2: sintr(); break;
			case 3: xfer(); break;
			default: wid(BIT(brp, 1), BIT(brp, 0));
			}
			break;

		case 0x02: // lpdi
			off = imm16();
			seg = imm16();
			lpdi(brp, seg, off);
			break;

		case 0x08: // add(b)i r, i
			if (w) addi_ri(brp, imm16());
			else   addbi_ri(brp, imm8());
			break;

		case 0x0a: // and(b)i r, i
			if (w) andi_ri(brp, imm16());
			else   andbi_ri(brp, imm8());
			break;

		case 0x0c: // mov(b)i r, i
			if (w) movi_ri(brp, imm16());
			else   movbi_ri(brp, imm8());
			break;

		case 0x0f: // dec r
			dec_r(brp);
			break;

		case 0x12: // hlt
			if (BIT(brp, 0)) hlt();
			else             invalid(opc);
			break;

		case 0x22: // lpd
			o = offset(aa);
			lpd(brp, mm, o);
			break;

		case 0x28: // add(b) r, m
			if (w) add_rm(brp, mm, offset(aa));
			else   addb_rm(brp, mm, offset(aa));
			break;

		case 0x2a: // and(b) r, m
			if (w) and_rm(brp, mm, offset(aa));
			else   andb_rm(brp, mm, offset(aa));
			break;

		case 0x27: // call
			o = offset(aa);
			call(mm, displacement(wb), o);
			break;

		case 0x30: // add(b)i m, i
			o = offset(aa);
			if (w) addi_mi(mm, imm16(), o);
			else   addbi_mi(mm, imm8(), o);
			break;

		case 0x32: // and(b)i m, i
			o = offset(aa);
			if (w) andi_mi(mm, imm16(), o);
			else   andbi_mi(mm, imm8(), o);
			break;

		case 0x34: // add(b) m, r
			if (w) add_mr(mm, brp, offset(aa));
			else   addb_mr(mm, brp, offset(aa));
			break;

		case 0x36: // and(b) m, r
			if (w) and_mr(mm, brp, offset(aa));
			else   andb_mr(mm, brp, offset(aa));
			break;

		case 0x3b: // dec(b) m
			if (w) dec_m(mm, offset(aa));
			else   decb(mm, offset(aa));
			break;

		case 0x3e: // clr
			clr(mm, brp, offset(aa));
			break;

		default:
			invalid(opc);
		}

		m_icount++;
	}

	// nothing to do
	else
	{
		m_icount++;
	}

	return m_icount;
}

void i8089_channel::examine_ccw(UINT8 ccw)
{
	// priority and bus load limit, bit 7 and 5
	m_r[PSW].w = (m_r[PSW].w & 0x5f) | (ccw & 0xa0);

	// acknowledge interrupt
	if (BIT(ccw, 4))
	{
		m_write_sintr(0);
		m_r[PSW].w &= ~(1 << 5);
	}

	// interrupt enable
	if (BIT(ccw, 5))
	{
		if (BIT(ccw, 4))
			m_r[PSW].w &= ~(1 << 4);
		else
			m_r[PSW].w |= 1 << 4;
	}
}

void i8089_channel::attention()
{
	// examine control byte
	UINT8 ccw = m_iop->read_byte(m_r[CP].w);

	switch (ccw & 0x07)
	{
	// no channel command
	case 0:
		if (VERBOSE)
			logerror("%s('%s'): command received: update psw\n", shortname(), tag());

		examine_ccw(ccw);
		break;

	// start channel, tb in local space
	case 1:
		if (VERBOSE)
			logerror("%s('%s'): command received: start channel in local space\n", shortname(), tag());

		examine_ccw(ccw);

		lpd(PP, CP, 2);
		movp_pm(TP, PP);
		movbi_mi(CP, 0xff, 1);

		m_r[PSW].w |= 1 << 2;

		break;

	// reserved
	case 2:
		if (VERBOSE)
			logerror("%s('%s'): command received: invalid command 010\n", shortname(), tag());

		break;

	// start channel, tb in system space
	case 3:
		if (VERBOSE)
			logerror("%s('%s'): command received: start channel in system space\n", shortname(), tag());

		examine_ccw(ccw);

		lpd(PP, CP, 2);
		lpd(TP, PP);
		movbi_mi(CP, 0xff, 1);

		m_r[PSW].w |= 1 << 2;

		if (VERBOSE)
		{
			logerror("%s('%s'): ---- starting channel ----\n", shortname(), tag());
			logerror("%s('%s'): parameter block address: %06x\n", shortname(), tag(), m_r[PP].w);
			logerror("%s('%s'): task pointer: %06x\n", shortname(), tag(), m_r[TP].w);
		}

		break;

	case 4:
		if (VERBOSE)
			logerror("%s('%s'): command received: invalid command 100\n", shortname(), tag());

		break;

	// continue channel processing
	case 5:
		if (VERBOSE)
			logerror("%s('%s'): command received: continue channel processing\n", shortname(), tag());

		// restore task pointer and parameter block
		movp_pm(TP, PP);
		movb_rm(PSW, PP, 3);
		movbi_mi(CP, 0xff, 1);

		m_r[PSW].w |= 1 << 2;

		if (VERBOSE)
		{
			logerror("%s('%s'): ---- continuing channel ----\n", shortname(), tag());
			logerror("%s('%s'): task pointer: %06x\n", shortname(), tag(), m_r[TP].w);
		}

		break;

	// halt channel, save tp
	case 6:
		if (VERBOSE)
			logerror("%s('%s'): command received: halt channel and save tp\n", shortname(), tag());

		// save task pointer and psw to parameter block
		movp_mp(PP, TP);
		movb_mr(PP, PSW, 3);
		hlt();

		break;

	// halt channel, don't save tp
	case 7:
		if (VERBOSE)
			logerror("%s('%s'): command received: halt channel\n", shortname(), tag());

		hlt();

		break;
	}
}

WRITE_LINE_MEMBER( i8089_channel::ext_w )
{
	if (VERBOSE)
		logerror("%s('%s'): ext_w: %d\n", shortname(), tag(), state);
}

WRITE_LINE_MEMBER( i8089_channel::drq_w )
{
	if (VERBOSE)
		logerror("%s('%s'): ext_w: %d\n", shortname(), tag(), state);
}
