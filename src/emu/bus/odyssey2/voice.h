#ifndef __O2_VOICE_H
#define __O2_VOICE_H

#include "slot.h"
#include "rom.h"
#include "sound/sp0256.h"


// ======================> o2_voice_device

class o2_voice_device : public o2_rom_device
{
public:
	// construction/destruction
	o2_voice_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_reset() {}

	virtual machine_config_constructor device_mconfig_additions() const;
	virtual const rom_entry *device_rom_region() const;

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_rom04) { if (m_subslot->exists()) return m_subslot->read_rom04(space, offset); else return 0xff; }
	virtual DECLARE_READ8_MEMBER(read_rom0c) { if (m_subslot->exists()) return m_subslot->read_rom0c(space, offset); else return 0xff; }

	virtual void write_bank(int bank) 	{ if (m_subslot->exists()) m_subslot->write_bank(bank); }

	DECLARE_WRITE_LINE_MEMBER(lrq_callback);
	DECLARE_WRITE8_MEMBER(io_write);
	DECLARE_READ8_MEMBER(t0_read) { return m_speech->lrq_r() ? 0 : 1; }

private:
	required_device<sp0256_device> m_speech;
	required_device<o2_cart_slot_device> m_subslot;

	int m_lrq_state;
};




// device type definition
extern const device_type O2_ROM_VOICE;


#endif
