/*************************************************************************

    Deniam games

*************************************************************************/


class deniam_state : public driver_device
{
public:
	deniam_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT16 *       videoram;
	UINT16 *       textram;
	UINT16 *       spriteram;
	UINT16 *       paletteram;
	size_t         spriteram_size;

	/* video-related */
	tilemap_t        *fg_tilemap, *bg_tilemap, *tx_tilemap;
	int            display_enable;
	int            bg_scrollx_offs, bg_scrolly_offs;
	int            fg_scrollx_offs, fg_scrolly_offs;
	int            bg_scrollx_reg, bg_scrolly_reg, bg_page_reg;
	int            fg_scrollx_reg, fg_scrolly_reg, fg_page_reg;
	int            bg_page[4], fg_page[4];
	UINT16         coinctrl;

	/* devices */
	device_t *audio_cpu;	// system 16c does not have sound CPU
};


/*----------- defined in video/deniam.c -----------*/

WRITE16_HANDLER( deniam_videoram_w );
WRITE16_HANDLER( deniam_textram_w );
WRITE16_HANDLER( deniam_palette_w );
READ16_HANDLER( deniam_coinctrl_r );
WRITE16_HANDLER( deniam_coinctrl_w );

VIDEO_START( deniam );
SCREEN_UPDATE( deniam );

DRIVER_INIT( logicpro );
DRIVER_INIT( karianx );
