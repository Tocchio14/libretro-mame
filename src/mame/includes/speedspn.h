class speedspn_state : public driver_device
{
public:
	speedspn_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	UINT8 *attram;
	tilemap_t *tilemap;
	UINT8 display_disable;
	int bank_vidram;
	UINT8* vidram;
};


/*----------- defined in video/speedspn.c -----------*/

VIDEO_START( speedspn );
SCREEN_UPDATE( speedspn );

WRITE8_HANDLER( speedspn_vidram_w );
WRITE8_HANDLER( speedspn_attram_w );
READ8_HANDLER( speedspn_vidram_r );
WRITE8_HANDLER( speedspn_banked_vidram_change );
WRITE8_HANDLER( speedspn_global_display_w );

