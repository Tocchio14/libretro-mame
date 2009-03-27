/***************************************************************************

 Super Golf
 preliminary WIP driver
 by Tomasz Slanina

 maybe vidram banking
 plenty of things unknown still

 sometimes extra bits are written to bank registers..

 its a z80 game.. i wonder if there is a palette or it should have proms
               -- the rest of the hardware makes me fear so

***************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/2203intf.h"

static tilemap *suprgolf_tilemap;
static UINT8 *suprgolf_bg_vram;
static UINT8 *suprgolf_bg_pen;
static int suprgolf_rom_bank;
static UINT8 suprgolf_bg_bank;
static UINT8 suprgolf_vreg;

static TILE_GET_INFO( get_tile_info )
{
	int code = videoram[tile_index*2]+256*(videoram[tile_index*2+1]);
	int color = videoram[tile_index*2+0x800] & 0x7f;

	SET_TILE_INFO(
		0,
		code,
		color,
		0);
}

static READ8_HANDLER( rom_bank_select_r )
{
    return suprgolf_rom_bank;
}

static WRITE8_HANDLER( rom_bank_select_w )
{
	UINT8 *region_base = memory_region(space->machine, "user1");

	suprgolf_rom_bank = data;

	//popmessage("%08x %02x",((data & 0x3f) * 0x4000),data);

	mame_printf_debug("ROM_BANK 0x8000 - %X @%X\n",data,cpu_get_previouspc(space->cpu));
	memory_set_bankptr(space->machine, 2, region_base + (data&0x3f ) * 0x4000);
}

static WRITE8_HANDLER( rom2_bank_select_w )
{
	UINT8 *region_base = memory_region(space->machine, "user2");
	mame_printf_debug("ROM_BANK 0x4000 - %X @%X\n",data,cpu_get_previouspc(space->cpu));
//  if(data == 0) data = 1; //test hack
	memory_set_bankptr(space->machine, 1, region_base + (data&0x0f ) * 0x4000);
}

static MACHINE_RESET( suprgolf )
{

}

static VIDEO_START( suprgolf )
{
	suprgolf_tilemap = tilemap_create( machine, get_tile_info,tilemap_scan_rows,8,8,32,32 );
	paletteram = auto_malloc(0x1000);
	suprgolf_bg_vram = auto_malloc(0x2000*0x20);
	suprgolf_bg_pen = auto_malloc(0x2000*0x20);

	tilemap_set_transparent_pen(suprgolf_tilemap,15);
}

/* TODO: fix this.*/
static void bg_draw(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect,UINT8 starting_offs)
{
	int x,y,trans;
	UINT32 count;

	count = starting_offs<<16;
	trans = starting_offs;

	for(y=0;y<256;y++)
	{
		for(x=0;x<512;x+=2)
		{
			UINT16 color;

			color = ((suprgolf_bg_pen[count] & 0xff)<<4) | ((suprgolf_bg_vram[count] & 0xf0)>>4);
			//color+= ((suprgolf_bg_pen[count+0x10000])<<4);
			//if((x)<video_screen_get_visible_area(machine)->max_x && ((y)+0)<video_screen_get_visible_area(machine)->max_y)
			if(x+1 < 256)
			{
				if(trans)
				{
					if((suprgolf_bg_vram[count] & 0xf0) != 0xf0)
						*BITMAP_ADDR32(bitmap, y, x+1) = machine->pens[(color & 0x7ff)];
				}
				else
					*BITMAP_ADDR32(bitmap, y, x+1) = machine->pens[(color & 0x7ff)];
			}

			color = ((suprgolf_bg_pen[count] & 0xff)<<4) | (suprgolf_bg_vram[count] & 0x0f);
			//color+= ((suprgolf_bg_pen[count+0x10000])<<4);

			//if((x+1)<video_screen_get_visible_area(screen)->max_x && ((y)+0)<video_screen_get_visible_area(screen)->max_y)
			if(x < 256)
			{
				if(trans)
				{
					if((suprgolf_bg_vram[count] & 0x0f) != 0x0f)
						*BITMAP_ADDR32(bitmap, y, x) = machine->pens[(color & 0x7ff)];
				}
				else
					*BITMAP_ADDR32(bitmap, y, x) = machine->pens[(color & 0x7ff)];
			}

			count++;
		}
	}
}

static VIDEO_UPDATE( suprgolf )
{
	bg_draw(screen->machine,bitmap,cliprect,0);
	//bg_draw(screen->machine,bitmap,cliprect,1);
	bg_draw(screen->machine,bitmap,cliprect,2); //this probably should be putted over the fg tilemap too.

	tilemap_draw(bitmap,cliprect,suprgolf_tilemap,0,0);
	return 0;
}

static UINT8 palette_switch;

static READ8_HANDLER( suprgolf_videoram_r )
{
	if(palette_switch)
		return paletteram[offset];
	else
		return videoram[offset];
}

static WRITE8_HANDLER( suprgolf_videoram_w )
{
	if(palette_switch)
	{
		int r,g,b,datax;
		paletteram[offset] = data;
		offset>>=1;
		datax=paletteram[offset*2]+256*paletteram[offset*2+1];

		b=(datax & 0x8000) ? 0 : ((datax)&0x001f)>>0;
		g=(datax & 0x8000) ? 0 : ((datax)&0x03e0)>>5;
		r=(datax & 0x8000) ? 0 : ((datax)&0x7c00)>>10;

		palette_set_color_rgb(space->machine, offset, pal5bit(r), pal5bit(g), pal5bit(b));
	}
	else
	{
		videoram[offset] = data;
		tilemap_mark_tile_dirty(suprgolf_tilemap, (offset & 0x7fe) >> 1);
	}
}

static READ8_HANDLER( suprgolf_vregs_r )
{
	return suprgolf_vreg;
}

static WRITE8_HANDLER( suprgolf_vregs_w )
{
	//bits 0,1,2 and probably 3 controls the background vram banking
	suprgolf_vreg = data;
	palette_switch = (data & 0x80);
	suprgolf_bg_bank = (data & 0x1f);

	if(data & 0x20) //TODO: understand the proper condition
		cpu_set_input_line(space->machine->cpu[0], INPUT_LINE_NMI, PULSE_LINE);
}

static UINT8 pen;

static READ8_HANDLER( suprgolf_bg_vram_r )
{
	return suprgolf_bg_vram[offset+suprgolf_bg_bank*0x2000];
}

static WRITE8_HANDLER( suprgolf_bg_vram_w )
{
	suprgolf_bg_vram[offset+suprgolf_bg_bank*0x2000] = data;
	suprgolf_bg_pen[offset+suprgolf_bg_bank*0x2000] = data ? pen : 0;
}

static WRITE8_HANDLER( suprgolf_pen_w )
{
	pen = data;
}

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4000, 0x7fff) AM_ROMBANK(1)
	AM_RANGE(0x4000, 0x4000) AM_WRITE( rom2_bank_select_w )
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(2)
	AM_RANGE(0xc000, 0xdfff) AM_READWRITE( suprgolf_bg_vram_r, suprgolf_bg_vram_w ) // banked background vram
	AM_RANGE(0xe000, 0xefff) AM_READWRITE( suprgolf_videoram_r, suprgolf_videoram_w ) AM_BASE(&videoram) //foreground vram + paletteram
	AM_RANGE(0xf000, 0xf000) AM_WRITE( suprgolf_pen_w )
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READ_PORT("P1")
	AM_RANGE(0x01, 0x01) AM_READ_PORT("P2")
	AM_RANGE(0x02, 0x02) AM_READ_PORT("IN2") // ??
	AM_RANGE(0x04, 0x04) AM_READ_PORT("SYSTEM")
	AM_RANGE(0x05, 0x05) AM_READ(rom_bank_select_r) AM_WRITE(rom_bank_select_w)
	AM_RANGE(0x06, 0x06) AM_READWRITE( suprgolf_vregs_r,suprgolf_vregs_w ) // game locks up or crashes? if this doesn't return right values?

	AM_RANGE(0x08, 0x09) AM_DEVREADWRITE("ym", ym2203_r, ym2203_w)
 ADDRESS_MAP_END

static INPUT_PORTS_START( suprgolf )
	PORT_START("P1")
	PORT_DIPNAME( 0x01, 0x01, "0" ) //USED!
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
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)	    /* D.L */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)	    /* D.R */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)			/* CNT */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)			/* SEL */

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN ) //USED!
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)	    /* D.L */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)	    /* D.R */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)			/* CNT */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)			/* SEL */

	PORT_START("IN2")
	PORT_DIPNAME( 0x01, 0x01, "2" )
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

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )               			/* 1P */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)			/* POW */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )  	                	/* 1P */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)			/* POW */
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "TST" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("IN4")
	PORT_DIPNAME( 0x01, 0x01, "4" )
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

	PORT_START("DSW0")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Tutorial" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, "Number of Balls" )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("DSW1")
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x02, 0x02, "DSW1" )
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

static WRITE8_DEVICE_HANDLER( suprgolf_writeA )
{
	mame_printf_debug("ymwA\n");
}

static WRITE8_DEVICE_HANDLER( suprgolf_writeB )
{
	mame_printf_debug("ymwA\n");
}

static void irqhandler(const device_config *device, int irq)
{
//  cpu_set_input_line(device->machine->cpu[1],0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static const ym2203_interface ym2203_config =
{
	{
		AY8910_LEGACY_OUTPUT,
		AY8910_DEFAULT_LOADS,
		DEVCB_INPUT_PORT("DSW0"),
		DEVCB_INPUT_PORT("DSW1"),
		DEVCB_HANDLER(suprgolf_writeA),
		DEVCB_HANDLER(suprgolf_writeB),
	},
	irqhandler
};
static const gfx_layout gfxlayout =
{
   8,8,
   RGN_FRAC(1,1),
   4,
   { 0, 1, 2, 3 },
   { 1*4, 0*4, 3*4, 2*4, 5*4, 4*4, 7*4, 6*4 },
   { 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
   8*8*4
};

static GFXDECODE_START( suprgolf )
	GFXDECODE_ENTRY( "gfx1", 0, gfxlayout,   0, 0x80 )
GFXDECODE_END

static MACHINE_DRIVER_START( suprgolf )
	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80,4000000)
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_IO_MAP(io_map,0)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)

	MDRV_VIDEO_START(suprgolf)
	MDRV_VIDEO_UPDATE(suprgolf)

	MDRV_MACHINE_RESET(suprgolf)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 255, 0, 191)

	MDRV_GFXDECODE(suprgolf)
	MDRV_PALETTE_LENGTH(0x1000)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym", YM2203, 3000000)
	MDRV_SOUND_CONFIG(ym2203_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


/*
----------------------
CG24     6K        CONN BD
CG1      6J         "
CG2      6G         "
CG3      6F         "
CG4      6D         "
CG5      6C         "
CG6      6A         "
CG7      5J         "
CG8      5G         "
CG9      5F         "
CG10     5D         "
CG11     5A         "
CG12     6K         "
CG13     6J         "
CG14     5K         "
CG15     5J         "
CG16     5G         "
CG17     5F         "

CG18     3K        DAUGHTER BOARD
CG20     7K         "
CG21     7J         "
CG22     7G         "
CG23     7F         "
*/

ROM_START( suprgolf )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cg24.6k",0x000000, 0x08000, CRC(de548044) SHA1(f96b4cfcfca4dffabfaf205eb903cbc70972626b) )

	ROM_REGION( 0x100000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD( "cg1.6j", 0x000000, 0x10000, CRC(ee545c71) SHA1(8ee459a85e52257d3f9a2aa7263b641aad87bafd) )
	ROM_LOAD( "cg2.6g", 0x010000, 0x10000, CRC(a2ed2159) SHA1(5e13b6c4eaba8146a4c6c2ff24197f3ffca29b92) )
	ROM_LOAD( "cg3.6f", 0x020000, 0x10000, CRC(4543334d) SHA1(7ee268ed6d02c78db8c222418313593df37cde4b) )
	ROM_LOAD( "cg4.6d", 0x030000, 0x10000, CRC(85ace664) SHA1(5267406c98e2d124a4985816f8e2e32e74e09614) )
	ROM_LOAD( "cg5.6c", 0x040000, 0x10000, CRC(609d5b37) SHA1(60640a9bd0883bf4dc999077d89ef793e827ac23) )
	ROM_LOAD( "cg6.6a", 0x050000, 0x10000, CRC(5e4a8ddb) SHA1(0c71c7eba9fe79187c4214eb639a481305070dcc) )
	ROM_LOAD( "cg7.5j", 0x060000, 0x10000, CRC(90ac6734) SHA1(2656397fca6dceabf8e35c093c0ba25e08d2ad1e) )
	ROM_LOAD( "cg8.5g", 0x070000, 0x10000, CRC(2e9edece) SHA1(a0961bb23f312ed137134746d2d3d438fe098085) )
	ROM_LOAD( "cg9.5f", 0x080000, 0x10000, CRC(139d71f1) SHA1(756ed068e1e2b76a9d1df95b432976e632edfa77) )
	ROM_LOAD( "cg10.5d",0x090000, 0x10000, CRC(c069e75e) SHA1(77f1b7571e677aef601b8b1c481b352ca6e485d6) )
	/* no 5c? */
	ROM_LOAD( "cg11.5a",0x0b0000, 0x10000, CRC(cfec1a0f) SHA1(c09ece059cb3c456b66c016c6fab3139d3f61c6a) )

	ROM_REGION( 0x100000, "user2", ROMREGION_ERASEFF )
	ROM_LOAD( "cg20.7k",0x000000, 0x10000, CRC(1e3fa2fd) SHA1(4771b90e40ebfbae4a98ff7ce6db50f635232597) )
	ROM_LOAD( "cg21.7j",0x010000, 0x10000, CRC(0323a2cd) SHA1(d7d4b35ad451acb2fa3d117bb0ae2f8fbd883f17) )
	ROM_LOAD( "cg22.7g",0x020000, 0x10000, CRC(83bcbefd) SHA1(77f29cfd1583d2506e95b8513cb9f87569c31821) )
	ROM_LOAD( "cg23.7f",0x030000, 0x10000, CRC(50191b4d) SHA1(8f74cba2a2b5fd2a03eaf13a6d6b39af8833a4ab) )

	ROM_REGION( 0x70000, "gfx1", ROMREGION_DISPOSE )
	ROM_LOAD( "cg18.3k",0x60000, 0x10000, CRC(36edd88e) SHA1(374c95721198a88831d6f7e0b71d05e2f8465271) )
	ROM_LOAD( "cg17.5f",0x50000, 0x10000, CRC(d27f87b5) SHA1(5b2927e89615589540e3853593aeff517584b6a0))
	ROM_LOAD( "cg16.5g",0x40000, 0x10000, CRC(0498aa2e) SHA1(988965c3a584dac17ad8c7e504fa1f1e49775611) )
	ROM_LOAD( "cg15.5j",0x30000, 0x10000, CRC(0fb88270) SHA1(d85a7f1bc5b3c4b13bbd887cea4c055541cbb737) )
	ROM_LOAD( "cg14.5k",0x20000, 0x10000, CRC(ca12e01d) SHA1(9c627fb527c8966e16dc6bdb99ec0b9728b5c5f9) )
	ROM_LOAD( "cg13.6j",0x10000, 0x10000, CRC(02ff0187) SHA1(aeeb3b2d15c3c8ff4695ecf6cfc0c385295ecce6) )
	ROM_LOAD( "cg12.6k",0x00000, 0x10000, CRC(5707b3d5) SHA1(9102a40fefb6426f2cd9d92d66fdc77e078e3f4c) )
ROM_END

GAME( 1989, suprgolf, 0, suprgolf,  suprgolf,  0, ROT0, "Nasco", "Super Crowns Golf (Japan)", GAME_NOT_WORKING )
