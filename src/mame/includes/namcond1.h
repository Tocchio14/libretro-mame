/***************************************************************************

  namcond1.h

  Common functions & declarations for the Namco ND-1 driver

***************************************************************************/

class namcond1_state : public driver_device
{
public:
	namcond1_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	UINT8 h8_irq5_enabled;
	UINT16 *shared_ram;
	int p8;
};


/*----------- defined in machine/namcond1.c -----------*/

READ16_HANDLER( namcond1_shared_ram_r );
READ16_HANDLER( namcond1_cuskey_r );
WRITE16_HANDLER( namcond1_shared_ram_w );
WRITE16_HANDLER( namcond1_cuskey_w );

MACHINE_START( namcond1 );
MACHINE_RESET( namcond1 );

