/*************************************************************************************

    Cybiko Wireless Inter-tainment System

    (c) 2001-2007 Tim Schuerewegen

    Cybiko Classic (V1)
    Cybiko Classic (V2)
    Cybiko Xtreme

ToDo:
- Remove the memory leak in the nvram handler.
- Provide facility to load games via a "cart".
- Need instructions! The black screen doesn't fill me with confidence..unable to test.

**************************************************************************************/


#include "includes/cybiko.h"
#include "rendlay.h"

//  +------------------------------------------------------+
//  | Cybiko Classic (CY6411)                         | V2 |
//  +------------------------------------------------------+
//  | - CYBIKO | CY-OS 1.1.7 | 6432241M04FA | 0028R JAPAN  |
//  | - SST 39VF020 | 90-4C-WH | 0012175-D                 |
//  | - ATMEL 0027 | AT90S2313-4SC                         |
//  | - ATMEL | AT45DB041A | TC | 0027                     |
//  | - RF2915 | RFMD0028 | 0F540BT                        |
//  | - EliteMT | LP62S2048X-70LLT | 0026B H4A27HA         |
//  | - MP02AB | LMX2315 | TMD                             |
//  +------------------------------------------------------+

//  +------------------------------------------------------+
//  | Cybiko Xtreme (CY44802)                              |
//  +------------------------------------------------------+
//  | - CYBIKO | CYBOOT 1.5A | HD6432323G03F | 0131 JAPAN  |
//  | - SST 39VF400A | 70-4C-EK                            |
//  | - ATMEL 0033 | AT90S2313-4SC                         |
//  | - SAMSUNG 129 | K4F171612D-TL60                      |
//  | - 2E16AB | USBN9604-28M | NSC00A1                    |
//  +------------------------------------------------------+

//  +------------------------------------------------------+
//  | Cybiko MP3 Player (CY65P10)                          |
//  +------------------------------------------------------+
//  | - H8S/2246 | 0G1 | HD6472246FA20 | JAPAN             |
//  | - MICRONAS | DAC3550A C2 | 0394 22 HM U | 089472.000 |
//  | - 2E08AJ | USBN9603-28M | NSC99A1                    |
//  +------------------------------------------------------+

///////////////////////////
// ADDRESS MAP - PROGRAM //
///////////////////////////

// 512 kbyte ram + no memory mapped flash
static ADDRESS_MAP_START( cybikov1_mem, AS_PROGRAM, 16, cybiko_state )
	AM_RANGE( 0x000000, 0x007fff ) AM_ROM
	AM_RANGE( 0x600000, 0x600001 ) AM_READWRITE( cybiko_lcd_r, cybiko_lcd_w )
	AM_RANGE( 0xe00000, 0xe07fff ) AM_READ( cybikov1_key_r )
ADDRESS_MAP_END

//  +-------------------------------------+
//  | Cybiko Classic (V2) - Memory Map    |
//  +-------------------------------------+
//  | 000000 - 007FFF | rom               |
//  | 008000 - 00FFFF | 17 51 17 51 ..    |
//  | 010000 - 0FFFFF | flash mirror      |
//  | 100000 - 13FFFF | flash             |
//  | 140000 - 1FFFFF | flash mirror      |
//  | 200000 - 23FFFF | ram               |
//  | 240000 - 3FFFFF | ram mirror        |
//  | 400000 - 5FFFFF | FF FF FF FF ..    |
//  | 600000 - 600001 | lcd               |
//  | 600002 - DFFFFF | FF FF FF FF ..    |
//  | E00000 - FFDBFF | keyboard          |
//  | FFDC00 - FFFFFF | onchip ram & regs |
//  +-------------------------------------+

// 256 kbyte ram + 256 kbyte memory mapped flash
static ADDRESS_MAP_START( cybikov2_mem, AS_PROGRAM, 16, cybiko_state )
	AM_RANGE( 0x000000, 0x007fff ) AM_ROM
	AM_RANGE( 0x100000, 0x13ffff ) AM_READ_BANK("bank2") AM_MIRROR( 0x0c0000 )
	AM_RANGE( 0x600000, 0x600001 ) AM_READWRITE( cybiko_lcd_r, cybiko_lcd_w ) AM_MIRROR( 0x1ffffe )
	AM_RANGE( 0xe00000, 0xffdbff ) AM_READ( cybikov2_key_r )
ADDRESS_MAP_END

// 2048 kbyte ram + 512 kbyte memory mapped flash
static ADDRESS_MAP_START( cybikoxt_mem, AS_PROGRAM, 16, cybiko_state )
	AM_RANGE( 0x000000, 0x007fff ) AM_ROM AM_MIRROR( 0x038000 )
	AM_RANGE( 0x100000, 0x100001 ) AM_READWRITE( cybiko_lcd_r, cybiko_lcd_w )
	AM_RANGE( 0x200000, 0x200003 ) AM_WRITE( cybiko_usb_w )
	AM_RANGE( 0x600000, 0x67ffff ) AM_READ_BANK("bank2") AM_MIRROR( 0x180000 )
	AM_RANGE( 0xe00000, 0xefffff ) AM_READ( cybikoxt_key_r )
ADDRESS_MAP_END

//////////////////////
// ADDRESS MAP - IO //
//////////////////////

static ADDRESS_MAP_START( cybikov1_io, AS_IO, 8, cybiko_state )
	AM_RANGE( 0xfffe40, 0xffffff ) AM_READWRITE( cybikov1_io_reg_r, cybikov1_io_reg_w )
ADDRESS_MAP_END

static ADDRESS_MAP_START( cybikov2_io, AS_IO, 8, cybiko_state )
	AM_RANGE( 0xfffe40, 0xffffff ) AM_READWRITE( cybikov2_io_reg_r, cybikov2_io_reg_w )
ADDRESS_MAP_END

static ADDRESS_MAP_START( cybikoxt_io, AS_IO, 8, cybiko_state )
	AM_RANGE( 0xfffe40, 0xffffff ) AM_READWRITE( cybikoxt_io_reg_r, cybikoxt_io_reg_w )
ADDRESS_MAP_END

/////////////////
// INPUT PORTS //
/////////////////


static INPUT_PORTS_START( cybiko )
	PORT_START("A1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F7) PORT_CHAR(UCHAR_MAMEKEY(F7))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Esc") PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Del") PORT_CODE(KEYCODE_DEL) PORT_CHAR(UCHAR_MAMEKEY(DEL))
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CHAR('`')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)

	PORT_START("A2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F6) PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Up") PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("As") PORT_CODE(KEYCODE_INSERT) PORT_CHAR(UCHAR_MAMEKEY(INSERT))
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('@')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Fn") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_SHIFT_2)

	PORT_START("A3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F5) PORT_CHAR(UCHAR_MAMEKEY(F5))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Help") PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(END))

	PORT_START("A4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_TAB) PORT_CHAR('\t')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[') PORT_CHAR('{')

	PORT_START("A5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Select") PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']') PORT_CHAR('}')

	PORT_START("A6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') PORT_CHAR(':')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ENTER) PORT_CHAR( 13 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('^')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\') PORT_CHAR('|')

	PORT_START("A7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("BkSp") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('&')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N) PORT_CHAR('N')

	PORT_START("A8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-') PORT_CHAR('_')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_CHAR('0') PORT_CHAR(')')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('*')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('M')

	PORT_START("A9")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('\'') PORT_CHAR('"')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('=') PORT_CHAR('+')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR('(')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')

INPUT_PORTS_END

static INPUT_PORTS_START( cybikoxt )
	PORT_START("A1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F7) PORT_CHAR(UCHAR_MAMEKEY(F7))
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('L')

	PORT_START("A2")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F6) PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('J')

	PORT_START("A3")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F5) PORT_CHAR(UCHAR_MAMEKEY(F5))
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('T')

	PORT_START("A4")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('E')

	PORT_START("A5")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ENTER) PORT_CHAR( 13 )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Select") PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')

	PORT_START("A6")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_TAB) PORT_CHAR('\t')
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Del") PORT_CODE(KEYCODE_DEL) PORT_CHAR(UCHAR_MAMEKEY(DEL))
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("As") PORT_CODE(KEYCODE_INSERT) PORT_CHAR(UCHAR_MAMEKEY(INSERT))
	PORT_BIT( 0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Esc") PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))

	PORT_START("A7")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Up") PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT( 0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT( 0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))

	PORT_START("A8")
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Fn") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_SHIFT_2)

	PORT_START("A9")
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)

	PORT_START("A10")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Help") PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(END))
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('P')

	PORT_START("A15")
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("On/Off") PORT_CODE(KEYCODE_F8) PORT_CHAR(UCHAR_MAMEKEY(F8))

INPUT_PORTS_END

////////////////////
// PALETTE INIT   //
////////////////////

PALETTE_INIT( hd66421 )
{
	// init palette
	for (int i = 0; i < 4; i++)
	{
		palette_set_color(machine, i, RGB_WHITE);
#ifndef HD66421_BRIGHTNESS_DOES_NOT_WORK
		palette_set_pen_contrast(machine, i, 1.0 * i / (4 - 1));
#endif
	}
}

////////////////////
// SCREEN UPDATE  //
////////////////////

static SCREEN_UPDATE_IND16( cybiko )
{
	hd66421_device *hd66421 = screen.machine().device<hd66421_device>( "hd66421" );
	hd66421->update_screen(bitmap, cliprect);
	return 0;
}


////////////////////
// MACHINE DRIVER //
////////////////////

static MACHINE_CONFIG_START( cybikov1, cybiko_state )
	// cpu
	MCFG_CPU_ADD( "maincpu", H8S2241, 11059200 )
	MCFG_CPU_PROGRAM_MAP( cybikov1_mem )
	MCFG_CPU_IO_MAP( cybikov1_io )
	// screen
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE( 60 )
	MCFG_SCREEN_SIZE( HD66421_WIDTH, HD66421_HEIGHT )
	MCFG_SCREEN_VISIBLE_AREA( 0, HD66421_WIDTH - 1, 0, HD66421_HEIGHT - 1 )
	MCFG_SCREEN_UPDATE_STATIC(cybiko)

	// video
	MCFG_HD66421_ADD("hd66421")
	MCFG_PALETTE_LENGTH(4)
	MCFG_PALETTE_INIT(hd66421)
	MCFG_DEFAULT_LAYOUT(layout_lcd)
	// sound
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD(SPEAKER_TAG, SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
	// machine
	MCFG_MACHINE_START(cybikov1)
	MCFG_MACHINE_RESET(cybikov1)
	/* rtc */
	MCFG_PCF8593_ADD("rtc")
	MCFG_AT45DB041_ADD("flash1")

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("512K")
	MCFG_RAM_EXTRA_OPTIONS("1M")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( cybikov2, cybikov1)
	// cpu
	MCFG_CPU_REPLACE("maincpu", H8S2246, 11059200)
	MCFG_CPU_PROGRAM_MAP(cybikov2_mem )
	MCFG_CPU_IO_MAP(cybikov2_io )
	// machine
	MCFG_MACHINE_START(cybikov2)
	MCFG_MACHINE_RESET(cybikov2)
	MCFG_SST39VF020_ADD("flash2", 16, ENDIANNESS_BIG)

	/* internal ram */
	MCFG_RAM_MODIFY(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("256K")
	MCFG_RAM_EXTRA_OPTIONS("512K,1M")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( cybikoxt, cybikov1)
	// cpu
	MCFG_CPU_REPLACE("maincpu", H8S2323, 18432000)
	MCFG_CPU_PROGRAM_MAP(cybikoxt_mem )
	MCFG_CPU_IO_MAP(cybikoxt_io )
	// machine
	MCFG_MACHINE_START(cybikoxt)
	MCFG_MACHINE_RESET(cybikoxt)
	MCFG_DEVICE_REMOVE("flash1")
	MCFG_SST39VF400A_ADD("flash2", 16, ENDIANNESS_BIG)

	/* internal ram */
	MCFG_RAM_MODIFY(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("2M")
MACHINE_CONFIG_END


/////////
// ROM //
/////////

ROM_START( cybikov1 )
	ROM_REGION( 0x8000, "maincpu", 0 )
	ROM_LOAD( "cyrom112.bin", 0, 0x8000, CRC(9e1f1a0f) SHA1(6fc08de6b2c67d884ec78f748e4a4bad27ee8045) )
ROM_END

ROM_START( cybikov2 )
	ROM_REGION( 0x8000, "maincpu", 0 )
	ROM_LOAD( "cyrom117.bin", 0, 0x8000, CRC(268da7bf) SHA1(135eaf9e3905e69582aabd9b06bc4de0a66780d5) )
ROM_END

ROM_START( cybikoxt )
	ROM_REGION( 0x8000, "maincpu", 0 )
	ROM_LOAD( "cyrom150.bin", 0, 0x8000, CRC(18b9b21f) SHA1(28868d6174eb198a6cec6c3c70b6e494517229b9) )
ROM_END

//////////////
// DRIVERS  //
//////////////

/*    YEAR  NAME        PARENT      COMPAT  MACHINE     INPUT     INIT        COMPANY       FULLNAME                FLAGS */
COMP( 2000, cybikov1,   0,          0,      cybikov1,   cybiko, cybiko_state,   cybikov1,   "Cybiko Inc", "Cybiko Classic (V1)",  GAME_IMPERFECT_SOUND )
COMP( 2000, cybikov2,   cybikov1,   0,      cybikov2,   cybiko, cybiko_state,   cybikov2,   "Cybiko Inc", "Cybiko Classic (V2)",  GAME_IMPERFECT_SOUND )
COMP( 2001, cybikoxt,   cybikov1,   0,      cybikoxt,   cybikoxt, cybiko_state, cybikoxt,   "Cybiko Inc", "Cybiko Xtreme",        GAME_IMPERFECT_SOUND )
