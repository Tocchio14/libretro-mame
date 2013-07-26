/***************************************************************************

3x3 Puzzle
Ace Enterprise

Driver by Mariusz Wojcieszek, David Haywood and Stephh

Typical and simple Korean hardware.....

68000 @ 10MHz
OKI M6295 @ 1MHz (4/4)
Lattice pLSI1032 for graphics generation
6264 SRAM x 2
6116 SRAM x 8
62256 SRAM x2 (main program RAM)
DIP SW 8-position X2 (SW1 SW2)
OSC 27MHz, 22.1184MHz, 10MHz
XTAL 4MHz

Additional ROMs with 'a' in label are probably to convert
the game back to normal version as current set on the PCB
has adult graphics (sets provided are 'Normal' and 'Enterprise')


***************************************************************************/


#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "sound/okim6295.h"

#define MAIN_CLOCK XTAL_10MHz

class _3x3puzzle_state : public driver_device
{
public:
	_3x3puzzle_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_videoram1(*this, "videoram1"),
			m_videoram2(*this, "videoram2"),
			m_videoram3(*this, "videoram3"),
			m_maincpu(*this, "maincpu"),
			m_oki(*this, "oki")
	{ }

	/* memory pointers */
	required_shared_ptr<UINT16> m_videoram1;
	required_shared_ptr<UINT16> m_videoram2;
	required_shared_ptr<UINT16> m_videoram3;

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<okim6295_device> m_oki;

	// screen updates
	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	/* video-related */
	tilemap_t   *m_tilemap1;
	tilemap_t   *m_tilemap2;
	tilemap_t   *m_tilemap3;

	DECLARE_WRITE16_MEMBER(videoram1_w);
	TILE_GET_INFO_MEMBER(get_tile1_info);
	DECLARE_WRITE16_MEMBER(videoram2_w);
	TILE_GET_INFO_MEMBER(get_tile2_info);
	DECLARE_WRITE16_MEMBER(videoram3_w);
	TILE_GET_INFO_MEMBER(get_tile3_info);

	int       m_oki_bank;

	DECLARE_WRITE16_HANDLER(gfx_ctrl_w);
	DECLARE_WRITE16_HANDLER(tilemap1_scrollx_w);
	DECLARE_WRITE16_HANDLER(tilemap1_scrolly_w);


protected:
	virtual void video_start();
	virtual void machine_start();
	virtual void machine_reset();
};

WRITE16_MEMBER(_3x3puzzle_state::videoram1_w)
{
	COMBINE_DATA(&m_videoram1[offset]);
	m_tilemap1->mark_tile_dirty(offset);
}

TILE_GET_INFO_MEMBER(_3x3puzzle_state::get_tile1_info)
{
	UINT16 code = m_videoram1[tile_index];
	SET_TILE_INFO_MEMBER(
			0,
			code,
			0,
			0);
}

WRITE16_MEMBER(_3x3puzzle_state::videoram2_w)
{
	COMBINE_DATA(&m_videoram2[offset]);
	m_tilemap2->mark_tile_dirty(offset);
}

TILE_GET_INFO_MEMBER(_3x3puzzle_state::get_tile2_info)
{
	UINT16 code = m_videoram2[tile_index];
	SET_TILE_INFO_MEMBER(
			1,
			code,
			1,
			0);
}

WRITE16_MEMBER(_3x3puzzle_state::videoram3_w)
{
	COMBINE_DATA(&m_videoram3[offset]);
	m_tilemap3->mark_tile_dirty(offset);
}

TILE_GET_INFO_MEMBER(_3x3puzzle_state::get_tile3_info)
{
	UINT16 code = m_videoram3[tile_index];
	SET_TILE_INFO_MEMBER(
			2,
			code,
			2,
			0);
}

WRITE16_MEMBER(_3x3puzzle_state::gfx_ctrl_w)
{
	// bit 5 (0x20) cleared when palette is written
	// bit 4 (0x10) screen width - 1: 512 pixels, 0: 320 pixels
	// bit 3 (0x08) is set when tilemap scroll registers are written
	// bit 1 (0x02) OKI banking

	if ( BIT(data,4) )
	{
		machine().primary_screen->set_visible_area(0*8, 64*8-1, 0*8, 30*8-1);
	}
	else
	{
		machine().primary_screen->set_visible_area(0*8, 40*8-1, 0*8, 30*8-1);
	}

	if ( BIT(data, 1) != m_oki_bank )
	{
		m_oki_bank = BIT(data,1);
		m_oki->set_bank_base(m_oki_bank * 0x40000);
	}
}

WRITE16_MEMBER(_3x3puzzle_state::tilemap1_scrollx_w)
{
	m_tilemap1->set_scrollx(data);
};

WRITE16_MEMBER(_3x3puzzle_state::tilemap1_scrolly_w)
{
	m_tilemap1->set_scrolly(data);
}

void _3x3puzzle_state::video_start()
{
	m_tilemap1 = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(_3x3puzzle_state::get_tile1_info),this), TILEMAP_SCAN_ROWS, 16, 16, 32, 32);
	m_tilemap2 = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(_3x3puzzle_state::get_tile2_info),this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);
	m_tilemap3 = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(_3x3puzzle_state::get_tile3_info),this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);
	m_tilemap2->set_transparent_pen(0);
	m_tilemap3->set_transparent_pen(0);
}

UINT32 _3x3puzzle_state::screen_update( screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	m_tilemap1->draw(screen, bitmap, cliprect, 0, 1);
	m_tilemap2->draw(screen, bitmap, cliprect, 0, 2);
	m_tilemap3->draw(screen, bitmap, cliprect, 0, 3);
	return 0;
}

static ADDRESS_MAP_START( _3x3puzzle_map, AS_PROGRAM, 16, _3x3puzzle_state )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM
	AM_RANGE(0x100000, 0x10ffff) AM_RAM
	AM_RANGE(0x200000, 0x2007ff) AM_WRITE(videoram1_w) AM_SHARE("videoram1")
	AM_RANGE(0x201000, 0x201fff) AM_WRITE(videoram2_w) AM_SHARE("videoram2")
	AM_RANGE(0x202000, 0x202fff) AM_WRITE(videoram3_w) AM_SHARE("videoram3")
	AM_RANGE(0x280000, 0x280001) AM_READ_PORT("VBLANK")
	AM_RANGE(0x300000, 0x3005ff) AM_RAM_WRITE(paletteram_xBBBBBGGGGGRRRRR_word_w) AM_SHARE("paletteram")
	AM_RANGE(0x400000, 0x400001) AM_WRITE(tilemap1_scrollx_w)
	AM_RANGE(0x480000, 0x480001) AM_WRITE(tilemap1_scrolly_w)
	AM_RANGE(0x500000, 0x500001) AM_READ_PORT("P1")
	AM_RANGE(0x580000, 0x580001) AM_READ_PORT("SYS")
	AM_RANGE(0x600000, 0x600001) AM_READ_PORT("DSW01")
	AM_RANGE(0x700000, 0x700001) AM_DEVREADWRITE8("oki", okim6295_device, read, write, 0x00ff)
	AM_RANGE(0x800000, 0x800001) AM_WRITE(gfx_ctrl_w)
	AM_RANGE(0x880000, 0x880001) AM_READNOP // read, but no tested afterwards
ADDRESS_MAP_END

static INPUT_PORTS_START( _3x3puzzle )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("SYS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN ) // must be active_low or coins won't work
	PORT_BIT( 0xfff0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("VBLANK")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")

	PORT_START("DSW01")
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPNAME( 0x0400, 0x0000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1800, 0x1800, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x1800, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Easiest ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, "Free Play / Debug mode" )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout tiles16x16x8_layout =
{
	16,16,
	RGN_FRAC(1,1),
	8,
	{ 0,1, 2,3, 4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128, 8*128, 9*128,10*128,11*128,12*128,13*128,14*128,15*128 },
	128*16
};

static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	8 ,
	{ 0,1,2,3,4,5,6,7 },
	{ 3*8,2*8,1*8,0*8,7*8,6*8,5*8,4*8 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64 },
	8*64
};


static GFXDECODE_START( _3x3puzzle )
	GFXDECODE_ENTRY( "gfx1", 0, tiles16x16x8_layout,     0, 3 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles8x8_layout,     0, 3 )
	GFXDECODE_ENTRY( "gfx3", 0, tiles8x8_layout,     0, 3 )
GFXDECODE_END

void _3x3puzzle_state::machine_start()
{
	save_item(NAME(m_oki_bank));
}

void _3x3puzzle_state::machine_reset()
{
	m_oki_bank = 0;
}


static MACHINE_CONFIG_START( _3x3puzzle, _3x3puzzle_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",M68000,MAIN_CLOCK)
	MCFG_CPU_PROGRAM_MAP(_3x3puzzle_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", _3x3puzzle_state,  irq4_line_hold)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_UPDATE_DRIVER(_3x3puzzle_state, screen_update)
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 30*8-1)

	MCFG_GFXDECODE(_3x3puzzle)

	MCFG_PALETTE_LENGTH(0x600/2)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_OKIM6295_ADD("oki", XTAL_4MHz/4, OKIM6295_PIN7_HIGH)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( 3x3puzzl )
	ROM_REGION( 0x400000, "maincpu", 0 )    /* 68000 code */
	ROM_LOAD16_BYTE( "1.bin",    0x000000, 0x20000, CRC(e9c39ee7) SHA1(8557eeaff33ac8e11fd545482bd9e48f9a58eba3) )
	ROM_LOAD16_BYTE( "2.bin",    0x000001, 0x20000, CRC(524963be) SHA1(05428ccc823c35b6c4d182a1dff1c9aa6b71e616) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD32_BYTE("3.bin", 0x000000, 0x080000, CRC(53c2aa6a) SHA1(d2ebb10eb8311ff5226793e7b373e152b21c602c) )
	ROM_LOAD32_BYTE("4.bin", 0x000001, 0x080000, CRC(fb0b76fd) SHA1(23c9a5979452c21381107641d5cd49b34ad00471) )
	ROM_LOAD32_BYTE("5.bin", 0x000002, 0x080000, CRC(b6c1e108) SHA1(44c97e0582b9ee85465040d56eda9efd06c25533) )
	ROM_LOAD32_BYTE("6.bin", 0x000003, 0x080000, CRC(47cb0e8e) SHA1(13d39b1eb4fcfeafc8d821871750f37377449b80) )

	ROM_REGION( 0x080000, "gfx2", ROMREGION_ERASE00 )
	ROM_LOAD32_BYTE("7.bin",  0x000000, 0x020000, CRC(45b1f58b) SHA1(7faee993b87ef14eef1c5ba3fcc7d0747494dbf5) )
	ROM_LOAD32_BYTE("8.bin",  0x000001, 0x020000, CRC(c0d404a7) SHA1(7991e711f6933bc3809f3c562e21a775a9a2dcf3) )
	ROM_LOAD32_BYTE("9.bin",  0x000002, 0x020000, CRC(6b303aa9) SHA1(1750da9148978f59904c2bf9e99f967e5bdd5a92) )
	ROM_LOAD32_BYTE("10.bin", 0x000003, 0x020000, CRC(6d0107bc) SHA1(a6f30a482586136304af510eee0a93df450673bd) )

	ROM_REGION( 0x080000, "gfx3", ROMREGION_ERASE00 )
	ROM_LOAD32_BYTE("11.bin", 0x000000, 0x020000, CRC(e124c0b5) SHA1(9b819691b7ca7f0561f4fce05083c8507e138bfe) )
	ROM_LOAD32_BYTE("12.bin", 0x000001, 0x020000, CRC(ae4a8707) SHA1(f54337e4b666dc2f38df0ea96e9c8f4d7c4ebe52) )
	ROM_LOAD32_BYTE("13.bin", 0x000002, 0x020000, CRC(f06925d1) SHA1(a45b72da57c7d967dec2fcad12e6a7864f5442e8) )
	ROM_LOAD32_BYTE("14.bin", 0x000003, 0x020000, CRC(07252636) SHA1(00730c203e20af9e18a792e26de7aeac5d090ebf) )

	ROM_REGION(0x80000, "oki", 0 )
	ROM_LOAD("15.bin", 0x000000, 0x080000, CRC(d3aff355) SHA1(117f7bbd6cab370f65e308d78291732dfc079365) )
ROM_END

ROM_START( 3x3puzzla )
	ROM_REGION( 0x400000, "maincpu", 0 )    /* 68000 code */
	ROM_LOAD16_BYTE( "1a.bin",    0x000000, 0x20000, CRC(425c5896) SHA1(78d709b729f160b1e20a61a795361113dbb4fb52) )
	ROM_LOAD16_BYTE( "2a.bin",    0x000001, 0x20000, CRC(4db710b7) SHA1(df7a3496aac9cfdaee4fd504d88772b07a8fdb2b) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD32_BYTE("3a.bin", 0x000000, 0x080000, CRC(33bff952) SHA1(11d06462041e7cf8aa2ae422ed74ba23f6934478) )
	ROM_LOAD32_BYTE("4a.bin", 0x000001, 0x080000, CRC(222996d8) SHA1(fb8fd45a43e78dd9700ffb46fa886f62e7c32e61) )
	ROM_LOAD32_BYTE("5a.bin", 0x000002, 0x080000, CRC(5b209844) SHA1(8a34958ecd9c26272708237028e279dd347c729f) )
	ROM_LOAD32_BYTE("6a.bin", 0x000003, 0x080000, CRC(f1136bba) SHA1(486c41bd08c4039bfe46500540fc4ae0f1497232) )

	ROM_REGION( 0x080000, "gfx2", ROMREGION_ERASE00 )
	ROM_LOAD32_BYTE("7.bin",  0x000000, 0x020000, CRC(45b1f58b) SHA1(7faee993b87ef14eef1c5ba3fcc7d0747494dbf5) )
	ROM_LOAD32_BYTE("8.bin",  0x000001, 0x020000, CRC(c0d404a7) SHA1(7991e711f6933bc3809f3c562e21a775a9a2dcf3) )
	ROM_LOAD32_BYTE("9.bin",  0x000002, 0x020000, CRC(6b303aa9) SHA1(1750da9148978f59904c2bf9e99f967e5bdd5a92) )
	ROM_LOAD32_BYTE("10.bin", 0x000003, 0x020000, CRC(6d0107bc) SHA1(a6f30a482586136304af510eee0a93df450673bd) )

	ROM_REGION( 0x080000, "gfx3", ROMREGION_ERASE00 )
	ROM_LOAD32_BYTE("11.bin", 0x000000, 0x020000, CRC(e124c0b5) SHA1(9b819691b7ca7f0561f4fce05083c8507e138bfe) )
	ROM_LOAD32_BYTE("12.bin", 0x000001, 0x020000, CRC(ae4a8707) SHA1(f54337e4b666dc2f38df0ea96e9c8f4d7c4ebe52) )
	ROM_LOAD32_BYTE("13.bin", 0x000002, 0x020000, CRC(f06925d1) SHA1(a45b72da57c7d967dec2fcad12e6a7864f5442e8) )
	ROM_LOAD32_BYTE("14.bin", 0x000003, 0x020000, CRC(07252636) SHA1(00730c203e20af9e18a792e26de7aeac5d090ebf) )

	ROM_REGION(0x80000, "oki", 0 )
	ROM_LOAD("15.bin", 0x000000, 0x080000, CRC(d3aff355) SHA1(117f7bbd6cab370f65e308d78291732dfc079365) )
ROM_END


GAME( 1998, 3x3puzzl,  0,          _3x3puzzle,  _3x3puzzle,  driver_device, 0,       ROT0, "Ace Enterprise",      "3X3 Puzzle (Enterprise)", 0 ) // 1998. 5. 28
GAME( 1998, 3x3puzzla, 3x3puzzl,   _3x3puzzle,  _3x3puzzle,  driver_device, 0,       ROT0, "Ace Enterprise",      "3X3 Puzzle (Normal)", 0 ) // 1998. 5. 28
