/*********************************************************************

    a2eext80col.c

    Apple IIe Extended 80 Column Card (64K of RAM, double-hi-res)

*********************************************************************/

#include "emu.h"
#include "includes/apple2.h"
#include "machine/a2eext80col.h"


/***************************************************************************
    PARAMETERS
***************************************************************************/

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type A2EAUX_EXT80COL = &device_creator<a2eaux_ext80col_device>;

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

a2eaux_ext80col_device::a2eaux_ext80col_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, A2EAUX_EXT80COL, "Apple IIe Extended 80-Column Card", tag, owner, clock),
		device_a2eauxslot_card_interface(mconfig, *this)
{
	m_shortname = "a2eext80";
}

a2eaux_ext80col_device::a2eaux_ext80col_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, type, name, tag, owner, clock),
		device_a2eauxslot_card_interface(mconfig, *this)
{
	m_shortname = "a2eext80";
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void a2eaux_ext80col_device::device_start()
{
	set_a2eauxslot_device();
	save_item(NAME(m_ram));
}

void a2eaux_ext80col_device::device_reset()
{
}

UINT8 a2eaux_ext80col_device::read_auxram(UINT16 offset)
{
	return m_ram[offset];
}

void a2eaux_ext80col_device::write_auxram(UINT16 offset, UINT8 data)
{
	m_ram[offset] = data;
}

UINT8 *a2eaux_ext80col_device::get_vram_ptr()
{
	return &m_ram[0];
}

