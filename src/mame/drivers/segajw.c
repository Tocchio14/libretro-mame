/***************************************************************************

============================================================================

SEGA GOLDEN POKER SERIES "JOKER'S WILD" (REV.B)
(c) SEGA

MAIN CPU  : 68000 Z-80
CRTC      : HITACHI HD63484 (24KHz OUTPUT)
SOUND     : YM3438

14584B.EPR  ; MAIN BOARD  IC20 EPR-14584B (27C1000 MAIN-ODD)
14585B.EPR  ; MAIN BOARD  IC22 EPR-14585B (27C1000 MAIN-EVEN)
14586.EPR   ; MAIN BOARD  IC26 EPR-14586  (27C4096 BG)
14587A.EPR  ; SOUND BOARD IC51 EPR-14587A (27C1000 SOUND)

------------------------------------------------------------------

***************************************************************************/


#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "machine/nvram.h"
#include "video/h63484.h"

class segajw_state : public driver_device
{
public:
	segajw_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_palette(*this, "palette")
	{ }

	DECLARE_READ16_MEMBER(coin_counter_r);
	DECLARE_WRITE16_MEMBER(coin_counter_w);
	DECLARE_READ16_MEMBER(hopper_r);
	DECLARE_WRITE16_MEMBER(hopper_w);
	DECLARE_READ16_MEMBER(coinlockout_r);
	DECLARE_WRITE16_MEMBER(coinlockout_w);
	DECLARE_READ16_MEMBER(soundboard_r);
	DECLARE_WRITE8_MEMBER(ramdac_io_w);
	DECLARE_INPUT_CHANGED_MEMBER(coin_drop_start);
	DECLARE_CUSTOM_INPUT_MEMBER(coin_sensors_r);
	DECLARE_CUSTOM_INPUT_MEMBER(hopper_sensors_r);

protected:

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<palette_device> m_palette;

	// driver_device overrides
	virtual void machine_start();
	virtual void machine_reset();
	struct { int r,g,b,offs,offs_internal; } m_pal;
	UINT64      m_coin_start_cycles;
	UINT64      m_hopper_start_cycles;
	UINT8       m_coin_counter;
	UINT16      m_coin_lockout;
	UINT8       m_hopper_ctrl;
};


READ16_MEMBER(segajw_state::coin_counter_r)
{
	return m_coin_counter ^ 0xff;
}

WRITE16_MEMBER(segajw_state::coin_counter_w)
{
	if(ACCESSING_BITS_0_7)
		m_coin_counter = data;
}

READ16_MEMBER(segajw_state::hopper_r)
{
	return m_hopper_ctrl;
}

WRITE16_MEMBER(segajw_state::hopper_w)
{
	if(ACCESSING_BITS_0_7)
	{
		m_hopper_start_cycles = data & 0x02 ? 0 : m_maincpu->total_cycles();
		m_hopper_ctrl = data;
	}
}

READ16_MEMBER(segajw_state::coinlockout_r)
{
	return m_coin_lockout;
}

WRITE16_MEMBER(segajw_state::coinlockout_w)
{
	coin_lockout_w(machine(), 0, data & 1);
	m_coin_lockout = data;
}


READ16_MEMBER(segajw_state::soundboard_r)
{
	// TODO: to replace with proper sound emulation
	return 0xfff0;  // value expected for pass the sound board test
}

WRITE8_MEMBER(segajw_state::ramdac_io_w)
{
	// copied from adp.c
	switch(offset)
	{
		case 0:
			m_pal.offs = data;
			m_pal.offs_internal = 0;
			break;
		case 2:
			//mask pen reg
			break;
		case 1:
			switch(m_pal.offs_internal)
			{
				case 0:
					m_pal.r = ((data & 0x3f) << 2) | ((data & 0x30) >> 4);
					m_pal.offs_internal++;
					break;
				case 1:
					m_pal.g = ((data & 0x3f) << 2) | ((data & 0x30) >> 4);
					m_pal.offs_internal++;
					break;
				case 2:
					m_pal.b = ((data & 0x3f) << 2) | ((data & 0x30) >> 4);
					m_palette->set_pen_color(m_pal.offs, rgb_t(m_pal.r, m_pal.g, m_pal.b));
					m_pal.offs_internal = 0;
					m_pal.offs++;
					m_pal.offs&=0xff;
					break;
			}

			break;
	}
}

INPUT_CHANGED_MEMBER( segajw_state::coin_drop_start )
{
	if (newval && !m_coin_start_cycles)
		m_coin_start_cycles = m_maincpu->total_cycles();
}

CUSTOM_INPUT_MEMBER( segajw_state::hopper_sensors_r )
{
	UINT8 data = 0;

	// if the hopper is active simulate the coin-out sensor
	if (m_hopper_start_cycles)
	{
		attotime diff = m_maincpu->cycles_to_attotime(m_maincpu->total_cycles() - m_hopper_start_cycles);

		if (diff > attotime::from_msec(100))
			data |= 0x01;

		if (diff > attotime::from_msec(200))
			m_hopper_start_cycles = m_maincpu->total_cycles();
	}

	return data;
}

CUSTOM_INPUT_MEMBER( segajw_state::coin_sensors_r )
{
	UINT8 data = 0;

	// simulates the passage of coins through multiple sensors
	if (m_coin_start_cycles)
	{
		attotime diff = m_maincpu->cycles_to_attotime(m_maincpu->total_cycles() - m_coin_start_cycles);

		if (diff > attotime::from_msec(20) && diff < attotime::from_msec(100))
			data |= 0x01;
		if (diff > attotime::from_msec(80) && diff < attotime::from_msec(200))
			data |= 0x02;
		if (diff <= attotime::from_msec(100))
			data |= 0x04;

		if (diff > attotime::from_msec(200))
			m_coin_start_cycles = 0;
	}

	return data;
}

static ADDRESS_MAP_START( segajw_map, AS_PROGRAM, 16, segajw_state )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM

	AM_RANGE(0x080000, 0x080001) AM_DEVREADWRITE("hd63484", h63484_device, status_r, address_w)
	AM_RANGE(0x080002, 0x080003) AM_DEVREADWRITE("hd63484", h63484_device, data_r, data_w)

	AM_RANGE(0x180000, 0x180001) AM_READ_PORT("DSW0")
	AM_RANGE(0x180004, 0x180005) AM_READ(soundboard_r)
	AM_RANGE(0x180008, 0x180009) AM_READ_PORT("DSW1")
	AM_RANGE(0x18000a, 0x18000b) AM_READ_PORT("DSW3")
	AM_RANGE(0x18000c, 0x18000d) AM_READ_PORT("DSW2")

	AM_RANGE(0x1a0000, 0x1a0001) AM_WRITE(coin_counter_w)
	AM_RANGE(0x1a0006, 0x1a0007) AM_READWRITE(hopper_r, hopper_w)
	AM_RANGE(0x1a000a, 0x1a000b) AM_READ(coin_counter_r)

	AM_RANGE(0x1a000e, 0x1a000f) AM_NOP

	AM_RANGE(0x1c0000, 0x1c0001) AM_READ_PORT("IN0")
	AM_RANGE(0x1c0002, 0x1c0003) AM_READ_PORT("IN1")
	AM_RANGE(0x1c0004, 0x1c0005) AM_READ_PORT("IN2")
	AM_RANGE(0x1c0006, 0x1c0007) AM_READ_PORT("IN3")
	AM_RANGE(0x1c000c, 0x1c000d) AM_READWRITE(coinlockout_r, coinlockout_w)

	AM_RANGE(0x280000, 0x280007) AM_WRITE8(ramdac_io_w, 0x00ff)

	AM_RANGE(0xff0000, 0xffffff) AM_RAM AM_SHARE("nvram")
ADDRESS_MAP_END

static ADDRESS_MAP_START( segajw_audiocpu_map, AS_PROGRAM, 8, segajw_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xe000, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( segajw_audiocpu_io_map, AS_IO, 8, segajw_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
ADDRESS_MAP_END

static ADDRESS_MAP_START( segajw_hd63484_map, AS_0, 16, segajw_state )
	AM_RANGE(0x00000, 0x3ffff) AM_RAM
	AM_RANGE(0x80000, 0xbffff) AM_ROM AM_REGION("gfx1", 0)
ADDRESS_MAP_END


static INPUT_PORTS_START( segajw )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_POKER_HOLD1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_POKER_HOLD2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_POKER_HOLD3 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_POKER_HOLD4 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_POKER_HOLD5 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_GAMBLE_BET )   PORT_NAME("1 Bet")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 )      PORT_NAME("Max Bet")
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON1 )      PORT_NAME("Deal / Draw")

	PORT_START("IN1")
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 )      PORT_NAME("Double")
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON4 )      PORT_NAME("Change")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_OTHER )        PORT_NAME("Reset")     PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x000d, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_GAMBLE_SERVICE ) PORT_NAME("Meter")
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_OTHER )          PORT_NAME("Last Game")   PORT_CODE(KEYCODE_T)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_OTHER )          PORT_NAME("M-Door")
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_OTHER )          PORT_NAME("D-Door")
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_SPECIAL )       PORT_CUSTOM_MEMBER(DEVICE_SELF, segajw_state, hopper_sensors_r, NULL)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_OTHER )          PORT_NAME("Hopper Full")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_OTHER )          PORT_NAME("Hopper Fill")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN3")
	PORT_BIT( 0x0007, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, segajw_state, coin_sensors_r, NULL)
	PORT_BIT( 0x00f8, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("COIN1") // start the coin drop sequence (see coin_sensors_r)
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )   PORT_CHANGED_MEMBER(DEVICE_SELF, segajw_state, coin_drop_start, NULL)

	PORT_START("DSW0")
	PORT_DIPNAME( 0x0001, 0x0000, "DSW0-1" )
	PORT_DIPSETTING(    0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0000, "DSW0-2" )
	PORT_DIPSETTING(    0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0000, "DSW0-3" )
	PORT_DIPSETTING(    0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0000, "DSW0-4" )
	PORT_DIPSETTING(    0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0000, "DSW0-5" )
	PORT_DIPSETTING(    0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0000, "DSW0-6" )
	PORT_DIPSETTING(    0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0000, "DSW0-7" )
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, "DSW0-8" )
	PORT_DIPSETTING(    0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0001, 0x0000, "DSW1-1" )
	PORT_DIPSETTING(    0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0000, "DSW1-2" )
	PORT_DIPSETTING(    0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0000, "DSW1-3" )
	PORT_DIPSETTING(    0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0000, "DSW1-4" )
	PORT_DIPSETTING(    0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0000, "DSW1-5" )
	PORT_DIPSETTING(    0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0000, "DSW1-6" )
	PORT_DIPSETTING(    0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0000, "DSW1-7" )
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, "DSW1-8" )
	PORT_DIPSETTING(    0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x0001, 0x0000, "DSW2-1" )
	PORT_DIPSETTING(    0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0000, "DSW2-2" )
	PORT_DIPSETTING(    0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0000, "DSW2-3" )
	PORT_DIPSETTING(    0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0000, "DSW2-4" )
	PORT_DIPSETTING(    0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0000, "DSW2-5" )
	PORT_DIPSETTING(    0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0000, "DSW2-6" )
	PORT_DIPSETTING(    0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0000, "DSW2-7" )
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, "DSW2-8" )
	PORT_DIPSETTING(    0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x0001, 0x0001, "DSW3-1" )
	PORT_DIPSETTING(    0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, "DSW3-2" )
	PORT_DIPSETTING(    0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, "DSW3-3" )
	PORT_DIPSETTING(    0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0000, "DSW3-4" )
	PORT_DIPSETTING(    0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0000, "DSW3-5" )
	PORT_DIPSETTING(    0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0000, "DSW3-6" )
	PORT_DIPSETTING(    0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0000, "DSW3-7" )
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, "DSW3-8" )
	PORT_DIPSETTING(    0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
INPUT_PORTS_END


void segajw_state::machine_start()
{
}


void segajw_state::machine_reset()
{
	m_coin_start_cycles = 0;
	m_hopper_start_cycles = 0;
	m_coin_counter = 0xff;
	m_coin_lockout = 0;
	m_hopper_ctrl = 0;
}

static MACHINE_CONFIG_START( segajw, segajw_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",M68000,8000000) // unknown clock
	MCFG_CPU_PROGRAM_MAP(segajw_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", segajw_state, irq4_line_hold)
	MCFG_CPU_PERIODIC_INT_DRIVER(segajw_state, irq5_line_hold, 300)    // FIXME: unknown source, but vblank is too slow

	MCFG_CPU_ADD("audiocpu", Z80, 4000000) // unknown clock
	MCFG_CPU_PROGRAM_MAP(segajw_audiocpu_map)
	MCFG_CPU_IO_MAP(segajw_audiocpu_io_map)
	MCFG_DEVICE_DISABLE()

	MCFG_NVRAM_ADD_NO_FILL("nvram")

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_UPDATE_DEVICE("hd63484", h63484_device, update_screen)
	MCFG_SCREEN_SIZE(720, 480)
	MCFG_SCREEN_VISIBLE_AREA(0, 720-1, 0, 448-1)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 16)

	MCFG_H63484_ADD("hd63484", 8000000, segajw_hd63484_map) // unknown clock

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
//  MCFG_SOUND_ADD("aysnd", AY8910, MAIN_CLOCK/4) /* guess */
//  MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( segajw )
	ROM_REGION( 0x40000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "14584b.epr",   0x00001, 0x20000, CRC(d3a6d63d) SHA1(ce9d4769b7514294a91af1dfd7cd10ee40b3572c) )
	ROM_LOAD16_BYTE( "14585b.epr",   0x00000, 0x20000, CRC(556d0a62) SHA1(d2def433a511cbdebbe2cd0c8e51fc8c4ff1ed7b) )

	ROM_REGION( 0x20000, "audiocpu", 0 )
	ROM_LOAD( "14587a.epr",   0x00000, 0x20000, CRC(66163b6c) SHA1(88e994bcad86c58dc730a93b48226e9296df7667) )

	ROM_REGION16_BE( 0x80000, "gfx1", 0 )
	ROM_LOAD16_WORD_SWAP( "14586.epr",   0x00000, 0x80000, CRC(daeb0616) SHA1(17a8bb7137ad46a7c3ac07d22cbc4430e76e2f71) )
ROM_END


GAME( 198?, segajw,  0,   segajw,  segajw, driver_device,  0, ROT0, "Sega", "Golden Poker Series \"Joker's Wild\" (Rev. B)", GAME_NOT_WORKING | GAME_NO_SOUND | GAME_IMPERFECT_GRAPHICS ) // TODO: correct title
