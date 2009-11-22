


typedef struct _aquarium_state aquarium_state;
struct _aquarium_state
{
	/* memory pointers */
	UINT16 *  scroll;
	UINT16 *  txt_videoram;
	UINT16 *  mid_videoram;
	UINT16 *  bak_videoram;
	UINT16 *  spriteram;
//	UINT16 *  paletteram16;	// currently this uses generic palette handling

	/* video-related */
	tilemap  *txt_tilemap, *mid_tilemap, *bak_tilemap;

	/* misc */
	int aquarium_snd_ack;
};


/*----------- defined in video/aquarium.c -----------*/

WRITE16_HANDLER( aquarium_txt_videoram_w );
WRITE16_HANDLER( aquarium_mid_videoram_w );
WRITE16_HANDLER( aquarium_bak_videoram_w );

VIDEO_START(aquarium);
VIDEO_UPDATE(aquarium);
