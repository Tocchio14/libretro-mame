/*************************************************************************

    Taito Dual Screen Games

*************************************************************************/

#include "sound/flt_vol.h"
#include "audio/taitosnd.h"

class warriorb_state : public driver_device
{
public:
	warriorb_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_spriteram(*this, "spriteram"),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_tc0140syt(*this, "tc0140syt"),
		m_tc0100scn_1(*this, "tc0100scn_1"),
		m_tc0100scn_2(*this, "tc0100scn_2"),
		m_2610_1l(*this, "2610.1.l"),
		m_2610_1r(*this, "2610.1.r"),
		m_2610_2l(*this, "2610.2.l"),
		m_2610_2r(*this, "2610.2.r") { }

	/* memory pointers */
	required_shared_ptr<UINT16> m_spriteram;

	/* misc */
	INT32      m_banknum;
	int        m_pandata[4];

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<tc0140syt_device> m_tc0140syt;
	required_device<tc0100scn_device> m_tc0100scn_1;
	required_device<tc0100scn_device> m_tc0100scn_2;
	required_device<filter_volume_device> m_2610_1l;
	required_device<filter_volume_device> m_2610_1r;
	required_device<filter_volume_device> m_2610_2l;
	required_device<filter_volume_device> m_2610_2r;
	DECLARE_WRITE8_MEMBER(sound_bankswitch_w);
	DECLARE_WRITE16_MEMBER(warriorb_sound_w);
	DECLARE_READ16_MEMBER(warriorb_sound_r);
	DECLARE_WRITE8_MEMBER(warriorb_pancontrol);
	DECLARE_WRITE16_MEMBER(tc0100scn_dual_screen_w);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_warriorb_left(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_warriorb_right(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void reset_sound_region();
	void draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect, int x_offs, int y_offs );
	UINT32 update_screen(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int xoffs, device_t *tc0100scn);
	DECLARE_WRITE_LINE_MEMBER(irqhandler);
};
