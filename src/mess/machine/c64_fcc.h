/**********************************************************************

    Tasc Final ChessCard cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#pragma once

#ifndef __FCC__
#define __FCC__

#include "emu.h"
#include "machine/c64exp.h"
#include "cpu/m6502/m65sc02.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_final_chesscard_device

class c64_final_chesscard_device : public device_t,
									public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	c64_final_chesscard_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	DECLARE_INPUT_CHANGED_MEMBER( reset );

protected:
	// device-level overrides
	virtual void device_config_complete() { m_shortname = "c64_fcc"; }
	virtual void device_start();
	virtual void device_reset();

	// device_c64_expansion_card_interface overrides
	virtual UINT8 c64_cd_r(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2);
	virtual void c64_cd_w(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2);

private:
	required_device<m65sc02_device> m_maincpu;

	UINT8 m_bank;
};


// device type definition
extern const device_type C64_FCC;



#endif
