/*************************************************************************

    Atari Arcade Classics hardware (prototypes)

*************************************************************************/


#include "machine/atarigen.h"

class arcadecl_state : public atarigen_state
{
public:
	arcadecl_state(const machine_config &mconfig, device_type type, const char *tag)
		: atarigen_state(mconfig, type, tag),
		  m_bitmap(*this, "bitmap") { }

	required_shared_ptr<UINT16> m_bitmap;
	UINT8			m_has_mo;
	DECLARE_WRITE16_MEMBER(latch_w);
	DECLARE_DRIVER_INIT(sparkz);
	DECLARE_MACHINE_START(arcadecl);
	DECLARE_MACHINE_RESET(arcadecl);
	DECLARE_VIDEO_START(arcadecl);
};

/*----------- defined in video/arcadecl.c -----------*/


SCREEN_UPDATE_IND16( arcadecl );
