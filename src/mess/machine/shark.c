/**********************************************************************

    Mator Systems SHARK Intelligent Winchester Disc Subsystem emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "shark.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define I8085_TAG       "i8085"
#define RS232_TAG		"rs232"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type SHARK = &device_creator<shark_device>;


//-------------------------------------------------
//  ROM( shark )
//-------------------------------------------------

ROM_START( shark )
	ROM_REGION( 0x5000, I8085_TAG, 0 )
	ROM_LOAD( "pch488 3450 v22.1 #1", 0x0000, 0x1000, CRC(03bff9d7) SHA1(ac506df6509e1b2185a69f9f8f44b8b456aa9834) )
	ROM_LOAD( "pch488 3450 v22.1 #2", 0x1000, 0x1000, CRC(c14fa5fe) SHA1(bcfd1dd65d692c76b90e6134b85f22c39c049430) )
	ROM_LOAD( "pch488 3450 v22.1 #3", 0x2000, 0x1000, CRC(4dfaa482) SHA1(fe2c44bb650572616c8bdad6358032fe64b1e363) )
	ROM_LOAD( "pch488 3450 v22.1 #4", 0x3000, 0x1000, NO_DUMP )
	ROM_LOAD( "pch488 3450 v22.1 #5", 0x4000, 0x1000, CRC(aef665e9) SHA1(80a4c00b717100b4e22fa3704e34060fffce2bc3) )

	ROM_REGION( 0x800, "micro", 0 ) // address decoder
	ROM_LOAD( "micro p3450 v1.3", 0x000, 0x800, CRC(0e69202e) SHA1(3b384951ff54c4b45a3a778a88966d13e2c9d57a) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const rom_entry *shark_device::device_rom_region() const
{
	return ROM_NAME( shark );
}


//-------------------------------------------------
//  ADDRESS_MAP( shark_mem )
//-------------------------------------------------

static ADDRESS_MAP_START( shark_mem, AS_PROGRAM, 8, shark_device )
	AM_RANGE(0x0000, 0x4fff) AM_ROM AM_REGION(I8085_TAG, 0)
ADDRESS_MAP_END


//-------------------------------------------------
//  ADDRESS_MAP( shark_io )
//-------------------------------------------------

static ADDRESS_MAP_START( shark_io, AS_IO, 8, shark_device )
ADDRESS_MAP_END


//-------------------------------------------------
//  I8085_CONFIG( cpu_intf )
//-------------------------------------------------

static I8085_CONFIG( cpu_intf )
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  rs232_port_interface rs232_intf
//-------------------------------------------------

static const rs232_port_interface rs232_intf =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  MACHINE_CONFIG_FRAGMENT( shark )
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( shark )
	// basic machine hardware
	MCFG_CPU_ADD(I8085_TAG, I8085A, 1000000)
	MCFG_CPU_CONFIG(cpu_intf)
	MCFG_CPU_PROGRAM_MAP(shark_mem)
	MCFG_CPU_IO_MAP(shark_io)

	// devices
	MCFG_HARDDISK_ADD("harddisk1")
	MCFG_RS232_PORT_ADD(RS232_TAG, rs232_intf, default_rs232_devices, NULL)
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor shark_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( shark );
}


//-------------------------------------------------
//  INPUT_PORTS( shark )
//-------------------------------------------------

INPUT_PORTS_START( shark )
INPUT_PORTS_END


//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor shark_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( shark );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  shark_device - constructor
//-------------------------------------------------

shark_device::shark_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, SHARK, "Mator SHARK", tag, owner, clock, "shark", __FILE__),
		device_ieee488_interface(mconfig, *this),
		m_maincpu(*this, I8085_TAG)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void shark_device::device_start()
{
}
