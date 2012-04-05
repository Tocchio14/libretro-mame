/***************************************************************************

    Atari Blasteroids hardware

    driver by Aaron Giles

    Games supported:
        * Blasteroids (1987) [5 sets]

    Known bugs:
        * none at this time

****************************************************************************

    Memory map (TBA)

***************************************************************************/


#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "machine/atarigen.h"
#include "audio/atarijsa.h"
#include "video/atarimo.h"
#include "includes/blstroid.h"



/*************************************
 *
 *  Initialization & interrupts
 *
 *************************************/

static void update_interrupts(running_machine &machine)
{
	blstroid_state *state = machine.driver_data<blstroid_state>();
	cputag_set_input_line(machine, "maincpu", 1, state->m_scanline_int_state ? ASSERT_LINE : CLEAR_LINE);
	cputag_set_input_line(machine, "maincpu", 2, state->m_video_int_state ? ASSERT_LINE : CLEAR_LINE);
	cputag_set_input_line(machine, "maincpu", 4, state->m_sound_int_state ? ASSERT_LINE : CLEAR_LINE);
}


static WRITE16_HANDLER( blstroid_halt_until_hblank_0_w )
{
	atarigen_halt_until_hblank_0(*space->machine().primary_screen);
}


static MACHINE_START( blstroid )
{
	atarigen_init(machine);
}


static MACHINE_RESET( blstroid )
{
	blstroid_state *state = machine.driver_data<blstroid_state>();

	atarigen_eeprom_reset(state);
	atarigen_interrupt_reset(state, update_interrupts);
	atarigen_scanline_timer_reset(*machine.primary_screen, blstroid_scanline_update, 8);
	atarijsa_reset();
}



/*************************************
 *
 *  I/O read dispatch
 *
 *************************************/

static READ16_HANDLER( inputs_r )
{
	static const char *const iptnames[] = { "IN0", "IN1" };
	blstroid_state *state = space->machine().driver_data<blstroid_state>();
	int temp = input_port_read(space->machine(), iptnames[offset & 1]);

	if (state->m_cpu_to_sound_ready) temp ^= 0x0040;
	if (atarigen_get_hblank(*space->machine().primary_screen)) temp ^= 0x0010;
	return temp;
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

/* full map verified from schematics */
static ADDRESS_MAP_START( main_map, AS_PROGRAM, 16, blstroid_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x000000, 0x03ffff) AM_MIRROR(0x7c0000) AM_ROM
	AM_RANGE(0xff8000, 0xff8001) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(watchdog_reset16_w)
	AM_RANGE(0xff8200, 0xff8201) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(atarigen_scanline_int_ack_w)
	AM_RANGE(0xff8400, 0xff8401) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(atarigen_video_int_ack_w)
	AM_RANGE(0xff8600, 0xff8601) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(atarigen_eeprom_enable_w)
	AM_RANGE(0xff8800, 0xff89ff) AM_MIRROR(0x7f8000) AM_WRITEONLY AM_BASE(m_priorityram)
	AM_RANGE(0xff8a00, 0xff8a01) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(atarigen_sound_w)
	AM_RANGE(0xff8c00, 0xff8c01) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(atarigen_sound_reset_w)
	AM_RANGE(0xff8e00, 0xff8e01) AM_MIRROR(0x7f81fe) AM_WRITE_LEGACY(blstroid_halt_until_hblank_0_w)
	AM_RANGE(0xff9400, 0xff9401) AM_MIRROR(0x7f83fe) AM_READ_LEGACY(atarigen_sound_r)
	AM_RANGE(0xff9800, 0xff9801) AM_MIRROR(0x7f83f8) AM_READ_PORT("DIAL0")
	AM_RANGE(0xff9804, 0xff9805) AM_MIRROR(0x7f83f8) AM_READ_PORT("DIAL1")
	AM_RANGE(0xff9c00, 0xff9c03) AM_MIRROR(0x7f83fc) AM_READ_LEGACY(inputs_r)
	AM_RANGE(0xffa000, 0xffa3ff) AM_MIRROR(0x7f8c00) AM_RAM_WRITE(paletteram16_xRRRRRGGGGGBBBBB_word_w) AM_SHARE("paletteram")
	AM_RANGE(0xffb000, 0xffb3ff) AM_MIRROR(0x7f8c00) AM_READWRITE_LEGACY(atarigen_eeprom_r, atarigen_eeprom_w) AM_SHARE("eeprom")
	AM_RANGE(0xffc000, 0xffcfff) AM_MIRROR(0x7f8000) AM_RAM_WRITE_LEGACY(atarigen_playfield_w) AM_BASE(m_playfield)
	AM_RANGE(0xffd000, 0xffdfff) AM_MIRROR(0x7f8000) AM_READWRITE_LEGACY(atarimo_0_spriteram_r, atarimo_0_spriteram_w)
	AM_RANGE(0xffe000, 0xffffff) AM_MIRROR(0x7f8000) AM_RAM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( blstroid )
	PORT_START("DIAL0")		/* ff9800 */
	PORT_BIT( 0x00ff, 0, IPT_DIAL ) PORT_SENSITIVITY(60) PORT_KEYDELTA(10) PORT_PLAYER(1)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DIAL1")		/* ff9804 */
	PORT_BIT( 0x00ff, 0, IPT_DIAL ) PORT_SENSITIVITY(60) PORT_KEYDELTA(10) PORT_PLAYER(2)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN0")		/* ff9c00 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_SPECIAL )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x0080, IP_ACTIVE_LOW )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")		/* ff9c02 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_SPECIAL )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_INCLUDE( atarijsa_i )		/* audio port */
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout pflayout =
{
	16,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0,0, 4,4, 8,8, 12,12, 16,16, 20,20, 24,24, 28,28 },
	{ 0*8, 4*8, 8*8, 12*8, 16*8, 20*8, 24*8, 28*8 },
	32*8
};


static const gfx_layout molayout =
{
	16,8,
	RGN_FRAC(1,2),
	4,
	{ 0, 1, 2, 3 },
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0, 4, RGN_FRAC(1,2)+8, RGN_FRAC(1,2)+12, 8, 12,
			RGN_FRAC(1,2)+16, RGN_FRAC(1,2)+20, 16, 20, RGN_FRAC(1,2)+24, RGN_FRAC(1,2)+28, 24, 28 },
	{ 0*8, 4*8, 8*8, 12*8, 16*8, 20*8, 24*8, 28*8 },
	32*8
};


static GFXDECODE_START( blstroid )
	GFXDECODE_ENTRY( "gfx1", 0, pflayout,  256, 16 )
	GFXDECODE_ENTRY( "gfx2", 0, molayout,    0, 16 )
GFXDECODE_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_CONFIG_START( blstroid, blstroid_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, ATARI_CLOCK_14MHz/2)
	MCFG_CPU_PROGRAM_MAP(main_map)
	MCFG_CPU_VBLANK_INT("screen", atarigen_video_int_gen)

	MCFG_MACHINE_START(blstroid)
	MCFG_MACHINE_RESET(blstroid)
	MCFG_NVRAM_ADD_1FILL("eeprom")

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_UPDATE_BEFORE_VBLANK)
	MCFG_GFXDECODE(blstroid)
	MCFG_PALETTE_LENGTH(512)

	MCFG_SCREEN_ADD("screen", RASTER)
	/* note: these parameters are from published specs, not derived */
	/* the board uses an SOS-2 chip to generate video signals */
	MCFG_SCREEN_RAW_PARAMS(ATARI_CLOCK_14MHz, 456*2, 0, 320*2, 262, 0, 240)
	MCFG_SCREEN_UPDATE_STATIC(blstroid)

	MCFG_VIDEO_START(blstroid)

	/* sound hardware */
	MCFG_FRAGMENT_ADD(jsa_i_stereo)
MACHINE_CONFIG_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( blstroid )
	ROM_REGION( 0x40000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136057-4123.6c",  0x000000, 0x010000, CRC(d14badc4) SHA1(ccba30e1eb6b3351cbc7ea18951debb7f7aa4520) )
	ROM_LOAD16_BYTE( "136057-4121.6b",  0x000001, 0x010000, CRC(ae3e93e8) SHA1(66ccff68e9b0f7e97abf126f977775e29ce4eee5) )
	ROM_LOAD16_BYTE( "136057-4124.4c",  0x020000, 0x010000, CRC(fd2365df) SHA1(63ed3f9a92fed985f9ddb93687f11a24c8309f56) )
	ROM_LOAD16_BYTE( "136057-4122.4b",  0x020001, 0x010000, CRC(c364706e) SHA1(e03cd60d139000607d83240b0b48865eafb1188b) )

	ROM_REGION( 0x14000, "jsa", 0 )	/* 64k for 6502 code */
	ROM_LOAD( "136057-1135.2k",  0x010000, 0x004000, CRC(baa8b5fe) SHA1(4af1f9bec3ffa856016a89bc20041d572305ba3a) )
	ROM_CONTINUE(                0x004000, 0x00c000 )

	ROM_REGION( 0x040000, "gfx1", 0 )
	ROM_LOAD( "136057-1101.1l",  0x000000, 0x010000, CRC(3c2daa5b) SHA1(2710a05e95afd8452104c4f4a9250a3b7d728a42) )
	ROM_LOAD( "136057-1102.1m",  0x010000, 0x010000, CRC(f84f0b97) SHA1(00cb5f1e0f92742683ee71854085b1e4db4bd6bb) )
	ROM_LOAD( "136057-1103.3l",  0x020000, 0x010000, CRC(ae5274f0) SHA1(87070e6e51d557c1b10ef32ac0ed670856d5aaf1) )
	ROM_LOAD( "136057-1104.3m",  0x030000, 0x010000, CRC(4bb72060) SHA1(94cd1a6900f47a5178cec041fa6dc9cfee1f9c3f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "136057-1105.5m",  0x000000, 0x010000, CRC(50e0823f) SHA1(f638becad83307ed43d138d452199e4c6725512f) )
	ROM_LOAD( "136057-1107.67m", 0x010000, 0x010000, CRC(729de7a9) SHA1(526b08e6d54cd0b991c4207c23119d2940a34009) )
	ROM_LOAD( "136057-1109.8m",  0x020000, 0x010000, CRC(090e42ab) SHA1(903aa99e6e39407319f6e90102b24604884ee047) )
	ROM_LOAD( "136057-1111.10m", 0x030000, 0x010000, CRC(1ff79e67) SHA1(12d408184f814bab411f567e8b29914a289e3fb8) )
	ROM_LOAD( "136057-1113.11m", 0x040000, 0x010000, CRC(4be1d504) SHA1(f41ff2d31e2e0e5b6d89fbbf014ba767c7b9f299) )
	ROM_LOAD( "136057-1115.13m", 0x050000, 0x010000, CRC(e4409310) SHA1(09180f1ab2ac8465b6641e94271c72bf566b2597) )
	ROM_LOAD( "136057-1117.14m", 0x060000, 0x010000, CRC(7aaca15e) SHA1(4014d60f2b6590c96796dbb2a538f1976194f3e7) )
	ROM_LOAD( "136057-1119.16m", 0x070000, 0x010000, CRC(33690379) SHA1(09ddfd18ccab1c639837171a763a981c867af0b1) )
	ROM_LOAD( "136057-1106.5n",  0x080000, 0x010000, CRC(2720ee71) SHA1(ebfd58effebadab361dfb4bd77d626911da4409a) )
	ROM_LOAD( "136057-1108.67n", 0x090000, 0x010000, CRC(2faecd15) SHA1(7fe9535b9bc72fd5527dbd1079f559ac16f2a31e) )
	ROM_LOAD( "136057-1110.8n",  0x0a0000, 0x010000, CRC(a15e79e1) SHA1(3fc8c33f438fd304b566a62bbe0f6e17a696edbc) )
	ROM_LOAD( "136057-1112.10n", 0x0b0000, 0x010000, CRC(4d5fc284) SHA1(c66f95af700828225a62f46437ca83453900f7fc) )
	ROM_LOAD( "136057-1114.11n", 0x0c0000, 0x010000, CRC(a70fc6e6) SHA1(fbf469b8f5c6e69540743748ad994a6490ad7745) )
	ROM_LOAD( "136057-1116.13n", 0x0d0000, 0x010000, CRC(f423b4f8) SHA1(a431686233b104074728a81cf41604deea0fbb56) )
	ROM_LOAD( "136057-1118.14n", 0x0e0000, 0x010000, CRC(56fa3d16) SHA1(9d9c1fb7912774954224d8f0220047324122ab23) )
	ROM_LOAD( "136057-1120.16n", 0x0f0000, 0x010000, CRC(f257f738) SHA1(a5904ec25d2190f11708c2e1e41832fd66332428) )
ROM_END


ROM_START( blstroid3 )
	ROM_REGION( 0x40000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136057-3123.6c",  0x000000, 0x010000, CRC(8fb050f5) SHA1(4944ffb0843262afe41fc6b876ab6858dcefc95f) )
	ROM_LOAD16_BYTE( "136057-3121.6b",  0x000001, 0x010000, CRC(21fae262) SHA1(2516a75d76bcfdea5ab41a4898d47ed166bd1996) )
	ROM_LOAD16_BYTE( "136057-3124.4c",  0x020000, 0x010000, CRC(a9140c31) SHA1(02518bf998c0c74dff66f3192dcb1f91b1812cf8) )
	ROM_LOAD16_BYTE( "136057-3122.4b",  0x020001, 0x010000, CRC(137fbb17) SHA1(3dda03ecdb2dc9a9cd78aeaa502497662496a26d) )

	ROM_REGION( 0x14000, "jsa", 0 )	/* 64k for 6502 code */
	ROM_LOAD( "136057-1135.2k",  0x010000, 0x004000, CRC(baa8b5fe) SHA1(4af1f9bec3ffa856016a89bc20041d572305ba3a) )
	ROM_CONTINUE(                0x004000, 0x00c000 )

	ROM_REGION( 0x040000, "gfx1", 0 )
	ROM_LOAD( "136057-1101.1l",  0x000000, 0x010000, CRC(3c2daa5b) SHA1(2710a05e95afd8452104c4f4a9250a3b7d728a42) )
	ROM_LOAD( "136057-1102.1m",  0x010000, 0x010000, CRC(f84f0b97) SHA1(00cb5f1e0f92742683ee71854085b1e4db4bd6bb) )
	ROM_LOAD( "136057-1103.3l",  0x020000, 0x010000, CRC(ae5274f0) SHA1(87070e6e51d557c1b10ef32ac0ed670856d5aaf1) )
	ROM_LOAD( "136057-1104.3m",  0x030000, 0x010000, CRC(4bb72060) SHA1(94cd1a6900f47a5178cec041fa6dc9cfee1f9c3f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "136057-1105.5m",  0x000000, 0x010000, CRC(50e0823f) SHA1(f638becad83307ed43d138d452199e4c6725512f) )
	ROM_LOAD( "136057-1107.67m", 0x010000, 0x010000, CRC(729de7a9) SHA1(526b08e6d54cd0b991c4207c23119d2940a34009) )
	ROM_LOAD( "136057-1109.8m",  0x020000, 0x010000, CRC(090e42ab) SHA1(903aa99e6e39407319f6e90102b24604884ee047) )
	ROM_LOAD( "136057-1111.10m", 0x030000, 0x010000, CRC(1ff79e67) SHA1(12d408184f814bab411f567e8b29914a289e3fb8) )
	ROM_LOAD( "136057-1113.11m", 0x040000, 0x010000, CRC(4be1d504) SHA1(f41ff2d31e2e0e5b6d89fbbf014ba767c7b9f299) )
	ROM_LOAD( "136057-1115.13m", 0x050000, 0x010000, CRC(e4409310) SHA1(09180f1ab2ac8465b6641e94271c72bf566b2597) )
	ROM_LOAD( "136057-1117.14m", 0x060000, 0x010000, CRC(7aaca15e) SHA1(4014d60f2b6590c96796dbb2a538f1976194f3e7) )
	ROM_LOAD( "136057-1119.16m", 0x070000, 0x010000, CRC(33690379) SHA1(09ddfd18ccab1c639837171a763a981c867af0b1) )
	ROM_LOAD( "136057-1106.5n",  0x080000, 0x010000, CRC(2720ee71) SHA1(ebfd58effebadab361dfb4bd77d626911da4409a) )
	ROM_LOAD( "136057-1108.67n", 0x090000, 0x010000, CRC(2faecd15) SHA1(7fe9535b9bc72fd5527dbd1079f559ac16f2a31e) )
	ROM_LOAD( "136057-1110.8n",  0x0a0000, 0x010000, CRC(a15e79e1) SHA1(3fc8c33f438fd304b566a62bbe0f6e17a696edbc) )
	ROM_LOAD( "136057-1112.10n", 0x0b0000, 0x010000, CRC(4d5fc284) SHA1(c66f95af700828225a62f46437ca83453900f7fc) )
	ROM_LOAD( "136057-1114.11n", 0x0c0000, 0x010000, CRC(a70fc6e6) SHA1(fbf469b8f5c6e69540743748ad994a6490ad7745) )
	ROM_LOAD( "136057-1116.13n", 0x0d0000, 0x010000, CRC(f423b4f8) SHA1(a431686233b104074728a81cf41604deea0fbb56) )
	ROM_LOAD( "136057-1118.14n", 0x0e0000, 0x010000, CRC(56fa3d16) SHA1(9d9c1fb7912774954224d8f0220047324122ab23) )
	ROM_LOAD( "136057-1120.16n", 0x0f0000, 0x010000, CRC(f257f738) SHA1(a5904ec25d2190f11708c2e1e41832fd66332428) )
ROM_END


ROM_START( blstroid2 )
	ROM_REGION( 0x40000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136057-2123.6c",  0x000000, 0x010000, CRC(5a092513) SHA1(11396125842ea3a43d61b4ce266bb8053fdefd73) )
	ROM_LOAD16_BYTE( "136057-2121.6b",  0x000001, 0x010000, CRC(486aac51) SHA1(5e7fe7eb225d1c2701c21658ba2bad14ef7b64b1) )
	ROM_LOAD16_BYTE( "136057-2124.4c",  0x020000, 0x010000, CRC(d0fa38fe) SHA1(8aeae50dff6bcd14ac5faf10f15724b7f7430f5c) )
	ROM_LOAD16_BYTE( "136057-2122.4b",  0x020001, 0x010000, CRC(744bf921) SHA1(bb9118bfc04745df2eb78e1d1e70f7fc2e0509d4) )

	ROM_REGION( 0x14000, "jsa", 0 )	/* 64k for 6502 code */
	ROM_LOAD( "136057-1135.2k",  0x010000, 0x004000, CRC(baa8b5fe) SHA1(4af1f9bec3ffa856016a89bc20041d572305ba3a) )
	ROM_CONTINUE(                0x004000, 0x00c000 )

	ROM_REGION( 0x040000, "gfx1", 0 )
	ROM_LOAD( "136057-1101.1l",  0x000000, 0x010000, CRC(3c2daa5b) SHA1(2710a05e95afd8452104c4f4a9250a3b7d728a42) )
	ROM_LOAD( "136057-1102.1m",  0x010000, 0x010000, CRC(f84f0b97) SHA1(00cb5f1e0f92742683ee71854085b1e4db4bd6bb) )
	ROM_LOAD( "136057-1103.3l",  0x020000, 0x010000, CRC(ae5274f0) SHA1(87070e6e51d557c1b10ef32ac0ed670856d5aaf1) )
	ROM_LOAD( "136057-1104.3m",  0x030000, 0x010000, CRC(4bb72060) SHA1(94cd1a6900f47a5178cec041fa6dc9cfee1f9c3f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "136057-1105.5m",  0x000000, 0x010000, CRC(50e0823f) SHA1(f638becad83307ed43d138d452199e4c6725512f) )
	ROM_LOAD( "136057-1107.67m", 0x010000, 0x010000, CRC(729de7a9) SHA1(526b08e6d54cd0b991c4207c23119d2940a34009) )
	ROM_LOAD( "136057-1109.8m",  0x020000, 0x010000, CRC(090e42ab) SHA1(903aa99e6e39407319f6e90102b24604884ee047) )
	ROM_LOAD( "136057-1111.10m", 0x030000, 0x010000, CRC(1ff79e67) SHA1(12d408184f814bab411f567e8b29914a289e3fb8) )
	ROM_LOAD( "136057-1113.11m", 0x040000, 0x010000, CRC(4be1d504) SHA1(f41ff2d31e2e0e5b6d89fbbf014ba767c7b9f299) )
	ROM_LOAD( "136057-1115.13m", 0x050000, 0x010000, CRC(e4409310) SHA1(09180f1ab2ac8465b6641e94271c72bf566b2597) )
	ROM_LOAD( "136057-1117.14m", 0x060000, 0x010000, CRC(7aaca15e) SHA1(4014d60f2b6590c96796dbb2a538f1976194f3e7) )
	ROM_LOAD( "136057-1119.16m", 0x070000, 0x010000, CRC(33690379) SHA1(09ddfd18ccab1c639837171a763a981c867af0b1) )
	ROM_LOAD( "136057-1106.5n",  0x080000, 0x010000, CRC(2720ee71) SHA1(ebfd58effebadab361dfb4bd77d626911da4409a) )
	ROM_LOAD( "136057-1108.67n", 0x090000, 0x010000, CRC(2faecd15) SHA1(7fe9535b9bc72fd5527dbd1079f559ac16f2a31e) )
	ROM_LOAD( "136057-1110.8n",  0x0a0000, 0x010000, CRC(a15e79e1) SHA1(3fc8c33f438fd304b566a62bbe0f6e17a696edbc) )
	ROM_LOAD( "136057-1112.10n", 0x0b0000, 0x010000, CRC(4d5fc284) SHA1(c66f95af700828225a62f46437ca83453900f7fc) )
	ROM_LOAD( "136057-1114.11n", 0x0c0000, 0x010000, CRC(a70fc6e6) SHA1(fbf469b8f5c6e69540743748ad994a6490ad7745) )
	ROM_LOAD( "136057-1116.13n", 0x0d0000, 0x010000, CRC(f423b4f8) SHA1(a431686233b104074728a81cf41604deea0fbb56) )
	ROM_LOAD( "136057-1118.14n", 0x0e0000, 0x010000, CRC(56fa3d16) SHA1(9d9c1fb7912774954224d8f0220047324122ab23) )
	ROM_LOAD( "136057-1120.16n", 0x0f0000, 0x010000, CRC(f257f738) SHA1(a5904ec25d2190f11708c2e1e41832fd66332428) )
ROM_END


ROM_START( blstroidg )
	ROM_REGION( 0x40000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136057-2223.6c",  0x000000, 0x010000, CRC(cc82108b) SHA1(487a80cac2a196e9b17c64c5d0b884d1ed8da401) )
	ROM_LOAD16_BYTE( "136057-2221.6b",  0x000001, 0x010000, CRC(84822e68) SHA1(763edc9b3605e583506ca1d9befab66411fc720a) )
	ROM_LOAD16_BYTE( "136057-2224.4c",  0x020000, 0x010000, CRC(849249d4) SHA1(61d6eaff7df54f0353639e192eb6074a80916e29) )
	ROM_LOAD16_BYTE( "136057-2222.4b",  0x020001, 0x010000, CRC(bdeaba0d) SHA1(f479514b5d9543f9e12aa1ac48e20bf054cb18d0) )

	ROM_REGION( 0x14000, "jsa", 0 )	/* 64k for 6502 code */
	ROM_LOAD( "136057-1135.2k",  0x010000, 0x004000, CRC(baa8b5fe) SHA1(4af1f9bec3ffa856016a89bc20041d572305ba3a) )
	ROM_CONTINUE(                0x004000, 0x00c000 )

	ROM_REGION( 0x040000, "gfx1", 0 )
	ROM_LOAD( "136057-1101.1l",  0x000000, 0x010000, CRC(3c2daa5b) SHA1(2710a05e95afd8452104c4f4a9250a3b7d728a42) )
	ROM_LOAD( "136057-1102.1m",  0x010000, 0x010000, CRC(f84f0b97) SHA1(00cb5f1e0f92742683ee71854085b1e4db4bd6bb) )
	ROM_LOAD( "136057-1103.3l",  0x020000, 0x010000, CRC(ae5274f0) SHA1(87070e6e51d557c1b10ef32ac0ed670856d5aaf1) )
	ROM_LOAD( "136057-1104.3m",  0x030000, 0x010000, CRC(4bb72060) SHA1(94cd1a6900f47a5178cec041fa6dc9cfee1f9c3f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "136057-1105.5m",  0x000000, 0x010000, CRC(50e0823f) SHA1(f638becad83307ed43d138d452199e4c6725512f) )
	ROM_LOAD( "136057-1107.67m", 0x010000, 0x010000, CRC(729de7a9) SHA1(526b08e6d54cd0b991c4207c23119d2940a34009) )
	ROM_LOAD( "136057-1109.8m",  0x020000, 0x010000, CRC(090e42ab) SHA1(903aa99e6e39407319f6e90102b24604884ee047) )
	ROM_LOAD( "136057-1111.10m", 0x030000, 0x010000, CRC(1ff79e67) SHA1(12d408184f814bab411f567e8b29914a289e3fb8) )
	ROM_LOAD( "136057-1113.11m", 0x040000, 0x010000, CRC(4be1d504) SHA1(f41ff2d31e2e0e5b6d89fbbf014ba767c7b9f299) )
	ROM_LOAD( "136057-1115.13m", 0x050000, 0x010000, CRC(e4409310) SHA1(09180f1ab2ac8465b6641e94271c72bf566b2597) )
	ROM_LOAD( "136057-1117.14m", 0x060000, 0x010000, CRC(7aaca15e) SHA1(4014d60f2b6590c96796dbb2a538f1976194f3e7) )
	ROM_LOAD( "136057-1119.16m", 0x070000, 0x010000, CRC(33690379) SHA1(09ddfd18ccab1c639837171a763a981c867af0b1) )
	ROM_LOAD( "136057-1106.5n",  0x080000, 0x010000, CRC(2720ee71) SHA1(ebfd58effebadab361dfb4bd77d626911da4409a) )
	ROM_LOAD( "136057-1108.67n", 0x090000, 0x010000, CRC(2faecd15) SHA1(7fe9535b9bc72fd5527dbd1079f559ac16f2a31e) )
	ROM_LOAD( "136057-1110.8n",  0x0a0000, 0x010000, CRC(a15e79e1) SHA1(3fc8c33f438fd304b566a62bbe0f6e17a696edbc) )
	ROM_LOAD( "136057-1112.10n", 0x0b0000, 0x010000, CRC(4d5fc284) SHA1(c66f95af700828225a62f46437ca83453900f7fc) )
	ROM_LOAD( "136057-1114.11n", 0x0c0000, 0x010000, CRC(a70fc6e6) SHA1(fbf469b8f5c6e69540743748ad994a6490ad7745) )
	ROM_LOAD( "136057-1116.13n", 0x0d0000, 0x010000, CRC(f423b4f8) SHA1(a431686233b104074728a81cf41604deea0fbb56) )
	ROM_LOAD( "136057-1118.14n", 0x0e0000, 0x010000, CRC(56fa3d16) SHA1(9d9c1fb7912774954224d8f0220047324122ab23) )
	ROM_LOAD( "136057-1120.16n", 0x0f0000, 0x010000, CRC(f257f738) SHA1(a5904ec25d2190f11708c2e1e41832fd66332428) )
ROM_END


ROM_START( blstroidh )
	ROM_REGION( 0x40000, "maincpu", 0 )	/* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "eheadh0.c6",  0x00000, 0x10000, CRC(061f0898) SHA1(a277399aa8af665b1fb40c2bb4cf5d36d333db8d) )
	ROM_LOAD16_BYTE( "eheadl0.b6",  0x00001, 0x10000, CRC(ae8df7cb) SHA1(9eaf377bbfa09e2d3ae77764dbf09ff79b65b34f) )
	ROM_LOAD16_BYTE( "eheadh1.c5",  0x20000, 0x10000, CRC(0b7a3cb6) SHA1(7dc585ff536055e85b0849aa075f2fdab34a8e1c) )
	ROM_LOAD16_BYTE( "eheadl1.b5",  0x20001, 0x10000, CRC(43971694) SHA1(a39a8da244645bb56081fd71609a33d8b7d78478) )

	ROM_REGION( 0x14000, "jsa", 0 )	/* 64k for 6502 code */
	ROM_LOAD( "136057-1135.2k",  0x010000, 0x004000, CRC(baa8b5fe) SHA1(4af1f9bec3ffa856016a89bc20041d572305ba3a) )
	ROM_CONTINUE(                0x004000, 0x00c000 )

	ROM_REGION( 0x040000, "gfx1", 0 )
	ROM_LOAD( "136057-1101.1l",  0x000000, 0x010000, CRC(3c2daa5b) SHA1(2710a05e95afd8452104c4f4a9250a3b7d728a42) )
	ROM_LOAD( "136057-1102.1m",  0x010000, 0x010000, CRC(f84f0b97) SHA1(00cb5f1e0f92742683ee71854085b1e4db4bd6bb) )
	ROM_LOAD( "136057-1103.3l",  0x020000, 0x010000, CRC(ae5274f0) SHA1(87070e6e51d557c1b10ef32ac0ed670856d5aaf1) )
	ROM_LOAD( "136057-1104.3m",  0x030000, 0x010000, CRC(4bb72060) SHA1(94cd1a6900f47a5178cec041fa6dc9cfee1f9c3f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "136057-1105.5m",  0x000000, 0x010000, CRC(50e0823f) SHA1(f638becad83307ed43d138d452199e4c6725512f) )
	ROM_LOAD( "136057-1107.67m", 0x010000, 0x010000, CRC(729de7a9) SHA1(526b08e6d54cd0b991c4207c23119d2940a34009) )
	ROM_LOAD( "136057-1109.8m",  0x020000, 0x010000, CRC(090e42ab) SHA1(903aa99e6e39407319f6e90102b24604884ee047) )
	ROM_LOAD( "136057-1111.10m", 0x030000, 0x010000, CRC(1ff79e67) SHA1(12d408184f814bab411f567e8b29914a289e3fb8) )
	ROM_LOAD( "mol4.m12",        0x040000, 0x010000, CRC(571139ea) SHA1(646ad4d98f2125aa14ff5e39493cbbbd2f7bf3f8) )
	ROM_LOAD( "136057-1115.13m", 0x050000, 0x010000, CRC(e4409310) SHA1(09180f1ab2ac8465b6641e94271c72bf566b2597) )
	ROM_LOAD( "136057-1117.14m", 0x060000, 0x010000, CRC(7aaca15e) SHA1(4014d60f2b6590c96796dbb2a538f1976194f3e7) )
	ROM_LOAD( "mol7.m16",        0x070000, 0x010000, CRC(d27b2d91) SHA1(5268936a99927c5d31a5f23129e2169abe29d23c) )
	ROM_LOAD( "136057-1106.5n",  0x080000, 0x010000, CRC(2720ee71) SHA1(ebfd58effebadab361dfb4bd77d626911da4409a) )
	ROM_LOAD( "136057-1108.67n", 0x090000, 0x010000, CRC(2faecd15) SHA1(7fe9535b9bc72fd5527dbd1079f559ac16f2a31e) )
	ROM_LOAD( "moh2.n8",         0x0a0000, 0x010000, CRC(a15e79e1) SHA1(3fc8c33f438fd304b566a62bbe0f6e17a696edbc) )
	ROM_LOAD( "136057-1112.10n", 0x0b0000, 0x010000, CRC(4d5fc284) SHA1(c66f95af700828225a62f46437ca83453900f7fc) )
	ROM_LOAD( "moh4.n12",        0x0c0000, 0x010000, CRC(1a74e960) SHA1(fb5a631254fd770fa9542ca4419d4d16bae9591b) )
	ROM_LOAD( "136057-1116.13n", 0x0d0000, 0x010000, CRC(f423b4f8) SHA1(a431686233b104074728a81cf41604deea0fbb56) )
	ROM_LOAD( "136057-1118.14n", 0x0e0000, 0x010000, CRC(56fa3d16) SHA1(9d9c1fb7912774954224d8f0220047324122ab23) )
	ROM_LOAD( "moh7.n16",        0x0f0000, 0x010000, CRC(a93cbbe7) SHA1(5583e2421ae25181039c6145319453fb73e7bbf5) )
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

static DRIVER_INIT( blstroid )
{
	atarijsa_init(machine, "IN0", 0x80);
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1987, blstroid,  0,        blstroid, blstroid, blstroid, ROT0, "Atari Games", "Blasteroids (rev 4)", 0 )
GAME( 1987, blstroid3, blstroid, blstroid, blstroid, blstroid, ROT0, "Atari Games", "Blasteroids (rev 3)", 0 )
GAME( 1987, blstroid2, blstroid, blstroid, blstroid, blstroid, ROT0, "Atari Games", "Blasteroids (rev 2)", 0 )
GAME( 1987, blstroidg, blstroid, blstroid, blstroid, blstroid, ROT0, "Atari Games", "Blasteroids (German, rev 2)", 0 )
GAME( 1987, blstroidh, blstroid, blstroid, blstroid, blstroid, ROT0, "Atari Games", "Blasteroids (with heads)", 0 )
