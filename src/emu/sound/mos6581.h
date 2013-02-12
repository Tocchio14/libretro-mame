/**********************************************************************

    MOS 6581/8580 Sound Interface Device emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************
                            _____   _____
                 CAP1A   1 |*    \_/     | 28  Vdd
                 CAP1B   2 |             | 27  AUDIO OUT
                 CAP2A   3 |             | 26  EXT IN
                 CAP2B   4 |             | 25  Vcc
                  _RES   5 |             | 24  POTX
                  phi2   6 |             | 23  POTY
                  R/_W   7 |   MOS6581   | 22  D7
                   _CS   8 |   MOS8580   | 21  D6
                    A0   9 |             | 20  D5
                    A1  10 |             | 19  D4
                    A2  11 |             | 18  D3
                    A3  12 |             | 17  D2
                    A4  13 |             | 16  D1
                   GND  14 |_____________| 15  D0

**********************************************************************/

#pragma once

#ifndef __MOS6581__
#define __MOS6581__

#include "emu.h"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_MOS6581_POTXY_CALLBACKS(_potx, _poty) \
	downcast<mos6581_device *>(device)->set_callbacks(DEVCB2_##_potx, DEVCB2_##_poty);



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> mos6581_device

struct SID6581_t;

class mos6581_device : public device_t,
						public device_sound_interface
{
public:
	mos6581_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, UINT32 variant);
	mos6581_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	template<class _potx, class _poty> void set_callbacks(_potx potx, _poty poty) {
		m_read_potx.set_callback(potx);
		m_read_poty.set_callback(poty);
	}

	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );

	enum
	{
		TYPE_6581,
		TYPE_8580
	};

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_sound_interface overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);

private:
	devcb2_read8  m_read_potx;
	devcb2_read8  m_read_poty;

	sound_stream *m_stream;

	int m_variant;

	SID6581_t *m_token;
};


// ======================> mos8580_device

class mos8580_device : public mos6581_device
{
public:
	mos8580_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
};


// device type definition
extern const device_type MOS6581;
extern const device_type MOS8580;


#endif
