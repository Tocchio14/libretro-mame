class pooyan_state : public driver_device
{
public:
	pooyan_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT8 *  m_videoram;
	UINT8 *  m_colorram;
	UINT8 *  m_spriteram;
	UINT8 *  m_spriteram2;

	/* video-related */
	tilemap_t  *m_bg_tilemap;

	/* misc */
	UINT8    m_irq_toggle;
	UINT8    m_irq_enable;

	/* devices */
	cpu_device *m_maincpu;
};


/*----------- defined in video/pooyan.c -----------*/

WRITE8_HANDLER( pooyan_videoram_w );
WRITE8_HANDLER( pooyan_colorram_w );
WRITE8_HANDLER( pooyan_flipscreen_w );

PALETTE_INIT( pooyan );
VIDEO_START( pooyan );
SCREEN_UPDATE( pooyan );
