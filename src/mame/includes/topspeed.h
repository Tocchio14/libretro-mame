/*************************************************************************

    Top Speed / Full Throttle

*************************************************************************/

class topspeed_state : public driver_device
{
public:
	topspeed_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	/* memory pointers */
	UINT16 *   m_spritemap;
	UINT16 *   m_raster_ctrl;
	UINT16 *   m_spriteram;
	UINT16 *   m_sharedram;
//  UINT16 *    paletteram;    // currently this uses generic palette handling
	size_t     m_spriteram_size;
	size_t     m_sharedram_size;

	/* misc */
	UINT16     m_cpua_ctrl;
	INT32      m_ioc220_port;
	INT32      m_banknum;
	int        m_adpcm_pos;
	int        m_adpcm_data;

	/* devices */
	device_t *m_maincpu;
	device_t *m_audiocpu;
	device_t *m_subcpu;
	device_t *m_pc080sn_1;
	device_t *m_pc080sn_2;
	device_t *m_tc0220ioc;

	UINT8 m_dislayer[5];
	DECLARE_READ16_MEMBER(sharedram_r);
	DECLARE_WRITE16_MEMBER(sharedram_w);
	DECLARE_WRITE16_MEMBER(cpua_ctrl_w);
	DECLARE_READ8_MEMBER(topspeed_input_bypass_r);
	DECLARE_READ16_MEMBER(topspeed_motor_r);
	DECLARE_WRITE16_MEMBER(topspeed_motor_w);
};


/*----------- defined in video/topspeed.c -----------*/

SCREEN_UPDATE_IND16( topspeed );
