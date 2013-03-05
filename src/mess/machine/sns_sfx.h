#ifndef __SNS_SFX_H
#define __SNS_SFX_H

#include "machine/sns_slot.h"
#include "machine/sns_rom.h"
#include "cpu/superfx/superfx.h"


// ======================> sns_rom_superfx_device

class sns_rom_superfx_device : public sns_rom_device
{
public:
	// construction/destruction
	sns_rom_superfx_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete() { m_shortname = "sns_rom_superfx"; }
	virtual machine_config_constructor device_mconfig_additions() const;

	required_device<device_t> m_superfx;

	// additional reading and writing
	virtual DECLARE_READ8_MEMBER(read_l);
	virtual DECLARE_WRITE8_MEMBER(write_l);
	virtual DECLARE_READ8_MEMBER(read_h);
	virtual DECLARE_WRITE8_MEMBER(write_h);
	virtual DECLARE_READ8_MEMBER(chip_read);
	virtual DECLARE_WRITE8_MEMBER(chip_write);

	virtual DECLARE_READ8_MEMBER(superfx_r_bank1);
	virtual DECLARE_READ8_MEMBER(superfx_r_bank2);
	virtual DECLARE_READ8_MEMBER(superfx_r_bank3);
	virtual DECLARE_WRITE8_MEMBER(superfx_w_bank1);
	virtual DECLARE_WRITE8_MEMBER(superfx_w_bank2);
	virtual DECLARE_WRITE8_MEMBER(superfx_w_bank3);
	virtual DECLARE_WRITE_LINE_MEMBER(snes_extern_irq_w);


	UINT8 sfx_ram[0x200000];
};


// device type definition
extern const device_type SNS_LOROM_SUPERFX;

#endif
