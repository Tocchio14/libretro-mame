/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "includes/zodiack.h"

WRITE8_MEMBER( zodiack_state::videoram_w )
{
	m_videoram[offset] = data;
	m_fg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER( zodiack_state::videoram2_w )
{
	m_videoram_2[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER( zodiack_state::attributes_w )
{
	if ((offset & 1) && m_attributeram[offset] != data)
	{
		int i;

		for (i = offset / 2; i < m_videoram.bytes(); i += 32)
		{
			m_bg_tilemap->mark_tile_dirty(i);
			m_fg_tilemap->mark_tile_dirty(i);
		}
	}

	m_attributeram[offset] = data;
}

WRITE8_MEMBER( zodiack_state::flipscreen_w )
{
	if (m_flipscreen != (~data & 1))
	{
		m_flipscreen = ~data & 1;
		machine().tilemap().set_flip_all(m_flipscreen ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
		machine().tilemap().mark_all_dirty();
	}
}

PALETTE_INIT_MEMBER(zodiack_state,zodiack)
{
	const UINT8 *color_prom = memregion("proms")->base();
	int i;

	/* allocate the colortable */
	machine().colortable = colortable_alloc(machine(), 0x31);

	/* create a lookup table for the palette */
	for (i = 0; i < 0x30; i++)
	{
		int bit0, bit1, bit2;
		int r, g, b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		/* blue component */
		bit0 = 0;
		bit1 = (color_prom[i] >> 6) & 0x01;
		bit2 = (color_prom[i] >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		colortable_palette_set_color(machine().colortable, i, MAKE_RGB(r, g, b));
	}

	/* white for bullets */
	colortable_palette_set_color(machine().colortable, 0x30, RGB_WHITE);

	for (i = 0; i < 0x20; i++)
		if ((i & 3) == 0)
			colortable_entry_set_value(machine().colortable, i, 0);

	for (i = 0; i < 0x10; i += 2)
	{
		colortable_entry_set_value(machine().colortable, 0x20 + i, 32 + (i / 2));
		colortable_entry_set_value(machine().colortable, 0x21 + i, 40 + (i / 2));
	}

	/* bullet */
	colortable_entry_set_value(machine().colortable, 0x30, 0);
	colortable_entry_set_value(machine().colortable, 0x31, 0x30);
}

TILE_GET_INFO_MEMBER(zodiack_state::get_bg_tile_info)
{
	int code = m_videoram_2[tile_index];
	int color = (m_attributeram[(tile_index & 0x1f) << 1 | 1] >> 4) & 0x07;

	SET_TILE_INFO_MEMBER(0, code, color, 0);
}

TILE_GET_INFO_MEMBER(zodiack_state::get_fg_tile_info)
{
	int code = m_videoram[tile_index];
	int color = (m_attributeram[(tile_index & 0x1f) << 1 | 1] >> 0) & 0x07;

	SET_TILE_INFO_MEMBER(3, code, color, 0);
}

void zodiack_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(zodiack_state::get_bg_tile_info),this), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);
	m_fg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(zodiack_state::get_fg_tile_info),this), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	m_fg_tilemap->set_transparent_pen(0);
	m_fg_tilemap->set_scroll_cols(32);

	m_bg_tilemap->set_scrolldx(0, 396 - 256);
	m_fg_tilemap->set_scrolldx(0, 396 - 256);
}

void zodiack_state::draw_bullets( bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	for (int offs = 0; offs < m_bulletsram.bytes(); offs += 4)
	{
		int sx = m_bulletsram[offs + 3] + 7;
		int sy = m_bulletsram[offs + 1];

		if (!(m_flipscreen && m_percuss_hardware))
			sy = 255 - sy;

		drawgfx_transpen(
			bitmap,
			cliprect, machine().gfx[2],
			0,  /* this is just a dot, generated by the hardware */
			0,
			0, 0,
			sx, sy, 0);
	}
}

void zodiack_state::draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	for (int offs = m_spriteram.bytes() - 4; offs >= 0; offs -= 4)
	{
		int sx = 240 - m_spriteram[offs + 3];
		int sy = 240 - m_spriteram[offs];
		int flipx = !(m_spriteram[offs + 1] & 0x40);
		int flipy = m_spriteram[offs + 1] & 0x80;
		int spritecode = m_spriteram[offs + 1] & 0x3f;

		if (m_flipscreen && m_percuss_hardware)
		{
			sy = 240 - sy;
			flipy = !flipy;
		}

		drawgfx_transpen(bitmap,cliprect,machine().gfx[1],
			spritecode,
			m_spriteram[offs + 2] & 0x07,
			flipx, flipy,
			sx, sy, 0);
	}
}

UINT32 zodiack_state::screen_update( screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	for (int i = 0; i < 32; i++)
		m_fg_tilemap->set_scrolly(i, m_attributeram[i * 2]);

	m_bg_tilemap->draw(bitmap, cliprect, 0, 0);
	m_fg_tilemap->draw(bitmap, cliprect, 0, 0);
	draw_bullets(bitmap, cliprect);
	draw_sprites(bitmap, cliprect);
	return 0;
}
