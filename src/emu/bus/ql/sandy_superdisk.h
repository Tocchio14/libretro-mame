// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Sandy Super Disk emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __SANDY_SUPER_DISK__
#define __SANDY_SUPER_DISK__

#include "exp.h"
#include "machine/wd_fdc.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sandy_super_disk_device

class sandy_super_disk_t : public device_t,
			   			   public device_ql_expansion_card_interface
{
public:
	// construction/destruction
	sandy_super_disk_t(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_ql_expansion_card_interface overrides

private:
	required_memory_region m_rom;
};


// device type definition
extern const device_type SANDY_SUPER_DISK;


#endif
