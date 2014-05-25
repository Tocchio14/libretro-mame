// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Sandy SuperQBoard (with HD upgrade) emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "sandy_superqboard.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define WD1772_TAG		"ic3"
#define TTL74273_TAG	"ic10"
#define CENTRONICS_TAG	"j2"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type SANDY_SUPERQBOARD = &device_creator<sandy_superqboard_t>;


//-------------------------------------------------
//  ROM( sandy_superqboard )
//-------------------------------------------------

ROM_START( sandy_superqboard )
	ROM_REGION( 0x8000, "rom", 0 )
	ROM_LOAD( "sandy_disk_controller_v1.18y_1984.ic2", 0x0000, 0x8000, CRC(d02425be) SHA1(e730576e3e0c6a1acad042c09e15fc62a32d8fbd) )

	ROM_REGION( 0x100, "plds", 0 )
	ROM_LOAD( "gal16v8.ic5", 0x000, 0x000, NO_DUMP )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const rom_entry *sandy_superqboard_t::device_rom_region() const
{
	return ROM_NAME( sandy_superqboard );
}


//-------------------------------------------------
//  SLOT_INTERFACE( sandy_superqboard_floppies )
//-------------------------------------------------

static SLOT_INTERFACE_START( sandy_superqboard_floppies )
	SLOT_INTERFACE( "35dd", FLOPPY_35_DD )
	SLOT_INTERFACE( "35hd", FLOPPY_35_HD )
SLOT_INTERFACE_END


//-------------------------------------------------
//  centronics
//-------------------------------------------------

WRITE_LINE_MEMBER( sandy_superqboard_t::busy_w )
{
	m_busy = state;
}


//-------------------------------------------------
//  MACHINE_CONFIG_FRAGMENT( sandy_superqboard )
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( sandy_superqboard )
	MCFG_DEVICE_ADD(WD1772_TAG, WD1772x, XTAL_16MHz/2)
	MCFG_FLOPPY_DRIVE_ADD(WD1772_TAG":0", sandy_superqboard_floppies, "35dd", floppy_image_device::default_floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD(WD1772_TAG":1", sandy_superqboard_floppies, NULL, floppy_image_device::default_floppy_formats)

	MCFG_CENTRONICS_ADD(CENTRONICS_TAG, centronics_printers, "printer")
	MCFG_CENTRONICS_BUSY_HANDLER(WRITELINE(sandy_superqboard_t, busy_w))
	MCFG_CENTRONICS_OUTPUT_LATCH_ADD(TTL74273_TAG, CENTRONICS_TAG)
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor sandy_superqboard_t::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( sandy_superqboard );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  sandy_superqboard_t - constructor
//-------------------------------------------------

sandy_superqboard_t::sandy_superqboard_t(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, SANDY_SUPERQBOARD, "SANDY_SUPERQBOARD", tag, owner, clock, "sandy_superqboard", __FILE__),
	device_ql_expansion_card_interface(mconfig, *this),
	m_fdc(*this, WD1772_TAG),
	m_floppy0(*this, WD1772_TAG":0"),
	m_floppy1(*this, WD1772_TAG":1"),
	m_centronics(*this, CENTRONICS_TAG),
	m_latch(*this, TTL74273_TAG),
	m_rom(*this, "rom"),
	m_ram(*this, "ram"),
	m_busy(1),
	m_int2(0),
	m_int3(0)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void sandy_superqboard_t::device_start()
{
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void sandy_superqboard_t::device_reset()
{
	m_fdc->reset();
	m_latch->write(0);
	
	m_int2 = 0;
	m_int3 = 0;
}


//-------------------------------------------------
//  read -
//-------------------------------------------------

UINT8 sandy_superqboard_t::read(address_space &space, offs_t offset, UINT8 data)
{
	switch ((offset >> 2) & 0x03)
	{
	case 0:
		data = m_fdc->read(space, offset & 0x03);
		break;
	
	case 3:
		/*

			bit		description

			0 		BUSY
			1 		mouse pin 8
			2 		mouse pin 1
			3 		mouse pin 2
			4 		mouse pin 4 flip-flop Q
			5 		mouse pin 3 flip-flop Q
			6 		INT3
			7 		INT2

		*/

		data = m_busy;
		data |= m_int3 << 6;
		data |= m_int2 << 7;
		break;
	}

	return data;
}


//-------------------------------------------------
//  write -
//-------------------------------------------------

void sandy_superqboard_t::write(address_space &space, offs_t offset, UINT8 data)
{
	switch ((offset >> 2) & 0x03)
	{
	case 0:
		m_fdc->write(space, offset & 0x03, data);
		break;

	case 1:
		{
		/*

			bit		description

			0 		SIDE ONE
			1 		DSEL0
			2 		DSEL1
			3 		M ON0
			4 		/DDEN
			5 		STROBE inverted
			6 		GAL pin 11
			7 		GAL pin 9

		*/

		floppy_image_device *floppy = NULL;

		if (BIT(data, 1)) 
		{
			floppy = m_floppy0->get_device();
		}
		else if (BIT(data, 2))
		{
			floppy = m_floppy1->get_device();
		}

		m_fdc->set_floppy(floppy);

		if (floppy)
		{
			floppy->ss_w(BIT(data, 0));
			floppy->mon_w(BIT(data, 3));
		}

		m_fdc->dden_w(BIT(data, 4));

		m_centronics->write_strobe(!BIT(data, 5));
		}
		break;

	case 2:
		m_latch->write(data);
		break;

	case 4:
		m_int2 = 0;
		m_int3 = 0;
		break;

	case 5:
		m_fdc->set_unscaled_clock(XTAL_16MHz >> !BIT(data, 0));
		break;
	}
}
