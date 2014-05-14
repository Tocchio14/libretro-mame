/***************************************************************************

    !!! DEPRECATED DO NOT USE !!!

    WILL BE DELETED WHEN src/mame/drivers/dlair.c USES z80dart.h
    
    Z80 SIO (Z8440) implementation

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#ifndef __Z80SIO_H__
#define __Z80SIO_H__

#include "cpu/z80/z80daisy.h"


//**************************************************************************
//  DEVICE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_Z80SIO_INT_CALLBACK(_write) \
	devcb = &z80sio_device::set_int_callback(*device, DEVCB_##_write);

#define MCFG_Z80SIO_TRANSMIT_CALLBACK(_write) \
	devcb = &z80sio_device::set_transmit_callback(*device, DEVCB_##_write);

#define MCFG_Z80SIO_RECEIVE_CALLBACK(_read) \
	devcb = &z80sio_device::set_receive_callback(*device, DEVCB_##_read);



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> z80sio_device

class z80sio_device :   public device_t,
						public device_z80daisy_interface
{
public:
	// construction/destruction
	z80sio_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	template<class _Object> static devcb_base &set_int_callback(device_t &device, _Object object) { return downcast<z80sio_device &>(device).m_irq.set_callback(object); }
	template<class _Object> static devcb_base &set_transmit_callback(device_t &device, _Object object) { return downcast<z80sio_device &>(device).m_transmit.set_callback(object); }
	template<class _Object> static devcb_base &set_receive_callback(device_t &device, _Object object) { return downcast<z80sio_device &>(device).m_received_poll.set_callback(object); }

	// control register I/O
	UINT8 control_read(int ch) { return m_channel[ch].control_read(); }
	void control_write(int ch, UINT8 data) { m_channel[ch].control_write(data); }

	// data register I/O
	UINT8 data_read(int ch) { return m_channel[ch].data_read(); }
	void data_write(int ch, UINT8 data) { m_channel[ch].data_write(data); }

	// communication line I/O
	int dtr(int ch) { return m_channel[ch].dtr(); }
	int rts(int ch) { return m_channel[ch].rts(); }
	void set_cts(int ch, int state) { m_channel[ch].set_cts(state); }
	void set_dcd(int ch, int state) { m_channel[ch].set_dcd(state); }
	void receive_data(int ch, int data) { m_channel[ch].receive_data(data); }

	// standard read/write, with C/D in bit 1, B/A in bit 0
	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );

	// alternate read/write, with C/D in bit 0, B/A in bit 1
	DECLARE_READ8_MEMBER( read_alt );
	DECLARE_WRITE8_MEMBER( write_alt );

private:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_z80daisy_interface overrides
	virtual int z80daisy_irq_state();
	virtual int z80daisy_irq_ack();
	virtual void z80daisy_irq_reti();

	// internal helpers
	void update_interrupt_state();

	// a single SIO channel
	class sio_channel
	{
	public:
		sio_channel();

		void start(z80sio_device *device, int index);
		void reset();

		UINT8 control_read();
		UINT8 data_read();
		void control_write(UINT8 data);
		void data_write(UINT8 data);

		int dtr();
		int rts();
		void set_cts(int state);
		void set_dcd(int state);
		void receive_data(int data);

	private:
		void set_interrupt(int type);
		void clear_interrupt(int type);
		attotime compute_time_per_character();

		static TIMER_CALLBACK( static_change_input_line ) { reinterpret_cast<sio_channel *>(ptr)->change_input_line(param >> 1, param & 1); }
		void change_input_line(int line, int state);

		static TIMER_CALLBACK( static_serial_callback ) { reinterpret_cast<sio_channel *>(ptr)->serial_callback(); }
		void serial_callback();

	public:
		UINT8       m_regs[8];              // 8 writeable registers

	private:
		z80sio_device *m_device;            // pointer back to our device
		int         m_index;                // our channel index
		UINT8       m_status[4];            // 3 readable registers
		int         m_inbuf;                // input buffer
		int         m_outbuf;               // output buffer
		bool        m_int_on_next_rx;       // interrupt on next rx?
		emu_timer * m_receive_timer;        // timer to clock data in
		UINT8       m_receive_buffer[16];   // buffer for incoming data
		UINT8       m_receive_inptr;        // index of data coming in
		UINT8       m_receive_outptr;       // index of data going out
	};

	// internal state
	sio_channel                 m_channel[2];           // 2 channels
	UINT8                       m_int_state[8];         // interrupt states

	// callbacks
	devcb_write_line m_irq;
	devcb_write8 m_dtr_changed;
	devcb_write8 m_rts_changed;
	devcb_write8 m_break_changed;
	devcb_write16 m_transmit;
	devcb_read16 m_received_poll;

	static const UINT8 k_int_priority[];
};


// device type definition
extern const ATTR_DEPRECATED device_type Z80SIO;


#endif
