#include "sound/sn76496.h"

class yiear_state : public driver_device
{
public:
	yiear_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_spriteram(*this, "spriteram"),
		m_spriteram2(*this, "spriteram2"),
		m_videoram(*this, "videoram"),
		m_sn(*this, "snsnd")
	{ }

	/* memory pointers */
	required_shared_ptr<UINT8> m_spriteram;
	required_shared_ptr<UINT8> m_spriteram2;
	required_shared_ptr<UINT8> m_videoram;
	optional_device<sn76489a_new_device> m_sn;

	/* video-related */
	tilemap_t  *m_bg_tilemap;

	UINT8      m_yiear_nmi_enable;
	UINT8      m_yiear_irq_enable;
	DECLARE_WRITE8_MEMBER(yiear_videoram_w);
	DECLARE_WRITE8_MEMBER(yiear_control_w);
	DECLARE_READ8_MEMBER(yiear_speech_r);
	DECLARE_WRITE8_MEMBER(yiear_VLM5030_control_w);

	UINT8 m_SN76496_latch;
	DECLARE_WRITE8_MEMBER( konami_SN76496_latch_w ) { m_SN76496_latch = data; };
	DECLARE_WRITE8_MEMBER( konami_SN76496_w ) { m_sn->write(space, offset, m_SN76496_latch); };
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
};


/*----------- defined in video/yiear.c -----------*/


PALETTE_INIT( yiear );
VIDEO_START( yiear );
SCREEN_UPDATE_IND16( yiear );
