/******************************************************************************

  AMERICAN MUSIC POKER V1.4
  1987 AMUSCO.

  Preliminary driver by Roberto Fresca.

*******************************************************************************

  Hardware Notes:

  1x Empty 40 pin socket (U57)    Suspected 8085/8086/8088 Processor (due to the D8259AC presence).

  1x NEC D8259AC          Programmable Interrupt Controller (PIC).
  1x AMD D8284A           Clock Generator and Driver for 8086/8088 Processors.
  1x M5L8253P-5           Programmable general-purpose timer device.

  1x MOS 6845 (U27)       CRT Controller.
  2x P8255                Programmable Peripheral Interface (I/O).

  2x SRM2264		8k X 8 CMOS Static RAM.

  1x 27128 (U35) ROM.	Handwritten sticker:  Char A U35.
  1x 27128 (U36) ROM.	Handwritten sticker:  Char B U36.
  1x 27128 (U37) ROM.	Handwritten sticker:  Char C U37.
  1x 27256 (U42) ROM.	Handwritten sticker:  PK V1.4 U42.

  1x TI SN76489		Digital Complex Sound Generator (DCSG).

  3x MMI PAL16L8ACN (U47, U48, U50)
  1x MMI PAL16R4 (U49)    <-- couldn't get a consistent read

  22.1184 MHz. Crystal
  15.000  MHz. Crystal

*****************************************************************************************

  DRIVER UPDATES:

  [2014-03-14]

  - Initial release.
  - Decoded graphics.
  - Preliminary memory map.
  - Added the CRTC 6845.
  - Added the SN76489 PSG.
  - Added technical notes.


  TODO:

  - Proper tile colors.
  - Hook PPI8255 devices.
  - I/O.

*******************************************************************************/


#define MASTER_CLOCK		XTAL_22_1184MHz		/* confirmed */
#define SECOND_CLOCK		XTAL_15MHz			/* confirmed */

#define CPU_CLOCK			MASTER_CLOCK / 4	/* guess */
#define CRTC_CLOCK			SECOND_CLOCK / 8	/* guess */
#define SND_CLOCK			SECOND_CLOCK / 8	/* guess */


#include "emu.h"
#include "cpu/i86/i86.h"
#include "video/mc6845.h"
#include "machine/i8255.h"
#include "sound/sn76496.h"


class amusco_state : public driver_device
{
public:
	amusco_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode") { }

	required_shared_ptr<UINT16> m_videoram;
	tilemap_t *m_bg_tilemap;
	DECLARE_WRITE16_MEMBER(amusco_videoram_w);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	virtual void video_start();
	DECLARE_PALETTE_INIT(amusco);
	UINT32 screen_update_amusco(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(amusco_interrupt);
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	INTERRUPT_GEN_MEMBER(amusco_irq);
};


/*************************
*     Video Hardware     *
*************************/

WRITE16_MEMBER(amusco_state::amusco_videoram_w)
{
	m_videoram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}


TILE_GET_INFO_MEMBER(amusco_state::get_bg_tile_info)
{
/*  - bits -
    7654 3210
    ---- ----   bank select.
    ---- ----   color code.
    ---- ----   seems unused.
*/
	int code = m_videoram[tile_index];

	SET_TILE_INFO_MEMBER(
							0 /* bank */,
							code,
							0 /* color */,
							0
						);
}

void amusco_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(m_gfxdecode, tilemap_get_info_delegate(FUNC(amusco_state::get_bg_tile_info),this), TILEMAP_SCAN_ROWS, 8, 10, 74, 24);
}

UINT32 amusco_state::screen_update_amusco(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	return 0;
}

PALETTE_INIT_MEMBER(amusco_state, amusco)
{
}


/**************************
*  Read / Write Handlers  *
**************************/


/*************************
* Memory Map Information *
*************************/

static ADDRESS_MAP_START( amusco_mem_map, AS_PROGRAM, 16, amusco_state )
	AM_RANGE(0x0c000, 0x0c3ff) AM_WRITE(amusco_videoram_w) AM_SHARE("videoram")	// placeholder
	AM_RANGE(0xf8000, 0xfffff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( amusco_io_map, AS_IO, 16, amusco_state )
	AM_RANGE(0x0000, 0x0001) AM_DEVREADWRITE8("crtc", mc6845_device, status_r,   address_w,  0x00ff)
	AM_RANGE(0x0000, 0x0001) AM_DEVREADWRITE8("crtc", mc6845_device, register_r, register_w, 0xff00)
	AM_RANGE(0x0060, 0x0061) AM_DEVWRITE8("sn", sn76489_device, write, 0x00ff)								/* sound */
//	AM_RANGE(0x0010, 0x0011) AM_READ_PORT("IN1")
//	AM_RANGE(0x0012, 0x0013) AM_READ_PORT("IN3")
//	AM_RANGE(0x0014, 0x0015) AM_READ_PORT("IN3")
//	AM_RANGE(0x0016, 0x0017) AM_READ_PORT("IN3")
ADDRESS_MAP_END

/* I/O byte R/W

  0000 writes CRTC register (high nibble)    screen size: 88*8 27*10  -  visible scr: 74*8 24*10
  0000 writes CRTC address (low nibble)      reg values: 57, 4a, 4b, 0b, 1a, 08, 18, 19, 48, 09, 40, 00, 00, 00, 00, 00, 00.
  
   -----------------

   unknown writes:


*/

/*************************
*      Input Ports       *
*************************/

static INPUT_PORTS_START( amusco )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_1) PORT_NAME("IN0-1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_2) PORT_NAME("IN0-2")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_3) PORT_NAME("IN0-3")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_4) PORT_NAME("IN0-4")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_5) PORT_NAME("IN0-5")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_6) PORT_NAME("IN0-6")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_7) PORT_NAME("IN0-7")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_8) PORT_NAME("IN0-8")

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Q) PORT_NAME("IN1-1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_W) PORT_NAME("IN1-2")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_E) PORT_NAME("IN1-3")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_R) PORT_NAME("IN1-4")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_T) PORT_NAME("IN1-5")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Y) PORT_NAME("IN1-6")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_U) PORT_NAME("IN1-7")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_I) PORT_NAME("IN1-8")

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_A) PORT_NAME("IN2-1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_S) PORT_NAME("IN2-2")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_D) PORT_NAME("IN2-3")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_F) PORT_NAME("IN2-4")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_G) PORT_NAME("IN2-5")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_H) PORT_NAME("IN2-6")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_J) PORT_NAME("IN2-7")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_K) PORT_NAME("IN2-8")

	PORT_START("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Z) PORT_NAME("IN3-1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_X) PORT_NAME("IN3-2")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_C) PORT_NAME("IN3-3")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_V) PORT_NAME("IN3-4")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_B) PORT_NAME("IN3-5")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_N) PORT_NAME("IN3-6")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_M) PORT_NAME("IN3-7")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_L) PORT_NAME("IN3-8")

	PORT_START("IN4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("IN4-1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("IN4-2")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("IN4-3")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("IN4-4")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("IN4-5")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("IN4-6")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_7_PAD) PORT_NAME("IN4-7")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_8_PAD) PORT_NAME("IN4-8")

	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW4")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

INPUT_PORTS_END


/*************************
*    Graphics Layouts    *
*************************/

static const gfx_layout charlayout =
{
	8, 10,
	RGN_FRAC(1,3),
	3,
	{ 0, RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 7, 6, 5, 4, 3, 2, 1, 0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8 },
	16*8
};


/******************************
* Graphics Decode Information *
******************************/

static GFXDECODE_START( amusco )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 0, 8 )
GFXDECODE_END


/***********************
*    CRTC Interface    *
************************/

static MC6845_INTERFACE( mc6845_intf )
{
	false,      /* show border area */
	0,0,0,0,    /* visarea adjustment */
	8,          /* number of pixels per video memory address */
	NULL,       /* before pixel update callback */
	NULL,       /* row update callback */
	NULL,       /* after pixel update callback */
	DEVCB_NULL, /* callback for display state changes */
	DEVCB_NULL, /* callback for cursor state changes */
	DEVCB_NULL, /* HSYNC callback */
	DEVCB_NULL, /* VSYNC callback */
	NULL        /* update address callback */
};


INTERRUPT_GEN_MEMBER(amusco_state::amusco_irq)
{
		device.execute().set_input_line(INPUT_LINE_NMI, PULSE_LINE);
}


/*************************
*    Machine Drivers     *
*************************/

static MACHINE_CONFIG_START( amusco, amusco_state )

	/* basic machine hardware */
	/* FIXME: Need to find the correct CPU */
	MCFG_CPU_ADD("maincpu", I8086, CPU_CLOCK)        // 5 MHz ?
	MCFG_CPU_PROGRAM_MAP(amusco_mem_map)
	MCFG_CPU_IO_MAP(amusco_io_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", amusco_state, amusco_irq)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(88*8, 27*10)							// screen size: 88*8 27*10 
	MCFG_SCREEN_VISIBLE_AREA(0*8, 74*8-1, 0*10, 24*10-1)	// visible scr: 74*8 24*10
	MCFG_SCREEN_UPDATE_DRIVER(amusco_state, screen_update_amusco)

	MCFG_SCREEN_PALETTE("palette")
	MCFG_GFXDECODE_ADD("gfxdecode", "palette", amusco)
	MCFG_PALETTE_ADD("palette", 8)
	MCFG_PALETTE_INIT_OWNER(amusco_state, amusco)

	MCFG_MC6845_ADD("crtc", MC6845, "screen", CRTC_CLOCK, mc6845_intf) /* guess */

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("sn", SN76489, SND_CLOCK)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

MACHINE_CONFIG_END


/*************************
*        Rom Load        *
*************************/

ROM_START( amusco )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD( "pk_v1.4_u42.u42",  0xf8000, 0x08000, CRC(bf57d7b1) SHA1(fc8b062b12c241c6c096325f728305316b80be8b) )

	ROM_REGION( 0xc000, "gfx1", 0 )
	ROM_LOAD( "char_a_u35.u35",  0x0000, 0x4000, CRC(ded67ef6) SHA1(da7326c190211e956e5a5f763d5045615bb8ffb3) )
	ROM_LOAD( "char_b_u36.u36",  0x4000, 0x4000, CRC(55523513) SHA1(97fd221c298698628d4f6564389d96eb89e55927) )
	ROM_LOAD( "char_c_u37.u37",  0x8000, 0x4000, CRC(d26f3b94) SHA1(e58af4f6f1a9091c23827997d03b91f02bb07856) )
//	ROM_LOAD( "char_a_u35.u35",  0x8000, 0x4000, CRC(ded67ef6) SHA1(da7326c190211e956e5a5f763d5045615bb8ffb3) )
//	ROM_LOAD( "char_b_u36.u36",  0x4000, 0x4000, CRC(55523513) SHA1(97fd221c298698628d4f6564389d96eb89e55927) )
//	ROM_LOAD( "char_c_u37.u37",  0x0000, 0x4000, CRC(d26f3b94) SHA1(e58af4f6f1a9091c23827997d03b91f02bb07856) )

	ROM_REGION( 0x0800, "plds", 0 )
	ROM_LOAD( "pal16l8a.u47", 0x0000, 0x0104, CRC(554b4286) SHA1(26bc991f2cc58644cd2d9ce5c1867a94455b95a8) )
	ROM_LOAD( "pal16l8a.u48", 0x0200, 0x0104, CRC(d8d1fb4b) SHA1(7a722420324d7efbe500279cbff6e08b7eeb4f22) )
	ROM_LOAD( "pal16r4a.u49", 0x0400, 0x0104, CRC(97813a68) SHA1(be4c7f2d38b7c5eec13dd803b78293d8e5f1c2ff) )
	ROM_LOAD( "pal16l8a.u50", 0x0600, 0x0104, CRC(f5d80001) SHA1(ba0e55ebb45eceec256d432aee6d4123365a0af2) )
ROM_END


/*************************
*      Game Drivers      *
*************************/

/*    YEAR  NAME      PARENT  MACHINE   INPUT     STATE          INIT  ROT    COMPANY   FULLNAME                      FLAGS */
GAME( 1987, amusco,   0,      amusco,   amusco,   driver_device, 0,    ROT0, "Amusco", "American Music Poker (V1.4)", GAME_NOT_WORKING )
