/***************************************************************************

    Z80 SIO (Z8440) implementation

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

/***************************************************************************
    CONSTANTS
***************************************************************************/

#define MAX_SIO			2



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef struct _z80sio_interface z80sio_interface;
struct _z80sio_interface
{
	int baseclock;
	void (*irq_cb)(running_machine *machine, int state);
	write8_machine_func dtr_changed_cb;
	write8_machine_func rts_changed_cb;
	write8_machine_func break_changed_cb;
	write8_machine_func transmit_cb;
	int (*receive_poll_cb)(int which);
};



/***************************************************************************
    INITIALIZATION/CONFIGURATION
***************************************************************************/

void z80sio_init(int which, z80sio_interface *intf);
void z80sio_reset(int which);



/***************************************************************************
    CONTROL REGISTER READ/WRITE
***************************************************************************/

void z80sio_c_w(running_machine *machine, int which, int ch, UINT8 data);
UINT8 z80sio_c_r(int which, int ch);



/***************************************************************************
    DATA REGISTER READ/WRITE
***************************************************************************/

void z80sio_d_w(running_machine *machine, int which, int ch, UINT8 data);
UINT8 z80sio_d_r(running_machine *machine, int which, int ch);



/***************************************************************************
    CONTROL LINE READ/WRITE
***************************************************************************/

int z80sio_get_dtr(int which, int ch);
int z80sio_get_rts(int which, int ch);
void z80sio_set_cts(int which, int ch, int state);
void z80sio_set_dcd(int which, int ch, int state);
void z80sio_receive_data(int which, int ch, UINT8 data);



/***************************************************************************
    DAISY CHAIN INTERFACE
***************************************************************************/

int z80sio_irq_state(int which);
int z80sio_irq_ack(int which);
void z80sio_irq_reti(int which);
