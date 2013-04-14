/***************************************************************************

        DAG Z80 Trainer

        2013-04-14 Working driver.

No diagram has been found. The following is guesswork.

ToDo:
- Although it works, there's a lot to learn about this unit.

****************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/i8279.h"
#include "dagz80.lh"


class dagz80_state : public driver_device
{
public:
	dagz80_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_maincpu(*this, "maincpu"),
		m_p_ram(*this, "ram")
	{ }

	DECLARE_WRITE8_MEMBER(scanlines_w);
	DECLARE_WRITE8_MEMBER(digit_w);
	DECLARE_READ8_MEMBER(kbd_r);
	DECLARE_MACHINE_RESET(dagz80);
	UINT8 m_digit;
	required_device<cpu_device> m_maincpu;
	required_shared_ptr<UINT8> m_p_ram;
};

static ADDRESS_MAP_START(dagz80_mem, AS_PROGRAM, 8, dagz80_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0xffff) AM_RAM AM_SHARE("ram")
ADDRESS_MAP_END

static ADDRESS_MAP_START(dagz80_io, AS_IO, 8, dagz80_state)
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_DEVREADWRITE("i8279", i8279_device, data_r, data_w )
	AM_RANGE(0x01, 0x01) AM_DEVREADWRITE("i8279", i8279_device, status_r, cmd_w)
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( dagz80 )
/* 2 x 16-key pads
RS SS RN MM     C(IS)  D(FL)  E(FL') F
EX BP TW TR     8(IX)  9(IY)  A(PC)  B(SP)
RL IN -  +      4(AF') 5(BC') 6(DE') 7(HL')
RG EN SA SD     0(AF)  1(BC)  2(DE)  3(HL)
  */
	PORT_START("X0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E FL'") PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D FL") PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C IS") PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("MM") PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RN") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SS") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RS") PORT_CODE(KEYCODE_T)

	PORT_START("X1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B SP") PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A PC") PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9 IY") PORT_CODE(KEYCODE_9) PORT_CHAR('9')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8 IX") PORT_CODE(KEYCODE_8) PORT_CHAR('8')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TR (tape read)") PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TW (tape write)") PORT_CODE(KEYCODE_U)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("BP") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("EX") PORT_CODE(KEYCODE_O)

	PORT_START("X2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7 HL'") PORT_CODE(KEYCODE_7) PORT_CHAR('7')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6 DE'") PORT_CODE(KEYCODE_6) PORT_CHAR('6')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5 BC'") PORT_CODE(KEYCODE_5) PORT_CHAR('5')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4 AF'") PORT_CODE(KEYCODE_4) PORT_CHAR('4')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("+") PORT_CODE(KEYCODE_UP) PORT_CHAR('^')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_DOWN) PORT_CHAR('V')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("IN") PORT_CODE(KEYCODE_S)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RL") PORT_CODE(KEYCODE_G)

	PORT_START("X3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3 HL") PORT_CODE(KEYCODE_3) PORT_CHAR('3')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2 DE") PORT_CODE(KEYCODE_2) PORT_CHAR('2')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1 BC") PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0 AF") PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SD") PORT_CODE(KEYCODE_H)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SA") PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("EN") PORT_CODE(KEYCODE_K)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RG") PORT_CODE(KEYCODE_L)
INPUT_PORTS_END

MACHINE_RESET_MEMBER(dagz80_state, dagz80)
{
	UINT8* rom = memregion("user1")->base();
	UINT16 size = memregion("user1")->bytes();
	memcpy(m_p_ram, rom, size);
	m_maincpu->reset();
}

WRITE8_MEMBER( dagz80_state::scanlines_w )
{
	m_digit = data;
}

WRITE8_MEMBER( dagz80_state::digit_w )
{
	output_set_digit_value(m_digit, BITSWAP8(data, 3, 2, 1, 0, 7, 6, 5, 4));
}

READ8_MEMBER( dagz80_state::kbd_r )
{
	UINT8 data = 0xff;

	if (m_digit < 4)
	{
		char kbdrow[6];
		sprintf(kbdrow,"X%X",m_digit);
		data = ioport(kbdrow)->read();
	}
	return data;
}

static I8279_INTERFACE( dagz80_intf )
{
	DEVCB_NULL, // irq
	DEVCB_DRIVER_MEMBER(dagz80_state, scanlines_w), // scan SL lines
	DEVCB_DRIVER_MEMBER(dagz80_state, digit_w),     // display A&B
	DEVCB_NULL,                     // BD
	DEVCB_DRIVER_MEMBER(dagz80_state, kbd_r),       // kbd RL lines
	DEVCB_LINE_VCC,                     // Shift key
	DEVCB_LINE_VCC
};

static MACHINE_CONFIG_START( dagz80, dagz80_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",Z80, XTAL_4MHz)
	MCFG_CPU_PROGRAM_MAP(dagz80_mem)
	MCFG_CPU_IO_MAP(dagz80_io)
	MCFG_MACHINE_RESET_OVERRIDE(dagz80_state, dagz80 )

	/* video hardware */
	MCFG_DEFAULT_LAYOUT(layout_dagz80)

	/* Devices */
	MCFG_I8279_ADD("i8279", 2500000, dagz80_intf) // based on divider
MACHINE_CONFIG_END

/* ROM definition */

ROM_START( dagz80 )
	ROM_REGION( 0x2000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD( "moni_1.5_15.08.1988.bin", 0x0000, 0x2000, CRC(318AEE6E) SHA1(c698fdee401b88e673791aabcba6a9628938a075) )
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT   CLASS           INIT     COMPANY   FULLNAME       FLAGS */
COMP( 1988, dagz80,  0,       0,     dagz80,    dagz80, driver_device,   0,       "DAG", "DAG Z80 Trainer", GAME_NO_SOUND_HW)
