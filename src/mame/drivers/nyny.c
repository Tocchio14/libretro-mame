/***************************************************************************

    New York! New York! hardware

    Games Supported:
        * New York! New York! (2 sets)
        * Waga Seishun no Arcadia

    Known issues/to-do's:
        * What does port A on IC37 AY8910 do?  It looks like a DAC, but
          this is not supported by the writes to the port.  All the
          writes are 0x9X, usually 0x9b or 0x9d.  Note that this is
          incorrectly referred to as port B on the schematics, but the
          pin #'s confirm it is, in fact port A.
        * What is the main CPU clock?  11.2Mhz / 16 goes through
          a MC4044 and a MC4024 analog chips before going to the EXTAL
          pin of the M6809

    Notes:
        * The Sigma set has Japanese voice samples, while the Gottlieb
          one is English
        * In cocktail mode New York! New York! programs the CRTC with an
          incorrect value.  Interestingly, when the Flip Screen DIP
          is set, the value programmed is correct.  This bug does not
          exist in Waga Seishun no Arcadia
        * The Crosshatch switch only works on the title screen
        * The Service Mode switch, which displays the total number of
          credits stored in the NVRAM, only works on the "Start Game"
          screen after a coin has been insered.  Hold down the key to
          display the coin count
        * The schematics mixed up port A and B on both AY-8910


    Memory map main cpu (m6809)

    fedcba98
     --------
    000xxxxx  we1   $0000 8k (bitmap)
    100xxxxx  we1   $8000 8k (ram)

    010xxxxx  we2   $4000 8k (bitmap)
    110xxxxx  we2   $C000 8k (ram)

    001xxxxx  we3   $2000 16k x 3bits (color)

    011xxxxx  we4   $6000 16k x 3bits (color)

    10100000  SRAM  $A000 (HB4334P 1024-byte SRAM, but A8/A9 are always 0)
    10100001  CRTC  $A100
    10100010  PIA   $A200
    10100011  SOUND $A300 one latch for read one for write same address

    10101xxx  ROM7  $A800
    10110xxx  ROM6  $B000
    10111xxx  ROM5  $B800

    11100xxx  ROM4  $E000
    11101xxx  ROM3  $E800
    11110xxx  ROM2  $F000
    11111xxx  ROM1  $F800

***************************************************************************/

#include "emu.h"
#include "machine/rescap.h"
#include "machine/6821pia.h"
#include "machine/74123.h"
#include "video/mc6845.h"
#include "cpu/m6800/m6800.h"
#include "cpu/m6809/m6809.h"
#include "sound/ay8910.h"
#include "sound/dac.h"
#include "machine/nvram.h"


#define MAIN_CPU_MASTER_CLOCK		XTAL_11_2MHz
#define PIXEL_CLOCK					(MAIN_CPU_MASTER_CLOCK / 2)
#define CRTC_CLOCK					(MAIN_CPU_MASTER_CLOCK / 16)
#define AUDIO_1_MASTER_CLOCK		XTAL_4MHz
#define AUDIO_CPU_1_CLOCK			AUDIO_1_MASTER_CLOCK
#define AUDIO_2_MASTER_CLOCK		XTAL_4MHz
#define AUDIO_CPU_2_CLOCK			AUDIO_2_MASTER_CLOCK


#define NUM_PENS	   8

class nyny_state : public driver_device
{
public:
	nyny_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_videoram1(*this, "videoram1"),
		m_colorram1(*this, "colorram1"),
		m_videoram2(*this, "videoram2"),
		m_colorram2(*this, "colorram2"){ }

	/* memory pointers */
	required_shared_ptr<UINT8> m_videoram1;
	required_shared_ptr<UINT8> m_colorram1;
	required_shared_ptr<UINT8> m_videoram2;
	required_shared_ptr<UINT8> m_colorram2;

	/* video-related */
	int      m_flipscreen;
	UINT8    m_star_enable;
	UINT16   m_star_delay_counter;
	UINT16   m_star_shift_reg;

	/* devices */
	cpu_device *m_maincpu;
	cpu_device *m_audiocpu;
	device_t *m_audiocpu2;
	device_t *m_ic48_1;
	mc6845_device *m_mc6845;
	pia6821_device *m_pia1;
	pia6821_device *m_pia2;
	pen_t m_pens[NUM_PENS];
	DECLARE_WRITE8_MEMBER(audio_1_command_w);
	DECLARE_WRITE8_MEMBER(audio_1_answer_w);
	DECLARE_WRITE8_MEMBER(audio_2_command_w);
	DECLARE_READ8_MEMBER(nyny_pia_1_2_r);
	DECLARE_WRITE8_MEMBER(nyny_pia_1_2_w);
	DECLARE_WRITE_LINE_MEMBER(main_cpu_irq);
	DECLARE_WRITE_LINE_MEMBER(main_cpu_firq);
	DECLARE_WRITE8_MEMBER(pia_2_port_a_w);
	DECLARE_WRITE8_MEMBER(pia_2_port_b_w);
	DECLARE_WRITE_LINE_MEMBER(flipscreen_w);
	DECLARE_WRITE_LINE_MEMBER(display_enable_changed);
	DECLARE_WRITE8_MEMBER(nyny_ay8910_37_port_a_w);
	virtual void machine_start();
	virtual void machine_reset();
};


/*************************************
 *
 *  Prototypes
 *
 *************************************/





/*************************************
 *
 *  Interrupt generation
 *
 *************************************/

WRITE_LINE_MEMBER(nyny_state::main_cpu_irq)
{
	int combined_state = m_pia1->irq_a_state() | m_pia1->irq_b_state() | m_pia2->irq_b_state();

	m_maincpu->set_input_line(M6809_IRQ_LINE, combined_state ? ASSERT_LINE : CLEAR_LINE);
}


WRITE_LINE_MEMBER(nyny_state::main_cpu_firq)
{
	m_maincpu->set_input_line(M6809_FIRQ_LINE, state ? ASSERT_LINE : CLEAR_LINE);
}



/*************************************
 *
 *  PIA1
 *
 *************************************/

static INTERRUPT_GEN( update_pia_1 )
{
	nyny_state *state = device->machine().driver_data<nyny_state>();

	/* update the different PIA pins from the input ports */

	/* CA1 - copy of PA0 (COIN1) */
	state->m_pia1->ca1_w(state->ioport("IN0")->read() & 0x01);

	/* CA2 - copy of PA1 (SERVICE1) */
	state->m_pia1->ca2_w(state->ioport("IN0")->read() & 0x02);

	/* CB1 - (crosshatch) */
	state->m_pia1->cb1_w(state->ioport("CROSS")->read());

	/* CB2 - NOT CONNECTED */
}


static const pia6821_interface pia_1_intf =
{
	DEVCB_INPUT_PORT("IN0"),		/* port A in */
	DEVCB_INPUT_PORT("IN1"),		/* port B in */
	DEVCB_NULL,						/* line CA1 in */
	DEVCB_NULL,						/* line CB1 in */
	DEVCB_NULL,						/* line CA2 in */
	DEVCB_NULL,						/* line CB2 in */
	DEVCB_NULL,						/* port A out */
	DEVCB_NULL,						/* port B out */
	DEVCB_NULL,						/* line CA2 out */
	DEVCB_NULL,						/* port CB2 out */
	DEVCB_DRIVER_LINE_MEMBER(nyny_state,main_cpu_irq),		/* IRQA */
	DEVCB_DRIVER_LINE_MEMBER(nyny_state,main_cpu_irq)		/* IRQB */
};



/*************************************
 *
 *  PIA2
 *
 *************************************/

WRITE8_MEMBER(nyny_state::pia_2_port_a_w)
{
	m_star_delay_counter = (m_star_delay_counter & 0x0f00) | data;
}


WRITE8_MEMBER(nyny_state::pia_2_port_b_w)
{

	/* bits 0-3 go to bits 8-11 of the star delay counter */
	m_star_delay_counter = (m_star_delay_counter & 0x00ff) | ((data & 0x0f) << 8);

	/* bit 4 is star field enable */
	m_star_enable = data & 0x10;

	/* bits 5-7 go to the music board connector */
	audio_2_command_w(*m_maincpu->space(AS_PROGRAM), 0, data & 0xe0);
}


static const pia6821_interface pia_2_intf =
{
	DEVCB_NULL,						/* port A in */
	DEVCB_NULL,						/* port B in */
	DEVCB_NULL,						/* line CA1 in */
	DEVCB_NULL,						/* line CB1 in */
	DEVCB_NULL,						/* line CA2 in */
	DEVCB_NULL,						/* line CB2 in */
	DEVCB_DRIVER_MEMBER(nyny_state,pia_2_port_a_w),	/* port A out */
	DEVCB_DRIVER_MEMBER(nyny_state,pia_2_port_b_w),	/* port B out */
	DEVCB_DRIVER_LINE_MEMBER(nyny_state,flipscreen_w),		/* line CA2 out */
	DEVCB_NULL,						/* port CB2 out */
	DEVCB_DRIVER_LINE_MEMBER(nyny_state,main_cpu_firq),		/* IRQA */
	DEVCB_DRIVER_LINE_MEMBER(nyny_state,main_cpu_irq)		/* IRQB */
};



/*************************************
 *
 *  IC48 #1 - 74123
 *
 *  This timer is responsible for
 *  delaying the setting of PIA2's
 *  CA1 line.  This delay ensures that
 *  CA1 is only changed in the VBLANK
 *  region, but not in HBLANK
 *
 *************************************/

static WRITE8_DEVICE_HANDLER( ic48_1_74123_output_changed )
{
	nyny_state *state = device->machine().driver_data<nyny_state>();
	state->m_pia2->ca1_w(data);
}


static const ttl74123_interface ic48_1_config =
{
	TTL74123_GROUNDED,	/* the hook up type */
	RES_K(22),			/* resistor connected to RCext */
	CAP_U(0.01),		/* capacitor connected to Cext and RCext */
	1,					/* A pin - driven by the CRTC */
	1,					/* B pin - pulled high */
	1,					/* Clear pin - pulled high */
	ic48_1_74123_output_changed
};



/*************************************
 *
 *  Video system
 *
 *************************************/


WRITE_LINE_MEMBER(nyny_state::flipscreen_w)
{
	m_flipscreen = state ? 0 : 1;
}


static MC6845_BEGIN_UPDATE( begin_update )
{
	nyny_state *state = device->machine().driver_data<nyny_state>();
	/* create the pens */
	offs_t i;

	for (i = 0; i < NUM_PENS; i++)
	{
		state->m_pens[i] = MAKE_RGB(pal1bit(i >> 0), pal1bit(i >> 1), pal1bit(i >> 2));
	}

	return state->m_pens;
}


static MC6845_UPDATE_ROW( update_row )
{
	nyny_state *state = device->machine().driver_data<nyny_state>();
	UINT8 cx;
	pen_t *pens = (pen_t *)param;
	UINT8 x = 0;

	for (cx = 0; cx < x_count; cx++)
	{
		int i;
		UINT8 data1, data2, color1, color2;

		/* the memory is hooked up to the MA, RA lines this way */
		offs_t offs = ((ma << 5) & 0x8000) |
					  ((ma << 3) & 0x1f00) |
					  ((ra << 5) & 0x00e0) |
					  ((ma << 0) & 0x001f);

		if (state->m_flipscreen)
			offs = offs ^ 0x9fff;

		data1 = state->m_videoram1[offs];
		data2 = state->m_videoram2[offs];
		color1 = state->m_colorram1[offs] & 0x07;
		color2 = state->m_colorram2[offs] & 0x07;

		for (i = 0; i < 8; i++)
		{
			UINT8 bit1, bit2, color;

			if (state->m_flipscreen)
			{
				bit1 = BIT(data1, 7);
				bit2 = BIT(data2, 7);

				data1 = data1 << 1;
				data2 = data2 << 1;
			}
			else
			{
				bit1 = BIT(data1, 0);
				bit2 = BIT(data2, 0);

				data1 = data1 >> 1;
				data2 = data2 >> 1;
			}

			/* plane 1 has priority over plane 2 */
			if (bit1)
				color = color1;
			else
				color = bit2 ? color2 : 0;

			bitmap.pix32(y, x) = pens[color];

			x += 1;
		}

		ma += 1;
	}
}


INLINE void shift_star_generator( running_machine &machine )
{
	nyny_state *state = machine.driver_data<nyny_state>();
	state->m_star_shift_reg = (state->m_star_shift_reg << 1) | (((~state->m_star_shift_reg >> 15) & 0x01) ^ ((state->m_star_shift_reg >> 2) & 0x01));
}


static MC6845_END_UPDATE( end_update )
{
	nyny_state *state = device->machine().driver_data<nyny_state>();

	/* draw the star field into the bitmap */
	int y;

	pen_t *pens = (pen_t *)param;
	UINT16 delay_counter = state->m_star_delay_counter;

	for (y = cliprect.min_y; y <= cliprect.max_y; y++)
	{
		int x;

		for (x = cliprect.min_x; x <= cliprect.max_x; x++)
		{
			/* check if the star status */
			if (state->m_star_enable &&
			    (bitmap.pix32(y, x) == pens[0]) &&
			    ((state->m_star_shift_reg & 0x80ff) == 0x00ff) &&
			    (((y & 0x01) ^ state->m_flipscreen) ^ (((x & 0x08) >> 3) ^ state->m_flipscreen)))
			{
				UINT8 color = ((state->m_star_shift_reg & 0x0100) >>  8) |	/* R */
							  ((state->m_star_shift_reg & 0x0400) >>  9) |	/* G */
							  ((state->m_star_shift_reg & 0x1000) >> 10);		/* B */

				bitmap.pix32(y, x) = pens[color];
			}

			if (delay_counter == 0)
				shift_star_generator(device->machine());
			else
				delay_counter = delay_counter - 1;
		}
	}
}


WRITE_LINE_MEMBER(nyny_state::display_enable_changed)
{
	ttl74123_a_w(m_ic48_1, 0, state);
}


static const mc6845_interface mc6845_intf =
{
	"screen",				/* screen we are acting on */
	8,						/* number of pixels per video memory address */
	begin_update,			/* before pixel update callback */
	update_row,				/* row update callback */
	end_update,				/* after pixel update callback */
	DEVCB_DRIVER_LINE_MEMBER(nyny_state,display_enable_changed),	/* callback for display state changes */
	DEVCB_NULL,				/* callback for cursor state changes */
	DEVCB_NULL,				/* HSYNC callback */
	DEVCB_NULL,				/* VSYNC callback */
	NULL					/* update address callback */
};



/*************************************
 *
 *  Audio system - CPU 1
 *
 *************************************/

WRITE8_MEMBER(nyny_state::audio_1_command_w)
{

	soundlatch_byte_w(space, 0, data);
	m_audiocpu->set_input_line(M6800_IRQ_LINE, HOLD_LINE);
}


WRITE8_MEMBER(nyny_state::audio_1_answer_w)
{

	soundlatch3_byte_w(space, 0, data);
	m_maincpu->set_input_line(M6809_IRQ_LINE, HOLD_LINE);
}


WRITE8_MEMBER(nyny_state::nyny_ay8910_37_port_a_w)
{
	/* not sure what this does */

	/*logerror("%x PORT A write %x at  Y=%x X=%x\n", space->device().safe_pc(), data, space->machine().primary_screen->vpos(), space->machine().primary_screen->hpos());*/
}


static const ay8910_interface ay8910_37_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(nyny_state,nyny_ay8910_37_port_a_w),
	DEVCB_DEVICE_MEMBER("dac", dac_device, write_unsigned8)
};


static const ay8910_interface ay8910_64_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_INPUT_PORT("SW2"),
	DEVCB_INPUT_PORT("SW1"),
	DEVCB_NULL,
	DEVCB_NULL
};



/*************************************
 *
 *  Audio system - CPU 2
 *
 *************************************/

WRITE8_MEMBER(nyny_state::audio_2_command_w)
{

	soundlatch2_byte_w(space, 0, (data & 0x60) >> 5);
	m_audiocpu2->execute().set_input_line(M6800_IRQ_LINE, BIT(data, 7) ? CLEAR_LINE : ASSERT_LINE);
}



/*************************************
 *
 *  Memory handlers
 *
 *************************************/

READ8_MEMBER(nyny_state::nyny_pia_1_2_r)
{
	UINT8 ret = 0;

	/* the address bits are directly connected to the chip selects */
	if (BIT(offset, 2))  ret = m_pia1->read(space, offset & 0x03);
	if (BIT(offset, 3))  ret = m_pia2->read_alt(space, offset & 0x03);

	return ret;
}


WRITE8_MEMBER(nyny_state::nyny_pia_1_2_w)
{

	/* the address bits are directly connected to the chip selects */
	if (BIT(offset, 2))  m_pia1->write(space, offset & 0x03, data);
	if (BIT(offset, 3))  m_pia2->write_alt(space, offset & 0x03, data);
}


static ADDRESS_MAP_START( nyny_main_map, AS_PROGRAM, 8, nyny_state )
	AM_RANGE(0x0000, 0x1fff) AM_RAM AM_SHARE("videoram1")
	AM_RANGE(0x2000, 0x3fff) AM_RAM AM_SHARE("colorram1")
	AM_RANGE(0x4000, 0x5fff) AM_RAM AM_SHARE("videoram2")
	AM_RANGE(0x6000, 0x7fff) AM_RAM AM_SHARE("colorram2")
	AM_RANGE(0x8000, 0x9fff) AM_RAM
	AM_RANGE(0xa000, 0xa0ff) AM_RAM AM_SHARE("nvram") /* SRAM (coin counter, shown when holding F2) */
	AM_RANGE(0xa100, 0xa100) AM_MIRROR(0x00fe) AM_DEVWRITE("crtc", mc6845_device, address_w)
	AM_RANGE(0xa101, 0xa101) AM_MIRROR(0x00fe) AM_DEVWRITE("crtc", mc6845_device, register_w)
	AM_RANGE(0xa200, 0xa20f) AM_MIRROR(0x00f0) AM_READWRITE(nyny_pia_1_2_r, nyny_pia_1_2_w)
	AM_RANGE(0xa300, 0xa300) AM_MIRROR(0x00ff) AM_READ(soundlatch3_byte_r) AM_WRITE(audio_1_command_w)
	AM_RANGE(0xa400, 0xa7ff) AM_NOP
	AM_RANGE(0xa800, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xdfff) AM_RAM
	AM_RANGE(0xe000, 0xffff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( nyny_audio_1_map, AS_PROGRAM, 8, nyny_state )
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)
	AM_RANGE(0x0000, 0x007f) AM_RAM		/* internal RAM */
	AM_RANGE(0x0080, 0x0fff) AM_NOP
	AM_RANGE(0x1000, 0x1000) AM_MIRROR(0x0fff) AM_READ(soundlatch_byte_r) AM_WRITE(audio_1_answer_w)
	AM_RANGE(0x2000, 0x2000) AM_MIRROR(0x0fff) AM_READ_PORT("SW3")
	AM_RANGE(0x3000, 0x3000) AM_MIRROR(0x0ffc) AM_DEVREAD_LEGACY("ay1", ay8910_r)
	AM_RANGE(0x3000, 0x3001) AM_MIRROR(0x0ffc) AM_DEVWRITE_LEGACY("ay1", ay8910_data_address_w)
	AM_RANGE(0x3002, 0x3002) AM_MIRROR(0x0ffc) AM_DEVREAD_LEGACY("ay2", ay8910_r)
	AM_RANGE(0x3002, 0x3003) AM_MIRROR(0x0ffc) AM_DEVWRITE_LEGACY("ay2", ay8910_data_address_w)
	AM_RANGE(0x4000, 0x4fff) AM_NOP
	AM_RANGE(0x5000, 0x57ff) AM_MIRROR(0x0800) AM_ROM
	AM_RANGE(0x6000, 0x67ff) AM_MIRROR(0x0800) AM_ROM
	AM_RANGE(0x7000, 0x77ff) AM_MIRROR(0x0800) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( nyny_audio_2_map, AS_PROGRAM, 8, nyny_state )
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)
	AM_RANGE(0x0000, 0x007f) AM_RAM		/* internal RAM */
	AM_RANGE(0x0080, 0x0fff) AM_NOP
	AM_RANGE(0x1000, 0x1000) AM_MIRROR(0x0fff) AM_READ(soundlatch2_byte_r)
	AM_RANGE(0x2000, 0x2000) AM_MIRROR(0x0ffe) AM_DEVREAD_LEGACY("ay3", ay8910_r)
	AM_RANGE(0x2000, 0x2001) AM_MIRROR(0x0ffe) AM_DEVWRITE_LEGACY("ay3", ay8910_data_address_w)
	AM_RANGE(0x3000, 0x6fff) AM_NOP
	AM_RANGE(0x7000, 0x77ff) AM_MIRROR(0x0800) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definition
 *
 *************************************/

static INPUT_PORTS_START( nyny )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )     /* PIA0 PA0 */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SERVICE1 )  /* PIA0 PA1 */
	PORT_SERVICE_NO_TOGGLE(0x04, IP_ACTIVE_HIGH)	/* PIA0 PA2 */
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )	/* PIA0 PA3 */
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL /* PIA0 PA4 */
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )	/* PIA0 PA5 */
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )	/* PIA0 PA6 */
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_2WAY PORT_COCKTAIL /* PIA0 PB0 */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_2WAY PORT_COCKTAIL /* PIA0 PB1 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_2WAY	/* PIA0 PB2 */
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_2WAY	/* PIA0 PB3 */
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("SW1")
	PORT_DIPNAME( 0x03, 0x03, "Bombs from UFO (Screens 3+)" ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(	0x01, "3" )
	PORT_DIPSETTING(	0x00, "6" )
	PORT_DIPSETTING(	0x03, "9" )
	PORT_DIPSETTING(	0x02, "12" )
	PORT_DIPNAME( 0x04, 0x00, "Bombs from UFO (Screens 1 and 2)" ) PORT_DIPLOCATION("SW1:3")
	PORT_DIPSETTING(	0x04, "6" )
	PORT_DIPSETTING(	0x00, "9" )
	PORT_DIPNAME( 0x80, 0x80, "Voice Volume" ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(	0x00, DEF_STR( Low ) )
	PORT_DIPSETTING(	0x80, DEF_STR( High ) )

	PORT_START("SW2")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(	0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x18, 0x00, "Bonus Game" ) PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(	0x18, "No Bonus Game" )
	PORT_DIPSETTING(	0x10, "5000" )
	PORT_DIPSETTING(	0x00, "10000" )
	PORT_DIPSETTING(	0x08, "15000" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(	0x00, "3000" )
	PORT_DIPSETTING(	0x40, "5000" )
	PORT_DIPNAME( 0x80, 0x80, "Bonus Life Awarded" ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(	0x00, DEF_STR( No ) )
	PORT_DIPSETTING(	0x80, DEF_STR( Yes ) )

	PORT_START("SW3")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(	0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(	0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW3:2")
	PORT_DIPSETTING(	0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(	0x02, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x1c, 0x00, "Vertical Screen Position" ) PORT_DIPLOCATION("SW3:3,4,5")
	PORT_DIPSETTING(	0x1c, "-1" )
	PORT_DIPSETTING(	0x18, "-2" )
	PORT_DIPSETTING(	0x14, "-3" )
	PORT_DIPSETTING(	0x00, "Neutral" )
	PORT_DIPSETTING(	0x04, "+1" )
	PORT_DIPSETTING(	0x08, "+2" )
	PORT_DIPSETTING(	0x0c, "+3" )
	PORT_DIPNAME( 0xe0, 0x00, "Horizontal Screen Position" ) PORT_DIPLOCATION("SW3:6,7,8")
	PORT_DIPSETTING(	0xe0, "-1" )
	PORT_DIPSETTING(	0xc0, "-2" )
	PORT_DIPSETTING(	0xa0, "-3" )
	PORT_DIPSETTING(	0x00, "Neutral" )
	PORT_DIPSETTING(	0x60, "+1" )
	PORT_DIPSETTING(	0x40, "+2" )
	PORT_DIPSETTING(	0x20, "+3" )

	PORT_START("CROSS")		/* connected to PIA1 CB1 input */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("PS1 (Crosshatch)") PORT_CODE(KEYCODE_F1)

INPUT_PORTS_END



/*************************************
 *
 *  Machine start & reset
 *
 *************************************/

void nyny_state::machine_start()
{

	m_maincpu = machine().device<cpu_device>("maincpu");
	m_audiocpu = machine().device<cpu_device>("audiocpu");
	m_audiocpu2 = machine().device("audio2");
	m_ic48_1 = machine().device("ic48_1");
	m_mc6845 = machine().device<mc6845_device>("crtc");
	m_pia1 = machine().device<pia6821_device>("pia1");
	m_pia2 = machine().device<pia6821_device>("pia2");

	/* setup for save states */
	save_item(NAME(m_flipscreen));
	save_item(NAME(m_star_enable));
	save_item(NAME(m_star_delay_counter));
	save_item(NAME(m_star_shift_reg));
}

void nyny_state::machine_reset()
{

	m_flipscreen = 0;
	m_star_enable = 0;
	m_star_delay_counter = 0;
	m_star_shift_reg = 0;
}

/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_CONFIG_START( nyny, nyny_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6809, 1400000)	/* 1.40 MHz? The clock signal is generated by analog chips */
	MCFG_CPU_PROGRAM_MAP(nyny_main_map)
	MCFG_CPU_PERIODIC_INT(update_pia_1, 25)

	MCFG_CPU_ADD("audiocpu", M6802, AUDIO_CPU_1_CLOCK)
	MCFG_CPU_PROGRAM_MAP(nyny_audio_1_map)

	MCFG_CPU_ADD("audio2", M6802, AUDIO_CPU_2_CLOCK)
	MCFG_CPU_PROGRAM_MAP(nyny_audio_2_map)

	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_RAW_PARAMS(PIXEL_CLOCK, 256, 0, 256, 256, 0, 256)	/* temporary, CRTC will configure screen */
	MCFG_SCREEN_UPDATE_DEVICE("crtc", mc6845_device, screen_update)

	MCFG_MC6845_ADD("crtc", MC6845, CRTC_CLOCK, mc6845_intf)

	/* 74LS123 */
	MCFG_TTL74123_ADD("ic48_1", ic48_1_config)

	MCFG_PIA6821_ADD("pia1", pia_1_intf)
	MCFG_PIA6821_ADD("pia2", pia_2_intf)

	/* audio hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("ay1", AY8910, AUDIO_CPU_1_CLOCK)
	MCFG_SOUND_CONFIG(ay8910_37_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_SOUND_ADD("ay2", AY8910, AUDIO_CPU_1_CLOCK)
	MCFG_SOUND_CONFIG(ay8910_64_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_SOUND_ADD("ay3", AY8910, AUDIO_CPU_2_CLOCK)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.03)

	MCFG_DAC_ADD("dac")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( nyny )
	ROM_REGION(0x10000, "maincpu", 0)	/* main CPU */
	ROM_LOAD( "nyny01s.100",  0xa800, 0x0800, CRC(a2b76eca) SHA1(e46717e6ad330be4c4e7d9fab4f055f89aa31bcc) )
	ROM_LOAD( "nyny02s.099",  0xb000, 0x0800, CRC(ef2d4dae) SHA1(718c0ecf7770a780aebb1dc8bf4ca86ea0a5ea28) )
	ROM_LOAD( "nyny03s.098",  0xb800, 0x0800, CRC(2734c229) SHA1(b028d057d26838bae50b8ddb90a3755b5315b4ee) )
	ROM_LOAD( "nyny04s.097",  0xe000, 0x0800, CRC(bd94087f) SHA1(02dde604bb84097fcd95c434847c55198b4e4309) )
	ROM_LOAD( "nyny05s.096",  0xe800, 0x0800, CRC(248b22c4) SHA1(d64d89bf78fa19d36e02720c296a60621ab8fe21) )
	ROM_LOAD( "nyny06s.095",  0xf000, 0x0800, CRC(8c073052) SHA1(0ce103ac0e79124ac9f1e097dda1a0664b92b89b) )
	ROM_LOAD( "nyny07s.094",  0xf800, 0x0800, CRC(d49d7429) SHA1(c12eaae7ba0b1d44c45a584232db03c5731c046a) )

	ROM_REGION(0x10000, "audiocpu", 0)	/* first audio CPU */
	ROM_LOAD( "nyny08.093",   0x5000, 0x0800, CRC(19ddb6c3) SHA1(0097fad542f9a33849565093c2fb106d90007b1a) )
	ROM_LOAD( "nyny09.092",   0x6000, 0x0800, CRC(a359c6f1) SHA1(1bc7b487581399908c3cec823733810fb6d944ce) )
	ROM_LOAD( "nyny10.091",   0x7000, 0x0800, CRC(a72a70fa) SHA1(deed7dec9cc43fa1d6c4854ba18169c894c9a2f0) )

	ROM_REGION(0x10000, "audio2", 0) /* second audio CPU */
	ROM_LOAD( "nyny11.snd",   0x7000, 0x0800, CRC(650450fc) SHA1(214693df394ca05eff5dbe1e800107d326ba80f6) )
ROM_END


ROM_START( nynyg )
	ROM_REGION(0x10000, "maincpu", 0)	/* main CPU */
	ROM_LOAD( "gny1.cpu",     0xa800, 0x0800, CRC(fb5b8f17) SHA1(2202325451dfd4e7c16cba93f0fade46929ffa72) )
	ROM_LOAD( "gny2.cpu",     0xb000, 0x0800, CRC(d248dd93) SHA1(0c4579698f8917332041c08af6902b8f8acd7d62) )
	ROM_LOAD( "gny3.cpu",     0xb800, 0x0800, CRC(223a9d09) SHA1(c2b12270d375587489208d6a1b37a4e3ec87bc20) )
	ROM_LOAD( "gny4.cpu",     0xe000, 0x0800, CRC(7964ec1f) SHA1(dba3dc2e928fb3fc04a9dca12951343669a4ecbe) )
	ROM_LOAD( "gny5.cpu",     0xe800, 0x0800, CRC(4799dcfc) SHA1(13dcc4a58a029c14a4e9acd0bf584c71d5302c03) )
	ROM_LOAD( "gny6.cpu",     0xf000, 0x0800, CRC(4839d4d2) SHA1(cfd6f2f252ee2f6a4d881496a017c02d7dd77944) )
	ROM_LOAD( "gny7.cpu",     0xf800, 0x0800, CRC(b7564c5b) SHA1(e1d8fe7f37aa7aa98f18c538fe6e688675cc2de1) )

	ROM_REGION(0x10000, "audiocpu", 0)	/* first audio CPU */
	ROM_LOAD( "gny8.cpu",     0x5000, 0x0800, CRC(e0bf7d00) SHA1(7afca3affa413179f4f59ce2cad89525cfa5efbc) )
	ROM_LOAD( "gny9.cpu",     0x6000, 0x0800, CRC(639bc81a) SHA1(91819d49099e438ac8c70920a787aeaed3ed82e9) )
	ROM_LOAD( "gny10.cpu",    0x7000, 0x0800, CRC(73764021) SHA1(bb2f62130142487afbd8d2540e2d4fe5bb67c4ee) )

	ROM_REGION(0x10000, "audio2", 0) /* second audio CPU */
	/* The original dump of this ROM was bad [FIXED BITS (x1xxxxxx)] */
	/* Since what's left is identical to the Sigma version, I'm assuming it's the same. */
	ROM_LOAD( "nyny11.snd",   0x7000, 0x0800, CRC(650450fc) SHA1(214693df394ca05eff5dbe1e800107d326ba80f6) )
ROM_END


ROM_START( warcadia )
	ROM_REGION(0x10000, "maincpu", 0)	/* main CPU */
	ROM_LOAD( "ar-01",        0xa800, 0x0800, CRC(7b7e8f27) SHA1(2bb1d07d87ad5b952de9460c840d7e8b59ed1b4a) )
	ROM_LOAD( "ar-02",        0xb000, 0x0800, CRC(81d9e172) SHA1(4279582f1edf54f0974fa277565d8ade6d9faa50) )
	ROM_LOAD( "ar-03",        0xb800, 0x0800, CRC(2c5feb05) SHA1(6f8952e7744ba7d7b8b345d67f546b504f7a3b30) )
	ROM_LOAD( "ar-04",        0xe000, 0x0800, CRC(66fcbd7f) SHA1(7b8c09593b7d0d25cbe0b28097d58772c32f13bb) )
	ROM_LOAD( "ar-05",        0xe800, 0x0800, CRC(b2320e20) SHA1(977afc2d26ef500eff4499e6bc61f14314b19130) )
	ROM_LOAD( "ar-06",        0xf000, 0x0800, CRC(27b79cc0) SHA1(2c5c3a9a09069751c5e9c23d0840ee4996006c0b) )
	ROM_LOAD( "ar-07",        0xf800, 0x0800, CRC(be77a477) SHA1(817c069855634dd844f0068d64bfbf1862980d6b) )

	ROM_REGION(0x10000, "audiocpu", 0)	/* first audio CPU */
	ROM_LOAD( "ar-08",        0x5000, 0x0800, CRC(38569b25) SHA1(887a9afaa65d0961097f7fb5f1ae390d40e9c164) )
	ROM_LOAD( "nyny09.092",   0x6000, 0x0800, CRC(a359c6f1) SHA1(1bc7b487581399908c3cec823733810fb6d944ce) )
	ROM_LOAD( "nyny10.091",   0x7000, 0x0800, CRC(a72a70fa) SHA1(deed7dec9cc43fa1d6c4854ba18169c894c9a2f0) )

	ROM_REGION(0x10000, "audio2", 0) /* second audio CPU */
	ROM_LOAD( "ar-11",        0x7000, 0x0800, CRC(208f4488) SHA1(533f8942e1c964cc88253e9dc4ec711f77607e4c) )
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1980, nyny,    0,    nyny, nyny, driver_device, 0, ROT270, "Sigma Enterprises Inc.", "New York! New York!", GAME_IMPERFECT_SOUND | GAME_SUPPORTS_SAVE )
GAME( 1980, nynyg,   nyny, nyny, nyny, driver_device, 0, ROT270, "Sigma Enterprises Inc. (Gottlieb license)", "New York! New York! (Gottlieb)", GAME_IMPERFECT_SOUND  | GAME_SUPPORTS_SAVE )
GAME( 1980, warcadia,nyny, nyny, nyny, driver_device, 0, ROT270, "Sigma Enterprises Inc.", "Waga Seishun no Arcadia", GAME_IMPERFECT_SOUND | GAME_SUPPORTS_SAVE )
