/*

  Malzak

  Video functions

  SAA 5050 -- Character display
  S2636 (x2) -- Sprites, Sprite->Sprite collisions
  Playfield graphics generator
      (TODO: probably best to switch this to tilemaps one day, figure out banking)

*/


#include "driver.h"
#include "video/s2636.h"
#include "video/saa5050.h"

int malzak_x;
int malzak_y;

static struct playfield
{
	//int x;
	//int y;
	int code;
} field[256];

VIDEO_UPDATE( malzak )
{
	int sx, sy;
	int x,y;
	bitmap_t *s2636_0_bitmap;
	bitmap_t *s2636_1_bitmap;
	const device_config *s2636_0 = devtag_get_device(screen->machine, "s2636_0");
	const device_config *s2636_1 = devtag_get_device(screen->machine, "s2636_1");
	const device_config *saa5050 = devtag_get_device(screen->machine, "saa5050");

	bitmap_fill(bitmap, 0, 0);

	saa5050_update(saa5050, bitmap, cliprect);
	saa5050_frame_advance(saa5050);

	// playfield - not sure exactly how this works...
	for(x = 0; x < 16; x++)
		for(y = 0; y < 16; y++)
		{
			sx = ((x * 16 - 48) - malzak_x);
			sy = ((y * 16) - malzak_y);

			if(sx < -271)
				sx += 512;
			if(sx < -15)
				sx += 256;

			drawgfx_transpen(bitmap,cliprect, screen->machine->gfx[0], field[x * 16 + y].code,7,0,0, sx, sy, 0);
		}

	/* update the S2636 chips */
	s2636_0_bitmap = s2636_update(s2636_0, cliprect);
	s2636_1_bitmap = s2636_update(s2636_1, cliprect);

	/* copy the S2636 images into the main bitmap */
	{
		int y;

		for (y = cliprect->min_y; y <= cliprect->max_y; y++)
		{
			int x;

			for (x = cliprect->min_x; x <= cliprect->max_x; x++)
			{
				int pixel0 = *BITMAP_ADDR16(s2636_0_bitmap, y, x);
				int pixel1 = *BITMAP_ADDR16(s2636_1_bitmap, y, x);

				if (S2636_IS_PIXEL_DRAWN(pixel0))
					*BITMAP_ADDR16(bitmap, y, x) = S2636_PIXEL_COLOR(pixel0);

				if (S2636_IS_PIXEL_DRAWN(pixel1))
					*BITMAP_ADDR16(bitmap, y, x) = S2636_PIXEL_COLOR(pixel1);
			}
		}
	}

	return 0;
}

WRITE8_HANDLER( malzak_playfield_w )
{
	int tile = ((malzak_x / 16) * 16) + (offset / 16);

//  field[tile].x = malzak_x / 16;
//  field[tile].y = malzak_y;
	field[tile].code = (data & 0x1f);
	logerror("GFX: 0x16%02x write 0x%02x\n",offset,data);
}
