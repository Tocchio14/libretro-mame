#include "emu.h"
#include "h8_sci.h"

const device_type H8_SCI = &device_creator<h8_sci_device>;

h8_sci_device::h8_sci_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, H8_SCI, "H8 Serial Communications Interface", tag, owner, clock, "h8_sci", __FILE__),
	cpu(*this, DEVICE_SELF_OWNER),
	tx_cb(*this),
	clk_cb(*this)
{
}

void h8_sci_device::set_info(const char *_intc_tag,int eri, int rxi, int txi, int tei)
{
	intc_tag = _intc_tag;
	eri_int = eri;
	rxi_int = rxi;
	txi_int = txi;
	tei_int = tei;
}

WRITE8_HANDLER(h8_sci_device::smr_w)
{
	smr = data;
	logerror("%s: smr_w %02x %s %c%c%c%s /%d (%06x)\n", tag(), data,
			 data & SMR_CA ? "sync" : "async",
			 data & SMR_CHR ? '7' : '8',
			 data & SMR_PE ? data & SMR_OE ? 'o' : 'e' : 'n',
			 data & SMR_STOP ? '2' : '1',
			 data & SMR_MP ? " mp" : "",
			 1 << 2*(data & SMR_CKS),
			 cpu->pc());
	clock_update();
}

READ8_HANDLER(h8_sci_device::smr_r)
{
	logerror("%s: smr_r %02x (%06x)\n", tag(), smr, cpu->pc());
	return smr;
}

WRITE8_HANDLER(h8_sci_device::brr_w)
{
	brr = data;
	logerror("%s: brr_w %02x (%06x)\n", tag(), data, cpu->pc());
	clock_update();
}

READ8_HANDLER(h8_sci_device::brr_r)
{
	logerror("%s: brr_r %02x (%06x)\n", tag(), brr, cpu->pc());
	return brr;
}

WRITE8_HANDLER(h8_sci_device::scr_w)
{
	logerror("%s: scr_w %02x%s%s%s%s%s%s clk=%d (%06x)\n", tag(), data,
			 data & SCR_TIE  ? " txi" : "",
			 data & SCR_RIE  ? " rxi" : "",
			 data & SCR_TE   ? " tx" : "",
			 data & SCR_RE   ? " rx" : "",
			 data & SCR_MPIE ? " mpi" : "",
			 data & SCR_TEIE ? " tei" : "",
			 data & SCR_CKE,
			 cpu->pc());

	UINT8 delta = scr ^ data;
	scr = data;
	clock_update();

	if((delta & SCR_RE) && !(scr & SCR_RE)) {
		rx_state = ST_IDLE;
		clock_stop(CLK_RX);
	}

	if((delta & SCR_RE) && (scr & SCR_RE) && rx_state == ST_IDLE && !(ssr & (SSR_ORER|SSR_PER|SSR_FER)))
		rx_start();
	if((delta & SCR_TIE) && (scr & SCR_TIE) && (ssr & SSR_TDRE))
		intc->internal_interrupt(txi_int);
	if((delta & SCR_TEIE) && (scr & SCR_TEIE) && (ssr & SSR_TEND))
		intc->internal_interrupt(tei_int);
	if((delta & SCR_RIE) && (scr & SCR_RIE) && (ssr & SSR_RDRF))
		intc->internal_interrupt(rxi_int);
	if((delta & SCR_RIE) && (scr & SCR_RIE) && (ssr & (SSR_ORER|SSR_PER|SSR_FER)))
		intc->internal_interrupt(eri_int);
}

READ8_HANDLER(h8_sci_device::scr_r)
{
	logerror("%s: scr_r %02x (%06x)\n", tag(), scr, cpu->pc());
	return scr;
}

WRITE8_HANDLER(h8_sci_device::tdr_w)
{
	logerror("%s: tdr_w %02x (%06x)\n", tag(), data, cpu->pc());
	tdr = data;
}

READ8_HANDLER(h8_sci_device::tdr_r)
{
	logerror("%s: tdr_r %02x (%06x)\n", tag(), tdr, cpu->pc());
	return tdr;
}

WRITE8_HANDLER(h8_sci_device::ssr_w)
{
	if(!(scr & SCR_TE)) {
		data |= SSR_TDRE;
		ssr |= SSR_TDRE;
	}
	if((ssr & SSR_TDRE) && !(data & SSR_TDRE))
		ssr &= ~SSR_TEND;
	ssr = ((ssr & ~SSR_MPBT) | (data & SSR_MPBT)) & (data | (SSR_TEND|SSR_MPB|SSR_MPBT));
	logerror("%s: ssr_w %02x -> %02x (%06x)\n", tag(), data, ssr, cpu->pc());

	if(tx_state == ST_IDLE && !(ssr & SSR_TDRE))
		tx_start();
}

READ8_HANDLER(h8_sci_device::ssr_r)
{
	logerror("%s: ssr_r %02x (%06x)\n", tag(), ssr, cpu->pc());
	return ssr;
}

READ8_HANDLER(h8_sci_device::rdr_r)
{
	logerror("%s: rdr_r %02x (%06x)\n", tag(), rdr, cpu->pc());
	return rdr;
}

WRITE8_HANDLER(h8_sci_device::scmr_w)
{
	logerror("%s: scmr_w %02x (%06x)\n", tag(), data, cpu->pc());
}

READ8_HANDLER(h8_sci_device::scmr_r)
{
	logerror("%s: scmr_r (%06x)\n", tag(), cpu->pc());
	return 0x00;
}

void h8_sci_device::clock_update()
{
	// Sync: Divider must be the time of a half-period (both edges are used, datarate*2)
	// Async: Divider must be the time of one period (only raising edge used, datarate*16)

	divider = 2 << (2*(smr & SMR_CKS));
	divider *= brr+1;

	if(smr & SMR_CA) {
		if(scr & SCR_CKE1)
			clock_mode = CLKM_EXTERNAL_SYNC;
		else
			clock_mode = CLKM_INTERNAL_SYNC_OUT;
	} else {
		if(scr & SCR_CKE1)
			clock_mode = CLKM_EXTERNAL_ASYNC;
		else if(scr & SCR_CKE0)
			clock_mode = CLKM_INTERNAL_ASYNC_OUT;
		else
			clock_mode = CLKM_INTERNAL_ASYNC;
	}

	switch(clock_mode) {
	case CLKM_INTERNAL_ASYNC:
		logerror("%s: clock internal at %d Hz, async, bitrate %d bps\n", tag(), int(cpu->clock() / (divider*2)), int(cpu->clock() / (divider*16)));
		break;
	case CLKM_INTERNAL_ASYNC_OUT:
		logerror("%s: clock internal at %d Hz, async, bitrate %d bps, output\n", tag(), int(cpu->clock() / (divider*2)), int(cpu->clock() / (divider*16)));
		break;
	case CLKM_EXTERNAL_ASYNC:
		logerror("%s: clock external, async\n", tag());
		break;
	case CLKM_INTERNAL_SYNC_OUT:
		logerror("%s: clock internal at %d Hz, sync, output\n", tag(), int(cpu->clock() / (divider*2)));
		break;
	case CLKM_EXTERNAL_SYNC:
		logerror("%s: clock external, sync\n", tag());
		break;
	}
}

void h8_sci_device::device_start()
{
	tx_cb.resolve_safe();
	clk_cb.resolve_safe();

	intc = siblingdevice<h8_intc_device>(intc_tag);
	save_item(NAME(rdr));
	save_item(NAME(tdr));
	save_item(NAME(smr));
	save_item(NAME(scr));
	save_item(NAME(ssr));
	save_item(NAME(brr));
	save_item(NAME(rsr));
	save_item(NAME(tsr));
	save_item(NAME(rx_bit));
	save_item(NAME(tx_bit));
	save_item(NAME(rx_state));
	save_item(NAME(tx_state));
	save_item(NAME(tx_parity));
	save_item(NAME(clock_state));
	save_item(NAME(clock_value));
	save_item(NAME(clock_base));
	save_item(NAME(divider));
	save_item(NAME(ext_clock_value));
	save_item(NAME(ext_clock_counter));
}

void h8_sci_device::device_reset()
{
	rdr = 0x00;
	tdr = 0xff;
	smr = 0x00;
	scr = 0x00;
	ssr = 0x84;
	brr = 0xff;
	rsr = 0x00;
	tsr = 0xff;
	rx_bit = 0;
	tx_bit = 0;
	tx_state = ST_IDLE;
	rx_state = ST_IDLE;
	clock_state = 0;
	clock_mode = CLKM_INTERNAL_ASYNC;
	clock_base = 0;
	clock_update();
	clock_value = true;
	ext_clock_value = true;
	ext_clock_counter = 0;
	rx_value = true;
	clk_cb(clock_value);
}

WRITE_LINE_MEMBER(h8_sci_device::rx_w)
{
	rx_value = state;
	logerror("%s: rx=%d\n", tag(), state);
	if(!rx_value && !(clock_state & CLK_RX) && rx_state != ST_IDLE)
		clock_start(CLK_RX);
}

WRITE_LINE_MEMBER(h8_sci_device::clk_w)
{
	if(ext_clock_value != state) {
		ext_clock_value = state;
		if(clock_state) {
			logerror("%s: clki=%d\n", tag(), state);
			switch(clock_mode) {
			case CLKM_EXTERNAL_ASYNC:
				if(ext_clock_value) {
					ext_clock_counter = (ext_clock_counter+1) & 15;

					if((clock_state & CLK_TX) && ext_clock_counter == 0)
						tx_dropped_edge();
					if((clock_state & CLK_RX) && ext_clock_counter == 8)
						rx_raised_edge();
				}
				break;

			case CLKM_EXTERNAL_SYNC:
				if((!ext_clock_value) && (clock_state & CLK_TX))
					tx_dropped_edge();
				
				else if(ext_clock_value && (clock_state & CLK_RX))
					rx_raised_edge();
				break;
			}
		}
	}
}

UINT64 h8_sci_device::internal_update(UINT64 current_time)
{
	switch(clock_mode) {
	case CLKM_INTERNAL_SYNC_OUT:
		if(clock_state || !clock_value) {
			UINT64 fp = divider*2;
			UINT64 delta = current_time - clock_base;
			if(delta >= fp) {
				delta -= fp;
				clock_base += fp;
			}
			assert(delta < fp);

			bool new_clock = delta >= divider;
			if(new_clock != clock_value) {
				if((!new_clock) && (clock_state & CLK_TX))
					tx_dropped_edge();
				
				else if(new_clock && (clock_state & CLK_RX))
					rx_raised_edge();

				if(clock_state || !clock_value) {
					clock_value = new_clock;
					clk_cb(clock_value);
				}
			}
			
			return clock_base + (clock_value ? fp : divider);
		} else
			return 0;

	case CLKM_INTERNAL_ASYNC:
	case CLKM_INTERNAL_ASYNC_OUT:
		if(clock_state || !clock_value) {
			UINT64 fp = divider*16;
			UINT64 delta = current_time - clock_base;
			if(delta >= fp) {
				delta -= fp;
				clock_base += fp;
			}
			assert(delta < fp);
			bool new_clock = delta >= divider*8;
			if(new_clock != clock_value) {
				if((!new_clock) && (clock_state & CLK_TX))
					tx_dropped_edge();
				
				else if(new_clock && (clock_state & CLK_RX))
					rx_raised_edge();

				if(clock_state || !clock_value) {
					clock_value = new_clock;
					if(clock_mode == CLKM_INTERNAL_ASYNC_OUT)
						clk_cb(clock_value);
				}
			}
			
			return clock_base + (clock_value ? fp : divider*16);
		} else
			return 0;

	case CLKM_EXTERNAL_ASYNC:
	case CLKM_EXTERNAL_SYNC:
		return 0;
	}
	return 0;
}

void h8_sci_device::clock_start(int mode)
{
	// Happens when back-to-back
	if(clock_state & mode)
		return;

	if(!clock_state) {
		clock_state = mode;
		switch(clock_mode) {
		case CLKM_INTERNAL_ASYNC:
		case CLKM_INTERNAL_ASYNC_OUT:
		case CLKM_INTERNAL_SYNC_OUT:
			logerror("%s: Starting internal clock\n", tag());
			clock_base = cpu->get_cycle();
			cpu->internal_update();
			break;

		case CLKM_EXTERNAL_ASYNC:
			logerror("%s: Waiting for external clock async\n", tag());
			ext_clock_counter = 15;
			break;

		case CLKM_EXTERNAL_SYNC:
			logerror("%s: Waiting for external clock sync\n", tag());
			break;
		}
	} else
		clock_state |= mode;
}

void h8_sci_device::clock_stop(int mode)
{
	clock_state &= ~mode;
	cpu->internal_update();
}

void h8_sci_device::tx_start()
{
	ssr |= SSR_TDRE;
	tsr = tdr;
	tx_parity = smr & SMR_OE ? 0 : 1;
	logerror("%s: start transmit %02x\n", tag(), tsr);
	if(scr & SCR_TIE)
		intc->internal_interrupt(txi_int);
	if(smr & SMR_CA) {
		tx_state = ST_BIT;
		tx_bit = 8;
	} else {
		tx_state = ST_START;
		tx_bit = 1;
	}
	clock_start(CLK_TX);
}
 
void h8_sci_device::tx_dropped_edge()
{
	logerror("%s: tx_dropped_edge state=%d bit=%d\n", tag(), tx_state, tx_bit);
	switch(tx_state) {
	case ST_START:
		tx_cb(false);
		assert(tx_bit == 1);
		tx_state = ST_BIT;
		tx_bit = smr & SMR_CHR ? 7 : 8;
		break;

	case ST_BIT:
		tx_parity ^= (tsr & 1);
		tx_cb(tsr & 1);
		tsr >>= 1;
		tx_bit--;
		if(!tx_bit) {
			if(smr & SMR_CA) {
				if(!(ssr & SSR_TDRE))
					tx_start();
				else {
					tx_state = ST_LAST_TICK;
					tx_bit = 0;
				}
			} else if(smr & SMR_PE) {
				tx_state = ST_PARITY;
				tx_bit = 1;
			} else {
				tx_state = ST_STOP;
				tx_bit = smr & SMR_STOP ? 2 : 1;
			}
		}
		break;

	case ST_PARITY:
		tx_cb(tx_parity);
		assert(tx_bit == 1);
		tx_state = ST_STOP;
		tx_bit = smr & SMR_STOP ? 2 : 1;
		break;

	case ST_STOP:
		tx_cb(true);
		tx_bit--;
		if(!tx_bit) {
			if(!(ssr & SSR_TDRE))
					tx_start();
			else {
				tx_state = ST_LAST_TICK;
				tx_bit = 0;
			}
		}
		break;

	case ST_LAST_TICK:
		tx_state = ST_IDLE;
		tx_bit = 0;
		clock_stop(CLK_TX);
		ssr |= SSR_TEND;
		if(scr & SCR_TEIE)
			intc->internal_interrupt(tei_int);
		break;

	default:
		abort();
	}
	logerror("%s:             -> state=%d bit=%d\n", tag(), tx_state, tx_bit);
}

void h8_sci_device::rx_start()
{
	ssr |= SSR_TDRE;
	rx_parity = smr & SMR_OE ? 0 : 1;
	rsr = 0x00;
	logerror("%s: start recieve\n", tag());
	if(smr & SMR_CA) {
		rx_state = ST_BIT;
		rx_bit = 8;
		clock_start(CLK_RX);
	} else {
		rx_state = ST_START;
		rx_bit = 1;
		if(!rx_value)
			clock_start(CLK_RX);
	}
}

void h8_sci_device::rx_done()
{
	if(!(ssr & SSR_FER)) {
		if((smr & SMR_PE) && rx_parity) {
			ssr |= SSR_PER;
			logerror("%s: Recieve parity error\n", tag());
		} else if(ssr & SSR_RDRF) {
			ssr |= SSR_ORER;
			logerror("%s: Recieve overrun\n", tag());
		} else {
			ssr |= SSR_RDRF;
			logerror("%s: Recieved %02x\n", tag(), rsr);
			rdr = rsr;
		}
	}
	if(scr & SCR_RIE) {
		if(ssr & (SSR_ORER|SSR_PER|SSR_FER))
			intc->internal_interrupt(eri_int);
		else
			intc->internal_interrupt(rxi_int);
	}
	if((scr & SCR_RE) && !(ssr & (SSR_ORER|SSR_PER|SSR_FER)))
		rx_start();
	else {
		clock_stop(CLK_RX);
		rx_state = ST_IDLE;
	}
}

void h8_sci_device::rx_raised_edge()
{
	logerror("%s: rx_raised_edge state=%d bit=%d\n", tag(), rx_state, rx_value);
	switch(rx_state) {
	case ST_START:
		if(rx_value) {
			clock_stop(CLK_RX);
			break;
		}
		rx_state = ST_BIT;
		rx_bit = smr & SMR_CHR ? 7 : 8;
		break;

	case ST_BIT:
		rx_parity ^= rx_value;
		rsr >>= 1;
		if(rx_value) {
			rx_parity = !rx_parity;
			rsr |= (smr & (SMR_CA|SMR_CHR)) == SMR_CHR ? 0x40 : 0x80;
		}
		rx_bit--;
		if(!rx_bit) {
			if(smr & SMR_CA)
				rx_done();
			else if(smr & SMR_PE) {
				rx_state = ST_PARITY;
				rx_bit = 1;
			} else {
				rx_state = ST_STOP;
				rx_bit = 1; // Always 1 on rx
			}
		}
		break;

	case ST_PARITY:
		rx_parity ^= rx_value;
		assert(rx_bit == 1);
		rx_state = ST_STOP;
		rx_bit = 1;
		break;

	case ST_STOP:
		assert(rx_bit == 1);
		if(!rx_value)
			ssr |= SSR_FER;
		else if((smr & SMR_PE) && rx_parity)
			ssr |= SSR_PER;
		rx_done();
		break;

	default:
		abort();
	}
	logerror("%s:             -> state=%d\n", tag(), rx_state);
}

