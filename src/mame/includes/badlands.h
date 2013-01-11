/*************************************************************************

    Atari Bad Lands hardware

*************************************************************************/

#include "machine/atarigen.h"

class badlands_state : public atarigen_state
{
public:
	badlands_state(const machine_config &mconfig, device_type type, const char *tag)
		: atarigen_state(mconfig, type, tag) { }

	UINT8           m_pedal_value[2];

	UINT8 *         m_bank_base;
	UINT8 *         m_bank_source_data;

	UINT8           m_playfield_tile_bank;
	virtual void update_interrupts();
	virtual void scanline_update(screen_device &screen, int scanline);
	DECLARE_READ16_MEMBER(sound_busy_r);
	DECLARE_READ16_MEMBER(pedal_0_r);
	DECLARE_READ16_MEMBER(pedal_1_r);
	DECLARE_READ8_MEMBER(audio_io_r);
	DECLARE_WRITE8_MEMBER(audio_io_w);
	DECLARE_READ16_MEMBER(badlandsb_unk_r);
	DECLARE_DRIVER_INIT(badlands);
	TILE_GET_INFO_MEMBER(get_playfield_tile_info);
	DECLARE_MACHINE_START(badlands);
	DECLARE_MACHINE_RESET(badlands);
	DECLARE_VIDEO_START(badlands);
	DECLARE_MACHINE_RESET(badlandsb);
	UINT32 screen_update_badlands(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(vblank_int);
};

/*----------- defined in video/badlands.c -----------*/
DECLARE_WRITE16_HANDLER( badlands_pf_bank_w );
