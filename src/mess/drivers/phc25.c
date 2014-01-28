// license:BSD-3-Clause
// copyright-holders:Curt Coder
/***************************************************************************

        Sanyo PHC-25

        Skeleton driver.

****************************************************************************



    http://www.geocities.jp/sanyo_phc_25/

    Z80 @ 4 MHz
    MC6847 video
    3x 8KB bios ROM
    1x 4KB chargen ROM
    16KB RAM
    6KB video RAM

    LOCK key (CAPSLOCK) selects upper-case/lower-case on international version
    (phc25), and selects hiragana/upper-case on Japanese version (phc25j).



    TODO:

    - MC6847 mode selection lines (should be ok now but need more testing)
    - tune cassette trigger level
    - accurate video timing

    - sound isn't working (should be a keyclick)
    - screen attribute bit 7 is unknown



10 SCREEN3,1,1:COLOR,,1:CLS
20 X1=INT(RND(1)*256):Y1=INT(RND(1)*192):X2=INT(RND(1)*256):Y2=INT(RND(1)*192):C=INT(RND(1)*4)+1:LINE(X1,Y1)-(X2,Y2),C:GOTO 20
RUN


10 SCREEN2,1,1:CLS:FORX=0TO8:LINE(X*24,0)-(X*24+16,191),X,BF:NEXT

*/

#include "includes/phc25.h"

/* Read/Write Handlers */

READ8_MEMBER( phc25_state::port40_r )
{
	/*

	    bit     description

	    0
	    1
	    2
	    3
	    4       vertical sync
	    5       cassette read
	    6       centronics busy
	    7       horizontal sync

	*/

	UINT8 data = 0;

	/* vertical sync */
	data |= !m_vdg->fs_r() << 4;

	/* cassette read */
	data |= ((m_cassette)->input() < +0.3) << 5;

	/* centronics busy */
	data |= m_centronics->busy_r() << 6;

	/* horizontal sync */
	data |= !m_vdg->hs_r() << 7;

	return data;
}

WRITE8_MEMBER( phc25_state::port40_w )
{
	/*

	    bit     description

	    0       cassette output
	    1       cassette motor
	    2       LED in the LOCK button (on = uppercase)
	    3       centronics strobe
	    4
	    5       MC6847 GM1
	    6       MC6847 GM0
	    7       MC6847 A/G

	*/

	/* cassette output */
	m_cassette->output( BIT(data, 0) ? -1.0 : +1.0);

	/* cassette motor */
	m_cassette->change_state(BIT(data,1) ? CASSETTE_MOTOR_DISABLED : CASSETTE_MOTOR_ENABLED, CASSETTE_MASK_MOTOR);

	/* centronics strobe */
	m_centronics->strobe_w(BIT(data, 3));

	/* MC6847 */
	m_ag = BIT(data, 7);
	m_vdg->intext_w(1);
	m_vdg->gm0_w(BIT(data, 5));
	m_vdg->gm1_w(1);
	m_vdg->css_w(BIT(data, 6));
	m_vdg->ag_w(m_ag);
}

/* Memory Maps */

static ADDRESS_MAP_START( phc25_mem, AS_PROGRAM, 8, phc25_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x5fff) AM_ROM AM_REGION(Z80_TAG, 0)
	AM_RANGE(0x6000, 0x77ff) AM_RAM AM_SHARE("video_ram")
	AM_RANGE(0xc000, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( phc25_io, AS_IO, 8, phc25_state )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_DEVWRITE(CENTRONICS_TAG, centronics_device, write)
	AM_RANGE(0x40, 0x40) AM_READWRITE(port40_r, port40_w)
	AM_RANGE(0x80, 0x80) AM_READ_PORT("KEY0")
	AM_RANGE(0x81, 0x81) AM_READ_PORT("KEY1")
	AM_RANGE(0x82, 0x82) AM_READ_PORT("KEY2")
	AM_RANGE(0x83, 0x83) AM_READ_PORT("KEY3")
	AM_RANGE(0x84, 0x84) AM_READ_PORT("KEY4")
	AM_RANGE(0x85, 0x85) AM_READ_PORT("KEY5")
	AM_RANGE(0x86, 0x86) AM_READ_PORT("KEY6")
	AM_RANGE(0x87, 0x87) AM_READ_PORT("KEY7")
	AM_RANGE(0x88, 0x88) AM_READ_PORT("KEY8")
	AM_RANGE(0xc0, 0xc0) AM_DEVWRITE(AY8910_TAG, ay8910_device, address_w)
	AM_RANGE(0xc1, 0xc1) AM_DEVREADWRITE(AY8910_TAG, ay8910_device, data_r, data_w)
ADDRESS_MAP_END

/* Input Ports */

static INPUT_PORTS_START( phc25 )
	PORT_START("KEY0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('s') PORT_CHAR('S')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('x') PORT_CHAR('X')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_UP) PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("INS DEL") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) // unlabeled key

	PORT_START("KEY1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('a') PORT_CHAR('A')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('z') PORT_CHAR('Z')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_DOWN) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("RETURN") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')

	PORT_START("KEY2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('r') PORT_CHAR('R')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('f') PORT_CHAR('F')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('v') PORT_CHAR('V')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_LEFT) PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('^') PORT_CHAR('~')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR('[')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('d') PORT_CHAR('D')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('c') PORT_CHAR('C')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_RIGHT) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR(']')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("SPACE") PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')

	PORT_START("KEY4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_CHAR('y') PORT_CHAR('Y')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N) PORT_CHAR('n') PORT_CHAR('N')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F3") PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('p') PORT_CHAR('P')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('t') PORT_CHAR('T')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('g') PORT_CHAR('G')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('b') PORT_CHAR('B')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F4") PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('@')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('u') PORT_CHAR('U')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('j') PORT_CHAR('J')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('m') PORT_CHAR('M')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F2") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('o') PORT_CHAR('O')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('i') PORT_CHAR('I')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('k') PORT_CHAR('K')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F1") PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('l') PORT_CHAR('L')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')

	PORT_START("KEY8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("GRAPH") PORT_CODE(KEYCODE_LALT) PORT_CHAR(UCHAR_MAMEKEY(LALT))
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("SHIFT") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("CTRL") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("LOCK") PORT_CODE(KEYCODE_CAPSLOCK) PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("JOY0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("JOY1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( phc25j )
	PORT_INCLUDE( phc25 )
INPUT_PORTS_END

/* Video */

READ8_MEMBER( phc25_state::video_ram_r )
{
	if (m_ag) // graphics (to be checked)
	{
		return m_video_ram[offset & 0x17ff];
	}
	else	// text
	{
		offset &= 0x7ff;
		m_vdg->inv_w(BIT(m_video_ram[offset | 0x800], 0)); // cursor attribute
		m_vdg->as_w(BIT(m_video_ram[offset | 0x800], 1)); // screen2 lores attribute
		m_vdg->css_w(BIT(m_video_ram[offset | 0x800], 2)); // css attribute
		// bit 7 is set for all text (not spaces), meaning is unknown
		return m_video_ram[offset];
	}
}

UINT8 phc25_state::pal_char_rom_r(running_machine &machine, UINT8 ch, int line)
{
	phc25_state *state = machine.driver_data<phc25_state>();

	return state->m_char_rom[((ch - 2) * 12) + line + 4];
}

// irq is inverted in emulation, so we need this trampoline
WRITE_LINE_MEMBER( phc25_state::irq_w )
{
	m_maincpu->set_input_line(0, state ? CLEAR_LINE : ASSERT_LINE);
}

UINT8 phc25_state::ntsc_char_rom_r(running_machine &machine, UINT8 ch, int line)
{
	phc25_state *state = machine.driver_data<phc25_state>();

	return state->m_char_rom[(ch * 16) + line];
}

static const mc6847_interface ntsc_vdg_intf =
{
	SCREEN_TAG,
	DEVCB_DRIVER_MEMBER(phc25_state, video_ram_r),

	DEVCB_NULL,                                         /* horizontal sync */
	DEVCB_DRIVER_LINE_MEMBER(phc25_state, irq_w),       /* field sync */

	DEVCB_NULL,                                         /* AG */
	DEVCB_NULL,                                         /* GM2 */
	DEVCB_NULL,                                         /* GM1 */
	DEVCB_NULL,                                         /* GM0 */
	DEVCB_NULL,                                         /* CSS */
	DEVCB_NULL,                                         /* AS */
	DEVCB_NULL,                                         /* INTEXT */
	DEVCB_NULL,                                         /* INV */

	&phc25_state::ntsc_char_rom_r
};

static const mc6847_interface pal_vdg_intf =
{
	SCREEN_TAG,
	DEVCB_DRIVER_MEMBER(phc25_state, video_ram_r),

	DEVCB_NULL,                                         /* horizontal sync */
	DEVCB_DRIVER_LINE_MEMBER(phc25_state, irq_w),       /* field sync */

	DEVCB_NULL,                                         /* AG */
	DEVCB_NULL,                                         /* GM2 */
	DEVCB_NULL,                                         /* GM1 */
	DEVCB_NULL,                                         /* GM0 */
	DEVCB_NULL,                                         /* CSS */
	DEVCB_NULL,                                         /* AS */
	DEVCB_NULL,                                         /* INTEXT */
	DEVCB_NULL,                                         /* INV */

	&phc25_state::pal_char_rom_r
};


void phc25_state::video_start()
{
	/* find memory regions */
	m_char_rom = memregion(Z80_TAG)->base() + 0x5000;
}

/* AY-3-8910 Interface */

static const ay8910_interface ay8910_intf =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_INPUT_PORT("JOY0"),
	DEVCB_INPUT_PORT("JOY1"),
	DEVCB_NULL,
	DEVCB_NULL
};

/* Cassette Configuration */

static const cassette_interface phc25_cassette_interface =
{
	cassette_default_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_MUTED),
	NULL,
	NULL
};

/* Machine Driver */

static MACHINE_CONFIG_START( phc25, phc25_state )
	/* basic machine hardware */
	MCFG_CPU_ADD(Z80_TAG, Z80, XTAL_4MHz)
	MCFG_CPU_PROGRAM_MAP(phc25_mem)
	MCFG_CPU_IO_MAP(phc25_io)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD(AY8910_TAG, AY8910, 1996750)
	MCFG_SOUND_CONFIG(ay8910_intf)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.15)

	/* devices */
	MCFG_CASSETTE_ADD("cassette", phc25_cassette_interface)
	MCFG_CENTRONICS_PRINTER_ADD(CENTRONICS_TAG, standard_centronics)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("16K")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pal, phc25 )
	/* video hardware */
	MCFG_SCREEN_MC6847_PAL_ADD(SCREEN_TAG, MC6847_TAG)
	MCFG_MC6847_ADD(MC6847_TAG, MC6847_PAL, XTAL_4_433619MHz, pal_vdg_intf)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( ntsc, phc25 )
	/* video hardware */
	MCFG_SCREEN_MC6847_NTSC_ADD(SCREEN_TAG, MC6847_TAG)
	MCFG_MC6847_ADD(MC6847_TAG, MC6847_NTSC, XTAL_3_579545MHz, ntsc_vdg_intf)
MACHINE_CONFIG_END

/* ROMs */

ROM_START( phc25 )
	ROM_REGION( 0x6000, Z80_TAG, 0 )
	ROM_LOAD( "phc25rom.0", 0x0000, 0x2000, CRC(fa28336b) SHA1(582376bee455e124de24ba4ac02326c8a592fa5a))
	ROM_LOAD( "phc25rom.1", 0x2000, 0x2000, CRC(38fd578b) SHA1(dc3db78c0cdc89f1605200d39535be65a4091705))
	ROM_LOAD( "phc25rom.2", 0x4000, 0x2000, CRC(54392b27) SHA1(1587827fe9438780b50164727ce3fdea1b98078a))
ROM_END

ROM_START( phc25j )
	ROM_REGION( 0x6000, Z80_TAG, 0 )
	ROM_LOAD( "phc25-11.0", 0x0000, 0x2000, CRC(287e83b0) SHA1(9fe960a8245f28efc04defeeeaceb1e5ec6793b8))
	ROM_LOAD( "phc25-11.1", 0x2000, 0x2000, CRC(6223f945) SHA1(5d44b883b6264cb5d2e21b2269308630c62e0e56))
	ROM_LOAD( "phc25-11.2", 0x4000, 0x2000, CRC(da859ae4) SHA1(6121e85947921e434d0157c378de3d81537f6b9f))
	//ROM_LOAD( "022 00aa.ic", 0x0000, 0x2000, NO_DUMP )
	//ROM_LOAD( "022 01aa.ic", 0x2000, 0x2000, NO_DUMP )
	//ROM_LOAD( "022 02aa.ic", 0x4000, 0x2000, NO_DUMP )
	//ROM_REGION( 0x1000, "chargen", 0 )
	//ROM_LOAD( "022 04a.ic", 0x0000, 0x1000, NO_DUMP )
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT  MACHINE INPUT   CLASS          INIT    COMPANY     FULLNAME            FLAGS */
COMP( 1983, phc25,  0,      0,      pal,    phc25,  driver_device,  0,     "Sanyo",  "PHC-25 (Europe)",  GAME_NOT_WORKING )
COMP( 1983, phc25j, phc25,  0,      ntsc,   phc25j, driver_device,  0,     "Sanyo",  "PHC-25 (Japan)",   GAME_NOT_WORKING )
