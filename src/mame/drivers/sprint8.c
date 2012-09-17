/***************************************************************************

Atari Sprint 8 driver

***************************************************************************/

#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "includes/sprint8.h"




void sprint8_set_collision(running_machine &machine, int n)
{
	sprint8_state *state = machine.driver_data<sprint8_state>();
	if (state->m_collision_reset == 0)
	{
		machine.device("maincpu")->execute().set_input_line(0, ASSERT_LINE);

		state->m_collision_index = n;
	}
}


static TIMER_DEVICE_CALLBACK( input_callback )
{
	sprint8_state *state = timer.machine().driver_data<sprint8_state>();
	static const char *const dialnames[] = { "DIAL1", "DIAL2", "DIAL3", "DIAL4", "DIAL5", "DIAL6", "DIAL7", "DIAL8" };

	int i;

	for (i = 0; i < 8; i++)
	{
		UINT8 val = timer.machine().root_device().ioport(dialnames[i])->read() >> 4;

		signed char delta = (val - state->m_dial[i]) & 15;

		if (delta & 8)
			delta |= 0xf0; /* extend sign to 8 bits */

		state->m_steer_flag[i] = (delta != 0);

		if (delta > 0)
			state->m_steer_dir[i] = 0;

		if (delta < 0)
			state->m_steer_dir[i] = 1;

		state->m_dial[i] = val;
	}
}


void sprint8_state::machine_reset()
{
	m_collision_reset = 0;
	m_collision_index = 0;
}


READ8_MEMBER(sprint8_state::sprint8_collision_r)
{
	return m_collision_index;
}


READ8_MEMBER(sprint8_state::sprint8_input_r)
{
	static const char *const portnames[] = { "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8" };
	UINT8 val = ioport(portnames[offset])->read();

	if (m_steer_dir[offset])
	{
		val |= 0x02;
	}
	if (m_steer_flag[offset])
	{
		val |= 0x04;
	}

	return val;
}


WRITE8_MEMBER(sprint8_state::sprint8_lockout_w)
{
	coin_lockout_w(machine(), offset, !(data & 1));
}


WRITE8_MEMBER(sprint8_state::sprint8_int_reset_w)
{
	m_collision_reset = !(data & 1);

	if (m_collision_reset)
		machine().device("maincpu")->execute().set_input_line(0, CLEAR_LINE);
}


static ADDRESS_MAP_START( sprint8_map, AS_PROGRAM, 8, sprint8_state )
	AM_RANGE(0x0000, 0x00ff) AM_RAM
	AM_RANGE(0x1800, 0x1bff) AM_RAM_WRITE(sprint8_video_ram_w) AM_SHARE("video_ram")
	AM_RANGE(0x1c00, 0x1c00) AM_READ(sprint8_collision_r)
	AM_RANGE(0x1c01, 0x1c08) AM_READ(sprint8_input_r)
	AM_RANGE(0x1c09, 0x1c09) AM_READ_PORT("IN0")
	AM_RANGE(0x1c0a, 0x1c0a) AM_READ_PORT("IN1")
	AM_RANGE(0x1c0f, 0x1c0f) AM_READ_PORT("VBLANK")
	AM_RANGE(0x1c00, 0x1c0f) AM_WRITEONLY AM_SHARE("pos_h_ram")
	AM_RANGE(0x1c10, 0x1c1f) AM_WRITEONLY AM_SHARE("pos_v_ram")
	AM_RANGE(0x1c20, 0x1c2f) AM_WRITEONLY AM_SHARE("pos_d_ram")
	AM_RANGE(0x1c30, 0x1c37) AM_WRITE(sprint8_lockout_w)
	AM_RANGE(0x1d00, 0x1d00) AM_WRITE(sprint8_int_reset_w)
	AM_RANGE(0x1d01, 0x1d01) AM_DEVWRITE_LEGACY("discrete", sprint8_crash_w)
	AM_RANGE(0x1d02, 0x1d02) AM_DEVWRITE_LEGACY("discrete", sprint8_screech_w)
	AM_RANGE(0x1d03, 0x1d03) AM_WRITENOP
	AM_RANGE(0x1d04, 0x1d04) AM_WRITENOP
	AM_RANGE(0x1d05, 0x1d05) AM_WRITEONLY AM_SHARE("team")
	AM_RANGE(0x1d06, 0x1d06) AM_DEVWRITE_LEGACY("discrete", sprint8_attract_w)
	AM_RANGE(0x1e00, 0x1e07) AM_DEVWRITE_LEGACY("discrete", sprint8_motor_w)
	AM_RANGE(0x1f00, 0x1f00) AM_WRITENOP /* probably a watchdog, disabled in service mode */
	AM_RANGE(0x2000, 0x3fff) AM_ROM
	AM_RANGE(0xf800, 0xffff) AM_ROM
ADDRESS_MAP_END


static INPUT_PORTS_START( sprint8 )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P1 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P1 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P2 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P2 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P3 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P3 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN4 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P4 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P4 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN5 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P5 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P5 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(5)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(5)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START("P6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN6 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P6 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P6 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(6)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(6)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN7 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P7 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P7 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(7)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(7)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN8 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P8 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P8 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(8)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(8)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("DIAL1")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("DIAL2")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START("DIAL3")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(3)

	PORT_START("DIAL4")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(4)

	PORT_START("DIAL5")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(5)

	PORT_START("DIAL6")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(6)

	PORT_START("DIAL7")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(7)

	PORT_START("DIAL8")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(8)

	PORT_START("IN0")
	PORT_DIPNAME( 0x0f, 0x08, "Play Time" )
	PORT_DIPSETTING(    0x0f, "60 seconds" )
	PORT_DIPSETTING(    0x0e, "69 seconds" )
	PORT_DIPSETTING(    0x0d, "77 seconds" )
	PORT_DIPSETTING(    0x0c, "86 seconds" )
	PORT_DIPSETTING(    0x0b, "95 seconds" )
	PORT_DIPSETTING(    0x0a, "103 seconds" )
	PORT_DIPSETTING(    0x09, "112 seconds" )
	PORT_DIPSETTING(    0x08, "120 seconds" )
	PORT_DIPSETTING(    0x07, "129 seconds" )
	PORT_DIPSETTING(    0x06, "138 seconds" )
	PORT_DIPSETTING(    0x05, "146 seconds" )
	PORT_DIPSETTING(    0x04, "155 seconds" )
	PORT_DIPSETTING(    0x03, "163 seconds" )
	PORT_DIPSETTING(    0x02, "172 seconds" )
	PORT_DIPSETTING(    0x01, "181 seconds" )
	PORT_DIPSETTING(    0x00, "189 seconds" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Track Select") PORT_CODE(KEYCODE_SPACE)

	PORT_START("VBLANK")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_VBLANK("screen")

	/* this is actually a variable resistor */
	PORT_START("R132")
	PORT_ADJUSTER(65, "R132 - Crash & Screech Volume")

INPUT_PORTS_END


static INPUT_PORTS_START( sprint8p )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P1 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P1 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P2 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P2 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P3 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P3 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN4 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P4 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P4 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN5 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P5 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P5 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(5)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(5)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START("P6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN6 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P6 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P6 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(6)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(6)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN7 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P7 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P7 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(7)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(7)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("P8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW,  IPT_COIN8 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER DIR P8 */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* STEER FLAG P8 */
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_BUTTON1 ) PORT_PLAYER(8)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_BUTTON2 ) PORT_PLAYER(8)
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START("DIAL1")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("DIAL2")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START("DIAL3")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(3)

	PORT_START("DIAL4")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(4)

	PORT_START("DIAL5")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(5)

	PORT_START("DIAL6")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(6)

	PORT_START("DIAL7")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(7)

	PORT_START("DIAL8")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(8)

	PORT_START("IN0")
	PORT_DIPNAME( 0x03, 0x03, "Play Time" )
	PORT_DIPSETTING(    0x00, "54 seconds" )
	PORT_DIPSETTING(    0x01, "108 seconds" )
	PORT_DIPSETTING(    0x03, "216 seconds" )
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, "Play Mode" )
	PORT_DIPSETTING(    0x00, "Chase" )
	PORT_DIPSETTING(    0x01, "Tag" )

	PORT_START("VBLANK")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_VBLANK("screen")

	/* this is actually a variable resistor */
	PORT_START("R132")
	PORT_ADJUSTER(65, "R132 - Crash & Screech Volume")

INPUT_PORTS_END


static const gfx_layout tile_layout_1 =
{
	16, 8,
	64,
	1,
	{ 0 },
	{
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0
	},
	{
		0x000, 0x200, 0x400, 0x600, 0x800, 0xa00, 0xc00, 0xe00
	},
	8
};


static const gfx_layout tile_layout_2 =
{
	16, 8,
	64,
	1,
	{ 0 },
	{
		0x000, 0x000, 0x200, 0x200, 0x400, 0x400, 0x600, 0x600,
		0x800, 0x800, 0xa00, 0xa00, 0xc00, 0xc00, 0xe00, 0xe00

	},
	{
		0, 1, 2, 3, 4, 5, 6, 7
	},
	8
};


static const gfx_layout car_layout =
{
	16, 8,
	8,
	1,
	{ 0 },
	{
		0x07, 0x06, 0x05, 0x04, 0x0f, 0x0e, 0x0d, 0x0c,
		0x17, 0x16, 0x15, 0x14, 0x1f, 0x1e, 0x1d, 0x1c
	},
	{
		0x000, 0x020, 0x040, 0x060, 0x080, 0x0a0, 0x0c0, 0x0e0
	},
	0x100
};


static GFXDECODE_START( sprint8 )
	GFXDECODE_ENTRY( "gfx1", 0, tile_layout_1, 0, 18 )
	GFXDECODE_ENTRY( "gfx1", 0, tile_layout_2, 0, 18 )
	GFXDECODE_ENTRY( "gfx2", 0, car_layout, 0, 16 )
GFXDECODE_END


static MACHINE_CONFIG_START( sprint8, sprint8_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, 11055000 / 11) /* ? */
	MCFG_CPU_PROGRAM_MAP(sprint8_map)


	MCFG_TIMER_ADD_PERIODIC("input_timer", input_callback, attotime::from_hz(60))

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_SIZE(512, 261)
	MCFG_SCREEN_VISIBLE_AREA(0, 495, 0, 231)
	MCFG_SCREEN_UPDATE_DRIVER(sprint8_state, screen_update_sprint8)
	MCFG_SCREEN_VBLANK_DRIVER(sprint8_state, screen_eof_sprint8)

	MCFG_GFXDECODE(sprint8)
	MCFG_PALETTE_LENGTH(36)



	/* sound hardware */
	/* the proper way is to hook up 4 speakers, but they are not really
     * F/R/L/R speakers.  Though you can pretend the 1-2 mix is the front. */
	MCFG_SPEAKER_ADD("speaker_1_2", 0.0, 0.0, 1.0)		/* front */
	MCFG_SPEAKER_ADD("speaker_3_7", -0.2, 0.0, 1.0)		/* left */
	MCFG_SPEAKER_ADD("speaker_5_6",  0.0, 0.0, -0.5)	/* back */
	MCFG_SPEAKER_ADD("speaker_4_8", 0.2, 0.0, 1.0)		/* right */

	MCFG_SOUND_ADD("discrete", DISCRETE, 0)
	MCFG_SOUND_CONFIG_DISCRETE(sprint8)
	MCFG_SOUND_ROUTE(0, "speaker_1_2", 1.0)
	/* volumes on other channels defaulted to off, */
	/* user can turn them up if needed. */
	/* The game does not sound good with all channels mixed to stereo. */
	MCFG_SOUND_ROUTE(1, "speaker_3_7", 0.0)
	MCFG_SOUND_ROUTE(2, "speaker_5_6", 0.0)
	MCFG_SOUND_ROUTE(3, "speaker_4_8", 0.0)
MACHINE_CONFIG_END


ROM_START( sprint8 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "7313.j1", 0x2400, 0x0800, CRC(1231f944) SHA1(d16c76da6a74513eb40811d806e0dd009f6dcafb) )
	ROM_LOAD( "7314.h1", 0x2c00, 0x0800, CRC(c77c0d49) SHA1(a57b5d340a41d02edb20fcb66875908110582bc5) )
	ROM_RELOAD(          0xf800, 0x0800 )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "7315-01.n6", 0x0000, 0x0200, CRC(e2f603d0) SHA1(8d82b72d2f4039afa3341774000105a745caf85f) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* cars */
	ROM_LOAD( "7316-01.j5", 0x0000, 0x0100, CRC(32c028e3) SHA1(bfa76cf0981640d08e9c7fb15da134afe46afe31) )
ROM_END


ROM_START( sprint8a )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD_NIB_LOW ( "l2800s8", 0x2800, 0x0800, CRC(3926de69) SHA1(ec03d7684e393061d1d48ae73897e9dc38131c14) )
	ROM_LOAD_NIB_HIGH( "m2800s8", 0x2800, 0x0800, CRC(d009d6da) SHA1(3210806b0eb344d88d2cbcc46895f7224771c1f2) )
	ROM_LOAD_NIB_LOW ( "l3000s8", 0x3000, 0x0800, CRC(c78d9888) SHA1(a854b50b2cf0261c1f966ef1bd001084500b3545) )
	ROM_LOAD_NIB_HIGH( "m3000s8", 0x3000, 0x0800, CRC(9ebfe8f8) SHA1(9709f697a7f9cce7ff4edbdccbaf14931328a052) )
	ROM_LOAD_NIB_LOW ( "l3800s8", 0x3800, 0x0800, CRC(74a8f103) SHA1(0cc15006cbd4579feac0f07690f32b2b61f97ae9) )
	ROM_RELOAD(                   0xf800, 0x0800 )
	ROM_LOAD_NIB_HIGH( "m3800s8", 0x3800, 0x0800, CRC(90aadc75) SHA1(34ca21c37573d9a2df92d3a1e73fdc0a9885c0a0) )
	ROM_RELOAD(                   0xf800, 0x0800 )

	ROM_REGION( 0x0200, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "s8.n6", 0x0000, 0x0200, CRC(92cf9a7e) SHA1(6bd2d396e0a299c2e731425cabd578d569c2061b) )

	ROM_REGION( 0x0100, "gfx2", 0 ) /* cars */
	ROM_LOAD( "s8.j5", 0x0000, 0x0100, CRC(d37fff36) SHA1(20a7a8caf2fbfe22e307fe8541d31784c8e39d1a) )
ROM_END


GAME( 1977, sprint8,  0,       sprint8, sprint8, driver_device,  0, ROT0, "Atari", "Sprint 8",  0 )
GAME( 1977, sprint8a, sprint8, sprint8, sprint8p, driver_device, 0, ROT0, "Atari", "Sprint 8 (play tag & chase)", 0 )
