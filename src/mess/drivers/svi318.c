/*
** svi318.c : driver for Spectravideo SVI-318 and SVI-328
**
** Sean Young, Tomas Karlsson
**
** Information taken from: http://www.samdal.com/sv318.htm
**
** SV-318 : 16KB Video RAM, 16KB RAM
** SV-328 : 16KB Video RAM, 64KB RAM
*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "sound/wave.h"
#include "video/mc6845.h"
#include "includes/svi318.h"
#include "video/tms9928a.h"
#include "machine/i8255.h"
#include "machine/wd17xx.h"
#include "bus/centronics/ctronics.h"
#include "imagedev/flopdrv.h"
#include "formats/svi_dsk.h"
#include "imagedev/cartslot.h"
#include "imagedev/cassette.h"
#include "formats/svi_cas.h"
#include "sound/dac.h"
#include "sound/ay8910.h"
#include "machine/ram.h"
#include "rendlay.h"

static ADDRESS_MAP_START( svi318_mem, AS_PROGRAM, 8, svi318_state )
	AM_RANGE( 0x0000, 0x7fff) AM_READ_BANK("bank1") AM_WRITE(svi318_writemem1 )
	AM_RANGE( 0x8000, 0xbfff) AM_READ_BANK("bank2") AM_WRITE(svi318_writemem2 )
	AM_RANGE( 0xc000, 0xffff) AM_READ_BANK("bank3") AM_WRITE(svi318_writemem3 )
ADDRESS_MAP_END

static ADDRESS_MAP_START( svi328_806_mem, AS_PROGRAM, 8, svi318_state )
	AM_RANGE( 0x0000, 0x7fff) AM_READ_BANK("bank1") AM_WRITE(svi318_writemem1 )
	AM_RANGE( 0x8000, 0xbfff) AM_READ_BANK("bank2") AM_WRITE(svi318_writemem2 )
	AM_RANGE( 0xc000, 0xefff) AM_READ_BANK("bank3") AM_WRITE(svi318_writemem3 )
	AM_RANGE( 0xf000, 0xffff) AM_READ_BANK("bank4") AM_WRITE(svi318_writemem4 )
ADDRESS_MAP_END

static ADDRESS_MAP_START( svi318_io, AS_IO, 8, svi318_state )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE( 0x00, 0x38) AM_READWRITE(svi318_io_ext_r, svi318_io_ext_w )
	AM_RANGE( 0x80, 0x80) AM_DEVWRITE( "tms9928a", tms9928a_device, vram_write )
	AM_RANGE( 0x81, 0x81) AM_DEVWRITE( "tms9928a", tms9928a_device, register_write )
	AM_RANGE( 0x84, 0x84) AM_DEVREAD( "tms9928a", tms9928a_device, vram_read )
	AM_RANGE( 0x85, 0x85) AM_DEVREAD( "tms9928a", tms9928a_device, register_read )
	AM_RANGE( 0x88, 0x88) AM_DEVWRITE("ay8910", ay8910_device, address_w )
	AM_RANGE( 0x8c, 0x8c) AM_DEVWRITE("ay8910", ay8910_device, data_w )
	AM_RANGE( 0x90, 0x90) AM_DEVREAD("ay8910", ay8910_device, data_r )
	AM_RANGE( 0x96, 0x97) AM_WRITE(svi318_ppi_w)
	AM_RANGE( 0x98, 0x9a) AM_DEVREAD("ppi8255", i8255_device, read)
ADDRESS_MAP_END

static ADDRESS_MAP_START( svi328_806_io, AS_IO, 8, svi318_state )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE( 0x00, 0x58) AM_READWRITE(svi318_io_ext_r, svi318_io_ext_w )
	AM_RANGE( 0x80, 0x80) AM_DEVWRITE( "tms9928a", tms9928a_device, vram_write )
	AM_RANGE( 0x81, 0x81) AM_DEVWRITE( "tms9928a", tms9928a_device, register_write )
	AM_RANGE( 0x84, 0x84) AM_DEVREAD( "tms9928a", tms9928a_device, vram_read )
	AM_RANGE( 0x85, 0x85) AM_DEVREAD( "tms9928a", tms9928a_device, register_read )
	AM_RANGE( 0x88, 0x88) AM_DEVWRITE("ay8910", ay8910_device, address_w )
	AM_RANGE( 0x8c, 0x8c) AM_DEVWRITE("ay8910", ay8910_device, data_w )
	AM_RANGE( 0x90, 0x90) AM_DEVREAD("ay8910", ay8910_device, data_r )
	AM_RANGE( 0x96, 0x97) AM_WRITE(svi318_ppi_w)
	AM_RANGE( 0x98, 0x9a) AM_DEVREAD("ppi8255", i8255_device, read)
ADDRESS_MAP_END

/*
  Keyboard status table
       Bit#:|  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
            |     |     |     |     |     |     |     |     |
  Line:     |     |     |     |     |     |     |     |     |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    0       | "7" | "6" | "5" | "4" | "3" | "2" | "1" | "0" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    1       | "/" | "." | "=" | "," | "'" | ":" | "9" | "8" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    2       | "G" | "F" | "E" | "D" | "C" | "B" | "A" | "-" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    3       | "O" | "N" | "M" | "L" | "K" | "J" | "I" | "H" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    4       | "W" | "V" | "U" | "T" | "S" | "R" | "Q" | "P" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    5       | UP  | BS  | "]" | "\" | "[" | "Z" | "Y" | "X" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    6       |LEFT |ENTER|STOP | ESC |RGRAP|LGRAP|CTRL |SHIFT|
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    7       |DOWN | INS | CLS | F5  | F4  | F3  | F2  | F1  |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    8       |RIGHT|     |PRINT| SEL |CAPS | DEL | TAB |SPACE|
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
    9*      | "7" | "6" | "5" | "4" | "3" | "2" | "1" | "0" |
   ---------+-----+-----+-----+-----+-----+-----+-----+-----|
   10*      | "," | "." | "/" | "*" | "-" | "+" | "9" | "8" |
   ----------------------------------------------------------
   * Numcerical keypad (SVI-328 only)

2008-05 FP:
Small note about natural keyboard: currently,
- "Keypad ," is not mapped
- "Left Grph" and "Right Grph" are mapped to 'Page Up' and 'Page Down'
- "Stop" is mapped to 'End'
- "Select" is mapped to 'F11'
- "CLS/HM" is mapped to 'Home'
TODO: How are multiple keys (Copy, Cut, Paste, CLS/HM) expected to
behave? Do they need multiple mapping in natural keyboard?
*/

static INPUT_PORTS_START( svi318 )
	PORT_START("LINE0")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)           PORT_CHAR('0') PORT_CHAR(')')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)           PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)           PORT_CHAR('2') PORT_CHAR('@')
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)           PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)           PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)           PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)           PORT_CHAR('6') PORT_CHAR('^')
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)           PORT_CHAR('7') PORT_CHAR('&')

	PORT_START("LINE1")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)           PORT_CHAR('8') PORT_CHAR('*')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)           PORT_CHAR('9') PORT_CHAR('(')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)       PORT_CHAR(':') PORT_CHAR(';')
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE)       PORT_CHAR('\'') PORT_CHAR('"')
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)       PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)      PORT_CHAR('=') PORT_CHAR('+')
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)        PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)       PORT_CHAR('/') PORT_CHAR('?')

	PORT_START("LINE2")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)       PORT_CHAR('-') PORT_CHAR('_')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)           PORT_CHAR('a') PORT_CHAR('A')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)           PORT_CHAR('b') PORT_CHAR('B')
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)           PORT_CHAR('c') PORT_CHAR('C')
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)           PORT_CHAR('d') PORT_CHAR('D')
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)           PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)           PORT_CHAR('f') PORT_CHAR('F')
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)           PORT_CHAR('g') PORT_CHAR('G')

	PORT_START("LINE3")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)           PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)           PORT_CHAR('i') PORT_CHAR('I')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)           PORT_CHAR('0') PORT_CHAR('J')
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)           PORT_CHAR('k') PORT_CHAR('K')
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)           PORT_CHAR('l') PORT_CHAR('L')
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)           PORT_CHAR('m') PORT_CHAR('M')
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)           PORT_CHAR('n') PORT_CHAR('N')
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)           PORT_CHAR('o') PORT_CHAR('O')

	PORT_START("LINE4")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)           PORT_CHAR('p') PORT_CHAR('P')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)           PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)           PORT_CHAR('r') PORT_CHAR('R')
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)           PORT_CHAR('s') PORT_CHAR('S')
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)           PORT_CHAR('t') PORT_CHAR('T')
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)           PORT_CHAR('u') PORT_CHAR('U')
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)           PORT_CHAR('v') PORT_CHAR('V')
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W)           PORT_CHAR('w') PORT_CHAR('W')

	PORT_START("LINE5")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X)           PORT_CHAR('x') PORT_CHAR('X')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)           PORT_CHAR('y') PORT_CHAR('Y')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)           PORT_CHAR('z') PORT_CHAR('Z')
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE)   PORT_CHAR('[') PORT_CHAR('{')
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH)   PORT_CHAR('\\') PORT_CHAR('~')
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE)  PORT_CHAR(']') PORT_CHAR('}')
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSPACE)   PORT_CHAR(8)
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(UTF8_UP) PORT_CODE(KEYCODE_UP)       PORT_CHAR(UCHAR_MAMEKEY(UP))

	PORT_START("LINE6")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT)    PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CODE(KEYCODE_LCONTROL)          PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left Grph") PORT_CODE(KEYCODE_LALT)         PORT_CHAR(UCHAR_MAMEKEY(PGUP))
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rright Grph") PORT_CODE(KEYCODE_RALT)       PORT_CHAR(UCHAR_MAMEKEY(PGDN))
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC)                                 PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Stop") PORT_CODE(KEYCODE_END)               PORT_CHAR(UCHAR_MAMEKEY(END))
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER)                               PORT_CHAR(13)
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(UTF8_LEFT) PORT_CODE(KEYCODE_LEFT)       PORT_CHAR(UCHAR_MAMEKEY(LEFT))

	PORT_START("LINE7")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F1  F6") PORT_CODE(KEYCODE_F1)              PORT_CHAR(UCHAR_MAMEKEY(F1)) PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F2  F7") PORT_CODE(KEYCODE_F2)              PORT_CHAR(UCHAR_MAMEKEY(F2)) PORT_CHAR(UCHAR_MAMEKEY(F7))
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F3  F8") PORT_CODE(KEYCODE_F3)              PORT_CHAR(UCHAR_MAMEKEY(F3)) PORT_CHAR(UCHAR_MAMEKEY(F8))
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F4  F9") PORT_CODE(KEYCODE_F4)              PORT_CHAR(UCHAR_MAMEKEY(F4)) PORT_CHAR(UCHAR_MAMEKEY(F9))
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F5  F10") PORT_CODE(KEYCODE_F5)             PORT_CHAR(UCHAR_MAMEKEY(F5)) PORT_CHAR(UCHAR_MAMEKEY(F10))
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CLS/HM  Copy") PORT_CODE(KEYCODE_HOME)      PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ins  Paste") PORT_CODE(KEYCODE_INSERT)      PORT_CHAR(UCHAR_MAMEKEY(INSERT))
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(UTF8_DOWN) PORT_CODE(KEYCODE_DOWN)       PORT_CHAR(UCHAR_MAMEKEY(DOWN))

	PORT_START("LINE8")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE)                               PORT_CHAR(' ')
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB)                                 PORT_CHAR('\t')
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Del  Cut") PORT_CODE(KEYCODE_DEL)           PORT_CHAR(UCHAR_MAMEKEY(DEL))
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Caps Lock") PORT_CODE(KEYCODE_CAPSLOCK)     PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Select") PORT_CODE(KEYCODE_PAUSE)           PORT_CHAR(UCHAR_MAMEKEY(F11))
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Print") PORT_CODE(KEYCODE_PRTSCR)           PORT_CHAR(UCHAR_MAMEKEY(PRTSCR))
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(UTF8_RIGHT) PORT_CODE(KEYCODE_RIGHT)     PORT_CHAR(UCHAR_MAMEKEY(RIGHT))

	PORT_START("LINE9")
	PORT_BIT (0xff, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("LINE10")
	PORT_BIT (0xff, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("JOYSTICKS")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP)
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN)
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT)
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT)
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP) PORT_PLAYER(2)
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN) PORT_PLAYER(2)
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT) PORT_PLAYER(2)
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(2)

	PORT_START("BUTTONS")
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_BUTTON1)
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( svi328 )

	PORT_INCLUDE( svi318 )

	PORT_MODIFY("LINE9")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0_PAD)           PORT_CHAR(UCHAR_MAMEKEY(0_PAD))
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1_PAD)           PORT_CHAR(UCHAR_MAMEKEY(1_PAD))
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2_PAD)           PORT_CHAR(UCHAR_MAMEKEY(2_PAD))
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3_PAD)           PORT_CHAR(UCHAR_MAMEKEY(3_PAD))
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4_PAD)           PORT_CHAR(UCHAR_MAMEKEY(4_PAD))
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5_PAD)           PORT_CHAR(UCHAR_MAMEKEY(5_PAD))
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6_PAD)           PORT_CHAR(UCHAR_MAMEKEY(6_PAD))
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7_PAD)           PORT_CHAR(UCHAR_MAMEKEY(7_PAD))

	PORT_MODIFY("LINE10")
	PORT_BIT (0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8_PAD)           PORT_CHAR(UCHAR_MAMEKEY(8_PAD))
	PORT_BIT (0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9_PAD)           PORT_CHAR(UCHAR_MAMEKEY(9_PAD))
	PORT_BIT (0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_PLUS_PAD)        PORT_CHAR(UCHAR_MAMEKEY(PLUS_PAD))
	PORT_BIT (0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS_PAD)       PORT_CHAR(UCHAR_MAMEKEY(MINUS_PAD))
	PORT_BIT (0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ASTERISK)        PORT_CHAR(UCHAR_MAMEKEY(ASTERISK))
	PORT_BIT (0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH_PAD)       PORT_CHAR(UCHAR_MAMEKEY(SLASH_PAD))
	PORT_BIT (0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER_PAD)       PORT_CHAR(UCHAR_MAMEKEY(DEL_PAD))
	PORT_BIT (0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Keypad ,") PORT_CODE(KEYCODE_DEL_PAD)
INPUT_PORTS_END

static const ay8910_interface svi318_ay8910_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_DRIVER_MEMBER(svi318_state, svi318_psg_port_a_r),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(svi318_state, svi318_psg_port_b_w)
};

WRITE_LINE_MEMBER(svi318_state::vdp_interrupt)
{
	m_maincpu->set_input_line(0, (state ? HOLD_LINE : CLEAR_LINE));
}

static TMS9928A_INTERFACE(svi318_tms9928a_interface)
{
	0x4000,
	DEVCB_DRIVER_LINE_MEMBER(svi318_state,vdp_interrupt)
};

static const cassette_interface svi318_cassette_interface =
{
	svi_cassette_formats,
	NULL,
	(cassette_state)(CASSETTE_PLAY),
	"svi318_cass",
	NULL
};

static const floppy_interface svi318_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_DSHD,
	LEGACY_FLOPPY_OPTIONS_NAME(svi318),
	"floppy_5_25",
	NULL
};

static MACHINE_CONFIG_FRAGMENT( svi318_cartslot )
	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_EXTENSION_LIST("rom")
	MCFG_CARTSLOT_NOT_MANDATORY
	MCFG_CARTSLOT_INTERFACE("svi318_cart")
	MCFG_CARTSLOT_LOAD(svi318_state,svi318_cart)
	MCFG_CARTSLOT_UNLOAD(svi318_state,svi318_cart)

	/* Software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list","svi318_cart")
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( svi318, svi318_state )
	/* Basic machine hardware */
	MCFG_CPU_ADD( "maincpu", Z80, 3579545 ) /* 3.579545 MHz */
	MCFG_CPU_PROGRAM_MAP( svi318_mem)
	MCFG_CPU_IO_MAP( svi318_io)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))

	MCFG_MACHINE_START_OVERRIDE(svi318_state, svi318_pal )
	MCFG_MACHINE_RESET_OVERRIDE(svi318_state, svi318 )

	MCFG_I8255_ADD( "ppi8255", svi318_ppi8255_interface )

	MCFG_INS8250_ADD( "ins8250_0", svi318_ins8250_interface[0], 1000000 )
	MCFG_INS8250_ADD( "ins8250_1", svi318_ins8250_interface[1], 3072000 )

	/* Video hardware */
	MCFG_TMS9928A_ADD( "tms9928a", TMS9929A, svi318_tms9928a_interface )
	MCFG_TMS9928A_SCREEN_ADD_PAL( "screen" )
	MCFG_SCREEN_UPDATE_DEVICE( "tms9928a", tms9929a_device, screen_update )

	/* Sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("dac", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_ADD("ay8910", AY8910, 1789773)
	MCFG_SOUND_CONFIG(svi318_ay8910_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	/* printer */
	MCFG_CENTRONICS_ADD("centronics", centronics_printers, "printer")
	MCFG_CENTRONICS_BUSY_HANDLER(WRITELINE(svi318_state, write_centronics_busy))

	MCFG_CENTRONICS_OUTPUT_LATCH_ADD("cent_data_out", "centronics")

	MCFG_CASSETTE_ADD( "cassette", svi318_cassette_interface )

	MCFG_FD1793_ADD("wd179x", svi_wd17xx_interface )

	MCFG_LEGACY_FLOPPY_2_DRIVES_ADD(svi318_floppy_interface)

	/* Software lists */
	MCFG_SOFTWARE_LIST_ADD("cass_list","svi318_flop")
	MCFG_SOFTWARE_LIST_ADD("disk_list","svi318_cass")

	MCFG_FRAGMENT_ADD( svi318_cartslot )

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("16K")
	MCFG_RAM_EXTRA_OPTIONS("32K,96K,160K")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svi318n, svi318 )

	MCFG_DEVICE_REMOVE("tms9928a")
	MCFG_DEVICE_REMOVE("screen")
	MCFG_TMS9928A_ADD( "tms9928a", TMS9928A, svi318_tms9928a_interface )
	MCFG_TMS9928A_SCREEN_ADD_NTSC( "screen" )
	MCFG_SCREEN_UPDATE_DEVICE( "tms9928a", tms9928a_device, screen_update )

	MCFG_MACHINE_START_OVERRIDE(svi318_state, svi318_ntsc )
	MCFG_MACHINE_RESET_OVERRIDE(svi318_state, svi318 )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svi328, svi318 )

	/* internal ram */
	MCFG_RAM_MODIFY(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("64K")
	MCFG_RAM_EXTRA_OPTIONS("96K,160K")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svi328n, svi318n )

	/* internal ram */
	MCFG_RAM_MODIFY(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("64K")
	MCFG_RAM_EXTRA_OPTIONS("96K,160K")
MACHINE_CONFIG_END


static MC6845_INTERFACE( svi806_crtc6845_interface )
{
	false,
	0,0,0,0,
	8 /*?*/,
	NULL,
	svi806_crtc6845_update_row,
	NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	NULL
};

/* F4 Character Displayer */
static const gfx_layout svi328_charlayout =
{
	8, 8,                   /* 8 x 16 characters */
	256,                    /* 128 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{  0*8,  1*8,  2*8,  3*8,  4*8,  5*8,  6*8,  7*8, 8*8,  9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	8*16                    /* every char takes 16 bytes */
};

static GFXDECODE_START( svi328 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, svi328_charlayout, 0, 9 )
GFXDECODE_END

static MACHINE_CONFIG_START( svi328_806, svi318_state )
	/* Basic machine hardware */
	MCFG_CPU_ADD( "maincpu", Z80, 3579545 ) /* 3.579545 MHz */
	MCFG_CPU_PROGRAM_MAP( svi328_806_mem)
	MCFG_CPU_IO_MAP( svi328_806_io)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))

	MCFG_MACHINE_START_OVERRIDE(svi318_state, svi318_pal )
	MCFG_MACHINE_RESET_OVERRIDE(svi318_state, svi328_806 )

	MCFG_I8255A_ADD( "ppi8255", svi318_ppi8255_interface )

	MCFG_INS8250_ADD( "ins8250_0", svi318_ins8250_interface[0], 1000000 )
	MCFG_INS8250_ADD( "ins8250_1", svi318_ins8250_interface[1], 3072000 )

	/* Video hardware */
	MCFG_DEFAULT_LAYOUT( layout_dualhsxs )

	MCFG_TMS9928A_ADD( "tms9928a", TMS9929A, svi318_tms9928a_interface )
	MCFG_TMS9928A_SET_SCREEN( "screen" )
	MCFG_TMS9928A_SCREEN_ADD_PAL( "screen" )
	MCFG_SCREEN_UPDATE_DEVICE( "tms9928a", tms9929a_device, screen_update )
	MCFG_PALETTE_ADD("palette", TMS9928A_PALETTE_SIZE + 2)  /* 2 additional entries for monochrome svi806 output */

	MCFG_SCREEN_ADD("svi806", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(640, 400)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 0, 400-1)
	MCFG_SCREEN_UPDATE_DEVICE("crtc", mc6845_device, screen_update)

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", svi328)

	MCFG_MC6845_ADD("crtc", MC6845, "svi806", XTAL_12MHz / 8, svi806_crtc6845_interface)

	MCFG_VIDEO_START_OVERRIDE(svi318_state, svi328_806 )

	/* Sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("dac", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_ADD("ay8910", AY8910, 1789773)
	MCFG_SOUND_CONFIG(svi318_ay8910_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	/* printer */
	MCFG_CENTRONICS_ADD("centronics", centronics_printers, "printer")
	MCFG_CENTRONICS_BUSY_HANDLER(WRITELINE(svi318_state, write_centronics_busy))

	MCFG_CENTRONICS_OUTPUT_LATCH_ADD("cent_data_out", "centronics")

	MCFG_CASSETTE_ADD( "cassette", svi318_cassette_interface )

	MCFG_FD1793_ADD("wd179x", svi_wd17xx_interface )

	MCFG_LEGACY_FLOPPY_2_DRIVES_ADD(svi318_floppy_interface)

	MCFG_FRAGMENT_ADD( svi318_cartslot )

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("64K")
	MCFG_RAM_EXTRA_OPTIONS("96K,160K")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( svi328n_806, svi328_806 )

	MCFG_MACHINE_START_OVERRIDE(svi318_state, svi318_ntsc )

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_REFRESH_RATE(60)
MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( svi318 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_SYSTEM_BIOS(0, "111", "SV BASIC v1.11")
	ROMX_LOAD("svi111.rom", 0x0000, 0x8000, CRC(bc433df6) SHA1(10349ce675f6d6d47f0976e39cb7188eba858d89), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "110", "SV BASIC v1.1")
	ROMX_LOAD("svi110.rom", 0x0000, 0x8000, CRC(709904e9) SHA1(7d8daf52f78371ca2c9443e04827c8e1f98c8f2c), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "100", "SV BASIC v1.0")
	ROMX_LOAD("svi100.rom", 0x0000, 0x8000, CRC(98d48655) SHA1(07758272df475e5e06187aa3574241df1b14035b), ROM_BIOS(3))

	ROM_REGION( 0x8000, "user1", ROMREGION_ERASEFF )
ROM_END

ROM_START( svi318n  )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_SYSTEM_BIOS(0, "111", "SV BASIC v1.11")
	ROMX_LOAD("svi111.rom", 0x0000, 0x8000, CRC(bc433df6) SHA1(10349ce675f6d6d47f0976e39cb7188eba858d89), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "110", "SV BASIC v1.1")
	ROMX_LOAD("svi110.rom", 0x0000, 0x8000, CRC(709904e9) SHA1(7d8daf52f78371ca2c9443e04827c8e1f98c8f2c), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "100", "SV BASIC v1.0")
	ROMX_LOAD("svi100.rom", 0x0000, 0x8000, CRC(98d48655) SHA1(07758272df475e5e06187aa3574241df1b14035b), ROM_BIOS(3))

	ROM_REGION( 0x8000, "user1", ROMREGION_ERASEFF )
ROM_END

ROM_START( svi328 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_SYSTEM_BIOS(0, "111", "SV BASIC v1.11")
	ROMX_LOAD("svi111.rom", 0x0000, 0x8000, CRC(bc433df6) SHA1(10349ce675f6d6d47f0976e39cb7188eba858d89), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "110", "SV BASIC v1.1")
	ROMX_LOAD("svi110.rom", 0x0000, 0x8000, CRC(709904e9) SHA1(7d8daf52f78371ca2c9443e04827c8e1f98c8f2c), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "100", "SV BASIC v1.0")
	ROMX_LOAD("svi100.rom", 0x0000, 0x8000, CRC(98d48655) SHA1(07758272df475e5e06187aa3574241df1b14035b), ROM_BIOS(3))

	ROM_REGION( 0x8000, "user1", ROMREGION_ERASEFF )
ROM_END

ROM_START( svi328n )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_SYSTEM_BIOS(0, "111", "SV BASIC v1.11")
	ROMX_LOAD("svi111.rom", 0x0000, 0x8000, CRC(bc433df6) SHA1(10349ce675f6d6d47f0976e39cb7188eba858d89), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "110", "SV BASIC v1.1")
	ROMX_LOAD("svi110.rom", 0x0000, 0x8000, CRC(709904e9) SHA1(7d8daf52f78371ca2c9443e04827c8e1f98c8f2c), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "100", "SV BASIC v1.0")
	ROMX_LOAD("svi100.rom", 0x0000, 0x8000, CRC(98d48655) SHA1(07758272df475e5e06187aa3574241df1b14035b), ROM_BIOS(3))

	ROM_REGION( 0x8000, "user1", ROMREGION_ERASEFF )
ROM_END

ROM_START( sv328p80 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("svi111.rom", 0x0000, 0x8000, CRC(bc433df6) SHA1(10349ce675f6d6d47f0976e39cb7188eba858d89))

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_SYSTEM_BIOS(0, "english", "English Character Set")
	ROMX_LOAD("svi806.rom",   0x0000, 0x1000, CRC(850bc232) SHA1(ed45cb0e9bd18a9d7bd74f87e620f016a7ae840f), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "swedish", "Swedish Character Set")
	ROMX_LOAD("svi806se.rom", 0x0000, 0x1000, CRC(daea8956) SHA1(3f16d5513ad35692488ae7d864f660e76c6e8ed3), ROM_BIOS(2))

	ROM_REGION( 0x8000, "user1", ROMREGION_ERASEFF )
ROM_END

ROM_START( sv328n80 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("svi111.rom", 0x0000, 0x8000, CRC(bc433df6) SHA1(10349ce675f6d6d47f0976e39cb7188eba858d89))

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_SYSTEM_BIOS(0, "english", "English Character Set")
	ROMX_LOAD("svi806.rom",   0x0000, 0x1000, CRC(850bc232) SHA1(ed45cb0e9bd18a9d7bd74f87e620f016a7ae840f), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "swedish", "Swedish Character Set")
	ROMX_LOAD("svi806se.rom", 0x0000, 0x1000, CRC(daea8956) SHA1(3f16d5513ad35692488ae7d864f660e76c6e8ed3), ROM_BIOS(2))

	ROM_REGION( 0x8000, "user1", ROMREGION_ERASEFF )
ROM_END


/*    YEAR  NAME        PARENT  COMPAT  MACHINE        INPUT   INIT     COMPANY         FULLNAME                    FLAGS */
COMP( 1983, svi318,     0,      0,      svi318,        svi318, svi318_state, svi318,  "Spectravideo", "SVI-318 (PAL)",            0 )
COMP( 1983, svi318n,    svi318, 0,      svi318n,       svi318, svi318_state, svi318,  "Spectravideo", "SVI-318 (NTSC)",           0 )
COMP( 1983, svi328,     svi318, 0,      svi328,        svi328, svi318_state, svi318,  "Spectravideo", "SVI-328 (PAL)",            0 )
COMP( 1983, svi328n,    svi318, 0,      svi328n,       svi328, svi318_state, svi318,  "Spectravideo", "SVI-328 (NTSC)",           0 )
COMP( 1983, sv328p80,   svi318, 0,      svi328_806,    svi328, svi318_state, svi318,  "Spectravideo", "SVI-328 (PAL) + SVI-806 80 column card", 0 )
COMP( 1983, sv328n80,   svi318, 0,      svi328n_806,   svi328, svi318_state, svi318,  "Spectravideo", "SVI-328 (NTSC) + SVI-806 80 column card", 0 )
