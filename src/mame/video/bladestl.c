#include "emu.h"
#include "includes/bladestl.h"


PALETTE_INIT_MEMBER(bladestl_state, bladestl)
{
	const UINT8 *color_prom = memregion("proms")->base();
	int i;

	/* characters use pens 0x00-0x1f, no look-up table */
	for (i = 0; i < 0x20; i++)
		palette.set_pen_indirect(i, i);

	/* sprites use pens 0x20-0x2f */
	for (i = 0x20; i < 0x120; i++)
	{
		UINT8 ctabentry = (color_prom[i - 0x20] & 0x0f) | 0x20;
		palette.set_pen_indirect(i, ctabentry);
	}
}


void bladestl_state::set_pens()
{
	int i;

	for (i = 0x00; i < 0x60; i += 2)
	{
		UINT16 data = m_paletteram[i | 1] | (m_paletteram[i] << 8);

		rgb_t color = rgb_t(pal5bit(data >> 0), pal5bit(data >> 5), pal5bit(data >> 10));

		m_palette->set_indirect_color(i >> 1, color);
	}
}



/***************************************************************************

  Callback for the K007342

***************************************************************************/

K007342_CALLBACK_MEMBER(bladestl_state::bladestl_tile_callback)
{
	*code |= ((*color & 0x0f) << 8) | ((*color & 0x40) << 6);
	*color = m_layer_colorbase[layer];
}

/***************************************************************************

  Callback for the K007420

***************************************************************************/

void bladestl_sprite_callback( running_machine &machine, int *code,int *color )
{
	bladestl_state *state = machine.driver_data<bladestl_state>();

	*code |= ((*color & 0xc0) << 2) + state->m_spritebank;
	*code = (*code << 2) | ((*color & 0x30) >> 4);
	*color = 0 + (*color & 0x0f);
}


/***************************************************************************

  Screen Refresh

***************************************************************************/

UINT32 bladestl_state::screen_update_bladestl(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	set_pens();

	m_k007342->tilemap_update();

	m_k007342->tilemap_draw(screen, bitmap, cliprect, 1, TILEMAP_DRAW_OPAQUE ,0);
	m_k007420->sprites_draw(bitmap, cliprect, m_gfxdecode->gfx(1));
	m_k007342->tilemap_draw(screen, bitmap, cliprect, 1, 1 | TILEMAP_DRAW_OPAQUE ,0);
	m_k007342->tilemap_draw(screen, bitmap, cliprect, 0, 0 ,0);
	m_k007342->tilemap_draw(screen, bitmap, cliprect, 0, 1 ,0);
	return 0;
}
