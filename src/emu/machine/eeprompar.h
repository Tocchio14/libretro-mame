/***************************************************************************

    eeprompar.h

    Parallel EEPROM devices.

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#pragma once

#ifndef __EEPROMPAR_H__
#define __EEPROMPAR_H__

#include "eeprom.h"


//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

// standard 28XX class of 8-bit parallel EEPROMs
#define MCFG_EEPROM_2804_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, EEPROM_PARALLEL_2804, 0)
#define MCFG_EEPROM_2816_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, EEPROM_PARALLEL_2816, 0)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> eeprom_parallel_base_device

class eeprom_parallel_base_device : public eeprom_base_device
{
protected:
	// construction/destruction
	eeprom_parallel_base_device(const machine_config &mconfig, device_type devtype, const char *name, const char *tag, device_t *owner, const char *shortname, const char *file);

public:

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
};



// ======================> eeprom_parallel_28xx_device

class eeprom_parallel_28xx_device : public eeprom_parallel_base_device
{
protected:
	// construction/destruction
	eeprom_parallel_28xx_device(const machine_config &mconfig, device_type devtype, const char *name, const char *tag, device_t *owner, const char *shortname, const char *file);

public:
	// read/write data lines - for now we cheat and ignore the control lines, assuming
	// they are handled reasonably
	DECLARE_WRITE8_MEMBER(write);
	DECLARE_READ8_MEMBER(read);
};



//**************************************************************************
//  DERIVED TYPES
//**************************************************************************

// macro for declaring a new device class
#define DECLARE_PARALLEL_EEPROM_DEVICE(_baseclass, _lowercase, _uppercase) \
class eeprom_parallel_##_lowercase##_device : public eeprom_parallel_##_baseclass##_device \
{ \
public: \
	eeprom_parallel_##_lowercase##_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock); \
}; \
extern const device_type EEPROM_PARALLEL_##_uppercase; \

// standard 28XX class of 8-bit EEPROMs
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 2804, 2804)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 2816, 2816)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 2864, 2864)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 28256, 28256)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 28512, 28512)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 28010, 28010)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 28020, 28020)
DECLARE_PARALLEL_EEPROM_DEVICE(28xx, 28040, 28040)

#endif
