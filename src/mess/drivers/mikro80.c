/***************************************************************************

        MIKRO80 driver by Miodrag Milanovic

        10/03/2008 Preliminary driver.

****************************************************************************/


#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "sound/wave.h"
#include "includes/mikro80.h"
#include "imagedev/cartslot.h"
#include "formats/rk_cas.h"
#include "sound/dac.h"

/* Address maps */
static ADDRESS_MAP_START(mikro80_mem, AS_PROGRAM, 8, mikro80_state )
	AM_RANGE( 0x0000, 0x07ff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x0800, 0xdfff ) AM_RAM  // RAM
	AM_RANGE( 0xe000, 0xe7ff ) AM_RAM  AM_SHARE("cursor_ram")// Video RAM
	AM_RANGE( 0xe800, 0xefff ) AM_RAM  AM_SHARE("video_ram") // Video RAM
	AM_RANGE( 0xd000, 0xf7ff ) AM_RAM  // RAM
	AM_RANGE( 0xf800, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( mikro80_io , AS_IO, 8, mikro80_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x01, 0x01) AM_READWRITE(mikro80_tape_r, mikro80_tape_w )
	AM_RANGE( 0x04, 0x07) AM_READWRITE(mikro80_keyboard_r, mikro80_keyboard_w )
ADDRESS_MAP_END

static ADDRESS_MAP_START( kristall_io , AS_IO, 8, mikro80_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x00, 0x03) AM_DEVREADWRITE("ppi8255", i8255_device, read, write)
ADDRESS_MAP_END

static ADDRESS_MAP_START( radio99_io , AS_IO, 8, mikro80_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x01, 0x01) AM_READWRITE(mikro80_tape_r, mikro80_tape_w )
	AM_RANGE( 0x04, 0x04) AM_DEVWRITE("dac", dac_device, write_unsigned8)
	AM_RANGE( 0x05, 0x05) AM_READWRITE(mikro80_8255_portc_r, mikro80_8255_portc_w )
	AM_RANGE( 0x06, 0x06) AM_READ(mikro80_8255_portb_r)
	AM_RANGE( 0x07, 0x07) AM_WRITE(mikro80_8255_porta_w)
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( mikro80 )
	PORT_START("LINE0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('@')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('~')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("<>") PORT_CODE(KEYCODE_MINUS) PORT_CHAR('_')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Up") PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rus/Lat") PORT_CODE(KEYCODE_LALT) PORT_CODE(KEYCODE_RALT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

/* Machine driver */
static const cassette_interface mikro80_cassette_interface =
{
	rk8_cassette_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED | CASSETTE_MOTOR_ENABLED),
	NULL
};


/* F4 Character Displayer */
static const gfx_layout mikro80_charlayout =
{
	8, 8,                   /* 8 x 8 characters */
	256,                    /* 256 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8                 /* every char takes 8 bytes */
};

static GFXDECODE_START( mikro80 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, mikro80_charlayout, 0, 1 )
GFXDECODE_END

static MACHINE_CONFIG_START( mikro80, mikro80_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",I8080, 2000000)
	MCFG_CPU_PROGRAM_MAP(mikro80_mem)
	MCFG_CPU_IO_MAP(mikro80_io)

	MCFG_DEVICE_ADD("ppi8255", I8255, 0)
	MCFG_I8255_OUT_PORTA_CB(WRITE8(mikro80_state, mikro80_8255_porta_w))
	MCFG_I8255_IN_PORTB_CB(READ8(mikro80_state, mikro80_8255_portb_r))
	MCFG_I8255_IN_PORTC_CB(READ8(mikro80_state, mikro80_8255_portc_r))

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0, 64*8-1, 0, 32*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(mikro80_state, screen_update_mikro80)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", mikro80)
	MCFG_PALETTE_ADD_BLACK_AND_WHITE("palette")


	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_CASSETTE_ADD( "cassette", mikro80_cassette_interface )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( radio99, mikro80 )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_IO_MAP(radio99_io)

	MCFG_SOUND_ADD("dac", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 16.00)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( kristall, mikro80 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_IO_MAP(kristall_io)
MACHINE_CONFIG_END


/* ROM definition */

ROM_START( mikro80 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "mikro80.rom", 0xf800, 0x0800, CRC(63a4b72a) SHA1(6bd3e396539a15e2ccffa7486cae06ef6ddd1d03))
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("mikro80.fnt", 0x0000, 0x0800, CRC(43eb72bb) SHA1(761319cc6747661b33e84aa449cec83800543b5b) )
ROM_END

ROM_START( radio99 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "monrk88.bin", 0xf800, 0x0800, CRC(5415D847) SHA1(c8233c72548bc79846b9d998766a10df349c5bda))
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("mikro80.fnt", 0x0000, 0x0800, CRC(43eb72bb) SHA1(761319cc6747661b33e84aa449cec83800543b5b) )
ROM_END

ROM_START( kristall2 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "kristall-2.rom", 0xf800, 0x0800, CRC(e1b5c60f) SHA1(8ce5158def7fca91ec7e11efbb10aa5d70b7c36d))
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD( "kristall-2.fnt", 0x0000, 0x0800, CRC(9661c9f5) SHA1(830c38735dcb1c8a271fa0027f94b4e034848fc8))
ROM_END
/* Driver */
/*    YEAR  NAME    PARENT  COMPAT  MACHINE     INPUT       INIT     COMPANY                  FULLNAME   FLAGS */
COMP( 1983, mikro80,     0,      0, mikro80,    mikro80, mikro80_state, mikro80, "<unknown>", "Mikro-80",    0)
COMP( 1993, radio99, mikro80,    0, radio99,    mikro80, mikro80_state, radio99, "<unknown>", "Radio-99DM",  0)
COMP( 1987, kristall2, mikro80,  0, kristall,   mikro80, mikro80_state, mikro80, "<unknown>", "Kristall-2",  0)
