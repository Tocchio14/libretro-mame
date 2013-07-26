/*************************************************************/
/*                                                           */
/* Zaccaria/Zelco S2650 based games video                    */
/*                                                           */
/*************************************************************/

#include "emu.h"
#include "sound/s2636.h"
#include "includes/zac2650.h"


/**************************************************************/
/* The S2636 is a standard sprite chip used by several boards */
/* Emulation of this chip may be moved into a separate unit   */
/* once it's workings are fully understood.                   */
/**************************************************************/

WRITE8_MEMBER(zac2650_state::tinvader_videoram_w)
{
	UINT8 *videoram = m_videoram;
	videoram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}

READ8_MEMBER(zac2650_state::zac_s2636_r)
{
	if(offset!=0xCB) return m_s2636_0_ram[offset];
	else return m_CollisionSprite;
}

WRITE8_MEMBER(zac2650_state::zac_s2636_w)
{
	m_s2636_0_ram[offset] = data;
	machine().gfx[1]->mark_dirty(offset/8);
	machine().gfx[2]->mark_dirty(offset/8);
	if (offset == 0xc7)
	{
		s2636_soundport_w(machine().device("s2636snd"), 0, data);
	}
}

READ8_MEMBER(zac2650_state::tinvader_port_0_r)
{
	return ioport("1E80")->read() - m_CollisionBackground;
}

/*****************************************/
/* Check for Collision between 2 sprites */
/*****************************************/

int zac2650_state::SpriteCollision(int first,int second)
{
	int Checksum=0;
	int x,y;
	const rectangle &visarea = machine().primary_screen->visible_area();

	if((m_s2636_0_ram[first * 0x10 + 10] < 0xf0) && (m_s2636_0_ram[second * 0x10 + 10] < 0xf0))
	{
		int fx     = (m_s2636_0_ram[first * 0x10 + 10] * 4)-22;
		int fy     = (m_s2636_0_ram[first * 0x10 + 12] * 3)+3;
		int expand = (first==1) ? 2 : 1;

		/* Draw first sprite */

		drawgfx_opaque(m_spritebitmap,m_spritebitmap.cliprect(), machine().gfx[expand],
				first * 2,
				0,
				0,0,
				fx,fy);

		/* Get fingerprint */

		for (x = fx; x < fx + machine().gfx[expand]->width(); x++)
		{
			for (y = fy; y < fy + machine().gfx[expand]->height(); y++)
			{
				if (visarea.contains(x, y))
					Checksum += m_spritebitmap.pix16(y, x);
			}
		}

		/* Blackout second sprite */

		drawgfx_transpen(m_spritebitmap,m_spritebitmap.cliprect(), machine().gfx[1],
				second * 2,
				1,
				0,0,
				(m_s2636_0_ram[second * 0x10 + 10] * 4)-22,(m_s2636_0_ram[second * 0x10 + 12] * 3) + 3, 0);

		/* Remove fingerprint */

		for (x = fx; x < fx + machine().gfx[expand]->width(); x++)
		{
			for (y = fy; y < fy + machine().gfx[expand]->height(); y++)
			{
				if (visarea.contains(x, y))
					Checksum -= m_spritebitmap.pix16(y, x);
			}
		}

		/* Zero bitmap */

		drawgfx_opaque(m_spritebitmap,m_spritebitmap.cliprect(), machine().gfx[expand],
				first * 2,
				1,
				0,0,
				fx,fy);
	}

	return Checksum;
}

TILE_GET_INFO_MEMBER(zac2650_state::get_bg_tile_info)
{
	UINT8 *videoram = m_videoram;
	int code = videoram[tile_index];

	SET_TILE_INFO_MEMBER(0, code, 0, 0);
}

void zac2650_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(zac2650_state::get_bg_tile_info),this), TILEMAP_SCAN_ROWS,
			24, 24, 32, 32);

	machine().primary_screen->register_screen_bitmap(m_bitmap);
	machine().primary_screen->register_screen_bitmap(m_spritebitmap);

	machine().gfx[1]->set_source(m_s2636_0_ram);
	machine().gfx[2]->set_source(m_s2636_0_ram);
}

void zac2650_state::draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int offs;
	const rectangle &visarea = machine().primary_screen->visible_area();

	/* -------------------------------------------------------------- */
	/* There seems to be a strange setup with this board, in that it  */
	/* appears that the S2636 runs from a different clock than the    */
	/* background generator, When the program maps sprite position to */
	/* character position it only has 6 pixels of sprite for 8 pixels */
	/* of character.                                                  */
	/* -------------------------------------------------------------- */
	/* n.b. The original has several graphic glitches as well, so it  */
	/* does not seem to be a fault of the emulation!                  */
	/* -------------------------------------------------------------- */

	m_CollisionBackground = 0;   /* Read from 0x1e80 bit 7 */

	// for collision detection checking
	copybitmap(m_bitmap,bitmap,0,0,0,0,visarea);

	for(offs=0;offs<0x50;offs+=0x10)
	{
		if((m_s2636_0_ram[offs+10]<0xF0) && (offs!=0x30))
		{
			int spriteno = (offs / 8);
			int expand   = ((m_s2636_0_ram[0xc0] & (spriteno*2))!=0) ? 2 : 1;
			int bx       = (m_s2636_0_ram[offs+10] * 4) - 22;
			int by       = (m_s2636_0_ram[offs+12] * 3) + 3;
			int x,y;

			/* Sprite->Background collision detection */
			drawgfx_transpen(bitmap,cliprect, machine().gfx[expand],
					spriteno,
					1,
					0,0,
					bx,by, 0);

			for (x = bx; x < bx + machine().gfx[expand]->width(); x++)
			{
				for (y = by; y < by + machine().gfx[expand]->height(); y++)
				{
					if (visarea.contains(x, y))
						if (bitmap.pix16(y, x) != m_bitmap.pix16(y, x))
						{
							m_CollisionBackground = 0x80;
							break;
						}
				}
			}

			drawgfx_transpen(bitmap,cliprect, machine().gfx[expand],
					spriteno,
					0,
					0,0,
					bx,by, 0);
		}
	}

	/* Sprite->Sprite collision detection */
	m_CollisionSprite = 0;
//  if(SpriteCollision(0,1)) m_CollisionSprite |= 0x20;   /* Not Used */
	if(SpriteCollision(0,2)) m_CollisionSprite |= 0x10;
	if(SpriteCollision(0,4)) m_CollisionSprite |= 0x08;
	if(SpriteCollision(1,2)) m_CollisionSprite |= 0x04;
	if(SpriteCollision(1,4)) m_CollisionSprite |= 0x02;
//  if(SpriteCollision(2,4)) m_CollisionSprite |= 0x01;   /* Not Used */
}

UINT32 zac2650_state::screen_update_tinvader(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect);
	return 0;
}
