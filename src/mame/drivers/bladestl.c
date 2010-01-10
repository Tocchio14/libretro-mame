/***************************************************************************

    Blades of Steel (GX797) (c) 1987 Konami

    Driver by Manuel Abadia <manu@teleline.es>

    Interrupts:

        CPU #0 (6309):
        --------------
        * IRQ: not used.
        * FIRQ: generated by VBLANK.
        * NMI: writes the sound command to the 6809.

        CPU #1 (6809):
        --------------
        * IRQ: triggered by the 6309 when a sound command is written.
        * FIRQ: not used.
        * NMI: not used.

    Notes:
        * The protection is not fully understood(Konami 051733). The
        game is playable, but is not 100% accurate.
        * Missing samples.

***************************************************************************/

#include "emu.h"
#include "deprecat.h"
#include "cpu/m6809/m6809.h"
#include "cpu/hd6309/hd6309.h"
#include "sound/2203intf.h"
#include "sound/upd7759.h"
#include "video/konicdev.h"
#include "includes/konamipt.h"
#include "includes/bladestl.h"


static INTERRUPT_GEN( bladestl_interrupt )
{
	bladestl_state *state = (bladestl_state *)device->machine->driver_data;

	if (cpu_getiloops(device) == 0)
	{
		if (k007342_is_int_enabled(state->k007342))
			cpu_set_input_line(device, HD6309_FIRQ_LINE, HOLD_LINE);
	}
	else if (cpu_getiloops(device) % 2)
	{
		cpu_set_input_line(device, INPUT_LINE_NMI, PULSE_LINE);
	}
}

/*************************************
 *
 *  Memory handlers
 *
 *************************************/

static READ8_HANDLER( trackball_r )
{
	bladestl_state *state = (bladestl_state *)space->machine->driver_data;
	static const char *const port[] = { "TRACKBALL_P1_1", "TRACKBALL_P1_2", "TRACKBALL_P2_1", "TRACKBALL_P1_2" };
	int curr, delta;

	curr = input_port_read(space->machine, port[offset]);
	delta = (curr - state->last_track[offset]) & 0xff;
	state->last_track[offset] = curr;

	return (delta & 0x80) | (curr >> 1);
}

static WRITE8_HANDLER( bladestl_bankswitch_w )
{
	bladestl_state *state = (bladestl_state *)space->machine->driver_data;

	/* bits 0 & 1 = coin counters */
	coin_counter_w(space->machine, 0,data & 0x01);
	coin_counter_w(space->machine, 1,data & 0x02);

	/* bits 2 & 3 = lamps */
	set_led_status(space->machine, 0,data & 0x04);
	set_led_status(space->machine, 1,data & 0x08);

	/* bit 4 = relay (???) */

	/* bits 5-6 = bank number */
	memory_set_bank(space->machine, "bank1", (data & 0x60) >> 5);

	/* bit 7 = select sprite bank */
	state->spritebank = (data & 0x80) << 3;

}

static WRITE8_HANDLER( bladestl_sh_irqtrigger_w )
{
	bladestl_state *state = (bladestl_state *)space->machine->driver_data;

	soundlatch_w(space, offset, data);
	cpu_set_input_line(state->audiocpu, M6809_IRQ_LINE, HOLD_LINE);
	//logerror("(sound) write %02x\n", data);
}

static WRITE8_DEVICE_HANDLER( bladestl_port_B_w )
{
	/* bit 1, 2 unknown */
	upd7759_set_bank_base(device, ((data & 0x38) >> 3) * 0x20000);
}

static READ8_DEVICE_HANDLER( bladestl_speech_busy_r )
{
	return upd7759_busy_r(device) ? 1 : 0;
}

static WRITE8_DEVICE_HANDLER( bladestl_speech_ctrl_w )
{
	upd7759_reset_w(device, data & 1);
	upd7759_start_w(device, data & 2);
}

/*************************************
 *
 *  Address maps
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_DEVREADWRITE("k007342", k007342_r, k007342_w)	/* Color RAM + Video RAM */
	AM_RANGE(0x2000, 0x21ff) AM_DEVREADWRITE("k007420", k007420_r, k007420_w)	/* Sprite RAM */
	AM_RANGE(0x2200, 0x23ff) AM_DEVREADWRITE("k007342", k007342_scroll_r, k007342_scroll_w)	/* Scroll RAM */
	AM_RANGE(0x2400, 0x245f) AM_RAM AM_BASE_MEMBER(bladestl_state, paletteram)		/* palette */
	AM_RANGE(0x2600, 0x2607) AM_DEVWRITE("k007342", k007342_vreg_w)			/* Video Registers */
	AM_RANGE(0x2e00, 0x2e00) AM_READ_PORT("COINSW")				/* DIPSW #3, coinsw, startsw */
	AM_RANGE(0x2e01, 0x2e01) AM_READ_PORT("P1")					/* 1P controls */
	AM_RANGE(0x2e02, 0x2e02) AM_READ_PORT("P2")					/* 2P controls */
	AM_RANGE(0x2e03, 0x2e03) AM_READ_PORT("DSW2")				/* DISPW #2 */
	AM_RANGE(0x2e40, 0x2e40) AM_READ_PORT("DSW1")				/* DIPSW #1 */
	AM_RANGE(0x2e80, 0x2e80) AM_WRITE(bladestl_sh_irqtrigger_w)	/* cause interrupt on audio CPU */
	AM_RANGE(0x2ec0, 0x2ec0) AM_WRITE(watchdog_reset_w)			/* watchdog reset */
	AM_RANGE(0x2f00, 0x2f03) AM_READ(trackball_r)				/* Trackballs */
	AM_RANGE(0x2f40, 0x2f40) AM_WRITE(bladestl_bankswitch_w)	/* bankswitch control */
	AM_RANGE(0x2f80, 0x2f9f) AM_DEVREADWRITE("k051733", k051733_r, k051733_w)	/* Protection: 051733 */
	AM_RANGE(0x2fc0, 0x2fc0) AM_WRITENOP						/* ??? */
	AM_RANGE(0x4000, 0x5fff) AM_RAM								/* Work RAM */
	AM_RANGE(0x6000, 0x7fff) AM_ROMBANK("bank1")						/* banked ROM */
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_RAM
	AM_RANGE(0x1000, 0x1001) AM_DEVREADWRITE("ymsnd", ym2203_r, ym2203_w)	/* YM2203 */
	AM_RANGE(0x3000, 0x3000) AM_DEVWRITE("upd", bladestl_speech_ctrl_w)	/* UPD7759 */
	AM_RANGE(0x4000, 0x4000) AM_DEVREAD("upd", bladestl_speech_busy_r)	/* UPD7759 */
	AM_RANGE(0x5000, 0x5000) AM_WRITENOP								/* ??? */
	AM_RANGE(0x6000, 0x6000) AM_READ(soundlatch_r)						/* soundlatch_r */
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END


/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( bladestl )
	PORT_START("DSW1")
	KONAMI_COINAGE_ALT_LOC(SW1)

	PORT_START("DSW2")
	PORT_DIPUNUSED_DIPLOC( 0x01, 0x01, "SW2:1" )
	PORT_DIPUNUSED_DIPLOC( 0x02, 0x02, "SW2:2" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )		PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(	0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(	0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x18, 0x18, "Bonus time set" )		PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x18, "30 secs" )
	PORT_DIPSETTING(    0x10, "20 secs" )
	PORT_DIPSETTING(    0x08, "15 secs" )
	PORT_DIPSETTING(    0x00, "10 secs" )
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )	PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(	0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(	0x40, DEF_STR( Normal ) )
	PORT_DIPSETTING(	0x20, DEF_STR( Difficult ) )
	PORT_DIPSETTING(	0x00, DEF_STR( Very_Difficult ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )	PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("COINSW")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )	PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW3:2" )
	PORT_SERVICE_DIPLOC(  0x80, IP_ACTIVE_LOW, "SW3:3" )

	PORT_START("P1")
	KONAMI8_B123(1)
	PORT_DIPNAME( 0x80, 0x80, "Period time set" )		PORT_DIPLOCATION("SW3:4")
	PORT_DIPSETTING(    0x80, "4" )
	PORT_DIPSETTING(    0x00, "7" )

	PORT_START("P2")
	KONAMI8_B123_UNK(2)

	PORT_START("TRACKBALL_P1_1")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(63) PORT_REVERSE PORT_PLAYER(1)

	PORT_START("TRACKBALL_P1_2")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(63) PORT_PLAYER(1)

	PORT_START("TRACKBALL_P2_1")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(63) PORT_REVERSE PORT_PLAYER(2)

	PORT_START("TRACKBALL_P2_2")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(63) PORT_PLAYER(2)
INPUT_PORTS_END

static INPUT_PORTS_START( bladstle )
	PORT_INCLUDE( bladestl )

	PORT_MODIFY("DSW2")
	PORT_DIPUNUSED_DIPLOC( 0x08, 0x08, "SW2:4" )	/* Listed as "Unused" */
	PORT_DIPUNUSED_DIPLOC( 0x10, 0x10, "SW2:5" )	/* Listed as "Unused" */

	PORT_MODIFY("P1")
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x80, "SW3:4" )	/* Listed as "Unused" */

	PORT_MODIFY("TRACKBALL_P2_1")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(63) PORT_PLAYER(2)

	PORT_MODIFY("TRACKBALL_P2_2")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(63) PORT_PLAYER(2)
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,			/* 8 x 8 characters */
	0x40000/32,		/* 8192 characters */
	4,				/* 4bpp */
	{ 0, 1, 2, 3 },	/* the four bitplanes are packed in one nibble */
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8			/* every character takes 32 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	8,8,			/* 8*8 sprites */
	0x40000/32,		/* 8192 sprites */
	4,				/* 4 bpp */
	{ 0, 1, 2, 3 },	/* the four bitplanes are packed in one nibble */
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8			/* every sprite takes 32 consecutive bytes */
};

static GFXDECODE_START( bladestl )
	GFXDECODE_ENTRY( "gfx1", 0x000000, charlayout,     0,	2 )	/* colors 00..31 */
	GFXDECODE_ENTRY( "gfx1", 0x040000, spritelayout,   32,	16 )	/* colors 32..47 but using lookup table */
GFXDECODE_END


/*************************************
 *
 *  Machine driver
 *
 *************************************/

static const ym2203_interface ym2203_config =
{
	{
		AY8910_LEGACY_OUTPUT,
		AY8910_DEFAULT_LOADS,
		DEVCB_NULL,
		DEVCB_NULL,
		DEVCB_DEVICE_HANDLER("upd", upd7759_port_w),
		DEVCB_DEVICE_HANDLER("upd", bladestl_port_B_w)
	},
	NULL
};

static const k007342_interface bladestl_k007342_intf =
{
	0,	bladestl_tile_callback	/* gfx_num (for tile creation), callback */
};

static const k007420_interface bladestl_k007420_intf =
{
	0x3ff,	bladestl_sprite_callback	/* banklimit, callback */
};


static MACHINE_START( bladestl )
{
	bladestl_state *state = (bladestl_state *)machine->driver_data;
	UINT8 *ROM = memory_region(machine, "maincpu");

	memory_configure_bank(machine, "bank1", 0, 4, &ROM[0x10000], 0x2000);

	state->audiocpu = devtag_get_device(machine, "audiocpu");
	state->k007342 = devtag_get_device(machine, "k007342");
	state->k007420 = devtag_get_device(machine, "k007420");

	state_save_register_global(machine, state->spritebank);
	state_save_register_global_array(machine, state->layer_colorbase);
	state_save_register_global_array(machine, state->last_track);
}

static MACHINE_RESET( bladestl )
{
	bladestl_state *state = (bladestl_state *)machine->driver_data;
	int i;

	state->layer_colorbase[0] = 0;
	state->layer_colorbase[1] = 1;
	state->spritebank = 0;

	for (i = 0; i < 4 ; i++)
		state->last_track[i] = 0;
}

static MACHINE_DRIVER_START( bladestl )

	/* driver data */
	MDRV_DRIVER_DATA(bladestl_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", HD6309, 24000000/2)		/* 24MHz/2 (?) */
	MDRV_CPU_PROGRAM_MAP(main_map)
	MDRV_CPU_VBLANK_INT_HACK(bladestl_interrupt,2) /* (1 IRQ + 1 NMI) */

	MDRV_CPU_ADD("audiocpu", M6809, 2000000)
	MDRV_CPU_PROGRAM_MAP(sound_map)

	MDRV_QUANTUM_TIME(HZ(600))

	MDRV_MACHINE_START(bladestl)
	MDRV_MACHINE_RESET(bladestl)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(bladestl)
	MDRV_PALETTE_INIT(bladestl)
	MDRV_PALETTE_LENGTH(32 + 16*16)

	MDRV_VIDEO_UPDATE(bladestl)

	MDRV_K007342_ADD("k007342", bladestl_k007342_intf)
	MDRV_K007420_ADD("k007420", bladestl_k007420_intf)
	MDRV_K051733_ADD("k051733")

	/* sound hardware */
	/* the initialization order is important, the port callbacks being
       called at initialization time */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("upd", UPD7759, UPD7759_STANDARD_CLOCK)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)

	MDRV_SOUND_ADD("ymsnd", YM2203, 3579545)
	MDRV_SOUND_CONFIG(ym2203_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.45)
MACHINE_DRIVER_END


/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( bladestl )
	ROM_REGION( 0x18000, "maincpu", 0 ) /* code + banked roms */
	ROM_LOAD( "797t01.bin", 0x10000, 0x08000, CRC(89d7185d) SHA1(0d2f346d9515cab0389106c0e227fb0bd84a2c9c) )	/* fixed ROM */
	ROM_CONTINUE(			0x08000, 0x08000 )				/* banked ROM */

	ROM_REGION( 0x10000, "audiocpu", 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "797c02", 0x08000, 0x08000, CRC(65a331ea) SHA1(f206f6c5f0474542a5b7686b2f4d2cc7077dd5b9) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "797a05",	0x000000, 0x40000, CRC(5491ba28) SHA1(c807774827c55c211ab68f548e1e835289cc5744) )	/* tiles */
	ROM_LOAD( "797a06",	0x040000, 0x40000, CRC(d055f5cc) SHA1(3723b39b2a3e6dd8e7fc66bbfe1eef9f80818774) )	/* sprites */

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "797a07", 0x0000, 0x0100, CRC(7aecad4e) SHA1(05150a8dd25bdd6ab0c5b350e6ffd272f040e46a) ) /* sprites lookup table */

	ROM_REGION( 0xc0000, "upd", 0 ) /* uPD7759 data (chip 1) */
	ROM_LOAD( "797a03", 0x00000, 0x80000, CRC(9ee1a542) SHA1(c9a142a326875a50f03e49e83a84af8bb423a467) )
	ROM_LOAD( "797a04",	0x80000, 0x40000, CRC(9ac8ea4e) SHA1(9f81eff970c9e8aea6f67d8a7d89805fae044ae1) )
ROM_END

ROM_START( bladestle )
	ROM_REGION( 0x18000, "maincpu", 0 ) /* code + banked roms */
	ROM_LOAD( "797e01", 0x10000, 0x08000, CRC(f8472e95) SHA1(8b6caa905fb1642300dd9da508871b00429872c3) )	/* fixed ROM */
	ROM_CONTINUE(		0x08000, 0x08000 )				/* banked ROM */

	ROM_REGION( 0x10000, "audiocpu", 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "797c02", 0x08000, 0x08000, CRC(65a331ea) SHA1(f206f6c5f0474542a5b7686b2f4d2cc7077dd5b9) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "797a05",	0x000000, 0x40000, CRC(5491ba28) SHA1(c807774827c55c211ab68f548e1e835289cc5744) )	/* tiles */
	ROM_LOAD( "797a06",	0x040000, 0x40000, CRC(d055f5cc) SHA1(3723b39b2a3e6dd8e7fc66bbfe1eef9f80818774) )	/* sprites */

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "797a07", 0x0000, 0x0100, CRC(7aecad4e) SHA1(05150a8dd25bdd6ab0c5b350e6ffd272f040e46a) ) /* sprites lookup table */

	ROM_REGION( 0xc0000, "upd", 0 ) /* uPD7759 data */
	ROM_LOAD( "797a03", 0x00000, 0x80000, CRC(9ee1a542) SHA1(c9a142a326875a50f03e49e83a84af8bb423a467) )
	ROM_LOAD( "797a04",	0x80000, 0x40000, CRC(9ac8ea4e) SHA1(9f81eff970c9e8aea6f67d8a7d89805fae044ae1) )
ROM_END


/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1987, bladestl,  0,        bladestl, bladestl, 0, ROT90, "Konami", "Blades of Steel (version T)", GAME_SUPPORTS_SAVE )
GAME( 1987, bladestle, bladestl, bladestl, bladstle, 0, ROT90, "Konami", "Blades of Steel (version E)", GAME_SUPPORTS_SAVE )
