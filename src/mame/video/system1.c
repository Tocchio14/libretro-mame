/*************************************************************************

	System1 / System 2
	original driver by Jarek Parchanski & Mirko Buffoni

	Many thanks to Roberto Ventura, for precious information about
	System 1 hardware.

**************************************************************************

	The System 1/System 2 video hardware is composed of two tilemap
	layers and a sprite layer.
	
	The tilemap layers are built up out of "pages" of 32x32 tilemaps.
	Each tile is described by two bytes, meaning each page is 2k bytes
	in size. One of the tilemaps is fixed in position, while the other
	has registers for scrolling that vary between board variants.
	
	The original System 1 hardware simply had two fixed pages. Page 0
	was the scrolling tilemap, and page 1 was the fixed tilemap.
	
	With later boards and the introduction of System 2, this was 
	expanded to support up to 8 pages. The fixed tilemap was hard-
	coded to page 0, but the scrolling tilemap was extended. Instead
	of a single page, the scrolling tilemap consisted of 4 pages glued
	together to form an effective large 64x64 tilemap. Further, each
	of the 4 pages that made up the scrolling tilemap could be
	independently selected from one of the 8 available pages. This
	unique paged tilemap system would continue on to form the basis of
	Sega's tilemap systems for their 16-bit era.
	
	Up to 32 sprites can be displayed. They are rendered one scanline
	ahead of the beam into 12-bit line buffers which store the sprite
	pixel data and sprite index. During rendering, collisions are 
	checked between sprites and if one is found a bit is set in a 
	special 32x32x1 collision RAM indiciating which pair of sprites 
	collided. Note that the sprite color is not stored in the line
	buffer, but it retrieved when the line buffer is read out on the
	following scanline using the stored sprite index.
	
	The 11-bit output from the two tilemaps (3 bits of pixel data,
	6 bits of color, 2 bits of priority), plus the 9-bit output from 
	the sprite line buffer (4 bits of pixel data, 5 bits of color) 
	are combined in a final step to produce the final pixel value. To 
	do this, a lookup PROM is used which accepts as input the priority
	bits from the two tilemaps and the whether each of the incoming
	pixel values is transparent (color 0).
	
	The output of the lookup PROM is a 4-bit value. The lower 2 bits
	select sprite data (0), fixed tilemap (1) or scrolling tilemap (2).
	9 bits of data from the appropriate source are used as a lookup
	into a palette RAM, and the lookup PROM's low 2 bits are used as
	the upper 2 bits of the palette RAM address, providing 512 
	independent colors for each souce.
	
	The upper 2 bits of the lookup PROM are used for an additional
	mixer collision detection. Bit 2 indicates that a collision 
	should be recorded, and bit 3 indicates which of two banks of 
	collision flags should be set. Each bank is 32 entries long, and 
	the sprite index is used to select which bit within the bank to 
	set.
	
	On the original System 1 hardware, the palette RAM value was used
	directly as RGB, with 3 bits each of red and green, and 2 bits of
	blue. Later hardware added an extra indirection layer, where the
	8-bit palette RAM value passed into 3 256x4 palette PROMs, one for
	each color.
	
	Collision data is accessed via a 4k window that is broken into
	4 equal-sized sections. The first section returns data from the
	2x32x1 mixer collision; the data for the collision is returned in
	D0, and a summary bit indicating that some sort of collision has
	occurred is returned in D7. The specific collision bit is cleared
	by writing to the equivalent address in the same region. The 
	collision summary bit is cleared by writing to the second region.
	
	The third and fourth collision regions operate similarly, but
	return data for the 32x32x1 sprite collisions.

*************************************************************************/

#include "driver.h"
#include "system1.h"

static UINT8 *mix_collide;
static UINT8 mix_collide_summary;
static UINT8 *sprite_collide;
static UINT8 sprite_collide_summary;

static bitmap_t *sprite_bitmap;

static UINT8 system1_video_mode;

static UINT8 wbml_videoram_bank;

static int system1_sprite_xoffset;

static tilemap *tilemap_page[8];
static UINT8 tilemap_pages;


READ8_HANDLER( system1_mixer_collision_r )
{
	video_screen_update_now(space->machine->primary_screen);
	return mix_collide[offset & 0x3f] | 0x7e | (mix_collide_summary << 7);
}

WRITE8_HANDLER( system1_mixer_collision_w )
{
	video_screen_update_now(space->machine->primary_screen);
	mix_collide[offset & 0x3f] = 0;
}

WRITE8_HANDLER( system1_mixer_collision_reset_w )
{
	video_screen_update_now(space->machine->primary_screen);
	mix_collide_summary = 0;
}

READ8_HANDLER( system1_sprite_collision_r )
{
	video_screen_update_now(space->machine->primary_screen);
	return sprite_collide[offset & 0x3ff] | 0x7e | (sprite_collide_summary << 7);
}

WRITE8_HANDLER( system1_sprite_collision_w )
{
	video_screen_update_now(space->machine->primary_screen);
	sprite_collide[offset & 0x3ff] = 0;
}

WRITE8_HANDLER( system1_sprite_collision_reset_w )
{
	video_screen_update_now(space->machine->primary_screen);
	sprite_collide_summary = 0;
}


READ8_HANDLER( system1_videoram_r )
{
	offset |= 0x1000 * ((wbml_videoram_bank >> 1) % (tilemap_pages / 2));
	return videoram[offset];
}

WRITE8_HANDLER( system1_videoram_w )
{
	offset |= 0x1000 * ((wbml_videoram_bank >> 1) % (tilemap_pages / 2));
	videoram[offset] = data;

	tilemap_mark_tile_dirty(tilemap_page[offset / 0x800], (offset % 0x800) / 2);

	/* force a partial update if the page is changing */
	if (tilemap_pages > 2 && offset >= 0x740 && offset < 0x748 && offset % 2 == 0)
		video_screen_update_now(space->machine->primary_screen);
}

WRITE8_DEVICE_HANDLER( system1_videoram_bank_w )
{
	wbml_videoram_bank = data;
}


/***************************************************************************

  There are two kind of color handling: in the System 1 games, values in the
  palette RAM are directly mapped to colors with the usual BBGGGRRR format;
  in the System 2 ones (Choplifter, WBML, etc.), the value in the palette RAM
  is a lookup offset for three palette PROMs in RRRRGGGGBBBB format.

  It's hard to tell for sure because they use resistor packs, but here's
  what I think the values are from measurment with a volt meter:

  Blue: .250K ohms
  Blue: .495K ohms
  Green:.250K ohms
  Green:.495K ohms
  Green:.995K ohms
  Red:  .495K ohms
  Red:  .250K ohms
  Red:  .995K ohms

  accurate to +/- .003K ohms.

***************************************************************************/

WRITE8_HANDLER( system1_paletteram_w )
{
	const UINT8 *color_prom = memory_region(space->machine, "palette");
	int val,r,g,b;

	paletteram[offset] = data;

	if (color_prom != NULL)
	{
		int bit0,bit1,bit2,bit3;

		val = color_prom[data+0*256];
		bit0 = (val >> 0) & 0x01;
		bit1 = (val >> 1) & 0x01;
		bit2 = (val >> 2) & 0x01;
		bit3 = (val >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		val = color_prom[data+1*256];
		bit0 = (val >> 0) & 0x01;
		bit1 = (val >> 1) & 0x01;
		bit2 = (val >> 2) & 0x01;
		bit3 = (val >> 3) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		val = color_prom[data+2*256];
		bit0 = (val >> 0) & 0x01;
		bit1 = (val >> 1) & 0x01;
		bit2 = (val >> 2) & 0x01;
		bit3 = (val >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
	}
	else
	{
		r = pal3bit(data >> 0);
		g = pal3bit(data >> 3);
		b = pal2bit(data >> 6);
	}

	palette_set_color(space->machine,offset,MAKE_RGB(r,g,b));
}

static TILE_GET_INFO( tile_get_info )
{
	UINT8 *rambase = param;
	UINT32 tiledata = rambase[tile_index*2+0] | (rambase[tile_index*2+1] << 8);
	UINT32 code = ((tiledata >> 4) & 0x800) | (tiledata & 0x7ff);	/* Heavy Metal only */
	UINT32 color = (tiledata >> 5) & 0xff;

	SET_TILE_INFO(0, code, color, 0);
}

static void video_start_common(running_machine *machine, int pagecount)
{
	int pagenum;
	
	/* allocate memory for the collision arrays */
	mix_collide = auto_malloc(64);
	memset(mix_collide, 0, 64);
	sprite_collide = auto_malloc(1024);
	memset(sprite_collide, 0, 1024);
	
	/* allocate memory for videoram */
	tilemap_pages = pagecount;
	videoram = auto_malloc(0x800 * pagecount);
	memset(videoram, 0, 0x800 * pagecount);

	/* create the tilemap pages */
	for (pagenum = 0; pagenum < pagecount; pagenum++)
	{
		tilemap_page[pagenum] = tilemap_create(machine, tile_get_info, tilemap_scan_rows, 8,8, 32,32);
		tilemap_set_transparent_pen(tilemap_page[pagenum], 0);
		tilemap_set_user_data(tilemap_page[pagenum], videoram + 0x800 * pagenum);
	}

	/* allocate a temporary bitmap for sprite rendering */
	sprite_bitmap = auto_bitmap_alloc(256, 256, BITMAP_FORMAT_INDEXED16);

	state_save_register_global(machine, system1_video_mode);
	state_save_register_global_pointer(machine, videoram, 0x800 * pagecount);
}

VIDEO_START( system1 )
{
	video_start_common(machine, 2);
	system1_sprite_xoffset = 1;
}

VIDEO_START( system2 )
{
	video_start_common(machine, 8);
	system1_sprite_xoffset = 1+7*2;
}

WRITE8_HANDLER( system1_videomode_w )
{
if (data & 0x6e) logerror("videomode = %02x\n",data);

	/* bit 0 is coin counter */
	coin_counter_w(0, data & 1);

	/* bit 4 is screen blank */
	system1_video_mode = data;

	/* bit 7 is flip screen */
	flip_screen_set(space->machine, data & 0x80);
}


INLINE void draw_sprite_pixel(bitmap_t *bitmap, int x, int y, int spr_number, int color)
{
	UINT16 *destpix = BITMAP_ADDR16(bitmap, y, x);
	int prevpix = *destpix;
	
	if ((prevpix & 0x0f) != 0)
		sprite_collide[(prevpix >> 11) + 32 * spr_number] = sprite_collide_summary = 1;

	*destpix = color | (spr_number << 11);
}

static void draw_sprite(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect, int spr_number)
{
	int flipscreen = flip_screen_get(machine);
	int sy,row,height,src,bank;
	UINT8 *sprite_base;
	int sprite_palette_base;
	INT16 skip;	/* bytes to skip before drawing each row (can be negative) */
	UINT8 *gfx;


	sprite_base	= spriteram + 0x10 * spr_number;

	src = sprite_base[SPR_GFXOFS_LO] + (sprite_base[SPR_GFXOFS_HI] << 8);
	bank = 0x8000 * (((sprite_base[SPR_X_HI] & 0x80) >> 7) + ((sprite_base[SPR_X_HI] & 0x40) >> 5) + ((sprite_base[SPR_X_HI] & 0x20) >> 3));
	bank &= (memory_region_length(machine, "sprites")-1);	/* limit to the range of available ROMs */
	skip = sprite_base[SPR_SKIP_LO] + (sprite_base[SPR_SKIP_HI] << 8);

	height = sprite_base[SPR_Y_BOTTOM] - sprite_base[SPR_Y_TOP];
	sprite_palette_base = 0x10 * spr_number;

	sy = sprite_base[SPR_Y_TOP] + 1;

	/* graphics region #2 contains the packed sprite data */
	gfx = &memory_region(machine, "sprites")[bank];
	
	for (row = 0;row < height;row++)
	{
		int x,x_flipped;
		int y,y_flipped;
		int src2;

		src = src2 = src + skip;

		x = sprite_base[SPR_X_LO] + ((sprite_base[SPR_X_HI] & 0x01) << 8) + system1_sprite_xoffset;
		x_flipped = x;
		y = y_flipped = sy+row;

		if (flipscreen)
		{
			y_flipped = 258 - sy - height + row;
			x_flipped = (252*2) - x;
		}
		
		x /= 2;	/* the hardware has sub-pixel placement, it seems */
		x_flipped /= 2;

		while (1)
		{
			int color1,color2,data;

			data = gfx[src2 & 0x7fff];

			if (src & 0x8000)
			{
				src2--;

				color1 = data & 0x0f;
				color2 = data >> 4;
			}
			else
			{
				src2++;

				color1 = data >> 4;
				color2 = data & 0x0f;
			}

			if (color1 == 15) break;
			if (color1 != 0 && x_flipped >= cliprect->min_x && x_flipped <= cliprect->max_x && y_flipped >= cliprect->min_y && y_flipped <= cliprect->max_y)
				draw_sprite_pixel(bitmap,x_flipped,y_flipped,spr_number,sprite_palette_base+color1);
			x++;
			x_flipped += flipscreen ? -1 : 1;

			if (color2 == 15) break;
			if (color2 != 0 && x_flipped >= cliprect->min_x && x_flipped <= cliprect->max_x && y_flipped >= cliprect->min_y && y_flipped <= cliprect->max_y)
				draw_sprite_pixel(bitmap,x_flipped,y_flipped,spr_number,sprite_palette_base+color2);
			x++;
			x_flipped += flipscreen ? -1 : 1;
		}
	}
}


static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	int spr_number,sprite_bottom_y,sprite_top_y;
	UINT8 *sprite_base;

	for (spr_number = 0;spr_number < 32;spr_number++)
	{
		sprite_base = spriteram + 0x10 * spr_number;
		sprite_top_y = sprite_base[SPR_Y_TOP];
		sprite_bottom_y = sprite_base[SPR_Y_BOTTOM];
		if (sprite_bottom_y && (sprite_bottom_y-sprite_top_y > 0))
			draw_sprite(machine, bitmap, cliprect, spr_number);
	}
}


static void video_update_common(const device_config *screen, bitmap_t *bitmap, const rectangle *cliprect, bitmap_t *fgpixmap, bitmap_t **bgpixmaps, const int *bgrowscroll, int bgyscroll)
{
	const UINT8 *lookup = memory_region(screen->machine, "proms");
	int x, y;

	/* first clear the sprite bitmap and draw sprites within this area */
	bitmap_fill(sprite_bitmap, cliprect, 0);
	draw_sprites(screen->machine, sprite_bitmap, cliprect);
	
	/* iterate over rows */
	for (y = cliprect->min_y; y <= cliprect->max_y; y++)
	{
		UINT16 *fgbase = BITMAP_ADDR16(fgpixmap, y, 0);
		UINT16 *sprbase = BITMAP_ADDR16(sprite_bitmap, y, 0);
		UINT16 *dstbase = BITMAP_ADDR16(bitmap, y, 0);
		int bgy = (y + bgyscroll) & 0x1ff;
		int bgxscroll = bgrowscroll[y / 8];
		UINT16 *bgbase[2];

		/* get the base of the left and right pixmaps for the effective background Y */
		bgbase[0] = BITMAP_ADDR16(bgpixmaps[(bgy >> 8) * 2 + 0], bgy & 0xff, 0);
		bgbase[1] = BITMAP_ADDR16(bgpixmaps[(bgy >> 8) * 2 + 1], bgy & 0xff, 0);

		/* iterate over pixels */		
		for (x = cliprect->min_x; x <= cliprect->max_x; x++)
		{
			int bgx = (x - bgxscroll) & 0x1ff;
			UINT16 fgpix = fgbase[x];
			UINT16 bgpix = bgbase[bgx >> 8][bgx & 0xff];
			UINT16 sprpix = sprbase[x];
			UINT8 lookup_index;
			UINT8 lookup_value;
			
			/* using the sprite, background, and foreground pixels, look up the color behavior */
			lookup_index = 	(((sprpix & 0xf) == 0) << 0) |
							(((fgpix & 7) == 0) << 1) |
							(((fgpix >> 9) & 3) << 2) |
							(((bgpix & 7) == 0) << 4) |
							(((bgpix >> 9) & 3) << 5);
			lookup_value = lookup[lookup_index];

			/* compute collisions based on two of the PROM bits */			
			if (!(lookup_value & 4))
				mix_collide[((lookup_value & 8) << 2) | (sprpix >> 11)] = mix_collide_summary = 1;

			/* the lower 2 PROM bits select the palette and which pixels */
			lookup_value &= 3;
			if (system1_video_mode & 0x10)
				dstbase[x] = 0;
			else if (lookup_value == 0)
				dstbase[x] = 0x000 | (sprpix & 0x1ff);
			else if (lookup_value == 1)
				dstbase[x] = 0x200 | (fgpix & 0x1ff);
			else
				dstbase[x] = 0x400 | (bgpix & 0x1ff);
		}
	}
}


VIDEO_UPDATE( system1 )
{
	bitmap_t *bgpixmaps[4], *fgpixmap;
	int bgrowscroll[32];
	int y;

	bgpixmaps[0] = bgpixmaps[1] = bgpixmaps[2] = bgpixmaps[3] = tilemap_get_pixmap(tilemap_page[0]);
	fgpixmap = tilemap_get_pixmap(tilemap_page[1]);
	for (y = 0; y < 32; y++)
		bgrowscroll[y] = (videoram[0xffc] | (videoram[0xffd] << 8)) / 2 + 14;
	
	video_update_common(screen, bitmap, cliprect, fgpixmap, bgpixmaps, bgrowscroll, videoram[0xfbd]);
	return 0;
}


VIDEO_UPDATE( system2 )
{
	int flip = flip_screen_get(screen->machine);
	int xscrolloffs = flip ? 0x7f6 : 0x7c0;
	int yscrolloffs = flip ? 0x784 : 0x7ba;
	bitmap_t *bgpixmaps[4], *fgpixmap;
	int rowscroll[32];
	int y;
	
	bgpixmaps[0] = tilemap_get_pixmap(tilemap_page[videoram[0x740] & 7]);
	bgpixmaps[1] = tilemap_get_pixmap(tilemap_page[videoram[0x742] & 7]);
	bgpixmaps[2] = tilemap_get_pixmap(tilemap_page[videoram[0x744] & 7]);
	bgpixmaps[3] = tilemap_get_pixmap(tilemap_page[videoram[0x746] & 7]);
	fgpixmap = tilemap_get_pixmap(tilemap_page[0]);
	for (y = 0; y < 32; y++)
		rowscroll[y] = (((videoram[xscrolloffs + 0] | (videoram[xscrolloffs + 1] << 8)) / 2) & 0xff) - 256 + 5;

	video_update_common(screen, bitmap, cliprect, fgpixmap, bgpixmaps, rowscroll, videoram[yscrolloffs]);
	return 0;
}


VIDEO_UPDATE( system2_rowscroll )
{
	bitmap_t *bgpixmaps[4], *fgpixmap;
	int rowscroll[32];
	int y;
	
	bgpixmaps[0] = tilemap_get_pixmap(tilemap_page[videoram[0x740] & 7]);
	bgpixmaps[1] = tilemap_get_pixmap(tilemap_page[videoram[0x742] & 7]);
	bgpixmaps[2] = tilemap_get_pixmap(tilemap_page[videoram[0x744] & 7]);
	bgpixmaps[3] = tilemap_get_pixmap(tilemap_page[videoram[0x746] & 7]);
	fgpixmap = tilemap_get_pixmap(tilemap_page[0]);
	for (y = 0; y < 32; y++)
		rowscroll[y] = (((videoram[0x7c0 + y * 2] | (videoram[0x7c1 + y * 2] << 8)) / 2) & 0xff) - 256 + 5;

	video_update_common(screen, bitmap, cliprect, fgpixmap, bgpixmaps, rowscroll, videoram[0x7ba]);
	return 0;
}
