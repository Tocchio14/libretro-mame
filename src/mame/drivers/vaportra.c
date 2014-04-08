/***************************************************************************

  Vapor Trail (World version)  (c) 1989 Data East Corporation
  Vapor Trail (USA version)    (c) 1989 Data East USA
  Kuhga (Japanese version)     (c) 1989 Data East Corporation

  Emulation by Bryan McPhail, mish@tendril.co.uk
  added pal & prom-maps - Highwayman.
***************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "cpu/h6280/h6280.h"
#include "sound/2203intf.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"
#include "includes/vaportra.h"

/******************************************************************************/

WRITE16_MEMBER(vaportra_state::vaportra_sound_w)
{
	/* Force synchronisation between CPUs with fake timer */
	machine().scheduler().synchronize();
	soundlatch_byte_w(space, 0, data & 0xff);
	m_audiocpu->set_input_line(0, ASSERT_LINE);
}

READ16_MEMBER(vaportra_state::vaportra_control_r)
{
	switch (offset << 1)
	{
		case 4:
			return ioport("DSW")->read();
		case 2:
			return ioport("COINS")->read();
		case 0:
			return ioport("PLAYERS")->read();
	}

	logerror("Unknown control read at %d\n",offset);
	return ~0;
}

/******************************************************************************/

static ADDRESS_MAP_START( main_map, AS_PROGRAM, 16, vaportra_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x100000, 0x100003) AM_WRITE(vaportra_priority_w)
	AM_RANGE(0x100006, 0x100007) AM_WRITE(vaportra_sound_w)
	AM_RANGE(0x100000, 0x10000f) AM_READ(vaportra_control_r)
	AM_RANGE(0x200000, 0x201fff) AM_DEVREADWRITE("tilegen2", deco16ic_device, pf1_data_r, pf1_data_w)
	AM_RANGE(0x202000, 0x203fff) AM_DEVREADWRITE("tilegen2", deco16ic_device, pf2_data_r, pf2_data_w)
	AM_RANGE(0x240000, 0x24000f) AM_DEVWRITE("tilegen2", deco16ic_device, pf_control_w)
	AM_RANGE(0x280000, 0x281fff) AM_DEVREADWRITE("tilegen1", deco16ic_device, pf1_data_r, pf1_data_w)
	AM_RANGE(0x282000, 0x283fff) AM_DEVREADWRITE("tilegen1", deco16ic_device, pf2_data_r, pf2_data_w)
	AM_RANGE(0x2c0000, 0x2c000f) AM_DEVWRITE("tilegen1", deco16ic_device, pf_control_w)
	AM_RANGE(0x300000, 0x3009ff) AM_RAM_WRITE(vaportra_palette_24bit_rg_w) AM_SHARE("paletteram")
	AM_RANGE(0x304000, 0x3049ff) AM_RAM_WRITE(vaportra_palette_24bit_b_w) AM_SHARE("paletteram2")
	AM_RANGE(0x308000, 0x308001) AM_NOP
	AM_RANGE(0x30c000, 0x30c001) AM_DEVWRITE("spriteram", buffered_spriteram16_device, write)
	AM_RANGE(0xff8000, 0xff87ff) AM_RAM AM_SHARE("spriteram")
	AM_RANGE(0xffc000, 0xffffff) AM_RAM
ADDRESS_MAP_END


/******************************************************************************/

READ8_MEMBER(vaportra_state::vaportra_soundlatch_r)
{
	m_audiocpu->set_input_line(0, CLEAR_LINE);
	return soundlatch_byte_r(space, offset);
}

static ADDRESS_MAP_START( sound_map, AS_PROGRAM, 8, vaportra_state )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x100000, 0x100001) AM_DEVREADWRITE("ym1", ym2203_device, read, write)
	AM_RANGE(0x110000, 0x110001) AM_DEVREADWRITE("ym2", ym2151_device, read, write)
	AM_RANGE(0x120000, 0x120001) AM_DEVREADWRITE("oki1", okim6295_device, read, write)
	AM_RANGE(0x130000, 0x130001) AM_DEVREADWRITE("oki2", okim6295_device, read, write)
	AM_RANGE(0x140000, 0x140001) AM_READ(vaportra_soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_RAMBANK("bank8")  /* ??? LOOKUP ??? */
	AM_RANGE(0x1fec00, 0x1fec01) AM_DEVWRITE("audiocpu", h6280_device, timer_w)
	AM_RANGE(0x1ff400, 0x1ff403) AM_DEVWRITE("audiocpu", h6280_device, irq_status_w)
ADDRESS_MAP_END

/******************************************************************************/

static INPUT_PORTS_START( vaportra )
	PORT_START("PLAYERS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("COINS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPSETTING(      0x0003, "3" )
	PORT_DIPSETTING(      0x0002, "4" )
	PORT_DIPSETTING(      0x0001, "5" )
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0030, 0x0030, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(      0x0020, "150k, 300k and 600k" )
	PORT_DIPSETTING(      0x0030, "200k and 600k" )
	PORT_DIPSETTING(      0x0010, "300k only" )
	PORT_DIPSETTING(      0x0000, DEF_STR( None ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x0080, 0x0000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_DIPNAME( 0x0700, 0x0700, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0500, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x3800, 0x3800, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x3800, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x3000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x2800, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_SERVICE( 0x8000, IP_ACTIVE_LOW )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0,RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	8*16
};

static const gfx_layout tilelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0,RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0 },
	{ 256,257,258,259,260,261,262,263,0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16
};

static GFXDECODE_START( vaportra )
	GFXDECODE_ENTRY( "gfx1", 0x000000, charlayout,    0x000, 0x500 )    /* Characters 8x8 */
	GFXDECODE_ENTRY( "gfx1", 0x000000, tilelayout,    0x000, 0x500 )    /* Tiles 16x16 */
	GFXDECODE_ENTRY( "gfx2", 0x000000, charlayout,    0x000, 0x500 )    /* Characters 8x8 */
	GFXDECODE_ENTRY( "gfx2", 0x000000, tilelayout,    0x000, 0x500 )    /* Tiles 16x16 */ // ok
	GFXDECODE_ENTRY( "gfx3", 0x000000, tilelayout,    0x100, 16 )   /* Sprites 16x16 */
GFXDECODE_END

/******************************************************************************/

DECO16IC_BANK_CB_MEMBER(vaportra_state::bank_callback)
{
	return ((bank >> 4) & 0x7) * 0x1000;
}

void vaportra_state::machine_start()
{
	save_item(NAME(m_priority));
}

void vaportra_state::machine_reset()
{
	m_priority[0] = 0;
	m_priority[1] = 0;
}

static MACHINE_CONFIG_START( vaportra, vaportra_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000,12000000) /* Custom chip 59 */
	MCFG_CPU_PROGRAM_MAP(main_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", vaportra_state,  irq6_line_hold)

	MCFG_CPU_ADD("audiocpu", H6280, 32220000/4) /* Custom chip 45; Audio section crystal is 32.220 MHz */
	MCFG_CPU_PROGRAM_MAP(sound_map)


	/* video hardware */
	MCFG_BUFFERED_SPRITERAM16_ADD("spriteram")

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(58)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(vaportra_state, screen_update_vaportra)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", vaportra)
	MCFG_PALETTE_ADD("palette", 1280)


	MCFG_DEVICE_ADD("tilegen1", DECO16IC, 0)
	MCFG_DECO16IC_SPLIT(0)
	MCFG_DECO16IC_WIDTH12(1)
	MCFG_DECO16IC_PF1_TRANS_MASK(0x0f)
	MCFG_DECO16IC_PF2_TRANS_MASK(0x0f)
	MCFG_DECO16IC_PF1_COL_BANK(0x00)
	MCFG_DECO16IC_PF2_COL_BANK(0x20)
	MCFG_DECO16IC_PF1_COL_MASK(0x0f)
	MCFG_DECO16IC_PF2_COL_MASK(0x0f)
	MCFG_DECO16IC_BANK1_CB(vaportra_state, bank_callback)
	MCFG_DECO16IC_BANK2_CB(vaportra_state, bank_callback)
	MCFG_DECO16IC_PF12_8X8_BANK(0)
	MCFG_DECO16IC_PF12_16X16_BANK(1)
	MCFG_DECO16IC_GFXDECODE("gfxdecode")
	MCFG_DECO16IC_PALETTE("palette")

	MCFG_DEVICE_ADD("tilegen2", DECO16IC, 0)
	MCFG_DECO16IC_SPLIT(0)
	MCFG_DECO16IC_WIDTH12(1)
	MCFG_DECO16IC_PF1_TRANS_MASK(0x0f)
	MCFG_DECO16IC_PF2_TRANS_MASK(0x0f)
	MCFG_DECO16IC_PF1_COL_BANK(0x30)
	MCFG_DECO16IC_PF2_COL_BANK(0x40)
	MCFG_DECO16IC_PF1_COL_MASK(0x0f)
	MCFG_DECO16IC_PF2_COL_MASK(0x0f)
	MCFG_DECO16IC_BANK1_CB(vaportra_state, bank_callback)
	MCFG_DECO16IC_BANK2_CB(vaportra_state, bank_callback)
	MCFG_DECO16IC_PF12_8X8_BANK(2)
	MCFG_DECO16IC_PF12_16X16_BANK(3)
	MCFG_DECO16IC_GFXDECODE("gfxdecode")
	MCFG_DECO16IC_PALETTE("palette")

	MCFG_DEVICE_ADD("spritegen", DECO_MXC06, 0)
	deco_mxc06_device::set_gfx_region(*device, 4);
	MCFG_DECO_MXC06_GFXDECODE("gfxdecode")
	MCFG_DECO_MXC06_PALETTE("palette")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("ym1", YM2203, 32220000/8)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)

	MCFG_YM2151_ADD("ym2", 32220000/9)
	MCFG_YM2151_IRQ_HANDLER(INPUTLINE("audiocpu", 1)) /* IRQ 2 */
	MCFG_SOUND_ROUTE(0, "mono", 0.60)
	MCFG_SOUND_ROUTE(1, "mono", 0.60)

	MCFG_OKIM6295_ADD("oki1", 32220000/32, OKIM6295_PIN7_HIGH)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	MCFG_OKIM6295_ADD("oki2", 32220000/16, OKIM6295_PIN7_HIGH)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)
MACHINE_CONFIG_END

/******************************************************************************/

ROM_START( vaportra )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fl_02-1.bin", 0x00000, 0x20000, CRC(9ae36095) SHA1(c8d11a6033a44277a267915b4ca471c43acd1143) )
	ROM_LOAD16_BYTE( "fl_00-1.bin", 0x00001, 0x20000, CRC(c08cc048) SHA1(b28f95856817b8a8cb6cc588d48e95196cbf52fd) )
	ROM_LOAD16_BYTE( "fl_03.bin",   0x40000, 0x20000, CRC(80bd2844) SHA1(3fcaa409c7134388fa9458df8e8aaecc93f085e6) )
	ROM_LOAD16_BYTE( "fl_01.bin",   0x40001, 0x20000, CRC(9474b085) SHA1(5510309ddab5fbf1dbb0a7b1e424a5dff5ec263d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "vtmaa00.bin",   0x000000, 0x80000, CRC(0330e13b) SHA1(dce70667ea738295332556752d1305c5e941b383) ) /* chars & tiles */

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, "gfx3", 0 )
	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x40000, "oki1", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x40000, "oki2", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, "proms", 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )

	ROM_REGION( 0x0a00, "plds", 0 )
	ROM_LOAD( "pal16l8a.6l",  0x0000, 0x0104, CRC(ee748e8f) SHA1(6ffe8b11f076305e82f64e0a12b76ffe725ce345) )
	ROM_LOAD( "pal16l8b.13g", 0x0200, 0x0104, CRC(6da13bda) SHA1(d7bade089d87015e1e95fbf3f292db4688ee4624) )
	ROM_LOAD( "pal16l8b.13h", 0x0400, 0x0104, CRC(62a9e098) SHA1(7b7c371c040d250d41fde021d191d62ce95bfc20) )
	ROM_LOAD( "pal16l8b.14g", 0x0600, 0x0104, CRC(036768aa) SHA1(96185989031e0a9b38ff29bf4cf6162482d33964) )
	ROM_LOAD( "pal16l8b.14h", 0x0800, 0x0104, CRC(bf421fce) SHA1(e8b0895b1fe99a3d5b3dcca004a7bfd1a09766b2) )
ROM_END

ROM_START( vaportra3 ) // 74 bytes of 68k code have been changed compared to vaportra set
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fl02-3.bin", 0x00000, 0x20000, CRC(6c59be54) SHA1(ce60be53fb2cc3a26a28e8632c8638771d3db3c9) ) // == fl_02-1.bin (99.973297%)
	ROM_LOAD16_BYTE( "fl00-3.bin", 0x00001, 0x20000, CRC(69f8bef4) SHA1(7249d097c33adac9b42dd98d1328ad1c496ff927) ) // == fl_00-1.bin (99.970245%)
	ROM_LOAD16_BYTE( "fl_03.bin",   0x40000, 0x20000, CRC(80bd2844) SHA1(3fcaa409c7134388fa9458df8e8aaecc93f085e6) )
	ROM_LOAD16_BYTE( "fl_01.bin",   0x40001, 0x20000, CRC(9474b085) SHA1(5510309ddab5fbf1dbb0a7b1e424a5dff5ec263d) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, "gfx1", 0 ) // original DE board with mask rom split into 4 roms
	ROM_LOAD16_BYTE( "fl23",   0x00000, 0x20000, CRC(6089f9e7) SHA1(a036068398a8e72c8fcf29fadaaad5a8930c2bfe) )
	ROM_LOAD16_BYTE( "fl25",   0x00001, 0x20000, CRC(3989290a) SHA1(7d2d8a334d4c206298d806eac4f2cd46e7d4f918) )
	ROM_LOAD16_BYTE( "fl24",   0x40000, 0x20000, CRC(41551bfa) SHA1(bc636fec6d610f651101656d6e9ad06656ce516a) )
	ROM_LOAD16_BYTE( "fl26",   0x40001, 0x20000, CRC(dc67fa5c) SHA1(459a1ee059d6bb2fb2c6744fffeb25b915b29e67) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, "gfx3", 0 )
	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x40000, "oki1", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x40000, "oki2", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, "proms", 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )

	ROM_REGION( 0x0a00, "plds", 0 )
	ROM_LOAD( "pal16l8a.6l",  0x0000, 0x0104, CRC(ee748e8f) SHA1(6ffe8b11f076305e82f64e0a12b76ffe725ce345) )
	ROM_LOAD( "pal16l8b.13g", 0x0200, 0x0104, CRC(6da13bda) SHA1(d7bade089d87015e1e95fbf3f292db4688ee4624) )
	ROM_LOAD( "pal16l8b.13h", 0x0400, 0x0104, CRC(62a9e098) SHA1(7b7c371c040d250d41fde021d191d62ce95bfc20) )
	ROM_LOAD( "pal16l8b.14g", 0x0600, 0x0104, CRC(036768aa) SHA1(96185989031e0a9b38ff29bf4cf6162482d33964) )
	ROM_LOAD( "pal16l8b.14h", 0x0800, 0x0104, CRC(bf421fce) SHA1(e8b0895b1fe99a3d5b3dcca004a7bfd1a09766b2) )
ROM_END

ROM_START( vaportrau )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fj02",   0x00000, 0x20000, CRC(a2affb73) SHA1(0d49397cc9891047a0b92e92e2e3d0e7fcaf8db9) )
	ROM_LOAD16_BYTE( "fj00",   0x00001, 0x20000, CRC(ef05e07b) SHA1(0e505709fa251e6b30f019c0c28ee9ba2b29a50a) )
	ROM_LOAD16_BYTE( "fj03",   0x40000, 0x20000, CRC(44893379) SHA1(da1340bc1821a552c317cb9a7c1ba69eb080b055) )
	ROM_LOAD16_BYTE( "fj01",   0x40001, 0x20000, CRC(97fbc107) SHA1(b2899eb4347c0471397b83051e46c94dff3526f5) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "vtmaa00.bin",   0x000000, 0x80000, CRC(0330e13b) SHA1(dce70667ea738295332556752d1305c5e941b383) ) /* chars & tiles */

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, "gfx3", 0 )
	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x40000, "oki1", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x40000, "oki2", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, "proms", 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )

	ROM_REGION( 0x0a00, "plds", 0 )
	ROM_LOAD( "pal16l8a.6l",  0x0000, 0x0104, CRC(ee748e8f) SHA1(6ffe8b11f076305e82f64e0a12b76ffe725ce345) )
	ROM_LOAD( "pal16l8b.13g", 0x0200, 0x0104, CRC(6da13bda) SHA1(d7bade089d87015e1e95fbf3f292db4688ee4624) )
	ROM_LOAD( "pal16l8b.13h", 0x0400, 0x0104, CRC(62a9e098) SHA1(7b7c371c040d250d41fde021d191d62ce95bfc20) )
	ROM_LOAD( "pal16l8b.14g", 0x0600, 0x0104, CRC(036768aa) SHA1(96185989031e0a9b38ff29bf4cf6162482d33964) )
	ROM_LOAD( "pal16l8b.14h", 0x0800, 0x0104, CRC(bf421fce) SHA1(e8b0895b1fe99a3d5b3dcca004a7bfd1a09766b2) )
ROM_END

ROM_START( kuhga )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "fp02-3.bin", 0x00000, 0x20000, CRC(d0705ef4) SHA1(781efbf36d9dda543895e0a59cd4d72667439a93) )
	ROM_LOAD16_BYTE( "fp00-3.bin", 0x00001, 0x20000, CRC(1da92e48) SHA1(6507bd9bbc31ee03e38b82cc135aebf090902761) )
	ROM_LOAD16_BYTE( "fp03.bin",   0x40000, 0x20000, CRC(ea0da0f1) SHA1(ca40e694cb0aa0c13672c14fd4a389bc6d26cbc6) )
	ROM_LOAD16_BYTE( "fp01.bin",   0x40001, 0x20000, CRC(e3ecbe86) SHA1(382e959111ec37ad94da8fd6dcefe2d2aab346b6) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "vtmaa00.bin",   0x000000, 0x80000, CRC(0330e13b) SHA1(dce70667ea738295332556752d1305c5e941b383) ) /* chars & tiles */

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, "gfx3", 0 )
	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x40000, "oki1", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x40000, "oki2", 0 )    /* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, "proms", 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )
ROM_END

/*
Pals
----
Fuse Plot - 1
?

PAL16L8/A/A-2/A-4
*
DD PAL16L8/A/A-2/A-4*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    10111011101110111111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   10101011101101111111111111111111*
L320   00000000000000000000000000000000*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   10011011101101111111111111111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   01011011101101111111111111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  01101011101101111111111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  10100111101101111111111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  11111111111111111011111110111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111111111111111111111111111111*
L1824  11111111111111111111101110111111*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3E44*
?

Fuse Plot - TD0
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    10101111111111111111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11111111111110111111111111111111*
L320   11111111111111111011111111111111*
L352   11111111111111111111101111111111*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11111111111111111111011111101111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   11111111111111111111101111101111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  10011111101111111111111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  01011111101111111111111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  01101111101111111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111111111111111111111111111111*
L1824  10100111111111111111111111111111*
L1856  11111111111111111111111110111011*
L1888  10011111101111111111111111111111*
L1920  01011111101111111111111111111111*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C52BB*
?

Fuse Plot - TD1
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    10111011111111111111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00100000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11101111101110111111111111111111*
L320   00000000000000000000000000000000*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11101111011110111111111111111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00100000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   10111111101110111111111111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  10111111011110111111111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  11100111111111111111111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  10110111111111111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111111111111111111111111111111*
L1824  11101011111111111111111111111111*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3EBA*
?

Fuse Plot - TD2
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    11110111011110110111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11111011011110110111111111111111*
L320   00000000000000000000000000000000*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11111011101110111011111111111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   11110111101110111011111111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  11111011011110111011111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  11110111011110111011111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111011101101111011111111111111*
L1568  11101111111111111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111011101101111011111111111111*
L1824  10111111111111111111111111111111*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3DEC*
?

Fuse Plot - TD3
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    11111010111111111111111111111111*
L64    11111101111111111111111111111111*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11111011111111111111111111111111*
L320   10111111111111111111111111111111*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11111110111111111111111110111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   11111110111111111111101111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  11111011111111111011111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  10111111111111111011111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  10111111111110111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  00000000000000000000000000000000*
L1824  00000000000000000000000000000000*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3D54*
?

*/
/******************************************************************************/

DRIVER_INIT_MEMBER(vaportra_state,vaportra)
{
	UINT8 *RAM = memregion("maincpu")->base();
	int i;

	for (i = 0x00000; i < 0x80000; i++)
		RAM[i] = (RAM[i] & 0x7e) | ((RAM[i] & 0x01) << 7) | ((RAM[i] & 0x80) >> 7);
}

/******************************************************************************/

GAME( 1989, vaportra, 0,        vaportra, vaportra, vaportra_state, vaportra, ROT270, "Data East Corporation", "Vapor Trail - Hyper Offence Formation (World revision 1)", GAME_SUPPORTS_SAVE )
GAME( 1989, vaportra3,vaportra, vaportra, vaportra, vaportra_state, vaportra, ROT270, "Data East Corporation", "Vapor Trail - Hyper Offence Formation (World revision 3?)", GAME_SUPPORTS_SAVE )
GAME( 1989, vaportrau,vaportra, vaportra, vaportra, vaportra_state, vaportra, ROT270, "Data East USA", "Vapor Trail - Hyper Offence Formation (US)", GAME_SUPPORTS_SAVE )
GAME( 1989, kuhga,    vaportra, vaportra, vaportra, vaportra_state, vaportra, ROT270, "Data East Corporation", "Kuhga - Operation Code 'Vapor Trail' (Japan revision 3)", GAME_SUPPORTS_SAVE )
