/* Sega MegaTech

About MegaTech:

Megatech games are identical to their Genesis/SMS equivlents, however the Megatech cartridges contain
a BIOS rom with the game instructions.  The last part number of the bios ROM is the cart/game ID code.

The instruction rom appears to map at 0x300000 in the cart space.

In Megatech games your coins buy you time to play the game, how you perform in the game does not
matter, you can die and start a new game providing you still have time, likewise you can be playing
well and run out of time if you fail to insert more coins.  This is the same method Nintendo used
with their Playchoice 10 system.

The BIOS screen is based around SMS hardware, with an additional Z80 and SMS VDP chip not present on
a standard Genesis.

SMS games run on Megatech in the Genesis's SMS compatability mode, where the Genesis Z80 becomes the
main CPU and the Genesis VDP acts in a mode mimicing the behavior of the SMS VDP. A pin on the carts
determines which mode the game runs in.

Additions will only be made to this driver if proof that the dumped set are original roms with original
Sega part numbers is given..


Sega Mega Tech Cartridges (Readme by Guru)
-------------------------

These are cart-based games for use with Sega Mega Tech hardware. There are 6 known types of carts. All carts
are very simple, almost exactly the same as Mega Play carts. They contain just 2 or 3 ROMs.
PCB 171-6215A has locations for 2 ROMs and is dated 1991. PCB 171-6215A is also used in Mega Play!
PCB 171-5782 has locations for 2 ROMs and is dated 1989.
PCB 171-5869A has locations for 3 ROMs and is dated 1989.
PCB 171-5834 has locations for 3 ROMs and is dated 1989.
PCB 171-5783 has locations for 2 ROMs and is dated 1989.
PCB 171-5784 has locations for 2 ROMs and is dated 1989. It also contains a custom Sega IC 315-5235

                                                                           |------------------------------- ROMs --------------------------------|
                                                                           |                                                                     |
Game                       PCB #       Sticker on PCB    Sticker on cart     IC1                          IC2                      IC3
-------------------------------------------------------------------------------------------------------------------------------------------------
Altered Beast              171-5782    837-6963-01       610-0239-01         MPR-12538F     (834200A)     EPR-12368-01   (27C256)  n/a
Space Harrier II           171-5782    837-6963-02       610-0239-02         MPR-11934      (834200)      EPR-12368-02   (27256)   n/a
Out Run                    171-5783    837-6963-06       610-0239-06         MPR-11078      (Mask)        EPR-12368-06   (27256)   n/a
Alien Syndrome             171-5783    837-6963-07       610-0239-07         MPR-11194      (232011)      EPR-12368-07   (27256)   n/a
Afterburner                171-5784    837-6963-10       610-0239-10         315-5235       (custom)      MPR-11271-T    (834000)  EPR-12368-10 (27256)
Great Football             171-5783    837-6963-19       610-0239-19         MPR-10576F     (831000)      EPR-12368-19   (27256)   n/a
World Championship Soccer  171-5782    837-6963-21       610-0239-21         MPR-12607B     (uPD23C4000)  EPR-12368-21   (27256)   n/a
Tetris                     171-5834    837-6963-22       610-0239-22         MPR-12356F     (831000)      MPR-12357F     (831000)  EPR-12368-22 (27256)
Ghouls & Ghosts            171-5869A   -                 610-0239-23         MPR-12605      (40 pins)     MPR-12606      (40 pins) EPR-12368-23 (27256)
Super Hang On              171-5782    837-6963-24       610-0239-24         MPR-12640      (234000)      EPR-12368-24   (27256)   n/a
Forgotten Worlds           171-5782    837-6963-26       610-0239-26         MPR-12672-H    (Mask)        EPR-12368-26   (27256)   n/a
The Revenge Of Shinobi     171-5782    837-6963-28       610-0239-28         MPR-12675 S44  (uPD23C4000)  EPR-12368-28   (27C256)  n/a
Arnold Palmer Tour Golf    171-5782    837-6963-31       610-0239-31         MPR-12645F     (23C4000)     EPR-12368-31   (27256)   n/a
Super Real Basket Ball     171-5782    837-6963-32       610-0239-32         MPR-12904F     (838200A)     EPR-12368-32   (27256)   n/a
Tommy Lasorda Baseball     171-5782    837-6963-35       610-0239-35         MPR-12706F     (834200A)     EPR-12368-35   (27256)   n/a
ESWAT                      171-5782    837-6963-38       610-0239-38         MPR-13192-H    (uPD23C4000)  EPR-12368-38   (27256)   n/a
Moonwalker                 171-5782    837-6963-40       610-0239-40         MPR-13285A S61 (uPD23C4000)  EPR-12368-40   (27256)   n/a
Shadow Dancer              171-5782    837-6963-43       610-0239-43         MPR-13571-S    (uPD23C4000)  EPR-12368-43   (27256)   n/a
Wrestle War                171-5782    837-6963-48       610-0239-48         MPR-14025-F    (23C4000)     EPR-12368-48   (27256)   n/a
Bonanza Bros.              171-5782    837-6963-49       610-0239-49         MPR-13905A-F   (23C4000)     EPR-12368-49   (27256)   n/a
Streets of Rage            171-5782    837-6963-51       610-0239-51         MPR-14125-SM   (uPD23C4000)  EPR-12368-51   (27C256)  n/a
Sonic The Hedgehog         171-5782    837-6963-52       610-0239-52         MPR-13913-F    (834200A)     EPR-12368-52   (27C256)  n/a
Spider-Man                 171-5782    837-6963-54       610-0239-54         MPR-14027-SM   (uPD23C4000)  EPR-12368-54   (27C256)  n/a
California Games           171-5834    837-6963-55-01    610-0239-55         EPR-14494      (27C020)      EPR-14495      (27C020)  EPR-12368-55 (27C256)
Mario Lemeux Hockey        171-5782    837-6963-59       610-0239-59         MPR-14376-H    (234000)      EPR-12368-59   (27256)   n/a
Turbo Outrun               171-5782    837-6963-61       610-0239-61         MPR-14674      (uPD23C4000)  EPR-12368-61   (27256)   n/a
Sonic Hedgehog 2           171-6215A   837-6963-62       610-0239-62         MPR-15000A-F   (838200)      EPR-12368-62   (27256)   n/a

*/
#include "emu.h"
#include "cpu/z80/z80.h"
#include "sound/sn76496.h"
#include "rendlay.h"

#include "includes/megadriv.h"
#include "imagedev/cartslot.h"

#define MASTER_CLOCK        53693100



class mtech_state : public md_base_state
{
public:
	enum
	{
		TIMER_Z80_RUN_STATE,
		TIMER_Z80_STOP_STATE
	};
	
	mtech_state(const machine_config &mconfig, device_type type, const char *tag)
	: md_base_state(mconfig, type, tag),
	m_vdp1(*this, "vdp1"),
	m_bioscpu(*this, "mtbios")
	{ }
	
	DECLARE_WRITE_LINE_MEMBER( snd_int_callback );
	DECLARE_WRITE_LINE_MEMBER( bios_int_callback );
	DECLARE_READ8_MEMBER(cart_select_r);
	DECLARE_WRITE8_MEMBER(cart_select_w);
	DECLARE_READ8_MEMBER(bios_ctrl_r);
	DECLARE_WRITE8_MEMBER(bios_ctrl_w);
	DECLARE_READ8_MEMBER(read_68k_banked_data);
	DECLARE_WRITE8_MEMBER(write_68k_banked_data);
	DECLARE_WRITE8_MEMBER(mt_z80_bank_w);
	DECLARE_READ8_MEMBER(banked_ram_r);
	DECLARE_WRITE8_MEMBER(banked_ram_w);
	DECLARE_WRITE8_MEMBER(bios_port_ctrl_w);
	DECLARE_READ8_MEMBER(bios_joypad_r);
	DECLARE_WRITE8_MEMBER(bios_port_7f_w);
	DECLARE_READ8_MEMBER(vdp1_count_r);
	DECLARE_READ8_MEMBER(sms_count_r);
	DECLARE_READ8_MEMBER(sms_ioport_dc_r);
	DECLARE_READ8_MEMBER(sms_ioport_dd_r);
	DECLARE_WRITE8_MEMBER(mt_sms_standard_rom_bank_w);	
	
	DECLARE_DRIVER_INIT(mt_crt);
	DECLARE_DRIVER_INIT(mt_slot);
	DECLARE_MACHINE_RESET(megatech);
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(megatech_cart);
	UINT32 screen_update_main(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_menu(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void screen_eof_main(screen_device &screen, bool state);
	
protected:
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	
private:	
	UINT8 m_mt_cart_select_reg;
	UINT32 m_bios_port_ctrl;
	int m_current_game_is_sms; // is the current game SMS based (running on genesis z80, in VDP compatibility mode)
	UINT32 m_bios_ctrl_inputs;
	UINT8 m_bios_ctrl[6];
	int m_mt_bank_addr;
	
	int m_cart_is_genesis[8];
	
	void set_genz80_as_md();
	void set_genz80_as_sms();
	
	TIMER_CALLBACK_MEMBER(z80_run_state);
	TIMER_CALLBACK_MEMBER(z80_stop_state);
	
	UINT8* m_banked_ram;
	UINT8* sms_mainram;
	UINT8* sms_rom;
	
	required_device<sega315_5124_device> m_vdp1;
	required_device<cpu_device>          m_bioscpu;	
};



/* not currently used */
static INPUT_PORTS_START( megatech ) /* Genesis Input Ports */
	PORT_INCLUDE(megadriv)

	PORT_START("BIOS_IN0") // port 6
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_NAME("Select") PORT_CODE(KEYCODE_0)
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_SERVICE_NO_TOGGLE( 0x80, IP_ACTIVE_LOW )

	PORT_START("BIOS_IN1") // port 6
	PORT_BIT(  0x01, IP_ACTIVE_LOW, IPT_COIN1 )  // a few coin inputs here
	PORT_BIT(  0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT(  0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT(  0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT(  0x10, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_NAME("Service coin") PORT_CODE(KEYCODE_9)
	PORT_BIT(  0x20, IP_ACTIVE_LOW, IPT_SERVICE3 ) PORT_NAME("Enter") PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(  0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT(  0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("BIOS_DSW0")
	PORT_DIPNAME( 0x02, 0x02, "Coin slot 3" )
	PORT_DIPSETTING(    0x00, "Inhibit" )
	PORT_DIPSETTING(    0x02, "Accept" )
	PORT_DIPNAME( 0x01, 0x01, "Coin slot 4" )
	PORT_DIPSETTING(    0x00, "Inhibit" )
	PORT_DIPSETTING(    0x01, "Accept" )
	PORT_DIPNAME( 0x1c, 0x1c, "Coin slot 3/4 value" )
	PORT_DIPSETTING(    0x1c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_8C ) )
	PORT_DIPSETTING(    0x00, "1 Coin/10 credits" )
	PORT_DIPNAME( 0xe0, 0x60, "Coin slot 2 value" )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Inhibit" )

	PORT_START("BIOS_DSW1")
	PORT_DIPNAME( 0x0f, 0x01, "Coin Slot 1 value" )
	PORT_DIPSETTING(    0x00, "Inhibit" )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_8C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 1C_9C ) )
	PORT_DIPSETTING(    0x0a, "1 coin/10 credits" )
	PORT_DIPSETTING(    0x0b, "1 coin/11 credits" )
	PORT_DIPSETTING(    0x0c, "1 coin/12 credits" )
	PORT_DIPSETTING(    0x0d, "1 coin/13 credits" )
	PORT_DIPSETTING(    0x0e, "1 coin/14 credits" )
	PORT_DIPSETTING(    0x0f, "1 coin/15 credits" )
	PORT_DIPNAME( 0xf0, 0xa0, "Time per credit" )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x10, "7:30" )
	PORT_DIPSETTING(    0x20, "7:00" )
	PORT_DIPSETTING(    0x30, "6:30" )
	PORT_DIPSETTING(    0x40, "6:00" )
	PORT_DIPSETTING(    0x50, "5:30" )
	PORT_DIPSETTING(    0x60, "5:00" )
	PORT_DIPSETTING(    0x70, "4:30" )
	PORT_DIPSETTING(    0x80, "4:00" )
	PORT_DIPSETTING(    0x90, "3:30" )
	PORT_DIPSETTING(    0xa0, "3:00" )
	PORT_DIPSETTING(    0xb0, "2:30" )
	PORT_DIPSETTING(    0xc0, "2:00" )
	PORT_DIPSETTING(    0xd0, "1:30" )
	PORT_DIPSETTING(    0xe0, "1:00" )
	PORT_DIPSETTING(    0xf0, "0:30" )


	PORT_START("BIOS_J1")
	PORT_DIPNAME( 0x0001, 0x0001, "5" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

/* MEGATECH specific */
READ8_MEMBER(mtech_state::cart_select_r )
{
	return m_mt_cart_select_reg;
}

void mtech_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_Z80_RUN_STATE:
		z80_run_state(ptr, param);
		break;
	case TIMER_Z80_STOP_STATE:
		z80_stop_state(ptr, param);
		break;
	default:
		assert_always(FALSE, "Unknown id in mtech_state::device_timer");
	}
}

READ8_MEMBER(mtech_state::sms_count_r)
{
	address_space &prg = m_z80snd->space(AS_PROGRAM);
	if (offset & 0x01)
		return m_vdp->hcount_read(prg, offset);
	else
		return m_vdp->vcount_read(prg, offset);
}


READ8_MEMBER (mtech_state::sms_ioport_dc_r)
{
	/* 2009-05 FP: would it be worth to give separate inputs to SMS? SMS has only 2 keys A,B (which are B,C on megadrive) */
	/* bit 4: TL-A; bit 5: TR-A */
	return (machine().root_device().ioport("PAD1")->read() & 0x3f) | ((machine().root_device().ioport("PAD2")->read() & 0x03) << 6);
}

READ8_MEMBER (mtech_state::sms_ioport_dd_r)
{
	/* 2009-05 FP: would it be worth to give separate inputs to SMS? SMS has only 2 keys A,B (which are B,C on megadrive) */
	/* bit 2: TL-B; bit 3: TR-B; bit 4: RESET; bit 5: unused; bit 6: TH-A; bit 7: TH-B*/
	return ((machine().root_device().ioport("PAD2")->read() & 0x3c) >> 2) | 0x10;
}


WRITE8_MEMBER( mtech_state::mt_sms_standard_rom_bank_w )
{
	int bank = data & 0x1f;
	//logerror("bank w %02x %02x\n", offset, data);
	
	sms_mainram[0x1ffc + offset] = data;
	switch (offset)
	{
		case 0:
			logerror("bank w %02x %02x\n", offset, data);
			space.install_rom(0x0000, 0xbfff, sms_rom);
			space.unmap_write(0x0000, 0xbfff);
			//printf("bank ram??\n");
			break;
		case 1:
			memcpy(sms_rom+0x0000, space.machine().root_device().memregion("maincpu")->base()+bank*0x4000, 0x4000);
			break;
		case 2:
			memcpy(sms_rom+0x4000, space.machine().root_device().memregion("maincpu")->base()+bank*0x4000, 0x4000);
			break;
		case 3:
			memcpy(sms_rom+0x8000, space.machine().root_device().memregion("maincpu")->base()+bank*0x4000, 0x4000);
			break;
			
	}
}

void mtech_state::set_genz80_as_sms()
{
	address_space &prg = machine().device("genesis_snd_z80")->memory().space(AS_PROGRAM);
	address_space &io = machine().device("genesis_snd_z80")->memory().space(AS_IO);
	sn76496_base_device *sn = machine().device<sn76496_base_device>("snsnd");
	
	// main ram area
	sms_mainram = (UINT8 *)prg.install_ram(0xc000, 0xdfff, 0, 0x2000);
	memset(sms_mainram,0x00,0x2000);
	
	// fixed rom bank area
	sms_rom = (UINT8 *)prg.install_rom(0x0000, 0xbfff, NULL);
	
	memcpy(sms_rom, machine().root_device().memregion("maincpu")->base(), 0xc000);
	
	prg.install_write_handler(0xfffc, 0xffff, write8_delegate(FUNC(mtech_state::mt_sms_standard_rom_bank_w),this));
	
	// ports
	io.install_read_handler      (0x40, 0x41, 0xff, 0x3e, read8_delegate(FUNC(mtech_state::sms_count_r),this));
	io.install_write_handler     (0x40, 0x41, 0xff, 0x3e, write8_delegate(FUNC(sn76496_device::write),sn));
	io.install_readwrite_handler (0x80, 0x80, 0xff, 0x3e, read8_delegate(FUNC(sega315_5124_device::vram_read),(sega315_5124_device *)m_vdp), write8_delegate(FUNC(sega315_5124_device::vram_write),(sega315_5124_device *)m_vdp));
	io.install_readwrite_handler (0x81, 0x81, 0xff, 0x3e, read8_delegate(FUNC(sega315_5124_device::register_read),(sega315_5124_device *)m_vdp), write8_delegate(FUNC(sega315_5124_device::register_write),(sega315_5124_device *)m_vdp));
	
	io.install_read_handler      (0x10, 0x10, read8_delegate(FUNC(mtech_state::sms_ioport_dd_r),this)); // super tetris
	
	io.install_read_handler      (0xdc, 0xdc, read8_delegate(FUNC(mtech_state::sms_ioport_dc_r),this));
	io.install_read_handler      (0xdd, 0xdd, read8_delegate(FUNC(mtech_state::sms_ioport_dd_r),this));
	io.install_read_handler      (0xde, 0xde, read8_delegate(FUNC(mtech_state::sms_ioport_dd_r),this));
	io.install_read_handler      (0xdf, 0xdf, read8_delegate(FUNC(mtech_state::sms_ioport_dd_r),this)); // adams family
}


/* sets the megadrive z80 to it's normal ports / map */
void mtech_state::set_genz80_as_md()
{
	address_space &prg = machine().device("genesis_snd_z80")->memory().space(AS_PROGRAM);
	ym2612_device *ym2612 = machine().device<ym2612_device>("ymsnd");

	prg.install_readwrite_bank(0x0000, 0x1fff, "bank1");
	machine().root_device().membank("bank1")->set_base(m_genz80.z80_prgram);
	
	prg.install_ram(0x0000, 0x1fff, m_genz80.z80_prgram);

	prg.install_readwrite_handler(0x4000, 0x4003, read8_delegate(FUNC(ym2612_device::read),ym2612), write8_delegate(FUNC(ym2612_device::write),ym2612));
	prg.install_write_handler    (0x6000, 0x6000, write8_delegate(FUNC(mtech_state::megadriv_z80_z80_bank_w),this));
	prg.install_write_handler    (0x6001, 0x6001, write8_delegate(FUNC(mtech_state::megadriv_z80_z80_bank_w),this));
	prg.install_read_handler     (0x6100, 0x7eff, read8_delegate(FUNC(mtech_state::megadriv_z80_unmapped_read),this));
	prg.install_readwrite_handler(0x7f00, 0x7fff, read8_delegate(FUNC(mtech_state::megadriv_z80_vdp_read),this), write8_delegate(FUNC(mtech_state::megadriv_z80_vdp_write),this));
	prg.install_readwrite_handler(0x8000, 0xffff, read8_delegate(FUNC(mtech_state::z80_read_68k_banked_data),this), write8_delegate(FUNC(mtech_state::z80_write_68k_banked_data),this));
}


TIMER_CALLBACK_MEMBER(mtech_state::z80_run_state )
{
	char tempname[20];
	UINT8* game_region;

	sprintf(tempname, "game%d", param);
	game_region = memregion(tempname)->base();

	memcpy(memregion("maincpu")->base(), game_region, 0x400000);

	if (!m_cart_is_genesis[param])
	{
		logerror("enabling SMS Z80\n");
		m_current_game_is_sms = 1;
		set_genz80_as_sms();
		//m_z80snd->set_input_line(INPUT_LINE_HALT, CLEAR_LINE);
		m_z80snd->set_input_line(INPUT_LINE_RESET, CLEAR_LINE);
	}
	else
	{
		logerror("disabling SMS Z80\n");
		m_current_game_is_sms = 0;
		set_genz80_as_md();
		m_maincpu->set_input_line(INPUT_LINE_RESET, CLEAR_LINE);
		//m_maincpu->set_input_line(INPUT_LINE_HALT, CLEAR_LINE);
	}
}

TIMER_CALLBACK_MEMBER(mtech_state::z80_stop_state )
{
	UINT8* game_region;
	char tempname[20];
	logerror("select game %d\n", param + 1);

	sprintf(tempname, "game%d", param);
	game_region = memregion(tempname)->base();

	m_maincpu->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
	m_z80snd->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
	//m_maincpu->set_input_line(INPUT_LINE_HALT, ASSERT_LINE);
	//m_z80snd->set_input_line(INPUT_LINE_HALT, ASSERT_LINE);
	machine().device("ymsnd")->reset();

	megadriv_stop_scanline_timer();// stop the scanline timer for the genesis vdp... it can be restarted in video eof when needed
	m_vdp->reset();

	/* if the regions exist we're fine */
	if (game_region)
	{
		timer_set(attotime::zero, TIMER_Z80_RUN_STATE, param);
	}
	else
	{
		/* no cart.. */
		memset(memregion("mtbios")->base() + 0x8000, 0x00, 0x8000);
		memset(memregion("maincpu")->base(), 0x00, 0x400000);
	}

	return;
}

WRITE8_MEMBER(mtech_state::cart_select_w )
{
	/* seems to write the slot number..
	  but it stores something in (banked?) ram
	  because it always seems to show the
	  same instructions ... */
	m_mt_cart_select_reg = data;
	timer_set(attotime::zero, TIMER_Z80_STOP_STATE, m_mt_cart_select_reg);
}


READ8_MEMBER(mtech_state::bios_ctrl_r )
{
	if (offset == 0)
		return 0;
	if (offset == 2)
		return m_bios_ctrl[offset] & 0xfe;

	return m_bios_ctrl[offset];
}

WRITE8_MEMBER(mtech_state::bios_ctrl_w )
{
	if (offset == 1)
	{
		output_set_value("Alarm_sound", BIT(data, 7));
		m_bios_ctrl_inputs = data & 0x04;  // Genesis/SMS input ports disable bit
	}
	else if (offset == 2)
		output_set_value("Flash_screen", BIT(data, 1));

	m_bios_ctrl[offset] = data;
}

/* this sets 0x300000 which may indicate that the 68k can see the instruction rom
   there, this limiting the max game rom capacity to 3meg. */

READ8_MEMBER(mtech_state::read_68k_banked_data )
{
	address_space &space68k = m_maincpu->space();
	UINT8 ret = space68k.read_byte(m_mt_bank_addr + offset);
	return ret;
}

WRITE8_MEMBER(mtech_state::write_68k_banked_data )
{
	address_space &space68k = m_maincpu->space();
	space68k.write_byte(m_mt_bank_addr + offset,data);
}

WRITE8_MEMBER(mtech_state::mt_z80_bank_w )
{
	m_mt_bank_addr = ((m_mt_bank_addr >> 1) | (data << 23)) & 0xff8000;
}

READ8_MEMBER(mtech_state::banked_ram_r )
{
	return m_banked_ram[offset + 0x1000 * (m_mt_cart_select_reg & 0x07)];
}

WRITE8_MEMBER(mtech_state::banked_ram_w )
{
	m_banked_ram[offset + 0x1000 * (m_mt_cart_select_reg & 0x07)] = data;
}



static ADDRESS_MAP_START( megatech_bios_map, AS_PROGRAM, 8, mtech_state )
	AM_RANGE(0x0000, 0x2fff) AM_ROM // from bios rom (0x0000-0x2fff populated in ROM)
	AM_RANGE(0x3000, 0x3fff) AM_READWRITE(banked_ram_r, banked_ram_w) // copies instruction data here at startup, must be banked
	AM_RANGE(0x4000, 0x5fff) AM_RAM // plain ram?
	AM_RANGE(0x6000, 0x6000) AM_WRITE(mt_z80_bank_w )
	AM_RANGE(0x6400, 0x6400) AM_READ_PORT("BIOS_DSW0")
	AM_RANGE(0x6401, 0x6401) AM_READ_PORT("BIOS_DSW1")
	AM_RANGE(0x6404, 0x6404) AM_READWRITE(cart_select_r, cart_select_w) // cart select & ram bank
	AM_RANGE(0x6800, 0x6800) AM_READ_PORT("BIOS_IN0")
	AM_RANGE(0x6801, 0x6801) AM_READ_PORT("BIOS_IN1")
	AM_RANGE(0x6802, 0x6807) AM_READWRITE(bios_ctrl_r, bios_ctrl_w)
//  AM_RANGE(0x6805, 0x6805) AM_READ_PORT("???")
	AM_RANGE(0x7000, 0x77ff) AM_ROM // from bios rom (0x7000-0x77ff populated in ROM)
	//AM_RANGE(0x7800, 0x7fff) AM_RAM // ?
	AM_RANGE(0x8000, 0x9fff) AM_READWRITE(read_68k_banked_data, write_68k_banked_data) // window into 68k address space, reads instr rom and writes to reset banks on z80 carts?
ADDRESS_MAP_END


WRITE8_MEMBER(mtech_state::bios_port_ctrl_w )
{
	m_bios_port_ctrl = data;
}

/* the test mode accesses the joypad/stick inputs like this */
READ8_MEMBER(mtech_state::bios_joypad_r )
{
	UINT8 retdata = 0;
	
	if (m_bios_port_ctrl == 0x55)
	{
		/* A keys */
		retdata = ((m_io_pad_3b[0]->read() & 0x40) >> 2) | ((m_io_pad_3b[1]->read() & 0x40) >> 4) | 0xeb;
	}
	else
	{
		if (offset == 0)
			retdata = (m_io_pad_3b[0]->read() & 0x3f) | ((m_io_pad_3b[1]->read() & 0x03) << 6);
		else
			retdata = ((m_io_pad_3b[1]->read() & 0x3c) >> 2) | 0xf0;
		
	}
	return retdata;
}

WRITE8_MEMBER(mtech_state::bios_port_7f_w)
{
//  popmessage("CPU #3: I/O port 0x7F write, data %02x", data);
}


READ8_MEMBER(mtech_state::vdp1_count_r)
{
	address_space &prg = m_bioscpu->space(AS_PROGRAM);
	if (offset & 0x01)
		return m_vdp1->hcount_read(prg, offset);
	else
		return m_vdp1->vcount_read(prg, offset);
}

static ADDRESS_MAP_START( megatech_bios_portmap, AS_IO, 8, mtech_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x3f, 0x3f) AM_WRITE(bios_port_ctrl_w)
	AM_RANGE(0x7f, 0x7f) AM_WRITE(bios_port_7f_w)

	AM_RANGE(0x40, 0x41) AM_MIRROR(0x3e) AM_READ(vdp1_count_r)
	AM_RANGE(0x80, 0x80) AM_MIRROR(0x3e) AM_DEVREADWRITE("vdp1", sega315_5124_device, vram_read, vram_write)
	AM_RANGE(0x81, 0x81) AM_MIRROR(0x3e) AM_DEVREADWRITE("vdp1", sega315_5124_device, register_read, register_write)

	AM_RANGE(0xdc, 0xdd) AM_READ(bios_joypad_r)  // player inputs
ADDRESS_MAP_END



DRIVER_INIT_MEMBER(mtech_state,mt_slot)
{
	m_banked_ram = auto_alloc_array(machine(), UINT8, 0x1000*8);

	DRIVER_INIT_CALL(megadriv);

	// this gets set in DEVICE_IMAGE_LOAD
	memset(m_cart_is_genesis, 0, sizeof(m_cart_is_genesis));
}

DRIVER_INIT_MEMBER(mtech_state,mt_crt)
{
	UINT8* pin = memregion("sms_pin")->base();
	DRIVER_INIT_CALL(mt_slot);

	m_cart_is_genesis[0] = !pin[0] ? 1 : 0;;
}


UINT32 mtech_state::screen_update_main(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	// if we're running an sms game then use the SMS update.. maybe this should be moved to the megadrive emulation core as compatibility mode is a feature of the chip
	if (!m_current_game_is_sms)
		screen_update_megadriv(screen, bitmap, cliprect);
	else
	{
		m_vdp->screen_update(screen, bitmap, cliprect);
#if 0
		// when launching megatech + both sms and megadrive games, the following would be needed...
		for (int y = 0; y < 224; y++)
		{
			UINT32* lineptr = &bitmap.pix32(y);
			UINT32* srcptr =  &m_vdp->get_bitmap().pix32(y + SEGA315_5124_TBORDER_START + SEGA315_5124_NTSC_224_TBORDER_HEIGHT);
			
			for (int x = 0; x < SEGA315_5124_WIDTH; x++)
				lineptr[x] = srcptr[x];
		}	
#endif
	}
	return 0;
}

void mtech_state::screen_eof_main(screen_device &screen, bool state)
{
	if (!m_current_game_is_sms)
		screen_eof_megadriv(screen, state);
}

MACHINE_RESET_MEMBER(mtech_state, megatech)
{
	m_mt_bank_addr = 0;
	MACHINE_RESET_CALL_MEMBER(megadriv);
	timer_set(attotime::zero, TIMER_Z80_STOP_STATE, 0);
}

UINT32 mtech_state::screen_update_menu(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	m_vdp1->screen_update(screen, bitmap, cliprect);
	return 0;
}



WRITE_LINE_MEMBER( mtech_state::bios_int_callback )
{
	m_bioscpu->set_input_line(0, state);
}

WRITE_LINE_MEMBER( mtech_state::snd_int_callback )
{
	m_z80snd->set_input_line(0, state);
}

static MACHINE_CONFIG_START( megatech, mtech_state )
	/* basic machine hardware */
	MCFG_FRAGMENT_ADD(md_ntsc)

	/* Megatech has an extra SMS based bios *and* an additional screen */
	MCFG_CPU_ADD("mtbios", Z80, MASTER_CLOCK / 15) /* ?? */
	MCFG_CPU_PROGRAM_MAP(megatech_bios_map)
	MCFG_CPU_IO_MAP(megatech_bios_portmap)

	MCFG_MACHINE_RESET_OVERRIDE(mtech_state, megatech)

	MCFG_DEFAULT_LAYOUT(layout_dualhovu)

	MCFG_SCREEN_MODIFY("megadriv")
	MCFG_SCREEN_RAW_PARAMS(XTAL_10_738635MHz/2, \
		SEGA315_5124_WIDTH , SEGA315_5124_LBORDER_START + SEGA315_5124_LBORDER_WIDTH, SEGA315_5124_LBORDER_START + SEGA315_5124_LBORDER_WIDTH + 256, \
		SEGA315_5124_HEIGHT_NTSC, SEGA315_5124_TBORDER_START + SEGA315_5124_NTSC_224_TBORDER_HEIGHT, SEGA315_5124_TBORDER_START + SEGA315_5124_NTSC_224_TBORDER_HEIGHT + 224)
	MCFG_SCREEN_UPDATE_DRIVER(mtech_state, screen_update_main)
	MCFG_SCREEN_VBLANK_DRIVER(mtech_state, screen_eof_main)

	MCFG_DEVICE_MODIFY("gen_vdp")
	MCFG_SEGA315_5313_INT_CB(WRITELINE(mtech_state, snd_int_callback))

	MCFG_SCREEN_ADD("menu", RASTER)
	// check frq
	MCFG_SCREEN_RAW_PARAMS(XTAL_10_738635MHz/2, \
		SEGA315_5124_WIDTH , SEGA315_5124_LBORDER_START + SEGA315_5124_LBORDER_WIDTH, SEGA315_5124_LBORDER_START + SEGA315_5124_LBORDER_WIDTH + 256, \
		SEGA315_5124_HEIGHT_NTSC, SEGA315_5124_TBORDER_START + SEGA315_5124_NTSC_224_TBORDER_HEIGHT, SEGA315_5124_TBORDER_START + SEGA315_5124_NTSC_224_TBORDER_HEIGHT + 224)
	MCFG_SCREEN_UPDATE_DRIVER(mtech_state, screen_update_menu)

	MCFG_DEVICE_ADD("vdp1", SEGA315_5246, 0)
	MCFG_SEGA315_5246_SET_SCREEN("menu")
	MCFG_SEGA315_5246_IS_PAL(false)
	MCFG_SEGA315_5246_INT_CB(WRITELINE(mtech_state, bios_int_callback))

	/* sound hardware */
	MCFG_SOUND_ADD("sn2", SN76496, MASTER_CLOCK/15)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.50)
MACHINE_CONFIG_END


struct megatech_cart_region
{
	const char *tag;
	int        slot;
	const char *region;
};

// we keep old region tags for compatibility with older macros... this might be changed at a later stage
static const struct megatech_cart_region megatech_cart_table[] =
{
	{ ":cart1", 0, ":game0" },
	{ ":cart2", 1, ":game1" },
	{ ":cart3", 2, ":game2" },
	{ ":cart4", 3, ":game3" },
	{ ":cart5", 4, ":game4" },
	{ ":cart6", 5, ":game5" },
	{ ":cart7", 6, ":game6" },
	{ ":cart8", 7, ":game7" },
	{ 0 }
};

DEVICE_IMAGE_LOAD_MEMBER( mtech_state, megatech_cart )
{
	const struct megatech_cart_region *mt_cart = &megatech_cart_table[0], *this_cart;
	const char  *pcb_name;

	/* First, determine where this cart has to be loaded */
	while (mt_cart->tag)
	{
		if (strcmp(mt_cart->tag, image.device().tag()) == 0)
			break;

		mt_cart++;
	}

	this_cart = mt_cart;

	if (image.software_entry() == NULL)
		return IMAGE_INIT_FAIL;

	UINT8 *ROM = image.device().memregion(this_cart->region)->base();
	UINT32 length = image.get_software_region_length("rom");
	memcpy(ROM, image.get_software_region("rom"), length);


	if ((pcb_name = image.get_feature("pcb_type")) == NULL)
		return IMAGE_INIT_FAIL;
	else
	{
		if (!core_stricmp("genesis", pcb_name))
		{
			mame_printf_debug("%s is genesis\n", mt_cart->tag);
			m_cart_is_genesis[this_cart->slot] = 1;
		}
		else if (!core_stricmp("sms", pcb_name))
		{
			mame_printf_debug("%s is sms\n", mt_cart->tag);
			m_cart_is_genesis[this_cart->slot] = 0;
		}
		else
		{
			mame_printf_debug("%s is invalid\n", mt_cart->tag);
		}

	}

	return IMAGE_INIT_PASS;
}

#define MCFG_MEGATECH_CARTSLOT_ADD(_tag) \
	MCFG_CARTSLOT_ADD(_tag) \
	MCFG_CARTSLOT_INTERFACE("megatech_cart") \
	MCFG_CARTSLOT_LOAD(mtech_state, megatech_cart)

MACHINE_CONFIG_FRAGMENT( megatech_cartslot )
	MCFG_MEGATECH_CARTSLOT_ADD("cart1")
	MCFG_MEGATECH_CARTSLOT_ADD("cart2")
	MCFG_MEGATECH_CARTSLOT_ADD("cart3")
	MCFG_MEGATECH_CARTSLOT_ADD("cart4")
	MCFG_MEGATECH_CARTSLOT_ADD("cart5")
	MCFG_MEGATECH_CARTSLOT_ADD("cart6")
	MCFG_MEGATECH_CARTSLOT_ADD("cart7")
	MCFG_MEGATECH_CARTSLOT_ADD("cart8")

	MCFG_SOFTWARE_LIST_ADD("cart_list","megatech")
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( megatech_slot, megatech )
	MCFG_FRAGMENT_ADD( megatech_cartslot )
MACHINE_CONFIG_END


/* MegaTech Games - Genesis & sms! Games with a timer */

#define MEGATECH_BIOS \
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASEFF ) \
	ROM_REGION( 0x10000, "mtbios", 0 ) \
	ROM_SYSTEM_BIOS( 0, "ver1", "Ver 1" ) \
	ROMX_LOAD( "epr-12664.20",  0x000000, 0x8000, CRC(f71e9526) SHA1(1c7887541d02c41426992d17f8e3db9e03975953), ROM_BIOS(1)) \
	ROM_SYSTEM_BIOS( 1, "ver0a", "Ver 0 Rev A" ) \
	ROMX_LOAD( "epr-12263a.20", 0x000000, 0x8000, CRC(07c3f423) SHA1(50c28bbc2d4349c820d988ae3f20aae3f808545f), ROM_BIOS(2)) \
	ROM_SYSTEM_BIOS( 2, "ver0b", "Ver 0 Rev B" ) \
	ROMX_LOAD( "epr-12263b.20", 0x000000, 0x8000, CRC(ca26c87a) SHA1(987a18bede6e54cd73c4434426eb6c302a37cdc5), ROM_BIOS(3))
/* no games */
ROM_START( megatech )
	MEGATECH_BIOS

	// empty memory areas, to copy data into
	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game1", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game2", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game3", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game4", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game5", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game6", ROMREGION_ERASE00 )
	ROM_REGION16_BE( 0x400000, "game7", ROMREGION_ERASE00 )
ROM_END


/* Game 01 - Altered Beast (Genesis) */
ROM_START( mt_beast ) /* Altered Beast */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12538.ic1", 0x000000, 0x080000, CRC(3bea3dce) SHA1(ec72e4fde191dedeb3f148f132603ed3c23f0f86) )
	ROM_LOAD16_BYTE( "epr-12368-01.ic2", 0x300001, 0x08000, CRC(40cb0088) SHA1(e1711532c29f395a35a1cb34d789015881b5a1ed) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 02 - Space Harrier 2 */
ROM_START( mt_shar2 ) /* Space Harrier 2 */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp11934.ic1", 0x000000, 0x080000, CRC(932daa09) SHA1(a2d7a76f3604c6227d43229908bfbd02b0ef5fd9) )
	ROM_LOAD16_BYTE( "epr-12368-02.ic2", 0x300001, 0x08000, CRC(c129c66c) SHA1(e7c0c97db9df9eb04e2f9ff561b64305219b8f1f) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 03 - Super Thunder Blade */
ROM_START( mt_stbld ) /* Super Thunder Blade */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp11996f.ic1", 0x000000, 0x080000,  CRC(9355c34e) SHA1(26ff91c2921408673c644b0b1c8931d98524bf63) )
	ROM_LOAD16_BYTE( "epr-12368-03.ic2", 0x300001, 0x08000,  CRC(1ba4ac5d) SHA1(9bde57d70189d159ebdc537a9026001abfd0deae) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 04 - Great Golf (SMS) */
/* Also known to have the ID# MPR-11128 instead of MPR-11129F, same contents */
ROM_START( mt_ggolf ) /* Great Golf */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp11129f.ic1", 0x000000, 0x020000, CRC(c6611c84) SHA1(eab0eed872dd26b13bcf0b2dd74fcbbc078812c9) )
	ROM_LOAD16_BYTE( "epr-12368-04.ic2", 0x300001, 0x08000, CRC(62e5579b) SHA1(e1f531be5c40a1216d4192baeda9352384444410) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 05 - Great Soccer (SMS) - bad dump */
ROM_START( mt_gsocr ) /* Great Soccer */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp10747f.ic1", 0x000000, 0x020000, BAD_DUMP CRC(9cf53703) SHA1(c6b4d1de56bd5bf067ec7fc80449c07686d01337) )
	ROM_LOAD16_BYTE( "epr-12368-05.ic2", 0x300001, 0x08000, CRC(bab91fcc) SHA1(a160c9d34b253e93ac54fdcef33f95f44d8fa90c) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 06 - Out Run (SMS) */
ROM_START( mt_orun ) /* Out Run */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-11078.ic1", 0x000000, 0x040000, CRC(5589d8d2) SHA1(4f9b61b24f0d9fee0448cdbbe8fc05411dbb1102) )
	ROM_LOAD16_BYTE( "epr-12368-06.ic2", 0x300001, 0x08000, CRC(c7c74429) SHA1(22ee261a653e10d66e0d6703c988bb7f236a7571) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 07 - Alien Syndrome (SMS) */
ROM_START( mt_asyn ) /* Alien Syndrome */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-11194.ic1", 0x000000, 0x040000, CRC(4cc11df9) SHA1(5d786476b275de34efb95f576dd556cf4b335a83) )
	ROM_LOAD16_BYTE( "epr-12368-07.ic2", 0x300001, 0x08000, CRC(14f4a17b) SHA1(0fc010ac95762534892f1ae16986dbf1c25399d3) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 08 - Shinobi (SMS) */
ROM_START( mt_shnbi ) /* Shinobi */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp11706.ic1", 0x000000, 0x040000, CRC(0C6FAC4E) SHA1(7C0778C055DC9C2B0AAE1D166DBDB4734E55B9D1) )
	ROM_LOAD16_BYTE( "epr-12368-08.ic2", 0x300001, 0x08000, CRC(103A0459) SHA1(D803DDF7926B83785E8503C985B8C78E7CCB5DAC) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 09 - Fantasy Zone (SMS) */
// note, dump was bad, but the good (uniquely identifiable) parts matched the 'fantasy zone (world) (v1.2).bin' SMS rom
// so I'm using that until it gets verified.
ROM_START( mt_fz ) /* Fantasy Zone */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-10118.ic1", 0x000000, 0x020000, CRC(65d7e4e0) SHA1(0278cd120dc3a7707eda9314c46c7f27f9e8fdda) )
	ROM_LOAD16_BYTE( "epr-12368-09.bin", 0x300001, 0x08000, CRC(373d2a70) SHA1(c39dd1003d71a417b12a359126bfef64c7a2fd00) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END



/* Game 10 - Afterburner (SMS) */
ROM_START( mt_aftrb ) /* Afterburner */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp11271.ic1", 0x000000, 0x080000, CRC(1C951F8E) SHA1(51531DF038783C84640A0CAB93122E0B59E3B69A) )
	ROM_LOAD16_BYTE( "epr-12368-10.ic2", 0x300001, 0x08000, CRC(2A7CB590) SHA1(2236963BDDC89CA9045B530259CC7B5CCF889EAF) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 11 - Thunder Force II */
ROM_START( mt_tfor2 ) /* Thunder Force II */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12559.ic1", 0x000000, 0x080000, CRC(b093bee3) SHA1(0bf6194c3d228425f8cf1903ed70d8da1b027b6a) )
	ROM_LOAD16_BYTE( "epr-12368-11.ic2", 0x300001, 0x08000, CRC(f4f27e8d) SHA1(ae1a2823deb416c53838115966f1833d5dac72d4) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 13 - Astro Warrior (SMS) */
ROM_START( mt_astro ) /* Astro Warrior */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "ep13817.ic2", 0x000000, 0x20000, CRC(299cbb74) SHA1(901697a3535ad70190647f34ad5b30b695d54542) )
	ROM_LOAD16_BYTE( "epr-12368-13.ic1", 0x300001, 0x08000,  CRC(4038cbd1) SHA1(696bc1efce45d9f0052b2cf0332a232687c8d6ab) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 19 - Great Football (SMS) */
ROM_START( mt_gfoot ) /* Great Football */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-10576f.ic1", 0x000000, 0x020000, CRC(2055825f) SHA1(a768f44ce7e50083ffe8c4b5e3ac93ceb7bd3266) )
	ROM_LOAD16_BYTE( "epr-12368-19.ic2", 0x300001, 0x08000, CRC(e27cb37a) SHA1(2b6259957e86d033a5689fd716a9efcfeff7d5ba) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 20 - Last Battle */
ROM_START( mt_lastb ) /* Last Battle */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12578f.ic1", 0x000000, 0x080000, CRC(531191a0) SHA1(f6bc26e975c01a3e10ab4033e4c5f494627a1e2f) )
	ROM_LOAD16_BYTE( "epr-12368-20.ic2", 0x300001, 0x08000, CRC(e1a71c91) SHA1(c250da18660d8aea86eb2abace41ba46130dabc8) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 21 - World Championship Soccer (Genesis) */
ROM_START( mt_wcsoc ) /* World Championship Soccer */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12607b.ic1", 0x000000, 0x080000, CRC(bc591b30) SHA1(55e8577171c0933eee53af1dabd0f4c6462d5fc8) )
	ROM_LOAD16_BYTE( "epr-12368-21.ic2", 0x300001, 0x08000, CRC(028ee46b) SHA1(cd8f81d66e5ae62107eb20e0ca5db4b66d4b2987) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 22 - Tetris */
ROM_START( mt_tetri ) /* Tetris */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "mpr-12356f.ic1", 0x000001, 0x020000, CRC(1e71c1a0) SHA1(44b2312792e49d46d71e0417a7f022e5ffddbbfe) )
	ROM_LOAD16_BYTE( "mpr-12357f.ic2", 0x000000, 0x020000, CRC(d52ca49c) SHA1(a9159892eee2c0cf28ebfcfa99f81f80781851c6) )
	ROM_LOAD16_BYTE( "epr-12368-22.ic3", 0x300001, 0x08000, CRC(1c1b6468) SHA1(568a38f4186167486e39ab4aa2c1ceffd0b81156) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 23 - Ghouls and Ghosts (Genesis) */
ROM_START( mt_gng ) /* Ghouls and Ghosts */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12605.ic1", 0x000000, 0x020000, CRC(1066C6AB) SHA1(C30E4442732BDB38C96D780542F8550A94D127B0) )
	ROM_LOAD16_WORD_SWAP( "mpr12606.ic2", 0x080000, 0x020000, CRC(D0BE7777) SHA1(A44B2A3D427F6973B5C1A3DCD8D1776366ACB9F7) )
	ROM_CONTINUE(0x020000,0x60000)
	ROM_LOAD16_BYTE( "epr-12368-23.ic3", 0x300001, 0x08000, CRC(7ee58546) SHA1(ad5bb0934475eacdc5e354f67c96fe0d2512d33b) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 24 - Super Hang-On (Genesis) */
ROM_START( mt_shang ) /* Super Hang-On */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-12640.ic1", 0x000000, 0x080000, CRC(2fe2cf62) SHA1(4728bcc847deb38b16338cbd0154837cd4a07b7d) )
	ROM_LOAD16_BYTE( "epr-12368-24.ic2", 0x300001, 0x08000, CRC(6c2db7e3) SHA1(8de0a10ed9185c9e98f17784811a79d3ce8c4c03) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 25 - Golden Axe (Genesis) */
ROM_START( mt_gaxe ) /* Golden Axe */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "12806.ic1", 0x000000, 0x080000, CRC(43456820) SHA1(2f7f1fcd979969ac99426f11ab99999a5494a121) )
	ROM_LOAD16_BYTE( "epr-12368-25.ic2", 0x300001, 0x08000, CRC(1f07ed28) SHA1(9d54192f4c6c1f8a51c38a835c1dd1e4e3e8279e) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 26 - Forgotten Worlds */
/* why is this pre-swapped like a console dump?? */
ROM_START( mt_fwrld ) /* Forgotten Worlds */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD( "mpr-12672-h.ic1", 0x000000, 0x080000, CRC(d0ee6434) SHA1(8b9a37c206c332ef23dc71f09ec40e1a92b1f83a) )
	ROM_LOAD16_BYTE( "epr-12368-26.ic2", 0x300001, 0x08000, CRC(4623b573) SHA1(29df4a5c5de66cd9cb7519e4f30000f7dddc2138) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 27 - Mystic Defender */
ROM_START( mt_mystd ) /* Mystic Defender */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12707.1", 0x000000, 0x080000, CRC(4f2c513d) SHA1(f9bb548b3688170fe18bb3f1b5b54182354143cf) )
	ROM_LOAD16_BYTE( "epr-12368-27.ic2", 0x300001, 0x08000, CRC(caf46f78) SHA1(a9659e86a6a223646338cd8f29c346866e4406c7) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 28 - The Revenge of Shinobi */
ROM_START( mt_revsh ) /* The Revenge Of Shinobi */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12675.ic1", 0x000000, 0x080000, CRC(672A1D4D) SHA1(5FD0AF14C8F2CF8CEAB1AE61A5A19276D861289A) )
	ROM_LOAD16_BYTE( "epr-12368-28.ic2", 0x300001, 0x08000, CRC(0D30BEDE) SHA1(73A090D84B78A570E02FB54A33666DCADA52849B) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 29 - Parlour Games (SMS) */
ROM_START( mt_parlg ) /* Parlour Games */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp11404.ic1", 0x000000, 0x020000, CRC(E030E66C) SHA1(06664DAF208F07CB00B603B12ECCFC3F01213A17) )
	ROM_LOAD16_BYTE( "epr-12368-29.ic2", 0x300001, 0x08000, CRC(534151e8) SHA1(219238d90c1d3ac07ff64c9a2098b490fff68f04) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASEFF )
ROM_END


/* Game 31 - Arnold Palmer Tournament Gold */
ROM_START( mt_tgolf ) /* Arnold Palmer Tournament Golf */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12645f.ic1", 0x000000, 0x080000, CRC(c07ef8d2) SHA1(9d111fdc7bb92d52bfa048cd134aa488b4f475ef) )
	ROM_LOAD16_BYTE( "epr-12368-31.ic2", 0x300001, 0x08000, CRC(30af7e4a) SHA1(baf91d527393dc90aba9371abcb1e690bcc83c7e) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 32 - Super Real Basketball */
/* why is this pre-swapped like a console dump?? */
ROM_START( mt_srbb ) /* Super Real Basketball */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD( "mpr-12904f.ic1", 0x000000, 0x080000, CRC(4346e11a) SHA1(c86725780027ef9783cb7884c8770cc030b0cd0d) )
	ROM_LOAD16_BYTE( "epr-12368-32.ic2", 0x300001, 0x08000, CRC(f70adcbe) SHA1(d4412a7cd59fe282a1c6619aa1051a2a2e00e1aa) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 35 - Tommy Lasorda Baseball */
ROM_START( mt_tlbba ) /* Tommy Lasorda Baseball */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp12706.ic1", 0x000000, 0x080000, CRC(8901214f) SHA1(f5ec166be1cf9b86623b9d7a78ec903b899da32a) )
	ROM_LOAD16_BYTE( "epr-12368-35.ic2", 0x300001, 0x08000, CRC(67bbe482) SHA1(6fc283b22e68befabb44b2cc61a7f82a71d6f029) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 36 - Columns */
ROM_START( mt_cols ) /* Columns */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13193-t.ic1", 0x000000, 0x080000, CRC(8c770e2f) SHA1(02a3626025c511250a3f8fb3176eebccc646cda9) )
	ROM_LOAD16_BYTE( "epr-12368-36.ic3",   0x300001, 0x008000,  CRC(a4b29bac) SHA1(c9be866ac96243897d09612fe17562e0481f66e3) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 38 - ESWAT */
ROM_START( mt_eswat ) /* ESWAT */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13192-h.ic1", 0x000000, 0x080000, CRC(82f458ef) SHA1(58444b783312def71ecffc4ad021b72a609685cb) )
	ROM_LOAD16_BYTE( "epr-12368-38.ic2", 0x300001, 0x08000, CRC(43c5529b) SHA1(104f85adea6da1612c0aa96d553efcaa387d7aaf) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 39 - Super Monaco Grand Prix (Genesis) */
ROM_START( mt_smgp ) /* Super Monaco Grand Prix */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "13250.ic1", 0x000000, 0x080000, CRC(189b885f) SHA1(31c06ffcb48b1604989a94e584261457de4f1f46) )
	ROM_LOAD16_BYTE( "epr-12368-39.ic2", 0x300001, 0x08000, CRC(64b3ce25) SHA1(83a9f2432d146a712b037f96f261742f7dc810bb) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 40 - Moon Walker */
ROM_START( mt_mwalk ) /* Moon Walker */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13285a.ic1", 0x000000, 0x080000, CRC(189516e4) SHA1(2a79e07da2e831832b8d448cae87a833c85e67c9) )
	ROM_LOAD16_BYTE( "epr-12368-40.ic2", 0x300001, 0x08000, CRC(0482378c) SHA1(734772f3ddb5ff82b76c3514d18a464b2bce8381) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 41 - Crackdown */
ROM_START( mt_crack ) /* Crackdown */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13578a-s.ic1", 0x000000, 0x080000, CRC(23f19893) SHA1(09aca793871e2246af4dc24925bc1eda8ff34446) )
	ROM_LOAD16_BYTE( "epr-12368-41.ic2", 0x300001, 0x08000, CRC(3014acec) SHA1(07953e9ae5c23fc7e7d08993b215f4dfa88aa5d7) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 43 - Shadow Dancer */
ROM_START( mt_shado ) /* Shadow Dancer */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-13571-s.ic1", 0x000000, 0x080000, CRC(56a29310) SHA1(55836177e4a1e2deb68408976b29d0282cf661a9) )
	ROM_LOAD16_BYTE( "epr-12368-43.ic2", 0x300001, 0x08000, CRC(1116cbc7) SHA1(ba6dd21ceadeedf730b71b67acbd20d9067114f3) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 44 - Arrow Flash */
ROM_START( mt_arrow ) /* Arrow Flash */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr13396h.ic1", 0x000000, 0x080000, CRC(091226e3) SHA1(cb15c6277314f3c4a86b5ae5823f72811d5d269d) )
	ROM_LOAD16_BYTE( "epr-12368-44.ic2", 0x300001, 0x08000, CRC(e653065d) SHA1(96b014fc4df8eb2188ac94ed0a778d974fe6dcad) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 47 - Alien Storm */
ROM_START( mt_astrm ) /* Alien Storm */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13941.ic1", 0x000000, 0x080000, CRC(D71B3EE6) SHA1(05F272DAD243D132D517C303388248DC4C0482ED) )
	ROM_LOAD16_BYTE( "epr-12368-47.ic2", 0x300001, 0x08000, CRC(31FB683D) SHA1(E356DA020BBF817B97FB10C27F75CF5931EDF4FC) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 48 - Wrestle War */
ROM_START( mt_wwar ) /* Wrestle War */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-14025-f.ic1", 0x000000, 0x080000, CRC(26e899fe) SHA1(6d28e154ae2e4196097a2aa96c5acd5dfe7e3d2b) )
	ROM_LOAD16_BYTE( "epr-12368-48.ic2", 0x300001, 0x08000, CRC(25817bc2) SHA1(ba1bbb952aff12fb4d3ecfb10d82c54128439395) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 49 - Bonanza Bros. */
ROM_START( mt_bbros ) /* Bonanza Bros. */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13905a.ic1", 0x000000, 0x100000, CRC(68a88d60) SHA1(2f56e8a2b0999de4fa0d14a1527f4e1df0f9c7a2) )
	ROM_LOAD16_BYTE( "epr-12368-49.ic2", 0x300001, 0x08000, CRC(c5101da2) SHA1(636f30043e2e9291e193ef9a2ead2e97a0bf7380) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 51 - Streets of Rage */
ROM_START( mt_srage ) /* Streets of Rage */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-14125-s.ic1", 0x000000, 0x080000, CRC(db4ac746) SHA1(c7cc24e2329f279574513fa32bbf79f72f75aeea) )
	ROM_LOAD16_BYTE( "epr-12368-51.ic2", 0x300001, 0x08000, CRC(49b7d6f4) SHA1(96e69851c92715e7daf35b184cf374147a8d2880) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 52 - Sonic The Hedgehog (Genesis) */
ROM_START( mt_sonic ) /* Sonic The Hedgehog */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13913.ic1", 0x000000, 0x080000, CRC(480b4b5c) SHA1(ab1dc1f738e3b2d0898a314b123fa71182bf572e) )
	ROM_LOAD16_BYTE( "epr-12368-52.ic2", 0x300001, 0x8000,  CRC(6a69d20c) SHA1(e483b39ff6eca37dc192dc296d004049e220554a) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


ROM_START( mt_sonia ) /* Sonic (alt)*/
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp13933.ic1", 0x000000, 0x080000, CRC(13775004) SHA1(5decfd35944a2d0e7b996b9a4a12b616a309fd5e) )
	ROM_LOAD16_BYTE( "epr-12368-52.ic2", 0x300001, 0x8000,  CRC(6a69d20c) SHA1(e483b39ff6eca37dc192dc296d004049e220554a) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 53 - Fire Shark */
	/* alt version with these roms exists, but the content is the same */
	/* (6a221fd6) ep14706.ic1             mp14341.ic1  [even]     IDENTICAL */
	/* (09fa48af) ep14707.ic2             mp14341.ic1  [odd]      IDENTICAL */

ROM_START( mt_fshrk ) /* Fire Shark */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp14341.ic1", 0x000000, 0x080000, CRC(04d65ebc) SHA1(24338aecdc52b6f416548be722ca475c83dbae96) )
	ROM_LOAD16_BYTE( "epr-12368-53.ic2", 0x300001, 0x08000,  CRC(4fa61044) SHA1(7810deea221c10b0b2f5233443d81f4f1998ee58) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 54 - Spiderman */
ROM_START( mt_spman ) /* Spiderman */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp14027-sm.ic1", 0x000000, 0x080000, CRC(e2c08a06) SHA1(39e592eafd47e2aa6edbb4845d44750057bff890) )
	ROM_LOAD16_BYTE( "epr-12368-54.ic2", 0x300001, 0x08000,  CRC(30b68988) SHA1(04eeb0fad732a791b6bc0c0846306d567573649f) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 55 - California Games */
ROM_START( mt_calga ) /* California Games */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "epr-14494.ic1", 0x000001, 0x040000, CRC(cbe58b1b) SHA1(ea067fc08e644c993f8d13731425c9296c1a2a75) )
	ROM_LOAD16_BYTE( "epr-14495.ic2", 0x000000, 0x040000, CRC(cb956f4f) SHA1(3574c496b79aefdec7d02975490ebe3bb373bc60) )
	ROM_LOAD16_BYTE( "epr-12368-55.ic3", 0x300001, 0x08000, CRC(6f7dd8f5) SHA1(a6cb1aa8c3635738dd9e4d3e0d729d089fd9b599) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 57 - Golden Axe 2 (Genesis) */
ROM_START( mt_gaxe2 ) /* Golden Axe 2 */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp14272.ic1", 0x000000, 0x080000, CRC(d4784cae) SHA1(b6c286027d06fd850016a2a1ee1f1aeea080c3bb) )
	ROM_LOAD16_BYTE( "epr-12368-57.ic2", 0x300001, 0x08000, CRC(dc9b4433) SHA1(efd3a598569010cdc4bf38ecbf9ed1b4e14ffe36) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 58 - Sports Talk Football */
ROM_START( mt_stf ) /* Sports Talk Football */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp14356a-f.ic1", 0x000000, 0x100000, CRC(20cf32f6) SHA1(752314346a7a98b3808b3814609e024dc0a4108c) )
	ROM_LOAD16_BYTE( "epr-12368-58.ic2", 0x300001, 0x08000, CRC(dce2708e) SHA1(fcebb1899ee11468f6bda705899f074e7de9d723) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 59 - Mario Lemieux Hockey */
ROM_START( mt_mlh ) /* Mario Lemieux Hockey */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mpr-14376-h.ic1", 0x000000, 0x80000, CRC(aa9be87e) SHA1(dceed94eaeb30e534f6953a4bc25ff37673b1e6b) )
	ROM_LOAD16_BYTE( "epr-12368-59.ic2", 0x300001, 0x08000, CRC(6d47b438) SHA1(0a145f6438e4e55c957ae559663c37662b685246) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 60 - Kid Chameleon */
ROM_START( mt_kcham ) /* Kid Chameleon */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp14557.ic1", 0x000000, 0x100000, CRC(e1a889a4) SHA1(a2768eacafc47d371e5276f0cce4b12b6041337a) )
	ROM_LOAD16_BYTE( "epr-12368-60.ic2", 0x300001, 0x08000, CRC(a8e4af18) SHA1(dfa49f6ec4047718f33dba1180f6204dbaff884c) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 61 - Turbo Outrun */
// original dump of epr-14674.ic1 had CRC(c2b9a802) SHA1(108cc844c944125f9d271a2f2db094301294e8c2)
// with the byte at offset 3 being F6 instead of Fe, this seems like a bad dump when compared to the Genesis rom which
// has been verified on multiple carts, chances are the ROM had developed a fault.
ROM_START( mt_tout ) /* Turbo Outrun */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "epr-14674.ic1", 0x000000, 0x080000, CRC(453712a2) SHA1(5d2c8430a9a14aac7f19c22617539b0503ab92cd) )
	ROM_LOAD16_BYTE( "epr-12368-61.ic2", 0x300001, 0x08000, CRC(4aa0b2a2) SHA1(bce03f88d6cfd02683d51c28058f6229fda13b49) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END


/* Game 62 - Sonic The Hedgehog 2 */
ROM_START( mt_soni2 ) /* Sonic The Hedgehog 2 */
	MEGATECH_BIOS

	ROM_REGION16_BE( 0x400000, "game0", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mp15000a-f.ic1", 0x000000, 0x100000, CRC(679ebb49) SHA1(557482064677702454562f753460993067ef9e16) )
	ROM_LOAD16_BYTE( "epr-12368-62.ic2", 0x300001, 0x08000, CRC(14a8566f) SHA1(d1d14162144bf068ddd19e9736477ff98fb43f9e) )

	ROM_REGION( 0x01, "sms_pin", ROMREGION_ERASE00 )
ROM_END



/* nn */ /* nn is part of the instruction rom name, should there be a game for each number? */
/* -- */ CONS( 1989, megatech, 0, 0,     megatech_slot, megatech, mtech_state, mt_slot, "Sega",                  "Mega-Tech", GAME_IS_BIOS_ROOT )
/* 01 */ GAME( 1988, mt_beast, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Altered Beast (Mega-Tech)", GAME_NOT_WORKING )
/* 02 */ GAME( 1988, mt_shar2, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Space Harrier II (Mega-Tech)", GAME_NOT_WORKING )
/* 03 */ GAME( 1988, mt_stbld, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Super Thunder Blade (Mega-Tech)", GAME_NOT_WORKING )
/* 04 */ GAME( 1987, mt_ggolf, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Great Golf (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! */
/* 05 */ GAME( 198?, mt_gsocr, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Great Soccer (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! also bad */
/* 06 */ GAME( 1987, mt_orun,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Out Run (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! */
/* 07 */ GAME( 1987, mt_asyn,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Alien Syndrome (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! */
/* 08 */ GAME( 1987, mt_shnbi, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Shinobi (Mega-Tech, SMS based)", GAME_NOT_WORKING) /* sms */
/* 09 */ GAME( 1987, mt_fz,    megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Fantasy Zone (Mega-Tech, SMS based)", GAME_NOT_WORKING) /* sms */
/* 10 */ GAME( 1987, mt_aftrb, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "After Burner (Mega-Tech, SMS based)", GAME_NOT_WORKING) /* sms */
/* 11 */ GAME( 1989, mt_tfor2, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Techno Soft / Sega",    "Thunder Force II MD (Mega-Tech)", GAME_NOT_WORKING )
/* 12 */ // unknown
/* 13 */ GAME( 1986, mt_astro, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Astro Warrior (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! */
/* 14 */ // unknown
/* 15 */ // unknown
/* 16 */ // unknown
/* 17 */ // unknown
/* 18 */ // Kung Fu Kid (sms)
/* 19 */ GAME( 1987, mt_gfoot, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Great Football (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! */
/* 20 */ GAME( 1989, mt_lastb, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Last Battle (Mega-Tech)", GAME_NOT_WORKING )
/* 21 */ GAME( 1989, mt_wcsoc, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "World Championship Soccer (Mega-Tech)", GAME_NOT_WORKING )
/* 22 */ GAME( 1989, mt_tetri, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Tetris (Mega-Tech)", GAME_NOT_WORKING )
/* 23 */ GAME( 1989, mt_gng,   megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Capcom / Sega",         "Ghouls'n Ghosts (Mega-Tech)", GAME_NOT_WORKING )
/* 24 */ GAME( 1989, mt_shang, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Super Hang-On (Mega-Tech)", GAME_NOT_WORKING )
/* 25 */ GAME( 1989, mt_gaxe,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Golden Axe (Mega-Tech)", GAME_NOT_WORKING )
/* 26 */ GAME( 1989, mt_fwrld, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Forgotten Worlds (Mega-Tech)", GAME_NOT_WORKING )
/* 27 */ GAME( 1989, mt_mystd, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Mystic Defender (Mega-Tech)", GAME_NOT_WORKING )
/* 28 */ GAME( 1989, mt_revsh, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "The Revenge of Shinobi (Mega-Tech)", GAME_NOT_WORKING )
/* 29 */ GAME( 1987, mt_parlg, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Parlour Games (Mega-Tech, SMS based)", GAME_NOT_WORKING ) /* sms! */
/* 30 */ // unknown
/* 31 */ GAME( 1989, mt_tgolf, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Arnold Palmer Tournament Golf (Mega-Tech)", GAME_NOT_WORKING )
/* 32 */ GAME( 1989, mt_srbb,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Super Real Basketball (Mega-Tech)", GAME_NOT_WORKING )
/* 33 */ // unknown
/* 34 */ // unknown
/* 35 */ GAME( 1989, mt_tlbba, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Tommy Lasorda Baseball (Mega-Tech)", GAME_NOT_WORKING )
/* 36 */ GAME( 1990, mt_cols,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Columns (Mega-Tech)", GAME_NOT_WORKING )
/* 37 */ // unknown
/* 38 */ GAME( 1990, mt_eswat, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Cyber Police ESWAT: Enhanced Special Weapons and Tactics (Mega-Tech)", GAME_NOT_WORKING )
/* 39 */ GAME( 1990, mt_smgp,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Super Monaco GP (Mega-Tech)", GAME_NOT_WORKING )
/* 40 */ GAME( 1990, mt_mwalk, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Michael Jackson's Moonwalker (Mega-Tech)", GAME_NOT_WORKING )
/* 41 */ GAME( 1990, mt_crack, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Crack Down (Mega-Tech)", GAME_NOT_WORKING )
/* 42 */ // unknown
/* 43 */ GAME( 1990, mt_shado, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Shadow Dancer (Mega-Tech)", GAME_NOT_WORKING )
/* 44 */ GAME( 1990, mt_arrow, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Arrow Flash (Mega-Tech)", GAME_NOT_WORKING )
/* 45 */ // unknown
/* 46 */ // unknown
/* 47 */ GAME( 1990, mt_astrm, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Alien Storm (Mega-Tech)", GAME_NOT_WORKING )
/* 48 */ GAME( 1991, mt_wwar,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Wrestle War (Mega-Tech)", GAME_NOT_WORKING ) /* Copyright 1989, 1991 Sega */
/* 49 */ GAME( 1991, mt_bbros, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Bonanza Bros. (Mega-Tech)", GAME_NOT_WORKING )
/* 50 */ // unknown
/* 51 */ GAME( 1991, mt_srage, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Streets of Rage (Mega-Tech)", GAME_NOT_WORKING )
/* 52 */ GAME( 1991, mt_sonic, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Sonic The Hedgehog (Mega-Tech, set 1)", GAME_NOT_WORKING )
/*    */ GAME( 1991, mt_sonia, mt_sonic, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Sonic The Hedgehog (Mega-Tech, set 2)", GAME_NOT_WORKING )
/* 53 */ GAME( 1990, mt_fshrk, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Toaplan / Sega",        "Fire Shark (Mega-Tech)", GAME_NOT_WORKING )
/* 54 */ GAME( 1991, mt_spman, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Marvel / Sega",         "Spider-Man vs The Kingpin (Mega-Tech)", GAME_NOT_WORKING )
/* 55 */ GAME( 1991, mt_calga, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "California Games (Mega-Tech)", GAME_NOT_WORKING )
/* 56 */ // unknown
/* 57 */ GAME( 1991, mt_gaxe2, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Golden Axe II (Mega-Tech)", GAME_NOT_WORKING )
/* 58 */ GAME( 1991, mt_stf,   megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Joe Montana II: Sports Talk Football (Mega-Tech)", GAME_NOT_WORKING )
/* 59 */ GAME( 1991, mt_mlh,   megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Mario Lemieux Hockey (Mega-Tech)", GAME_NOT_WORKING )
/* 60 */ GAME( 1992, mt_kcham, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Kid Chameleon (Mega-Tech)", GAME_NOT_WORKING )
/* 61 */ GAME( 1992, mt_tout,  megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Turbo Outrun (Mega-Tech)", GAME_NOT_WORKING )
/* 62 */ GAME( 1992, mt_soni2, megatech, megatech, megatech, mtech_state, mt_crt, ROT0, "Sega",                  "Sonic The Hedgehog 2 (Mega-Tech)", GAME_NOT_WORKING )

/* more? */
