// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
/***************************************************************************

        Casio PB-1000 / PB-2000c driver

        Driver by Sandro Ronco

        TODO:
        - improve the pb2000c gate array emulation
        - i/o port

        Known issues:
        - the second memory card is not recognized by the HW

        More info:
            http://www.itkp.uni-bonn.de/~wichmann/pb1000-wrobel.html

****************************************************************************/


#include "emu.h"
#include "cpu/hd61700/hd61700.h"
#include "video/hd44352.h"
#include "imagedev/cartslot.h"
#include "machine/nvram.h"
#include "sound/beep.h"
#include "rendlay.h"

class pb1000_state : public driver_device
{
public:
	pb1000_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_beeper(*this, "beeper"),
			m_hd44352(*this, "hd44352")
		{ }

	required_device<hd61700_cpu_device> m_maincpu;
	required_device<beep_device> m_beeper;
	required_device<hd44352_device> m_hd44352;

	emu_timer *m_kb_timer;
	UINT8 m_kb_matrix;
	UINT8 m_gatearray[2];

	virtual void machine_start();
	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE16_MEMBER( gatearray_w );
	UINT16 pb2000c_kb_r(running_machine &machine);
	UINT16 pb1000_kb_r(running_machine &machine);
	void kb_matrix_w(running_machine &machine, UINT8 matrix);
	UINT16 read_touchscreen(running_machine &machine, UINT8 line);
	virtual void palette_init();
	TIMER_CALLBACK_MEMBER(keyboard_timer);
};

static ADDRESS_MAP_START(pb1000_mem, AS_PROGRAM, 16, pb1000_state)
	ADDRESS_MAP_UNMAP_LOW
	AM_RANGE( 0x00000, 0x00bff ) AM_ROM
	//AM_RANGE( 0x00c00, 0x00c0f ) AM_NOP   //I/O
	AM_RANGE( 0x06000, 0x07fff ) AM_RAM                 AM_SHARE("nvram1")
	AM_RANGE( 0x08000, 0x0ffff ) AM_ROMBANK("bank1")
	AM_RANGE( 0x18000, 0x1ffff ) AM_RAM                 AM_SHARE("nvram2")
ADDRESS_MAP_END

static ADDRESS_MAP_START(pb2000c_mem, AS_PROGRAM, 16, pb1000_state)
	ADDRESS_MAP_UNMAP_LOW
	AM_RANGE( 0x00000, 0x00bff ) AM_ROM
	//AM_RANGE( 0x00c00, 0x00c0f ) AM_NOP   //I/O
	AM_RANGE( 0x00c10, 0x00c11 ) AM_WRITE(gatearray_w)
	AM_RANGE( 0x00000, 0x0ffff ) AM_ROMBANK("bank1")
	AM_RANGE( 0x10000, 0x1ffff ) AM_RAM                 AM_SHARE("nvram1")
	AM_RANGE( 0x20000, 0x27fff ) AM_ROM                 AM_REGION("card1", 0)
	AM_RANGE( 0x28000, 0x2ffff ) AM_RAM                 AM_SHARE("nvram2")
ADDRESS_MAP_END


/* Input ports */
static INPUT_PORTS_START( pb1000 )
	PORT_START("KO1")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BRK")     PORT_CODE(KEYCODE_F10)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OFF")     PORT_CODE(KEYCODE_7_PAD)
	PORT_START("KO2")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EXE")     PORT_CODE(KEYCODE_ENTER)        PORT_CHAR(13)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("IN")      PORT_CODE(KEYCODE_F4)
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MEMO IN") PORT_CODE(KEYCODE_0_PAD)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",")       PORT_CODE(KEYCODE_COMMA)        PORT_CHAR(',')  PORT_CHAR('?')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("'")       PORT_CODE(KEYCODE_QUOTE)        PORT_CHAR('"')  PORT_CHAR('!')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("$")       PORT_CODE(KEYCODE_1_PAD)        PORT_CHAR('$')  PORT_CHAR('#')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("&")       PORT_CODE(KEYCODE_2_PAD)        PORT_CHAR('&')  PORT_CHAR('%')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("=")       PORT_CODE(KEYCODE_EQUALS)       PORT_CHAR('=')  PORT_CHAR('\'')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(";")       PORT_CODE(KEYCODE_COLON)        PORT_CHAR(';')  PORT_CHAR('<')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(":")       PORT_CODE(KEYCODE_BACKSLASH2)   PORT_CHAR(':')  PORT_CHAR('>')
	PORT_START("KO3")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RIGHT")   PORT_CODE(KEYCODE_RIGHT)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OUT")     PORT_CODE(KEYCODE_F3)
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MEMO")    PORT_CODE(KEYCODE_F11)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U")       PORT_CODE(KEYCODE_U)            PORT_CHAR('U')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q")       PORT_CODE(KEYCODE_Q)            PORT_CHAR('Q')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W")       PORT_CODE(KEYCODE_W)            PORT_CHAR('W')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E")       PORT_CODE(KEYCODE_E)            PORT_CHAR('E')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R")       PORT_CODE(KEYCODE_R)            PORT_CHAR('R')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T")       PORT_CODE(KEYCODE_T)            PORT_CHAR('T')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y")       PORT_CODE(KEYCODE_Y)            PORT_CHAR('Y')
	PORT_START("KO4")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DOWN")    PORT_CODE(KEYCODE_DOWN)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CALC")    PORT_CODE(KEYCODE_HOME)
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAL")     PORT_CODE(KEYCODE_F9)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H")       PORT_CODE(KEYCODE_H)            PORT_CHAR('H')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAPS")    PORT_CODE(KEYCODE_CAPSLOCK)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A")       PORT_CODE(KEYCODE_A)            PORT_CHAR('A')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S")       PORT_CODE(KEYCODE_S)            PORT_CHAR('S')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D")       PORT_CODE(KEYCODE_D)            PORT_CHAR('D')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F")       PORT_CODE(KEYCODE_F)            PORT_CHAR('F')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G")       PORT_CODE(KEYCODE_G)            PORT_CHAR('G')
	PORT_START("KO5")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("UP")      PORT_CODE(KEYCODE_UP)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LEFT")    PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MENU")    PORT_CODE(KEYCODE_F5)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M")       PORT_CODE(KEYCODE_M)            PORT_CHAR('M')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z")       PORT_CODE(KEYCODE_Z)            PORT_CHAR('Z')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X")       PORT_CODE(KEYCODE_X)            PORT_CHAR('X')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C")       PORT_CODE(KEYCODE_C)            PORT_CHAR('C')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V")       PORT_CODE(KEYCODE_V)            PORT_CHAR('V')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B")       PORT_CODE(KEYCODE_B)            PORT_CHAR('B')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N")       PORT_CODE(KEYCODE_N)            PORT_CHAR('N')
	PORT_START("KO6")
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LCKEY")   PORT_CODE(KEYCODE_F1)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CONTR")   PORT_CODE(KEYCODE_F2)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(")")       PORT_CODE(KEYCODE_PGDN)         PORT_CHAR(')')      PORT_CHAR(']')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STOP")    PORT_CODE(KEYCODE_F7)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("INS")     PORT_CODE(KEYCODE_INSERT)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("NEW ALL") PORT_CODE(KEYCODE_ESC)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BS")      PORT_CODE(KEYCODE_BACKSPACE)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CLS")     PORT_CODE(KEYCODE_DEL)
	PORT_START("KO7")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^")       PORT_CODE(KEYCODE_3_PAD)        PORT_CHAR('^')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/")       PORT_CODE(KEYCODE_SLASH)        PORT_CHAR('/')      PORT_CHAR('}')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(")       PORT_CODE(KEYCODE_PGUP)         PORT_CHAR('(')      PORT_CHAR('[')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9")       PORT_CODE(KEYCODE_9)            PORT_CHAR('9')      PORT_CHAR('\'')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8")       PORT_CODE(KEYCODE_8)            PORT_CHAR('8')      PORT_CHAR('_')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7")       PORT_CODE(KEYCODE_7)            PORT_CHAR('7')      PORT_CHAR('@')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENG")     PORT_CODE(KEYCODE_F6)
	PORT_START("KO8")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I")       PORT_CODE(KEYCODE_I)            PORT_CHAR('I')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*")       PORT_CODE(KEYCODE_ASTERISK)     PORT_CHAR('*')      PORT_CHAR('{')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6")       PORT_CODE(KEYCODE_6)            PORT_CHAR('6')      PORT_CHAR('\\')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5")       PORT_CODE(KEYCODE_5)            PORT_CHAR('5')      PORT_CHAR('~')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4")       PORT_CODE(KEYCODE_4)            PORT_CHAR('4')      PORT_CHAR('|')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P")       PORT_CODE(KEYCODE_P)            PORT_CHAR('P')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O")       PORT_CODE(KEYCODE_O)            PORT_CHAR('O')
	PORT_START("KO9")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J")       PORT_CODE(KEYCODE_J)            PORT_CHAR('J')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+")       PORT_CODE(KEYCODE_PLUS_PAD)     PORT_CHAR('+')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3")       PORT_CODE(KEYCODE_3)            PORT_CHAR('3')
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2")       PORT_CODE(KEYCODE_2)            PORT_CHAR('2')
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1")       PORT_CODE(KEYCODE_1)            PORT_CHAR('1')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L")       PORT_CODE(KEYCODE_L)            PORT_CHAR('L')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K")       PORT_CODE(KEYCODE_K)            PORT_CHAR('K')
	PORT_START("KO10")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SPACE")   PORT_CODE(KEYCODE_SPACE)        PORT_CHAR(' ')
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-")       PORT_CODE(KEYCODE_MINUS)        PORT_CHAR('-')
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EXE")     PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ANS")     PORT_CODE(KEYCODE_END)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".")       PORT_CODE(KEYCODE_STOP)         PORT_CHAR('.')
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0")       PORT_CODE(KEYCODE_0)            PORT_CHAR('0')
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("??")      PORT_CODE(KEYCODE_5_PAD)
	PORT_START("KO11")
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SHIFT")   PORT_CODE(KEYCODE_LSHIFT)       PORT_CHAR(UCHAR_SHIFT_1)
	PORT_START("KO12")
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F")       PORT_CODE(KEYCODE_LALT)
	PORT_START("NULL")
		PORT_BIT(0xffff, IP_ACTIVE_HIGH, IPT_UNUSED)

	//touchscreen
	PORT_START("POSX")
		PORT_BIT( 0xff, 0x00, IPT_MOUSE_X ) PORT_CROSSHAIR(X, 1, 0, 0) PORT_SENSITIVITY(20) PORT_KEYDELTA(0)
	PORT_START("POSY")
		PORT_BIT( 0xff, 0x00, IPT_MOUSE_Y ) PORT_CROSSHAIR(Y, 1, 0, 0) PORT_SENSITIVITY(120) PORT_KEYDELTA(0)
	PORT_START("TOUCH")
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(MOUSECODE_BUTTON1) PORT_NAME("Touchscreen")
INPUT_PORTS_END

static INPUT_PORTS_START( pb2000c )
	PORT_START("KO1")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BRK")     PORT_CODE(KEYCODE_F10)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OFF")     PORT_CODE(KEYCODE_7_PAD)
	PORT_START("KO2")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAB")     PORT_CODE(KEYCODE_TAB)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("'")       PORT_CODE(KEYCODE_QUOTE)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAPS")    PORT_CODE(KEYCODE_CAPSLOCK)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z")       PORT_CODE(KEYCODE_Z)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A")       PORT_CODE(KEYCODE_A)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q")       PORT_CODE(KEYCODE_Q)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W")       PORT_CODE(KEYCODE_W)
	PORT_START("KO3")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(")       PORT_CODE(KEYCODE_PGUP)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(")")       PORT_CODE(KEYCODE_PGDN)
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M1")      PORT_CODE(KEYCODE_F1)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X")       PORT_CODE(KEYCODE_X)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C")       PORT_CODE(KEYCODE_C)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S")       PORT_CODE(KEYCODE_S)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D")       PORT_CODE(KEYCODE_D)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E")       PORT_CODE(KEYCODE_E)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R")       PORT_CODE(KEYCODE_R)
	PORT_START("KO4")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("[")       PORT_CODE(KEYCODE_OPENBRACE)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("]")       PORT_CODE(KEYCODE_CLOSEBRACE)
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M2")      PORT_CODE(KEYCODE_F2)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V")       PORT_CODE(KEYCODE_V)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B")       PORT_CODE(KEYCODE_B)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F")       PORT_CODE(KEYCODE_F)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G")       PORT_CODE(KEYCODE_G)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T")       PORT_CODE(KEYCODE_T)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y")       PORT_CODE(KEYCODE_Y)
	PORT_START("KO5")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("|")       PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MEMO")    PORT_CODE(KEYCODE_F11)
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M3")      PORT_CODE(KEYCODE_F3)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N")       PORT_CODE(KEYCODE_N)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M")       PORT_CODE(KEYCODE_M)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H")       PORT_CODE(KEYCODE_H)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J")       PORT_CODE(KEYCODE_J)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U")       PORT_CODE(KEYCODE_U)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I")       PORT_CODE(KEYCODE_I)
	PORT_START("KO6")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("IN")      PORT_CODE(KEYCODE_F6)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("OUT")     PORT_CODE(KEYCODE_F7)
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M4")      PORT_CODE(KEYCODE_F4)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",")       PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SPACE")   PORT_CODE(KEYCODE_SPACE)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K")       PORT_CODE(KEYCODE_K)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L")       PORT_CODE(KEYCODE_L)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O")       PORT_CODE(KEYCODE_O)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P")       PORT_CODE(KEYCODE_P)
	PORT_START("KO7")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CALC")    PORT_CODE(KEYCODE_HOME)
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ETC")     PORT_CODE(KEYCODE_F12)
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SS")      PORT_CODE(KEYCODE_LSHIFT)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ANS")     PORT_CODE(KEYCODE_END)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(";")       PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(":")       PORT_CODE(KEYCODE_BACKSLASH2)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("=")       PORT_CODE(KEYCODE_EQUALS)
	PORT_START("KO8")
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("NEW ALL") PORT_CODE(KEYCODE_ESC)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7")       PORT_CODE(KEYCODE_7)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0")       PORT_CODE(KEYCODE_0)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1")       PORT_CODE(KEYCODE_1)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2")       PORT_CODE(KEYCODE_2)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4")       PORT_CODE(KEYCODE_4)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5")       PORT_CODE(KEYCODE_5)
	PORT_START("KO9")
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9")       PORT_CODE(KEYCODE_9)
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".")       PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EXE")     PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3")       PORT_CODE(KEYCODE_3)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+")       PORT_CODE(KEYCODE_PLUS_PAD)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6")       PORT_CODE(KEYCODE_6)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-")       PORT_CODE(KEYCODE_MINUS)
	PORT_START("KO10")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL")     PORT_CODE(KEYCODE_F8)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MENU")    PORT_CODE(KEYCODE_F5)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*")       PORT_CODE(KEYCODE_ASTERISK)
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BS")      PORT_CODE(KEYCODE_BACKSPACE)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/")       PORT_CODE(KEYCODE_SLASH)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RIGHT")   PORT_CODE(KEYCODE_RIGHT)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAL")     PORT_CODE(KEYCODE_F9)
	PORT_START("KO11")
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("INS")     PORT_CODE(KEYCODE_INSERT)
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("UP")      PORT_CODE(KEYCODE_UP)
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8")       PORT_CODE(KEYCODE_8)
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CLS")     PORT_CODE(KEYCODE_DEL)
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LEFT")    PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DOWN")    PORT_CODE(KEYCODE_DOWN)
	PORT_START("KO12")
		PORT_BIT(0xffff, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_START("NULL")
		PORT_BIT(0xffff, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END

void pb1000_state::palette_init()
{
	palette_set_color(machine(), 0, rgb_t(138, 146, 148));
	palette_set_color(machine(), 1, rgb_t(92, 83, 88));
}


static const gfx_layout pb1000_charlayout =
{
	8, 8,                   /* 8 x 8 characters */
	256,                    /* 256 characters */
	1,                      /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8},
	{ 0, 1, 2, 3, 4, 5, 6, 7},
	8*8                     /* 8 bytes */
};

static GFXDECODE_START( pb1000 )
	GFXDECODE_ENTRY( "hd44352", 0x0000, pb1000_charlayout, 0, 1 )
GFXDECODE_END

WRITE16_MEMBER( pb1000_state::gatearray_w )
{
	m_gatearray[offset] = data&0xff;

	if (m_gatearray[0])
		membank("bank1")->set_base(memregion("card1")->base());
	else if (m_gatearray[1])
		membank("bank1")->set_base(memregion("card2")->base());
	else
		membank("bank1")->set_base(memregion("rom")->base());
}

static void lcd_control(hd61700_cpu_device &device, UINT8 data)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();

	state->m_hd44352->control_write(data);
}


static UINT8 lcd_data_r(hd61700_cpu_device &device)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();

	return state->m_hd44352->data_read();
}


static void lcd_data_w(hd61700_cpu_device &device, UINT8 data)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();

	state->m_hd44352->data_write(data);
}


UINT16 pb1000_state::read_touchscreen(running_machine &machine, UINT8 line)
{
	UINT8 x = machine.root_device().ioport("POSX")->read()/0x40;
	UINT8 y = machine.root_device().ioport("POSY")->read()/0x40;

	if (machine.root_device().ioport("TOUCH")->read())
	{
		if (x == line-7)
			return (0x1000<<y);
	}

	return 0x0000;
}


UINT16 pb1000_state::pb1000_kb_r(running_machine &machine)
{
	static const char *const bitnames[] = {"NULL", "KO1", "KO2", "KO3", "KO4", "KO5", "KO6", "KO7", "KO8", "KO9", "KO10", "KO11", "KO12", "NULL", "NULL", "NULL"};
	UINT16 data = 0;

	if ((m_kb_matrix & 0x0f) == 0x0d)
	{
		//Read all the input lines
		for (int line = 1; line <= 12; line++)
		{
			data |= machine.root_device().ioport(bitnames[line])->read();
			data |= read_touchscreen(machine, line);
		}

	}
	else
	{
		data = machine.root_device().ioport(bitnames[m_kb_matrix & 0x0f])->read();
		data |= read_touchscreen(machine, m_kb_matrix & 0x0f);
	}

	return data;
}

UINT16 pb1000_state::pb2000c_kb_r(running_machine &machine)
{
	static const char *const bitnames[] = {"NULL", "KO1", "KO2", "KO3", "KO4", "KO5", "KO6", "KO7", "KO8", "KO9", "KO10", "KO11", "KO12", "NULL", "NULL", "NULL"};
	UINT16 data = 0;

	if ((m_kb_matrix & 0x0f) == 0x0d)
	{
		//Read all the input lines
		for (int line = 1; line <= 12; line++)
		{
			data |= machine.root_device().ioport(bitnames[line])->read();
		}

	}
	else
	{
		data = machine.root_device().ioport(bitnames[m_kb_matrix & 0x0f])->read();
	}

	return data;
}

void pb1000_state::kb_matrix_w(running_machine &machine, UINT8 matrix)
{
	if (matrix & 0x80)
	{
		if ((m_kb_matrix & 0x80) != (matrix & 0x80))
			m_kb_timer->adjust(attotime::never, 0, attotime::never);
	}
	else
	{
		if ((m_kb_matrix & 0x40) != (matrix & 0x40))
		{
			if (matrix & 0x40)
				m_kb_timer->adjust(attotime::from_hz(32), 0, attotime::from_hz(32));
			else
				m_kb_timer->adjust(attotime::from_hz(256), 0, attotime::from_hz(256));
		}
	}

	m_kb_matrix = matrix;
}

//-------------------------------------------------
//  HD61700 interface
//-------------------------------------------------

static void kb_matrix_w_call(hd61700_cpu_device &device, UINT8 matrix)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();

	state->kb_matrix_w(device.machine(), matrix);
}

static UINT8 pb1000_port_r(hd61700_cpu_device &device)
{
	//TODO
	return 0x00;
}

static UINT8 pb2000c_port_r(hd61700_cpu_device &device)
{
	//TODO
	return 0xfc;
}

static void port_w(hd61700_cpu_device &device, UINT8 data)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();
	state->m_beeper->set_state((BIT(data,7) ^ BIT(data,6)));
	//printf("%x\n", data);
}

static UINT16 pb1000_kb_r_call(hd61700_cpu_device &device)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();

	return state->pb1000_kb_r(device.machine());
}

static UINT16 pb2000c_kb_r_call(hd61700_cpu_device &device)
{
	pb1000_state *state = device.machine().driver_data<pb1000_state>();

	return state->pb2000c_kb_r(device.machine());
}

static const hd61700_config pb1000_config =
{
	lcd_control,            //lcd control
	lcd_data_r,             //lcd data read
	lcd_data_w,             //lcd data write
	pb1000_kb_r_call,       //keyboard matrix read
	kb_matrix_w_call,       //keyboard matrix write
	pb1000_port_r,          //8 bit port read
	port_w                  //8 bit port  write
};

static const hd61700_config pb2000c_config =
{
	lcd_control,            //lcd control
	lcd_data_r,             //lcd data read
	lcd_data_w,             //lcd data write
	pb2000c_kb_r_call,      //keyboard matrix read
	kb_matrix_w_call,       //keyboard matrix write
	pb2000c_port_r,         //8 bit port read
	port_w                  //8 bit port  write
};

TIMER_CALLBACK_MEMBER(pb1000_state::keyboard_timer)
{
	m_maincpu->set_input_line(HD61700_KEY_INT, ASSERT_LINE);
	m_maincpu->set_input_line(HD61700_KEY_INT, CLEAR_LINE);
}

void pb1000_state::machine_start()
{
	membank("bank1")->set_base(memregion("rom")->base());

	m_kb_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(pb1000_state::keyboard_timer),this));
	m_kb_timer->adjust(attotime::from_hz(192), 0, attotime::from_hz(192));
}

static const hd44352_interface hd44352_pb1000_conf =
{
	DEVCB_CPU_INPUT_LINE("maincpu", HD61700_ON_INT)
};

static MACHINE_CONFIG_START( pb1000, pb1000_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", HD61700, 910000)
	MCFG_CPU_PROGRAM_MAP(pb1000_mem)
	MCFG_HD61700_CONFIG(pb1000_config)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_UPDATE_DEVICE("hd44352", hd44352_device, screen_update)
	MCFG_SCREEN_SIZE(192, 32)
	MCFG_SCREEN_VISIBLE_AREA(0, 192-1, 0, 32-1)
	MCFG_DEFAULT_LAYOUT(layout_lcd)
	MCFG_PALETTE_LENGTH(2)
	MCFG_GFXDECODE_ADD("gfxdecode",  pb1000 )

	MCFG_HD44352_ADD("hd44352", 910000, hd44352_pb1000_conf)

	MCFG_NVRAM_ADD_0FILL("nvram1")
	MCFG_NVRAM_ADD_0FILL("nvram2")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO( "mono" )
	MCFG_SOUND_ADD( "beeper", BEEP, 0 )
	MCFG_SOUND_ROUTE( ALL_OUTPUTS, "mono", 1.00 )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pb2000c, pb1000 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(pb2000c_mem)
	MCFG_HD61700_CONFIG(pb2000c_config)

	MCFG_CARTSLOT_ADD("card1")
	MCFG_CARTSLOT_EXTENSION_LIST("bin")
	MCFG_CARTSLOT_NOT_MANDATORY
	MCFG_CARTSLOT_INTERFACE("pb2000c_card")

	MCFG_CARTSLOT_ADD("card2")
	MCFG_CARTSLOT_EXTENSION_LIST("bin")
	MCFG_CARTSLOT_NOT_MANDATORY
	MCFG_CARTSLOT_INTERFACE("pb2000c_card")

	/* Software lists */
	MCFG_SOFTWARE_LIST_ADD("card_list", "pb2000c")
MACHINE_CONFIG_END

/* ROM definition */

ROM_START( pb1000 )
	ROM_REGION( 0x30000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "hd61700.bin", 0x0000, 0x1800, CRC(b28c21a3) SHA1(be7ea62a15ff0c612f6efb2c95e6c2a11a738423))

	ROM_REGION( 0x10000, "rom", ROMREGION_ERASE )
	ROM_SYSTEM_BIOS(0, "basic", "BASIC")
	ROMX_LOAD( "pb1000.bin", 0x0000, 0x8000, CRC(8127a090) SHA1(067c1c2e7efb5249e95afa7805bb98543b30b630), ROM_BIOS(1) | ROM_SKIP(1))
	ROM_SYSTEM_BIOS(1, "basicj", "BASIC Jap")
	ROMX_LOAD( "pb1000j.bin", 0x0000, 0x8000, CRC(14a0df57) SHA1(ab47bb54eb2a24dcd9d2663462e9272d974fa7da), ROM_BIOS(2) | ROM_SKIP(1))


	ROM_REGION( 0x0800, "hd44352", 0 )
	ROM_LOAD( "charset.bin", 0x0000, 0x0800, CRC(7f144716) SHA1(a02f1ecc6dc0ac55b94f00931d8f5cb6b9ffb7b4))
ROM_END

ROM_START( pb2000c )
	ROM_REGION( 0x1800, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "hd61700.bin", 0x0000, 0x1800, CRC(25f9540c) SHA1(ecf98efadbdd4d1a74bc183eaf23f7113f2a12b1))

	ROM_REGION( 0x20000, "rom", ROMREGION_ERASE )
	ROMX_LOAD( "pb2000c.bin", 0x0000, 0x10000, CRC(41933631) SHA1(70b654dc375b647afa042baf8b0a139e7fa604e8), ROM_SKIP(1))

	ROM_REGION( 0x0800, "hd44352", 0 )
	ROM_LOAD( "charset.bin", 0x0000, 0x0800, CRC(7f144716) SHA1(a02f1ecc6dc0ac55b94f00931d8f5cb6b9ffb7b4))

	ROM_REGION( 0x20000, "card1", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "card1", 0, 0x20000, 0 )

	ROM_REGION( 0x20000, "card2", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "card2", 0, 0x20000, 0 )
ROM_END

ROM_START( ai1000 )
	ROM_REGION( 0x1800, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "hd61700.bin", 0x0000, 0x1800, CRC(25f9540c) SHA1(ecf98efadbdd4d1a74bc183eaf23f7113f2a12b1))

	ROM_REGION( 0x20000, "rom", ROMREGION_ERASE )
	ROMX_LOAD( "ai1000.bin", 0x0000, 0x10000, CRC(72aa3ee3) SHA1(ed1d0bc470902ea73bc4588147a589b1793afb40), ROM_SKIP(1))

	ROM_REGION( 0x0800, "hd44352", 0 )
	ROM_LOAD( "charset.bin", 0x0000, 0x0800, CRC(7f144716) SHA1(a02f1ecc6dc0ac55b94f00931d8f5cb6b9ffb7b4))

	ROM_REGION( 0x20000, "card1", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "card1", 0, 0x20000, 0 )

	ROM_REGION( 0x20000, "card2", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "card2", 0, 0x20000, 0 )
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT    COMPANY   FULLNAME       FLAGS */
COMP( 1987, pb1000 ,  0,       0,   pb1000 ,    pb1000 , driver_device,  0, "Casio",   "PB-1000",   GAME_NOT_WORKING)
COMP( 1989, pb2000c,  0,       0,   pb2000c,    pb2000c, driver_device,  0, "Casio",   "PB-2000c",  GAME_NOT_WORKING)
COMP( 1989, ai1000,  pb2000c,  0,   pb2000c,    pb2000c, driver_device,  0, "Casio",   "AI-1000",   GAME_NOT_WORKING)
