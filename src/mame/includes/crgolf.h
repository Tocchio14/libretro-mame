/*************************************************************************

    Kitco Crowns Golf hardware

**************************************************************************/

#define MASTER_CLOCK		18432000


class crgolf_state : public driver_device
{
public:
	crgolf_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT8 *  videoram_a;
	UINT8 *  videoram_b;
	UINT8 *  color_select;
	UINT8 *  screen_flip;
	UINT8 *  screen_select;
	UINT8 *  screena_enable;
	UINT8 *  screenb_enable;

	/* misc */
	UINT8    port_select;
	UINT8    main_to_sound_data, sound_to_main_data;
	UINT16   sample_offset;
	UINT8    sample_count;

	/* devices */
	device_t *maincpu;
	device_t *audiocpu;
};

/*----------- defined in video/crgolf.c -----------*/

WRITE8_HANDLER( crgolf_videoram_w );
READ8_HANDLER( crgolf_videoram_r );

MACHINE_CONFIG_EXTERN( crgolf_video );
