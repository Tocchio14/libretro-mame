/***************************************************************************

    Sega X-board hardware

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#include "emu.h"
#include "includes/segaxbd.h"


//**************************************************************************
//  VIDEO STARTUP
//**************************************************************************

void segaxbd_state::video_start()
{
	// compute palette info
	segaic16_palette_init(0x2000);

	// initialize the tile/text layers
	segaic16_tilemap_init(machine(), 0, SEGAIC16_TILEMAP_16B, 0x1c00, 0, 2);

	// initialize the road
	segaic16_road_init(machine(), 0, SEGAIC16_ROAD_XBOARD, 0x1700, 0x1720, 0x1780, -166);
}



//**************************************************************************
//  VIDEO UPDATE
//**************************************************************************

UINT32 segaxbd_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	// if no drawing is happening, fill with black and get out
	if (!segaic16_display_enable)
	{
		bitmap.fill(get_black_pen(machine()), cliprect);
		return 0;
	}

	// reset priorities
	machine().priority_bitmap.fill(0, cliprect);

	// draw the low priority road layer
	segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_BACKGROUND);
	if (m_road_priority == 0)
		segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_FOREGROUND);

	// draw background
	segaic16_tilemap_draw(screen, bitmap, cliprect, 0, SEGAIC16_TILEMAP_BACKGROUND, 0, 0x01);
	segaic16_tilemap_draw(screen, bitmap, cliprect, 0, SEGAIC16_TILEMAP_BACKGROUND, 1, 0x02);

	// draw foreground
	segaic16_tilemap_draw(screen, bitmap, cliprect, 0, SEGAIC16_TILEMAP_FOREGROUND, 0, 0x02);
	segaic16_tilemap_draw(screen, bitmap, cliprect, 0, SEGAIC16_TILEMAP_FOREGROUND, 1, 0x04);

	// draw the high priority road
	if (m_road_priority == 1)
		segaic16_road_draw(0, bitmap, cliprect, SEGAIC16_ROAD_FOREGROUND);

	// text layer
	segaic16_tilemap_draw(screen, bitmap, cliprect, 0, SEGAIC16_TILEMAP_TEXT, 0, 0x04);
	segaic16_tilemap_draw(screen, bitmap, cliprect, 0, SEGAIC16_TILEMAP_TEXT, 1, 0x08);

	// draw the sprites
	segaic16_sprites_draw(screen, bitmap, cliprect, 0);
	return 0;
}
