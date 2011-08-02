/****************************************************************************************

    bfm_sc2.c

    Bellfruit scorpion2/3 driver, (under heavy construction !!!)

*****************************************************************************************

     04-2011: J Wallace: Fixed watchdog to match actual circuit, also fixed lamping code.
  30-12-2006: J Wallace: Fixed init routines.
  07-03-2006: El Condor: Recoded to more accurately represent the hardware setup.
  18-01-2006: Cleaned up for MAME inclusion
  19-08-2005: Re-Animator

Standard scorpion2 memorymap
The hardware in Scorpion 2 is effectively a Scorpion 1 board with better, non-compatible
microcontrollers, incorporating many of the old expansions on board.

   hex     |r/w| D D D D D D D D |
 location  |   | 7 6 5 4 3 2 1 0 | function
-----------+---+-----------------+-----------------------------------------
0000-1FFF  |R/W| D D D D D D D D | RAM (8k) battery backed up
-----------+---+-----------------+-----------------------------------------
2000-20FF  | W | D D D D D D D D | Reel 1 + 2 stepper latch
-----------+---+-----------------+-----------------------------------------
2000       | R | D D D D D D D D | vfd status
-----------+---+-----------------+-----------------------------------------
2100-21FF  | W | D D D D D D D D | Reel 3 + 4 stepper latch
-----------+---+-----------------+-----------------------------------------
2200-22FF  | W | D D D D D D D D | Reel 5 + 6 stepper latch
-----------+---+-----------------+-----------------------------------------
2300-231F  | W | D D D D D D D D | output mux
-----------+---+-----------------+-----------------------------------------
2300-230B  | R | D D D D D D D D | input mux
-----------+---+-----------------+-----------------------------------------
2320       |R/W| D D D D D D D D | dimas0 ?
-----------+---+-----------------+-----------------------------------------
2321       |R/W| D D D D D D D D | dimas1 ?
-----------+---+-----------------+-----------------------------------------
2322       |R/W| D D D D D D D D | dimas2 ?
-----------+---+-----------------+-----------------------------------------
2323       |R/W| D D D D D D D D | dimas3 ?
-----------+---+-----------------+-----------------------------------------
2324       |R/W| D D D D D D D D | expansion latch
-----------+---+-----------------+-----------------------------------------
2325       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
2326       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
2327       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
2328       |R/W| D D D D D D D D | muxena
-----------+---+-----------------+-----------------------------------------
2329       | W | D D D D D D D D | Timer IRQ enable
-----------+---+-----------------+-----------------------------------------
232A       |R/W| D D D D D D D D | blkdiv ?
-----------+---+-----------------+-----------------------------------------
232B       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
232C       |R/W| D D D D D D D D | dimena ?
-----------+---+-----------------+-----------------------------------------
232D       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
232E       | R | D D D D D D D D | chip status b0 = IRQ status
-----------+---+-----------------+-----------------------------------------
232F       | W | D D D D D D D D | coin inhibits
-----------+---+-----------------+-----------------------------------------
2330       | W | D D D D D D D D | payout slide latch
-----------+---+-----------------+-----------------------------------------
2331       | W | D D D D D D D D | payout triac latch
-----------+---+-----------------+-----------------------------------------
2332       |R/W| D D D D D D D D | Watchdog timer
-----------+---+-----------------+-----------------------------------------
2333       | W | D D D D D D D D | electro mechanical meters
-----------+---+-----------------+-----------------------------------------
2334       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
2335       | ? | D D D D D D D D | ???
-----------+---+-----------------+-----------------------------------------
2336       |?/W| D D D D D D D D | dimcnt ?
-----------+---+-----------------+-----------------------------------------
2337       | W | D D D D D D D D | volume override
-----------+---+-----------------+-----------------------------------------
2338       | W | D D D D D D D D | payout chip select
-----------+---+-----------------+-----------------------------------------
2339       | W | D D D D D D D D | clkden ?
-----------+---+-----------------+-----------------------------------------
2400       |R/W| D D D D D D D D | uart1 (MC6850 compatible) control/status
-----------+---+-----------------+-----------------------------------------
2500       |R/W| D D D D D D D D | uart1 (MC6850 compatible) data
-----------+---+-----------------+-----------------------------------------
2600       |R/W| D D D D D D D D | uart2 (MC6850 compatible) control/status
-----------+---+-----------------+-----------------------------------------
2700       |R/W| D D D D D D D D | uart2 (MC6850 compatible) data
-----------+---+-----------------+-----------------------------------------
2800       |R/W| D D D D D D D D | vfd1
-----------+---+-----------------+-----------------------------------------
2900       |R/W| D D D D D D D D | reset vfd1 + vfd2
-----------+---+-----------------+-----------------------------------------
2D00       |R/W| D D D D D D D D | ym2413 control
-----------+---+-----------------+-----------------------------------------
2D01       |R/W| D D D D D D D D | ym2413 data
-----------+---+-----------------+-----------------------------------------
2E00       |R/W| D D D D D D D D | ROM page latch
-----------+---+-----------------+-----------------------------------------
2F00       |R/W| D D D D D D D D | vfd2
-----------+---+-----------------+-----------------------------------------
3FFE       | R | D D D D D D D D | direct input1
-----------+---+-----------------+-----------------------------------------
3FFF       | R | D D D D D D D D | direct input2
-----------+---+-----------------+-----------------------------------------
2A00       | W | D D D D D D D D | NEC uPD7759 data
-----------+---+-----------------+-----------------------------------------
2B00       | W | D D D D D D D D | NEC uPD7759 reset
-----------+---+-----------------+-----------------------------------------
4000-5FFF  | R | D D D D D D D D | ROM (8k)
-----------+---+-----------------+-----------------------------------------
6000-7FFF  | R | D D D D D D D D | Paged ROM (8k)
           |   |                 |   page 0 : rom area 0x0000 - 0x1FFF
           |   |                 |   page 1 : rom area 0x2000 - 0x3FFF
           |   |                 |   page 2 : rom area 0x4000 - 0x5FFF
           |   |                 |   page 3 : rom area 0x6000 - 0x7FFF
-----------+---+-----------------+-----------------------------------------
8000-FFFF  | R | D D D D D D D D | ROM (32k)
-----------+---+-----------------+-----------------------------------------

Adder hardware:
    Games supported:
        * Quintoon (2 sets Dutch, 2 sets UK)
        * Pokio (1 set)
        * Paradice (1 set)
        * Pyramid (1 set)
        * Slots (1 set Dutch, 2 sets Belgian)
        * Golden Crown (1 Set)

    Known issues:
        * Need to find the 'missing' game numbers
        * Fix RS232 protocol
***************************************************************************/

#include "emu.h"
#include "cpu/m6809/m6809.h"

#include "machine/nvram.h"

#include "video/bfm_adr2.h"

#include "sound/2413intf.h"
#include "sound/upd7759.h"

/* fruit machines only */
#include "video/awpvid.h"
#include "machine/steppers.h" // stepper motor

#include "machine/bfm_bd1.h"  // vfd
#include "machine/meters.h"

#include "bfm_sc2.lh"
#include "gldncrwn.lh"
#include "paradice.lh"
#include "pokio.lh"
#include "pyramid.lh"
#include "quintoon.lh"
#include "sltblgpo.lh"
#include "sltblgtk.lh"
#include "slots.lh"

/* fruit machines only */
#include "video/bfm_dm01.h"
#include "awpdmd.lh"
#include "drwho.lh"
#include "awpvid14.lh"
#include "awpvid16.lh"


class bfm_sc2_state : public driver_device
{
public:
	bfm_sc2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	int m_sc2gui_update_mmtr;
	UINT8 *m_nvram;
	UINT8 m_key[16];
	UINT8 m_e2ram[1024];
	int m_mmtr_latch;
	int m_triac_latch;
	int m_vfd1_latch;
	int m_vfd2_latch;
	int m_irq_status;
	int m_optic_pattern;
	int m_uart1_data;
	int m_uart2_data;
	int m_data_to_uart1;
	int m_data_to_uart2;
	int m_locked;
	int m_is_timer_enabled;
	int m_reel_changed;
	int m_coin_inhibits;
	int m_irq_timer_stat;
	int m_expansion_latch;
	int m_global_volume;
	int m_volume_override;
	int m_sc2_show_door;
	int m_sc2_door_state;
	int m_reels;
	int m_reel12_latch;
	int m_reel34_latch;
	int m_reel56_latch;
	int m_pay_latch;
	int m_slide_states[6];
	int m_slide_pay_sensor[6];
	int m_has_hopper;
	int m_triac_select;
	int m_hopper_running;
	int m_hopper_coin_sense;
	int m_timercnt;
	UINT8 m_sc2_Inputs[64];
	UINT8 m_input_override[64];
	int m_e2reg;
	int m_e2state;
	int m_e2cnt;
	int m_e2data;
	int m_e2address;
	int m_e2rw;
	int m_e2data_pin;
	int m_e2dummywrite;
	int m_e2data_to_read;
	UINT8 m_codec_data[256];
};


#ifdef MAME_DEBUG
#define VERBOSE 1
#else
#define VERBOSE 0
#endif

// log serial communication between mainboard (scorpion2) and videoboard (adder2)
#define LOG_SERIAL(x) do { if (VERBOSE) logerror x; } while (0)
#define UART_LOG(x) do { if (VERBOSE) logerror x; } while (0)
#define LOG(x) do { if (VERBOSE) logerror x; } while (0)

#define MASTER_CLOCK		(XTAL_8MHz)

// local prototypes ///////////////////////////////////////////////////////

static int  read_e2ram(running_machine &machine);
static void e2ram_reset(running_machine &machine);

/*      INPUTS layout

     b7 b6 b5 b4 b3 b2 b1 b0

     82 81 80 04 03 02 01 00  0
     92 91 90 14 13 12 11 10  1
     A2 A1 A0 24 23 22 21 20  2
     B2 B1 B0 34 33 32 31 30  3
     -- 84 83 44 43 42 41 40  4
     -- 94 93 54 53 52 51 50  5
     -- A4 A3 64 63 62 61 60  6
     -- B4 B3 74 73 72 71 70  7

     B7 B6 B5 B4 B3 B2 B1 B0
      0  1  1  0  0  0

*/

///////////////////////////////////////////////////////////////////////////
// called if board is reset ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

static void on_scorpion2_reset(running_machine &machine)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	state->m_vfd1_latch        = 0;
	state->m_vfd2_latch        = 0;
	state->m_mmtr_latch        = 0;
	state->m_triac_latch       = 0;
	state->m_irq_status        = 0;
	state->m_is_timer_enabled  = 1;
	state->m_coin_inhibits     = 0;
	state->m_irq_timer_stat    = 0;
	state->m_expansion_latch   = 0;
	state->m_global_volume     = 0;
	state->m_volume_override   = 0;
	state->m_triac_select      = 0;
	state->m_pay_latch         = 0;

	state->m_reel12_latch      = 0;
	state->m_reel34_latch      = 0;
	state->m_reel56_latch      = 0;

	state->m_hopper_running    = 0;  // for video games
	state->m_hopper_coin_sense = 0;

	state->m_slide_states[0] = 0;
	state->m_slide_states[1] = 0;
	state->m_slide_states[2] = 0;
	state->m_slide_states[3] = 0;
	state->m_slide_states[4] = 0;
	state->m_slide_states[5] = 0;

	BFM_BD1_reset(0);	// reset display1
	BFM_BD1_reset(1);	// reset display2

	e2ram_reset(machine);

	devtag_reset(machine, "ymsnd");

  // reset stepper motors /////////////////////////////////////////////////
	{
		int pattern =0, i;

		for ( i = 0; i < state->m_reels; i++)
		{
			stepper_reset_position(i);
			if ( stepper_optic_state(i) ) pattern |= 1<<i;
		}

		state->m_optic_pattern = pattern;

	}

	state->m_locked        = 0;

	// make sure no inputs are overidden ////////////////////////////////////
	memset(state->m_input_override, 0, sizeof(state->m_input_override));

	// init rom bank ////////////////////////////////////////////////////////

	{
		UINT8 *rom = machine.region("maincpu")->base();

		memory_configure_bank(machine, "bank1", 0, 1, &rom[0x10000], 0);
		memory_configure_bank(machine, "bank1", 1, 3, &rom[0x02000], 0x02000);

		memory_set_bank(machine, "bank1",3);
	}
}

///////////////////////////////////////////////////////////////////////////

void Scorpion2_SetSwitchState(running_machine &machine, int strobe, int data, int state)
{
	bfm_sc2_state *drvstate = machine.driver_data<bfm_sc2_state>();
	if ( strobe < 11 && data < 8 )
	{
		if ( strobe < 8 )
		{
			drvstate->m_input_override[strobe] |= (1<<data);

			if ( state ) drvstate->m_sc2_Inputs[strobe] |=  (1<<data);
			else		 drvstate->m_sc2_Inputs[strobe] &= ~(1<<data);
		}
		else
		{
			if ( data > 2 )
			{
				drvstate->m_input_override[strobe-8+4] |= (1<<(data+2));

				if ( state ) drvstate->m_sc2_Inputs[strobe-8+4] |=  (1<<(data+2));
				else		 drvstate->m_sc2_Inputs[strobe-8+4] &= ~(1<<(data+2));
			}
			else
			{
				drvstate->m_input_override[strobe-8] |= (1<<(data+5));

				if ( state ) drvstate->m_sc2_Inputs[strobe-8] |=  (1 << (data+5));
				else		 drvstate->m_sc2_Inputs[strobe-8] &= ~(1 << (data+5));
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////

int Scorpion2_GetSwitchState(running_machine &machine, int strobe, int data)
{
	bfm_sc2_state *drvstate = machine.driver_data<bfm_sc2_state>();
	int state = 0;

	if ( strobe < 11 && data < 8 )
	{
		if ( strobe < 8 )
		{
			state = (drvstate->m_sc2_Inputs[strobe] & (1<<data) ) ? 1 : 0;
		}
		else
		{
			if ( data > 2 )
			{
				state = (drvstate->m_sc2_Inputs[strobe-8+4] & (1<<(data+2)) ) ? 1 : 0;
			}
			else
			{
				state = (drvstate->m_sc2_Inputs[strobe-8] & (1 << (data+5)) ) ? 1 : 0;
			}
		}
	}
	return state;
}

///////////////////////////////////////////////////////////////////////////

static NVRAM_HANDLER( bfm_sc2 )
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	static const UINT8 init_e2ram[10] = { 1, 4, 10, 20, 0, 1, 1, 4, 10, 20 };
	if ( read_or_write )
	{	// writing
		file->write(state->m_e2ram,sizeof(state->m_e2ram));
	}
	else
	{ // reading
		if ( file )
		{
			file->read(state->m_e2ram,sizeof(state->m_e2ram));
		}
		else
		{
			memset(state->m_e2ram,0x00,sizeof(state->m_e2ram));
			memcpy(state->m_e2ram,init_e2ram,sizeof(init_e2ram));
		}
	}
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( bankswitch_w )
{
	memory_set_bank(space->machine(), "bank1",data & 0x03);
}

///////////////////////////////////////////////////////////////////////////

static INTERRUPT_GEN( timer_irq )
{
	bfm_sc2_state *state = device->machine().driver_data<bfm_sc2_state>();
	state->m_timercnt++;

	if ( state->m_is_timer_enabled )
	{
		state->m_irq_timer_stat = 0x01;
		state->m_irq_status     = 0x02;

		generic_pulse_irq_line(device, M6809_IRQ_LINE);
	}
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( reel12_vid_w )  // in a video cabinet this is used to drive a hopper
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_reel12_latch = data;

	if ( state->m_has_hopper )
	{
		int oldhop = state->m_hopper_running;

		if ( data & 0x01 )
		{ // hopper power
			if ( data & 0x02 )
			{
				state->m_hopper_running    = 1;
			}
			else
			{
				state->m_hopper_running    = 0;
			}
		}
		else
		{
			//state->m_hopper_coin_sense = 0;
			state->m_hopper_running    = 0;
		}

		if ( oldhop != state->m_hopper_running )
		{
			state->m_hopper_coin_sense = 0;
			oldhop = state->m_hopper_running;
		}
	}
}


/* Reels 1 and 2 */
static WRITE8_HANDLER( reel12_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_reel12_latch = data;

	if ( stepper_update(0, data&0x0f   ) ) state->m_reel_changed |= 0x01;
	if ( stepper_update(1, (data>>4))&0x0f ) state->m_reel_changed |= 0x02;

	if ( stepper_optic_state(0) ) state->m_optic_pattern |=  0x01;
	else                          state->m_optic_pattern &= ~0x01;
	if ( stepper_optic_state(1) ) state->m_optic_pattern |=  0x02;
	else                          state->m_optic_pattern &= ~0x02;

	awp_draw_reel(0);
	awp_draw_reel(1);
}

static WRITE8_HANDLER( reel34_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_reel34_latch = data;

	if ( stepper_update(2, data&0x0f ) ) state->m_reel_changed |= 0x04;
	if ( stepper_update(3, (data>>4)&0x0f) ) state->m_reel_changed |= 0x08;

	if ( stepper_optic_state(2) ) state->m_optic_pattern |=  0x04;
	else                          state->m_optic_pattern &= ~0x04;
	if ( stepper_optic_state(3) ) state->m_optic_pattern |=  0x08;
	else                          state->m_optic_pattern &= ~0x08;

	awp_draw_reel(2);
	awp_draw_reel(3);
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( reel56_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_reel56_latch = data;

	if ( stepper_update(4, data&0x0f   ) ) state->m_reel_changed |= 0x10;
	if ( stepper_update(5, (data>>4)&0x0f) ) state->m_reel_changed |= 0x20;

	if ( stepper_optic_state(4) ) state->m_optic_pattern |=  0x10;
	else                          state->m_optic_pattern &= ~0x10;
	if ( stepper_optic_state(5) ) state->m_optic_pattern |=  0x20;
	else                          state->m_optic_pattern &= ~0x20;

	awp_draw_reel(4);
	awp_draw_reel(5);
}



///////////////////////////////////////////////////////////////////////////
// mechanical meters //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( mmtr_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int i;
	int  changed = state->m_mmtr_latch ^ data;

	state->m_mmtr_latch = data;

	for (i = 0; i<8; i++)
	{
		if ( changed & (1 << i) )
		{
			MechMtr_update(i, data & (1 << i) );
		}
	}
	if ( data & 0x1F ) cputag_set_input_line(space->machine(), "maincpu", M6809_FIRQ_LINE, ASSERT_LINE );
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( mux_output_w )
{
	int i;
	int off = offset<<3;

	for (i=0; i<8; i++)
		output_set_lamp_value(off+i, ((data & (1 << i)) != 0));

}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( mux_input_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int result = 0xFF,t1,t2;
	static const char *const port[] = { "STROBE0", "STROBE1", "STROBE2", "STROBE3", "STROBE4", "STROBE5", "STROBE6", "STROBE7", "STROBE8", "STROBE9", "STROBE10", "STROBE11" };

	if (offset < 8)
	{
		int idx = (offset & 4) ? 4 : 8;
		t1 = state->m_input_override[offset];	// strobe 0-7 data 0-4
		t2 = state->m_input_override[offset+idx];	// strobe 8-B data 0-4

		t1 = (state->m_sc2_Inputs[offset]   & t1) | ( ( input_port_read(space->machine(), port[offset])   & ~t1) & 0x1F);
		if (idx == 8)
			t2 = (state->m_sc2_Inputs[offset+8] & t2) | ( ( input_port_read(space->machine(), port[offset+8]) & ~t2) << 5);
		else
			t2 =  (state->m_sc2_Inputs[offset+4] & t2) | ( ( ( input_port_read(space->machine(), port[offset+4]) & ~t2) << 2) & 0x60);

		state->m_sc2_Inputs[offset]   = (state->m_sc2_Inputs[offset]   & ~0x1F) | t1;
		state->m_sc2_Inputs[offset+idx] = (state->m_sc2_Inputs[offset+idx] & ~0x60) | t2;
		result = t1 | t2;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( unlock_w )
{
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( dimas_w )
{
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( dimcnt_w )
{
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( unknown_w )
{
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( volume_override_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int old = state->m_volume_override;

	state->m_volume_override = data?1:0;

	if ( old != state->m_volume_override )
	{
		ym2413_device *ym = space->machine().device<ym2413_device>("ymsnd");
		upd7759_device *upd = space->machine().device<upd7759_device>("upd");
		float percent = state->m_volume_override? 1.0f : (32-state->m_global_volume)/32.0f;

		ym->set_output_gain(0, percent);
		ym->set_output_gain(1, percent);
		upd->set_output_gain(0, percent);
	}
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_DEVICE_HANDLER( nec_reset_w )
{
	upd7759_start_w(device, 0);
	upd7759_reset_w(device, data);
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_DEVICE_HANDLER( nec_latch_w )
{
	bfm_sc2_state *state = device->machine().driver_data<bfm_sc2_state>();
	int bank = 0;

	if ( data & 0x80 )         bank |= 0x01;
	if ( state->m_expansion_latch & 2 ) bank |= 0x02;

	upd7759_set_bank_base(device, bank*0x20000);

	upd7759_port_w(device, 0, data&0x3F);	// setup sample
	upd7759_start_w(device, 0);
	upd7759_start_w(device, 1);
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( vfd_status_hop_r )	// on video games, hopper inputs are connected to this
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	// b7 = NEC busy
	// b6 = alpha busy (also matrix board)
	// b5 - b0 = reel optics

	int result = 0;

	if ( state->m_has_hopper )
	{
		result |= 0x04; // hopper high level
		result |= 0x08; // hopper low  level

		result |= 0x01|0x02;

		if ( state->m_hopper_running )
		{
			result &= ~0x01;								  // set motor running input

			if ( state->m_timercnt & 0x04 ) state->m_hopper_coin_sense ^= 1;	  // toggle coin seen

			if ( state->m_hopper_coin_sense ) result &= ~0x02;		  // update coin seen input
		}
	}

	if ( !upd7759_busy_r(space->machine().device("upd")) ) result |= 0x80;			  // update sound busy input

	return result;
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( expansion_latch_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int changed = state->m_expansion_latch^data;

	state->m_expansion_latch = data;

	// bit0,  1 = lamp mux disabled, 0 = lamp mux enabled
	// bit1,  ? used in Del's millions
	// bit2,  digital volume pot meter, clock line
	// bit3,  digital volume pot meter, direction line
	// bit4,  ?
	// bit5,  ?
	// bit6,  ? used in Del's millions
	// bit7   ?

	if ( changed & 0x04)
	{ // digital volume clock line changed
		if ( !(data & 0x04) )
		{ // changed from high to low,
			if ( !(data & 0x08) )
			{
				if ( state->m_global_volume < 31 ) state->m_global_volume++; //0-31 expressed as 1-32
			}
			else
			{
				if ( state->m_global_volume > 0  ) state->m_global_volume--;
			}

			{
				ym2413_device *ym = space->machine().device<ym2413_device>("ymsnd");
				upd7759_device *upd = space->machine().device<upd7759_device>("upd");
				float percent = state->m_volume_override ? 1.0f : (32-state->m_global_volume)/32.0f;

				ym->set_output_gain(0, percent);
				ym->set_output_gain(1, percent);
				upd->set_output_gain(0, percent);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( expansion_latch_r )
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( muxena_w )
{
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( timerirq_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_is_timer_enabled = data & 1;
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( timerirqclr_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_irq_timer_stat = 0;
	state->m_irq_status     = 0;

	return 0;
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( irqstatus_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int result = state->m_irq_status | state->m_irq_timer_stat | 0x80;	// 0x80 = ~MUXERROR

	state->m_irq_timer_stat = 0;

	return result;
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( coininhib_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int changed = state->m_coin_inhibits^data,i,p;

	state->m_coin_inhibits = data;

	p = 0x01;
	i = 0;

	while ( i < 8 && changed )
	{
		if ( changed & p )
		{ // this inhibit line has changed
			coin_lockout_w(space->machine(), i, (~data & p) ); // update lockouts
			changed &= ~p;
		}

		p <<= 1;
		i++;
	}
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( coin_input_r )
{
	return input_port_read(space->machine(), "COINS");
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( payout_latch_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_pay_latch = data;
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( payout_triac_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	if ( state->m_triac_select == 0x57 )
	{
		int slide = 0;

		switch ( state->m_pay_latch )
		{
			case 0x01: slide = 1;
				break;

			case 0x02: slide = 2;
				break;

			case 0x04: slide = 3;
				break;

			case 0x08: slide = 4;
				break;

			case 0x10: slide = 5;
				break;

			case 0x20: slide = 6;
				break;
		}

		if ( slide )
		{
			if ( data == 0x4D )
			{
				if ( !state->m_slide_states[slide] )
				{
					if ( state->m_slide_pay_sensor[slide] )
					{
						int strobe = state->m_slide_pay_sensor[slide]>>4, data = state->m_slide_pay_sensor[slide]&0x0F;

						Scorpion2_SetSwitchState(space->machine(), strobe, data, 0);
					}
					state->m_slide_states[slide] = 1;
				}
			}
			else
			{
				if ( state->m_slide_states[slide] )
				{
					if ( state->m_slide_pay_sensor[slide] )
					{
						int strobe = state->m_slide_pay_sensor[slide]>>4, data = state->m_slide_pay_sensor[slide]&0x0F;

						Scorpion2_SetSwitchState(space->machine(), strobe, data, 1);
					}
					state->m_slide_states[slide] = 0;
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( payout_select_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_triac_select = data;
}



///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( vfd2_data_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_vfd2_latch = data;
	BFM_BD1_newdata(1, data);
	BFM_BD1_draw(1);
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( vfd_reset_w )
{
	BFM_BD1_reset(0);	  // reset both VFD's
	BFM_BD1_reset(1);
	BFM_BD1_draw(0);
	BFM_BD1_draw(1);
}

///////////////////////////////////////////////////////////////////////////
// serial port ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( uart1stat_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int status = 0x06;

	if ( state->m_data_to_uart1  ) status |= 0x01;
	if ( !state->m_data_to_uart2 ) status |= 0x02;

	return status;
}
///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( uart1data_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	return state->m_uart1_data;
}

//////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( uart1ctrl_w )
{
	UART_LOG(("uart1ctrl:%x\n", data));
}
///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( uart1data_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_data_to_uart2 = 1;
	state->m_uart1_data    = data;
	UART_LOG(("uart1:%x\n", data));
}
///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( uart2stat_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int status = 0x06;

	if ( state->m_data_to_uart2  ) status |= 0x01;
	if ( !state->m_data_to_uart1 ) status |= 0x02;

	return status;
}
///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( uart2data_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	return state->m_uart2_data;
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( uart2ctrl_w )
{
	UART_LOG(("uart2ctrl:%x\n", data));
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( uart2data_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_data_to_uart1 = 1;
	state->m_uart2_data    = data;
	UART_LOG(("uart2:%x\n", data));
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( vid_uart_tx_w )
{
	adder2_send(data);
	cputag_set_input_line(space->machine(), "adder2", M6809_IRQ_LINE, HOLD_LINE );

	LOG_SERIAL(("sadder  %02X  (%c)\n",data, data ));
}

///////////////////////////////////////////////////////////////////////////

static WRITE8_HANDLER( vid_uart_ctrl_w )
{
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( vid_uart_rx_r )
{
	int data = adder2_receive();

	LOG_SERIAL(("radder:  %02X(%c)\n",data, data ));

	return data;
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( vid_uart_ctrl_r )
{
	return adder2_status();
}

///////////////////////////////////////////////////////////////////////////

static READ8_HANDLER( key_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	int result = state->m_key[ offset ];

	if ( offset == 7 )
	{
		result = (result & 0xFE) | read_e2ram(space->machine());
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////
/*

The X24C08 is a CMOS 8,192 bit serial EEPROM,
internally organized 1024 x 8. The X24C08 features a
serial interface and software protocol allowing operation
on a simple two wire bus.

*/




#define SCL 0x01	//SCL pin (clock)
#define SDA	0x02	//SDA pin (data)


static void e2ram_reset(running_machine &machine)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	state->m_e2reg   = 0;
	state->m_e2state = 0;
	state->m_e2address = 0;
	state->m_e2rw    = 0;
	state->m_e2data_pin = 0;
	state->m_e2data  = (SDA|SCL);
	state->m_e2dummywrite = 0;
	state->m_e2data_to_read = 0;
}

static int recdata(bfm_sc2_state *state, int changed, int data)
{
	int res = 1;

	if ( state->m_e2cnt < 8 )
	{
		res = 0;

		if ( (changed & SCL) && (data & SCL) )
		{ // clocked in new data
			int pattern = 1 << (7-state->m_e2cnt);

			if ( data & SDA ) state->m_e2data |=  pattern;
			else              state->m_e2data &= ~pattern;

			state->m_e2data_pin = state->m_e2data_to_read & 0x80 ? 1 : 0;

			state->m_e2data_to_read <<= 1;

			LOG(("e2d pin= %d\n", state->m_e2data_pin));

			state->m_e2cnt++;
			if ( state->m_e2cnt >= 8 )
			{
				res++;
			}
		}
	}

	return res;
}

static int recAck(int changed, int data)
{
	int result = 0;

	if ( (changed & SCL) && (data & SCL) )
	{
		if ( data & SDA )
		{
			result = 1;
		}
		else
		{
			result = -1;
		}
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////


/* VFD Status */
static READ8_HANDLER( vfd_status_r )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	/* b7 = NEC busy */
	/* b6 = alpha busy (also matrix board) */
	/* b5 - b0 = reel optics */

	int result = state->m_optic_pattern;

	if ( !upd7759_busy_r(space->machine().device("upd")) ) result |= 0x80;

	if (space->machine().device("matrix"))
		if ( BFM_dm01_busy() ) result |= 0x40;

	return result;
}

static WRITE8_HANDLER( vfd1_data_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>();
	state->m_vfd1_latch = data;
	
	if (space->machine().device("matrix"))
	{
		BFM_dm01_writedata(space->machine(),data);
	}
	else
	{
		BFM_BD1_newdata(0, data);
		BFM_BD1_draw(0);
	}
}


//
static WRITE8_HANDLER( e2ram_w )
{
	bfm_sc2_state *state = space->machine().driver_data<bfm_sc2_state>(); // b0 = clock b1 = data

	int changed, ack;

	data ^= (SDA|SCL);  // invert signals

	changed  = (state->m_e2reg^data) & 0x03;

	state->m_e2reg = data;

	if ( changed )
	{
		while ( 1 )
		{
			if ( (  (changed & SDA) && !(data & SDA))	&&  // 1->0 on SDA  AND
				( !(changed & SCL) && (data & SCL) )    // SCL=1 and not changed
				)
			{	// X24C08 Start condition (1->0 on SDA while SCL=1)
				state->m_e2dummywrite = ( state->m_e2state == 5 );

				LOG(("e2ram:   c:%d d:%d Start condition dummywrite=%d\n", (data & SCL)?1:0, (data&SDA)?1:0, state->m_e2dummywrite ));

				state->m_e2state = 1; // ready for commands
				state->m_e2cnt   = 0;
				state->m_e2data  = 0;
				break;
			}

			if ( (  (changed & SDA) && (data & SDA))	&&  // 0->1 on SDA  AND
				( !(changed & SCL) && (data & SCL) )     // SCL=1 and not changed
				)
			{	// X24C08 Stop condition (0->1 on SDA while SCL=1)
				LOG(("e2ram:   c:%d d:%d Stop condition\n", (data & SCL)?1:0, (data&SDA)?1:0 ));
				state->m_e2state = 0;
				state->m_e2data  = 0;
				break;
			}

			switch ( state->m_e2state )
			{
				case 1: // Receiving address + R/W bit

					if ( recdata(state, changed, data) )
					{
						state->m_e2address = (state->m_e2address & 0x00FF) | ((state->m_e2data>>1) & 0x03) << 8;
						state->m_e2cnt   = 0;
						state->m_e2rw    = state->m_e2data & 1;

						LOG(("e2ram: Slave address received !!  device id=%01X device adr=%01d high order adr %0X RW=%d) %02X\n",
							state->m_e2data>>4, (state->m_e2data & 0x08)?1:0, (state->m_e2data>>1) & 0x03, state->m_e2rw , state->m_e2data ));

						state->m_e2state = 2;
					}
					break;

				case 2: // Receive Acknowledge

					ack = recAck(changed,data);
					if ( ack )
					{
						state->m_e2data_pin = 0;

						if ( ack < 0 )
						{
							LOG(("ACK = 0\n"));
							state->m_e2state = 0;
						}
						else
						{
							LOG(("ACK = 1\n"));
							if ( state->m_e2dummywrite )
							{
								state->m_e2dummywrite = 0;

								state->m_e2data_to_read = state->m_e2ram[state->m_e2address];

								if ( state->m_e2rw & 1 ) state->m_e2state = 7; // read data
								else		  state->m_e2state = 0; //?not sure
							}
							else
							{
								if ( state->m_e2rw & 1 ) state->m_e2state = 7; // reading
								else            state->m_e2state = 3; // writing
							}
							switch ( state->m_e2state )
							{
								case 7:
									LOG(("read address %04X\n",state->m_e2address));
									state->m_e2data_to_read = state->m_e2ram[state->m_e2address];
									break;
								case 3:
									LOG(("write, awaiting address\n"));
									break;
								default:
									LOG(("?unknow action %04X\n",state->m_e2address));
									break;
							}
						}
						state->m_e2data = 0;
					}
					break;

				case 3: // writing data, receiving address

					if ( recdata(state, changed, data) )
					{
						state->m_e2data_pin = 0;
						state->m_e2address = (state->m_e2address & 0xFF00) | state->m_e2data;

						LOG(("write address = %04X waiting for ACK\n", state->m_e2address));
						state->m_e2state = 4;
						state->m_e2cnt   = 0;
						state->m_e2data  = 0;
					}
					break;

				case 4: // wait ack, for write address

					ack = recAck(changed,data);
					if ( ack )
					{
						state->m_e2data_pin = 0;	// pin=0, no error !!

						if ( ack < 0 )
						{
							state->m_e2state = 0;
							LOG(("ACK = 0, cancel write\n" ));
						}
						else
						{
							state->m_e2state = 5;
							LOG(("ACK = 1, awaiting data to write\n" ));
						}
					}
					break;

				case 5: // receive data to write
					if ( recdata(state, changed, data) )
					{
						LOG(("write data = %02X received, awaiting ACK\n", state->m_e2data));
						state->m_e2cnt   = 0;
						state->m_e2state = 6;  // wait ack
					}
					break;

				case 6: // Receive Acknowlede after writing

					ack = recAck(changed,data);
					if ( ack )
					{
						if ( ack < 0 )
						{
							state->m_e2state = 0;
							LOG(("ACK=0, write canceled\n"));
						}
						else
						{
							LOG(("ACK=1, writing %02X to %04X\n", state->m_e2data, state->m_e2address));

							state->m_e2ram[state->m_e2address] = state->m_e2data;

							state->m_e2address = (state->m_e2address & ~0x000F) | ((state->m_e2address+1)&0x0F);

							state->m_e2state = 5; // write next address
						}
					}
					break;

				case 7: // receive address from read

					if ( recdata(state, changed, data) )
					{
						//state->m_e2data_pin = 0;

						LOG(("address read, data = %02X waiting for ACK\n", state->m_e2data ));

						state->m_e2state = 8;
					}
					break;

				case 8:

					if ( recAck(changed, data) )
					{
						state->m_e2state = 7;

						state->m_e2address = (state->m_e2address & ~0x0F) | ((state->m_e2address+1)&0x0F); // lower 4 bits wrap around

						state->m_e2data_to_read = state->m_e2ram[state->m_e2address];

						LOG(("ready for next address %04X\n", state->m_e2address));

						state->m_e2cnt   = 0;
						state->m_e2data  = 0;
					}
					break;

				case 0:

					LOG(("e2ram: ? c:%d d:%d\n", (data & SCL)?1:0, (data&SDA)?1:0 ));
					break;
			}
			break;
		}
	}
}

static int read_e2ram(running_machine &machine)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	LOG(("e2ram: r %d (%02X) \n", state->m_e2data_pin, state->m_e2data_to_read ));

	return state->m_e2data_pin;
}

static const UINT16 AddressDecode[]=
{
	0x0800,0x1000,0x0001,0x0004,0x0008,0x0020,0x0080,0x0200,
	0x0100,0x0040,0x0002,0x0010,0x0400,0x2000,0x4000,0x8000,
	0
};

static const UINT8 DataDecode[]=
{
	0x02,0x08,0x20,0x40,0x10,0x04,0x01,0x80,
	0
};


///////////////////////////////////////////////////////////////////////////
static void decode_mainrom(running_machine &machine, const char *rom_region)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	UINT8 *tmp, *rom;

	rom = machine.region(rom_region)->base();

	tmp = auto_alloc_array(machine, UINT8, 0x10000);
	{
		int i;
		long address;

		memcpy(tmp, rom, 0x10000);

		for ( i = 0; i < 256; i++ )
		{
			UINT8 data,pattern,newdata,*tab;
			data    = i;

			tab     = (UINT8*)DataDecode;
			pattern = 0x01;
			newdata = 0;

			do
			{
				newdata |= data & pattern ? *tab : 0;
				pattern <<= 1;
			} while ( *(++tab) );

			state->m_codec_data[i] = newdata;
		}

		for ( address = 0; address < 0x10000; address++)
		{
			int	newaddress,pattern;
			UINT16 *tab;

			tab      = (UINT16*)AddressDecode;
			pattern  = 0x0001;
			newaddress = 0;
			do
			{
				newaddress |= address & pattern ? *tab : 0;
				pattern <<= 1;
			} while ( *(++tab) );

			rom[newaddress] = state->m_codec_data[ tmp[address] ];
		}
		auto_free(machine, tmp);
	}
}

// machine init (called only once) ////////////////////////////////////////

static MACHINE_RESET( init )
{
	// reset adder2
	MACHINE_RESET_CALL(adder2);

	// reset the board //////////////////////////////////////////////////////

	on_scorpion2_reset(machine);
	BFM_BD1_init(0);
	BFM_BD1_init(1);
}

static SCREEN_UPDATE( addersc2 )
{
	bfm_sc2_state *state = screen->machine().driver_data<bfm_sc2_state>();
	if ( state->m_sc2_show_door )
	{
		output_set_value("door",( Scorpion2_GetSwitchState(screen->machine(),state->m_sc2_door_state>>4, state->m_sc2_door_state & 0x0F) ) );
	}

	return SCREEN_UPDATE_CALL(adder2);
}


static READ8_HANDLER( direct_input_r )
{
	return 0;
}




static ADDRESS_MAP_START( sc2_basemap, AS_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_RAM AM_SHARE("nvram") //8k
	AM_RANGE(0x2000, 0x2000) AM_READ(vfd_status_r)
	AM_RANGE(0x2000, 0x20FF) AM_WRITE(reel12_w)
	AM_RANGE(0x2100, 0x21FF) AM_WRITE(reel34_w)
	AM_RANGE(0x2200, 0x22FF) AM_WRITE(reel56_w)

	AM_RANGE(0x2300, 0x230B) AM_READ(mux_input_r)
	AM_RANGE(0x2300, 0x231F) AM_WRITE(mux_output_w)
	AM_RANGE(0x2320, 0x2323) AM_WRITE(dimas_w)				/* ?unknown dim related */

	AM_RANGE(0x2324, 0x2324) AM_READWRITE(expansion_latch_r, expansion_latch_w)
	AM_RANGE(0x2325, 0x2327) AM_WRITE(unknown_w)
	AM_RANGE(0x2328, 0x2328) AM_WRITE(muxena_w)
	AM_RANGE(0x2329, 0x2329) AM_READWRITE(timerirqclr_r, timerirq_w)
	AM_RANGE(0x232A, 0x232D) AM_WRITE(unknown_w)
	AM_RANGE(0x232E, 0x232E) AM_READ(irqstatus_r)

	AM_RANGE(0x232F, 0x232F) AM_WRITE(coininhib_w)
	AM_RANGE(0x2330, 0x2330) AM_WRITE(payout_latch_w)
	AM_RANGE(0x2331, 0x2331) AM_WRITE(payout_triac_w)
	AM_RANGE(0x2332, 0x2332) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x2333, 0x2333) AM_WRITE(mmtr_w)
	AM_RANGE(0x2334, 0x2335) AM_WRITE(unknown_w)
	AM_RANGE(0x2336, 0x2336) AM_WRITE(dimcnt_w)
	AM_RANGE(0x2337, 0x2337) AM_WRITE(volume_override_w)
	AM_RANGE(0x2338, 0x2338) AM_WRITE(payout_select_w)
	AM_RANGE(0x2339, 0x2339) AM_WRITE(unknown_w)
	AM_RANGE(0x2400, 0x2400) AM_READWRITE(uart1stat_r, uart1ctrl_w)	/* mc6850 compatible uart */
	AM_RANGE(0x2500, 0x2500) AM_READWRITE(uart1data_r, uart1data_w)
	AM_RANGE(0x2600, 0x2600) AM_READWRITE(uart2stat_r, uart2ctrl_w)	/* mc6850 compatible uart */
	AM_RANGE(0x2700, 0x2700) AM_READWRITE(uart2data_r, uart2data_w)
	AM_RANGE(0x2800, 0x2800) AM_WRITE(vfd1_data_w)					/* vfd1 data */
	AM_RANGE(0x2900, 0x2900) AM_WRITE(vfd_reset_w)					/* vfd1+vfd2 reset line */
	AM_RANGE(0x2A00, 0x2AFF) AM_DEVWRITE("upd", nec_latch_w)
	AM_RANGE(0x2B00, 0x2BFF) AM_DEVWRITE("upd", nec_reset_w)
	AM_RANGE(0x2C00, 0x2C00) AM_WRITE(unlock_w)						/* custom chip unlock */
	AM_RANGE(0x2D00, 0x2D01) AM_DEVWRITE("ymsnd", ym2413_w)
	AM_RANGE(0x2E00, 0x2E00) AM_WRITE(bankswitch_w)					/* write bank (rom page select for 0x6000 - 0x7fff ) */
	AM_RANGE(0x2F00, 0x2F00) AM_WRITE(vfd2_data_w)					/* vfd2 data */

	AM_RANGE(0x3FFE, 0x3FFE) AM_READ( direct_input_r )
	AM_RANGE(0x3FFF, 0x3FFF) AM_READ( coin_input_r)
	AM_RANGE(0x4000, 0x5FFF) AM_ROM
	AM_RANGE(0x4000, 0xFFFF) AM_WRITE(unknown_w)			// contains unknown I/O registers
	AM_RANGE(0x6000, 0x7FFF) AM_ROMBANK("bank1")
	AM_RANGE(0x8000, 0xFFFF) AM_ROM
ADDRESS_MAP_END

// memory map for scorpion2 board video addon /////////////////////////////

static ADDRESS_MAP_START( memmap_vid, AS_PROGRAM, 8 )
	AM_IMPORT_FROM( sc2_basemap )

	AM_RANGE(0x2000, 0x2000) AM_READ(vfd_status_hop_r)		// vfd status register
	AM_RANGE(0x2000, 0x20FF) AM_WRITE(reel12_vid_w)
	AM_RANGE(0x2100, 0x21FF) AM_WRITENOP
	AM_RANGE(0x2200, 0x22FF) AM_WRITENOP

	AM_RANGE(0x3C00, 0x3C07) AM_READ(  key_r   )
	AM_RANGE(0x3C80, 0x3C80) AM_WRITE( e2ram_w )

	AM_RANGE(0x3E00, 0x3E00) AM_READWRITE(vid_uart_ctrl_r, vid_uart_ctrl_w)		// video uart control reg
	AM_RANGE(0x3E01, 0x3E01) AM_READWRITE(vid_uart_rx_r, vid_uart_tx_w)			// video uart data  reg
ADDRESS_MAP_END

// input ports for pyramid ////////////////////////////////////////

static INPUT_PORTS_START( pyramid )
	PORT_START("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Fl 5.00")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Fl 2.50")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("Fl 1.00")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.50")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Left")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Up")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Right")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Enter") PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Collect") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER)     PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "Coin 1 Lockout")PORT_DIPLOCATION("DIL:!02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Coin 2 Lockout")PORT_DIPLOCATION("DIL:!03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Coin 3 Lockout")PORT_DIPLOCATION("DIL:!04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Coin 4 Lockout")PORT_DIPLOCATION("DIL:!05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )

	PORT_START("STROBE10")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!06" )
	PORT_DIPNAME( 0x02, 0x00, "Attract mode language" ) PORT_DIPLOCATION("DIL:!07")
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x02, "Dutch"       )
	PORT_DIPNAME( 0x0C, 0x00, "Skill Level" ) PORT_DIPLOCATION("DIL:!08,!10")
	PORT_DIPSETTING(    0x00, DEF_STR( Low ) )
	PORT_DIPSETTING(    0x04, "Medium-Low" )
	PORT_DIPSETTING(    0x08, "Medium-High")
	PORT_DIPSETTING(    0x0C, DEF_STR( High ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x00, "DIL:!11" )

	PORT_START("STROBE11")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!12" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!13" )
	PORT_DIPNAME( 0x04, 0x00, "Attract mode" ) PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x18, 0x00, "Stake" ) PORT_DIPLOCATION("DIL:!15,!16")
	PORT_DIPSETTING(    0x00, "4 credits per game"  )
	PORT_DIPSETTING(    0x08, "1 credit  per round" )
	PORT_DIPSETTING(    0x10, "2 credit  per round" )
	PORT_DIPSETTING(    0x18, "4 credits per round" )
INPUT_PORTS_END

// input ports for golden crown ///////////////////////////////////

static INPUT_PORTS_START( gldncrwn )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.25")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER)   PORT_NAME( "Collect") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME( "Reel 1" )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME( "Reel 2" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME( "Reel 3" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME( "Reel 4" )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME( "Reel 5" )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME( "Reel 6" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME( "Hall Of Fame" ) PORT_CODE( KEYCODE_J )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_MODIFY("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Attract mode language" )PORT_DIPLOCATION("DIL:!06")
	PORT_DIPSETTING(    0x00, "Dutch")
	PORT_DIPSETTING(    0x01, DEF_STR( English ) )
	PORT_DIPNAME( 0x02, 0x00, "Max number of spins" )PORT_DIPLOCATION("DIL:!07")
	PORT_DIPSETTING(    0x00, "99")
	PORT_DIPSETTING(    0x02, "50")
	PORT_DIPNAME( 0x0C, 0x00, "Skill Level" )PORT_DIPLOCATION("DIL:!08,!10")
	PORT_DIPSETTING(    0x00, DEF_STR( Low ))
	PORT_DIPSETTING(    0x04, "Medium-Low"  )
	PORT_DIPSETTING(    0x08, "Medium-High" )
	PORT_DIPSETTING(    0x0C, DEF_STR( High ) )
	PORT_DIPNAME( 0x10, 0x00, "Base Pricing on:" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, "Full Game")
	PORT_DIPSETTING(    0x10, "Individual Rounds")

	PORT_MODIFY("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "Credits required:" )PORT_DIPLOCATION("DIL:!12")
	PORT_DIPSETTING(    0x00, "4 credits per game")PORT_CONDITION("STROBE10",0x10,PORTCOND_EQUALS,0x00)
	PORT_DIPSETTING(    0x01, "2 credits per game")PORT_CONDITION("STROBE10",0x10,PORTCOND_EQUALS,0x00)
	PORT_DIPSETTING(    0x00, "1 credit  per round")PORT_CONDITION("STROBE10",0x10,PORTCOND_EQUALS,0x10)
	PORT_DIPSETTING(    0x01, "4 credits per round")PORT_CONDITION("STROBE10",0x10,PORTCOND_EQUALS,0x10)
	PORT_DIPNAME( 0x02, 0x00, "Attract Mode" )PORT_DIPLOCATION("DIL:!13")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "Time bar" )PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On  ) )
	PORT_DIPNAME( 0x18, 0x00, "Time bar speed" )PORT_DIPLOCATION("DIL:!15,!16")
	PORT_DIPSETTING(    0x00, "1 (fast)" )
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x10, "3" )
	PORT_DIPSETTING(    0x18, "4 (slow)" )
INPUT_PORTS_END

// input ports for dutch quintoon /////////////////////////////////

static INPUT_PORTS_START( qntoond )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Fl 5.00")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Fl 2.50")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("Fl 1.00")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.50")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER)	  PORT_NAME("Collect") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Hand 1" )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Hand 2" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Hand 3" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Hand 4" )

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Hand 5" )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_MODIFY("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x1e, 0x1c, DEF_STR( Coinage ) ) PORT_DIPLOCATION("DIL:!02,!03,!04,!05")
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x02, "C1=2.5 C2=1.25 C3=0.5 C4=2C_0.25C" )
	PORT_DIPSETTING(    0x04, "C1=10 C2=5 C3=2 C4=0.5" )
	PORT_DIPSETTING(    0x06, "C1=1.5/3.25/5 C2=0.75/1.5/3.25 C3=0.25/0.5/1 C4=3C_0.25C" )
	PORT_DIPSETTING(    0x08, "C1=20 C2=10 C3=4 C4=1" )
	PORT_DIPSETTING(    0x0a, "C1=2 C2=1 C3=0.25/0.75/1/1.5/2 C4=3C_0.25C 5C_0.5C" )
	PORT_DIPSETTING(    0x0c, "C1=5 C2=2.5 C3=1 C4=0.25" )
	PORT_DIPSETTING(    0x0e, "C1=1.25 C2=0.5/1.25/1.75 C3=0.25 C4=0.25" )
	//PORT_DIPSETTING(    0x10, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x12, "C1=3 C2=1.5 C3=0.5 C4=0.25" )
	PORT_DIPSETTING(    0x14, "C1=12 C2=6 C3=2 C4=0.5" )
	PORT_DIPSETTING(    0x16, "C1=2 C2=1 C3=0.25 C4=3C_0.25C" )
	PORT_DIPSETTING(    0x18, "C1=24 C2=12 C3=4 C4=1" )
	PORT_DIPSETTING(    0x1a, "C1=2.25/4.75 C2=1/2.25/3.5/4.75/6 C3=0.25/0.75/1/1.5/2 C4=3C_0.25C 5C_0.5C" )
	PORT_DIPSETTING(    0x1c, "C1=6 C2=3 C3=1 C4=0.25" )
	PORT_DIPSETTING(    0x1e, "C1=1.5 C2=0.75 C3=0.25 C4=4C_0.25C" )

	PORT_MODIFY("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Coin 1 Lockout")PORT_DIPLOCATION("DIL:!06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "Coin 2 Lockout")PORT_DIPLOCATION("DIL:!07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Coin 3 Lockout")PORT_DIPLOCATION("DIL:!08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Coin 4 Lockout")PORT_DIPLOCATION("DIL:!10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Coin Jam Alarm" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_MODIFY("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "Time bar" )PORT_DIPLOCATION("DIL:!12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "Clear credits on reset" )PORT_DIPLOCATION("DIL:!13")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Yes  ) )
	PORT_DIPNAME( 0x04, 0x00, "Attract mode" )PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "Attract mode language" )PORT_DIPLOCATION("DIL:!15")
	PORT_DIPSETTING(    0x00, DEF_STR( English  ) )
	PORT_DIPSETTING(    0x08, "Dutch"    )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x00, "DIL:!16" )
INPUT_PORTS_END

// input ports for UK quintoon ////////////////////////////////////////////

static INPUT_PORTS_START( quintoon )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("10p")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("20p")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("50p")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("GBP 1.00")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER)   PORT_NAME("Collect") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Hand 1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hand 2")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hand 3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hand 4")

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Hand 5")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON7) PORT_NAME("?1") PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON8) PORT_NAME("?2") PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON9) PORT_NAME("?3") PORT_CODE(KEYCODE_O)

	PORT_MODIFY("STROBE5")
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_SPECIAL) //Payout opto

	PORT_MODIFY("STROBE9")
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!02" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "DIL:!03" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x00, "DIL:!04" )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x00, "DIL:!05" )

	PORT_MODIFY("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Coin Lockout")PORT_DIPLOCATION("DIL:!06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) ) //Will activate coin lockout when Credit >= 1 Play
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!07" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "DIL:!08" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x00, "DIL:!10" )
	PORT_DIPNAME( 0x10, 0x00, "Stake per Game / Jackpot" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, "20p / 6 Pounds" )
	PORT_DIPSETTING(    0x10, "50p / 20 Pounds" )

	PORT_MODIFY("STROBE11")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!12" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!13" )
	PORT_DIPNAME( 0x1C, 0x00, "Target percentage" )PORT_DIPLOCATION("DIL:!14,!15,!16")
	PORT_DIPSETTING(    0x1C, "50%")
	PORT_DIPSETTING(    0x0C, "55%")
	PORT_DIPSETTING(    0x08, "60%")
	PORT_DIPSETTING(    0x18, "65%")
	PORT_DIPSETTING(    0x10, "70%")
	PORT_DIPSETTING(    0x00, "75%")
	PORT_DIPSETTING(    0x04, "80%")
	PORT_DIPSETTING(    0x14, "85%")
INPUT_PORTS_END

// input ports for slotsnl  ///////////////////////////////////////////////

static INPUT_PORTS_START( slotsnl )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.25")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Fl 1.00")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("Fl 2.50")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 5.00")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Slot 1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Slot 2")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_NAME("Slot 3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_NAME("Slot 4")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )   PORT_NAME("Enter") PORT_CODE( KEYCODE_E )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_MODIFY("STROBE3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1)

	PORT_MODIFY("STROBE10")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!06" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!07" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "DIL:!08" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x00, "DIL:!10" )
	PORT_DIPNAME( 0x10, 0x00, "Coin Jam Alarm" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_MODIFY("STROBE11")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!12" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!13" )
	PORT_DIPNAME( 0x04, 0x00, "Attract mode" )PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x18, 0x00, "Timebar speed" )PORT_DIPLOCATION("DIL:!15,!16")
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x08, "3" )
	PORT_DIPSETTING(    0x18, "4" )
INPUT_PORTS_END

// input ports for sltblgtk  //////////////////////////////////////////////

static INPUT_PORTS_START( sltblgtk )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Token")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("20 BFr")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("50 BFr")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Slot 1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Slot 2")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Slot 3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Slot 4")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER  ) PORT_NAME("Enter") PORT_CODE( KEYCODE_E )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_MODIFY("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_SPECIAL ) //Tube 1
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_SPECIAL ) //Tube 2
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1)

	PORT_MODIFY("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "CashMeters in refill menu" )PORT_DIPLOCATION("DIL:!02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "Token Lockout" )PORT_DIPLOCATION("DIL:!03")
	PORT_DIPSETTING(    0x00, DEF_STR( No  ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x00, "20 Bfr Lockout" )PORT_DIPLOCATION("DIL:!04")
	PORT_DIPSETTING(    0x00, DEF_STR( No  ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x10, 0x00, "50 Bfr Lockout" )PORT_DIPLOCATION("DIL:!05")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes  ) )

	PORT_MODIFY("STROBE10")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!06" )
	PORT_DIPNAME( 0x0E, 0x00, "Payout Percentage" )PORT_DIPLOCATION("DIL:!07,!08,!10")
	PORT_DIPSETTING(    0x00, "60%")
	PORT_DIPSETTING(    0x08, "65%")
	PORT_DIPSETTING(    0x04, "70%")
	PORT_DIPSETTING(    0x0C, "75%")
	PORT_DIPSETTING(    0x02, "80%")
	PORT_DIPSETTING(    0x0A, "84%")
	PORT_DIPSETTING(    0x06, "88%")
	PORT_DIPSETTING(    0x0E, "90%")
	PORT_DIPNAME( 0x10, 0x00, "Coin Jam Alarm" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_MODIFY("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "Timebar" )PORT_DIPLOCATION("DIL:!12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, "Clear credits" )PORT_DIPLOCATION("DIL:!13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Attract mode" )PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off  ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On   ) )
	PORT_DIPNAME( 0x08, 0x00, "Show hints" )PORT_DIPLOCATION("DIL:!15")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "Pay win to credits" )PORT_DIPLOCATION("DIL:!16")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
INPUT_PORTS_END

// input ports for sltblgpo  //////////////////////////////////////////////

static INPUT_PORTS_START( sltblgpo )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Bfr 20")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Bfr 50")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Hand 1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hand 2")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3)  PORT_NAME("Hand 3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON4)  PORT_NAME("Hand 4")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)  PORT_NAME("Stake")  PORT_CODE( KEYCODE_O )

	PORT_MODIFY("STROBE3")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Collect") PORT_CODE(KEYCODE_C)

	PORT_MODIFY("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "Hopper Limit" )PORT_DIPLOCATION("DIL:!02")
	PORT_DIPSETTING(    0x00, "300" )
	PORT_DIPSETTING(    0x02, "500" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "DIL:!03" )
	PORT_DIPNAME( 0x18, 0x00, "Attendant payout" )PORT_DIPLOCATION("DIL:!04,!05")
	PORT_DIPSETTING(    0x00, "1000 Bfr" )
	PORT_DIPSETTING(    0x08, "1250 Bfr" )
	PORT_DIPSETTING(    0x10, "1500 Bfr" )
	PORT_DIPSETTING(    0x18, "1750 Bfr" )

	PORT_MODIFY("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Bfr 20 Lockout" )PORT_DIPLOCATION("DIL:!06")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Yes ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!07" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "DIL:!08" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x00, "DIL:!10" )
	PORT_DIPNAME( 0x10, 0x00, "Coin Jam Alarm" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_MODIFY("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "Clear credits on reset?" )PORT_DIPLOCATION("DIL:!12")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x02, 0x00, "Attract Mode" )PORT_DIPLOCATION("DIL:!13")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On  ) )
	PORT_DIPNAME( 0x1C, 0x00, "Target Percentage" )PORT_DIPLOCATION("DIL:!14,!15,!16")
	PORT_DIPSETTING(    0x14, "80%")
	PORT_DIPSETTING(    0x04, "82%")
	PORT_DIPSETTING(    0x1C, "84%")
	PORT_DIPSETTING(    0x0C, "86%")
	PORT_DIPSETTING(    0x10, "90%")
	PORT_DIPSETTING(    0x00, "92%")
	PORT_DIPSETTING(    0x18, "94%")
	PORT_DIPSETTING(    0x08, "96%")
INPUT_PORTS_END

// input ports for paradice ///////////////////////////////////////////////

static INPUT_PORTS_START( paradice )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.25")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Fl 1.00")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("Fl 2.50")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 5.00")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 ) PORT_NAME( "1 Player Start (Left)" )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_NAME( "2 Player Start (Right)" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME( "A" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME( "B" )

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME( "C" )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER  ) PORT_NAME( "Enter" ) PORT_CODE( KEYCODE_E )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Joker" )PORT_DIPLOCATION("DIL:!06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Language ) )PORT_DIPLOCATION("DIL:!07")
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x02, "Dutch"    )
	PORT_DIPNAME( 0x0C, 0x00, "Payout level" )PORT_DIPLOCATION("DIL:!08,!10")
	PORT_DIPSETTING(    0x00, DEF_STR( Low ) )
	PORT_DIPSETTING(    0x08, "Medium-Low"  )
	PORT_DIPSETTING(    0x04, "Medium-High" )
	PORT_DIPSETTING(    0x0C, DEF_STR( High ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Difficulty ) )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_MODIFY("STROBE11")
	PORT_DIPNAME( 0x03, 0x00, "Winlines to go" )PORT_DIPLOCATION("DIL:!12,!13")
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x01, "8" )
	PORT_DIPSETTING(    0x03, "9" )
	PORT_DIPNAME( 0x04, 0x00, "Attract mode" )PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x18, 0x00, "Timebar speed" )PORT_DIPLOCATION("DIL:!15,!16")
	PORT_DIPSETTING(    0x00, "0" )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x18, "3" )
INPUT_PORTS_END

// input ports for pokio //////////////////////////////////////////////////

static INPUT_PORTS_START( pokio )
	PORT_INCLUDE( pyramid )

	PORT_MODIFY("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.25")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Fl 1.00")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("Fl 2.50")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 5.00")

	PORT_MODIFY("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME( "Hand 1 Left" )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME( "Hand 2 Left" )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME( "Hand 3 Left" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_MODIFY("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 ) PORT_NAME( "1 Player Start (Left)" )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME( "Enter" ) PORT_CODE( KEYCODE_SPACE )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START2 ) PORT_NAME( "2 Player Start (Right)" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON6 )PORT_NAME( "Hand 3 Right" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5 )PORT_NAME( "Hand 2 Right" )

	PORT_MODIFY("STROBE3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON4 )PORT_NAME( "Hand 1 Right" )

	PORT_MODIFY("STROBE10")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x00, "DIL:!06" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!07" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x00, "DIL:!08" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x00, "DIL:!10" )
	PORT_DIPNAME( 0x10, 0x00, "Coin Jam Alarm" )PORT_DIPLOCATION("DIL:!11")
	PORT_DIPSETTING(    0x10, DEF_STR( Off  ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On   ) )

	PORT_MODIFY("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "Time bar" ) PORT_DIPLOCATION("DIL:!12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x00, "DIL:!13" )
	PORT_DIPNAME( 0x04, 0x00, "Attract mode" )PORT_DIPLOCATION("DIL:!14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x18, 0x00, "Timebar speed" )PORT_DIPLOCATION("DIL:!15,!16")
	PORT_DIPSETTING(    0x00, "0" )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x18, "3" )
INPUT_PORTS_END


///////////////////////////////////////////////////////////////////////////
// machine driver for scorpion2 board + adder2 expansion //////////////////
///////////////////////////////////////////////////////////////////////////

static MACHINE_CONFIG_START( scorpion2_vid, bfm_sc2_state )
	MCFG_MACHINE_RESET( init )							// main scorpion2 board initialisation
	MCFG_QUANTUM_TIME(attotime::from_hz(960))									// needed for serial communication !!
	MCFG_CPU_ADD("maincpu", M6809, MASTER_CLOCK/4 )	// 6809 CPU at 2 Mhz
	MCFG_CPU_PROGRAM_MAP(memmap_vid)					// setup scorpion2 board memorymap
	MCFG_CPU_PERIODIC_INT(timer_irq, 1000)				// generate 1000 IRQ's per second
	MCFG_WATCHDOG_TIME_INIT(PERIOD_OF_555_MONOSTABLE(120000,100e-9))

	MCFG_NVRAM_ADD_0FILL("nvram")
	MCFG_NVRAM_HANDLER(bfm_sc2)
	MCFG_DEFAULT_LAYOUT(layout_bfm_sc2)

	MCFG_SCREEN_ADD("adder", RASTER)
	MCFG_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MCFG_SCREEN_SIZE( 400, 280)
	MCFG_SCREEN_VISIBLE_AREA(  0, 400-1, 0, 280-1)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_UPDATE(addersc2)

	MCFG_VIDEO_START( adder2)
	MCFG_VIDEO_RESET( adder2)

	MCFG_PALETTE_LENGTH(16)
	MCFG_PALETTE_INIT(adder2)
	MCFG_GFXDECODE(adder2)

	MCFG_CPU_ADD("adder2", M6809, MASTER_CLOCK/4 )	// adder2 board 6809 CPU at 2 Mhz
	MCFG_CPU_PROGRAM_MAP(adder2_memmap)				// setup adder2 board memorymap
	MCFG_CPU_VBLANK_INT("adder", adder2_vbl)		// board has a VBL IRQ

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("upd", UPD7759, UPD7759_STANDARD_CLOCK)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MCFG_SOUND_ADD("ymsnd", YM2413, XTAL_3_579545MHz)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END

static void sc2_common_init(running_machine &machine, int decrypt)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	UINT8 *rom;

	if (decrypt) decode_mainrom(machine, "maincpu");		  // decode main rom

	rom = machine.region("maincpu")->base();
	if ( rom )
	{
		memcpy(&rom[0x10000], &rom[0x00000], 0x2000);
	}

	memset(state->m_sc2_Inputs, 0, sizeof(state->m_sc2_Inputs));  // clear all inputs
}

static void adder2_common_init(running_machine &machine)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	UINT8 *pal;

	pal = machine.region("proms")->base();
	if ( pal )
	{
		memcpy(state->m_key, pal, 8);
	}
}

// UK quintoon initialisation ////////////////////////////////////////////////

static DRIVER_INIT (quintoon)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2_common_init(machine, 1);
	adder2_decode_char_roms(machine);
	MechMtr_config(machine,8);					// setup mech meters

	state->m_has_hopper = 0;

	Scorpion2_SetSwitchState(machine,3,0,1);	// tube1 level switch
	Scorpion2_SetSwitchState(machine,3,1,1);	// tube2 level switch
	Scorpion2_SetSwitchState(machine,3,2,1);	// tube3 level switch

	Scorpion2_SetSwitchState(machine,5,2,1);
	Scorpion2_SetSwitchState(machine,6,4,1);

	state->m_sc2_show_door   = 1;
	state->m_sc2_door_state  = 0x41;
}

// dutch pyramid intialisation //////////////////////////////////////////////

static DRIVER_INIT( pyramid )
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2_common_init(machine, 1);
	adder2_decode_char_roms(machine);			// decode GFX roms
	adder2_common_init(machine);

	state->m_has_hopper = 1;

	Scorpion2_SetSwitchState(machine,3,0,1);	// tube1 level switch
	Scorpion2_SetSwitchState(machine,3,1,1);	// tube2 level switch
	Scorpion2_SetSwitchState(machine,3,2,1);	// tube3 level switch

	state->m_sc2_show_door   = 1;
	state->m_sc2_door_state  = 0x41;
}
// belgian slots initialisation /////////////////////////////////////////////

static DRIVER_INIT( sltsbelg )
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2_common_init(machine, 1);
	adder2_decode_char_roms(machine);			// decode GFX roms
	adder2_common_init(machine);

	state->m_has_hopper = 1;

	state->m_sc2_show_door   = 1;
	state->m_sc2_door_state  = 0x41;
}

// other dutch adder games ////////////////////////////////////////////////

static DRIVER_INIT( adder_dutch )
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2_common_init(machine, 1);
	adder2_decode_char_roms(machine);			// decode GFX roms
	adder2_common_init(machine);

	state->m_has_hopper = 0;

	Scorpion2_SetSwitchState(machine,3,0,1);	// tube1 level switch
	Scorpion2_SetSwitchState(machine,3,1,1);	// tube2 level switch
	Scorpion2_SetSwitchState(machine,3,2,1);	// tube3 level switch

	state->m_sc2_show_door   = 1;
	state->m_sc2_door_state  = 0x41;
}

// golden crown //////////////////////////////////////////////////////////

static DRIVER_INIT( gldncrwn )
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2_common_init(machine, 1);
	adder2_decode_char_roms(machine);			// decode GFX roms
	adder2_common_init(machine);

	state->m_has_hopper = 0;

	Scorpion2_SetSwitchState(machine,3,0,1);	// tube1 level switch
	Scorpion2_SetSwitchState(machine,3,1,1);	// tube2 level switch
	Scorpion2_SetSwitchState(machine,3,2,1);	// tube3 level switch

	state->m_sc2_show_door   = 0;
	state->m_sc2_door_state  = 0x41;
}

// ROM definition UK Quintoon ////////////////////////////////////////////

ROM_START( quintoon )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750206.p1",	0x00000, 0x10000,  CRC(05f4bfad) SHA1(22751573f3a51a9fd2d2a75a7d1b20d78112e0bb))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("quinp132",		0x00000, 0x20000,  CRC(63896a7f) SHA1(81aa56874a15faa3aabdfc0fc524b2e25b751f22))

	ROM_REGION( 0x20000, "upd", 0 ) // using Dutch samples, need to check a UK Quintoon PCB
	ROM_LOAD("95001016.snd",	0x00000, 0x20000, BAD_DUMP CRC(cf097d41) SHA1(6712f93896483360256d8baffc05977c8e532ef1))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("quinp233",		0x00000, 0x20000, CRC(3d4ebecf) SHA1(b339cf16797ccf7a1ec20fcebf52b6edad9a1047))
ROM_END

// ROM definition UK Quintoon (older) ////////////////////////////////////

ROM_START( quintono )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750203.bin",	0x00000, 0x10000,  CRC(037ef2d0) SHA1(6958624e29629a7639a80e8929b833a8b0201833))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("quinp132",		0x00000, 0x20000,  CRC(63896a7f) SHA1(81aa56874a15faa3aabdfc0fc524b2e25b751f22))

	ROM_REGION( 0x20000, "upd", 0 ) // using Dutch samples, need to check a UK Quintoon PCB
	ROM_LOAD("95001016.snd",	0x00000, 0x20000, BAD_DUMP CRC(cf097d41) SHA1(6712f93896483360256d8baffc05977c8e532ef1))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("quinp233",		0x00000, 0x20000, CRC(3d4ebecf) SHA1(b339cf16797ccf7a1ec20fcebf52b6edad9a1047))
ROM_END

// ROM definition UK Quintoon (data) /////////////////////////////////////

ROM_START( quintond )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95751206.bin",	0x00000, 0x10000,  CRC(63def707) SHA1(d016df74f4f83cd72b16f9ccbe78cc382bf056c8))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("quinp132",		0x00000, 0x20000,  CRC(63896a7f) SHA1(81aa56874a15faa3aabdfc0fc524b2e25b751f22))

	ROM_REGION( 0x20000, "upd", 0 ) // using Dutch samples, need to check a UK Quintoon PCB
	ROM_LOAD("95001016.snd",	0x00000, 0x20000, BAD_DUMP CRC(cf097d41) SHA1(6712f93896483360256d8baffc05977c8e532ef1))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("quinp233",		0x00000, 0x20000, CRC(3d4ebecf) SHA1(b339cf16797ccf7a1ec20fcebf52b6edad9a1047))
ROM_END

// ROM definition Dutch Quintoon ///////////////////////////////////////////

ROM_START( qntoond )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750243.bin", 0x00000, 0x10000, CRC(36a8dcd1) SHA1(ab21301312fbb6609f850e1cf6bcda5a2b7f66f5))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770024.vid", 0x00000, 0x20000, CRC(5bc7ac55) SHA1(b54e9684f750b73c357d41b88ca8c527258e2a10))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001016.snd", 0x00000, 0x20000, CRC(cf097d41) SHA1(6712f93896483360256d8baffc05977c8e532ef1))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770025.chr", 0x00000, 0x20000, CRC(f59748ea) SHA1(f0f7f914fdf72db8eb60717b95e7d027c0081339))
ROM_END

// ROM definition Dutch Quintoon alternate set /////////////////////////////

ROM_START( qntoondo )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750136.bin", 0x00000, 0x10000, CRC(839ea01d) SHA1(d7f77dbaea4e87c3d782408eb50d10f44b6df5e2))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770024.vid", 0x00000, 0x20000, CRC(5bc7ac55) SHA1(b54e9684f750b73c357d41b88ca8c527258e2a10))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001016.snd", 0x00000, 0x20000, CRC(cf097d41) SHA1(6712f93896483360256d8baffc05977c8e532ef1))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770025.chr", 0x00000, 0x20000, CRC(f59748ea) SHA1(f0f7f914fdf72db8eb60717b95e7d027c0081339))
ROM_END

// ROM definition dutch golden crown //////////////////////////////////////

ROM_START( gldncrwn )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95752011.bin", 0x00000, 0x10000, CRC(54f7cca0) SHA1(835727d88113700a38060f880b4dfba2ded41487))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770117.vid", 0x00000, 0x20000, CRC(598ba7cb) SHA1(ab518d7df24b0b453ec3fcddfc4db63e0391fde7))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001039.snd", 0x00000, 0x20000, CRC(6af26157) SHA1(9b3a85f5dd760c4430e38e2844928b74aadc7e75))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770118.ch1", 0x00000, 0x20000, CRC(9c9ac946) SHA1(9a571e7d00f6654242aface032c2fb186ef44aba))
	ROM_LOAD("95770119.ch2", 0x20000, 0x20000, CRC(9e0fdb2e) SHA1(05e8257285b0009df4fcc73e93490876358a8be8))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD("gcrpal.bin", 0, 8 , CRC(4edd5a1d) SHA1(d6fe38377d5f2291d33ee8ed808548871e63c4d7))
ROM_END

// ROM definition Dutch Paradice //////////////////////////////////////////

ROM_START( paradice )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750615.bin", 0x00000, 0x10000, CRC(f51192e5) SHA1(a1290e32bba698006e83fd8d6075202586232929))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770084.vid", 0x00000, 0x20000, CRC(8f27bd34) SHA1(fccf7283b5c952b74258ee6e5138c1ca89384e24))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001037.snd", 0x00000, 0x20000, CRC(82f74276) SHA1(c51c3caeb7bf514ec7a1b452c8effc4c79186062))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770085.ch1", 0x00000, 0x20000, CRC(4d1fb82f) SHA1(054f683d1d7c884911bd2d0f85aab4c59ddf9930))
	ROM_LOAD("95770086.ch2", 0x20000, 0x20000, CRC(7b566e11) SHA1(f34c82ad75a0f88204ac4ae83a00801215c46ca9))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD( "pdcepal.bin", 0, 8 , CRC(64020c97) SHA1(9371841e2df950c1f2e5b5a4b52621beb6f60945))
ROM_END

// ROM definition Dutch Pokio /////////////////////////////////////////////

ROM_START( pokio )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750278.bin", 0x00000, 0x10000, CRC(5124b24d) SHA1(9bc63891a8e9283c2baa64c264a5d6d1625d44b2))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770044.vid", 0x00000, 0x20000, CRC(46d7a6d8) SHA1(01f58e735621661b57c61491b3769ae99e92476a))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001016.snd", 0x00000, 0x20000, CRC(98aaff76) SHA1(4a59cf83daf018d93f1ff7805e06309d2f3d7252))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770045.chr", 0x00000, 0x20000, CRC(dd30da90) SHA1(b4f5a229d88613c0c7d43adf3f325c619abe38a3))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD("pokiopal.bin", 0, 8 , CRC(53535184) SHA1(c5c98085e39ca3671dca72c21a8466d7d70cd341))
ROM_END

// ROM definition pyramid prototype  //////////////////////////////////////

ROM_START( pyramid )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750898.bin", 0x00000, 0x10000,  CRC(3b0df16c) SHA1(9af599fe604f86c72986aa1610d74837852e023f))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770108.vid", 0x00000, 0x20000,  CRC(216ff683) SHA1(227764771600ce88c5f36bed9878e6bb9988ae8f))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001038.snd", 0x00000, 0x20000, CRC(f885c42e) SHA1(4d79fc5ae4c58247740d78d81302bfbb43331c43))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770106.ch1", 0x00000, 0x20000, CRC(a83c27ae) SHA1(f61ca3cdf19a933bae18c1b32a5fb0a2204dde78))
	ROM_LOAD("95770107.ch2", 0x20000, 0x20000, CRC(52e59f64) SHA1(ea4828c2cfb72cd77c92c60560b4d5ee424f7dca))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD("pyrmdpal.bin", 0, 8 , CRC(1c7c37bb) SHA1(fe0276603fee8f58e4318f91645260368212b78b))
ROM_END

// ROM definition Dutch slots /////////////////////////////////////////////

ROM_START( slotsnl )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750368.bin", 0x00000, 0x10000, CRC(3a43048c) SHA1(13728e05b334cba90ea9cc51ea00c4384baa8614))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("video.vid",	 0x00000, 0x20000, CRC(cc760208) SHA1(cc01b1e31335b26f2d0f3470d8624476b153655f))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("95001029.snd", 0x00000, 0x20000, CRC(7749c724) SHA1(a87cce0c99e392f501bba44b3936a7059d682c9c))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("charset.chr",	 0x00000, 0x20000,  CRC(ef4300b6) SHA1(a1f765f38c2f146651fc685ea6195af72465f559))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD( "slotspal.bin", 0, 8 , CRC(ee5421f0) SHA1(21bdcbf11dda8b1a93c49ae1c706954bba53c917))
ROM_END

// ROM definition Belgian Slots (Token pay per round) Payslide ////////////

ROM_START( sltblgtk )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750943.bin", 0x00000, 0x10000, CRC(c9fb8153) SHA1(7c1d0660c15f05b1e0784d8322c62981fe8dc4c9))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("adder121.bin", 0x00000, 0x20000, CRC(cedbbf28) SHA1(559ae341b55462feea771127394a54fc65266818))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("sound029.bin", 0x00000, 0x20000, CRC(7749c724) SHA1(a87cce0c99e392f501bba44b3936a7059d682c9c))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("chr122.bin",	 0x00000, 0x20000, CRC(a1e3bdf4) SHA1(f0cabe08dee028e2014cbf0fc3fe0806cdfa60c6))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD("stsbtpal.bin", 0, 8 , CRC(20e13635) SHA1(5aa7e7cac8c00ebc193d63d0c6795904f42c70fa))
ROM_END

// ROM definition Belgian Slots (Cash Payout) /////////////////////////////

ROM_START( sltblgp1 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95752008.bin", 0x00000, 0x10000, CRC(3167d3b9) SHA1(a28563f65d55c4d47f3e7fdb41e050d8a733b9bd))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("adder142.bin", 0x00000, 0x20000, CRC(a6f6356b) SHA1(b3d3063155ee3ea888273081f844279b6e33f7d9))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("sound033.bin", 0x00000, 0x20000, CRC(bb1dfa55) SHA1(442454fccfe03e6f4c3353551cb7459e184a099d))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("chr143.bin",	 0x00000, 0x20000, CRC(a40e91e2) SHA1(87dc76963ea961fcfbe4f3e25df9162348d39d79))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD("stsbcpal.bin", 0, 8 , CRC(c63bcab6) SHA1(238841165d5b3241b0bcc5c1792e9c0be1fc0177))
ROM_END

// ROM definition Belgian Slots (Cash Payout) /////////////////////////////

ROM_START( sltblgpo )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95770938.bin", 0x00000, 0x10000, CRC(7e802634) SHA1(fecf86e632546649d5e647c42a248b39fc2cf982))

	ROM_REGION( 0x20000, "adder2", 0 )
	ROM_LOAD("95770120.chr", 0x00000, 0x20000, CRC(ad505138) SHA1(67ccd8dc30e76283247ab5a62b22337ebaff74cd))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("sound033.bin", 0x00000, 0x20000, CRC(bb1dfa55) SHA1(442454fccfe03e6f4c3353551cb7459e184a099d))

	ROM_REGION( 0x40000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD("95770110.add", 0x00000, 0x20000, CRC(64b03284) SHA1(4b1c17b75e449c9762bb949d7cde0694a3aaabeb))

	ROM_REGION( 0x10, "proms", 0 )
	ROM_LOAD("stsbcpal.bin", 0, 8 , CRC(c63bcab6) SHA1(238841165d5b3241b0bcc5c1792e9c0be1fc0177))
ROM_END


/**************************************************************************

    Mechanical Scorpion 2 Games
        AGEMAME driver

***************************************************************************

  30-12-2006: J Wallace: Fixed init routines.
  07-03-2006: El Condor: Recoded to more accurately represent the hardware
              setup.
  18-01-2006: Cleaned up for MAME inclusion
  19-08-2005: Re-Animator

***************************************************************************/



///////////////////////////////////////////////////////////////////////////






#ifdef UNUSED_FUNCTION
/* Scorpion 3 expansion */
static READ8_HANDLER( sc3_expansion_r )
{
    int result = 0;

    switch ( offset )
    {
        case 0: result = 0;
        break;
        case 1: result = input_port_read_indexed(machine,0);  /* coin input */
    }

    return result;
}


static WRITE8_HANDLER( sc3_expansion_w )
{
    switch ( offset )
    {
	    case 0:
    	break;
	    case 1:
    	break;
    }
}
#endif


/* machine init (called only once) */
static MACHINE_RESET( awp_init )
{
	on_scorpion2_reset(machine);
	BFM_BD1_init(0);
	BFM_BD1_init(1);
}


static MACHINE_RESET( dm01_init )
{
	on_scorpion2_reset(machine);
	BFM_dm01_reset(machine);
}




#ifdef UNREFERENCED_CODE
static INPUT_PORTS_START( scorpion2 )
	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN5 ) PORT_IMPULSE(3)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN6 ) PORT_IMPULSE(3)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN7 ) PORT_IMPULSE(3)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN8 ) PORT_IMPULSE(3)

	PORT_START("STROBE0")
	PORT_BIT( 0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("I10")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("I11")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("I12")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("I13")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("I14")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("I20")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_NAME("I21")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON8 ) PORT_NAME("I22")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON9 ) PORT_NAME("I23")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON10) PORT_NAME("I24")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED  )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED  )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED  )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )	PORT_NAME("I30")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )	PORT_NAME("I31")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )	PORT_NAME("I32")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )	PORT_NAME("I33")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )	PORT_NAME("I34")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK)	PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_INTERLOCK)	PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )		PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )		PORT_NAME("I43")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )		PORT_NAME("I44")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I50")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I51")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I52")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I53")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I54")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I60")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I61")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I62")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I63")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I64")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I70")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I71")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I72")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I73")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I74")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I80")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I81")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I82")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I83")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("I84")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL03" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL04" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "DIL06" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL07" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL10" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL14" ) PORT_DIPLOCATION("DIL:14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL15" ) PORT_DIPLOCATION("DIL:15")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL16" ) PORT_DIPLOCATION("DIL:16")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
INPUT_PORTS_END
#endif

static INPUT_PORTS_START( bbrkfst )
	PORT_START("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("10p")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("20p")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("50p")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("GBP 1.00")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN5 ) PORT_IMPULSE(3) PORT_NAME("Token")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE0")
	PORT_BIT( 0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Cancel")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hold 1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hold 2/Hi")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold 3/Lo")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Stop/Collect")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Exchange")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START1  )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_NAME("Take Big Breakfast")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON8 ) PORT_NAME("Take Feature")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED  )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED  )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED  )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK)	PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_INTERLOCK)	PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )		PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL03" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL04" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "DIL06" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL07" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL10" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL14" ) PORT_DIPLOCATION("DIL:14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL15" ) PORT_DIPLOCATION("DIL:15")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL16" ) PORT_DIPLOCATION("DIL:16")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
INPUT_PORTS_END

static INPUT_PORTS_START( drwho )
	PORT_START("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("10p")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("20p")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("50p")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("GBP 1.00")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN5 ) PORT_IMPULSE(3) PORT_NAME("Token")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Cancel")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hold 1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hold 2/Hi")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold 3/Lo")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Stop/Collect")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME("Exchange")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK)PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_INTERLOCK)PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER  )  PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status Low switch for 1 Pound*/
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status Low switch for 20p*/
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status Low switch for Token Front*/
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status Low switch for Token Rear*/
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_DIPNAME( 0x07, 0x07, "PERCENTAGE KEY" ) PORT_DIPLOCATION("STROBE6:01,02,03")/*Certain combinations give different percentages*/
	PORT_DIPSETTING(    0x00, "No key") /*Some day, I'll work all these values out.*/
	PORT_DIPSETTING(    0x01, "Key 1" )
	PORT_DIPSETTING(    0x02, "Key 2" )
	PORT_DIPSETTING(    0x03, "Key 3" )
	PORT_DIPSETTING(    0x04, "Key 4" )
	PORT_DIPSETTING(    0x05, "Key 5" )
	PORT_DIPSETTING(    0x06, "Key 6" )
	PORT_DIPSETTING(    0x07, "Key 7" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status High switch for 1 Pound*/
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status High switch for 20p*/
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status High switch for Token Front*/
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SPECIAL )/*Tube status High switch for Token Rear*/
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL03" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL04" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "DIL06" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL07" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL10" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL14" ) PORT_DIPLOCATION("DIL:14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL15" ) PORT_DIPLOCATION("DIL:15")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL16" ) PORT_DIPLOCATION("DIL:16")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
INPUT_PORTS_END

static INPUT_PORTS_START( cpeno1 )
	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN5 ) PORT_IMPULSE(3)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN6 ) PORT_IMPULSE(3)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN7 ) PORT_IMPULSE(3)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN8 ) PORT_IMPULSE(3)

	PORT_START("STROBE0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Hold 1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hold 1?")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hold 2/Hi")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold 3/Lo")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Cancel/Collect")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME("Stop/Exchange")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_INTERLOCK) PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER  )   PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_DIPNAME( 0x07, 0x07, "PERCENTAGE KEY" ) PORT_DIPLOCATION("STROBE6:01,02,03")
	PORT_DIPSETTING(    0x00, "No key" )
	PORT_DIPSETTING(    0x01, "Key 1" )
	PORT_DIPSETTING(    0x02, "Key 2" )
	PORT_DIPSETTING(    0x03, "Key 3" )
	PORT_DIPSETTING(    0x04, "Key 4" )
	PORT_DIPSETTING(    0x05, "Key 5" )
	PORT_DIPSETTING(    0x06, "Key 6" )
	PORT_DIPSETTING(    0x07, "Key 7" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "Attract Hi/Lo reel" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "Acceptor type" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, "Mars" )
	PORT_DIPSETTING(    0x08, "Sentinel" )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Coin play" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, "Multi coin" )
	PORT_DIPSETTING(    0x01, "Single coin" )
	PORT_DIPNAME( 0x02, 0x00, "CashPot Freq" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( High) )
	PORT_DIPSETTING(    0x02, DEF_STR( Low ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "Coin Jam Alarm" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x1C, 0x00, "Percentage setting" ) PORT_DIPLOCATION("DIL:14,15,16")
	PORT_DIPSETTING(    0x0C, "72%" )
	PORT_DIPSETTING(    0x04, "75%" )
	PORT_DIPSETTING(    0x00, "78%" )
	PORT_DIPSETTING(    0x10, "81%" )
	PORT_DIPSETTING(    0x18, "85%" )
INPUT_PORTS_END

static INPUT_PORTS_START( luvjub )
	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN5 ) PORT_IMPULSE(3)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN6 ) PORT_IMPULSE(3)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN7 ) PORT_IMPULSE(3)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN8 ) PORT_IMPULSE(3)

	PORT_START("STROBE0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Cancel")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hold1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hold2")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Stop")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME("Take win")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON7) PORT_NAME( "Yes!" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON8) PORT_NAME( "No!" )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER  )   PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON9)   PORT_NAME("Answer the phone")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_DIPNAME( 0x07, 0x07, "PERCENTAGE KEY" ) PORT_DIPLOCATION("STROBE6:01,02,03")
	PORT_DIPSETTING(    0x00, "No key" )
	PORT_DIPSETTING(    0x01, "Key 1" )
	PORT_DIPSETTING(    0x02, "Key 2" )
	PORT_DIPSETTING(    0x03, "Key 3" )
	PORT_DIPSETTING(    0x04, "Key 4" )
	PORT_DIPSETTING(    0x05, "Key 5" )
	PORT_DIPSETTING(    0x06, "Key 6" )
	PORT_DIPSETTING(    0x07, "Key 7" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL03" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL04" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "DIL06" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL07" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL10" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL14" ) PORT_DIPLOCATION("DIL:14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL15" ) PORT_DIPLOCATION("DIL:15")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL16" ) PORT_DIPLOCATION("DIL:16")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
INPUT_PORTS_END

static INPUT_PORTS_START( bfmcgslm )
	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_COIN5 ) PORT_IMPULSE(3)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN6 ) PORT_IMPULSE(3)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN7 ) PORT_IMPULSE(3)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN8 ) PORT_IMPULSE(3)

	PORT_START("STROBE0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Cancel")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hold 1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hold 2")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold 3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Stop")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME("Exchange")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER)     PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_DIPNAME( 0x07, 0x07, "PERCENTAGE KEY" ) PORT_DIPLOCATION("STROBE6:01,02,03")
	PORT_DIPSETTING(    0x00, "No key" )
	PORT_DIPSETTING(    0x01, "Key 1" )
	PORT_DIPSETTING(    0x02, "Key 2" )
	PORT_DIPSETTING(    0x03, "Key 3" )
	PORT_DIPSETTING(    0x04, "Key 4" )
	PORT_DIPSETTING(    0x05, "Key 5" )
	PORT_DIPSETTING(    0x06, "Key 6" )
	PORT_DIPSETTING(    0x07, "Key 7" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL03" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL04" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "DIL06" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL07" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL10" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL14" ) PORT_DIPLOCATION("DIL:14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL15" ) PORT_DIPLOCATION("DIL:15")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL16" ) PORT_DIPLOCATION("DIL:16")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
INPUT_PORTS_END

static INPUT_PORTS_START( scorpion3 )
	PORT_START("COINS")
    PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(3) PORT_NAME("Fl 5.00")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(3) PORT_NAME("Fl 2.50")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 ) PORT_IMPULSE(3) PORT_NAME("Fl 1.00")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_COIN4 ) PORT_IMPULSE(3) PORT_NAME("Fl 0.25")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Hold 1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Hold 1?")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Hold 2/Hi")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold 3/Lo")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Cancel/Collect")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME("Stop/Exchange")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_INTERLOCK) PORT_NAME("Cashbox Door") PORT_CODE(KEYCODE_Q) PORT_TOGGLE
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_INTERLOCK) PORT_NAME("Front Door") PORT_CODE(KEYCODE_W) PORT_TOGGLE
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER  )   PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE6")
	PORT_DIPNAME( 0x07, 0x07, "PERCENTAGE KEY" ) PORT_DIPLOCATION("STROBE6:01,02,03")
	PORT_DIPSETTING(    0x00, "No key" )
	PORT_DIPSETTING(    0x01, "Key 1" )
	PORT_DIPSETTING(    0x02, "Key 2" )
	PORT_DIPSETTING(    0x03, "Key 3" )
	PORT_DIPSETTING(    0x04, "Key 4" )
	PORT_DIPSETTING(    0x05, "Key 5" )
	PORT_DIPSETTING(    0x06, "Key 6" )
	PORT_DIPSETTING(    0x07, "Key 7" )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("STROBE7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("STROBE9")
	PORT_SERVICE_NO_TOGGLE(0x01,IP_ACTIVE_HIGH)
	PORT_DIPNAME( 0x02, 0x00, "DIL02" ) PORT_DIPLOCATION("DIL:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "Attract Hi/Lo reel" ) PORT_DIPLOCATION("DIL:03")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "Acceptor type" ) PORT_DIPLOCATION("DIL:04")
	PORT_DIPSETTING(    0x00, "Mars" )
	PORT_DIPSETTING(    0x08, "Sentinel" )
	PORT_DIPNAME( 0x10, 0x00, "DIL05" ) PORT_DIPLOCATION("DIL:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE10")
	PORT_DIPNAME( 0x01, 0x00, "Coin play" ) PORT_DIPLOCATION("DIL:06")
	PORT_DIPSETTING(    0x00, "Multi coin" )
	PORT_DIPSETTING(    0x01, "Single coin" )
	PORT_DIPNAME( 0x02, 0x00, "CashPot Freq" ) PORT_DIPLOCATION("DIL:07")
	PORT_DIPSETTING(    0x00, DEF_STR( High) )
	PORT_DIPSETTING(    0x02, DEF_STR( Low ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL08" ) PORT_DIPLOCATION("DIL:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "Coin Jam Alarm" ) PORT_DIPLOCATION("DIL:10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL11" ) PORT_DIPLOCATION("DIL:11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )

	PORT_START("STROBE11")
	PORT_DIPNAME( 0x01, 0x00, "DIL12" ) PORT_DIPLOCATION("DIL:12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL13" ) PORT_DIPLOCATION("DIL:13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x1C, 0x00, "Percentage setting" ) PORT_DIPLOCATION("DIL:14,15,16")
	PORT_DIPSETTING(    0x0C, "72%" )
	PORT_DIPSETTING(    0x04, "75%" )
	PORT_DIPSETTING(    0x00, "78%" )
	PORT_DIPSETTING(    0x10, "81%" )
	PORT_DIPSETTING(    0x18, "85%" )

INPUT_PORTS_END


/* machine driver for scorpion2 board */

static MACHINE_CONFIG_START( scorpion2, bfm_sc2_state )
	MCFG_MACHINE_RESET(awp_init)
	MCFG_CPU_ADD("maincpu", M6809, MASTER_CLOCK/4 )
	MCFG_CPU_PROGRAM_MAP(sc2_basemap)
	MCFG_CPU_PERIODIC_INT(timer_irq, 1000 )
	MCFG_WATCHDOG_TIME_INIT(PERIOD_OF_555_MONOSTABLE(120000,100e-9))

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("upd",UPD7759, UPD7759_STANDARD_CLOCK)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MCFG_SOUND_ADD("ymsnd",YM2413, XTAL_3_579545MHz)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MCFG_NVRAM_ADD_0FILL("nvram")
	MCFG_NVRAM_HANDLER(bfm_sc2)

	/* video hardware */
	MCFG_DEFAULT_LAYOUT(layout_awpvid14)
MACHINE_CONFIG_END


/* machine driver for scorpion3 board */
static MACHINE_CONFIG_DERIVED( scorpion3, scorpion2 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(sc2_basemap)
MACHINE_CONFIG_END


/* machine driver for scorpion2 board + matrix board */
static MACHINE_CONFIG_START( scorpion2_dm01, bfm_sc2_state )
	MCFG_MACHINE_RESET(dm01_init)
	MCFG_QUANTUM_TIME(attotime::from_hz(960))									// needed for serial communication !!
	MCFG_CPU_ADD("maincpu", M6809, MASTER_CLOCK/4 )
	MCFG_CPU_PROGRAM_MAP(sc2_basemap)
	MCFG_CPU_PERIODIC_INT(timer_irq, 1000 )
	MCFG_WATCHDOG_TIME_INIT(PERIOD_OF_555_MONOSTABLE(120000,100e-9))


	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("ymsnd",YM2413, XTAL_3_579545MHz)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

	MCFG_SOUND_ADD("upd",UPD7759, UPD7759_STANDARD_CLOCK)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MCFG_NVRAM_ADD_0FILL("nvram")
	MCFG_NVRAM_HANDLER(bfm_sc2)

	/* video hardware */
	MCFG_DEFAULT_LAYOUT(layout_awpdmd)
	MCFG_CPU_ADD("matrix", M6809, 2000000 )				/* matrix board 6809 CPU at 2 Mhz ?? I don't know the exact freq.*/
	MCFG_CPU_PROGRAM_MAP(bfm_dm01_memmap)
	MCFG_CPU_PERIODIC_INT(bfm_dm01_vbl, 1500 )			/* generate 1500 NMI's per second ?? what is the exact freq?? */
MACHINE_CONFIG_END

static void sc2awp_common_init(running_machine &machine,int reels, int decrypt)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();

	int n;
	sc2_common_init(machine, decrypt);
	/* setup n default 96 half step reels */

	state->m_reels=reels;

	for ( n = 0; n < reels; n++ )
	{
		stepper_config(machine, n, &starpoint_interface_48step);
	}
	if (reels)
	{
		awp_reel_setup();
	}
}

static DRIVER_INIT (bbrkfst)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2awp_common_init(machine,5, 1);
	MechMtr_config(machine,8);

	BFM_BD1_init(0);
	BFM_BD1_init(1);

	state->m_has_hopper = 0;

	Scorpion2_SetSwitchState(machine,4,0, 1);	  /* GBP1 Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,1, 1);	  /* 20p Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,2, 1);	  /* Token Front Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,3, 1);	  /* Token Rear  Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,4, 1);
	Scorpion2_SetSwitchState(machine,6,0, 0);
	Scorpion2_SetSwitchState(machine,6,1, 1);
	Scorpion2_SetSwitchState(machine,6,2, 0);
	Scorpion2_SetSwitchState(machine,6,3, 1);

}

static DRIVER_INIT (drwho_common)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();

	MechMtr_config(machine,8);

	BFM_BD1_init(0);
	BFM_BD1_init(1);

	state->m_has_hopper = 0;

	Scorpion2_SetSwitchState(machine,4,0, 0);	  /* GBP1 Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,1, 0);	  /* 20p Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,2, 0);	  /* Token Front Low Level Switch */
	Scorpion2_SetSwitchState(machine,4,3, 0);	  /* Token Rear  Low Level Switch */
	Scorpion2_SetSwitchState(machine,7,0, 0);	  /* GBP1 High Level Switch */
	Scorpion2_SetSwitchState(machine,7,1, 0);	  /* 20P High Level Switch */
	Scorpion2_SetSwitchState(machine,7,2, 0);	  /* Token Front High Level Switch */
	Scorpion2_SetSwitchState(machine,7,3, 0);	  /* Token Rear High Level Switch */
}

static DRIVER_INIT (drwho)
{
	sc2awp_common_init(machine,4, 1);
	DRIVER_INIT_CALL(drwho_common);
}

static DRIVER_INIT (drwhon)
{
	sc2awp_common_init(machine,4, 0);
	DRIVER_INIT_CALL(drwho_common);
}


static DRIVER_INIT (focus)
{
	sc2awp_common_init(machine,6, 1);
	MechMtr_config(machine,5);

	BFM_BD1_init(0);
}

static DRIVER_INIT (cpeno1)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2awp_common_init(machine,6, 1);

	MechMtr_config(machine,5);

	Scorpion2_SetSwitchState(machine,3,3,1);	/*  5p play */
	Scorpion2_SetSwitchState(machine,3,4,1);	/* 20p play */

	Scorpion2_SetSwitchState(machine,4,0,1);	/* pay tube low (1 pound front) */
	Scorpion2_SetSwitchState(machine,4,1,1);	/* pay tube low (20p) */
	Scorpion2_SetSwitchState(machine,4,2,1);	/* pay tube low (?1 right) */
	Scorpion2_SetSwitchState(machine,4,3,1);	/* pay tube low (?1 left) */

	Scorpion2_SetSwitchState(machine,5,0,1);	/* pay sensor (GBP1 front) */
	Scorpion2_SetSwitchState(machine,5,1,1);	/* pay sensor (20 p) */
	Scorpion2_SetSwitchState(machine,5,2,1);	/* pay sensor (1 right) */
	Scorpion2_SetSwitchState(machine,5,3,1);	/* pay sensor (?1 left) */
	Scorpion2_SetSwitchState(machine,5,4,1);	/* payout unit present */

	state->m_slide_pay_sensor[0] = 0x50;
	state->m_slide_pay_sensor[1] = 0x51;
	state->m_slide_pay_sensor[2] = 0x52;
	state->m_slide_pay_sensor[3] = 0x53;
	state->m_slide_pay_sensor[4] = 0;
	state->m_slide_pay_sensor[5] = 0;

	Scorpion2_SetSwitchState(machine,6,0,1);	/* ? percentage key */
	Scorpion2_SetSwitchState(machine,6,1,1);
	Scorpion2_SetSwitchState(machine,6,2,1);
	Scorpion2_SetSwitchState(machine,6,3,1);
	Scorpion2_SetSwitchState(machine,6,4,1);

	Scorpion2_SetSwitchState(machine,7,0,0);	/* GBP1 High Level Switch  */
	Scorpion2_SetSwitchState(machine,7,1,0);	/* 20P High Level Switch */
	Scorpion2_SetSwitchState(machine,7,2,0);	/* Token Front High Level Switch */
	Scorpion2_SetSwitchState(machine,7,3,0);	/* Token Rear High Level Switch */

	state->m_sc2_show_door   = 1;
	state->m_sc2_door_state  = 0x31;

	state->m_has_hopper = 0;
}

static DRIVER_INIT (bfmcgslm)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2awp_common_init(machine,6, 1);
	MechMtr_config(machine,8);
	BFM_BD1_init(0);
	state->m_has_hopper = 0;
}

static DRIVER_INIT (luvjub)
{
	bfm_sc2_state *state = machine.driver_data<bfm_sc2_state>();
	sc2awp_common_init(machine,6, 1);
	MechMtr_config(machine,8);
	state->m_has_hopper = 0;

	Scorpion2_SetSwitchState(machine,3,0,1);
	Scorpion2_SetSwitchState(machine,3,1,1);

	Scorpion2_SetSwitchState(machine,4,0,1);
	Scorpion2_SetSwitchState(machine,4,1,1);
	Scorpion2_SetSwitchState(machine,4,2,1);
	Scorpion2_SetSwitchState(machine,4,3,1);

	Scorpion2_SetSwitchState(machine,6,0,1);
	Scorpion2_SetSwitchState(machine,6,1,1);
	Scorpion2_SetSwitchState(machine,6,2,1);
	Scorpion2_SetSwitchState(machine,6,3,0);

	Scorpion2_SetSwitchState(machine,7,0,0);
	Scorpion2_SetSwitchState(machine,7,1,0);
	Scorpion2_SetSwitchState(machine,7,2,0);
	Scorpion2_SetSwitchState(machine,7,3,0);
}


ROM_START( sc2brkfs )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("big-breakfast_std_ar_var_a.bin",	0x00000, 0x10000, CRC(5f016daa) SHA1(25ee10138bddf453588e3c458268533a88a51217) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "big-breakfast_dat_ac_10pnd-20p_a.bin", 0x0000, 0x010000, CRC(cc54617f) SHA1(078e56b948d68ebcfaf986dd0f15be64607d0e4f) )
	ROM_LOAD( "big-breakfast_dat_ac_var_10pnd-20p_a.bin", 0x0000, 0x010000, CRC(a5967b05) SHA1(f0d4bc804181781a391fa052251c4bbf7d8f5e50) )
	ROM_LOAD( "big-breakfast_dat_ac_var_8pnd_a.bin", 0x0000, 0x010000, CRC(d97dbf7a) SHA1(d46270ff69cbc636744fc902d38cc282613cfdd2) )
	ROM_LOAD( "big-breakfast_dat_ar_var_a.bin", 0x0000, 0x010000, CRC(ade2834f) SHA1(54914fbc8416b2d08c13c56088b1665e267e6777) )
	ROM_LOAD( "big-breakfast_dat_ss_var_a.bin", 0x0000, 0x010000, CRC(57aff227) SHA1(5d4c6190194719b3fa5c02d30e7c6b59978c93c3) )


	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("bigbreakfastsnd.bin", 0x00000, 0x80000, CRC(bf91aa2b) SHA1(40942165e65ff9b027015d500e5a9726c44ba1c5))
ROM_END

ROM_START( sc2brkfs1 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("big-breakfast_std_ss_var_a.bin",	0x00000, 0x10000, CRC(08d1fa7d) SHA1(a3dba79eef32835f0b46dbd7b376b797324df904) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("bigbreakfastsnd.bin", 0x00000, 0x80000, CRC(bf91aa2b) SHA1(40942165e65ff9b027015d500e5a9726c44ba1c5))
ROM_END

ROM_START( sc2brkfs2 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("big-breakfast_std_ac_var_jp-8_a.bin",	0x00000, 0x10000, CRC(2671af1b) SHA1(0a34dd2953a99be9fb2a128f9d1f7ddc0fc8242a) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("bigbreakfastsnd.bin", 0x00000, 0x80000, CRC(bf91aa2b) SHA1(40942165e65ff9b027015d500e5a9726c44ba1c5))
ROM_END

ROM_START( sc2brkfs3 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("big-breakfast_std_ac_8pnd20p_a.bin",	0x00000, 0x10000, CRC(054c38ad) SHA1(f4ab55f977848e3d2a933bba1ab619ffa3e14db6) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("bigbreakfastsnd.bin", 0x00000, 0x80000, CRC(bf91aa2b) SHA1(40942165e65ff9b027015d500e5a9726c44ba1c5))
ROM_END

ROM_START( sc2brkfs4 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("big-breakfast_std_ac_var_10pnd-20p_a.bin",	0x00000, 0x10000, CRC(d879feaa) SHA1(2656fbe018fe40194c2b77d289b77fabbc9e537c) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("bigbreakfastsnd.bin", 0x00000, 0x80000, CRC(bf91aa2b) SHA1(40942165e65ff9b027015d500e5a9726c44ba1c5))
ROM_END

ROM_START( sc2brkfs5 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("big-breakfast_std_ac_10pnd-20p_a.bin",	0x00000, 0x10000, CRC(55d7321c) SHA1(0b4a6b66aa64fbb3238539a2167f761d0910b814) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("bigbreakfastsnd.bin", 0x00000, 0x80000, CRC(bf91aa2b) SHA1(40942165e65ff9b027015d500e5a9726c44ba1c5))
ROM_END


ROM_START( sc2drwho )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750288.bin",	0x00000, 0x10000, CRC(fe95b5a5) SHA1(876a812f69903fd99f896b35eeaf132c215b0035) ) // dr-who-time-lord_std_ss_20p_ass.bin

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho1 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750661.p1",	0x00000, 0x10000, CRC(4b5b50eb) SHA1(fe2b820c214b3e967348b99ccff30a4bfe0251dc) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho2 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("dr-who-time-lord_dat_ac_ass.bin",	0x00000, 0x10000, CRC(5a467a44) SHA1(d5a3dcdf50e07e36187350072b5d82d620f8f1d8) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho3 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("dr-who-time-lord_dat_ss_20p_ass.bin",	0x00000, 0x10000, CRC(8ce06af9) SHA1(adb58507b2b6aae59857384748d59485f1739eaf) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho4 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("dr-who-time-lord_std_ac_ass.bin",	0x00000, 0x10000, CRC(053313cc) SHA1(2a52b7edae0ce676255eb347bba17a2e48c1707a) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho5 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("dr-who-time-lord_std_var_20p_ass.bin",	0x00000, 0x10000, CRC(35f4e6ab) SHA1(5e5e35889adb7d3384aae663c667b0251d39aeee) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho6 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_dat_20p_uk94_ass.bin",	0x00000, 0x10000, CRC(e65717c2) SHA1(9b8db0bcac9fd996de29527440d6af3592102120) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho7 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_dat_ac_10pnd-20p-25p_ass.bin",	0x00000, 0x10000, CRC(9a27ac6d) SHA1(d1b0e85d41198c5d2cd1b492e53359a5dc1ac474) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho8 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_dat_ac_8pnd-20p_ass.bin",	0x00000, 0x10000, CRC(b6629b5e) SHA1(d20085b4ab9a0786063eb063f7d1df2a6814f40c) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho9 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_dat_ar_10p_ass.bin",	0x00000, 0x10000, CRC(04653c3b) SHA1(0c23f939103772fac628342074de820ec6b472ce) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho10 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_dat_ar_20p_uk94_ass.bin",	0x00000, 0x10000, CRC(40aaa98f) SHA1(80705e24e419558d8a7b1f886bfc2b3ce5465446) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho11 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_dat_var_no-jp-spin_ass.bin",	0x00000, 0x10000, CRC(bf087547) SHA1(f4b7289a76e814af5fb3affc360a9ac659c09bbe) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho12 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_std_20p_uk94_ass.bin",	0x00000, 0x10000, CRC(278f559e) SHA1(d4396df02a5e24b3684c26fcaa57c8e499789332) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho13 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_std_ac_8pnd-20p_ass.bin",	0x00000, 0x10000, CRC(0b2850c8) SHA1(5fac64f35a6b6158d8c15f41e82574768b1c3617) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho14 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_std_ar_10p_ass.bin",	0x00000, 0x10000, CRC(f716a21d) SHA1(340df4cdea3309bfebeba7c419057f1bf5ed5024) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho15 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("time-lord_std_ar_20p_uk94_ass.bin",	0x00000, 0x10000, CRC(8dd0f908) SHA1(2eca748874cc061f9a8145b081d2c097a40e1e47) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END

ROM_START( sc2drwho16 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("tmld5pa",	0x00000, 0x10000, CRC(b9ddfd0d) SHA1(915afd83eab330a0e70635c35f031f2041b9f5ad) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END


/* not encrypted, bootleg? */
ROM_START( sc2drwho17 )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("drwho.bin",	0x00000, 0x10000, CRC(9e53a1f7) SHA1(60c6aa226c96678a6e487fbf0f32554fd85ebd66) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("timelordsnd.bin", 0x00000, 0x80000, CRC(57fdaf3a) SHA1(f7cbaddb7f2ab8e1c7b17f187bab263e0dde463b))
ROM_END



ROM_START( sc2focus )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("focus.bin",	 0x00000, 0x10000, CRC(ddd1a21e) SHA1(cbb467b03642d6de37f6dc204b902f2d7e92230e))

	ROM_REGION( 0x20000, "upd", 0 )
	ROM_LOAD("focsound.bin", 0x00000, 0x20000, CRC(fce86700) SHA1(546680dd85234608c1b7e850bad3165400fd981c))
ROM_END


ROM_START( sc2gslam )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750843.bin", 0x00000, 0x10000, CRC(e159ddf6) SHA1(c897564a956becbd9d4c155df33b239e899156c0))

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "95752056", 0x0000, 0x010000, CRC(b28dcd9c) SHA1(f20ef0f0a1b5cc287cf93a175fede98dde3fecf4) )
	ROM_LOAD( "club-grand-slam_dat_ac_var_rot_ass.bin", 0x0000, 0x010000, CRC(d505db66) SHA1(6e40186a699a81138674e332acbd0d7d3939b9f6) )
	ROM_LOAD( "club-grand-slam_dat_acss.bin", 0x0000, 0x010000, CRC(82ff3cb9) SHA1(87794063421724201c8a3e67cd6e454b0f578c3e) )
	ROM_LOAD( "club-grand-slam_std_ac_ass.bin", 0x0000, 0x010000, CRC(b28dcd9c) SHA1(f20ef0f0a1b5cc287cf93a175fede98dde3fecf4) )

	ROM_REGION( 0x40000, "upd", 0 )
	ROM_LOAD("gslamsnd.bin", 0x00000, 0x40000, CRC(9afb8b42) SHA1(20e108c0041412fcd7b2969701f47a4a99d3677c))

	ROM_REGION( 0x80000, "altupd", 0 )
	ROM_LOAD( "grandslamsnd.bin", 0x0000, 0x080000, CRC(e4af3787) SHA1(9aa40f7c4c4db3618b553505b02663c1d5f297c3) )
	ROM_LOAD( "gslamsnd.bin", 0x0000, 0x080000, CRC(c9dfb6f5) SHA1(6e529c210b26e7ce164cebbff8ec314c6fa8f7bf) )
ROM_END



ROM_START( sc2cshcl )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "club_cashino_dat_ac_f65_rot_ass.bin", 0x0000, 0x010000, CRC(c2552162) SHA1(2c373b60588d870acd34d88025f6bb14687694fb) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "club_cashino_dat_ac_var_ass.bin", 0x0000, 0x010000, CRC(b529604e) SHA1(87f8dca7e570472697de2cbe7565a038503a6251) )
	ROM_LOAD( "club_cashino_std_ac_f65_rot_ass.bin", 0x0000, 0x010000, CRC(23aa2c72) SHA1(155df9b501cf5ae9eb3afca48c4100617793ac09) )
	ROM_LOAD( "club_cashino_std_ac_var_ass.bin", 0x0000, 0x010000, CRC(0e9fad24) SHA1(d14569f106ba29f9cb7769234f5531382e28bd69) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "cashsnd", 0x0000, 0x080000, CRC(807d37a6) SHA1(bd5f7c39a64a562e96a850a2cc82bfe3f74f1e54) )
ROM_END


ROM_START( sc2catms )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "cat and mouse p1.bin", 0x0000, 0x010000, CRC(b33b2a75) SHA1(ac57b4d33ac1218e39b8bbd669c40bdbb3839ccf) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "cat and mouse ver puss7.2.bin", 0x0000, 0x010000, CRC(6968bf9c) SHA1(c44faf2e5b391bee43021ad8544fb8d502f90433) )
	ROM_LOAD( "cat-and-mouse-mk2_dat_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(6806cfad) SHA1(8eb427688bc19e9b1508de1afa584bcba7e8d421) )
	ROM_LOAD( "cat-and-mouse-mk2_dat_ar_10p_ass.bin", 0x0000, 0x010000, CRC(c332595b) SHA1(3ea62b98129913b2ff576c42cfa7fe4d15a34b8e) )
	ROM_LOAD( "cat-and-mouse-mk2_dat_ar_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(22e2d319) SHA1(ca3f335f9f52cd152e420bd6c2e15fc1fac4eb29) )
	ROM_LOAD( "cat-and-mouse-mk2_dat_ar_ac_8pnd-20p_uk94_ass.bin", 0x0000, 0x010000, CRC(87b5fc94) SHA1(3e2b4aba0847fe1958710bff394ea98e02276b43) )
	ROM_LOAD( "cat-and-mouse-mk2_std_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(d8e72750) SHA1(b0431cbb311c88b4701bae3bbfdf1d45a070181c) )
	ROM_LOAD( "cat-and-mouse-mk2_std_ar_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(95beca0c) SHA1(6e2b175139c616cf80f020588b073f325a0c2684) )
	ROM_LOAD( "cat-and-mouse-mk2_std_ar_ac_8pnd-20p_uk94_ass.bin", 0x0000, 0x010000, CRC(c5fccfb0) SHA1(c427b42da60cd14516991a08a08f68421fa9ff88) )
	ROM_LOAD( "cat-and-mouse_dat_ac_10pnd-25p_ass.bin", 0x0000, 0x010000, CRC(d9811472) SHA1(dffab64155ed2c5193c24a660af7ad7c3c7bc093) )
	ROM_LOAD( "cat-and-mouse_dat_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(40ba729e) SHA1(d7b4fe209588d77921d6c37d1739805aed80f103) )
	ROM_LOAD( "cat-and-mouse_std_ac_10pnd-25p_ass.bin", 0x0000, 0x010000, CRC(4c538143) SHA1(4045599cfe57f442ac58aa1f0ed3a03ce63e2e4c) )
	ROM_LOAD( "cat-and-mouse_std_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(05396936) SHA1(61d976c22ba82bbff12fdcfb6b9320efebc9ad37) )
	ROM_LOAD( "cm20std", 0x0000, 0x010000, CRC(74ca0fd5) SHA1(2345bf3810820a12c613013fedad936ab9134b22) )
	ROM_LOAD( "cnm20mk2", 0x0000, 0x010000, CRC(0604a78a) SHA1(c75b90f93b1d36928ad46643cfce03dda2b20408) )

	// are these something else? different hw?
	ROM_LOAD( "catmouse1.bin", 0x0000, 0x002000, CRC(fa2f26a1) SHA1(a85cfde6e2f14d49f627fd8c0bf2c34b331a24b5) )
	ROM_LOAD( "catmouse2.bin", 0x0000, 0x002000, CRC(51f1ad0a) SHA1(a21196553bb41a025d26fe91ead6282dfc61afe5) )
	ROM_LOAD( "catmouse3.bin", 0x0000, 0x002000, CRC(5eb5e699) SHA1(ca78a29b607ecf2367d7213e37d5894973fe2a09) )
	ROM_LOAD( "catmouse4.bin", 0x0000, 0x002000, CRC(e96f1ea7) SHA1(d7c6d0f5852e0ee56e3316840fed034ddd7bf242) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "catandmousesnd.bin", 0x0000, 0x080000, CRC(00d3b224) SHA1(5ae35a7bfa65e8343564e6f6a219bc674710fadc) )
ROM_END




ROM_START( sc2eggs )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "eggs-on-legs_std_wi_ac_10pnd-20p_ass.bin", 0x0000, 0x010000, CRC(3fdad116) SHA1(d5fc405af8b14d8b85acb10aaa3c8a219753c864) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "eggs-on-legs_dat_ac_var_10pnd_ass.bin", 0x0000, 0x010000, CRC(718915f2) SHA1(717b57c0e81a48db005516135fdd4d82f7cfda28) )
	ROM_LOAD( "eggs-on-legs_dat_wi_ac_10pnd-20p_ass.bin", 0x0000, 0x010000, CRC(cdde5a4d) SHA1(b61e61193db4921217a7c285fd8fe2780d1f8091) )
	ROM_LOAD( "95750746.p1", 0x0000, 0x010000, CRC(a4b13487) SHA1(7ef2953ca11526bbae57b1aebb7a90de59c2d379) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "eggsonlegssnd.bin", 0x0000, 0x080000, CRC(24fef504) SHA1(75a05e0cf064f736dd9164c24ccef77a46aaee94) )
ROM_END


ROM_START( sc2gsclb )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "club-game-show_std_ac_p65_ass.bin", 0x0000, 0x010000, CRC(9a390095) SHA1(ee4b08956de0b018b9ceaf16a6410463053c1f3d) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "club-game-show_dat_ac_250pnd-25p_ass.bin", 0x0000, 0x010000, CRC(299b89f3) SHA1(eb78378410ca2380ec564e8268a51309dc8044ce) )
	ROM_LOAD( "club-game-show_dat_ac_250pnd-25p_p65_ass.bin", 0x0000, 0x010000, CRC(5d59e87e) SHA1(91684551db11d95768c364515cf5cd337b3f482b) )
	ROM_LOAD( "club-game-show_dat_ac_p65_ass.bin", 0x0000, 0x010000, CRC(61adb76f) SHA1(a7fcc6504d5eeae664b9aaca190bbf43bd989c93) )
	ROM_LOAD( "club-game-show_dat_ac_var_ass.bin", 0x0000, 0x010000, CRC(85cf033f) SHA1(ca7e506437e1ff229f2d79bedb13ae0fe5dd2696) )
	ROM_LOAD( "club-game-show_dat_ac_var_ffp_ass.bin", 0x0000, 0x010000, CRC(7e003d2a) SHA1(f8a6f6810b1733f46e470e89fa821cd51fbe1c5e) )
	ROM_LOAD( "club-game-show_dat_fe_ac_ass.bin", 0x0000, 0x010000, CRC(b5a03c26) SHA1(ef1bc28905a8a9db71299f5c30a15c5576766346) )
	ROM_LOAD( "club-game-show_std_ac_250pnd-24p_p65_ass.bin", 0x0000, 0x010000, CRC(142d828a) SHA1(2fe40e9d641be1cf89cfe9fe5cd4b29dd9ea01e7) )
	ROM_LOAD( "club-game-show_std_ac_var_ass.bin", 0x0000, 0x010000, CRC(01ae9d52) SHA1(3b85a7ebc346d4eb6a16b2b9a03aa12220020aff) )
	ROM_LOAD( "club-game-show_std_ac_var_ffp_ass.bin", 0x0000, 0x010000, CRC(d2819fc3) SHA1(23c7cbf9e04913f5cb62ef6accdd5b470eed3cd4) )
	ROM_LOAD( "club-game-show_std_fe_ac_ass.bin", 0x0000, 0x010000, CRC(6e479cc4) SHA1(99c15b0d1584ab7b460f273de825eb17681c5d0a) )
	ROM_LOAD( "gameshow.bin", 0x0000, 0x010000, CRC(babeb912) SHA1(41bc1cf82bef84f840998af1278c55ea1727a163) )
	ROM_LOAD( "95750844.p1", 0x0000, 0x010000, CRC(36efa743) SHA1(0f5392f55e42d7ac17e179c966997f41859f925a) )

	ROM_REGION( 0x80000, "upd", 0 )
	//ROM_LOAD( "gameshowsnd.bin", 0x0000, 0x080000, CRC(e1a0323f) SHA1(a015d99c882962651869d8ec71a6c17a1cba687f) )
	ROM_LOAD( "95004024.bin", 0x0000, 0x080000, CRC(e1a0323f) SHA1(a015d99c882962651869d8ec71a6c17a1cba687f) )
ROM_END



ROM_START( sc2cpg )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "club-pharaohs-gold_std_ac_250pnd-20p_rot_ass.bin", 0x0000, 0x010000, CRC(f83a68dc) SHA1(1a7aa08835d03116199034378ae0c617520a5ac6) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "club-pharaohs-gold_dat_ac_250pnd-20p_rot_ass.bin", 0x0000, 0x010000, CRC(2de3b252) SHA1(02c3bfabd5c732e37e71278be5aad0b6b44d28c6) )
	ROM_LOAD( "club-pharaohs-gold_dat_ac_var_ass.bin", 0x0000, 0x010000, CRC(bb790c4b) SHA1(d1126b9848047f15a65119e6446caced2c982287) )
	ROM_LOAD( "club-pharaohs-gold_dat_fe_ac_p65_rot_ass.bin", 0x0000, 0x010000, CRC(4ccba14d) SHA1(a0529a732a1a8c5c9a3d9830072ff1003c80b7d2) )
	ROM_LOAD( "club-pharaohs-gold_std_ac_var_ass.bin", 0x0000, 0x010000, CRC(9376c3c4) SHA1(9e67c982dfb838cde538d0893ea36eafe8bda2d3) )
	ROM_LOAD( "club-pharaohs-gold_std_fe_ac_p65_rot_ass.bin", 0x0000, 0x010000, CRC(e97c5bb4) SHA1(4df5f50bbfe453fbc351855dc6f6a24296563498) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "pharaohsgoldsnd.bin", 0x0000, 0x080000, CRC(7d67d53e) SHA1(159e0e9af1cfd6adc141daaa0f75d38af55218c3) )
ROM_END


ROM_START( sc2suprz )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "surprize-surprise_std_ga_20p_ass.bin", 0x0000, 0x010000, CRC(7e52c975) SHA1(a610f7170fda13f64e805e3d99b5f57c61206cfe) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "surprize-surprise_dat_ac_6pnd-20p_ass.bin", 0x0000, 0x010000, CRC(7e0b263e) SHA1(bcbd82a87e7db65db22e55d9111b0f819a62150a) )
	ROM_LOAD( "surprize-surprise_dat_ga_20p_ass.bin", 0x0000, 0x010000, CRC(8ee54a57) SHA1(471a06d9840ecbf850c8896f8bf45264c0b8390f) )
	ROM_LOAD( "surprize-surprise_dat_var_ass.bin", 0x0000, 0x010000, CRC(37ab423e) SHA1(6b2ab927eb851b8f77eb474a1c5b68c335a17b2f) )
	ROM_LOAD( "surprize-surprise_std_ac_6pnd-20p_ass.bin", 0x0000, 0x010000, CRC(297959d7) SHA1(9bc8bc3d1be1f282573a3ad6994f06ee7bb64dfd) )
	ROM_LOAD( "surprize-surprise_std_var_ass.bin", 0x0000, 0x010000, CRC(5ef85273) SHA1(2ca9e3245c97fbed97a781e135fbb79df5b1bf18) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "surprisesurprizesnd.bin", 0x0000, 0x01fedb, CRC(c0981343) SHA1(71278c3446cf204a31415dd2ed8f1de7f7a16645) )
ROM_END


ROM_START( sc2motd )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "match-of-the-day_std_20p_ass.bin", 0x0000, 0x010000, CRC(441931ef) SHA1(9c8c79470dda2a6589d04e4eb8d00d8a984bd1ed) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "match-of-the-day_dat_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(fa9216fa) SHA1(3d5d164419f022488e60e738958d3f66f4206e87) )
	ROM_LOAD( "match-of-the-day_dat_8pnd-20p_ass.bin", 0x0000, 0x010000, CRC(da77960d) SHA1(e6fc97994612d9280b60df6600c26aa7919381d2) )
	ROM_LOAD( "match-of-the-day_dat_ac_10pnd_tri1_ass.bin", 0x0000, 0x010000, CRC(948b3ede) SHA1(f1c7b4e9fb83ba848d4d8a3ab02a1a5e3b630054) )
	ROM_LOAD( "match-of-the-day_dat_ac_10pnd_uk94_ass.bin", 0x0000, 0x010000, CRC(632325d8) SHA1(92c68b51b4e594bec5d9af43a697a4dd912ed864) )
	ROM_LOAD( "match-of-the-day_dat_ac_8pnd-20p_ass.bin", 0x0000, 0x010000, CRC(86baaf46) SHA1(acb9c5cad4c35621219380a997ae67accaea4206) )
	ROM_LOAD( "match-of-the-day_dat_ar_20p_ass.bin", 0x0000, 0x010000, CRC(ab1c44b9) SHA1(ce34570fabcb2c6ceab48ef7c4367ccafa95ef1a) )
	ROM_LOAD( "match-of-the-day_dat_ar_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(f5adb4aa) SHA1(85afff3251e13808f140d6e58f1c9e2e23ce9d8c) )
	ROM_LOAD( "match-of-the-day_dat_ms_20p_ass.bin", 0x0000, 0x010000, CRC(77710913) SHA1(709fff877ee863021e958bcecbd5cd58a977ea09) )
	ROM_LOAD( "match-of-the-day_dat_ss_20p_ass.bin", 0x0000, 0x010000, CRC(19dafe2d) SHA1(8a7bc4bfb7acd5386fdcadf91c2ba4f5615fa3c9) )
	ROM_LOAD( "match-of-the-day_dat_wi_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(412a30ed) SHA1(c7118954c086fb1243e441ed7728d801667e98ba) )
	ROM_LOAD( "match-of-the-day_std_8pnd-20p_ass.bin", 0x0000, 0x010000, CRC(8042a61d) SHA1(3e0e75918d6df2d4ed537ee532d1a7fa0bb359b7) )
	ROM_LOAD( "match-of-the-day_std_ac_10pnd_tri1_ass.bin", 0x0000, 0x010000, CRC(10b7a217) SHA1(615bf8e6d1b79c96efd91335a9c6f5db0df95891) )
	ROM_LOAD( "match-of-the-day_std_ac_10pnd_uk94_ass.bin", 0x0000, 0x010000, CRC(f75d128d) SHA1(7da2fb6bc7265848c20cfc137de846439af83b90) )
	ROM_LOAD( "match-of-the-day_std_ac_var_uk94.bin", 0x0000, 0x010000, CRC(ae2330f0) SHA1(d309284f0f0333f6e065f30d7ac9416b2fc4ee1f) )
	ROM_LOAD( "match-of-the-day_std_ar_20p_ass.bin", 0x0000, 0x010000, CRC(27f942a3) SHA1(928d3c2eef6b202c0d71b0843f64aba15aab4f42) )
	ROM_LOAD( "match-of-the-day_std_ar_20p_uk94_ass.bin", 0x0000, 0x010000, CRC(96687a5a) SHA1(dafd7b0af3e26d609b5927c431f4adf2f424322a) )
	ROM_LOAD( "match-of-the-day_std_ss_20p_ass.bin", 0x0000, 0x010000, CRC(ce926573) SHA1(dff243d0eb12d4c13c8334099c5958e897cb8bd5) )
	ROM_LOAD( "match-of-the-day_std_wi_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(b059fe93) SHA1(33d15c464f3f80f4600d961ddade0b6a661747ba) )
	ROM_LOAD( "motd6ac", 0x0000, 0x010000, CRC(d8e7811c) SHA1(ac67683984465aaf8a96322e71ab7b7bffe92361) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "modsndf.bin", 0x0000, 0x080000, CRC(088471f5) SHA1(49fb22daf04450186e9a83aee3312bb85ccf6842) )

	ROM_REGION( 0x80000, "altupd", 0 ) // this one seems to say 'no thanks' when you insert a coin? for different rev?
	ROM_LOAD( "match_of_the_day_sound.bin", 0x0000, 0x080000, CRC(5ce2fc50) SHA1(26533428582058f0cd618e3657f967bc64e551fc) )
ROM_END



ROM_START( sc2easy )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "easy-money_std_ac_var_8-10pnd_ass.bin", 0x0000, 0x010000, CRC(e9f581ca) SHA1(aee8a1af609921a0b33db7b460e4a58517bf9276) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "easy-money_dat_ac_var_8-10pnd_ass.bin", 0x0000, 0x010000, CRC(e5633ac3) SHA1(d868d782e7d5f6c62ab8958150857336b7acff97) )
	ROM_LOAD( "easy-money_dat_wi_ac_10pnd_tri3_ass.bin", 0x0000, 0x010000, CRC(f841d5cf) SHA1(05afdfa483271635b530652385e2e566920e533d) )
	ROM_LOAD( "easy-money_dat_wi_ac_var_10pnd-20p_ass.bin", 0x0000, 0x010000, CRC(97f62e2d) SHA1(0884ddd0b25e78dd402983158e8c623ff4326cbd) )
	ROM_LOAD( "easy-money_std_wi_ac_10pnd_tri3_ass.bin", 0x0000, 0x010000, CRC(38434925) SHA1(17148ba440c8fd139f7889a211a914ed679a195f) )
	ROM_LOAD( "easy-money_std_wi_ac_var_10pnd-20p_ass.bin", 0x0000, 0x010000, CRC(05622afc) SHA1(169a492870a70aeb17078b2b27c36f5b82274b3f) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "easy-money_snd.bin", 0x0000, 0x080000, CRC(56d224c5) SHA1(43b81a1a9a7d30ef7bfb2bbc61e3106faa927778) )
ROM_END



ROM_START( sc2majes )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "majestic.p1", 0x0000, 0x010000, CRC(37289a5f) SHA1(a9d86ed16fc2ff2b83b60e48a1704b4e189c3ac7) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "majesticsnd.bin", 0x0000, 0x080000, CRC(3ee3fee3) SHA1(6a5e72e8a808d870a84a0e3523eebfadfab6d5df) )
ROM_END


ROM_START( sc2luvv )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("95750808.bin", 0x00000, 0x10000, CRC(e6668fc7) SHA1(71dd412114c6386cba72e2b29ea07f2d99d14065))

	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD("mtx_ass.bin",	 0x00000, 0x10000, CRC(cfdd7bb2) SHA1(90086aaff743a7b2385488af1e8a126029113028))

	ROM_REGION( 0x20000, "altmatrix", 0 )
	ROM_LOAD( "luvvley-jubbley_mat_ass.bin", 0x0000, 0x010000, CRC(e4e06767) SHA1(bee2385c2a9c7ca39ff6a599f827ddba4324b903) )
	ROM_LOAD( "95000575.mtx", 0x0000, 0x0054e8, CRC(d81296df) SHA1(c248cdd5eb59a19fab9098d5bee2c60e9e474fd6) )
	ROM_LOAD( "95000584.mtx", 0x0000, 0x0054d3, CRC(d372b3ef) SHA1(076460d8aaf996d80397da2ebc32e8f1efb63572) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "luvvley-jubbley_dat_ac_10pnd-20p_ass.bin", 0x0000, 0x010000, CRC(9dee74fc) SHA1(d29756d743b781ab9ce7baf990f4a2cc0e9d7972) )
	ROM_LOAD( "luvvley-jubbley_dat_ac_10pnd-25p_ass.bin", 0x0000, 0x010000, CRC(355210a0) SHA1(c03e1109ee1a419fc4ebdcf861d5220303a9c587) )
	ROM_LOAD( "luvvley-jubbley_dat_ac_4pnd-5p_ass.bin", 0x0000, 0x010000, CRC(4b3155b8) SHA1(aaba2e3d54a2b099b63ee4f5d3560d8eb562c4f1) )
	ROM_LOAD( "luvvley-jubbley_dat_ga_20p_ass.bin", 0x0000, 0x010000, CRC(8c0a6180) SHA1(1c1ee2b5081ee901b5929405a78d3e7a7989916a) )
	ROM_LOAD( "luvvley-jubbley_dat_ms_20p_ass.bin", 0x0000, 0x010000, CRC(886a3a8e) SHA1(4c986e0c7278bd058ce2df2d755cbc8e4f31b3fa) )
	ROM_LOAD( "luvvley-jubbley_std_ac_4pnd-5p_ass.bin", 0x0000, 0x010000, CRC(065ee9bb) SHA1(5d46f0e1b5d48dc94b9843998dedf6d3dfc83e3c) )
	ROM_LOAD( "luvvley-jubbley_std_ms_20p_ass.bin", 0x0000, 0x010000, CRC(d40a59d0) SHA1(7173fc6d349868b9194c4ad581762d299dfb1c69) )
	ROM_LOAD( "luvvley-jubbley_std_ac_10pnd-20p_a.bin", 0x0000, 0x010000, CRC(e4440803) SHA1(be9b49cbe2cfcaa0e640365e190da9c3fcf82bea) )
	ROM_LOAD( "luvvley-jubbley_std_ac_10pnd-20p_ass.bin", 0x0000, 0x010000, CRC(e4440803) SHA1(be9b49cbe2cfcaa0e640365e190da9c3fcf82bea) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("snd.bin",		 0x00000, 0x80000, CRC(19efac32) SHA1(26f901fc11f052a4d3cff67f8f61dcdd04f3dc22))
ROM_END



ROM_START( sc2ptytm )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "95750806.p1", 0x0000, 0x010000, CRC(4e98c6c6) SHA1(7f4ec51f384b5203229da28f39c3127cd40cf67d) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "party-time_dat_ac_4pnd-10p_ass.bin", 0x0000, 0x010000, CRC(a33a6d08) SHA1(cf93f42971978b00a15e17d4da6bb6e16e8f1fab) )
	ROM_LOAD( "partytime.bin", 0x0000, 0x010000, CRC(20ef430c) SHA1(b5d35704da425e7ca84500071f34b4d65d87b9fa) )

	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD( "party-time_mtx_ass.bin", 0x0000, 0x010000, CRC(0672a9f4) SHA1(9e8e01aaa081ffb68aa494fe9dbae0620da0f6b9) )

	ROM_REGION( 0x20000, "altmatrix", 0 )
	ROM_LOAD( "95000585.mtx", 0x0000, 0x004a27, CRC(84682dd9) SHA1(038dd54c071d59f164b39b53c4e0888113489cf1) )
	ROM_LOAD( "partydot.bin", 0x0000, 0x010000, CRC(8a09b858) SHA1(bc932bebc7718da2b97e5f6ef06eb739748353f4) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "partysnd1.bin", 0x0000, 0x020000, CRC(b5a5cc9e) SHA1(c9b132ad0d1ce9ff6b56ebde89d5006a5cf7dff6) )
ROM_END



ROM_START( sc2ofool )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "fools & horses 10m 6.bin", 0x0000, 0x010000, CRC(5fe48a02) SHA1(fd5b07a58567e0c5eb75bf1526a853b3a60ddfa9) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "fools scor ii 10p.bin", 0x0000, 0x010000, CRC(1d6245b7) SHA1(f73b4741cf07d96ec79d907b88d07cd20c748dd3) )
	ROM_LOAD( "fools.bin", 0x0000, 0x010000, CRC(eaa0757a) SHA1(b6bec8f4f443d6c22c18e16ec0d65839fe30b61c) )
	ROM_LOAD( "fools6ac.bin", 0x0000, 0x010000, CRC(5fe48a02) SHA1(fd5b07a58567e0c5eb75bf1526a853b3a60ddfa9) )
	ROM_LOAD( "game 147s only fools.bin", 0x0000, 0x010000, CRC(6cb6cef1) SHA1(bfa40f517b1455e4d563be5964605be63e950e87) )
	ROM_LOAD( "onlyfoolsnhorses_std.bin", 0x0000, 0x010000, CRC(03cc611a) SHA1(e37d6b87017a52f8de339bbd69b2ccbff9872fae) )

	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD( "onlyfoolsnhorsesdotmatrix.bin", 0x0000, 0x010000, CRC(521611f7) SHA1(08cdc9f7434657151d90fcfd26ce4668477c2998) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "onlyfools_snd.bin", 0x0000, 0x080000, CRC(c073bb0c) SHA1(54b3df8c8d814af1fbb662834739a32a693fc7ee) )
ROM_END




ROM_START( sc2town )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "atown20p", 0x0000, 0x010000, CRC(4f7ec25e) SHA1(52af065633942a9e4c195f3294b81ae57bf0c414) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "round-the-town_dat_ac_10pnd-20p-25p_ass.bin", 0x0000, 0x010000, CRC(8291ad4e) SHA1(cd304052123dfe6d8504a6f5e92413c569bcaf8e) )
	ROM_LOAD( "round-the-town_dat_ar_var_ass.bin", 0x0000, 0x010000, CRC(3d811bb4) SHA1(134e1c65f4f8377eca6d7ccfded5d4600d2949bf) )
	ROM_LOAD( "round-the-town_dat_var_ass.bin", 0x0000, 0x010000, CRC(85110517) SHA1(30eba3987cc60ccbaecbc4c700bb2f1ba088d12f) )
	ROM_LOAD( "round-the-town_std_ac_10pnd-20p-25p_ass.bin", 0x0000, 0x010000, CRC(8394c0e9) SHA1(b9b45e0c855a5f7270259543337fb441694b61e2) )
	ROM_LOAD( "round-the-town_std_ac_20p_20po_ass.bin", 0x0000, 0x010000, CRC(6bc0c2ff) SHA1(9a2bac50978f2b7d2072e0febe4bf4a935bf287d) )
	ROM_LOAD( "round-the-town_std_ar_var_ass.bin", 0x0000, 0x010000, CRC(e5be3a13) SHA1(8a31c67641bce3c2160bb1c651535902374349b4) )
	ROM_LOAD( "round-the-town_std_var_ass.bin", 0x0000, 0x010000, CRC(1909994f) SHA1(47268e1119c808096ddff872e28444ed67bc5dbf) )
	ROM_LOAD( "rtt8ac", 0x0000, 0x010000, CRC(e495e5ea) SHA1(4fb6a43cee1c79ce05b71b35b195f2d35913c40c) )
	ROM_LOAD( "95750069.p1", 0x0000, 0x010000, CRC(6bc0c2ff) SHA1(9a2bac50978f2b7d2072e0febe4bf4a935bf287d) )

	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD( "round-the-town_mtx.bin", 0x0000, 0x010000, CRC(aa6aac1d) SHA1(57ed376f602dd70495b3bd356bea5113fa8e861e) )
	//ROM_LOAD( "attdot.bin", 0x0000, 0x010000, CRC(aa6aac1d) SHA1(57ed376f602dd70495b3bd356bea5113fa8e861e) )

	ROM_REGION( 0x20000, "altmatrix", 0 )
	ROM_LOAD( "round-the-town_mtx_ass.bin", 0x0000, 0x010000, CRC(1a3b2fb1) SHA1(3d51c6e16558c1ac8ad852a461cd89aef9bc91e4) )
	ROM_LOAD( "95000581.mtx", 0x0000, 0x005c57, CRC(55c55c76) SHA1(3db65ba2acd8cd09f8c12a9135a1d93b71e0838b) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "attsnd.bin", 0x0000, 0x040000, CRC(9b5327c8) SHA1(b9e5aeb3e9a6ece796e9164e425829d97c5f3a82) )
ROM_END


ROM_START( sc2cpe )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD("ce1std25p.bin", 0x00000, 0x10000, CRC(2fad9a49) SHA1(5ffb53031eef8778363836143c4e8d2a65361d51))

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "95000572.p1", 0x0000, 0x010000, CRC(551ef8ca) SHA1(825f4c3ff56cb2da20ffe1b2ec33f1692f6806b2) )
	ROM_LOAD( "95750273.p1", 0x0000, 0x010000, CRC(950da13c) SHA1(2c544e06112969f7914a5b4fd15e6b0dfedf6b0b) )
	ROM_LOAD( "club-public-enemy-no1_dat_ac_200pnd_ass.bin", 0x0000, 0x010000, CRC(fec925a3) SHA1(5ce3b6f1236f511ae8975c7ecd1549e8d427a245) )
	ROM_LOAD( "club-public-enemy-no1_dat_ac_250pnd-25p_p65_ass.bin", 0x0000, 0x010000, CRC(131375cd) SHA1(4899e8dd4acec9563fa40109bb9b839c5d7209a8) )
	ROM_LOAD( "club-public-enemy-no1_dat_ac_25p_ass.bin", 0x0000, 0x010000, CRC(00bedbdf) SHA1(97b3e23fed6692ae88e6a6110008124422478355) )
	ROM_LOAD( "club-public-enemy-no1_dat_fe_ac_200pnd_p65_rot_ass.bin", 0x0000, 0x010000, CRC(8d5ff953) SHA1(bdf6b5e014c46f6abac792a5913e98cb897b2a73) )
	ROM_LOAD( "club-public-enemy-no1_dat_fe_ac_250pnd-25p_ass.bin", 0x0000, 0x010000, CRC(5a79358b) SHA1(bf728108aad6937be0a5d79fa604f7ac3b191b42) )
	ROM_LOAD( "club-public-enemy-no1_std_ac_200pnd_ass.bin", 0x0000, 0x010000, CRC(5704e52d) SHA1(dfae48734794cea2e9a952d808dedb96fd5204b3) )
	ROM_LOAD( "club-public-enemy-no1_std_ac_250pnd-25p_p65_ass.bin", 0x0000, 0x010000, CRC(2d56a73b) SHA1(31195fa16c1c95d49716448b80f1d0aa973f29d5) )
	ROM_LOAD( "club-public-enemy-no1_std_fe_ac_250pnd-25p_ass.bin", 0x0000, 0x010000, CRC(0a36fd07) SHA1(6338858eb0dd6ba43bfea66afde0d6d1d5097aee) )
	ROM_LOAD( "pe1.bin", 0x0000, 0x010000, CRC(5704e52d) SHA1(dfae48734794cea2e9a952d808dedb96fd5204b3) )


	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD("cpe1_mtx.bin",  0x00000, 0x10000, CRC(5fd1fd7c) SHA1(7645f8c011be77ac48f4eb2c75c92cc4245fdad4))

	ROM_REGION( 0x20000, "altmatrix", 0 )
	ROM_LOAD( "club-public-enemy-no1_mtx_25pss.hex", 0x0000, 0x01be8c, CRC(e57e66b5) SHA1(f3e44cdb697e6e666bd0008824e802a2cf997aa5) )
	ROM_LOAD( "matrix.bin", 0x0000, 0x010000, CRC(64014f73) SHA1(67d44db91944738fcadc38bfd0d2b7c0536adb9a) )
	ROM_LOAD( "95000572.mtx", 0x0000, 0x008680, CRC(b7f486a0) SHA1(298ae0cf1b256517daa052efd25769230d0ce8a5) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD("cpe1_snd.bin",  0x00000, 0x80000, CRC(ca8a56bb) SHA1(36434dae4369f004fa5b4dd00eb6b1a965be60f9))

	ROM_REGION( 0x80000, "altupd", 0 )
	ROM_LOAD( "pen1c_snd.bin", 0x0000, 0x080000, CRC(57f3d152) SHA1(f5ccd11042d54396352df149e85c4aa271342d49) )
	ROM_LOAD( "95004012.p1", 0x0000, 0x080000, CRC(30d1f22a) SHA1(73cb2d12b090841a12a2ed21653248f41d02e125) )
ROM_END




ROM_START( sc2cops )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "cops & robbers 10 p1 (27512)", 0x0000, 0x010000, CRC(2a74bf68) SHA1(e6d0cf5c26815184d74bc2b1769d13321ce5e33a) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "casino-cops-and-robbers_dat_ac_var_10pnd_ass.bin", 0x0000, 0x010000, CRC(fadde12b) SHA1(9b041c932558a0132c853514ca3f325f6f97bc65) )
	ROM_LOAD( "casino-cops-and-robbers_dat_ms_to_8pnd_ass.bin", 0x0000, 0x010000, CRC(361ad99f) SHA1(444f2aeef404b087d49e2283bb36bde5e4e673ee) )
	ROM_LOAD( "casino-cops-and-robbers_std_ac_var_10pnd_ass.bin", 0x0000, 0x010000, CRC(549457c2) SHA1(271c7077fd3ee5de67c914faf095b5295dfb6207) )
	ROM_LOAD( "casino-cops-and-robbers_std_ms_to_8pnd_ass.bin", 0x0000, 0x010000, CRC(600a91fd) SHA1(b04bce98df824d2c217c70bd8a49349f93043360) )
	ROM_LOAD( "cops & robbers 6 25p (27512)", 0x0000, 0x010000, CRC(0ad3fedf) SHA1(25775a80272c72234be9f528cc8f13cf9e1adbf7) )
	ROM_LOAD( "cops-and-robbers_dat_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(2e3d0614) SHA1(b8be9a1d0be643d0dde7f6d89c067af1e85018bf) )
	ROM_LOAD( "cops-and-robbers_dat_ar_var_ass.bin", 0x0000, 0x010000, CRC(6f544505) SHA1(177a8d4038759dc0e52c14b463aaa6afce81d338) )
	ROM_LOAD( "cops-and-robbers_dat_ss_var_ass.bin", 0x0000, 0x010000, CRC(f14af5f8) SHA1(8bb4d9fc78f1f2c274c4b21c7f4e67c3856f0019) )
	ROM_LOAD( "cops-and-robbers_std_ac_10pnd_a.bin", 0x0000, 0x010000, CRC(2a74bf68) SHA1(e6d0cf5c26815184d74bc2b1769d13321ce5e33a) )
	ROM_LOAD( "cops-and-robbers_std_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(2a74bf68) SHA1(e6d0cf5c26815184d74bc2b1769d13321ce5e33a) )
	ROM_LOAD( "cops-and-robbers_std_ss_var_ass.bin", 0x0000, 0x010000, CRC(664216d2) SHA1(e222147d71f251554207627b7e5e9de5f10cfff8) )
	ROM_LOAD( "cops1020", 0x0000, 0x010000, CRC(3219a07f) SHA1(1f775189b50eeb55c584dd1054c9119d02b2f738) )
	ROM_LOAD( "cops8ac", 0x0000, 0x010000, CRC(c2ef20ff) SHA1(3841fcaacb739ee90ddc064d42d3275dc6a64016) )
	// are these different HW? (SC1?)
	ROM_LOAD( "cop56cp1", 0x0000, 0x008000, CRC(214edd7d) SHA1(007c17cc522c8f0d30bc1fd08bb18850344f62ad) )
	ROM_LOAD( "cop56cp2", 0x0000, 0x008000, CRC(c862ee34) SHA1(e807d1072953e67581ce0181bfd82a7efcee7bf0) )
	ROM_LOAD( "cops&robbers5pv1-3a(27256)", 0x0000, 0x008000, CRC(29513083) SHA1(f2ce0b573d6756e7d835488b8d8eed3266787255) )
	ROM_LOAD( "cops&robbers5pv1-3b(27256)", 0x0000, 0x008000, CRC(6f5425d6) SHA1(7673841ccfe16eaa0a5cfca1596383f7711f2dbe) )
	ROM_LOAD( "cops & robbers 5p v1-3 a (27256)", 0x0000, 0x008000, CRC(29513083) SHA1(f2ce0b573d6756e7d835488b8d8eed3266787255) )
	ROM_LOAD( "cops & robbers 5p v1-3 b (27256)", 0x0000, 0x008000, CRC(6f5425d6) SHA1(7673841ccfe16eaa0a5cfca1596383f7711f2dbe) )


	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD( "cops & robbers 10 p2 (27512)", 0x0000, 0x010000, CRC(bdd56a09) SHA1(92d0416578c55075a127f1c2af8d6de5216dd189) )

	ROM_REGION( 0x20000, "altmatrix", 0 )
	ROM_LOAD( "copsdot8", 0x0000, 0x010000, CRC(0eff2127) SHA1(e9788999ac6006faf0eb4e9d8ef1fd52f092be5a) )
	ROM_LOAD( "cops-and-robbers_mtx_a.bin", 0x0000, 0x010000, CRC(bdd56a09) SHA1(92d0416578c55075a127f1c2af8d6de5216dd189) )
	ROM_LOAD( "cops-and-robbers_mtx_ass.bin", 0x0000, 0x010000, CRC(bdd56a09) SHA1(92d0416578c55075a127f1c2af8d6de5216dd189) )
	ROM_LOAD( "copdot10", 0x0000, 0x010000, CRC(30c41ddd) SHA1(9aa66c30aa0fcbd3fb79a6d0d45d777a116f951c) )
	ROM_LOAD( "95000578.mtx", 0x0000, 0x00438f, CRC(8fd08810) SHA1(fbb278629067ed2fb17479f6a9fd439e41809f53) ) // same as bdd56a09 rom, but zipped? check


	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "copssnd.bin", 0x0000, 0x040000, CRC(4bebbc37) SHA1(10eb8542a9de35efc0f75b532c94e1b3e0d21e47) )

	ROM_REGION( 0x80000, "altupd", 0 ) // probably just the same but with data repeated, check
	ROM_LOAD( "copsnrobbers.bin", 0x0000, 0x080000, CRC(04ebfc07) SHA1(3c8e9f0e47f3b9b4d787dcd576e11a9b4a71757e) )
ROM_END



ROM_START( sc2dels )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "del's-millions_std_ac_10pnd-20p-25p_a.bin", 0x0000, 0x010000, CRC(b1e8d4ef) SHA1(189184aa6f9ff2204e35d0f7ae40493bcb0751bd) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "95751541.p1", 0x0000, 0x010000, CRC(495b7cec) SHA1(779a80371580b9154f0915e7c438dbf965dd1a02) )
	ROM_LOAD( "del's-millions_dat_ac_10pnd-20p-25p_a.bin", 0x0000, 0x010000, CRC(c81f200f) SHA1(8a9ee842e17a63276a0850adc52159dc46a239c0) )
	ROM_LOAD( "del's-millions_dat_ac_8pnd-20p_a.bin", 0x0000, 0x010000, CRC(92c0e403) SHA1(5410365137ab8debb10358f24cdd0b0b74755677) )
	ROM_LOAD( "del's-millions_dat_ac_8pnd_a.bin", 0x0000, 0x010000, CRC(23eca216) SHA1(f427d92929e51d6f0148d212e13067ddc15e2307) )
	ROM_LOAD( "del's-millions_dat_ms_20p_a.bin", 0x0000, 0x010000, CRC(57ade491) SHA1(3aed99d92c391f99fa8ff7d61370d59245156121) )
	ROM_LOAD( "del's-millions_dat_wi_ac_10pnd-20p_a.bin", 0x0000, 0x010000, CRC(fdb33c9b) SHA1(2506fe8e7e1e49f90652309996813ac5967442a0) )
	ROM_LOAD( "del's-millions_std_ac_8pnd-20p_a.bin", 0x0000, 0x010000, CRC(9194fb69) SHA1(30d2c5a8a16c96c081f442a66172f8b9fb1d602d) )
	ROM_LOAD( "del's-millions_std_ac_8pnd_a.bin", 0x0000, 0x010000, CRC(58f87c90) SHA1(a6dcdf1edc7620226d89c907a5910c4a4b2d4190) )
	ROM_LOAD( "del's-millions_std_ms_20p_ass.bin", 0x0000, 0x010000, CRC(f4a5803d) SHA1(c9b6f71847a4dd87ea34b51935618df5a735150d) )
	ROM_LOAD( "del's-millions_std_ss_20p_a.bin", 0x0000, 0x010000, CRC(755b8546) SHA1(67d2bb5556c03acf71e0b50c8cf54ac92acbce69) )
	ROM_LOAD( "del's-millions_std_wi_ac_10pnd-20p_a.bin", 0x0000, 0x010000, CRC(dd44aecb) SHA1(1e8ced54323580f43facf683c1f489f1ea281e16) )
	ROM_LOAD( "delm20p", 0x0000, 0x010000, CRC(9d8acc21) SHA1(04d9cb4d01ddfb4e33774b313446dcd763f869fa) )
	ROM_LOAD( "dels millions ck 8f98 std 8.bin", 0x0000, 0x010000, CRC(755b8546) SHA1(67d2bb5556c03acf71e0b50c8cf54ac92acbce69) )
	ROM_LOAD( "dels10", 0x0000, 0x010000, CRC(8bf1b9f5) SHA1(eb9c36579d56f83d72952fab9911a991aeec0579) )
	ROM_LOAD( "dels8mss", 0x0000, 0x002000, CRC(a91764fc) SHA1(3196cfbe04af74ea330a23a1155a6e223cb670bb) ) // bad dump?
	ROM_LOAD( "delsdlx6", 0x0000, 0x010000, CRC(64acb285) SHA1(7a011b915809712fd69902258f1e6c9b42f163eb) )
	ROM_LOAD( "delsmillions.bin", 0x0000, 0x010000, CRC(58f87c90) SHA1(a6dcdf1edc7620226d89c907a5910c4a4b2d4190) )
	ROM_LOAD( "dem20arc", 0x0000, 0x010000, CRC(9ae6291d) SHA1(966416d234e2ec708984595dedbfbe554ff1c867) )

	ROM_REGION( 0x80000, "upd", 0 )
	ROM_LOAD( "dmsnd.bin", 0x0000, 0x080000, CRC(0a68550b) SHA1(82a4a8d2a754a59da553b3568df870107e33f978) )

	ROM_REGION( 0x80000, "altupd", 0 )
	ROM_LOAD( "delssnd.bin", 0x0000, 0x080000, CRC(cb298f06) SHA1(fdc857101ad15d58aeb7ffc4a489c3de9373fc80) )	
ROM_END



ROM_START( sc2wembl )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "road-to-wembley_std_ac_10pnd_15rm_ass.bin", 0x0000, 0x010000, CRC(7b8e7a47) SHA1(3026850a18ef9cb44584550e28f62165bfa690e9) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "95750499.p1", 0x0000, 0x010000, CRC(a2b11ca6) SHA1(cc1931504f8da98119f771499db616898d92e0d9) )
	ROM_LOAD( "95750500.p1", 0x0000, 0x010000, CRC(bfe45926) SHA1(6a2814735e0894bb5152cba8f90d98cfa98c250b) )
	ROM_LOAD( "95750501.p1", 0x0000, 0x010000, CRC(cab3da07) SHA1(8ef7ed8427cbb213f218328666da3ebd92aca5a5) )
	ROM_LOAD( "road-to-wembley_dat_20p_15rm_ass.bin", 0x0000, 0x010000, CRC(45c3df4c) SHA1(48ef0e46a94a815e1e429f402cc8fd13bde4d738) )
	ROM_LOAD( "road-to-wembley_dat_ac_10pnd_15rm_ass.bin", 0x0000, 0x010000, CRC(6ab89e2f) SHA1(6b2faa587153f453e9fdf043c6ca5a90d8c6b66d) )
	ROM_LOAD( "road-to-wembley_dat_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(bf15d498) SHA1(f94d21d1202107db7955829340ada445d59f74ff) )
	ROM_LOAD( "road-to-wembley_dat_ac_8pnd_16rm_ass.bin", 0x0000, 0x010000, CRC(512fafcb) SHA1(fe90c7fc58bd3dc0bc84e060c6b7a37dd855733b) )
	ROM_LOAD( "road-to-wembley_dat_ar_20p_16rm_ass.bin", 0x0000, 0x010000, CRC(550f82ec) SHA1(80b1d0839f600b01f2a60de0e191add0faaad089) )
	ROM_LOAD( "road-to-wembley_dat_ss_10p_ass.bin", 0x0000, 0x010000, CRC(630b5306) SHA1(aa23645cc7f1c86e88a62420a837ab64c5090d09) )
	ROM_LOAD( "road-to-wembley_dat_ss_20p_16rm_ass.bin", 0x0000, 0x010000, CRC(55b1764a) SHA1(1b1e5b89eda0d07662af003d1259e0da725abbc9) )
	ROM_LOAD( "road-to-wembley_std_20p_15rm_ass.bin", 0x0000, 0x010000, CRC(065f2f8b) SHA1(81471db8de879b7d5b8741beefa5214f2c48ef84) )
	ROM_LOAD( "road-to-wembley_std_ac_10pnd_ass.bin", 0x0000, 0x010000, CRC(ae2330f0) SHA1(d309284f0f0333f6e065f30d7ac9416b2fc4ee1f) )
	ROM_LOAD( "road-to-wembley_std_ss_20p_16rm_ass.bin", 0x0000, 0x010000, CRC(17cd6162) SHA1(80129b26db4617281bb6e5aa1f573cf222660303) )
	ROM_LOAD( "rtw816rm", 0x0000, 0x010000, CRC(337264ae) SHA1(5e3e67bd20416331df6e35c6a384d5b88b70aa17) )
	ROM_LOAD( "rtwn8arc.bin", 0x0000, 0x010000, CRC(b054b38e) SHA1(98aa68a4fb6db4a53a63a4976954277c082ee8bf) )

	ROM_REGION( 0x80000, "upd", ROMREGION_ERASE00 )
	/* Nothing?? (missing?) */
ROM_END


ROM_START( sc2prem )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "premier-club-manager_std_ac_250pnd-25p_ass.bin", 0x0000, 0x010000, CRC(404716ed) SHA1(57916fb70621c96eccb0e5bbee821ca2133aaa5f) )

	ROM_REGION( 0x12000, "altrevs", 0 )
	ROM_LOAD( "premclub.bin", 0x0000, 0x010000, CRC(5231ab3e) SHA1(a9e16a5bbeaa0612212d3ef0e78fbc7628cfc0fa) )
	ROM_LOAD( "premier-club-manager_dat_ac_250pnd-25p_ass.bin", 0x0000, 0x010000, CRC(6446176c) SHA1(17cccc00d443ffde11943ebda112ef1e79134455) )
	ROM_LOAD( "premier-club-manager_dat_ac_var_ass.bin", 0x0000, 0x010000, CRC(d1880c7a) SHA1(d1f7891fc8d4570e02c0bfc23e1ed0b159e280c1) )
	ROM_LOAD( "premier-club-manager_std_ac_var_ass.bin", 0x0000, 0x010000, CRC(68e5474e) SHA1(927d41f73e287c71546823ffe829f1e046f3cca6) )

	ROM_REGION( 0x20000, "matrix", 0 )
	ROM_LOAD( "premier-club-manager_mtx_250pnd-25p_ass.bin", 0x0000, 0x010000, CRC(4b4bdb8b) SHA1(de9b52da600629e680fd96f0d82a9f76fbc84bdf) )

	ROM_REGION( 0x20000, "altmatrix", 0 )
	ROM_LOAD( "premier-club-manager_mtx_ass.bin", 0x0000, 0x010000, CRC(7ac2a278) SHA1(f95a7451d1514be19d747707a32bf7280dcfb8b6) )
	ROM_LOAD( "95000570.mtx", 0x0000, 0x004e21, CRC(1b38ddeb) SHA1(86795dcb67306eccabbf0d2a214667497104ef77) )
	ROM_LOAD( "95000571.mtx", 0x0000, 0x004ddc, CRC(0772adea) SHA1(6d3beb1662fd4e1eeef0ca57cdc07f347879bf15) )

	ROM_REGION( 0x80000, "upd", ROMREGION_ERASE00 )
	ROM_LOAD( "premclubsnd.bin", 0x0000, 0x080000, CRC(b20c74f1) SHA1(b43a79f8f59387ef777fffd07a39b7333811d464) )
ROM_END


/* Video Based (Adder 2) */

GAMEL( 1993, qntoondo, qntoond,	  scorpion2_vid, qntoond,   adder_dutch,0,       "BFM/ELAM", "Quintoon (Dutch, Game Card 95-750-136)",		GAME_SUPPORTS_SAVE,layout_quintoon )
GAMEL( 1993, quintoon, 0,		  scorpion2_vid, quintoon,  quintoon,   0,       "BFM",      "Quintoon (UK, Game Card 95-750-206)",			GAME_SUPPORTS_SAVE|GAME_IMPERFECT_SOUND,layout_quintoon ) //Current samples need verification
GAMEL( 1993, quintond, quintoon,  scorpion2_vid, quintoon,  quintoon,   0,       "BFM",      "Quintoon (UK, Game Card 95-751-206, Datapak)",GAME_SUPPORTS_SAVE|GAME_IMPERFECT_SOUND|GAME_NOT_WORKING,layout_quintoon ) //Current samples need verification
GAMEL( 1993, quintono, quintoon,  scorpion2_vid, quintoon,  quintoon,   0,       "BFM",      "Quintoon (UK, Game Card 95-750-203)",			GAME_SUPPORTS_SAVE|GAME_IMPERFECT_SOUND,layout_quintoon ) //Current samples need verification
GAMEL( 1993, qntoond,  0,		  scorpion2_vid, qntoond,   adder_dutch,0,       "BFM/ELAM", "Quintoon (Dutch, Game Card 95-750-243)",		GAME_SUPPORTS_SAVE,layout_quintoon )
GAMEL( 1994, pokio,    0,		  scorpion2_vid, pokio,     adder_dutch,0,       "BFM/ELAM", "Pokio (Dutch, Game Card 95-750-278)",			GAME_SUPPORTS_SAVE,layout_pokio )
GAMEL( 1995, slotsnl,  0,		  scorpion2_vid, slotsnl,   adder_dutch,0,       "BFM/ELAM", "Slots (Dutch, Game Card 95-750-368)",			GAME_SUPPORTS_SAVE,layout_slots )
GAMEL( 1995, paradice, 0,		  scorpion2_vid, paradice,  adder_dutch,0,       "BFM/ELAM", "Paradice (Dutch, Game Card 95-750-615)",		GAME_SUPPORTS_SAVE,layout_paradice )
GAMEL( 1996, pyramid,  0,		  scorpion2_vid, pyramid,   pyramid,	0,       "BFM/ELAM", "Pyramid (Dutch, Game Card 95-750-898)",		GAME_SUPPORTS_SAVE,layout_pyramid )

GAMEL( 1996, sltblgtk, 0,		  scorpion2_vid, sltblgtk,  sltsbelg,   0,       "BFM/ELAM", "Slots (Belgian Token, Game Card 95-750-943)",	GAME_SUPPORTS_SAVE,layout_sltblgtk )
GAMEL( 1996, sltblgpo, 0,		  scorpion2_vid, sltblgpo,  sltsbelg,   0,       "BFM/ELAM", "Slots (Belgian Cash, Game Card 95-750-938)",	GAME_SUPPORTS_SAVE,layout_sltblgpo )
GAMEL( 1996, sltblgp1, sltblgpo,  scorpion2_vid, sltblgpo,  sltsbelg,   0,       "BFM/ELAM", "Slots (Belgian Cash, Game Card 95-752-008)",	GAME_SUPPORTS_SAVE,layout_sltblgpo )
GAMEL( 1997, gldncrwn, 0,		  scorpion2_vid, gldncrwn,  gldncrwn,   0,       "BFM/ELAM", "Golden Crown (Dutch, Game Card 95-752-011)",	GAME_SUPPORTS_SAVE,layout_gldncrwn )

/* Non-Video */

GAMEL( 1994, sc2drwho	, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 1, UK, Game Card 95-750-288) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho1	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 2, UK, Game Card 95-750-661) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho2	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 3) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho3	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 4) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho4	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 5) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho5	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 6) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho6	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 7) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho7	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 8) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho8	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 9) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho9	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 10) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho10	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 11) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho11	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 12) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho12	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 13) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho13	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 14) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho14	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 15) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho15	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 16) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho16	, sc2drwho	,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Dr.Who The Timelord (set 17) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)
GAMEL( 1994, sc2drwho17	, sc2drwho	,  scorpion2		, drwho		, drwhon	, 0,		 "BFM",      "Dr.Who The Timelord (set 18, not encrypted) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_drwho)

GAME( 1994, sc2brkfs	, 0			,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "The Big Breakfast (set 1 UK, Game Card 95-750-524) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 1994, sc2brkfs1	, sc2brkfs	,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "The Big Breakfast (set 2) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 1994, sc2brkfs2	, sc2brkfs	,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "The Big Breakfast (set 3) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 1994, sc2brkfs3	, sc2brkfs	,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "The Big Breakfast (set 4) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 1994, sc2brkfs4	, sc2brkfs	,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "The Big Breakfast (set 5) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 1994, sc2brkfs5	, sc2brkfs	,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "The Big Breakfast (set 6) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)

GAME( 1995, sc2focus	, 0			,  scorpion3		, scorpion3	, focus		, 0,		 "BFM/ELAM", "Focus (Dutch, Game Card 95-750-347) (Scorpion 2/3)", GAME_NOT_WORKING|GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL)
GAME( 1996, sc2gslam	, 0			,  scorpion2		, bfmcgslm	, bfmcgslm	, 0,		 "BFM",      "Grandslam Club (UK, Game Card 95-750-843) (Scorpion 2/3)", GAME_NOT_WORKING|GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL)
GAME( 199?, sc2cshcl	, 0			,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "Cashino Club (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2catms	, 0			,  scorpion2		, bbrkfst	, bbrkfst	, 0,		 "BFM",      "Cat & Mouse (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2eggs		, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Eggs On Legs Tour (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2gsclb	, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "The Game Show Club (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2suprz	, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Surprise Surprize (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2cpg		, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Pharaoh's Gold Club (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2motd		, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Match Of The Day (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2easy		, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Easy Money (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2majes	, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Majestic Bells (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2dels		, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Del's Millions (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2wembl	, 0			,  scorpion2		, drwho		, drwho		, 0,		 "BFM",      "Road To Wembley (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)

// Games with Dot Matrix Displays */

GAME( 1996, sc2luvv		, 0			,  scorpion2_dm01	, luvjub	, luvjub	, 0,		 "BFM",      "Luvvly Jubbly (UK Multisite 10/25p, Game Card 95-750-808) (Scorpion 2/3)", GAME_NOT_WORKING|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL)
GAME( 1996, sc2cpe		, 0			,  scorpion2_dm01	, cpeno1	, cpeno1	, 0,		 "BFM",      "Club Public Enemy No.1 (UK, Game Card 95-750-846) (Scorpion 2/3)", GAME_NOT_WORKING|GAME_REQUIRES_ARTWORK|GAME_MECHANICAL)
GAME( 199?, sc2town		, 0			,  scorpion2_dm01	, drwho		, drwho		, 0,		 "BFM",      "Round The Town (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2ofool	, 0			,  scorpion2_dm01	, drwho		, drwho		, 0,		 "BFM",      "Only Fools & Horses (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2ptytm	, 0			,  scorpion2_dm01	, drwho		, drwho		, 0,		 "BFM",      "Party Time (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2cops		, 0			,  scorpion2_dm01	, drwho		, drwho		, 0,		 "BFM",      "Cops 'n' Robbers (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
GAME( 199?, sc2prem		, 0			,  scorpion2_dm01	, drwho		, drwho		, 0,		 "BFM",      "Premier Club Manager (Bellfruit) (Scorpion 2/3)", GAME_SUPPORTS_SAVE|GAME_REQUIRES_ARTWORK|GAME_NOT_WORKING|GAME_MECHANICAL)
