/*************************************************************************

    Atari Battle Zone hardware

*************************************************************************/

#define BZONE_DISCRETE		(1)

#define BZONE_MASTER_CLOCK (XTAL_12_096MHz)
#define BZONE_CLOCK_3KHZ  (MASTER_CLOCK / 4096)

/*----------- defined in drivers/bzone.c -----------*/

extern UINT8 rb_input_select;


/*----------- defined in audio/bzone.c -----------*/

#if BZONE_DISCRETE

WRITE8_DEVICE_HANDLER( bzone_sounds_w );

MACHINE_DRIVER_EXTERN( bzone_audio );
#else
WRITE8_HANDLER( bzone_sounds_w );

DEVICE_GET_INFO( bzone_sound );
#define SOUND_BZONE DEVICE_GET_INFO_NAME(bzone_sound)
#endif

/*----------- defined in audio/redbaron.c -----------*/

WRITE8_HANDLER( redbaron_sounds_w );
WRITE8_DEVICE_HANDLER( redbaron_pokey_w );

DEVICE_GET_INFO( redbaron_sound );
#define SOUND_REDBARON DEVICE_GET_INFO_NAME(redbaron_sound)
