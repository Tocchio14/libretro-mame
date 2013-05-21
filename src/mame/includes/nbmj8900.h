class nbmj8900_state : public driver_device
{
public:
	enum
	{
		TIMER_BLITTER
	};

	nbmj8900_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_maincpu(*this, "maincpu") { }

	int m_scrolly;
	int m_blitter_destx;
	int m_blitter_desty;
	int m_blitter_sizex;
	int m_blitter_sizey;
	int m_blitter_src_addr;
	int m_blitter_direction_x;
	int m_blitter_direction_y;
	int m_vram;
	int m_gfxrom;
	int m_dispflag;
	int m_flipscreen;
	int m_clutsel;
	int m_screen_refresh;
	int m_gfxdraw_mode;
	int m_screen_height;
	int m_screen_width;
	bitmap_ind16 m_tmpbitmap0;
	bitmap_ind16 m_tmpbitmap1;
	UINT8 *m_videoram0;
	UINT8 *m_videoram1;
	UINT8 *m_palette;
	UINT8 *m_clut;
	int m_flipscreen_old;
	DECLARE_READ8_MEMBER(nbmj8900_palette_type1_r);
	DECLARE_WRITE8_MEMBER(nbmj8900_palette_type1_w);
	DECLARE_READ8_MEMBER(nbmj8900_palette_type2_r);
	DECLARE_WRITE8_MEMBER(nbmj8900_palette_type2_w);
	DECLARE_READ8_MEMBER(nbmj8900_palette_type3_r);
	DECLARE_WRITE8_MEMBER(nbmj8900_palette_type3_w);
	DECLARE_WRITE8_MEMBER(nbmj8900_clutsel_w);
	DECLARE_READ8_MEMBER(nbmj8900_clut_r);
	DECLARE_WRITE8_MEMBER(nbmj8900_clut_w);
	DECLARE_WRITE8_MEMBER(nbmj8900_blitter_w);
	DECLARE_WRITE8_MEMBER(nbmj8900_scrolly_w);
	DECLARE_WRITE8_MEMBER(nbmj8900_vramsel_w);
	DECLARE_WRITE8_MEMBER(nbmj8900_romsel_w);
	DECLARE_DRIVER_INIT(togenkyo);
	DECLARE_DRIVER_INIT(ohpaipee);
	virtual void video_start();
	UINT32 screen_update_nbmj8900(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void nbmj8900_vramflip(int vram);
	void update_pixel0(int x, int y);
	void update_pixel1(int x, int y);
	void nbmj8900_gfxdraw();
	required_device<cpu_device> m_maincpu;

protected:
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
};
