/***************************************************************************

Mitsubishi M50458 OSD chip

***************************************************************************/

#pragma once

#ifndef __M50458DEV_H__
#define __M50458DEV_H__



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_M50458_ADD(_tag,_freq) \
	MCFG_DEVICE_ADD(_tag, M50458,_freq) \


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

typedef enum
{
	OSD_SET_ADDRESS = 0,
	OSD_SET_DATA
} m50458_state_t;

// ======================> m50458_device

class m50458_device :	public device_t,
						public device_memory_interface
{
public:
	// construction/destruction
	m50458_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// I/O operations
	WRITE_LINE_MEMBER( write_bit );
	WRITE_LINE_MEMBER( set_cs_line );
	WRITE_LINE_MEMBER( set_clock_line );
	DECLARE_WRITE16_MEMBER(vreg_120_w);
	DECLARE_WRITE16_MEMBER(vreg_126_w);
	DECLARE_WRITE16_MEMBER(vreg_127_w);

	UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	virtual const rom_entry *device_rom_region() const;

protected:
	// device-level overrides
	virtual void device_validity_check(validity_checker &valid) const;
	virtual void device_start();
	virtual void device_reset();
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const;

	int m_latch;
	int	m_reset_line;
	int	m_clock_line;
	UINT16 m_current_cmd;
	int m_cmd_stream_pos;
	UINT16 m_osd_addr;
	UINT8 m_bg_pen;
	UINT8 m_phase;

	m50458_state_t m_osd_state;

private:
	inline UINT16 read_word(offs_t address);
	inline void write_word(offs_t address, UINT16 data);

	const address_space_config		m_space_config;
};


// device type definition
extern const device_type M50458;



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************



#endif
