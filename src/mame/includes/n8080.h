#include "sound/dac.h"

class n8080_state : public driver_device
{
public:
	n8080_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_colorram(*this, "colorram"),
		m_maincpu(*this, "maincpu"),
		m_dac(*this, "dac") { }

	/* memory pointers */
	required_shared_ptr<UINT8> m_videoram;
	optional_shared_ptr<UINT8> m_colorram;      // for helifire

	/* video-related */
	emu_timer* m_cannon_timer;
	int m_spacefev_red_screen;
	int m_spacefev_red_cannon;
	int m_sheriff_color_mode;
	int m_sheriff_color_data;
	int m_helifire_flash;
	UINT8 m_helifire_LSFR[63];
	unsigned m_helifire_mv;
	unsigned m_helifire_sc; /* IC56 */

	/* sound-related */
	int m_n8080_hardware;
	emu_timer* m_sound_timer[3];
	int m_helifire_dac_phase;
	double m_helifire_dac_volume;
	double m_helifire_dac_timing;
	UINT16 m_prev_sound_pins;
	UINT16 m_curr_sound_pins;
	int m_mono_flop[3];
	UINT8 m_prev_snd_data;

	/* other */
	unsigned m_shift_data;
	unsigned m_shift_bits;
	int m_inte;

	/* devices */
	required_device<cpu_device> m_maincpu;
	DECLARE_WRITE8_MEMBER(n8080_shift_bits_w);
	DECLARE_WRITE8_MEMBER(n8080_shift_data_w);
	DECLARE_READ8_MEMBER(n8080_shift_r);
	DECLARE_WRITE8_MEMBER(n8080_video_control_w);
	DECLARE_WRITE8_MEMBER(n8080_sound_1_w);
	DECLARE_WRITE8_MEMBER(n8080_sound_2_w);
	DECLARE_READ8_MEMBER(n8080_8035_p1_r);
	DECLARE_READ8_MEMBER(n8080_8035_t0_r);
	DECLARE_READ8_MEMBER(n8080_8035_t1_r);
	DECLARE_READ8_MEMBER(helifire_8035_t0_r);
	DECLARE_READ8_MEMBER(helifire_8035_t1_r);
	DECLARE_READ8_MEMBER(helifire_8035_external_ram_r);
	DECLARE_READ8_MEMBER(helifire_8035_p2_r);
	DECLARE_WRITE8_MEMBER(n8080_dac_w);
	DECLARE_WRITE8_MEMBER(helifire_dac_w);
	DECLARE_WRITE8_MEMBER(helifire_sound_ctrl_w);
	DECLARE_WRITE_LINE_MEMBER(n8080_inte_callback);
	DECLARE_WRITE8_MEMBER(n8080_status_callback);
	DECLARE_MACHINE_START(spacefev);
	DECLARE_MACHINE_RESET(spacefev);
	DECLARE_VIDEO_START(spacefev);
	DECLARE_PALETTE_INIT(n8080);
	DECLARE_MACHINE_START(sheriff);
	DECLARE_MACHINE_RESET(sheriff);
	DECLARE_VIDEO_START(sheriff);
	DECLARE_MACHINE_START(helifire);
	DECLARE_MACHINE_RESET(helifire);
	DECLARE_VIDEO_START(helifire);
	DECLARE_PALETTE_INIT(helifire);
	DECLARE_MACHINE_START(spacefev_sound);
	DECLARE_MACHINE_RESET(spacefev_sound);
	DECLARE_MACHINE_START(sheriff_sound);
	DECLARE_MACHINE_RESET(sheriff_sound);
	DECLARE_MACHINE_START(helifire_sound);
	DECLARE_MACHINE_RESET(helifire_sound);
	DECLARE_MACHINE_START(n8080);
	DECLARE_MACHINE_RESET(n8080);
	UINT32 screen_update_spacefev(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_sheriff(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_helifire(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_eof_helifire(screen_device &screen, bool state);
	TIMER_CALLBACK_MEMBER(spacefev_stop_red_cannon);
	TIMER_DEVICE_CALLBACK_MEMBER(rst1_tick);
	TIMER_DEVICE_CALLBACK_MEMBER(rst2_tick);
	TIMER_DEVICE_CALLBACK_MEMBER(spacefev_vco_voltage_timer);
	TIMER_DEVICE_CALLBACK_MEMBER(helifire_dac_volume_timer);
	void spacefev_start_red_cannon(  );
	void helifire_next_line(  );
	required_device<dac_device> m_dac;
};

/*----------- defined in audio/n8080.c -----------*/

MACHINE_CONFIG_EXTERN( spacefev_sound );
MACHINE_CONFIG_EXTERN( sheriff_sound );
MACHINE_CONFIG_EXTERN( helifire_sound );
