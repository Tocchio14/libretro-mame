/**********************************************************************

    Sega Master System "Gender Adapter" emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SMS_GENDER_ADAPTER__
#define __SMS_GENDER_ADAPTER__


#include "emu.h"
#include "machine/smsexp.h"
#include "machine/sega8_slot.h"
#include "machine/sega8_rom.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_gender_adapter_device

class sms_gender_adapter_device : public device_t,
							public device_sms_expansion_slot_interface
{
public:
	// construction/destruction
	sms_gender_adapter_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device_sms_expansion_slot_interface overrides
	virtual DECLARE_READ8_MEMBER(read);
	virtual DECLARE_WRITE8_MEMBER(write);
	virtual DECLARE_WRITE8_MEMBER(write_mapper);
	virtual DECLARE_READ8_MEMBER(read_ram);
	virtual DECLARE_WRITE8_MEMBER(write_ram);

protected:
	// device-level overrides
	virtual void device_start();
	virtual machine_config_constructor device_mconfig_additions() const;

private:
	required_device<sega8_cart_slot_device> m_subslot;
};


// device type definition
extern const device_type SMS_GENDER_ADAPTER;


#endif
