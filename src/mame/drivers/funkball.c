// license:?
// copyright-holders:Angelo Salese, Phil Bennett
// dgPIX 'VRender 2 Beta Rev4' hardware
// MEDIAGX CPU + 3dFX VooDoo chipset

/***************************************************************************

driver by Angelo Salese & Phil Bennett

Notes:
-

Funky Ball
dgPIX, 1998

PCB Layout
----------

VRender 2Beta Rev4
  |--------------------------------------------------------------------|
  |TDA2005  14.31818MHz       |---------|   KM416C254 KM416C254        |
  |VOL KDA0340D    |-------|  |3DFX     |                              |
  |4558            |GENDAC |  |500-0004-02                             |
  |                |ICS5342|  |F004221.1|                              |
  |                |       |  |TMU      |                    KM416S1020|
  |                |-------|  |---------|   KM416C254 KM416C254        |
  |SERVICE_SW |---SUB---|                                              |
  |           |         |                                              |
  |           |FLASH.U3 |     |-----------|                            |
|-|           |         |     |3DFX       |                  KM416S1020|
|             |         |     |500-0003-03| KM416C254 KM416C254        |
|             |         |     |F006531.1  |                            |
|             |         |     |FBI        |                            |
|J            |         |     |           | KM416C254 KM416C254        |
|A            |         |     |-----------|                            |
|M            |         |RESET_SW                                      |
|M            |         |                                              |
|A            |---------|                     |-------------|          |
|      512K-EPR.U62       14.31818MHz         |Cyrix        |KM416S1020|
|                            |---------|      |GX MEDIA     |          |
|    |-------|  |------|     |LSI      |      |GXm-233GP    |          |
|    |XILINX |  |KS0164|     |L2A0788  |      |             |          |
|-|  |XCS05  |  |      |     |Cyrix    |      |             |          |
  |  |       |  |------|     |CX5520   |      |             |          |
  |  |-------| 16.9344MHz    |---------|      |-------------|KM416S1020|
  | LED               DIP20                                            |
  | |--------------FLASH-DAUGHTERBOARD----------------|                |
  | |                                                 |                |
  | |           FLASH.U30 FLASH.U29              DIP20|                |
  | |-------------------------------------------------|                |
  |--------------------------------------------------------------------|
Notes:
      Cyrix GXm233 - Main CPU; Cyrix GX Media running at 233MHz. Clock is generated by the Cyrix CX5520
                     and a 14.31818MHz xtal. That gives a 66.6MHz bus clock with a 3.5X multiplier for 233MHz
      Cyrix CX5520 - CPU-support chipset (BGA IC)
      FLASH.U29/30 - Intel Strata-Flash DA28F320J5 SSOP56 contained on a plug-in daughterboard; graphics ROMs
      FLASH.U3     - Intel Strata-Flash DA28F320J5 SSOP56 contained on a plug-in daughterboard; main program
      KS0164       - Samsung Electronics KS0164 General Midi compliant 32-voice Wavetable Synthesizer chip
                     with built-in 16bit CPU and MPU-401 compatibility (QFP100)
      512K-EPR     - 512k EPROM, boot-loader program. EPROM is tied to the KS0164 and the XCS05
      DIP20        - not-populated sockets
      KDA0340D     - Samsung KDA0340D CMOS low-power two-channel digital-to-analog converter (SOP28)
      KM416S1020   - Samsung 1M x16 SDRAM (x4, TSSOP50)
      KM416C254    - Samsung 256k x16 DRAM (x8, SOJ40)
      ICS5342      - combination dual programmable clock generator, 256bytes x18-bit RAM and a triple 8-bit video DAC (PLCC68)
      XCS05        - Xilinx Spartan XCS05 FPGA (PLCC84)

***************************************************************************/



#include "emu.h"
#include "cpu/i386/i386.h"
#include "machine/pci.h"
#include "machine/pckeybrd.h"
#include "machine/idectrl.h"
#include "video/voodoo.h"
#include "machine/pcshare.h"


class funkball_state : public pcat_base_state
{
public:
	funkball_state(const machine_config &mconfig, device_type type, const char *tag)
		: pcat_base_state(mconfig, type, tag),
			m_voodoo(*this, "voodoo_0"),
			m_unk_ram(*this, "unk_ram"){ }

	UINT8 m_funkball_config_reg_sel;
	UINT8 m_funkball_config_regs[256];
	UINT32 m_cx5510_regs[256/4];
	UINT16 m_flash_addr;
	UINT8 *m_bios_ram;
	UINT8 m_flash_cmd;
	UINT8 m_flash_data_cmd;

	UINT32 m_biu_ctrl_reg[256/4];

	// devices
	required_device<voodoo_1_device> m_voodoo;

	required_shared_ptr<UINT32> m_unk_ram;

	DECLARE_READ8_MEMBER( get_slave_ack );
	DECLARE_WRITE8_MEMBER( flash_w );
	DECLARE_READ8_MEMBER( flash_data_r );
	DECLARE_WRITE8_MEMBER( flash_data_w );
//  DECLARE_WRITE8_MEMBER( bios_ram_w );
	DECLARE_READ8_MEMBER( test_r );
	DECLARE_READ8_MEMBER( serial_r );
	DECLARE_WRITE8_MEMBER( serial_w );

	UINT8 funkball_config_reg_r();
	void funkball_config_reg_w(UINT8 data);

	virtual void video_start();
	UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	struct
	{
		/* PCI */
		UINT32 command;
		UINT32 base_addr;

		UINT32 init_enable;
	} m_voodoo_pci_regs;
	DECLARE_READ32_MEMBER(biu_ctrl_r);
	DECLARE_WRITE32_MEMBER(biu_ctrl_w);
	DECLARE_WRITE8_MEMBER(bios_ram_w);
	DECLARE_READ32_MEMBER(serial_r);
	DECLARE_WRITE32_MEMBER(serial_w);
	DECLARE_READ8_MEMBER(io20_r);
	DECLARE_WRITE8_MEMBER(io20_w);
	virtual void machine_start();
	virtual void machine_reset();
	UINT32 screen_update_funkball(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
};

void funkball_state::video_start()
{
}

UINT32 funkball_state::screen_update( screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	return voodoo_update(m_voodoo, bitmap, cliprect) ? 0 : UPDATE_HAS_NOT_CHANGED;
}

static UINT32 voodoo_0_pci_r(device_t *busdevice, device_t *device, int function, int reg, UINT32 mem_mask)
{
	funkball_state* state = device->machine().driver_data<funkball_state>();
	UINT32 val = 0;

	printf("Voodoo PCI R: %x\n", reg);

	switch (reg)
	{
		case 0:
			val = 0x0001121a;
			break;
		case 0x10:
			val = state->m_voodoo_pci_regs.base_addr;
			break;
		case 0x40:
			val = state->m_voodoo_pci_regs.init_enable;
			break;
	}
	return val;
}

static void voodoo_0_pci_w(device_t *busdevice, device_t *device, int function, int reg, UINT32 data, UINT32 mem_mask)
{
	funkball_state* state = device->machine().driver_data<funkball_state>();

	printf("Voodoo [%x]: %x\n", reg, data);

	switch (reg)
	{
		case 0x04:
			state->m_voodoo_pci_regs.command = data & 0x3;
			break;
		case 0x10:
			if (data == 0xffffffff)
				state->m_voodoo_pci_regs.base_addr = 0xff000000;
			else
				state->m_voodoo_pci_regs.base_addr = data;
			break;
		case 0x40:
			state->m_voodoo_pci_regs.init_enable = data;
			voodoo_set_init_enable(state->m_voodoo, data);
			break;
	}
}

static UINT32 cx5510_pci_r(device_t *busdevice, device_t *device, int function, int reg, UINT32 mem_mask)
{
	funkball_state *state = busdevice->machine().driver_data<funkball_state>();

	//osd_printf_debug("CX5510: PCI read %d, %02X, %08X\n", function, reg, mem_mask);
	switch (reg)
	{
		case 0:     return 0x00001078;
	}

	return state->m_cx5510_regs[reg/4];
}

static void cx5510_pci_w(device_t *busdevice, device_t *device, int function, int reg, UINT32 data, UINT32 mem_mask)
{
	funkball_state *state = busdevice->machine().driver_data<funkball_state>();

	//osd_printf_debug("CX5510: PCI write %d, %02X, %08X, %08X\n", function, reg, data, mem_mask);
	COMBINE_DATA(state->m_cx5510_regs + (reg/4));
}

READ8_MEMBER( funkball_state::serial_r )
{
	//printf("%02x\n",offset);
	if(offset == 5)
		return 0x20;

	return 0;
}

WRITE8_MEMBER( funkball_state::serial_w )
{
	if(offset == 0)
	{
		if(data == 0x0d)
			printf("\n");
		else
			printf("%c",data);
	}
}

UINT8 funkball_state::funkball_config_reg_r()
{
	//osd_printf_debug("funkball_config_reg_r %02X\n", funkball_config_reg_sel);
	return m_funkball_config_regs[m_funkball_config_reg_sel];
}

void funkball_state::funkball_config_reg_w(UINT8 data)
{
	//osd_printf_debug("funkball_config_reg_w %02X, %02X\n", funkball_config_reg_sel, data);
	m_funkball_config_regs[m_funkball_config_reg_sel] = data;
}

READ8_MEMBER(funkball_state::io20_r)
{
	UINT8 r = 0;

	// 0x22, 0x23, Cyrix configuration registers
	if (offset == 0x00)
	{
	}
	else if (offset == 0x01)
	{
		r = funkball_config_reg_r();
	}
	return r;
}

WRITE8_MEMBER(funkball_state::io20_w)
{
	// 0x22, 0x23, Cyrix configuration registers
	if (offset == 0x00)
	{
		m_funkball_config_reg_sel = data;
	}
	else if (offset == 0x01)
	{
		funkball_config_reg_w(data);
	}
}

WRITE8_MEMBER( funkball_state::flash_w )
{
	if(!(offset & 0x2))
	{
		m_flash_addr = (offset & 1) ? ((m_flash_addr & 0xff) | (data << 8)) : ((m_flash_addr & 0xff00) | (data));
		//printf("%08x ADDR\n",m_flash_addr << 16);
	}
	else if(offset == 2)
	{
		/* 0x83: read from u29/u30
		   0x03: read from u3
		   0x81: init device
		*/
		m_flash_cmd = data;
		printf("%02x CMD\n",data);
	}
	else
		printf("%02x %02x\n",offset,data);
}

READ8_MEMBER( funkball_state::flash_data_r )
{
	if(m_flash_data_cmd == 0x90)
	{
		if(offset == 0 && (m_flash_addr == 0))
			return 0x89; // manufacturer code

		if(offset == 2 && (m_flash_addr == 0))
			return (m_flash_cmd & 0x80) ? 0x15 : 0x14; // device code, 32 MBit in both cases

		if(offset > 3)
			printf("%02x FLASH DATA 0x90\n",offset);

		return 0;
	}

	if(m_flash_data_cmd == 0xff)
	{
		UINT8 *ROM = memregion(m_flash_cmd & 0x80 ? "data_flash" : "prg_flash")->base();

		return ROM[offset + (m_flash_addr << 16)];
	}

	printf("%02x %08x %02x %02x\n",offset,m_flash_addr << 16,m_flash_cmd,m_flash_data_cmd);

	return 0;
}

WRITE8_MEMBER( funkball_state::flash_data_w )
{
	if(offset == 0)
	{
		m_flash_data_cmd = data;
	}
	else
		printf("%08x %02x FLASH DATA W %08x\n",offset,data,m_flash_addr << 16);
}

READ32_MEMBER(funkball_state::biu_ctrl_r)
{
	if (offset == 0)
	{
		return 0xffffff;
	}
	return m_biu_ctrl_reg[offset];
}

WRITE32_MEMBER(funkball_state::biu_ctrl_w)
{
	//osd_printf_debug("biu_ctrl_w %08X, %08X, %08X\n", data, offset, mem_mask);
	COMBINE_DATA(m_biu_ctrl_reg + offset);

	if (offset == 0x0c/4)       // BC_XMAP_3 register
	{
		const char *const banknames[8] = { "bios_ext1", "bios_ext2", "bios_ext3","bios_ext4", "bios_bank1", "bios_bank2", "bios_bank3", "bios_bank4" };
		int i;

		for(i=0;i<8;i++)
		{
			if (data & 0x1 << i*4)      // enable RAM access to region 0xe0000 - 0xfffff
				membank(banknames[i])->set_base(m_bios_ram + (0x4000 * i));
			else                    // disable RAM access (reads go to BIOS ROM)
				membank(banknames[i])->set_base(memregion("bios")->base() + (0x4000 * i));
		}
	}
}

WRITE8_MEMBER(funkball_state::bios_ram_w)
{
	if(m_biu_ctrl_reg[0x0c/4] & (2 << ((offset & 0x4000)>>14)*4)) // memory is write-able
	{
		m_bios_ram[offset] = data;
	}
}

READ8_MEMBER( funkball_state::test_r )
{
	static const char *const portnames[] = { "IN0", "IN1", "IN2", "IN3", "IN4", "IN5", "IN6", "IN7",
												"IN8", "IN9", "INA", "INB", "INC", "IND", "INE", "INF",};

	return ioport(portnames[offset])->read();
}

static ADDRESS_MAP_START(funkball_map, AS_PROGRAM, 32, funkball_state)
	AM_RANGE(0x00000000, 0x0009ffff) AM_RAM
	AM_RANGE(0x000a0000, 0x000affff) AM_RAM
	AM_RANGE(0x000b0000, 0x000bffff) AM_READWRITE8(flash_data_r,flash_data_w,0xffffffff)
	AM_RANGE(0x000c0000, 0x000cffff) AM_RAM
	AM_RANGE(0x000d0000, 0x000dffff) AM_RAM
	AM_RANGE(0x000e0000, 0x000e3fff) AM_ROMBANK("bios_ext1")
	AM_RANGE(0x000e4000, 0x000e7fff) AM_ROMBANK("bios_ext2")
	AM_RANGE(0x000e8000, 0x000ebfff) AM_ROMBANK("bios_ext3")
	AM_RANGE(0x000ec000, 0x000effff) AM_ROMBANK("bios_ext4")
	AM_RANGE(0x000f0000, 0x000f3fff) AM_ROMBANK("bios_bank1")
	AM_RANGE(0x000f4000, 0x000f7fff) AM_ROMBANK("bios_bank2")
	AM_RANGE(0x000f8000, 0x000fbfff) AM_ROMBANK("bios_bank3")
	AM_RANGE(0x000fc000, 0x000fffff) AM_ROMBANK("bios_bank4")
	AM_RANGE(0x000e0000, 0x000fffff) AM_WRITE8(bios_ram_w,0xffffffff)
	AM_RANGE(0x00100000, 0x07ffffff) AM_RAM
//  AM_RANGE(0x08000000, 0x0fffffff) AM_NOP
	AM_RANGE(0x40008000, 0x400080ff) AM_READWRITE(biu_ctrl_r, biu_ctrl_w)
	AM_RANGE(0x40010e00, 0x40010eff) AM_RAM AM_SHARE("unk_ram")
	AM_RANGE(0xff000000, 0xffffdfff) AM_DEVREADWRITE_LEGACY("voodoo_0", voodoo_r, voodoo_w)
	AM_RANGE(0xfffe0000, 0xffffffff) AM_ROM AM_REGION("bios", 0)    /* System BIOS */
ADDRESS_MAP_END

static ADDRESS_MAP_START(funkball_io, AS_IO, 32, funkball_state)
	AM_RANGE(0x0020, 0x0023) AM_READWRITE8(io20_r, io20_w, 0xffff0000)
	AM_IMPORT_FROM(pcat32_io_common)
	AM_RANGE(0x00e8, 0x00ef) AM_NOP

	AM_RANGE(0x01f0, 0x01f7) AM_DEVREADWRITE16("ide", ide_controller_device, read_cs0, write_cs0, 0xffffffff)
	AM_RANGE(0x03f0, 0x03f7) AM_DEVREADWRITE16("ide", ide_controller_device, read_cs1, write_cs1, 0xffffffff)
	AM_RANGE(0x03f8, 0x03ff) AM_READWRITE8(serial_r,serial_w,0xffffffff)

	AM_RANGE(0x0cf8, 0x0cff) AM_DEVREADWRITE("pcibus", pci_bus_legacy_device, read, write)

	AM_RANGE(0x0360, 0x0363) AM_WRITE8(flash_w,0xffffffff)

//  AM_RANGE(0x0320, 0x0323) AM_READ(test_r)
	AM_RANGE(0x0360, 0x036f) AM_READ8(test_r,0xffffffff) // inputs
ADDRESS_MAP_END

static INPUT_PORTS_START( funkball )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "0" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, "1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN2")
	PORT_DIPNAME( 0x01, 0x01, "2" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN3")
	PORT_DIPNAME( 0x01, 0x01, "3" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN4")
	PORT_DIPNAME( 0x01, 0x01, "4" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN5")
	PORT_DIPNAME( 0x01, 0x01, "5" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN6")
	PORT_DIPNAME( 0x01, 0x01, "6" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	/* 7-8 P1/P2 E-F "dgDelay" */
	PORT_START("IN7")
	PORT_DIPNAME( 0x01, 0x01, "7" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN8")
	PORT_DIPNAME( 0x01, 0x01, "8" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN9")
	PORT_DIPNAME( 0x01, 0x01, "9" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("INA")
	PORT_DIPNAME( 0x01, 0x01, "A" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("INB")
	PORT_DIPNAME( 0x01, 0x01, "B" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("INC")
	PORT_DIPNAME( 0x01, 0x01, "C" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IND")
	PORT_DIPNAME( 0x01, 0x01, "D" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("INE")
	PORT_DIPNAME( 0x01, 0x01, "E" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("INF")
	PORT_DIPNAME( 0x01, 0x01, "F" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

void funkball_state::machine_start()
{
	m_bios_ram = auto_alloc_array(machine(), UINT8, 0x20000);

	m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(funkball_state::irq_callback),this));

	/* defaults, otherwise it won't boot */
	m_unk_ram[0x010/4] = 0x2f8d85ff;
	m_unk_ram[0x018/4] = 0x000018c5;
}

void funkball_state::machine_reset()
{
	membank("bios_ext1")->set_base(memregion("bios")->base() + 0x00000);
	membank("bios_ext2")->set_base(memregion("bios")->base() + 0x04000);
	membank("bios_ext3")->set_base(memregion("bios")->base() + 0x08000);
	membank("bios_ext4")->set_base(memregion("bios")->base() + 0x0c000);
	membank("bios_bank1")->set_base(memregion("bios")->base() + 0x10000);
	membank("bios_bank2")->set_base(memregion("bios")->base() + 0x14000);
	membank("bios_bank3")->set_base(memregion("bios")->base() + 0x18000);
	membank("bios_bank4")->set_base(memregion("bios")->base() + 0x1c000);
	m_voodoo_pci_regs.base_addr = 0xff000000;
}

UINT32 funkball_state::screen_update_funkball(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	voodoo_update(machine().device("voodoo_0"), bitmap, cliprect);
	return 0;
}

static const voodoo_config voodoo_intf =
{
	2, //               fbmem;
	4,//                tmumem0;
	0,//                tmumem1;
	"screen",//         screen;
	"maincpu",//        cputag;
	DEVCB_NULL,//             vblank;
	DEVCB_NULL//             stall;
};

static MACHINE_CONFIG_START( funkball, funkball_state )
	MCFG_CPU_ADD("maincpu", MEDIAGX, 66666666*3.5) // 66,6 MHz x 3.5
	MCFG_CPU_PROGRAM_MAP(funkball_map)
	MCFG_CPU_IO_MAP(funkball_io)

	MCFG_FRAGMENT_ADD( pcat_common )

	MCFG_PCI_BUS_LEGACY_ADD("pcibus", 0)
	MCFG_PCI_BUS_LEGACY_DEVICE(7, "voodoo_0", voodoo_0_pci_r, voodoo_0_pci_w)
	MCFG_PCI_BUS_LEGACY_DEVICE(18, NULL, cx5510_pci_r, cx5510_pci_w)

	MCFG_IDE_CONTROLLER_ADD("ide", ata_devices, "hdd", NULL, true)
	MCFG_ATA_INTERFACE_IRQ_HANDLER(DEVWRITELINE("pic8259_2", pic8259_device, ir6_w))

	/* video hardware */
	MCFG_3DFX_VOODOO_1_ADD("voodoo_0", STD_VOODOO_1_CLOCK, voodoo_intf)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_UPDATE_DRIVER(funkball_state, screen_update)
	MCFG_SCREEN_SIZE(1024, 1024)
	MCFG_SCREEN_VISIBLE_AREA(0, 511, 16, 447)
MACHINE_CONFIG_END

ROM_START( funkball )
	ROM_REGION32_LE(0x20000, "bios", ROMREGION_ERASEFF)
	ROM_LOAD( "512k-epr.u62", 0x010000, 0x010000, CRC(cced894a) SHA1(298c81716e375da4b7215f3e588a45ca3ea7e35c) )

	ROM_REGION(0x8000000, "prg_flash", ROMREGION_ERASE00)
	ROM_LOAD16_WORD_SWAP( "flash.u3", 0x0000000, 0x400000, CRC(fb376abc) SHA1(ea4c48bb6cd2055431a33f5c426e52c7af6997eb) )

	ROM_REGION(0x8000000, "data_flash", ROMREGION_ERASE00)
	ROM_LOAD( "flash.u29",0x0000000, 0x400000, CRC(7cf6ff4b) SHA1(4ccdd4864ad92cc218998f3923997119a1a9dd1d) )
	ROM_LOAD( "flash.u30",0x0400000, 0x400000, CRC(1d46717a) SHA1(acfbd0a2ccf4d717779733c4a9c639296c3bbe0e) )
ROM_END


GAME(1998, funkball, 0, funkball, funkball, driver_device, 0, ROT0, "dgPIX Entertainment Inc.", "Funky Ball", GAME_NOT_WORKING | GAME_NO_SOUND)
