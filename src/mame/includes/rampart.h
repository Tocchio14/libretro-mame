/*************************************************************************

    Atari Rampart hardware

*************************************************************************/

#include "machine/atarigen.h"

class rampart_state : public atarigen_state
{
public:
	rampart_state(const machine_config &mconfig, device_type type, const char *tag)
		: atarigen_state(mconfig, type, tag),
		  m_bitmap(*this, "bitmap") { }

	required_shared_ptr<UINT16> m_bitmap;
	UINT8			m_has_mo;
	DECLARE_WRITE16_MEMBER(latch_w);
	DECLARE_DRIVER_INIT(rampart);
	DECLARE_MACHINE_START(rampart);
	DECLARE_MACHINE_RESET(rampart);
	DECLARE_VIDEO_START(rampart);
};


/*----------- defined in video/rampart.c -----------*/


SCREEN_UPDATE_IND16( rampart );
