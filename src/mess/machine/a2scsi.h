/*********************************************************************

    a2scsi.h

    Implementation of the Apple II SCSI Card

*********************************************************************/

#ifndef __A2BUS_SCSI__
#define __A2BUS_SCSI__

#include "emu.h"
#include "machine/a2bus.h"
#include "machine/ncr5380.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_scsi_device:
    public device_t,
    public device_a2bus_card_interface
{
public:
    // construction/destruction
    a2bus_scsi_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);
    a2bus_scsi_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

    // optional information overrides
    virtual machine_config_constructor device_mconfig_additions() const;
    virtual const rom_entry *device_rom_region() const;

protected:
    virtual void device_start();
    virtual void device_reset();

    // overrides of standard a2bus slot functions
    virtual UINT8 read_c0nx(address_space &space, UINT8 offset);
    virtual void write_c0nx(address_space &space, UINT8 offset, UINT8 data);
    virtual UINT8 read_cnxx(address_space &space, UINT8 offset);
    virtual void write_cnxx(address_space &space, UINT8 offset, UINT8 data);
    virtual UINT8 read_c800(address_space &space, UINT16 offset);
    virtual void write_c800(address_space &space, UINT16 offset, UINT8 data);

    required_device<ncr5380_device> m_ncr5380;

private:
    UINT8 *m_rom;
    UINT8 m_ram[8192];  // 8 banks of 1024 bytes
    int m_rambank, m_rombank;
};

// device type definition
extern const device_type A2BUS_SCSI;

#endif /* __A2BUS_SCSI__ */

