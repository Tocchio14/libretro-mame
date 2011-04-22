/**********************************************************************

    Ricoh RP5C15 Real Time Clock emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************
                            _____   _____
                   _CS   1 |*    \_/     | 18  Vcc
                    CS   2 |             | 17  OSCOUT
                CLKOUT   3 |             | 16  OSCIN
                    A0   4 |   RP5C15    | 15  _ALARM
                    A1   5 |   RF5C15    | 14  D3
                    A2   6 |   RJ5C15    | 13  D2
                    A3   7 |             | 12  D1
                   _RD   8 |             | 11  D0
                   GND   9 |_____________| 10  _WR

**********************************************************************/

#pragma once

#ifndef __RP5C15__
#define __RP5C15__

#include "emu.h"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_RP5C15_ADD(_tag, _clock, _config) \
	MCFG_DEVICE_ADD((_tag), RP5C15, _clock)	\
	MCFG_DEVICE_CONFIG(_config)


#define RP5C15_INTERFACE(name) \
	const rp5c15_interface (name) =



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> rp5c15_interface

struct rp5c15_interface
{
	devcb_write_line		m_out_alarm_func;
	devcb_write_line		m_out_clkout_func;
};



// ======================> rp5c15_device_config

class rp5c15_device_config :   public device_config,
                               public rp5c15_interface
{
    friend class rp5c15_device;

    // construction/destruction
    rp5c15_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock);

public:
    // allocators
    static device_config *static_alloc_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock);
    virtual device_t *alloc_device(running_machine &machine) const;

protected:
	// device_config overrides
	virtual void device_config_complete();
};



// ======================> rp5c15_device

class rp5c15_device :	public device_t
{
    friend class rp5c15_device_config;

    // construction/destruction
    rp5c15_device(running_machine &_machine, const rp5c15_device_config &_config);

public:
	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );
	DECLARE_WRITE_LINE_MEMBER( adj_w );

protected:
    // device-level overrides
    virtual void device_start();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:
	inline void set_alarm_line();
	inline int read_counter(int counter);
	inline void write_counter(int counter, int value);
	inline void advance_seconds();
	inline void advance_minutes();
	inline void adjust_seconds();
	inline void check_alarm();

	static const device_timer_id TIMER_CLOCK = 0;
	static const device_timer_id TIMER_16HZ = 1;
	static const device_timer_id TIMER_CLKOUT = 2;

	devcb_resolved_write_line	m_out_alarm_func;
	devcb_resolved_write_line	m_out_clkout_func;

	UINT8 m_reg[2][13];			// clock registers
	UINT8 m_ram[13];			// RAM

	UINT8 m_mode;				// mode register
	UINT8 m_reset;				// reset register
	int m_alarm;				// alarm output
	int m_alarm_on;				// alarm condition
	int m_1hz;					// 1 Hz condition
	int m_16hz;					// 16 Hz condition
	int m_clkout;				// clock output

	// timers
	emu_timer *m_clock_timer;
	emu_timer *m_16hz_timer;
	emu_timer *m_clkout_timer;

	const rp5c15_device_config &m_config;
};


// device type definition
extern const device_type RP5C15;



#endif
