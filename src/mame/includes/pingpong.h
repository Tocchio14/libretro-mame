class pingpong_state : public driver_device
{
public:
	pingpong_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this,"maincpu")
		{ }

	int m_intenable;
	int m_question_addr_high;
	UINT8 *m_videoram;
	UINT8 *m_colorram;
	tilemap_t *m_bg_tilemap;
	UINT8 *m_spriteram;
	size_t m_spriteram_size;

	required_device<cpu_device> m_maincpu;
	DECLARE_WRITE8_MEMBER(cashquiz_question_bank_high_w);
	DECLARE_WRITE8_MEMBER(cashquiz_question_bank_low_w);
	DECLARE_WRITE8_MEMBER(coin_w);
	DECLARE_WRITE8_MEMBER(pingpong_videoram_w);
	DECLARE_WRITE8_MEMBER(pingpong_colorram_w);
};


/*----------- defined in video/pingpong.c -----------*/


PALETTE_INIT( pingpong );
VIDEO_START( pingpong );
SCREEN_UPDATE_IND16( pingpong );
