/********************************************************************************************

    PINBALL
    Stern MP-100 MPU
    (almost identical to Bally MPU-17)


ToDo:
- Display to fix
- Dips, Inputs, Solenoids vary per game
- Mechanical

*********************************************************************************************/


#include "machine/genpin.h"
#include "cpu/m6800/m6800.h"
#include "machine/6821pia.h"
#include "st_mp100.lh"


class st_mp100_state : public genpin_class
{
public:
	st_mp100_state(const machine_config &mconfig, device_type type, const char *tag)
		: genpin_class(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_pia_u10(*this, "pia_u10")
		, m_pia_u11(*this, "pia_u11")
		, m_io_test(*this, "TEST")
		, m_io_dsw0(*this, "DSW0")
		, m_io_dsw1(*this, "DSW1")
		, m_io_dsw2(*this, "DSW2")
		, m_io_dsw3(*this, "DSW3")
		, m_io_x0(*this, "X0")
		, m_io_x1(*this, "X1")
		, m_io_x2(*this, "X2")
		, m_io_x3(*this, "X3")
		, m_io_x4(*this, "X4")
	{ }

	DECLARE_DRIVER_INIT(st_mp100);
	DECLARE_READ8_MEMBER(u10_a_r);
	DECLARE_WRITE8_MEMBER(u10_a_w);
	DECLARE_READ8_MEMBER(u10_b_r);
	DECLARE_WRITE8_MEMBER(u10_b_w);
	DECLARE_READ8_MEMBER(u11_a_r);
	DECLARE_WRITE8_MEMBER(u11_a_w);
	DECLARE_WRITE8_MEMBER(u11_b_w);
	DECLARE_WRITE_LINE_MEMBER(u10_ca2_w);
	DECLARE_WRITE_LINE_MEMBER(u10_cb2_w);
	DECLARE_WRITE_LINE_MEMBER(u11_ca2_w);
	DECLARE_WRITE_LINE_MEMBER(u11_cb2_w);
	DECLARE_INPUT_CHANGED_MEMBER(activity_test);
	DECLARE_INPUT_CHANGED_MEMBER(self_test);
	TIMER_DEVICE_CALLBACK_MEMBER(u10_timer);
	TIMER_DEVICE_CALLBACK_MEMBER(u11_timer);
private:
	UINT8 m_u10;
	UINT8 m_u10_a;
	UINT8 m_u10_b;
	UINT8 m_u11_a;
	UINT8 m_u11_b;
	bool m_u10_ca2;
	bool m_u10_cb2;
	bool m_u10_timer;
	bool m_u11_timer;
	UINT8 m_digit;
	UINT8 m_segment;
	virtual void machine_reset();
	required_device<m6800_cpu_device> m_maincpu;
	required_device<pia6821_device> m_pia_u10;
	required_device<pia6821_device> m_pia_u11;
	required_ioport m_io_test;
	required_ioport m_io_dsw0;
	required_ioport m_io_dsw1;
	required_ioport m_io_dsw2;
	required_ioport m_io_dsw3;
	required_ioport m_io_x0;
	required_ioport m_io_x1;
	required_ioport m_io_x2;
	required_ioport m_io_x3;
	required_ioport m_io_x4;
};


static ADDRESS_MAP_START( st_mp100_map, AS_PROGRAM, 8, st_mp100_state )
	ADDRESS_MAP_GLOBAL_MASK(0x1fff)
	AM_RANGE(0x0000, 0x007f) AM_RAM // internal to the cpu
	AM_RANGE(0x0088, 0x008b) AM_DEVREADWRITE("pia_u10", pia6821_device, read, write)
	AM_RANGE(0x0090, 0x0093) AM_DEVREADWRITE("pia_u11", pia6821_device, read, write)
	AM_RANGE(0x0200, 0x02ff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0x1000, 0x1fff) AM_ROM AM_REGION("roms", 0 )
ADDRESS_MAP_END

static INPUT_PORTS_START( st_mp100 )
	PORT_START("TEST")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_NAME("Self Test") PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, st_mp100_state, self_test, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_NAME("Activity") PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, st_mp100_state, activity_test, 0)

	PORT_START("DSW0")
	PORT_DIPNAME( 0x1f, 0x02, "Coin Slot 2")
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C )) // same as 01
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_2C ))
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_4C ))
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ))
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_7C ))
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ))
	PORT_DIPSETTING(    0x11, DEF_STR( 2C_8C ))
	PORT_DIPSETTING(    0x12, DEF_STR( 1C_9C ))
	PORT_DIPSETTING(    0x13, "2 coins 9 credits")
	PORT_DIPSETTING(    0x14, "1 coin 10 credits")
	PORT_DIPSETTING(    0x15, "2 coins 10 credits")
	PORT_DIPSETTING(    0x16, "1 coin 11 credits")
	PORT_DIPSETTING(    0x17, "2 coins 11 credits")
	PORT_DIPSETTING(    0x18, "1 coin 12 credits")
	PORT_DIPSETTING(    0x19, "2 coins 12 credits")
	PORT_DIPSETTING(    0x1a, "1 coin 13 credits")
	PORT_DIPSETTING(    0x1b, "2 coins 13 credits")
	PORT_DIPSETTING(    0x1c, "1 coin 14 credits")
	PORT_DIPSETTING(    0x1d, "2 coins 14 credits")
	PORT_DIPSETTING(    0x1e, "1 coin 15 credits")
	PORT_DIPSETTING(    0x1f, "2 coins 15 credits")
	PORT_DIPNAME( 0x20, 0x20, "S06")
	PORT_DIPSETTING(    0x00, DEF_STR( No ))
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ))
	PORT_DIPNAME( 0x40, 0x40, "S07")
	PORT_DIPSETTING(    0x00, DEF_STR( No ))
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ))
	PORT_DIPNAME( 0x80, 0x80, "S08")
	PORT_DIPSETTING(    0x00, DEF_STR( No ))
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ))

	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x00, "S09")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x01, DEF_STR( On ))
	PORT_DIPNAME( 0x02, 0x00, "S10")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x02, DEF_STR( On ))
	PORT_DIPNAME( 0x04, 0x00, "S11")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x04, DEF_STR( On ))
	PORT_DIPNAME( 0x08, 0x00, "S12")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x00, "S13")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "S14")
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ))
	PORT_DIPSETTING(    0x20, DEF_STR( No ))
	PORT_DIPNAME( 0x40, 0x40, "S15")
	PORT_DIPSETTING(    0x00, DEF_STR( No ))
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ))
	PORT_DIPNAME( 0x80, 0x00, "S16")
	PORT_DIPSETTING(    0x00, DEF_STR( No ))
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ))

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x00, "S17")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x01, DEF_STR( On ))
	PORT_DIPNAME( 0x02, 0x00, "S18")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x02, DEF_STR( On ))
	PORT_DIPNAME( 0x04, 0x00, "S19")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x04, DEF_STR( On ))
	PORT_DIPNAME( 0x08, 0x00, "S20")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x00, "S21")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "S22")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0x40, 0x00, "S23")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x40, DEF_STR( On ))
	PORT_DIPNAME( 0x80, 0x00, "S24")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x80, DEF_STR( On ))

	PORT_START("DSW3")
	PORT_DIPNAME( 0x03, 0x03, "Maximum Credits")
	PORT_DIPSETTING(    0x00, "10")
	PORT_DIPSETTING(    0x01, "15")
	PORT_DIPSETTING(    0x02, "25")
	PORT_DIPSETTING(    0x03, "40")
	PORT_DIPNAME( 0x04, 0x04, "Credits displayed")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x04, DEF_STR( On ))
	PORT_DIPNAME( 0x08, 0x08, "Match")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x00, "Keep all replays")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "Voice" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0xC0, 0x40, "Balls")
	PORT_DIPSETTING(    0xC0, "2")
	PORT_DIPSETTING(    0x00, "3")
	PORT_DIPSETTING(    0x80, "4")
	PORT_DIPSETTING(    0x40, "5")

	PORT_START("X0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )
	PORT_BIT( 0x0a, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("Outhole") PORT_CODE(KEYCODE_X)

	PORT_START("X1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_BIT( 0x38, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_TILT1 ) PORT_NAME("Slam Tilt")

	// from here, vary per game
	PORT_START("X2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_K)

	PORT_START("X3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_O)

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_COMMA)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_STOP)
INPUT_PORTS_END

INPUT_CHANGED_MEMBER( st_mp100_state::activity_test )
{
	if(newval)
		m_maincpu->set_input_line(INPUT_LINE_NMI, PULSE_LINE);
}

INPUT_CHANGED_MEMBER( st_mp100_state::self_test )
{
	m_pia_u10->ca1_w(newval);
}

WRITE_LINE_MEMBER( st_mp100_state::u10_ca2_w )
{
	m_u10_ca2 = state;
}
		
WRITE_LINE_MEMBER( st_mp100_state::u10_cb2_w )
{
}

WRITE_LINE_MEMBER( st_mp100_state::u11_ca2_w )
{
	output_set_value("led0", !state);
}

WRITE_LINE_MEMBER( st_mp100_state::u11_cb2_w )
{
}

READ8_MEMBER( st_mp100_state::u10_a_r )
{
	return m_u10_a;
}

WRITE8_MEMBER( st_mp100_state::u10_a_w )
{
	static const UINT8 patterns[16] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0,0,0,0,0,0 }; // MC14543
	m_segment = data >> 4;
	m_u10_a = data;
	m_u10 = (data & 15) | (BIT(m_u11_a, 0) << 4);

	if (!m_u10_ca2)
	{
		switch (m_u10)
		{
		case 0x10:
			output_set_digit_value(m_digit, patterns[m_segment]);
			break;
		case 0x1d:
			output_set_digit_value(8+m_digit, patterns[m_segment]);
			break;
		case 0x1b:
			output_set_digit_value(16+m_digit, patterns[m_segment]);
			break;
		case 0x07:
			output_set_digit_value(24+m_digit, patterns[m_segment]);
			break;
		case 0x0f:
			output_set_digit_value(32+m_digit, patterns[m_segment]);
			break;
		default:
			break;
		}
	}
}

READ8_MEMBER( st_mp100_state::u10_b_r )
{
	UINT8 data = 0;

	if (BIT(m_u10_a, 0))
		data |= m_io_x0->read();

	if (BIT(m_u10_a, 1))
		data |= m_io_x1->read();

	if (BIT(m_u10_a, 2))
		data |= m_io_x2->read();

	if (BIT(m_u10_a, 3))
		data |= m_io_x3->read();

	if (BIT(m_u10_a, 4))
		data |= m_io_x4->read();

	if (BIT(m_u10_a, 5))
		data |= m_io_dsw0->read();

	if (BIT(m_u10_a, 6))
		data |= m_io_dsw1->read();

	if (BIT(m_u10_a, 7))
		data |= m_io_dsw2->read();

	if (m_u10_cb2)
		data |= m_io_dsw3->read();

	return data;
}

WRITE8_MEMBER( st_mp100_state::u10_b_w )
{
	m_u10_b = data;
}

READ8_MEMBER( st_mp100_state::u11_a_r )
{
	return m_u11_a;
}

WRITE8_MEMBER( st_mp100_state::u11_a_w )
{
	m_u11_a = data;

	m_digit = 0xff;
	if BIT(data, 2)
		m_digit = 4;
	else
	if BIT(data, 3)
		m_digit = 3;
	else
	if BIT(data, 4)
		m_digit = 2;
	else
	if BIT(data, 5)
		m_digit = 1;
	else
	if BIT(data, 6)
		m_digit = 0;
	else
	if BIT(data, 7)
		m_digit = 5;
}

WRITE8_MEMBER( st_mp100_state::u11_b_w )
{
	m_u11_b = data;
	switch (data & 15)
	{
		case 0x0: //
			//m_samples->start(0, 3);
			break;
		case 0x1: // chime 10
			m_samples->start(1, 1);
			break;
		case 0x2: // chime 100
			m_samples->start(2, 2);
			break;
		case 0x3: // chime 1000
			m_samples->start(3, 3);
			break;
		case 0x4: // chime 10000
			m_samples->start(0, 4);
			break;
		case 0x5: // knocker
			m_samples->start(0, 6);
			break;
		case 0x6: // outhole
			m_samples->start(0, 5);
			break;
		// from here, vary per game
		case 0x7:
		case 0x8:
		case 0x9:
			//m_samples->start(0, 5);
			break;
		case 0xa:
			//m_samples->start(0, 5);
			break;
		case 0xb:
			//m_samples->start(0, 0);
			break;
		case 0xc:
			//m_samples->start(0, 5);
			break;
		case 0xd:
			//m_samples->start(0, 0);
			break;
		case 0xe:
			//m_samples->start(0, 5);
			break;
		case 0xf: // not used
			break;
	}
}

void st_mp100_state::machine_reset()
{
	m_u10_a = 0;
	m_u10_b = 0;
	m_u10_cb2 = 0;
	m_u11_a = 0;
	m_u11_b = 0;
}

DRIVER_INIT_MEMBER(st_mp100_state,st_mp100)
{
}

// zero-cross detection
TIMER_DEVICE_CALLBACK_MEMBER( st_mp100_state::u10_timer )
{
	m_u10_timer ^= 1;
	m_pia_u10->cb1_w(m_u10_timer);
}

// 555 timer for display refresh
TIMER_DEVICE_CALLBACK_MEMBER( st_mp100_state::u11_timer )
{
	m_u11_timer ^= 1;
	m_pia_u11->ca1_w(m_u11_timer);
}

static MACHINE_CONFIG_START( st_mp100, st_mp100_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, 1000000) // no xtal, just 2 chips forming a random oscillator
	MCFG_CPU_PROGRAM_MAP(st_mp100_map)

	MCFG_NVRAM_ADD_0FILL("nvram")

	/* Video */
	MCFG_DEFAULT_LAYOUT(layout_st_mp100)

	/* Sound */
	MCFG_FRAGMENT_ADD( genpin_audio )

	/* Devices */
	MCFG_DEVICE_ADD("pia_u10", PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(st_mp100_state, u10_a_r))
	MCFG_PIA_WRITEPA_HANDLER(WRITE8(st_mp100_state, u10_a_w))
	MCFG_PIA_READPB_HANDLER(READ8(st_mp100_state, u10_b_r))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(st_mp100_state, u10_b_w))
	MCFG_PIA_CA2_HANDLER(WRITELINE(st_mp100_state, u10_ca2_w))
	MCFG_PIA_CB2_HANDLER(WRITELINE(st_mp100_state, u10_cb2_w))
	MCFG_PIA_IRQA_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))
	MCFG_PIA_IRQB_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))
	MCFG_TIMER_DRIVER_ADD_PERIODIC("timer_z", st_mp100_state, u10_timer, attotime::from_hz(120)) // mains freq*2

	MCFG_DEVICE_ADD("pia_u11", PIA6821, 0)
	MCFG_PIA_READPA_HANDLER(READ8(st_mp100_state, u11_a_r))
	MCFG_PIA_WRITEPA_HANDLER(WRITE8(st_mp100_state, u11_a_w))
	MCFG_PIA_WRITEPB_HANDLER(WRITE8(st_mp100_state, u11_b_w))
	MCFG_PIA_CA2_HANDLER(WRITELINE(st_mp100_state, u11_ca2_w))
	MCFG_PIA_CB2_HANDLER(WRITELINE(st_mp100_state, u11_cb2_w))
	MCFG_PIA_IRQA_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))
	MCFG_PIA_IRQB_HANDLER(DEVWRITELINE("maincpu", m6800_cpu_device, irq_line))
	MCFG_TIMER_DRIVER_ADD_PERIODIC("timer_d", st_mp100_state, u11_timer, attotime::from_hz(634)) // 555 timer*2
MACHINE_CONFIG_END


/*-------------------------------------
/ Cosmic Princess - same ROMs as Magic
/-------------------------------------*/
ROM_START(princess)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(8838091f) SHA1(d2702b5e15076793b4560c77b78eed6c1da571b6))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))
ROM_END

/*--------------------------------
/ Dracula #109
/-------------------------------*/
ROM_START(dracula)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(063f8b5e) SHA1(80434de549102bff829b474603d6736b839b8999))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(00336fbc) SHA1(d2c360b8a80b209ecf4ec02ee19a5234c0364504))
ROM_END

/*--------------------------------
/ Hot Hand - uses MPU-200 inports #112
/-------------------------------*/
ROM_START(hothand)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(5e79ea2e) SHA1(9b45c59b2076fcb3a35de1dd3ba2444ea852f149))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))
ROM_END

/*--------------------------------
/ Lectronamo #105
/-------------------------------*/
ROM_START(lectrono)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(79e918ff) SHA1(a728eb26d941a9c7484be593a216905237d32551))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(7c6e5fb5) SHA1(3aa4e0c1f377ba024e6b34bd431a188ff02d4eaa))
ROM_END

/*--------------------------------
/ Magic - uses MPU-200 inports #115
/-------------------------------*/
ROM_START(magic)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(8838091f) SHA1(d2702b5e15076793b4560c77b78eed6c1da571b6))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))
ROM_END

/*--------------------------------
/ Memory Lane #104
/-------------------------------*/
ROM_START(memlane)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(aff1859d) SHA1(5a9801d139bf2477b6d351a2654ae07516be144a))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(3e236e3c) SHA1(7f631a5fac8a1b1af3b5332ba38d52553f13531a))
ROM_END

/*--------------------------------
/ Nugent #108
/-------------------------------*/
ROM_START(nugent)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(79e918ff) SHA1(a728eb26d941a9c7484be593a216905237d32551))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(7c6e5fb5) SHA1(3aa4e0c1f377ba024e6b34bd431a188ff02d4eaa))
ROM_END

/*--------------------------------
/ Pinball #101
/-------------------------------*/
ROM_START(pinball)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(1db32a33) SHA1(2f0a3ca36968b81f29373e4f2cf7ee28a4071882))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(432e9b9e) SHA1(292e509f50bc841f6e469c198fc82c2a9095f008))
ROM_END

/*--------------------------------
/ Stars #103
/-------------------------------*/
ROM_START(stars)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(630d05df) SHA1(2baa16265d524297332fa951d9eab3e0e8d26078))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(57e63d42) SHA1(619ef955553654893c3071d8b70855fee8a5e6a7))
ROM_END

/*--------------------------------
/ Stingray #102
/-------------------------------*/
ROM_START(stingray)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(1db32a33) SHA1(2f0a3ca36968b81f29373e4f2cf7ee28a4071882))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(432e9b9e) SHA1(292e509f50bc841f6e469c198fc82c2a9095f008))
ROM_END

/*--------------------------------
/ Trident - uses MPU-200 inports #110
/-------------------------------*/
ROM_START(trident)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(934e49dd) SHA1(cbf6ca2759166f522f651825da0c75cf7248d3da))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(540bce56) SHA1(0b21385501b83e448403e0216371487ed54026b7))
ROM_END

/*--------------------------------
/ Wildfyre #106
/-------------------------------*/
ROM_START(wildfyre)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(063f8b5e) SHA1(80434de549102bff829b474603d6736b839b8999))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(00336fbc) SHA1(d2c360b8a80b209ecf4ec02ee19a5234c0364504))
ROM_END

GAME(1977,  pinball,    0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Pinball",              GAME_IS_SKELETON_MECHANICAL)
GAME(1977,  stingray,   0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Stingray",             GAME_IS_SKELETON_MECHANICAL)
GAME(1978,  stars,      0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Stars",                GAME_IS_SKELETON_MECHANICAL)
GAME(1978,  memlane,    0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Memory Lane",          GAME_IS_SKELETON_MECHANICAL)
GAME(1978,  lectrono,   0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Lectronamo",           GAME_IS_SKELETON_MECHANICAL)
GAME(1978,  wildfyre,   0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Wildfyre",             GAME_IS_SKELETON_MECHANICAL)
GAME(1978,  nugent,     0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Nugent",               GAME_IS_SKELETON_MECHANICAL)
GAME(1979,  dracula,    0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Dracula",              GAME_IS_SKELETON_MECHANICAL)
GAME(1979,  trident,    0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Trident",              GAME_IS_SKELETON_MECHANICAL)
GAME(1979,  hothand,    0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Hot Hand",             GAME_IS_SKELETON_MECHANICAL)
GAME(1979,  magic,      0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Magic",                GAME_IS_SKELETON_MECHANICAL)
GAME(1979,  princess,   0,      st_mp100,   st_mp100, st_mp100_state,   st_mp100,   ROT0,   "Stern",    "Cosmic Princess",      GAME_IS_SKELETON_MECHANICAL)
