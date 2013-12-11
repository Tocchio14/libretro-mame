// license:MAME|LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    SNUG BwG Disk Controller
    Based on WD1770
    Double Density, Double-sided

    Michael Zapf, September 2010
    February 2012: Rewritten as class

*****************************************************************************/

#ifndef __BWG__
#define __BWG__

#include "ti99defs.h"
#include "imagedev/flopdrv.h"
#include "machine/mm58274c.h"
#include "machine/wd17xx.h"

extern const device_type TI99_BWG;

class snug_bwg_device : public ti_expansion_card_device
{
public:
	snug_bwg_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	DECLARE_READ8Z_MEMBER(readz);
	DECLARE_WRITE8_MEMBER(write);
	DECLARE_SETADDRESS_DBIN_MEMBER(setaddress_dbin);

	DECLARE_WRITE_LINE_MEMBER( intrq_w );
	DECLARE_WRITE_LINE_MEMBER( drq_w );

	DECLARE_READ8Z_MEMBER(crureadz);
	DECLARE_WRITE8_MEMBER(cruwrite);

protected:
	virtual void device_start(void);
	virtual void device_reset(void);
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

private:
	void set_ready_line();
	void set_all_geometries(floppy_type_t type);
	void set_geometry(device_t *drive, floppy_type_t type);
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

	// Holds the status of the DRQ and IRQ lines.
	line_state      m_DRQ, m_IRQ;

	// DIP switch state
	int             m_dip1, m_dip2, m_dip34;

	// Page selection for ROM and RAM
	int             m_ram_page;  // 0-1
	int             m_rom_page;  // 0-3

	// When true, the READY line will be cleared (create wait states) when
	// waiting for data from the controller.
	bool            m_WAITena;

	// Address in card area
	bool            m_inDsrArea;

	// WD selected
	bool            m_WDsel, m_WDsel0;

	// RTC selected
	bool            m_RTCsel;

	// last 1K area selected
	bool            m_lastK;

	// Data register +1 selected
	bool            m_dataregLB;

	/* Indicates whether the clock is mapped into the address space. */
	bool            m_rtc_enabled;

	/* When TRUE, keeps DVENA high. */
	bool            m_strobe_motor;

	// Signal DVENA. When TRUE, makes some drive turning.
	line_state      m_DVENA;

	// Recent address
	int             m_address;

	/* Indicates which drive has been selected. Values are 0, 1, 2, and 4. */
	// 000 = no drive
	// 001 = drive 1
	// 010 = drive 2
	// 100 = drive 3
	int             m_DSEL;

	/* Signal SIDSEL. 0 or 1, indicates the selected head. */
	int             m_SIDE;

	// Link to the WD1773 controller on the board.
	required_device<wd1773_device>   m_wd1773;

	// Link to the real-time clock on the board.
	required_device<mm58274c_device> m_clock;

	// count 4.23s from rising edge of motor_on
	emu_timer*      m_motor_on_timer;

	// DSR ROM
	UINT8*          m_dsrrom;

	// Buffer RAM
	UINT8*          m_buffer_ram;
};

#endif
