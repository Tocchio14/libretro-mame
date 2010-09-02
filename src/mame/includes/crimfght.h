/*************************************************************************

    Crime Fighters

*************************************************************************/

class crimfght_state : public driver_device
{
public:
	crimfght_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
//  UINT8 *    paletteram;    // currently this uses generic palette handling

	/* video-related */
	int        layer_colorbase[3], sprite_colorbase;

	/* devices */
	running_device *maincpu;
	running_device *audiocpu;
	running_device *k007232;
	running_device *k052109;
	running_device *k051960;
};

/*----------- defined in video/crimfght.c -----------*/

extern void crimfght_tile_callback(running_machine *machine, int layer,int bank,int *code,int *color,int *flags,int *priority);
extern void crimfght_sprite_callback(running_machine *machine, int *code,int *color,int *priority,int *shadow);

VIDEO_START( crimfght );
VIDEO_UPDATE( crimfght );
