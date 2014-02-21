// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Namesoft MIDI Interface cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "midi_namesoft.h"
#include "machine/clock.h"
#include "bus/midi/midi.h"
#include "bus/midi/midiinport.h"
#include "bus/midi/midioutport.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define MC6850_TAG      "mc6850"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type C64_MIDI_NAMESOFT = &device_creator<c64_namesoft_midi_cartridge_device>;


WRITE_LINE_MEMBER( c64_namesoft_midi_cartridge_device::acia_irq_w )
{
	m_slot->nmi_w(state);
}

//-------------------------------------------------
//  SLOT_INTERFACE( midiin_slot )
//-------------------------------------------------

static SLOT_INTERFACE_START( midiin_slot )
	SLOT_INTERFACE("midiin", MIDIIN_PORT)
SLOT_INTERFACE_END

WRITE_LINE_MEMBER( c64_namesoft_midi_cartridge_device::write_acia_clock )
{
	m_acia->write_txc(state);
	m_acia->write_rxc(state);
}


//-------------------------------------------------
//  SLOT_INTERFACE( midiout_slot )
//-------------------------------------------------

static SLOT_INTERFACE_START( midiout_slot )
	SLOT_INTERFACE("midiout", MIDIOUT_PORT)
SLOT_INTERFACE_END


//-------------------------------------------------
//  MACHINE_CONFIG_FRAGMENT( c64_passport_midi )
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( c64_passport_midi )
	MCFG_DEVICE_ADD(MC6850_TAG, ACIA6850, 0)
	MCFG_ACIA6850_TXD_HANDLER(DEVWRITELINE("mdout", midi_port_device, write_txd))
	MCFG_ACIA6850_IRQ_HANDLER(WRITELINE(c64_namesoft_midi_cartridge_device, acia_irq_w))

	MCFG_MIDI_PORT_ADD("mdin", midiin_slot, "midiin")
	MCFG_MIDI_RX_HANDLER(DEVWRITELINE(MC6850_TAG, acia6850_device, write_rxd))

	MCFG_MIDI_PORT_ADD("mdout", midiout_slot, "midiout")

	MCFG_DEVICE_ADD("acia_clock", CLOCK, 31250*16)
	MCFG_CLOCK_SIGNAL_HANDLER(WRITELINE(c64_namesoft_midi_cartridge_device, write_acia_clock))
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor c64_namesoft_midi_cartridge_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( c64_passport_midi );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c64_namesoft_midi_cartridge_device - constructor
//-------------------------------------------------

c64_namesoft_midi_cartridge_device::c64_namesoft_midi_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, C64_MIDI_NAMESOFT, "C64 Namesoft MIDI", tag, owner, clock, "c64_midins", __FILE__),
	device_c64_expansion_card_interface(mconfig, *this),
	m_acia(*this, MC6850_TAG)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c64_namesoft_midi_cartridge_device::device_start()
{
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void c64_namesoft_midi_cartridge_device::device_reset()
{
	m_acia->reset();
}


//-------------------------------------------------
//  c64_cd_r - cartridge data read
//-------------------------------------------------

UINT8 c64_namesoft_midi_cartridge_device::c64_cd_r(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!io1)
	{
		switch (offset & 0xff)
		{
		case 2:
			data = m_acia->status_r(space, 0);
			break;

		case 3:
			data = m_acia->data_r(space, 0);
			break;
		}
	}

	return data;
}


//-------------------------------------------------
//  c64_cd_w - cartridge data write
//-------------------------------------------------

void c64_namesoft_midi_cartridge_device::c64_cd_w(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!io1)
	{
		switch (offset & 0xff)
		{
		case 0:
			m_acia->control_w(space, 0, data);
			break;

		case 1:
			m_acia->data_w(space, 0, data);
			break;
		}
	}
}
