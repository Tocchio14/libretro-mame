class deco_mlc_state : public driver_device
{
public:
	deco_mlc_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_mlc_ram(*this, "mlc_ram"),
		m_irq_ram(*this, "irq_ram"),
		m_mlc_clip_ram(*this, "mlc_clip_ram"),
		m_mlc_vram(*this, "mlc_vram")
	
	{ }

	required_shared_ptr<UINT32> m_mlc_ram;
	required_shared_ptr<UINT32> m_irq_ram;
	required_shared_ptr<UINT32> m_mlc_clip_ram;
	required_shared_ptr<UINT32> m_mlc_vram;
	timer_device *m_raster_irq_timer;
	int m_mainCpuIsArm;
	UINT32 m_mlc_raster_table_1[4*256];
	UINT32 m_mlc_raster_table_2[4*256];
	UINT32 m_mlc_raster_table_3[4*256];
	UINT32 m_vbl_i;
	int m_lastScanline[9];
	UINT32 m_colour_mask;

	UINT16 *m_mlc_spriteram;
	UINT16 *m_mlc_spriteram_spare;
	UINT16 *m_mlc_buffered_spriteram;
	DECLARE_READ32_MEMBER(test2_r);
	DECLARE_READ32_MEMBER(mlc_440000_r);
	DECLARE_READ32_MEMBER(mlc_440004_r);
	DECLARE_READ32_MEMBER(mlc_440008_r);
	DECLARE_READ32_MEMBER(mlc_44001c_r);
	DECLARE_WRITE32_MEMBER(mlc_44001c_w);

	DECLARE_WRITE32_MEMBER(avengrs_palette_w);
	DECLARE_READ32_MEMBER(mlc_200000_r);
	DECLARE_READ32_MEMBER(mlc_200004_r);
	DECLARE_READ32_MEMBER(mlc_200070_r);
	DECLARE_READ32_MEMBER(mlc_20007c_r);
	DECLARE_READ32_MEMBER(mlc_scanline_r);
	DECLARE_WRITE32_MEMBER(mlc_irq_w);
	DECLARE_READ32_MEMBER(mlc_vram_r);
	DECLARE_READ32_MEMBER(stadhr96_prot_146_r);
	DECLARE_WRITE32_MEMBER(stadhr96_prot_146_w);
	DECLARE_READ32_MEMBER(avengrgs_speedup_r);
	DECLARE_WRITE32_MEMBER(avengrs_eprom_w);
	DECLARE_READ32_MEMBER(mlc_spriteram_r);
	DECLARE_WRITE32_MEMBER(mlc_spriteram_w);

	

	DECLARE_DRIVER_INIT(mlc);
	DECLARE_DRIVER_INIT(avengrgs);
	DECLARE_MACHINE_RESET(mlc);
	DECLARE_VIDEO_START(mlc);
	UINT32 screen_update_mlc(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void screen_eof_mlc(screen_device &screen, bool state);
	TIMER_DEVICE_CALLBACK_MEMBER(interrupt_gen);
	void blitRaster(bitmap_rgb32 &bitmap, int rasterMode);
	void draw_sprites( const rectangle &cliprect, int scanline, UINT32* dest);
	void descramble_sound(  );
};
