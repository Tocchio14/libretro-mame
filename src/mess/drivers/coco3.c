/***************************************************************************

    coco3.c

    TRS-80 Radio Shack Color Computer 3

    Mathis Rosenhauer (original driver)
    Nate Woods (current maintainer)
    Tim Lindner (VHD and other work)

***************************************************************************/

#include "includes/coco3.h"
#include "cpu/m6809/m6809.h"
#include "cpu/m6809/hd6309.h"
#include "formats/coco_cas.h"
#include "coco3.lh"



//**************************************************************************
//  ADDRESS MAPS
//**************************************************************************

//-------------------------------------------------
//  ADDRESS_MAP( coco3_mem )
//-------------------------------------------------

static ADDRESS_MAP_START( coco3_mem, AS_PROGRAM, 8, coco3_state )
	AM_RANGE(0x0000, 0x1FFF) AM_READ_BANK("rbank0") AM_WRITE_BANK("wbank0")
	AM_RANGE(0x2000, 0x3FFF) AM_READ_BANK("rbank1") AM_WRITE_BANK("wbank1")
	AM_RANGE(0x4000, 0x5FFF) AM_READ_BANK("rbank2") AM_WRITE_BANK("wbank2")
	AM_RANGE(0x6000, 0x7FFF) AM_READ_BANK("rbank3") AM_WRITE_BANK("wbank3")
	AM_RANGE(0x8000, 0x9FFF) AM_READ_BANK("rbank4") AM_WRITE_BANK("wbank4")
	AM_RANGE(0xA000, 0xBFFF) AM_READ_BANK("rbank5") AM_WRITE_BANK("wbank5")
	AM_RANGE(0xC000, 0xDFFF) AM_READ_BANK("rbank6") AM_WRITE_BANK("wbank6")
	AM_RANGE(0xE000, 0xFDFF) AM_READ_BANK("rbank7") AM_WRITE_BANK("wbank7")
	AM_RANGE(0xFE00, 0xFEFF) AM_READ_BANK("rbank8") AM_WRITE_BANK("wbank8")
	AM_RANGE(0xFF00, 0xFF1F) AM_READWRITE(ff00_read, ff00_write)
	AM_RANGE(0xFF20, 0xFF3F) AM_READWRITE(ff20_read, ff20_write)
	AM_RANGE(0xFF40, 0xFF5F) AM_READWRITE(ff40_read, ff40_write)
	AM_RANGE(0xFF60, 0xFF8F) AM_READWRITE(ff60_read, ff60_write)
	AM_RANGE(0xFF90, 0xFFDF) AM_DEVREADWRITE(GIME_TAG, gime_base_device, read, write)

	// While Tepolt and other sources say that the interrupt vectors are mapped to
	// the same memory accessed at $BFFx, William Astle offered evidence that this
	// memory on a CoCo 3 is not the same.
	//
	// http://lost.l-w.ca/0x05/coco3-and-interrupt-vectors/
	AM_RANGE(0xFFE0, 0xFFFF) AM_ROM AM_REGION(MAINCPU_TAG, 0x7FE0)
ADDRESS_MAP_END



//**************************************************************************
//  INPUT PORTS
//**************************************************************************

//-------------------------------------------------
//  INPUT_PORTS( coco3_keyboard )
//-------------------------------------------------
//
//  CoCo 3 keyboard
//
//         PB0 PB1 PB2 PB3 PB4 PB5 PB6 PB7
//    PA6: Ent Clr Brk Alt Ctr F1  F2 Shift
//    PA5: 8   9   :   ;   ,   -   .   /
//    PA4: 0   1   2   3   4   5   6   7
//    PA3: X   Y   Z   Up  Dwn Lft Rgt Space
//    PA2: P   Q   R   S   T   U   V   W
//    PA1: H   I   J   K   L   M   N   O
//    PA0: @   A   B   C   D   E   F   G
//-------------------------------------------------

static INPUT_PORTS_START( coco3_keyboard )
	PORT_START("row0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_ASTERISK) PORT_CHAR('@')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_G) PORT_CHAR('G')

	PORT_START("row1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_O) PORT_CHAR('O')

	PORT_START("row2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_W) PORT_CHAR('W')

	PORT_START("row3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("UP") PORT_CODE(KEYCODE_UP) PORT_CHAR('^')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("DOWN") PORT_CODE(KEYCODE_DOWN) PORT_CHAR(10)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("LEFT") PORT_CODE(KEYCODE_LEFT) PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("RIGHT") PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(9)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("SPACE") PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')

	PORT_START("row4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('\"')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')

	PORT_START("row5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_COLON) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-') PORT_CHAR('=') PORT_CHAR('_')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')

	PORT_START("row6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("CLEAR") PORT_CODE(KEYCODE_HOME) PORT_CHAR(12)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("BREAK") PORT_CODE(KEYCODE_END) PORT_CODE(KEYCODE_ESC) PORT_CHAR(27)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("ALT") PORT_CODE(KEYCODE_LALT)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("CTRL") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_NAME("SHIFT") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
INPUT_PORTS_END



//-------------------------------------------------
//  INPUT_PORTS( coco3_joystick )
//-------------------------------------------------

static INPUT_PORTS_START( coco3_joystick )
	PORT_START(JOYSTICK_RX_TAG)
	PORT_BIT( 0xff, 0x80,  IPT_AD_STICK_X) PORT_SENSITIVITY(JOYSTICK_SENSITIVITY) PORT_KEYDELTA(JOYSTICK_DELTA) PORT_MINMAX(0x00,0xFF) PORT_CODE_DEC(KEYCODE_4_PAD) PORT_CODE_INC(KEYCODE_6_PAD) PORT_CODE_DEC(JOYCODE_X_LEFT_SWITCH) PORT_CODE_INC(JOYCODE_X_RIGHT_SWITCH) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x01)
	PORT_START(JOYSTICK_RY_TAG)
	PORT_BIT( 0xff, 0x80,  IPT_AD_STICK_Y) PORT_SENSITIVITY(JOYSTICK_SENSITIVITY) PORT_KEYDELTA(JOYSTICK_DELTA) PORT_MINMAX(0x00,0xFF) PORT_CODE_DEC(KEYCODE_8_PAD) PORT_CODE_INC(KEYCODE_2_PAD) PORT_CODE_DEC(JOYCODE_Y_UP_SWITCH)   PORT_CODE_INC(JOYCODE_Y_DOWN_SWITCH)  PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x01)
	PORT_START(JOYSTICK_LX_TAG)
	PORT_BIT( 0xff, 0x80,  IPT_AD_STICK_X) PORT_SENSITIVITY(JOYSTICK_SENSITIVITY) PORT_KEYDELTA(JOYSTICK_DELTA) PORT_MINMAX(0x00,0xFF) PORT_CODE_DEC(KEYCODE_4_PAD) PORT_CODE_INC(KEYCODE_6_PAD) PORT_CODE_DEC(JOYCODE_X_LEFT_SWITCH) PORT_CODE_INC(JOYCODE_X_RIGHT_SWITCH) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x10)
	PORT_START(JOYSTICK_LY_TAG)
	PORT_BIT( 0xff, 0x80,  IPT_AD_STICK_Y) PORT_SENSITIVITY(JOYSTICK_SENSITIVITY) PORT_KEYDELTA(JOYSTICK_DELTA) PORT_MINMAX(0x00,0xFF) PORT_CODE_DEC(KEYCODE_8_PAD) PORT_CODE_INC(KEYCODE_2_PAD) PORT_CODE_DEC(JOYCODE_Y_UP_SWITCH)   PORT_CODE_INC(JOYCODE_Y_DOWN_SWITCH)  PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x10)
	PORT_START(JOYSTICK_BUTTONS_TAG)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Right Button 1") PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(JOYCODE_BUTTON1) PORT_CODE(MOUSECODE_BUTTON1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Right Button 1") PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_DEL_PAD) PORT_CODE(JOYCODE_BUTTON2) PORT_CODE(MOUSECODE_BUTTON2) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Left Button 1")  PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(JOYCODE_BUTTON1) PORT_CODE(MOUSECODE_BUTTON1) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Left Button 1")  PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_DEL_PAD) PORT_CODE(JOYCODE_BUTTON2) PORT_CODE(MOUSECODE_BUTTON2) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x10)
INPUT_PORTS_END



//-------------------------------------------------
//  INPUT_PORTS( coco_rat_mouse )
//-------------------------------------------------

static INPUT_PORTS_START( coco_rat_mouse )
	PORT_START(RAT_MOUSE_RX_TAG)
	PORT_BIT( 0x03, 0x00,  IPT_MOUSE_X) PORT_NAME("Rat Mouse X (Right Port)") PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_LEFT) PORT_CODE_INC(KEYCODE_RIGHT) PORT_CODE_DEC(JOYCODE_X_LEFT_SWITCH) PORT_CODE_INC(JOYCODE_X_RIGHT_SWITCH) PORT_PLAYER(1)  PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x02)
	PORT_START(RAT_MOUSE_RY_TAG)
	PORT_BIT( 0x03, 0x00,  IPT_MOUSE_Y) PORT_NAME("Rat Mouse Y (Right Port)") PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_UP) PORT_CODE_INC(KEYCODE_DOWN) PORT_CODE_DEC(JOYCODE_Y_UP_SWITCH) PORT_CODE_INC(JOYCODE_Y_DOWN_SWITCH) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x02)
	PORT_START(RAT_MOUSE_LX_TAG)
	PORT_BIT( 0x03, 0x00,  IPT_MOUSE_X) PORT_NAME("Rat Mouse X (Left Port)") PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_LEFT) PORT_CODE_INC(KEYCODE_RIGHT) PORT_CODE_DEC(JOYCODE_X_LEFT_SWITCH) PORT_CODE_INC(JOYCODE_X_RIGHT_SWITCH) PORT_PLAYER(2)  PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x20)
	PORT_START(RAT_MOUSE_LY_TAG)
	PORT_BIT( 0x03, 0x00,  IPT_MOUSE_Y) PORT_NAME("Rat Mouse Y (Left Port)") PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_UP) PORT_CODE_INC(KEYCODE_DOWN) PORT_CODE_DEC(JOYCODE_Y_UP_SWITCH) PORT_CODE_INC(JOYCODE_Y_DOWN_SWITCH) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x20)
	PORT_START(RAT_MOUSE_BUTTONS_TAG)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Rat Mouse Button 1 (Right Port)") PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(JOYCODE_BUTTON1) PORT_CODE(MOUSECODE_BUTTON1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Rat Mouse Button 2 (Right Port)") PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_DEL_PAD) PORT_CODE(JOYCODE_BUTTON2) PORT_CODE(MOUSECODE_BUTTON2) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Rat Mouse Button 1 (Left Port)")  PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(JOYCODE_BUTTON1) PORT_CODE(MOUSECODE_BUTTON1) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Rat Mouse Button 2 (Left Port)")  PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_DEL_PAD) PORT_CODE(JOYCODE_BUTTON2) PORT_CODE(MOUSECODE_BUTTON2) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x20)
INPUT_PORTS_END



//-------------------------------------------------
//  INPUT_PORTS( coco_lightgun )
//-------------------------------------------------

static INPUT_PORTS_START( coco_lightgun )
	PORT_START(DIECOM_LIGHTGUN_RX_TAG)
	PORT_BIT( 0x1ff, 266, IPT_LIGHTGUN_X ) PORT_NAME("Lightgun X (Right Port)") PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_MINMAX(116,416) PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x03)
	PORT_START(DIECOM_LIGHTGUN_RY_TAG)
	PORT_BIT( 0xff, 121, IPT_LIGHTGUN_Y ) PORT_NAME("Lightgun Y (Right Port)") PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_MINMAX(0,242) PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x03)
	PORT_START(DIECOM_LIGHTGUN_LX_TAG)
	PORT_BIT( 0x1ff, 266, IPT_LIGHTGUN_X ) PORT_NAME("Lightgun X (Left Port)") PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_MINMAX(116,416) PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x30)
	PORT_START(DIECOM_LIGHTGUN_LY_TAG)
	PORT_BIT( 0xff, 121, IPT_LIGHTGUN_Y ) PORT_NAME("Lightgun Y (Left Port)") PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_MINMAX(0,242) PORT_SENSITIVITY(100) PORT_KEYDELTA(1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x30)
	PORT_START(DIECOM_LIGHTGUN_BUTTONS_TAG)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Lightgun Trigger (Right Port)") PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(JOYCODE_BUTTON1) PORT_CODE(MOUSECODE_BUTTON1) PORT_PLAYER(1) PORT_CONDITION(CTRL_SEL_TAG, 0x0f, EQUALS, 0x03)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Lightgun Trigger (Left Port)")  PORT_CHANGED_MEMBER(DEVICE_SELF, coco3_state, coco_state::keyboard_changed, NULL) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(JOYCODE_BUTTON1) PORT_CODE(MOUSECODE_BUTTON1) PORT_PLAYER(2) PORT_CONDITION(CTRL_SEL_TAG, 0xf0, EQUALS, 0x30)
INPUT_PORTS_END



//-------------------------------------------------
//  INPUT_PORTS( coco3 )
//-------------------------------------------------

static INPUT_PORTS_START( coco3 )
	PORT_INCLUDE( coco3_keyboard )
	PORT_INCLUDE( coco3_joystick )
	PORT_INCLUDE( coco_analog_control )
	PORT_INCLUDE( coco_cart_autostart )
	PORT_INCLUDE( coco_rat_mouse )
	PORT_INCLUDE( coco_lightgun )
	PORT_INCLUDE( coco_rtc )
INPUT_PORTS_END

static DEVICE_INPUT_DEFAULTS_START( printer )
	DEVICE_INPUT_DEFAULTS( "RS232_RXBAUD", 0xff, RS232_BAUD_600 )
	DEVICE_INPUT_DEFAULTS( "RS232_STARTBITS", 0xff, RS232_STARTBITS_1 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_8 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_NONE )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_1 )
DEVICE_INPUT_DEFAULTS_END


//**************************************************************************
//  MACHINE CONFIGURATION
//**************************************************************************

static const gime_interface coco_gime_config =
{
	/* device tags */
	SCREEN_TAG,
	MAINCPU_TAG,
	RAM_TAG,
	CARTRIDGE_TAG,
};

static MACHINE_CONFIG_START( coco3, coco3_state )
	// basic machine hardware
	MCFG_CPU_ADD(MAINCPU_TAG, M6809E, XTAL_3_579545MHz)
	MCFG_CPU_PROGRAM_MAP(coco3_mem)

	// devices
	MCFG_DEVICE_ADD(PIA0_TAG, PIA6821, 0)
	MCFG_PIA_WRITEPA_HANDLER(WRITE8(coco_state, pia0_pa_w))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(coco_state, pia0_pb_w))
	MCFG_PIA_CA2_HANDLER(WRITELINE(coco_state, pia0_ca2_w))
	MCFG_PIA_CB2_HANDLER(WRITELINE(coco_state, pia0_cb2_w))
	MCFG_PIA_IRQA_HANDLER(WRITELINE(coco_state, pia0_irq_a))
	MCFG_PIA_IRQB_HANDLER(WRITELINE(coco_state, pia0_irq_b))

	MCFG_DEVICE_ADD(PIA1_TAG, PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(coco_state, pia1_pa_r))
	MCFG_PIA_READPB_HANDLER(READ8(coco_state, pia1_pb_r))
	MCFG_PIA_WRITEPA_HANDLER(WRITE8(coco_state, pia1_pa_w))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(coco_state, pia1_pb_w))
	MCFG_PIA_CA2_HANDLER(WRITELINE(coco_state, pia1_ca2_w))
	MCFG_PIA_CB2_HANDLER(WRITELINE(coco_state, pia1_cb2_w))
	MCFG_PIA_IRQA_HANDLER(WRITELINE(coco_state, pia1_firq_a))
	MCFG_PIA_IRQB_HANDLER(WRITELINE(coco_state, pia1_firq_b))

	MCFG_CASSETTE_ADD("cassette")
	MCFG_CASSETTE_FORMATS(coco_cassette_formats)
	MCFG_CASSETTE_DEFAULT_STATE(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_MUTED)

	MCFG_RS232_PORT_ADD(RS232_TAG, default_rs232_devices, "printer")
	MCFG_RS232_DCD_HANDLER(DEVWRITELINE(PIA1_TAG, pia6821_device, ca1_w))
	MCFG_DEVICE_CARD_DEVICE_INPUT_DEFAULTS("printer", printer)

	MCFG_COCO_CARTRIDGE_ADD(CARTRIDGE_TAG, coco_cart, "fdcv11")
	MCFG_COCO_CARTRIDGE_CART_CB(WRITELINE(coco_state, cart_w))
	MCFG_COCO_CARTRIDGE_NMI_CB(INPUTLINE(MAINCPU_TAG, INPUT_LINE_NMI))
	MCFG_COCO_CARTRIDGE_HALT_CB(INPUTLINE(MAINCPU_TAG, INPUT_LINE_HALT))
		
	MCFG_COCO_VHD_ADD(VHD0_TAG)
	MCFG_COCO_VHD_ADD(VHD1_TAG)

	// video hardware
	MCFG_DEFAULT_LAYOUT(layout_coco3)

	MCFG_DEVICE_ADD(GIME_TAG, GIME_NTSC, XTAL_3_579545MHz)
	MCFG_DEVICE_CONFIG(coco_gime_config)
	MCFG_GIME_HSYNC_CALLBACK(DEVWRITELINE(PIA0_TAG, pia6821_device, ca1_w))
	MCFG_GIME_FSYNC_CALLBACK(DEVWRITELINE(PIA0_TAG, pia6821_device, cb1_w))
	MCFG_GIME_IRQ_CALLBACK(WRITELINE(coco3_state, gime_irq_w))
	MCFG_GIME_FIRQ_CALLBACK(WRITELINE(coco3_state, gime_firq_w))
	MCFG_GIME_FLOATING_BUS_CALLBACK(READ8(coco_state, floating_bus_read))

	// composite monitor
	MCFG_SCREEN_ADD(COMPOSITE_SCREEN_TAG, RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_UPDATE_DRIVER(coco3_state, screen_update)
	MCFG_SCREEN_SIZE(640, 243)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 1, 241-1)
	MCFG_SCREEN_VBLANK_TIME(0)

	// RGB monitor
	MCFG_SCREEN_ADD(RGB_SCREEN_TAG, RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_UPDATE_DRIVER(coco3_state, screen_update)
	MCFG_SCREEN_SIZE(640, 243)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 1, 241-1)
	MCFG_SCREEN_VBLANK_TIME(0)

	// sound hardware
	MCFG_FRAGMENT_ADD( coco_sound )

	// internal ram
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("512K")
	MCFG_RAM_EXTRA_OPTIONS("128K,2M,8M")

	// software lists
	MCFG_SOFTWARE_LIST_ADD("cart_list","coco_cart")

	MCFG_SOFTWARE_LIST_ADD("flop_list","coco_flop")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( coco3p, coco3 )
	MCFG_DEVICE_REPLACE(GIME_TAG, GIME_PAL, XTAL_4_433619MHz)
	MCFG_DEVICE_CONFIG(coco_gime_config)
	MCFG_GIME_HSYNC_CALLBACK(DEVWRITELINE(PIA0_TAG, pia6821_device, ca1_w))
	MCFG_GIME_FSYNC_CALLBACK(DEVWRITELINE(PIA0_TAG, pia6821_device, cb1_w))
	MCFG_GIME_IRQ_CALLBACK(WRITELINE(coco3_state, gime_irq_w))
	MCFG_GIME_FIRQ_CALLBACK(WRITELINE(coco3_state, gime_firq_w))
	MCFG_GIME_FLOATING_BUS_CALLBACK(READ8(coco_state, floating_bus_read))
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( coco3h, coco3 )
	MCFG_CPU_REPLACE(MAINCPU_TAG, HD6309, XTAL_3_579545MHz)
	MCFG_CPU_PROGRAM_MAP(coco3_mem)
MACHINE_CONFIG_END



//**************************************************************************
//  ROMS
//**************************************************************************

ROM_START(coco3)
	ROM_REGION(0x8000,MAINCPU_TAG,0)
	ROM_LOAD("coco3.rom",   0x0000, 0x8000, CRC(b4c88d6c) SHA1(e0d82953fb6fd03768604933df1ce8bc51fc427d))
ROM_END

ROM_START(coco3p)
	ROM_REGION(0x8000,MAINCPU_TAG,0)
	ROM_LOAD("coco3p.rom",  0x0000, 0x8000, CRC(ff050d80) SHA1(631e383068b1f52a8f419f4114b69501b21cf379))
ROM_END

#define rom_coco3h  rom_coco3



//**************************************************************************
//  SYSTEM DRIVERS
//**************************************************************************

COMP(  1986,    coco3,      coco,   0,      coco3,     coco3, driver_device,     0,      "Tandy Radio Shack",            "Color Computer 3 (NTSC)", 0)
COMP(  1986,    coco3p,     coco,   0,      coco3p,    coco3, driver_device,     0,      "Tandy Radio Shack",            "Color Computer 3 (PAL)", 0)
COMP(  19??,    coco3h,     coco,   0,      coco3h,    coco3, driver_device,     0,      "Tandy Radio Shack",            "Color Computer 3 (NTSC; HD6309)", GAME_UNOFFICIAL)
