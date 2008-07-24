/***************************************************************************

    Atari Tetris hardware

    driver by Zsolt Vasvari

    Games supported:
        * Tetris

    Known bugs:
        * none at this time

****************************************************************************

    Memory map

****************************************************************************

    ========================================================================
    CPU #1
    ========================================================================
    0000-0FFF   R/W   xxxxxxxx    Program RAM
    1000-1FFF   R/W   xxxxxxxx    Playfield RAM
                      xxxxxxxx       (byte 0: LSB of character code)
                      -----xxx       (byte 1: MSB of character code)
                      xxxx----       (byte 1: palette index)
    2000-20FF   R/W   xxxxxxxx    Palette RAM
                      xxx----        (red component)
                      ---xxx--       (green component)
                      ------xx       (blue component)
    2400-25FF   R/W   xxxxxxxx    EEPROM
    2800-280F   R/W   xxxxxxxx    POKEY #1
    2810-281F   R/W   xxxxxxxx    POKEY #2
    3000          W   --------    Watchdog
    3400          W   --------    EEPROM write enable
    3800          W   --------    IRQ acknowledge
    3C00          W   --xx----    Coin counters
                  W   --x-----       (right coin counter)
                  W   ---x----       (left coin counter)
    4000-7FFF   R     xxxxxxxx    Banked program ROM
    8000-FFFF   R     xxxxxxxx    Program ROM
    ========================================================================
    Interrupts:
        IRQ generated by 32V
    ========================================================================

***************************************************************************/


#include "driver.h"
#include "cpu/m6502/m6502.h"
#include "slapstic.h"
#include "atetris.h"
#include "sound/sn76496.h"
#include "sound/pokey.h"
#include "state.h"


#define MASTER_CLOCK		XTAL_14_31818MHz
#define BOOTLEG_CLOCK		XTAL_14_7456MHz


/* Local variables */
static UINT8 *slapstic_source;
static UINT8 *slapstic_base;
static UINT8 current_bank;
static UINT8 nvram_write_enable;
static emu_timer *interrupt_timer;



/*************************************
 *
 *  Interrupt generation
 *
 *************************************/

static TIMER_CALLBACK( interrupt_gen )
{
	int scanline = param;

	/* assert/deassert the interrupt */
	cpunum_set_input_line(machine, 0, 0, (scanline & 32) ? ASSERT_LINE : CLEAR_LINE);

	/* set the next timer */
	scanline += 32;
	if (scanline >= 256)
		scanline -= 256;
	timer_adjust_oneshot(interrupt_timer, video_screen_get_time_until_pos(machine->primary_screen, scanline, 0), scanline);
}


static WRITE8_HANDLER( irq_ack_w )
{
	cpunum_set_input_line(machine, 0, 0, CLEAR_LINE);
}



/*************************************
 *
 *  Machine init
 *
 *************************************/

static void reset_bank(void)
{
	memcpy(slapstic_base, &slapstic_source[current_bank * 0x4000], 0x4000);
}


static STATE_POSTLOAD( atetris_postload )
{
	reset_bank();
}


static MACHINE_START( atetris )
{
	/* Allocate interrupt timer */
	interrupt_timer = timer_alloc(interrupt_gen, NULL);

	/* Set up save state */
	state_save_register_global(current_bank);
	state_save_register_global(nvram_write_enable);
	state_save_register_postload(machine, atetris_postload, NULL);
}


static MACHINE_RESET( atetris )
{
	/* reset the slapstic */
	slapstic_reset();
	current_bank = slapstic_bank() & 1;
	reset_bank();

	/* start interrupts going (32V clocked by 16V) */
	timer_adjust_oneshot(interrupt_timer, video_screen_get_time_until_pos(machine->primary_screen, 48, 0), 48);
}



/*************************************
 *
 *  Slapstic handler
 *
 *************************************/

static READ8_HANDLER( atetris_slapstic_r )
{
	int result = slapstic_base[0x2000 + offset];
	int new_bank = slapstic_tweak(offset) & 1;

	/* update for the new bank */
	if (new_bank != current_bank)
	{
		current_bank = new_bank;
		memcpy(slapstic_base, &slapstic_source[current_bank * 0x4000], 0x4000);
	}
	return result;
}



/*************************************
 *
 *  Coin counters
 *
 *************************************/

static WRITE8_HANDLER( coincount_w )
{
	coin_counter_w(0, (data >> 5) & 1);
	coin_counter_w(1, (data >> 4) & 1);
}



/*************************************
 *
 *  NVRAM handlers
 *
 *************************************/

static WRITE8_HANDLER( nvram_w )
{
	if (nvram_write_enable)
		generic_nvram[offset] = data;
	nvram_write_enable = 0;
}


static WRITE8_HANDLER( nvram_enable_w )
{
	nvram_write_enable = 1;
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

/* full address map derived from schematics */
static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_RAM
	AM_RANGE(0x1000, 0x1fff) AM_RAM_WRITE(atetris_videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2000, 0x20ff) AM_MIRROR(0x0300) AM_RAM_WRITE(paletteram_RRRGGGBB_w) AM_BASE(&paletteram)
	AM_RANGE(0x2400, 0x25ff) AM_MIRROR(0x0200) AM_RAM_WRITE(nvram_w) AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x2800, 0x280f) AM_MIRROR(0x03e0) AM_READWRITE(pokey1_r, pokey1_w)
	AM_RANGE(0x2810, 0x281f) AM_MIRROR(0x03e0) AM_READWRITE(pokey2_r, pokey2_w)
	AM_RANGE(0x3000, 0x3000) AM_MIRROR(0x03ff) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x3400, 0x3400) AM_MIRROR(0x03ff) AM_WRITE(nvram_enable_w)
	AM_RANGE(0x3800, 0x3800) AM_MIRROR(0x03ff) AM_WRITE(irq_ack_w)
	AM_RANGE(0x3c00, 0x3c00) AM_MIRROR(0x03ff) AM_WRITE(coincount_w)
	AM_RANGE(0x4000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x7fff) AM_READ(atetris_slapstic_r)
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( atetrsb2_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_RAM
	AM_RANGE(0x1000, 0x1fff) AM_RAM_WRITE(atetris_videoram_w) AM_BASE(&videoram) AM_SIZE(&videoram_size)
	AM_RANGE(0x2000, 0x20ff) AM_RAM_WRITE(paletteram_RRRGGGBB_w) AM_BASE(&paletteram)
	AM_RANGE(0x2400, 0x25ff) AM_RAM_WRITE(nvram_w) AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x2802, 0x2802) AM_WRITE(SN76496_0_w)
	AM_RANGE(0x2804, 0x2804) AM_WRITE(SN76496_1_w)
	AM_RANGE(0x2806, 0x2806) AM_WRITE(SN76496_2_w)
	AM_RANGE(0x2808, 0x2808) AM_READ_PORT("IN0")
	AM_RANGE(0x2818, 0x2818) AM_READ_PORT("IN1")
	AM_RANGE(0x3000, 0x3000) AM_WRITE(watchdog_reset_w)
	AM_RANGE(0x3400, 0x3400) AM_WRITE(nvram_enable_w)
	AM_RANGE(0x3800, 0x3800) AM_WRITE(irq_ack_w)
	AM_RANGE(0x3c00, 0x3c00) AM_WRITE(coincount_w)
	AM_RANGE(0x4000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x7fff) AM_READ(atetris_slapstic_r)
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( atetris )
	// These ports are read via the Pokeys
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_DIPNAME( 0x04, 0x00, "Freeze" )			PORT_DIPLOCATION("50H:!4")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Freeze Step" )		PORT_DIPLOCATION("50H:!3")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x10, 0x00, "50H:!2" )	/* Listed As "SPARE2 (Unused)" */
	PORT_DIPUNUSED_DIPLOC( 0x20, 0x00, "50H:!1" )	/* Listed As "SPARE1 (Unused)" */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_SERVICE( 0x80, IP_ACTIVE_HIGH )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
INPUT_PORTS_END


// Same as the regular one except they added a Flip Controls switch
static INPUT_PORTS_START( atetcktl )
	PORT_INCLUDE( atetris )

	PORT_MODIFY("IN0")
	PORT_DIPNAME( 0x20, 0x00, "Flip Controls" )		PORT_DIPLOCATION("50H:!1")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
INPUT_PORTS_END



/*************************************
 *
 *  Graphics layouts
 *
 *************************************/

static const gfx_layout charlayout =
{
   8,8,
   RGN_FRAC(1,1),
   4,
   { 0,1,2,3 },
   { 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4},
   { 0*4*8, 1*4*8, 2*4*8, 3*4*8, 4*4*8, 5*4*8, 6*4*8, 7*4*8},
   8*8*4
};


static GFXDECODE_START( atetris )
	GFXDECODE_ENTRY( REGION_GFX1, 0, charlayout, 0, 16 )
GFXDECODE_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static const struct POKEYinterface pokey_interface_1 =
{
	{ 0 },
	input_port_0_r
};


static const struct POKEYinterface pokey_interface_2 =
{
	{ 0 },
	input_port_1_r
};



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( atetris )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M6502,MASTER_CLOCK/8)
	MDRV_CPU_PROGRAM_MAP(main_map,0)

	MDRV_MACHINE_START(atetris)
	MDRV_MACHINE_RESET(atetris)
	MDRV_NVRAM_HANDLER(generic_1fill)

	/* video hardware */
	MDRV_GFXDECODE(atetris)
	MDRV_PALETTE_LENGTH(256)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	/* note: these parameters are from published specs, not derived */
	/* the board uses an SOS-2 chip to generate video signals */
	MDRV_SCREEN_RAW_PARAMS(MASTER_CLOCK/2, 456, 0, 336, 262, 0, 240)

	MDRV_VIDEO_START(atetris)
	MDRV_VIDEO_UPDATE(atetris)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("pokey1", POKEY, MASTER_CLOCK/8)
	MDRV_SOUND_CONFIG(pokey_interface_1)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD("pokey2", POKEY, MASTER_CLOCK/8)
	MDRV_SOUND_CONFIG(pokey_interface_2)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( atetrsb2 )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M6502,BOOTLEG_CLOCK/8)
	MDRV_CPU_PROGRAM_MAP(atetrsb2_map,0)

	MDRV_MACHINE_START(atetris)
	MDRV_MACHINE_RESET(atetris)
	MDRV_NVRAM_HANDLER(generic_1fill)

	/* video hardware */
	MDRV_GFXDECODE(atetris)
	MDRV_PALETTE_LENGTH(256)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	/* note: these parameters are from published specs, not derived */
	/* the board uses an SOS-2 chip to generate video signals */
	MDRV_SCREEN_RAW_PARAMS(MASTER_CLOCK/2, 456, 0, 336, 262, 0, 240)

	MDRV_VIDEO_START(atetris)
	MDRV_VIDEO_UPDATE(atetris)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("sn1", SN76496, BOOTLEG_CLOCK/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD("sn2", SN76496, BOOTLEG_CLOCK/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MDRV_SOUND_ADD("sn3", SN76496, BOOTLEG_CLOCK/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( atetris )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "136066-1100.45f", 0x10000, 0x8000, CRC(2acbdb09) SHA1(5e1189227f26563fd3e5372121ea5c915620f892) )
	ROM_CONTINUE(                0x08000, 0x8000 )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136066-1101.35a", 0x0000, 0x10000, CRC(84a1939f) SHA1(d8577985fc8ed4e74f74c68b7c00c4855b7c3270) )
ROM_END


ROM_START( atetrisa )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "d1",           0x10000, 0x8000, CRC(2bcab107) SHA1(3cfb8df8cd3782f3ff7f6b32ff15c461352061ee) )
	ROM_CONTINUE(             0x08000, 0x8000 )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136066-1101.35a",     0x0000, 0x10000, CRC(84a1939f) SHA1(d8577985fc8ed4e74f74c68b7c00c4855b7c3270) )
ROM_END


ROM_START( atetrisb )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "tetris.01",    0x10000, 0x8000, CRC(944d15f6) SHA1(926fa5cb26b6e6a50bea455eec1f6d3fb92aa95c) )
	ROM_CONTINUE(             0x08000, 0x8000 )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "tetris.02",    0x0000, 0x10000, CRC(5c4e7258) SHA1(58060681a728e74d69b2b6f5d02faa597ca6c226) )

	/* there's an extra EEPROM, maybe used for protection crack, which */
	/* however doesn't seem to be required to run the game in this driver. */
	ROM_REGION( 0x0800, REGION_USER1, 0 )
	ROM_LOAD( "tetris.03",    0x0000, 0x0800, CRC(26618c0b) SHA1(4d6470bf3a79be3b0766e246abe00582d4c85a97) )
ROM_END


ROM_START( atetrsb2 )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "k1-01",    0x10000, 0x8000, CRC(fa056809) SHA1(e4ccccdf9b04b68127c7b03ae263519cf00f94cb) )
	ROM_CONTINUE(         0x08000, 0x8000 )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136066-1101.35a", 0x0000, 0x10000, CRC(84a1939f) SHA1(d8577985fc8ed4e74f74c68b7c00c4855b7c3270) )
ROM_END


ROM_START( atetcktl )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "tetcktl1.rom", 0x10000, 0x8000, CRC(9afd1f4a) SHA1(323d1576d92c905e8e95108b39cabf6fa0c10db6) )
	ROM_CONTINUE(             0x08000, 0x8000 )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136066-1103.35a", 0x0000, 0x10000, CRC(ec2a7f93) SHA1(cb850141ffd1504f940fa156a39e71a4146d7fea) )
ROM_END


ROM_START( atetckt2 )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "136066-1102.45f", 0x10000, 0x8000, CRC(1bd28902) SHA1(ae8c34f082bce1f827bf60830f207c46cb282421) )
	ROM_CONTINUE(                0x08000, 0x8000 )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "136066-1103.35a", 0x0000, 0x10000, CRC(ec2a7f93) SHA1(cb850141ffd1504f940fa156a39e71a4146d7fea) )
ROM_END



/*************************************
 *
 *  Driver init
 *
 *************************************/

static DRIVER_INIT( atetris )
{
	UINT8 *rgn = memory_region(machine, REGION_CPU1);
	slapstic_init(machine, 101);
	slapstic_source = &rgn[0x10000];
	slapstic_base = &rgn[0x04000];
}



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1988, atetris,  0,       atetris,  atetris,  atetris, ROT0,   "Atari Games", "Tetris (set 1)", GAME_SUPPORTS_SAVE )
GAME( 1988, atetrisa, atetris, atetris,  atetris,  atetris, ROT0,   "Atari Games", "Tetris (set 2)", GAME_SUPPORTS_SAVE )
GAME( 1988, atetrisb, atetris, atetris,  atetris,  atetris, ROT0,   "bootleg",     "Tetris (bootleg set 1)", GAME_SUPPORTS_SAVE )
GAME( 1988, atetrsb2, atetris, atetrsb2, atetris,  atetris, ROT0,   "bootleg",     "Tetris (bootleg set 2)", GAME_SUPPORTS_SAVE )
GAME( 1989, atetcktl, atetris, atetris,  atetcktl, atetris, ROT270, "Atari Games", "Tetris (cocktail set 1)", GAME_SUPPORTS_SAVE )
GAME( 1989, atetckt2, atetris, atetris,  atetcktl, atetris, ROT270, "Atari Games", "Tetris (cocktail set 2)", GAME_SUPPORTS_SAVE )
