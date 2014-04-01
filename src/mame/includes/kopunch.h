/*************************************************************************

  Sega KO Punch

*************************************************************************/

class kopunch_state : public driver_device
{
public:
	kopunch_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_vram_fg(*this, "vram_fg"),
		m_vram_bg(*this, "vram_bg")
	{ }

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;

	/* memory pointers */
	required_shared_ptr<UINT8> m_vram_fg;
	required_shared_ptr<UINT8> m_vram_bg;

	/* video-related */
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_fg_tilemap;
	UINT8 m_gfxbank;
	UINT8 m_scrollx;

	DECLARE_READ8_MEMBER(kopunch_in_r);
	DECLARE_WRITE8_MEMBER(kopunch_lamp_w);
	DECLARE_WRITE8_MEMBER(kopunch_coin_w);
	DECLARE_WRITE8_MEMBER(kopunch_fg_w);
	DECLARE_WRITE8_MEMBER(kopunch_bg_w);
	DECLARE_WRITE8_MEMBER(kopunch_scroll_x_w);
	DECLARE_WRITE8_MEMBER(kopunch_scroll_y_w);
	DECLARE_WRITE8_MEMBER(kopunch_gfxbank_w);

	DECLARE_INPUT_CHANGED_MEMBER(left_coin_inserted);
	DECLARE_INPUT_CHANGED_MEMBER(right_coin_inserted);

	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	DECLARE_PALETTE_INIT(kopunch);
	UINT32 screen_update_kopunch(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	virtual void machine_start();
	virtual void video_start();

	INTERRUPT_GEN_MEMBER(kopunch_interrupt);
};
