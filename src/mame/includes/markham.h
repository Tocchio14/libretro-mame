/*************************************************************************

    Markham

*************************************************************************/

typedef struct _markham_state markham_state;
struct _markham_state
{
	/* memory pointers */
	UINT8 *    videoram;
	UINT8 *    spriteram;
	UINT8 *    xscroll;
	size_t     spriteram_size;

	/* video-related */
	tilemap_t  *bg_tilemap;
};


/*----------- defined in video/markham.c -----------*/

WRITE8_HANDLER( markham_videoram_w );
WRITE8_HANDLER( markham_flipscreen_w );

PALETTE_INIT( markham );
VIDEO_START( markham );
VIDEO_UPDATE( markham );
