#include "emu.h"
#include "includes/ssrj.h"

/* tilemap 1 */

WRITE8_HANDLER(ssrj_vram1_w)
{
	ssrj_state *state = space->machine().driver_data<ssrj_state>();

	state->m_vram1[offset] = data;
	tilemap_mark_tile_dirty(state->m_tilemap1, offset>>1);
}

static TILE_GET_INFO( get_tile_info1 )
{
	ssrj_state *state = machine.driver_data<ssrj_state>();
	int code;
	code = state->m_vram1[tile_index<<1] + (state->m_vram1[(tile_index<<1)+1]<<8);
	SET_TILE_INFO(
		0,
		code&0x3ff,
		(code>>12)&0x3,
	  ((code & 0x8000) ? TILE_FLIPY:0) |( (code & 0x4000) ? TILE_FLIPX:0)	);
}

/* tilemap 2 */

WRITE8_HANDLER(ssrj_vram2_w)
{
	ssrj_state *state = space->machine().driver_data<ssrj_state>();

	state->m_vram2[offset] = data;
	tilemap_mark_tile_dirty(state->m_tilemap2, offset>>1);
}

static TILE_GET_INFO( get_tile_info2 )
{
	ssrj_state *state = machine.driver_data<ssrj_state>();
	int code;
	code = state->m_vram2[tile_index<<1] + (state->m_vram2[(tile_index<<1)+1]<<8);
	SET_TILE_INFO(
		0,
		code&0x3ff,
		((code>>12)&0x3)+4,
	  ((code & 0x8000) ? TILE_FLIPY:0) |( (code & 0x4000) ? TILE_FLIPX:0)	);
}

/* tilemap 4 */

WRITE8_HANDLER(ssrj_vram4_w)
{
	ssrj_state *state = space->machine().driver_data<ssrj_state>();

	state->m_vram4[offset] = data;
	tilemap_mark_tile_dirty(state->m_tilemap4, offset>>1);
}

static TILE_GET_INFO( get_tile_info4 )
{
	ssrj_state *state = machine.driver_data<ssrj_state>();
	int code;
	code = state->m_vram4[tile_index<<1] + (state->m_vram4[(tile_index<<1)+1]<<8);
	SET_TILE_INFO(
		0,
		code&0x3ff,
		((code>>12)&0x3)+12,
	  ((code & 0x8000) ? TILE_FLIPY:0) |( (code & 0x4000) ? TILE_FLIPX:0)	);
}



static const UINT8 fakecols[4*4][8][3]=
{

{{0x00,0x00,0x00},
 {42,87,140},
 {0,0,0},
 {33,75,160},
 {0xff,0xff,0xff},
 {37,56,81},
 {0x1f,0x1f,0x2f},
 {55,123,190}},

{{0x00,0x00,0x00},
 {0x00,99,41},
 {0x00,0x00,0xff},
 {0x00,0xff,0},
 {255,255,255},
 {0xff,0x00,0x00},
 {0,45,105},
 {0xff,0xff,0}},


{{0x00,0x00,0x00},
 {0x00,0x20,0x00},
 {0x00,0x40,0x00},
 {0x00,0x60,0x00},
 {0x00,0x80,0x00},
 {0x00,0xa0,0x00},
 {0x00,0xc0,0x00},
 {0x00,0xf0,0x00}},

 {{0x00,0x00,0x00},
 {0x20,0x00,0x20},
 {0x40,0x00,0x40},
 {0x60,0x00,0x60},
 {0x80,0x00,0x80},
 {0xa0,0x00,0xa0},
 {0xc0,0x00,0xc0},
 {0xf0,0x00,0xf0}},

{{0x00,0x00,0x00},
 {0xff,0x00,0x00},
 {0x7f,0x00,0x00},
 {0x00,0x00,0x00},
 {0x00,0x00,0x00},
 {0xaf,0x00,0x00},
 {0xff,0xff,0xff},
 {0xff,0x7f,0x7f}},

{{0x00,0x00,0x00},
 {0x20,0x20,0x20},
 {0x40,0x40,0x40},
 {0x60,0x60,0x60},
 {0x80,0x80,0x80},
 {0xa0,0xa0,0xa0},
 {0xc0,0xc0,0xc0},
 {0xf0,0xf0,0xf0}},

{{0x00,0x00,0x00},
 {0x20,0x20,0x20},
 {0x40,0x40,0x40},
 {0x60,0x60,0x60},
 {0x80,0x80,0x80},
 {0xa0,0xa0,0xa0},
 {0xc0,0xc0,0xc0},
 {0xf0,0xf0,0xf0}},

{{0x00,0x00,0x00},
 {0xff,0x00,0x00},
 {0x00,0x00,0x9f},
 {0x60,0x60,0x60},
 {0x00,0x00,0x00},
 {0xff,0xff,0x00},
 {0x00,0xff,0x00},
 {0xff,0xff,0xff}},

{
 {0x00,0x00,0x00},
 {0x00,0x00,0xff},
 {0x00,0x00,0x7f},
 {0x00,0x00,0x00},
 {0x00,0x00,0x00},
 {0x00,0x00,0xaf},
 {0xff,0xff,0xff},
 {0x7f,0x7f,0xff}},

{{0x00,0x00,0x00},
 {0xff,0xff,0x00},
 {0x7f,0x7f,0x00},
 {0x00,0x00,0x00},
 {0x00,0x00,0x00},
 {0xaf,0xaf,0x00},
 {0xff,0xff,0xff},
 {0xff,0xff,0x7f}},

{{0x00,0x00,0x00},
 {0x00,0xff,0x00},
 {0x00,0x7f,0x00},
 {0x00,0x00,0x00},
 {0x00,0x00,0x00},
 {0x00,0xaf,0x00},
 {0xff,0xff,0xff},
 {0x7f,0xff,0x7f}},

{{0x00,0x00,0x00},
 {0x20,0x20,0x20},
 {0x40,0x40,0x40},
 {0x60,0x60,0x60},
 {0x80,0x80,0x80},
 {0xa0,0xa0,0xa0},
 {0xc0,0xc0,0xc0},
 {0xf0,0xf0,0xf0}},

{{0x00,0x00,0x00},
 {0x20,0x20,0x20},
 {0x40,0x40,0x40},
 {0x60,0x60,0x60},
 {0x80,0x80,0x80},
 {0xa0,0xa0,0xa0},
 {0xc0,0xc0,0xc0},
 {0xf0,0xf0,0xf0}},

{{0x00,0x00,0x00},
 {0x20,0x20,0x20},
 {0x40,0x40,0x40},
 {0x60,0x60,0x60},
 {0x80,0x80,0x80},
 {0xa0,0xa0,0xa0},
 {0xc0,0xc0,0xc0},
 {0xf0,0xf0,0xf0}},

{{0x00,0x00,0x00},
 {0x20,0x20,0x20},
 {0x40,0x40,0x40},
 {0x60,0x60,0x60},
 {0x80,0x80,0x80},
 {0xa0,0xa0,0xa0},
 {0xc0,0xc0,0xc0},
 {0xf0,0xf0,0xf0}},

{
 {0x00,0x00,0x00},
 {0xff,0xaf,0xaf},
 {0x00,0x00,0xff},
 {0xff,0xff,0xff},
 {0x00,0x00,0x00},
 {0xff,0x50,0x50},
 {0xff,0xff,0x00},
 {0x00,0xff,0x00}
}

};

VIDEO_START( ssrj )
{
	ssrj_state *state = machine.driver_data<ssrj_state>();

	state->m_tilemap1 = tilemap_create(machine, get_tile_info1, tilemap_scan_cols, 8, 8, 32, 32);
	state->m_tilemap2 = tilemap_create(machine, get_tile_info2, tilemap_scan_cols, 8, 8, 32, 32);
	state->m_tilemap4 = tilemap_create(machine, get_tile_info4, tilemap_scan_cols, 8, 8, 32, 32);
	tilemap_set_transparent_pen(state->m_tilemap2, 0);
	tilemap_set_transparent_pen(state->m_tilemap4, 0);

	state->m_buffer_spriteram = auto_alloc_array(machine, UINT8, 0x0800);
}


static void draw_objects(running_machine &machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	ssrj_state *state = machine.driver_data<ssrj_state>();
	int i,j,k,x,y;

	for(i=0;i<6;i++)
	{
		y = state->m_buffer_spriteram[0x80+20*i];
		x = state->m_buffer_spriteram[0x80+20*i+2];
		if (!state->m_buffer_spriteram[0x80+20*i+3])
		{

			for(k=0;k<5;k++,x+=8)
			{
				for(j=0;j<0x20;j++)
				{
					int code;
					int offs = (i * 5 + k) * 64 + (31 - j) * 2;

					code = state->m_vram3[offs] + 256 * state->m_vram3[offs + 1];
					drawgfx_transpen(bitmap,
						cliprect,machine.gfx[0],
						code&1023,
						((code>>12)&0x3)+8,
						code&0x4000,
						code&0x8000,
						x,
						(247-(y+(j<<3)))&0xff,
						0);
				}
			}
		}
	}
}


PALETTE_INIT( ssrj )
{
	int i, j;
	for(i=0; i<4*4; i++)
		for(j=0; j<8; j++)
			palette_set_color_rgb(machine, i*8+j, fakecols[i][j][0], fakecols[i][j][1], fakecols[i][j][2]);
}

SCREEN_UPDATE( ssrj )
{
	ssrj_state *state = screen->machine().driver_data<ssrj_state>();

	tilemap_set_scrollx(state->m_tilemap1, 0, 0xff-state->m_scrollram[2] );
	tilemap_set_scrolly(state->m_tilemap1, 0, state->m_scrollram[0] );
	tilemap_draw(bitmap, cliprect, state->m_tilemap1, 0, 0);
	draw_objects(screen->machine(), bitmap, cliprect);
	tilemap_draw(bitmap, cliprect, state->m_tilemap2, 0, 0);

	if (state->m_scrollram[0x101] == 0xb) tilemap_draw(bitmap, cliprect, state->m_tilemap4, 0, 0);/* hack to display 4th tilemap */
	return 0;
}

SCREEN_EOF( ssrj )
{
	ssrj_state *state = screen->machine().driver_data<ssrj_state>();

	memcpy(state->m_buffer_spriteram, state->m_scrollram, 0x800);
}
