/* Tao Taido Video Hardware */

/*

its similar to other video system games

zooming might be wrong (only used on title logo?)

*/

#include "emu.h"
#include "includes/taotaido.h"


/* sprite tile codes 0x4000 - 0x7fff get remapped according to the content of these registers */
WRITE16_HANDLER( taotaido_sprite_character_bank_select_w )
{
	taotaido_state *state = space->machine->driver_data<taotaido_state>();
	if(ACCESSING_BITS_8_15)
		state->sprite_character_bank_select[offset*2] = data >> 8;
	if(ACCESSING_BITS_0_7)
		state->sprite_character_bank_select[offset*2+1] = data &0xff;
}

/* sprites are like the other video system / psikyo games, we can merge this with aerofgt and plenty of other
   things eventually */

static void draw_sprite(running_machine *machine, UINT16 spriteno, bitmap_t *bitmap, const rectangle *cliprect )
{
	taotaido_state *state = machine->driver_data<taotaido_state>();
	/*- SPR RAM Format -**

      4 words per sprite

      zzzz sssp  pppp pppp (y zoom, y size, y position)
      zzzz sssp  pppp pppp (x zoom, x size, x position)
      yxpc cccc  ---- ---- (flipy, flipx, priority?, colour)
      -nnn nnnn  nnnn nnnn (tile lookup)

    */

	int x,y;

	UINT16 *source = &state->spriteram_older[spriteno*4];
	const gfx_element *gfx = machine->gfx[0];


	int yzoom = (source[0] & 0xf000) >> 12;
	int xzoom = (source[1] & 0xf000) >> 12;

	int ysize = (source[0] & 0x0e00) >> 9;
	int xsize = (source[1] & 0x0e00) >> 9;

	int ypos = source[0] & 0x01ff;
	int xpos = source[1] & 0x01ff;

	int yflip = source[2] & 0x8000;
	int xflip = source[2] & 0x4000;
	int color = (source[2] & 0x1f00) >> 8;

	int tile = source[3] & 0xffff;

	xpos += (xsize*xzoom+2)/4;
	ypos += (ysize*yzoom+2)/4;

	xzoom = 32 - xzoom;
	yzoom = 32 - yzoom;


	for (y = 0;y <= ysize;y++)
	{
		int sx,sy;

		if (yflip) sy = ((ypos + yzoom * (ysize - y)/2 + 16) & 0x1ff) - 16;
			else sy = ((ypos + yzoom * y / 2 + 16) & 0x1ff) - 16;

		for (x = 0;x <= xsize;x++)
		{

			/* this indirection is a bit different to the other video system games */
			int realtile;

			realtile = state->spriteram2_older[tile&0x7fff];

			if (realtile > 0x3fff)
			{
				int block;

				block = (realtile & 0x3800)>>11;

				realtile &= 0x07ff;
				realtile |= state->sprite_character_bank_select[block] * 0x800;
			}

			if (xflip) sx = ((xpos + xzoom * (xsize - x) / 2 + 16) & 0x1ff) - 16;
				else sx = ((xpos + xzoom * x / 2 + 16) & 0x1ff) - 16;


			drawgfxzoom_transpen(bitmap,cliprect,gfx,
						realtile,
						color,
						xflip,yflip,
						sx,sy,
						xzoom << 11, yzoom << 11,15);

			tile++;

		}
	}
}

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	taotaido_state *state = machine->driver_data<taotaido_state>();
	/* first part of sprite ram is the list of sprites to draw, terminated with 0x4000 */
	UINT16 *source = state->spriteram_older;
	UINT16 *finish = state->spriteram_older + 0x2000/2;

	while( source<finish )
	{
		if (source[0] == 0x4000) break;

		draw_sprite(machine, source[0]&0x3ff, bitmap, cliprect);

		source++;
	}
}


/* the tilemap */

WRITE16_HANDLER( taotaido_tileregs_w )
{
	taotaido_state *state = space->machine->driver_data<taotaido_state>();
	switch (offset)
	{
		case 0: // would normally be x scroll?
		case 1: // would normally be y scroll?
		case 2:
		case 3:
			logerror ("unhanded tilemap register write offset %02x data %04x \n",offset,data);
			break;

		/* tile banks */
		case 4:
		case 5:
		case 6:
		case 7:
			if(ACCESSING_BITS_8_15)
				state->video_bank_select[(offset-4)*2] = data >> 8;
			if(ACCESSING_BITS_0_7)
				state->video_bank_select[(offset-4)*2+1] = data &0xff;
				tilemap_mark_all_tiles_dirty(state->bg_tilemap);
			break;
	}
}

WRITE16_HANDLER( taotaido_bgvideoram_w )
{
	taotaido_state *state = space->machine->driver_data<taotaido_state>();
	COMBINE_DATA(&state->bgram[offset]);
	tilemap_mark_tile_dirty(state->bg_tilemap,offset);
}

static TILE_GET_INFO( taotaido_bg_tile_info )
{
	taotaido_state *state = machine->driver_data<taotaido_state>();
	int code = state->bgram[tile_index]&0x01ff;
	int bank = (state->bgram[tile_index]&0x0e00)>>9;
	int col  = (state->bgram[tile_index]&0xf000)>>12;

	code |= state->video_bank_select[bank]*0x200;

	SET_TILE_INFO(
			1,
			code,
			col,
			0);
}

static TILEMAP_MAPPER( taotaido_tilemap_scan_rows )
{
	/* logical (col,row) -> memory offset */
	return row*0x40 + (col&0x3f) + ((col&0x40)<<6);
}

VIDEO_START(taotaido)
{
	taotaido_state *state = machine->driver_data<taotaido_state>();
	state->bg_tilemap = tilemap_create(machine, taotaido_bg_tile_info,taotaido_tilemap_scan_rows,     16,16,128,64);

	state->spriteram_old = auto_alloc_array(machine, UINT16, 0x2000/2);
	state->spriteram_older = auto_alloc_array(machine, UINT16, 0x2000/2);

	state->spriteram2_old = auto_alloc_array(machine, UINT16, 0x10000/2);
	state->spriteram2_older = auto_alloc_array(machine, UINT16, 0x10000/2);
}


SCREEN_UPDATE(taotaido)
{
	taotaido_state *state = screen->machine->driver_data<taotaido_state>();
//  tilemap_set_scrollx(state->bg_tilemap,0,(state->scrollram[0x380/2]>>4)); // the values put here end up being wrong every other frame
//  tilemap_set_scrolly(state->bg_tilemap,0,(state->scrollram[0x382/2]>>4)); // the values put here end up being wrong every other frame

	/* not amazingly efficient however it should be functional for row select and linescroll */
	int line;
	rectangle clip;

	const rectangle &visarea = screen->visible_area();
	clip.min_x = visarea.min_x;
	clip.max_x = visarea.max_x;
	clip.min_y = visarea.min_y;
	clip.max_y = visarea.max_y;

	for (line = 0; line < 224;line++)
	{
		clip.min_y = clip.max_y = line;

		tilemap_set_scrollx(state->bg_tilemap,0,((state->scrollram[(0x00+4*line)/2])>>4)+30);
		tilemap_set_scrolly(state->bg_tilemap,0,((state->scrollram[(0x02+4*line)/2])>>4)-line);

		tilemap_draw(bitmap,&clip,state->bg_tilemap,0,0);
	}

	draw_sprites(screen->machine, bitmap,cliprect);
	return 0;
}

SCREEN_EOF( taotaido )
{
	taotaido_state *state = machine->driver_data<taotaido_state>();
	/* sprites need to be delayed by 2 frames? */

	memcpy(state->spriteram2_older,state->spriteram2_old,0x10000);
	memcpy(state->spriteram2_old,state->spriteram2,0x10000);

	memcpy(state->spriteram_older,state->spriteram_old,0x2000);
	memcpy(state->spriteram_old,state->spriteram,0x2000);
}
