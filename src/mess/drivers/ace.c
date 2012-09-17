/***************************************************************************
Jupiter Ace memory map

    CPU: Z80
        0000-1fff ROM
        2000-23ff Mirror of 2400-27FF
        2400-27ff RAM (1K RAM used for screen and edit/cassette buffer)
        2800-2bff Mirror of 2C00-2FFF
        2c00-2fff RAM (1K RAM for char set, write only)
        3000-3bff Mirror of 3C00-3FFF
        3c00-3fff RAM (1K RAM standard)
        4000-7fff RAM (16K Expansion)
        8000-ffff RAM (48K Expansion)

Interrupts:

    IRQ:
        50Hz vertical sync

Ports:

    Out 0xfe:
        Tape and buzzer

    In 0xfe:
        Keyboard input, tape, and buzzer

    http://www.jupiter-ace.co.uk/

***************************************************************************/

/*

    TODO:

    - Ace Colour Board
    - 96K ram expansion
    - Deep Thought disc interface (6850, 6821)

*/


#include "emu.h"
#include "cpu/z80/z80.h"
#include "formats/ace_ace.h"
#include "formats/ace_tap.h"
#include "imagedev/cassette.h"
#include "imagedev/snapquik.h"
#include "machine/ctronics.h"
#include "machine/i8255.h"
#include "machine/ram.h"
#include "machine/z80pio.h"
#include "sound/ay8910.h"
#include "sound/sp0256.h"
#include "sound/speaker.h"
#include "sound/wave.h"
#include "includes/ace.h"



//**************************************************************************
//  READ/WRITE HANDLERS
//**************************************************************************

//-------------------------------------------------
//   io_r -
//-------------------------------------------------

READ8_MEMBER( ace_state::io_r )
{
	UINT8 data = 0xff;

	if (!BIT(offset, 8)) data &= ioport("A8")->read();
	if (!BIT(offset, 9)) data &= ioport("A9")->read();
	if (!BIT(offset, 10)) data &= ioport("A10")->read();
	if (!BIT(offset, 11)) data &= ioport("A11")->read();
	if (!BIT(offset, 12)) data &= ioport("A12")->read();
	if (!BIT(offset, 13)) data &= ioport("A13")->read();
	if (!BIT(offset, 14)) data &= ioport("A14")->read();

	if (!BIT(offset, 15))
	{
		data &= ioport("A15")->read();

		m_cassette->output(-1);
		speaker_level_w(m_speaker, 0);
	}

	if (m_cassette->input() > 0)
	{
		data &= ~0x20;
	}

	return data;
}


//-------------------------------------------------
//   io_w -
//-------------------------------------------------

WRITE8_MEMBER( ace_state::io_w )
{
	m_cassette->output(1);
	speaker_level_w(m_speaker, 1);
}


//-------------------------------------------------
//   ppi_r -
//-------------------------------------------------

READ8_MEMBER( ace_state::ppi_pa_r )
{
	return m_ppi->read(space, 0);
}

READ8_MEMBER( ace_state::ppi_pb_r )
{
	return m_ppi->read(space, 1);
}

READ8_MEMBER( ace_state::ppi_pc_r )
{
	return m_ppi->read(space, 2);
}

READ8_MEMBER( ace_state::ppi_control_r )
{
	return m_ppi->read(space, 3);
}


//-------------------------------------------------
//   ppi_w -
//-------------------------------------------------

WRITE8_MEMBER( ace_state::ppi_pa_w )
{
	m_ppi->write(space, 0, data);
}

WRITE8_MEMBER( ace_state::ppi_pb_w )
{
	m_ppi->write(space, 1, data);
}

WRITE8_MEMBER( ace_state::ppi_pc_w )
{
	m_ppi->write(space, 2, data);
}

WRITE8_MEMBER( ace_state::ppi_control_w )
{
	m_ppi->write(space, 3, data);
}


//-------------------------------------------------
//   pio_r -
//-------------------------------------------------

static READ8_DEVICE_HANDLER( pio_ad_r )
{
	return dynamic_cast<z80pio_device*>(device)->data_read(0);
}

static READ8_DEVICE_HANDLER( pio_bd_r )
{
	return dynamic_cast<z80pio_device*>(device)->data_read(1);
}

static READ8_DEVICE_HANDLER( pio_ac_r )
{
	return dynamic_cast<z80pio_device*>(device)->control_read();
}

static READ8_DEVICE_HANDLER( pio_bc_r )
{
	return dynamic_cast<z80pio_device*>(device)->control_read();
}


//-------------------------------------------------
//   pio_w -
//-------------------------------------------------

static WRITE8_DEVICE_HANDLER( pio_ad_w )
{
	dynamic_cast<z80pio_device*>(device)->data_write(0, data);
}

static WRITE8_DEVICE_HANDLER( pio_bd_w )
{
	dynamic_cast<z80pio_device*>(device)->data_write(1, data);
}

static WRITE8_DEVICE_HANDLER( pio_ac_w )
{
	dynamic_cast<z80pio_device*>(device)->control_write(0, data);
}

static WRITE8_DEVICE_HANDLER( pio_bc_w )
{
	dynamic_cast<z80pio_device*>(device)->control_write(1, data);
}



//**************************************************************************
//  ADDRESS MAPS
//**************************************************************************

//-------------------------------------------------
//  ADDRESS_MAP( ace_mem )
//-------------------------------------------------

static ADDRESS_MAP_START( ace_mem, AS_PROGRAM, 8, ace_state )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x23ff) AM_MIRROR(0x0400) AM_RAM AM_SHARE("video_ram")
	AM_RANGE(0x2800, 0x2bff) AM_MIRROR(0x0400) AM_RAM AM_SHARE("char_ram") AM_REGION(Z80_TAG, 0xfc00)
	AM_RANGE(0x3000, 0x33ff) AM_MIRROR(0x0c00) AM_RAM
	AM_RANGE(0x4000, 0xffff) AM_RAM
ADDRESS_MAP_END


//-------------------------------------------------
//  ADDRESS_MAP( ace_io )
//-------------------------------------------------

static ADDRESS_MAP_START( ace_io, AS_IO, 8, ace_state )
	AM_RANGE(0x00, 0x00) AM_MIRROR(0xfffe) AM_MASK(0xff00) AM_READWRITE(io_r, io_w)
	AM_RANGE(0x01, 0x01) AM_MIRROR(0xff00) AM_READ_PORT("JOY")
	AM_RANGE(0x41, 0x41) AM_MIRROR(0xff80) AM_READWRITE(ppi_pa_r, ppi_pa_w)
	AM_RANGE(0x43, 0x43) AM_MIRROR(0xff80) AM_READWRITE(ppi_pb_r, ppi_pb_w)
	AM_RANGE(0x45, 0x45) AM_MIRROR(0xff80) AM_READWRITE(ppi_pc_r, ppi_pc_w)
	AM_RANGE(0x47, 0x47) AM_MIRROR(0xff80) AM_READWRITE(ppi_control_r, ppi_control_w)
	AM_RANGE(0x81, 0x81) AM_MIRROR(0xff38) AM_DEVREADWRITE_LEGACY(Z80PIO_TAG, pio_ad_r, pio_ad_w)
	AM_RANGE(0x83, 0x83) AM_MIRROR(0xff38) AM_DEVREADWRITE_LEGACY(Z80PIO_TAG, pio_bd_r, pio_bd_w)
	AM_RANGE(0x85, 0x85) AM_MIRROR(0xff38) AM_DEVREADWRITE_LEGACY(Z80PIO_TAG, pio_ac_r, pio_ac_w)
	AM_RANGE(0x87, 0x87) AM_MIRROR(0xff38) AM_DEVREADWRITE_LEGACY(Z80PIO_TAG, pio_bc_r, pio_bc_w)
	AM_RANGE(0xfd, 0xfd) AM_MIRROR(0xff00) AM_DEVWRITE_LEGACY(AY8910_TAG, ay8910_address_w)
	AM_RANGE(0xff, 0xff) AM_MIRROR(0xff00) AM_DEVREADWRITE_LEGACY(AY8910_TAG, ay8910_r, ay8910_data_w)
ADDRESS_MAP_END



//**************************************************************************
//  INPUT PORTS
//**************************************************************************

//-------------------------------------------------
//  INPUT_PORTS( ace )
//-------------------------------------------------

static INPUT_PORTS_START( ace )
	PORT_START("A8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_RSHIFT)		PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Symbol Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z)			PORT_CHAR('z') PORT_CHAR('Z') PORT_CHAR(':')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X)			PORT_CHAR('x') PORT_CHAR('X') PORT_CHAR('\xA3')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C)			PORT_CHAR('c') PORT_CHAR('C') PORT_CHAR('?')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A9")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A)			PORT_CHAR('a') PORT_CHAR('A') PORT_CHAR('~')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S)			PORT_CHAR('s') PORT_CHAR('S') PORT_CHAR('|')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D)			PORT_CHAR('d') PORT_CHAR('D') PORT_CHAR('\\')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F)			PORT_CHAR('f') PORT_CHAR('F') PORT_CHAR('{')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G)			PORT_CHAR('g') PORT_CHAR('G') PORT_CHAR('}')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A10")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q)			PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W)			PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E)			PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R)			PORT_CHAR('r') PORT_CHAR('R') PORT_CHAR('<')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T)			PORT_CHAR('t') PORT_CHAR('T') PORT_CHAR('>')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A11")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1)			PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2)			PORT_CHAR('2') PORT_CHAR('@')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3)			PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4)			PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_LEFT) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A12")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0)			PORT_CHAR('0') PORT_CHAR('_')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9)			PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_DOWN) PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_UP)	PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A13")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P)			PORT_CHAR('p') PORT_CHAR('P') PORT_CHAR('"')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O)			PORT_CHAR('o') PORT_CHAR('O') PORT_CHAR(';')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I)			PORT_CHAR('i') PORT_CHAR('I') PORT_CHAR(0x00A9)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U)			PORT_CHAR('u') PORT_CHAR('U') PORT_CHAR(']')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y)			PORT_CHAR('y') PORT_CHAR('Y') PORT_CHAR('[')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A14")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ENTER)		PORT_CHAR(13)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L)			PORT_CHAR('l') PORT_CHAR('L') PORT_CHAR('=')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K)			PORT_CHAR('k') PORT_CHAR('K') PORT_CHAR('+')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J)			PORT_CHAR('j') PORT_CHAR('J') PORT_CHAR('-')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("h  H  \xE2\x86\x91") PORT_CODE(KEYCODE_H) PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("A15")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE)		PORT_CHAR(' ')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M)			PORT_CHAR('m') PORT_CHAR('M') PORT_CHAR('.')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N)			PORT_CHAR('n') PORT_CHAR('N') PORT_CHAR(',')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B)			PORT_CHAR('b') PORT_CHAR('B') PORT_CHAR('*')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V)			PORT_CHAR('v') PORT_CHAR('V') PORT_CHAR('/')
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("JOY")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END



//**************************************************************************
//  VIDEO
//**************************************************************************

//-------------------------------------------------
//  gfx_layout ace_charlayout
//-------------------------------------------------

static const gfx_layout ace_charlayout =
{
	8,8,
	128,
	1,
	{ RGN_FRAC(0,1) },
	{ STEP8(0,1) },
	{ STEP8(0,8) },
	8*8
};


//-------------------------------------------------
//  GFXDECODE( ace )
//-------------------------------------------------

static GFXDECODE_START( ace )
	GFXDECODE_ENTRY( Z80_TAG, 0xfc00, ace_charlayout, 0, 1 )
GFXDECODE_END


//-------------------------------------------------
//  TIMER_DEVICE_CALLBACK( set_irq )
//-------------------------------------------------

static TIMER_DEVICE_CALLBACK( set_irq )
{
	timer.machine().device(Z80_TAG)->execute().set_input_line(INPUT_LINE_IRQ0, ASSERT_LINE);
}


//-------------------------------------------------
//  TIMER_DEVICE_CALLBACK( clear_irq )
//-------------------------------------------------

static TIMER_DEVICE_CALLBACK( clear_irq )
{
	timer.machine().device(Z80_TAG)->execute().set_input_line(INPUT_LINE_IRQ0, CLEAR_LINE);
}


//-------------------------------------------------
//  SCREEN_UPDATE_IND16( ace )
//-------------------------------------------------

UINT32 ace_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	UINT8 y,ra,chr,gfx;
	UINT16 sy=56,ma=0,x;

	for (y = 0; y < 24; y++)
	{
		for (ra = 0; ra < 8; ra++)
		{
			UINT16 *p = &bitmap.pix16(sy++, 40);

			for (x = ma; x < ma+32; x++)
			{
				chr = m_video_ram[x];

				/* get pattern of pixels for that character scanline */
				gfx = m_char_ram[((chr&0x7f)<<3) | ra] ^ (BIT(chr, 7) ? 0xff : 0);

				/* Display a scanline of a character (8 pixels) */
				*p++ = BIT(gfx, 7);
				*p++ = BIT(gfx, 6);
				*p++ = BIT(gfx, 5);
				*p++ = BIT(gfx, 4);
				*p++ = BIT(gfx, 3);
				*p++ = BIT(gfx, 2);
				*p++ = BIT(gfx, 1);
				*p++ = BIT(gfx, 0);
			}
		}
		ma+=32;
	}
	return 0;
}



//**************************************************************************
//  DEVICE CONFIGURATION
//**************************************************************************

//-------------------------------------------------
//  cassette_interface ace_cassette_interface
//-------------------------------------------------

static const cassette_interface ace_cassette_interface =
{
	ace_cassette_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED),
	NULL,
	NULL
};


//-------------------------------------------------
//  ay8910_interface psg_intf
//-------------------------------------------------

static const ay8910_interface psg_intf =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  sp0256_interface sp0256_intf
//-------------------------------------------------

static const sp0256_interface sp0256_intf =
{
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  I8255A_INTERFACE( ppi_intf )
//-------------------------------------------------

static READ8_DEVICE_HANDLER( sby_r )
{
	/*

        bit     description

        PC0     SP0256 SBY
        PC1
        PC2
        PC3
        PC4
        PC5
        PC6
        PC7

    */

	return sp0256_sby_r(device);
}

static WRITE8_DEVICE_HANDLER( ald_w )
{
	/*

        bit     description

        PA0     SP0256 A1
        PA1     SP0256 A2
        PA2     SP0256 A3
        PA3     SP0256 A4
        PA4     SP0256 A5
        PA5     SP0256 A6
        PA6     SP0256 _ALD
        PA7

    */

	if (!BIT(data, 6))
	{
		sp0256_ALD_w(device, space, 0, data & 0x3f);
	}
}

static I8255A_INTERFACE( ppi_intf )
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DEVICE_HANDLER(SP0256AL2_TAG, sby_r),
	DEVCB_DEVICE_HANDLER(SP0256AL2_TAG, ald_w),
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  Z80PIO_INTERFACE( pio_intf )
//-------------------------------------------------

READ8_MEMBER( ace_state::pio_pa_r )
{
	/*

        bit     description

        PA0
        PA1     RxD
        PA2
        PA3
        PA4
        PA5
        PA6
        PA7

    */

	return 0;
};

WRITE8_MEMBER( ace_state::pio_pa_w )
{
	/*

        bit     description

        PA0     RTS
        PA1
        PA2     CTS
        PA3     TxD
        PA4
        PA5
        PA6     STB
        PA7

    */

	// centronics strobe
	m_centronics->strobe_w(!BIT(data, 6));
};

static Z80PIO_INTERFACE( pio_intf )
{
	DEVCB_CPU_INPUT_LINE(Z80_TAG, INPUT_LINE_IRQ0),
	DEVCB_DRIVER_MEMBER(ace_state, pio_pa_r),
	DEVCB_DRIVER_MEMBER(ace_state, pio_pa_w),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DEVICE_MEMBER(CENTRONICS_TAG, centronics_device, write),
	DEVCB_NULL
};



//**************************************************************************
//  MACHINE INITIALIZATION
//**************************************************************************

//-------------------------------------------------
//  MACHINE_START( ace )
//-------------------------------------------------

void ace_state::machine_start()
{
	address_space *program = m_maincpu->space(AS_PROGRAM);

	/* configure RAM */
	switch (m_ram->size())
	{
	case 1*1024:
		program->unmap_readwrite(0x4000, 0xffff);
		break;

	case 16*1024:
		program->unmap_readwrite(0x8000, 0xffff);
		break;

	case 32*1024:
		program->unmap_readwrite(0xc000, 0xffff);
		break;
	}
}



//**************************************************************************
//  MACHINE CONFIGURATION
//**************************************************************************

//-------------------------------------------------
//  MACHINE_CONFIG( ace )
//-------------------------------------------------

static MACHINE_CONFIG_START( ace, ace_state )
	// basic machine hardware
	MCFG_CPU_ADD(Z80_TAG, Z80, XTAL_6_5MHz/2)
	MCFG_CPU_PROGRAM_MAP(ace_mem)
	MCFG_CPU_IO_MAP(ace_io)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))

	// video hardware
	MCFG_SCREEN_ADD(SCREEN_TAG, RASTER)
	MCFG_SCREEN_UPDATE_DRIVER(ace_state, screen_update)
	MCFG_SCREEN_RAW_PARAMS(XTAL_6_5MHz, 416, 0, 336, 312, 0, 304)
	MCFG_TIMER_ADD_SCANLINE("set_irq", set_irq, SCREEN_TAG, 31*8, 264)
	MCFG_TIMER_ADD_SCANLINE("clear_irq", clear_irq, SCREEN_TAG, 32*8, 264)

	MCFG_PALETTE_LENGTH(2)
	MCFG_PALETTE_INIT(black_and_white)

	MCFG_GFXDECODE(ace)

	// sound hardware
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, CASSETTE_TAG)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_ADD(SPEAKER_TAG, SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	MCFG_SOUND_ADD(AY8910_TAG, AY8910, XTAL_6_5MHz/2)
	MCFG_SOUND_CONFIG(psg_intf)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_SOUND_ADD(SP0256AL2_TAG, SP0256, XTAL_3MHz)
	MCFG_SOUND_CONFIG(sp0256_intf)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	// devices
	MCFG_CASSETTE_ADD(CASSETTE_TAG, ace_cassette_interface)
	MCFG_SNAPSHOT_ADD("snapshot", ace, "ace", 1)
	MCFG_I8255A_ADD(I8255_TAG, ppi_intf)
	MCFG_Z80PIO_ADD(Z80PIO_TAG, XTAL_6_5MHz/2, pio_intf)
	MCFG_CENTRONICS_PRINTER_ADD(CENTRONICS_TAG, standard_centronics)

	// internal ram
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("1K")
	MCFG_RAM_EXTRA_OPTIONS("16K,32K,48K")
MACHINE_CONFIG_END



//**************************************************************************
//  ROMS
//**************************************************************************

//-------------------------------------------------
//  ROM( ace )
//-------------------------------------------------

ROM_START( jupace )
	ROM_REGION( 0x10000, Z80_TAG, 0 )
	ROM_LOAD( "rom-a.z1", 0x0000, 0x1000, CRC(dc8438a5) SHA1(8fa97eb71e5dd17c7d190c6587ee3840f839347c) )
	ROM_LOAD( "rom-b.z2", 0x1000, 0x1000, CRC(4009f636) SHA1(98c5d4bcd74bcf014268cf4c00b2007ea5cc21f3) )

	ROM_REGION( 0x1000, "fdc", 0 ) // Deep Thought disc interface
	ROM_LOAD( "dos 4.bin", 0x0000, 0x1000, CRC(04c70448) SHA1(53ddcced6ae2feafd687a3b55864726656b71412) )

	ROM_REGION( 0x10000, SP0256AL2_TAG, 0 )
	ROM_LOAD( "sp0256-al2.ic1", 0x000, 0x800, CRC(b504ac15) SHA1(e60fcb5fa16ff3f3b69d36c7a6e955744d3feafc) )
ROM_END



//**************************************************************************
//  SYSTEM DRIVERS
//**************************************************************************

//    YEAR  NAME     PARENT    COMPAT  MACHINE    INPUT     INIT     COMPANY         FULLNAME      FLAGS
COMP( 1981, jupace,     0,        0,      ace,       ace, driver_device,      0,   "Jupiter Cantab", "Jupiter Ace" , 0 )
