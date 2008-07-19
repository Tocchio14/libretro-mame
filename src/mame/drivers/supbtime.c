/***************************************************************************

  Super Burger Time     (c) 1990 Data East Corporation (DE-0343)
  China Town            (c) 1991 Data East Corporation (DE-0343)

  Sound:  Ym2151, Oki adpcm - NOTE!  The sound program writes to the address
of a YM2203 and a 2nd Oki chip but the board does _not_ have them.  The sound
program is simply the 'generic' Data East sound program unmodified for this cut
down hardware (it doesn't write any good sound data btw, mostly zeros).

  Super Burgertime has a few bugs:

  Some sprites clip at the edges of the screen.
  Some burgers (from crushing an enemy) appear with wrong colour.
  Colour cycle on title screen doesn't work first time around.

  These are NOT driver bugs!  They all exist in the original game.

  Emulation by Bryan McPhail, mish@tendril.co.uk

***************************************************************************/

#include "driver.h"
#include "cpu/h6280/h6280.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"
#include "deco16ic.h"

VIDEO_START( supbtime );
VIDEO_UPDATE( supbtime );

/******************************************************************************/

static READ16_HANDLER( supbtime_controls_r )
{
 	switch (offset<<1)
	{
		case 0: /* Player 1 & Player 2 joysticks & fire buttons */
			return (input_port_read(machine, "P1") + (input_port_read(machine, "P2") << 8));
		case 2: /* Dips */
			return (input_port_read(machine, "DSW1") + (input_port_read(machine, "DSW2") << 8));
		case 8: /* Credits */
			return input_port_read(machine, "COIN");
		case 10: /* ?  Not used for anything */
		case 12:
			return 0;
	}

	logerror("CPU #0 PC %06x: warning - read unmapped control address %06x\n",activecpu_get_pc(),offset);
	return ~0;
}

static WRITE16_HANDLER( sound_w )
{
	soundlatch_w(machine,0,data & 0xff);
	cpunum_set_input_line(machine, 1,0,HOLD_LINE);
}

/******************************************************************************/

static ADDRESS_MAP_START( supbtime_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(SMH_ROM)
	AM_RANGE(0x100000, 0x103fff) AM_READ(SMH_RAM)
	AM_RANGE(0x120000, 0x1207ff) AM_READ(SMH_RAM)
	AM_RANGE(0x140000, 0x1407ff) AM_READ(SMH_RAM)
	AM_RANGE(0x180000, 0x18000f) AM_READ(supbtime_controls_r)
	AM_RANGE(0x320000, 0x321fff) AM_READ(SMH_RAM)
	AM_RANGE(0x322000, 0x323fff) AM_READ(SMH_RAM)
	AM_RANGE(0x340000, 0x3407ff) AM_READ(SMH_RAM)
	AM_RANGE(0x342000, 0x3427ff) AM_READ(SMH_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( supbtime_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(SMH_ROM)
	AM_RANGE(0x100000, 0x103fff) AM_WRITE(SMH_RAM)
	AM_RANGE(0x104000, 0x11ffff) AM_WRITE(SMH_NOP) /* Nothing there */
	AM_RANGE(0x120000, 0x1207ff) AM_WRITE(SMH_RAM) AM_BASE(&spriteram16)
	AM_RANGE(0x120800, 0x13ffff) AM_WRITE(SMH_NOP) /* Nothing there */
	AM_RANGE(0x140000, 0x1407ff) AM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x18000a, 0x18000d) AM_WRITE(SMH_NOP)
	AM_RANGE(0x1a0000, 0x1a0001) AM_WRITE(sound_w)

	AM_RANGE(0x300000, 0x30000f) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf12_control)
	AM_RANGE(0x320000, 0x321fff) AM_WRITE(deco16_pf1_data_w) AM_BASE(&deco16_pf1_data)
	AM_RANGE(0x322000, 0x323fff) AM_WRITE(deco16_pf2_data_w) AM_BASE(&deco16_pf2_data)
	AM_RANGE(0x340000, 0x3407ff) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf1_rowscroll)
	AM_RANGE(0x342000, 0x3427ff) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf2_rowscroll)
ADDRESS_MAP_END

static ADDRESS_MAP_START( chinatwn_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_READ(SMH_ROM)
	AM_RANGE(0x120000, 0x1207ff) AM_READ(SMH_RAM)
	AM_RANGE(0x140000, 0x1407ff) AM_READ(SMH_RAM)
	AM_RANGE(0x180000, 0x18000f) AM_READ(supbtime_controls_r)
	AM_RANGE(0x1a0000, 0x1a3fff) AM_READ(SMH_RAM)
	AM_RANGE(0x320000, 0x321fff) AM_READ(SMH_RAM)
	AM_RANGE(0x322000, 0x323fff) AM_READ(SMH_RAM)
	AM_RANGE(0x340000, 0x3407ff) AM_READ(SMH_RAM)
	AM_RANGE(0x342000, 0x3427ff) AM_READ(SMH_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( chinatwn_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x03ffff) AM_WRITE(SMH_ROM)
	AM_RANGE(0x100000, 0x100001) AM_WRITE(sound_w)
	AM_RANGE(0x120000, 0x1207ff) AM_WRITE(SMH_RAM) AM_BASE(&spriteram16)
	AM_RANGE(0x140000, 0x1407ff) AM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x18000a, 0x18000d) AM_WRITE(SMH_NOP)
	AM_RANGE(0x1a0000, 0x1a3fff) AM_WRITE(SMH_RAM)

	AM_RANGE(0x300000, 0x30000f) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf12_control)
	AM_RANGE(0x320000, 0x321fff) AM_WRITE(deco16_pf1_data_w) AM_BASE(&deco16_pf1_data)
	AM_RANGE(0x322000, 0x323fff) AM_WRITE(deco16_pf2_data_w) AM_BASE(&deco16_pf2_data)
	AM_RANGE(0x340000, 0x3407ff) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf1_rowscroll) // unused
	AM_RANGE(0x342000, 0x3427ff) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf2_rowscroll) // unused
ADDRESS_MAP_END

/******************************************************************************/

static WRITE8_HANDLER( YM2151_w )
{
	switch (offset) {
	case 0:
		YM2151_register_port_0_w(machine,0,data);
		break;
	case 1:
		YM2151_data_port_0_w(machine,0,data);
		break;
	}
}

/* Physical memory map (21 bits) */
static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_READ(SMH_ROM)
	AM_RANGE(0x100000, 0x100001) AM_READ(SMH_NOP)
	AM_RANGE(0x110000, 0x110001) AM_READ(YM2151_status_port_0_r)
	AM_RANGE(0x120000, 0x120001) AM_READ(OKIM6295_status_0_r)
	AM_RANGE(0x130000, 0x130001) AM_READ(SMH_NOP) /* This board only has 1 oki chip */
	AM_RANGE(0x140000, 0x140001) AM_READ(soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_READ(SMH_BANK8)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_WRITE(SMH_ROM)
	AM_RANGE(0x100000, 0x100001) AM_WRITE(SMH_NOP) /* YM2203 - this board doesn't have one */
	AM_RANGE(0x110000, 0x110001) AM_WRITE(YM2151_w)
	AM_RANGE(0x120000, 0x120001) AM_WRITE(OKIM6295_data_0_w)
	AM_RANGE(0x130000, 0x130001) AM_WRITE(SMH_NOP)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_WRITE(SMH_BANK8)
	AM_RANGE(0x1fec00, 0x1fec01) AM_WRITE(H6280_timer_w)
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(H6280_irq_status_w)
ADDRESS_MAP_END

/******************************************************************************/

static INPUT_PORTS_START( supbtime )
	PORT_START_TAG("P1")	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* button 3 - unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START_TAG("P2")	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* button 3 - unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START_TAG("COIN")	/* Credits */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("DSW1")	/* Dip switch bank 1 - inverted with respect to other Deco games */
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW2")	/* Dip switch bank 2 */
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x80, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Yes ) )
  	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( chinatwn )
	PORT_START_TAG("P1")	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) /* Used? */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )	/* button 3 - unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START_TAG("P2")	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)  /* Used? */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* button 3 - unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START_TAG("COIN")	/* Credits */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("DSW1")	/* Dip switch bank 1 - inverted with respect to other Deco games */
	PORT_DIPNAME( 0xe0, 0xe0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x1c, 0x1c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START_TAG("DSW2")	/* Dip switch bank 2 */
	PORT_DIPNAME( 0xc0, 0xc0, "Time" )
	PORT_DIPSETTING(    0x00, "1500" )
	PORT_DIPSETTING(    0x80, "2000" )
	PORT_DIPSETTING(    0xc0, "2500" )
	PORT_DIPSETTING(    0x40, "3000" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
  	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout tile_8x8_layout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	8*16
};

static const gfx_layout tile_16x16_layout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0 },
	{ 256,257,258,259,260,261,262,263,0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 24,8,16,0 },
	{ 512,513,514,515,516,517,518,519, 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
	  8*32, 9*32,10*32,11*32,12*32,13*32,14*32,15*32},
	32*32
};

static GFXDECODE_START( supbtime )
	GFXDECODE_ENTRY( REGION_GFX1, 0, tile_8x8_layout,     0x100, 32 )	/* Tiles (8x8) */
	GFXDECODE_ENTRY( REGION_GFX1, 0, tile_16x16_layout,   0x100, 32 )	/* Tiles (16x16) */
	GFXDECODE_ENTRY( REGION_GFX2, 0, spritelayout,        0, 16 )	/* Sprites (16x16) */
GFXDECODE_END

/******************************************************************************/

static void sound_irq(running_machine *machine, int state)
{
	cpunum_set_input_line(machine, 1,1,state); /* IRQ 2 */
}

static const struct YM2151interface ym2151_interface =
{
	sound_irq
};

static MACHINE_DRIVER_START( supbtime )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M68000, 14000000)
	MDRV_CPU_PROGRAM_MAP(supbtime_readmem,supbtime_writemem)
	MDRV_CPU_VBLANK_INT("main", irq6_line_hold)

	MDRV_CPU_ADD("audio", H6280, 32220000/8)	/* Custom chip 45, audio section crystal is 32.220 MHz */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_UPDATE_BEFORE_VBLANK)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(58)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(supbtime)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(supbtime)
	MDRV_VIDEO_UPDATE(supbtime)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym", YM2151, 32220000/9)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.45)
	MDRV_SOUND_ROUTE(1, "mono", 0.45)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( chinatwn )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M68000, 14000000)
	MDRV_CPU_PROGRAM_MAP(chinatwn_readmem,chinatwn_writemem)
	MDRV_CPU_VBLANK_INT("main", irq6_line_hold)

	MDRV_CPU_ADD("audio", H6280, 32220000/8) /* Custom chip 45, audio section crystal is 32.220 MHz */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_UPDATE_BEFORE_VBLANK)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(58)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(supbtime)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(supbtime)
	MDRV_VIDEO_UPDATE(supbtime)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym", YM2151, 32220000/9)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.45)
	MDRV_SOUND_ROUTE(1, "mono", 0.45)

	MDRV_SOUND_ADD("oki", OKIM6295, 1023924)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END

/******************************************************************************/

ROM_START( supbtime )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "gk03", 0x00000, 0x20000, CRC(aeaeed61) SHA1(4bceb4475a642a36406395f1e84b16fa137f67a5) )
	ROM_LOAD16_BYTE( "gk04", 0x00001, 0x20000, CRC(2bc5a4eb) SHA1(721ec73c32af8b998babb6d7c9e526ced0c2389b) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gc06.bin",    0x00000, 0x10000, CRC(e0e6c0f4) SHA1(5a8b29752c58ea76d9c7961c5b0d8c94f35037af) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mae02.bin", 0x000000, 0x80000, CRC(a715cca0) SHA1(0539bba39c60324d85599ac69ff78bb215deb511) ) /* chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
  	ROM_LOAD16_BYTE( "mae00.bin", 0x000001, 0x80000, CRC(30043094) SHA1(5302cfd9bdaf90c4901fda75407379c4ce1cbdec) ) /* sprites */
	ROM_LOAD16_BYTE( "mae01.bin", 0x000000, 0x80000, CRC(434af3fb) SHA1(1cfd30d14f03554e826576d6d32ce424f0df3748) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "gc05.bin",    0x00000, 0x20000, CRC(2f2246ff) SHA1(3fcceb6f5aa5f33187bcf4c59d88327f396fa80d) )
ROM_END

/* is this actually a good dump?, there are no backgrounds ingame! */
ROM_START( supbtima )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "3.11f", 0x00000, 0x20000, CRC(98b5f263) SHA1(ee4b0d2fcdc95aba0e78d066bd6c4d553a902848) )
	ROM_LOAD16_BYTE( "4.12f", 0x00001, 0x20000, CRC(937e68b9) SHA1(4779e150518b9014c2154f33d38767c6a7447334) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gc06.bin",    0x00000, 0x10000, CRC(e0e6c0f4) SHA1(5a8b29752c58ea76d9c7961c5b0d8c94f35037af) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mae02.bin", 0x000000, 0x80000, CRC(a715cca0) SHA1(0539bba39c60324d85599ac69ff78bb215deb511) ) /* chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
  	ROM_LOAD16_BYTE( "mae00.bin", 0x000001, 0x80000, CRC(30043094) SHA1(5302cfd9bdaf90c4901fda75407379c4ce1cbdec) ) /* sprites */
	ROM_LOAD16_BYTE( "mae01.bin", 0x000000, 0x80000, CRC(434af3fb) SHA1(1cfd30d14f03554e826576d6d32ce424f0df3748) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "gc05.bin",    0x00000, 0x20000, CRC(2f2246ff) SHA1(3fcceb6f5aa5f33187bcf4c59d88327f396fa80d) )
ROM_END

ROM_START( supbtimj )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "gc03.bin", 0x00000, 0x20000, CRC(b5621f6a) SHA1(2dfd361e81dc4805bc248cc293d94131162df2d2) )
	ROM_LOAD16_BYTE( "gc04.bin", 0x00001, 0x20000, CRC(551b2a0c) SHA1(8a6dde2d64029b8e7f7c9b88bd05633b69417dc1) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gc06.bin",    0x00000, 0x10000, CRC(e0e6c0f4) SHA1(5a8b29752c58ea76d9c7961c5b0d8c94f35037af) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mae02.bin", 0x000000, 0x80000, CRC(a715cca0) SHA1(0539bba39c60324d85599ac69ff78bb215deb511) ) /* chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
  	ROM_LOAD16_BYTE( "mae00.bin", 0x000001, 0x80000, CRC(30043094) SHA1(5302cfd9bdaf90c4901fda75407379c4ce1cbdec) ) /* sprites */
	ROM_LOAD16_BYTE( "mae01.bin", 0x000000, 0x80000, CRC(434af3fb) SHA1(1cfd30d14f03554e826576d6d32ce424f0df3748) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "gc05.bin",    0x00000, 0x20000, CRC(2f2246ff) SHA1(3fcceb6f5aa5f33187bcf4c59d88327f396fa80d) )
ROM_END

ROM_START( chinatwn )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "gv_00-.f11", 0x00000, 0x20000, CRC(2ea7ea5d) SHA1(3d0eb63f3af00bcf10ba7416dd26b366578006bf) )
	ROM_LOAD16_BYTE( "gv_01-.f13", 0x00001, 0x20000, CRC(bcab03c7) SHA1(cd6c1ad26a0867482565a0544ea1870012cabf34) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "gv_02-.f16",    0x00000, 0x10000, CRC(95151d84) SHA1(9f49e49f966c3fc460773b187a110073eb595880) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "mak-02.h2", 0x000000, 0x80000, CRC(745b2c50) SHA1(557ac71da170a04caaab393dc43e46858ef8dd70) ) /* chars */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
  	ROM_LOAD16_BYTE( "mak-00.a2", 0x000001, 0x80000, CRC(18e8cc1b) SHA1(afa79557222a94de7d9fde526ca45796f74fb3b2) ) /* sprites */
	ROM_LOAD16_BYTE( "mak-01.a4", 0x000000, 0x80000, CRC(d88ebda8) SHA1(ec6eab95f3ca8ee946151c46c6570b0b0c508ffc) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
  	ROM_LOAD( "gv_03-.j14",    0x00000, 0x20000, CRC(948faf92) SHA1(2538c7d4fa7fe0bfdd5dccece8ee82e911cee63f) )
ROM_END

/******************************************************************************/

GAME( 1990, supbtime, 0,        supbtime, supbtime, 0, ROT0, "Data East Corporation", "Super Burger Time (World, set 1)", 0 )
GAME( 1990, supbtima, supbtime, supbtime, supbtime, 0, ROT0, "Data East Corporation", "Super Burger Time (World, set 2)", 0 )
GAME( 1990, supbtimj, supbtime, supbtime, supbtime, 0, ROT0, "Data East Corporation", "Super Burger Time (Japan)", 0 )
GAME( 1991, chinatwn, 0,        chinatwn, chinatwn, 0, ROT0, "Data East Corporation", "China Town (Japan)", 0 )
