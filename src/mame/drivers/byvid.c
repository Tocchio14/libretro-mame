/*
    Bally MPU AS-2518-133
*/


#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "cpu/m6809/m6809.h"
#include "video/tms9928a.h"
#include "machine/6821pia.h"

class by133_state : public driver_device
{
public:
	by133_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_maincpu(*this, "maincpu"),
		  m_videocpu(*this, "videocpu"),
		  m_pia(*this, "pia")
	{ }


	// devices
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_videocpu;
	required_device<cpu_device> m_pia;

	DECLARE_INPUT_CHANGED_MEMBER(test_switch_press);
//  UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
protected:

	// driver_device overrides
	virtual void machine_reset();
};

#if 0
UINT32 by133_state::screen_update( screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	return 0;
}
#endif

static ADDRESS_MAP_START( by133_map, AS_PROGRAM, 8, by133_state )
	AM_RANGE(0x0000, 0xffff) AM_NOP
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)
	AM_RANGE(0x1000, 0x7fff) AM_ROM
ADDRESS_MAP_END



static ADDRESS_MAP_START( by133_video_map, AS_PROGRAM, 8, by133_state )
//  AM_RANGE(0x0000, 0x1fff) communication with main CPU
	AM_RANGE(0x2000, 0x2003) AM_DEVREADWRITE("pia", pia6821_device, read, write)
	AM_RANGE(0x4000, 0x4000) AM_DEVREADWRITE( "tms9928a", tms9928a_device, vram_read, vram_write )
	AM_RANGE(0x4001, 0x4001) AM_DEVREADWRITE( "tms9928a", tms9928a_device, register_read, register_write )
	AM_RANGE(0x6000, 0x63ff) AM_RAM
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

INPUT_CHANGED_MEMBER(by133_state::test_switch_press)
{

	if(newval)
		device_set_input_line(m_videocpu, INPUT_LINE_NMI, PULSE_LINE);
}

static INPUT_PORTS_START( by133 )
	/* service switch is directly hard-wired with the NMI signal */
	PORT_START("TEST")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE ) PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, by133_state,test_switch_press, 0)
INPUT_PORTS_END

void by133_state::machine_reset()
{
}

static DRIVER_INIT( by133 )
{
}

static WRITE_LINE_DEVICE_HANDLER(vdp_interrupt)
{
	cputag_set_input_line(device->machine(), "videocpu", M6809_IRQ_LINE, (state ? ASSERT_LINE : CLEAR_LINE));
}

static TMS9928A_INTERFACE(byvid_tms9928a_interface)
{
	"screen",
	0x4000,
	DEVCB_LINE(vdp_interrupt)
};

static WRITE_LINE_DEVICE_HANDLER( by133_firq )
{
	cputag_set_input_line(device->machine(), "videocpu", M6809_FIRQ_LINE, (state ? ASSERT_LINE : CLEAR_LINE));
}

static READ8_DEVICE_HANDLER(by133_portb_r)
{
	return 0;
}

static const pia6821_interface pia_intf =
{
	DEVCB_NULL,		/* port A in */
	DEVCB_HANDLER(by133_portb_r),		/* port B in */
	DEVCB_NULL,		/* line CA1 in */
	DEVCB_NULL,		/* line CB1 in */
	DEVCB_NULL,		/* line CA2 in */
	DEVCB_NULL,		/* line CB2 in */
	DEVCB_NULL,		/* port A out */
	DEVCB_NULL,		/* port B out */
	DEVCB_NULL,		/* line CA2 out */
	DEVCB_NULL,		/* port CB2 out */
	DEVCB_LINE(by133_firq),		/* IRQA */
	DEVCB_LINE(by133_firq)		/* IRQB */
};


static MACHINE_CONFIG_START( by133, by133_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, 3580000/4)
	MCFG_DEVICE_DISABLE()
	MCFG_CPU_PROGRAM_MAP(by133_map)

	MCFG_CPU_ADD("videocpu", M6809, 3580000/4)
	MCFG_CPU_PROGRAM_MAP(by133_video_map)

	MCFG_PIA6821_ADD("pia", pia_intf)

	/* video hardware */
	MCFG_TMS9928A_ADD( "tms9928a", TMS9928A, byvid_tms9928a_interface )
	MCFG_TMS9928A_SCREEN_ADD_NTSC( "screen" )
	MCFG_SCREEN_UPDATE_DEVICE( "tms9928a", tms9928a_device, screen_update )


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
//  MCFG_SOUND_ADD("aysnd", AY8910, MAIN_CLOCK/4)
//  MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_CONFIG_END

/*-----------------------------------------------------
/ Baby Pacman (Video/Pinball Combo) (BY133-891:  10/82)
/-----------------------------------------------------*/
ROM_START(babypac)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "891-u2.732", 0x1000, 0x0800, CRC(7f7242d1) SHA1(213a697bb7fc69f93ea04621f0fcfdd796f35196))
	ROM_CONTINUE( 0x5000, 0x0800)
	ROM_LOAD( "891-u6.732", 0x1800, 0x0800, CRC(6136d636) SHA1(c01a0a2fcad3bdabd649128e012ab558b1c90cd3) )
	ROM_CONTINUE( 0x5800, 0x0800)
	ROM_RELOAD( 0x7000, 0x1000)
	ROM_REGION(0x10000, "videocpu", 0)
	ROM_LOAD( "891-u9.764",  0x8000, 0x2000, CRC(7fa570f3) SHA1(423ad9266b1ded00fa52ce4180d518874142a203))
	ROM_LOAD( "891-u10.764", 0xa000, 0x2000, CRC(28f4df8b) SHA1(bd6a3598c2c90b5a3a59327616d2f5b9940d98bc))
	ROM_LOAD( "891-u11.764", 0xc000, 0x2000, CRC(0a5967a4) SHA1(26d56ddea3f39d41e382449007bf7ba113c0285f))
	ROM_LOAD( "891-u12.764", 0xe000, 0x2000, CRC(58cfe542) SHA1(e024d14019866bd460d1da6b901f9b786a76a181))
	ROM_REGION(0x10000, "soundcpu", 0)
	ROM_LOAD( "891-u29.764", 0xe000, 0x2000, CRC(0b57fd5d) SHA1(43a03e6d16c87c3305adb04722484f992f23a1bd))
ROM_END

/*-----------------------------------------------------------------
/ Granny and the Gators (Video/Pinball Combo) - (BY35-???: 01/84)
/----------------------------------------------------------------*/
ROM_START(granny)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "cpu_u2.532", 0x1000, 0x0800, CRC(d45bb956) SHA1(86a6942ff9fe38fa109ecde40dc2dd19adf938a9))
	ROM_CONTINUE( 0x5000, 0x0800)
	ROM_LOAD( "cpu_u6.532", 0x1800, 0x0800, CRC(306aa673) SHA1(422c3d9decf9214a18edb536c2077bf52b272e7d) )
	ROM_CONTINUE( 0x5800, 0x0800)
	ROM_RELOAD( 0xf000, 0x1000)
	ROM_REGION(0x10000, "videocpu", 0)
	ROM_LOAD( "vid_u4.764", 0x4000, 0x2000, CRC(3a3d4c6b) SHA1(a6c27eee178a4bde67004e11f6ddf3b6414571dd))
	ROM_LOAD( "vid_u5.764", 0x6000, 0x2000, CRC(78bcb0fb) SHA1(d9dc1cc1bef063d5fbdbf2d1daf793234a9c55a0))
	ROM_LOAD( "vid_u6.764", 0x8000, 0x2000, CRC(8d8220a6) SHA1(64aa7d6ef2702c1b9afc61528434caf56cb91396))
	ROM_LOAD( "vid_u7.764", 0xa000, 0x2000, CRC(aa71cf29) SHA1(b69cd4060f5d4d2a7f85d901552cdc987013fde2))
	ROM_LOAD( "vid_u8.764", 0xc000, 0x2000, CRC(a442bc01) SHA1(2c01123dc5799561ae9e7c5d6db588b82b5ae59c))
	ROM_LOAD( "vid_u9.764", 0xe000, 0x2000, CRC(6b67a1f7) SHA1(251c2b941898363bbd6ee1a94710e2b2938ec851))
	ROM_REGION(0x10000, "soundcpu", 0)
	ROM_LOAD( "cs_u3.764", 0xe000, 0x2000, CRC(0a39a51d) SHA1(98342ba38e48578ce9870f2ee85b553d46c0e35f))
ROM_END


GAME(1982,  babypac,  0,  by133,  by133,  by133,  ROT90,  "Bally",    "Baby Pacman (Video/Pinball Combo)",              GAME_IS_SKELETON_MECHANICAL)
GAME(1984,  granny,   0,  by133,  by133,  by133,  ROT0,  "Bally",    "Granny and the Gators (Video/Pinball Combo)",    GAME_IS_SKELETON_MECHANICAL)
