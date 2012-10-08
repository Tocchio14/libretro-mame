/*
    drivers/rmnimbus.c

    Research machines Nimbus.

    2009-11-14, P.Harvey-Smith.

*/

#include "emu.h"
#include "cpu/i86/i86.h"
#include "cpu/mcs51/mcs51.h"
#include "imagedev/flopdrv.h"
#include "machine/ram.h"
#include "formats/pc_dsk.h"
#include "includes/rmnimbus.h"
#include "machine/er59256.h"
#include "machine/scsibus.h"
#include "machine/scsicb.h"
#include "machine/scsihd.h"
#include "machine/s1410.h"
#include "machine/acb4070.h"
#include "machine/6522via.h"
#include "machine/ctronics.h"
#include "sound/ay8910.h"
#include "sound/msm5205.h"

const unsigned char nimbus_palette[SCREEN_NO_COLOURS][3] =
{
	/*normal brightness */
	{ 0x00,0x00,0x00 },     /* black */
	{ 0x00,0x00,0x80 },      /* blue */
	{ 0x80,0x00,0x00 },      /* red */
	{ 0x80,0x00,0x80 },      /* magenta */
	{ 0x00,0x80,0x00 },      /* green */
	{ 0x00,0x80,0x80 },      /* cyan */
	{ 0x80,0x80,0x00 },      /* yellow */
	{ 0x80,0x80,0x80 },      /* light grey */

	/*enhanced brightness*/
	{ 0x40,0x40,0x40 },      /* dark grey */
	{ 0x00,0x00,0xFF },      /* light blue */
	{ 0xFF,0x00,0x00 },      /* light red */
	{ 0xFF,0x00,0xFF },      /* light magenta */
	{ 0x00,0xFF,0x00 },      /* light green */
	{ 0x00,0xFF,0xFF },      /* light cyan */
	{ 0xFF,0xFF,0x00 },      /* yellow */
	{ 0xFF,0xFF,0xFF }       /* white */
};

static const floppy_interface nimbus_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_DSSD_35,
	LEGACY_FLOPPY_OPTIONS_NAME(pc),
	NULL,
	NULL
};

/* Null port handlers for now, I believe that the IO ports are used for */
/* the nimbus rompacks */

static const ay8910_interface nimbus_ay8910_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_NULL,                     /* portA read */
	DEVCB_NULL,                     /* portB read */
	DEVCB_DRIVER_MEMBER(rmnimbus_state, nimbus_sound_ay8910_porta_w),   /* portA write */
	DEVCB_DRIVER_MEMBER(rmnimbus_state, nimbus_sound_ay8910_portb_w)    /* portB write */
};

static const msm5205_interface msm5205_config =
{
	nimbus_msm5205_vck, /* VCK function */
	MSM5205_S48_4B      /* 8 kHz */
};

//-------------------------------------------------
//  SCSICB_interface sasi_intf
//-------------------------------------------------

static const SCSICB_interface scsibus_config =
{
	DEVCB_DRIVER_LINE_MEMBER(rmnimbus_state, nimbus_scsi_bsy_w),
	DEVCB_NULL,
	DEVCB_DRIVER_LINE_MEMBER(rmnimbus_state, nimbus_scsi_cd_w),
	DEVCB_DRIVER_LINE_MEMBER(rmnimbus_state, nimbus_scsi_io_w),
	DEVCB_DRIVER_LINE_MEMBER(rmnimbus_state, nimbus_scsi_msg_w),
	DEVCB_DRIVER_LINE_MEMBER(rmnimbus_state, nimbus_scsi_req_w),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

static const centronics_interface nimbus_centronics_config =
{
	DEVCB_DRIVER_LINE_MEMBER(rmnimbus_state, nimbus_ack_w),
	DEVCB_NULL,
	DEVCB_NULL
};

static ADDRESS_MAP_START(nimbus_mem, AS_PROGRAM, 16, rmnimbus_state )
	AM_RANGE( 0x00000, 0x1FFFF ) AM_RAMBANK(RAM_BANK00_TAG)
	AM_RANGE( 0x20000, 0x3FFFF ) AM_RAMBANK(RAM_BANK01_TAG)
	AM_RANGE( 0x40000, 0x5FFFF ) AM_RAMBANK(RAM_BANK02_TAG)
	AM_RANGE( 0x60000, 0x7FFFF ) AM_RAMBANK(RAM_BANK03_TAG)
	AM_RANGE( 0x80000, 0x9FFFF ) AM_RAMBANK(RAM_BANK04_TAG)
	AM_RANGE( 0xA0000, 0xBFFFF ) AM_RAMBANK(RAM_BANK05_TAG)
	AM_RANGE( 0xC0000, 0xDFFFF ) AM_RAMBANK(RAM_BANK06_TAG)
	AM_RANGE( 0xE0000, 0xEFFFF ) AM_RAMBANK(RAM_BANK07_TAG)
	AM_RANGE( 0xF0000, 0xFFFFF ) AM_ROM AM_REGION(MAINCPU_TAG, 0x0f0000)
ADDRESS_MAP_END

static ADDRESS_MAP_START(nimbus_io, AS_IO, 16, rmnimbus_state )
	AM_RANGE( 0x0000, 0x0031) AM_READWRITE(nimbus_video_io_r, nimbus_video_io_w)
	AM_RANGE( 0x0032, 0x007f) AM_READWRITE(nimbus_io_r, nimbus_io_w)
	AM_RANGE( 0x0080, 0x0081) AM_READWRITE8(nimbus_mcu_r, nimbus_mcu_w, 0x00FF)
	AM_RANGE( 0x0092, 0x0093) AM_READWRITE8(nimbus_iou_r, nimbus_iou_w, 0x00FF)
	AM_RANGE( 0x00A4, 0x00A5) AM_READWRITE8(nimbus_mouse_js_r, nimbus_mouse_js_w, 0x00FF)
	AM_RANGE( 0X00c0, 0X00cf) AM_READWRITE8(nimbus_pc8031_r, nimbus_pc8031_w, 0x00FF)
	AM_RANGE( 0X00e0, 0X00ef) AM_READWRITE8(nimbus_sound_ay8910_r, nimbus_sound_ay8910_w, 0x00FF)
	AM_RANGE( 0x00f0, 0x00f7) AM_DEVREADWRITE8(Z80SIO_TAG, z80sio_device, read, write, 0x00ff)
	AM_RANGE( 0x0400, 0x041f) AM_READWRITE8(nimbus_disk_r, nimbus_disk_w, 0x00FF)
	AM_RANGE( 0x0480, 0x049f) AM_DEVREADWRITE8(VIA_TAG, via6522_device, read, write, 0x00FF)
	AM_RANGE( 0xff00, 0xffff) AM_READWRITE(nimbus_i186_internal_port_r, nimbus_i186_internal_port_w)/* CPU 80186         */
ADDRESS_MAP_END

static INPUT_PORTS_START( nimbus )
	PORT_START("config")
	PORT_CONFNAME( 0x01, 0x00, "Input Port 0 Device")
	PORT_CONFSETTING( 0x00, "Mouse" )
	PORT_CONFSETTING( 0x01, DEF_STR( Joystick ) )

	PORT_START("KEY0") /* Key row 0 scancodes 00..07 */
	//PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ESC")    PORT_CODE(KEYCODE_ESC)          PORT_CHAR(0x1B)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1")      PORT_CODE(KEYCODE_1)            PORT_CHAR('1')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2")      PORT_CODE(KEYCODE_2)            PORT_CHAR('2')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3")      PORT_CODE(KEYCODE_3)            PORT_CHAR('3')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4")      PORT_CODE(KEYCODE_4)            PORT_CHAR('4')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5")      PORT_CODE(KEYCODE_5)            PORT_CHAR('5')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6")      PORT_CODE(KEYCODE_6)            PORT_CHAR('6')

	PORT_START("KEY1") /* Key row 1 scancodes 08..0F */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7")      PORT_CODE(KEYCODE_7)            PORT_CHAR('7')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8")      PORT_CODE(KEYCODE_8)            PORT_CHAR('8')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9")      PORT_CODE(KEYCODE_9)            PORT_CHAR('9')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0")      PORT_CODE(KEYCODE_0)            PORT_CHAR('0')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-")      PORT_CODE(KEYCODE_MINUS)        PORT_CHAR('-')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("=")      PORT_CODE(KEYCODE_EQUALS)       PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("BSPACE") PORT_CODE(KEYCODE_BACKSPACE)    PORT_CHAR(0x08)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TAB")    PORT_CODE(KEYCODE_TAB)          PORT_CHAR(0x09)

	PORT_START("KEY2") /* Key row 2 scancodes 10..17 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q")      PORT_CODE(KEYCODE_Q)            PORT_CHAR('Q')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W")      PORT_CODE(KEYCODE_W)            PORT_CHAR('W')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E")      PORT_CODE(KEYCODE_E)            PORT_CHAR('E')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R")      PORT_CODE(KEYCODE_R)            PORT_CHAR('R')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T")      PORT_CODE(KEYCODE_T)            PORT_CHAR('T')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y")      PORT_CODE(KEYCODE_Y)            PORT_CHAR('Y')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U")      PORT_CODE(KEYCODE_U)            PORT_CHAR('U')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I")      PORT_CODE(KEYCODE_I)            PORT_CHAR('I')

	PORT_START("KEY3") /* Key row 3 scancodes 18..1F */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O")      PORT_CODE(KEYCODE_O)            PORT_CHAR('O')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P")      PORT_CODE(KEYCODE_P)            PORT_CHAR('P')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("[")      PORT_CODE(KEYCODE_OPENBRACE)    PORT_CHAR('[')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("]")      PORT_CODE(KEYCODE_CLOSEBRACE)   PORT_CHAR(']')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ENTER")  PORT_CODE(KEYCODE_ENTER)        PORT_CHAR(0x0D)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CTRL")   PORT_CODE(KEYCODE_LCONTROL)     PORT_CODE(KEYCODE_RCONTROL) // Ether control
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")      PORT_CODE(KEYCODE_A)            PORT_CHAR('A')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S")      PORT_CODE(KEYCODE_S)            PORT_CHAR('S')

	PORT_START("KEY4") /* Key row 4 scancodes 20..27 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D")      PORT_CODE(KEYCODE_D)            PORT_CHAR('D')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F")      PORT_CODE(KEYCODE_F)            PORT_CHAR('F')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G")      PORT_CODE(KEYCODE_G)            PORT_CHAR('G')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H")      PORT_CODE(KEYCODE_H)            PORT_CHAR('H')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J")      PORT_CODE(KEYCODE_J)            PORT_CHAR('J')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K")      PORT_CODE(KEYCODE_K)            PORT_CHAR('K')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L")      PORT_CODE(KEYCODE_L)            PORT_CHAR('L')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";")      PORT_CODE(KEYCODE_COLON)        PORT_CHAR(';')

	PORT_START("KEY5") /* Key row 5 scancodes 28..2F */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("#")      PORT_CODE(KEYCODE_QUOTE)        PORT_CHAR('#')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("BACKSLASH") PORT_CODE(KEYCODE_BACKSLASH)    PORT_CHAR('\\')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("LSHIFT") PORT_CODE(KEYCODE_LSHIFT)       PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TILDE")  PORT_CODE(KEYCODE_TILDE)        PORT_CHAR('`')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")      PORT_CODE(KEYCODE_Z)            PORT_CHAR('Z')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")      PORT_CODE(KEYCODE_X)            PORT_CHAR('X')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C")      PORT_CODE(KEYCODE_C)            PORT_CHAR('C')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V")      PORT_CODE(KEYCODE_V)            PORT_CHAR('V')


	PORT_START("KEY6") /* Key row 6 scancodes 30..37 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")      PORT_CODE(KEYCODE_B)            PORT_CHAR('B')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N")      PORT_CODE(KEYCODE_N)            PORT_CHAR('N')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M")      PORT_CODE(KEYCODE_M)            PORT_CHAR('M')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",")      PORT_CODE(KEYCODE_COMMA)        PORT_CHAR(',')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".")      PORT_CODE(KEYCODE_STOP)         PORT_CHAR('.')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("/")      PORT_CODE(KEYCODE_SLASH)        PORT_CHAR('/')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RSHIFT") PORT_CODE(KEYCODE_RSHIFT)       PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PRSCR")  PORT_CODE(KEYCODE_ASTERISK)     PORT_CHAR('*')

	PORT_START("KEY7") /* Key row 7 scancodes 38..3F */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ALT")    PORT_CODE(KEYCODE_LALT)         PORT_CODE(KEYCODE_RALT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SPACE")  PORT_CODE(KEYCODE_SPACE)        PORT_CHAR(' ')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CAPS")   PORT_CODE(KEYCODE_CAPSLOCK)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F1")     PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F2")     PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F3")     PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F4")     PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F5")     PORT_CODE(KEYCODE_F5)


	PORT_START("KEY8") /* Key row 8 scancodes 40..47 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F6")     PORT_CODE(KEYCODE_F6)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F7")     PORT_CODE(KEYCODE_F7)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F8")     PORT_CODE(KEYCODE_F8)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F9")     PORT_CODE(KEYCODE_F9)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F10")    PORT_CODE(KEYCODE_F10)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("NUMLK")  PORT_CODE(KEYCODE_NUMLOCK)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SCRLK")  PORT_CODE(KEYCODE_SCRLOCK)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP7")    PORT_CODE(KEYCODE_7_PAD)        PORT_CHAR('7')

	PORT_START("KEY9") /* Key row 9 scancodes 48..4F */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP8")    PORT_CODE(KEYCODE_8_PAD)        //PORT_CHAR('8')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP9")    PORT_CODE(KEYCODE_9_PAD)        //PORT_CHAR('9')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP-")    PORT_CODE(KEYCODE_MINUS_PAD)    //PORT_CHAR('-')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP4")    PORT_CODE(KEYCODE_4_PAD)        //PORT_CHAR('4')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP5")    PORT_CODE(KEYCODE_5_PAD)        //PORT_CHAR('5')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP6")    PORT_CODE(KEYCODE_6_PAD)        //PORT_CHAR('6')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP+")    PORT_CODE(KEYCODE_PLUS_PAD)     //PORT_CHAR('+')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP1")    PORT_CODE(KEYCODE_1_PAD)        //PORT_CHAR('1')

	PORT_START("KEY10") /* Key row 10 scancodes 50..57 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP2")    PORT_CODE(KEYCODE_2_PAD)        //PORT_CHAR('2')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP3")    PORT_CODE(KEYCODE_3_PAD)        //PORT_CHAR('3')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP0")    PORT_CODE(KEYCODE_0_PAD)        //PORT_CHAR('0')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP.")    PORT_CODE(KEYCODE_DEL_PAD)      //PORT_CHAR('.')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
	//PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP5")    PORT_CODE(KEYCODE_5_PAD)        PORT_CHAR('5')
	//PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP6")    PORT_CODE(KEYCODE_6_PAD)        PORT_CHAR('6')
	//PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP+")    PORT_CODE(KEYCODE_PLUS_PAD)     PORT_CHAR('+')
	//PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KP1")    PORT_CODE(KEYCODE_1_PAD)        PORT_CHAR('1')

	PORT_START(JOYSTICK0_TAG)
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )    PORT_PLAYER(1) PORT_8WAY // XB
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )  PORT_PLAYER(1) PORT_8WAY // XA
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )  PORT_PLAYER(1) PORT_8WAY // YA
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_8WAY // YB
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)

	PORT_START(MOUSE_BUTTON_TAG)  /* Mouse buttons */
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Mouse Button 2") PORT_CODE(MOUSECODE_BUTTON2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Mouse Button 1") PORT_CODE(MOUSECODE_BUTTON1)

	PORT_START(MOUSEX_TAG) /* Mouse - X AXIS */
	PORT_BIT( 0xff, 0x00, IPT_MOUSE_X) PORT_SENSITIVITY(100) PORT_KEYDELTA(0) PORT_PLAYER(1)

	PORT_START(MOUSEY_TAG) /* Mouse - Y AXIS */
	PORT_BIT( 0xff, 0x00, IPT_MOUSE_Y) PORT_SENSITIVITY(100) PORT_KEYDELTA(0) PORT_PLAYER(1)

INPUT_PORTS_END

static ADDRESS_MAP_START(nimbus_iocpu_mem, AS_PROGRAM, 8, rmnimbus_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x7fff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( nimbus_iocpu_io , AS_IO, 8, rmnimbus_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0x000FF) AM_READWRITE(nimbus_pc8031_iou_r, nimbus_pc8031_iou_w)
	AM_RANGE(0x00010, 0x07fff) AM_RAM
	AM_RANGE(0x20000, 0x20004) AM_READWRITE(nimbus_pc8031_port_r, nimbus_pc8031_port_w)
ADDRESS_MAP_END


void rmnimbus_state::palette_init()
{
	int colourno;

	for ( colourno = 0; colourno < SCREEN_NO_COLOURS; colourno++ )
	{
		palette_set_color_rgb(machine(), colourno, nimbus_palette[colourno][RED], nimbus_palette[colourno][GREEN], nimbus_palette[colourno][BLUE]);
	}
}


static MACHINE_CONFIG_START( nimbus, rmnimbus_state )
	/* basic machine hardware */
	MCFG_CPU_ADD(MAINCPU_TAG, I80186, 10000000)
	MCFG_CPU_PROGRAM_MAP(nimbus_mem)
	MCFG_CPU_IO_MAP(nimbus_io)

	MCFG_CPU_ADD(IOCPU_TAG, I8031, 11059200)
	MCFG_CPU_PROGRAM_MAP(nimbus_iocpu_mem)
	MCFG_CPU_IO_MAP(nimbus_iocpu_io)



	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(100))
	MCFG_SCREEN_UPDATE_DRIVER(rmnimbus_state, screen_update_nimbus)
	MCFG_SCREEN_VBLANK_DRIVER(rmnimbus_state, screen_eof_nimbus)

	MCFG_VIDEO_ATTRIBUTES(VIDEO_UPDATE_SCANLINE)

	MCFG_PALETTE_LENGTH(SCREEN_NO_COLOURS * 3)

	MCFG_SCREEN_SIZE(650, 260)
	MCFG_SCREEN_VISIBLE_AREA(0, 639, 0, 249)



	/* Backing storage */
	MCFG_WD2793_ADD(FDC_TAG, nimbus_wd17xx_interface )
	MCFG_LEGACY_FLOPPY_4_DRIVES_ADD(nimbus_floppy_interface)

	MCFG_SCSIBUS_ADD(SCSIBUS_TAG)
	MCFG_SCSIDEV_ADD(SCSIBUS_TAG ":harddisk0", ACB4070, SCSI_ID_0)
	MCFG_SCSIDEV_ADD(SCSIBUS_TAG ":harddisk1", S1410, SCSI_ID_1)
	MCFG_SCSIDEV_ADD(SCSIBUS_TAG ":harddisk2", SCSIHD, SCSI_ID_2)
	MCFG_SCSIDEV_ADD(SCSIBUS_TAG ":harddisk3", SCSIHD, SCSI_ID_3)
	MCFG_SCSICB_ADD(SCSIBUS_TAG ":host", scsibus_config)

	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("1536K")
	MCFG_RAM_EXTRA_OPTIONS("128K,256K,384K,512K,640K,1024K")

	/* Peripheral chips */
	MCFG_Z80SIO_ADD(Z80SIO_TAG, 4000000, nimbus_sio_intf)

	MCFG_ER59256_ADD(ER59256_TAG)

	MCFG_VIA6522_ADD(VIA_TAG, 1000000, nimbus_via)
	MCFG_CENTRONICS_PRINTER_ADD(CENTRONICS_TAG, nimbus_centronics_config)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO(MONO_TAG)
	MCFG_SOUND_ADD(AY8910_TAG, AY8910, 2000000)
	MCFG_SOUND_CONFIG(nimbus_ay8910_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS,MONO_TAG, 0.75)

	MCFG_SOUND_ADD(MSM5205_TAG, MSM5205, 384000)
	MCFG_SOUND_CONFIG(msm5205_config)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, MONO_TAG, 0.75)


MACHINE_CONFIG_END


ROM_START( nimbus )
	ROM_REGION( 0x100000, MAINCPU_TAG, 0 )

	ROM_SYSTEM_BIOS(0, "v131a", "Nimbus BIOS v1.31a (1986-06-18)")
	ROMX_LOAD("sys1-1.31a-16128-1986-06-18.rom", 0xf0001, 0x8000, CRC(6416eb05) SHA1(1b640163a7efbc24381c7b24976a8609c066959b),ROM_SKIP(1) | ROM_BIOS(1)  )
	ROMX_LOAD("sys2-1.31a-16129-1986-06-18.rom", 0xf0000, 0x8000, CRC(b224359d) SHA1(456bbe37afcd4429cca76ba2d6bd534dfda3fc9c),ROM_SKIP(1) | ROM_BIOS(1)  )

	ROM_SYSTEM_BIOS(1, "v132f", "Nimbus BIOS v1.32f (1989-10-20)")
	ROMX_LOAD("sys-1-1.32f-22779-1989-10-20.rom", 0xf0001, 0x8000, CRC(786c31e8) SHA1(da7f828f7f96087518bea1a3d89fee59b283b4ba),ROM_SKIP(1) | ROM_BIOS(2) )
	ROMX_LOAD("sys-2-1.32f-22779-1989-10-20.rom", 0xf0000, 0x8000, CRC(0be3db64) SHA1(af806405ec6fbc20385705f90d5059a47de17b08),ROM_SKIP(1) | ROM_BIOS(2) )

	ROM_SYSTEM_BIOS(2, "v140d", "Nimbus BIOS v1.40d (1990-xx-xx)")
	ROMX_LOAD("sys-1-1.40d.rom", 0xf0001, 0x8000, CRC(b8d3dc0b) SHA1(82e0dcdc6c7a83339af68d6cb61211fcb14bed88),ROM_SKIP(1) | ROM_BIOS(3) )
	ROMX_LOAD("sys-2-1.40d.rom", 0xf0000, 0x8000, CRC(b0826b0b) SHA1(3baa369a0e7ef138ca29aae0ee8a89ab670a02b9),ROM_SKIP(1) | ROM_BIOS(3) )

	ROM_REGION( 0x4000, IOCPU_TAG, 0 )
	ROM_LOAD("hexec-v1.02u-13488-1985-10-29.rom", 0x0000, 0x1000, CRC(75c6adfd) SHA1(0f11e0b7386c6368d20e1fc7a6196d670f924825))
ROM_END


/*    YEAR  NAME        PARENT  COMPAT  MACHINE INPUT   INIT  COMPANY  FULLNAME   FLAGS */
COMP( 1986, nimbus,     0,      0,      nimbus, nimbus, driver_device, 0,   "Research Machines", "Nimbus", 0)
