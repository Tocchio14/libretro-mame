/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"

UINT8 *tankbatt_bulletsram;
size_t tankbatt_bulletsram_size;

static tilemap_t *bg_tilemap;

/***************************************************************************

  Convert the color PROMs into a more useable format.

***************************************************************************/
PALETTE_INIT( tankbatt )
{
	int i;

	#define RES_1	0xc0 /* this is a guess */
	#define RES_2	0x3f /* this is a guess */

	/* allocate the colortable */
	machine->colortable = colortable_alloc(machine, 0x100);

	/* create a lookup table for the palette */
	for (i = 0; i < 0x100; i++)
	{
		int bit0, bit1, bit2, bit3;
		int r, g, b;

		bit0 = (color_prom[i] >> 0) & 0x01; /* intensity */
		bit1 = (color_prom[i] >> 1) & 0x01; /* red */
		bit2 = (color_prom[i] >> 2) & 0x01; /* green */
		bit3 = (color_prom[i] >> 3) & 0x01; /* blue */

		/* red component */
		r = RES_1 * bit1;
		if (bit1) r += RES_2 * bit0;

		/* green component */
		g = RES_1 * bit2;
		if (bit2) g += RES_2 * bit0;

		/* blue component */
		b = RES_1 * bit3;
		if (bit3) b += RES_2 * bit0;

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r, g, b));
	}

	for (i = 0; i < 0x200; i += 2)
	{
		colortable_entry_set_value(machine->colortable, i + 0, 0);
		colortable_entry_set_value(machine->colortable, i + 1, i >> 1);
	}
}

WRITE8_HANDLER( tankbatt_videoram_w )
{
	space->machine->generic.videoram.u8[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset);
}

static TILE_GET_INFO( get_bg_tile_info )
{
	int code = machine->generic.videoram.u8[tile_index];
	int color = machine->generic.videoram.u8[tile_index] | 0x01;

	SET_TILE_INFO(0, code, color, 0);
}

VIDEO_START( tankbatt )
{
	bg_tilemap = tilemap_create(machine, get_bg_tile_info, tilemap_scan_rows, 8, 8, 32, 32);
}

static void draw_bullets(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	int offs;

	for (offs = 0;offs < tankbatt_bulletsram_size;offs += 2)
	{
		int color = 0xff;	/* cyan, same color as the tanks */
		int x = tankbatt_bulletsram[offs + 1];
		int y = 255 - tankbatt_bulletsram[offs] - 2;

		drawgfx_opaque(bitmap,cliprect,machine->gfx[1],
			0,	/* this is just a square, generated by the hardware */
			color,
			0,0,
			x,y);
	}
}

VIDEO_UPDATE( tankbatt )
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	draw_bullets(screen->machine, bitmap, cliprect);
	return 0;
}
