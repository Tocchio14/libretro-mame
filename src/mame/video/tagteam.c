/***************************************************************************

    video.c

    Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "includes/tagteam.h"


PALETTE_INIT( tagteam )
{
	int i;

	for (i = 0;i < machine->total_colors();i++)
	{
		int bit0,bit1,bit2,r,g,b;


		/* red component */
		bit0 = (*color_prom >> 0) & 0x01;
		bit1 = (*color_prom >> 1) & 0x01;
		bit2 = (*color_prom >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (*color_prom >> 3) & 0x01;
		bit1 = (*color_prom >> 4) & 0x01;
		bit2 = (*color_prom >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (*color_prom >> 6) & 0x01;
		bit2 = (*color_prom >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(machine,i,MAKE_RGB(r,g,b));
		color_prom++;
	}
}

WRITE8_HANDLER( tagteam_videoram_w )
{
	tagteam_state *state = space->machine->driver_data<tagteam_state>();
	state->videoram[offset] = data;
	tilemap_mark_tile_dirty(state->bg_tilemap, offset);
}

WRITE8_HANDLER( tagteam_colorram_w )
{
	tagteam_state *state = space->machine->driver_data<tagteam_state>();
	state->colorram[offset] = data;
	tilemap_mark_tile_dirty(state->bg_tilemap, offset);
}

READ8_HANDLER( tagteam_mirrorvideoram_r )
{
	tagteam_state *state = space->machine->driver_data<tagteam_state>();
	int x,y;

	/* swap x and y coordinates */
	x = offset / 32;
	y = offset % 32;
	offset = 32 * y + x;

	return state->videoram[offset];
}

READ8_HANDLER( tagteam_mirrorcolorram_r )
{
	tagteam_state *state = space->machine->driver_data<tagteam_state>();
	int x,y;

	/* swap x and y coordinates */
	x = offset / 32;
	y = offset % 32;
	offset = 32 * y + x;

	return state->colorram[offset];
}

WRITE8_HANDLER( tagteam_mirrorvideoram_w )
{
	int x,y;

	/* swap x and y coordinates */
	x = offset / 32;
	y = offset % 32;
	offset = 32 * y + x;

	tagteam_videoram_w(space,offset,data);
}

WRITE8_HANDLER( tagteam_mirrorcolorram_w )
{
	int x,y;

	/* swap x and y coordinates */
	x = offset / 32;
	y = offset % 32;
	offset = 32 * y + x;

	tagteam_colorram_w(space,offset,data);
}

WRITE8_HANDLER( tagteam_control_w )
{
	tagteam_state *state = space->machine->driver_data<tagteam_state>();
logerror("%04x: control = %02x\n",cpu_get_pc(space->cpu),data);

	/* bit 7 is the palette bank */
	state->palettebank = (data & 0x80) >> 7;
}

WRITE8_HANDLER( tagteam_flipscreen_w )
{
	if (flip_screen_get(space->machine) != (data &0x01))
	{
		flip_screen_set(space->machine, data & 0x01);
		tilemap_mark_all_tiles_dirty_all(space->machine);
	}
}

static TILE_GET_INFO( get_bg_tile_info )
{
	tagteam_state *state = machine->driver_data<tagteam_state>();
	int code = state->videoram[tile_index] + 256 * state->colorram[tile_index];
	int color = state->palettebank * 2; // GUESS

	SET_TILE_INFO(0, code, color, 0);
}

VIDEO_START( tagteam )
{
	tagteam_state *state = machine->driver_data<tagteam_state>();
	state->bg_tilemap = tilemap_create(machine, get_bg_tile_info, tilemap_scan_rows_flip_x,
		 8, 8, 32, 32);
}

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	tagteam_state *state = machine->driver_data<tagteam_state>();
	int offs;

	for (offs = 0; offs < 0x20; offs += 4)
	{
		int spritebank = (state->videoram[offs] & 0x30) << 4;
		int code = state->videoram[offs + 1] + 256 * spritebank;
		int color = 1 + 2 * state->palettebank; // GUESS
		int flipx = state->videoram[offs] & 0x04;
		int flipy = state->videoram[offs] & 0x02;
		int sx = 240 - state->videoram[offs + 3];
		int sy = 240 - state->videoram[offs + 2];

		if (!(state->videoram[offs] & 0x01)) continue;

		if (flip_screen_get(machine))
		{
			sx = 240 - sx;
			sy = 240 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		drawgfx_transpen(bitmap, cliprect,
			machine->gfx[1],
			code, color,
			flipx, flipy,
			sx, sy, 0);

		/* Wrap around */

		code = state->videoram[offs + 0x20] + 256 * spritebank;
		color = state->palettebank;
		sy += (flip_screen_get(machine) ? -256 : 256);

		drawgfx_transpen(bitmap, cliprect,
			machine->gfx[1],
			code, color,
			flipx, flipy,
			sx, sy, 0);
	}
}

SCREEN_UPDATE( tagteam )
{
	tagteam_state *state = screen->machine->driver_data<tagteam_state>();
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, 0, 0);
	draw_sprites(screen->machine, bitmap, cliprect);
	return 0;
}
