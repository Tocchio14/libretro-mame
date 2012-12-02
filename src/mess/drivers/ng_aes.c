/***************************************************************************

    Neo-Geo AES hardware

    Credits (from MAME neogeo.c, since this is just a minor edit of that driver):
        * This driver was made possible by the research done by
          Charles MacDonald.  For a detailed description of the Neo-Geo
          hardware, please visit his page at:
          http://cgfm2.emuviews.com/temp/mvstech.txt
        * Presented to you by the Shin Emu Keikaku team.
        * The following people have all spent probably far
          too much time on this:
          AVDB
          Bryan McPhail
          Fuzz
          Ernesto Corvi
          Andrew Prime
          Zsolt Vasvari

    MESS cartridge support by R. Belmont based on work by Michael Zapf

    Current status:
        - Cartridges run.

    ToDo :
        - Change input code to allow selection of the mahjong panel in PORT_CATEGORY.
        - Clean up code, to reduce duplication of MAME source

****************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "includes/neogeo.h"
#include "machine/pd4990a.h"
#include "cpu/z80/z80.h"
#include "sound/2610intf.h"
#include "imagedev/cartslot.h"
#include "imagedev/chd_cd.h"
#include "sound/cdda.h"
#include "machine/megacdcd.h"


extern const char layout_neogeo[];

static const char *audio_banks[4] =
{
    NEOGEO_BANK_AUDIO_CPU_CART_BANK0, NEOGEO_BANK_AUDIO_CPU_CART_BANK1, NEOGEO_BANK_AUDIO_CPU_CART_BANK2, NEOGEO_BANK_AUDIO_CPU_CART_BANK3
};




static IRQ_CALLBACK(neocd_int_callback);

/* Stubs for various functions called by the FBA code, replace with MAME specifics later */

UINT8 *NeoSpriteRAM, *NeoTextRAM;
//UINT8* NeoSpriteROM;
//UINT8* NeoTextROM;
UINT8* YM2610ADPCMAROM;
UINT8* NeoZ80ROMActive;
UINT8 NeoSystem = 6;
INT32 nNeoCDZ80ProgWriteWordCancelHack = 0;







static void MapVectorTable(bool bMapBoardROM)
{
	/*
    if (!bMapBoardROM && Neo68KROMActive) {
        SekMapMemory(Neo68KFix, 0x000000, 0x0003FF, SM_ROM);
    } else {
        SekMapMemory(NeoVectorActive, 0x000000, 0x0003FF, SM_ROM);
    }
    */
}


class ng_aes_state : public neogeo_state
{
public:
	ng_aes_state(const machine_config &mconfig, device_type type, const char *tag)
		: neogeo_state(mconfig, type, tag),
		m_tempcdc(*this,"tempcdc")
	{

	}

	required_device<lc89510_temp_device> m_tempcdc;


	UINT8 *m_memcard_data;
	DECLARE_WRITE8_MEMBER(audio_cpu_clear_nmi_w);
	DECLARE_WRITE16_MEMBER(io_control_w);
	DECLARE_WRITE16_MEMBER(save_ram_w);
	DECLARE_READ16_MEMBER(memcard_r);
	DECLARE_WRITE16_MEMBER(memcard_w);
	DECLARE_READ16_MEMBER(neocd_memcard_r);
	DECLARE_WRITE16_MEMBER(neocd_memcard_w);
	DECLARE_WRITE16_MEMBER(audio_command_w);
	DECLARE_READ8_MEMBER(audio_command_r);
	DECLARE_WRITE8_MEMBER(audio_result_w);
	DECLARE_WRITE16_MEMBER(main_cpu_bank_select_w);
	DECLARE_READ8_MEMBER(audio_cpu_bank_select_f000_f7ff_r);
	DECLARE_READ8_MEMBER(audio_cpu_bank_select_e000_efff_r);
	DECLARE_READ8_MEMBER(audio_cpu_bank_select_c000_dfff_r);
	DECLARE_READ8_MEMBER(audio_cpu_bank_select_8000_bfff_r);
	DECLARE_WRITE16_MEMBER(system_control_w);
	DECLARE_READ16_MEMBER(neocd_control_r);
	DECLARE_WRITE16_MEMBER(neocd_control_w);
	DECLARE_READ16_MEMBER(neocd_transfer_r);
	DECLARE_WRITE16_MEMBER(neocd_transfer_w);
	DECLARE_READ16_MEMBER(aes_in0_r);
	DECLARE_READ16_MEMBER(aes_in1_r);
	DECLARE_READ16_MEMBER(aes_in2_r);
	DECLARE_DRIVER_INIT(neogeo);
	DECLARE_MACHINE_START(neocd);
	DECLARE_MACHINE_START(neogeo);
	DECLARE_MACHINE_RESET(neogeo);

	DECLARE_CUSTOM_INPUT_MEMBER(get_memcard_status);

	// neoCD

	UINT16 neogeoReadWordCDROM(UINT32 sekAddress);
	void neogeoWriteWordCDROM(UINT32 sekAddress, UINT16 wordValue);
	UINT8 neogeoReadTransfer(UINT32 sekAddress, int is_byte_transfer);
	void neogeoWriteTransfer(UINT32 sekAddress, UINT8 byteValue, int is_byte_transfer);

	INT32 nActiveTransferArea;
	INT32 nSpriteTransferBank;
	INT32 nADPCMTransferBank;

	UINT8 nTransferWriteEnable;

	address_space* curr_space;


};


#define LOG_VIDEO_SYSTEM		(0)
#define LOG_CPU_COMM			(0)
#define LOG_MAIN_CPU_BANKING	(1)
#define LOG_AUDIO_CPU_BANKING	(1)

#define NEOCD_AREA_SPR    0
#define NEOCD_AREA_PCM    1
#define NEOCD_AREA_AUDIO  4
#define NEOCD_AREA_FIX    5

/*************************************
 *
 *  Global variables
 *
 *************************************/

//static UINT16 *save_ram;

//UINT16* neocd_work_ram;

/*************************************
 *
 *  Forward declarations
 *
 *************************************/

//static void set_output_latch(running_machine &machine, UINT8 data);
//static void set_output_data(running_machine &machine, UINT8 data);



/*************************************
 *
 *  Audio CPU interrupt generation
 *
 *************************************/

static void audio_cpu_irq(device_t *device, int assert)
{
	neogeo_state *state = device->machine().driver_data<neogeo_state>();
	state->m_audiocpu->set_input_line(0, assert ? ASSERT_LINE : CLEAR_LINE);
}


static void audio_cpu_assert_nmi(running_machine &machine)
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	state->m_audiocpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}


WRITE8_MEMBER(ng_aes_state::audio_cpu_clear_nmi_w)
{
	m_audiocpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}



/*************************************
 *
 *  Input ports / Controllers
 *
 *************************************/

static void select_controller( running_machine &machine, UINT8 data )
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	state->m_controller_select = data;
}


WRITE16_MEMBER(ng_aes_state::io_control_w)
{
	switch (offset)
	{
	case 0x00: select_controller(machine(), data & 0x00ff); break;
//  case 0x18: set_output_latch(machine(), data & 0x00ff); break;
//  case 0x20: set_output_data(machine(), data & 0x00ff); break;
	case 0x28: upd4990a_control_16_w(m_upd4990a, space, 0, data, mem_mask); break;
//  case 0x30: break; // coin counters
//  case 0x31: break; // coin counters
//  case 0x32: break; // coin lockout
//  case 0x33: break; // coui lockout

	default:
		logerror("PC: %x  Unmapped I/O control write.  Offset: %x  Data: %x\n", space.device().safe_pc(), offset, data);
		break;
	}
}


static CUSTOM_INPUT( get_calendar_status )
{
	neogeo_state *state = field.machine().driver_data<neogeo_state>();
	return (upd4990a_databit_r(state->m_upd4990a, state->generic_space(), 0) << 1) | upd4990a_testbit_r(state->m_upd4990a, state->generic_space(), 0);
}


/*************************************
 *
 *  Memory card
 *
 *************************************/

#define MEMCARD_SIZE	0x0800

CUSTOM_INPUT_MEMBER(ng_aes_state::get_memcard_status)
{
	/* D0 and D1 are memcard presence indicators, D2 indicates memcard
       write protect status (we are always write enabled) */
	if(strcmp((char*)machine().system().name,"aes") != 0)
		return 0x00;  // On the Neo Geo CD, the memory card is internal and therefore always present.
	else
		return (memcard_present(machine()) == -1) ? 0x07 : 0x00;
}

READ16_MEMBER(ng_aes_state::memcard_r)
{
	UINT16 ret;

	if (memcard_present(machine()) != -1)
		ret = m_memcard_data[offset] | 0xff00;
	else
		ret = 0xffff;

	return ret;
}


WRITE16_MEMBER(ng_aes_state::memcard_w)
{
	if (ACCESSING_BITS_0_7)
	{
		if (memcard_present(machine()) != -1)
			m_memcard_data[offset] = data;
	}
}

/* The NeoCD has an 8kB internal memory card, instead of memcard slots like the MVS and AES */
READ16_MEMBER(ng_aes_state::neocd_memcard_r)
{
	return m_memcard_data[offset] | 0xff00;
}


WRITE16_MEMBER(ng_aes_state::neocd_memcard_w)
{
	if (ACCESSING_BITS_0_7)
	{
		m_memcard_data[offset] = data;
	}
}

static MEMCARD_HANDLER( neogeo )
{
	ng_aes_state *state = machine.driver_data<ng_aes_state>();
	switch (action)
	{
	case MEMCARD_CREATE:
		memset(state->m_memcard_data, 0, MEMCARD_SIZE);
		file.write(state->m_memcard_data, MEMCARD_SIZE);
		break;

	case MEMCARD_INSERT:
		file.read(state->m_memcard_data, MEMCARD_SIZE);
		break;

	case MEMCARD_EJECT:
		file.write(state->m_memcard_data, MEMCARD_SIZE);
		break;
	}
}



/*************************************
 *
 *  Inter-CPU communications
 *
 *************************************/

WRITE16_MEMBER(ng_aes_state::audio_command_w)
{
	/* accessing the LSB only is not mapped */
	if (mem_mask != 0x00ff)
	{
		soundlatch_byte_w(space, 0, data >> 8);

		audio_cpu_assert_nmi(machine());

		/* boost the interleave to let the audio CPU read the command */
		machine().scheduler().boost_interleave(attotime::zero, attotime::from_usec(50));

		if (LOG_CPU_COMM) logerror("MAIN CPU PC %06x: audio_command_w %04x - %04x\n", space.device().safe_pc(), data, mem_mask);
	}
}


READ8_MEMBER(ng_aes_state::audio_command_r)
{
	UINT8 ret = soundlatch_byte_r(space, 0);

	if (LOG_CPU_COMM) logerror(" AUD CPU PC   %04x: audio_command_r %02x\n", space.device().safe_pc(), ret);

	/* this is a guess */
	audio_cpu_clear_nmi_w(space, 0, 0);

	return ret;
}


WRITE8_MEMBER(ng_aes_state::audio_result_w)
{

	if (LOG_CPU_COMM && (m_audio_result != data)) logerror(" AUD CPU PC   %04x: audio_result_w %02x\n", space.device().safe_pc(), data);

	m_audio_result = data;
}


static CUSTOM_INPUT( get_audio_result )
{
	neogeo_state *state = field.machine().driver_data<neogeo_state>();
	UINT32 ret = state->m_audio_result;

//  if (LOG_CPU_COMM) logerror("MAIN CPU PC %06x: audio_result_r %02x\n", field.machine(->safe_pc().device("maincpu")), ret);

	return ret;
}



/*************************************
 *
 *  Main CPU banking
 *
 *************************************/

static void _set_main_cpu_vector_table_source( running_machine &machine )
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	state->membank(NEOGEO_BANK_VECTORS)->set_entry(state->m_main_cpu_vector_table_source);
}


static void set_main_cpu_vector_table_source( running_machine &machine, UINT8 data )
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	state->m_main_cpu_vector_table_source = data;

	_set_main_cpu_vector_table_source(machine);
}


static void _set_main_cpu_bank_address( running_machine &machine )
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	state->membank(NEOGEO_BANK_CARTRIDGE)->set_base(&state->memregion("maincpu")->base()[state->m_main_cpu_bank_address]);
}



WRITE16_MEMBER(ng_aes_state::main_cpu_bank_select_w)
{
	UINT32 bank_address;
	UINT32 len = memregion("maincpu")->bytes();

	if ((len <= 0x100000) && (data & 0x07))
		logerror("PC %06x: warning: bankswitch to %02x but no banks available\n", space.device().safe_pc(), data);
	else
	{
		bank_address = ((data & 0x07) + 1) * 0x100000;

		if (bank_address >= len)
		{
			logerror("PC %06x: warning: bankswitch to empty bank %02x\n", space.device().safe_pc(), data);
			bank_address = 0x100000;
		}

		neogeo_set_main_cpu_bank_address(space, bank_address);
	}
}


static void main_cpu_banking_init( running_machine &machine )
{
	ng_aes_state *state = machine.driver_data<ng_aes_state>();
	address_space &mainspace = machine.device("maincpu")->memory().space(AS_PROGRAM);

	/* create vector banks */
	state->membank(NEOGEO_BANK_VECTORS)->configure_entry(0, machine.root_device().memregion("mainbios")->base());
	state->membank(NEOGEO_BANK_VECTORS)->configure_entry(1, machine.root_device().memregion("maincpu")->base());

	/* set initial main CPU bank */
	if (machine.root_device().memregion("maincpu")->bytes() > 0x100000)
		neogeo_set_main_cpu_bank_address(mainspace, 0x100000);
	else
		neogeo_set_main_cpu_bank_address(mainspace, 0x000000);
}



/*************************************
 *
 *  Audio CPU banking
 *
 *************************************/

static void set_audio_cpu_banking( running_machine &machine )
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	int region;

	for (region = 0; region < 4; region++)
		state->membank(audio_banks[region])->set_entry(state->m_audio_cpu_banks[region]);
}


static void audio_cpu_bank_select( address_space &space, int region, UINT8 bank )
{
	neogeo_state *state = space.machine().driver_data<neogeo_state>();

	if (LOG_AUDIO_CPU_BANKING) logerror("Audio CPU PC %03x: audio_cpu_bank_select: Region: %d   Bank: %02x\n", space.device().safe_pc(), region, bank);

	state->m_audio_cpu_banks[region] = bank;

	set_audio_cpu_banking(space.machine());
}


READ8_MEMBER(ng_aes_state::audio_cpu_bank_select_f000_f7ff_r)
{
	audio_cpu_bank_select(space, 0, offset >> 8);

	return 0;
}


READ8_MEMBER(ng_aes_state::audio_cpu_bank_select_e000_efff_r)
{
	audio_cpu_bank_select(space, 1, offset >> 8);

	return 0;
}


READ8_MEMBER(ng_aes_state::audio_cpu_bank_select_c000_dfff_r)
{
	audio_cpu_bank_select(space, 2, offset >> 8);

	return 0;
}


READ8_MEMBER(ng_aes_state::audio_cpu_bank_select_8000_bfff_r)
{
	audio_cpu_bank_select(space, 3, offset >> 8);

	return 0;
}


static void _set_audio_cpu_rom_source( address_space &space )
{
	neogeo_state *state = space.machine().driver_data<neogeo_state>();

/*  if (!state->memregion("audiobios")->base())   */
		state->m_audio_cpu_rom_source = 1;

	state->membank(NEOGEO_BANK_AUDIO_CPU_MAIN_BANK)->set_entry(state->m_audio_cpu_rom_source);

	/* reset CPU if the source changed -- this is a guess */
	if (state->m_audio_cpu_rom_source != state->m_audio_cpu_rom_source_last)
	{
		state->m_audio_cpu_rom_source_last = state->m_audio_cpu_rom_source;

		space.machine().device("audiocpu")->execute().set_input_line(INPUT_LINE_RESET, PULSE_LINE);

		if (LOG_AUDIO_CPU_BANKING) logerror("Audio CPU PC %03x: selectign %s ROM\n", space.device().safe_pc(), state->m_audio_cpu_rom_source ? "CARTRIDGE" : "BIOS");
	}
}


static void set_audio_cpu_rom_source( address_space &space, UINT8 data )
{
	neogeo_state *state = space.machine().driver_data<neogeo_state>();
	state->m_audio_cpu_rom_source = data;

	_set_audio_cpu_rom_source(space);
}


static void audio_cpu_banking_init( running_machine &machine )
{
	neogeo_state *state = machine.driver_data<neogeo_state>();
	int region;
	int bank;
	UINT8 *rgn;
	UINT32 address_mask;

	/* audio bios/cartridge selection */
	if (machine.root_device().memregion("audiobios")->base())
		state->membank(NEOGEO_BANK_AUDIO_CPU_MAIN_BANK)->configure_entry(0, machine.root_device().memregion("audiobios")->base());
	state->membank(NEOGEO_BANK_AUDIO_CPU_MAIN_BANK)->configure_entry(1, machine.root_device().memregion("audiocpu")->base());

	/* audio banking */
	address_mask = machine.root_device().memregion("audiocpu")->bytes() - 0x10000 - 1;

	rgn = state->memregion("audiocpu")->base();
	for (region = 0; region < 4; region++)
	{
		for (bank = 0; bank < 0x100; bank++)
		{
			UINT32 bank_address = 0x10000 + (((bank << (11 + region)) & 0x3ffff) & address_mask);
			state->membank(audio_banks[region])->configure_entry(bank, &rgn[bank_address]);
		}
	}

	/* set initial audio banks --
       how does this really work, or is it even necessary? */
	state->m_audio_cpu_banks[0] = 0x1e;
	state->m_audio_cpu_banks[1] = 0x0e;
	state->m_audio_cpu_banks[2] = 0x06;
	state->m_audio_cpu_banks[3] = 0x02;

	set_audio_cpu_banking(machine);

	state->m_audio_cpu_rom_source_last = 0;
	set_audio_cpu_rom_source(machine.device("maincpu")->memory().space(AS_PROGRAM), 0);
}



/*************************************
 *
 *  System control register
 *
 *************************************/

WRITE16_MEMBER(ng_aes_state::system_control_w)
{
	if (ACCESSING_BITS_0_7)
	{
		UINT8 bit = (offset >> 3) & 0x01;

		switch (offset & 0x07)
		{
		default:
		case 0x00: neogeo_set_screen_dark(machine(), bit); break;
		case 0x01: set_main_cpu_vector_table_source(machine(), bit);
				   set_audio_cpu_rom_source(space, bit); /* this is a guess */
				   break;
		case 0x05: neogeo_set_fixed_layer_source(machine(), bit); break;
//      case 0x06: set_save_ram_unlock(machine(), bit); break;
		case 0x07: neogeo_set_palette_bank(machine(), bit); break;

		case 0x02: /* unknown - HC32 middle pin 1 */
		case 0x03: /* unknown - uPD4990 pin ? */
		case 0x04: /* unknown - HC32 middle pin 10 */
			logerror("PC: %x  Unmapped system control write.  Offset: %x  Data: %x\n", space.device().safe_pc(), offset & 0x07, bit);
			break;
		}

		if (LOG_VIDEO_SYSTEM && ((offset & 0x07) != 0x06)) logerror("PC: %x  System control write.  Offset: %x  Data: %x\n", space.device().safe_pc(), offset & 0x07, bit);
	}
}


UINT8 ng_aes_state::neogeoReadTransfer(UINT32 sekAddress, int is_byte_transfer)
{
//  if ((sekAddress & 0x0FFFFF) < 16)
//      printf(PRINT_NORMAL, _T("  - NGCD port 0x%06X read (byte, PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));

	sekAddress ^= 1;

	switch (nActiveTransferArea) {
		case 0:							// Sprites
			return NeoSpriteRAM[nSpriteTransferBank + (sekAddress & 0x0FFFFF)];
			break;
		case 1:							// ADPCM
			return YM2610ADPCMAROM[nADPCMTransferBank + ((sekAddress & 0x0FFFFF) >> 1)];
			break;
		case 4:							// Z80
			if ((sekAddress & 0xfffff) >= 0x20000) break;
			return NeoZ80ROMActive[(sekAddress & 0x1FFFF) >> 1];
			break;
		case 5:							// Text
			return NeoTextRAM[(sekAddress & 0x3FFFF) >> 1];
			break;
	}

	return ~0;
}


void ng_aes_state::neogeoWriteTransfer(UINT32 sekAddress, UINT8 byteValue, int is_byte_transfer)
{
//  if ((sekAddress & 0x0FFFFF) < 16)
//      bprintf(PRINT_NORMAL, _T("  - Transfer: 0x%06X -> 0x%02X (PC: 0x%06X)\n"), sekAddress, byteValue, SekGetPC(-1));

	if (!nTransferWriteEnable) {
//      return;
	}
	int address;

	// why, is our DMA broken?
	sekAddress ^= 1;

	switch (nActiveTransferArea) {
		case 0:							// Sprites
			address = (nSpriteTransferBank + (sekAddress & 0x0FFFFF));

			// wtf? is this just due to how we decode the sprite gfx or is something bad happening?
			if ((address&3)==0) NeoSpriteRAM[address] = byteValue;
			if ((address&3)==1) NeoSpriteRAM[address^3] = byteValue;
			if ((address&3)==2) NeoSpriteRAM[address^3] = byteValue;
			if ((address&3)==3) NeoSpriteRAM[address] = byteValue;

			//  NeoCDOBJBankUpdate[nSpriteTransferBank >> 20] = true;
			break;
		case 1:							// ADPCM
			YM2610ADPCMAROM[nADPCMTransferBank + ((sekAddress & 0x0FFFFF) >> 1)] = byteValue;
			break;
		case 4:							// Z80
			if ((sekAddress & 0xfffff) >= 0x20000) break;

			if (!is_byte_transfer)
			{
				if (((sekAddress & 0xfffff) >= 0x20000) || nNeoCDZ80ProgWriteWordCancelHack) break;
				if (sekAddress == 0xe1fdf2) nNeoCDZ80ProgWriteWordCancelHack = 1;
			}

			NeoZ80ROMActive[(sekAddress & 0x1FFFF) >> 1] = byteValue;
			break;
		case 5:							// Text
			NeoTextRAM[(sekAddress & 0x3FFFF) >> 1] = byteValue;
//          NeoUpdateTextOne((sekAddress & 0x3FFFF) >> 1, byteValue);
			break;
	}
}



UINT16 ng_aes_state::neogeoReadWordCDROM(UINT32 sekAddress)
{
//  bprintf(PRINT_NORMAL, _T("  - CDROM: 0x%06X read (word, PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));


	switch (sekAddress & 0xFFFF) {

		case 0x0016:
			return m_tempcdc->nff0016_r();

		// LC8951 registers
		case 0x0100:
			return m_tempcdc->segacd_cdc_mode_address_r(*curr_space, 0, 0xffff);
		case 0x0102:
			return m_tempcdc->nLC8951_r();

		// CD mechanism communication
		case 0x0160:
			return m_tempcdc->neocd_cdd_rx_r();

		case 0x011C: // region
			return ~((0x10 | (NeoSystem & 3)) << 8);
	}


//  bprintf(PRINT_NORMAL, _T("  - NGCD port 0x%06X read (word, PC: 0x%06X)\n"), sekAddress, SekGetPC(-1));

	return ~0;
}


void ng_aes_state::neogeoWriteWordCDROM(UINT32 sekAddress, UINT16 wordValue)
{
//  bprintf(PRINT_NORMAL, _T("  - NGCD port 0x%06X -> 0x%04X (PC: 0x%06X)\n"), sekAddress, wordValue, SekGetPC(-1));
	int byteValue = wordValue & 0xff;

	switch (sekAddress & 0xFFFE) {
		case 0x0002:

			m_tempcdc->nff0002_set(wordValue);

			break;

		case 0x000E:
			m_tempcdc->NeoCDIRQUpdate(wordValue); // irqack
			break;

		case 0x0016:
			m_tempcdc->nff0016_set(byteValue);
			break;

			// DMA controller
		case 0x0060:
			if (byteValue & 0x40) {
				m_tempcdc->NeoCDDoDMA();
			}
			break;

		case 0x0064:
		case 0x0066:
		case 0x0068:
		case 0x006A:
		case 0x006C:
		case 0x006E:
		case 0x0070:
		case 0x0072:
		case 0x007E:
			m_tempcdc->set_DMA_regs(sekAddress & 0xFFFE, wordValue);
			break;

		// upload DMA controller program

		case 0x0080:
		case 0x0082:
		case 0x0084:
		case 0x0086:
		case 0x0088:
		case 0x008A:
		case 0x008C:
		case 0x008E:
//          bprintf(PRINT_NORMAL, _T("  - DMA controller program[%02i] -> 0x%04X (PC: 0x%06X)\n"), sekAddress & 0x0F, wordValue, SekGetPC(-1));
			break;

		// LC8951 registers
		case 0x0100:
			m_tempcdc->segacd_cdc_mode_address_w(*curr_space, 0, byteValue, 0xffff);
			break;
		case 0x0102:
			m_tempcdc->nLC8951_w(byteValue);
			break;

		case 0x0104:
//          bprintf(PRINT_NORMAL, _T("  - NGCD 0xE00000 area -> 0x%02X (PC: 0x%06X)\n"), byteValue, SekGetPC(-1));
			nActiveTransferArea = byteValue;
			break;

		case 0x0120:
//          bprintf(PRINT_NORMAL, _T("  - NGCD OBJ BUSREQ -> 1 (PC: 0x%06X)\n"), SekGetPC(-1));
			break;
		case 0x0122:
//          bprintf(PRINT_NORMAL, _T("  - NGCD PCM BUSREQ -> 1 (PC: 0x%06X) %x\n"), SekGetPC(-1), byteValue);
			break;
		case 0x0126:
//          bprintf(PRINT_NORMAL, _T("  - NGCD Z80 BUSREQ -> 1 (PC: 0x%06X)\n"), SekGetPC(-1));
			curr_space->machine().scheduler().synchronize();
			curr_space->machine().device("audiocpu")->execute().set_input_line(INPUT_LINE_HALT, ASSERT_LINE);

			break;
		case 0x0128:
//          bprintf(PRINT_NORMAL, _T("  - NGCD FIX BUSREQ -> 1 (PC: 0x%06X)\n"), SekGetPC(-1));
			break;

		case 0x0140:
//          bprintf(PRINT_NORMAL, _T("  - NGCD OBJ BUSREQ -> 0 (PC: 0x%06X)\n"), SekGetPC(-1));
			video_reset();
			break;
		case 0x0142:
//          bprintf(PRINT_NORMAL, _T("  - NGCD PCM BUSREQ -> 0 (PC: 0x%06X)\n"), SekGetPC(-1));
			break;
		case 0x0146:
//          bprintf(PRINT_NORMAL, _T("  - NGCD Z80 BUSREQ -> 0 (PC: 0x%06X)\n"), SekGetPC(-1));
			curr_space->machine().scheduler().synchronize();
			curr_space->machine().device("audiocpu")->execute().set_input_line(INPUT_LINE_HALT, CLEAR_LINE);
			break;
		case 0x0148:
//          bprintf(PRINT_NORMAL, _T("  - NGCD FIX BUSREQ -> 0 (PC: 0x%06X)\n"), SekGetPC(-1));
			video_reset();
			break;

		// CD mechanism communication
		case 0x0162:
			m_tempcdc->neocd_cdd_tx_w(byteValue);
			break;
		case 0x0164:
			m_tempcdc->NeoCDCommsControl(byteValue & 1, byteValue & 2);
			break;

		case 0x016c:
//          bprintf(PRINT_ERROR, _T("  - NGCD port 0x%06X -> 0x%02X (PC: 0x%06X)\n"), sekAddress, byteValue, SekGetPC(-1));

			MapVectorTable(!(byteValue == 0xFF));

//extern INT32 bRunPause;
//bRunPause = 1;
			break;

		case 0x016e:
//          bprintf(PRINT_IMPORTANT, _T("  - NGCD 0xE00000 area write access %s (0x%02X, PC: 0x%06X)\n"), byteValue ? _T("enabled") : _T("disabled"), byteValue, SekGetPC(-1));

			nTransferWriteEnable = byteValue;
			break;

		case 0x0180: {
			static UINT8 clara = 0;
			if (!byteValue && clara) {
//              bprintf(PRINT_IMPORTANT, _T("  - NGCD CD communication reset (PC: 0x%06X)\n"), SekGetPC(-1));
//              NeoCDCommsReset();
			}
			clara = byteValue;
			break;
		}
		case 0x0182: {
			static UINT8 clara = 0;
			if (!byteValue && clara) {
//              bprintf(PRINT_IMPORTANT, _T("  - NGCD Z80 reset (PC: 0x%06X)\n"), SekGetPC(-1));
				//ZetReset();
			}
			clara = byteValue;
			break;
		}
		case 0x01A0:
			nSpriteTransferBank = (byteValue & 3) << 20;
			break;
		case 0x01A2:
			nADPCMTransferBank  = (byteValue & 1) << 19;
			break;


		default: {
//          bprintf(PRINT_NORMAL, _T("  - NGCD port 0x%06X -> 0x%04X (PC: 0x%06X)\n"), sekAddress, wordValue, SekGetPC(-1));
		}
	}

}



READ16_MEMBER(ng_aes_state::neocd_control_r)
{
	return neogeoReadWordCDROM(0xff0000+ (offset*2));
}


WRITE16_MEMBER(ng_aes_state::neocd_control_w)
{
	neogeoWriteWordCDROM(0xff0000+ (offset*2), data);
}




/*
 *  Handling NeoCD banked RAM
 *  When the Z80 space is banked in to 0xe00000, only the low byte of each word is used
 */


READ16_MEMBER(ng_aes_state::neocd_transfer_r)
{
	int is_byte_transfer = 0;
	if (mem_mask != 0xffff) is_byte_transfer = 1;

	UINT16 ret = 0x0000;



	if (mem_mask & 0x00ff)
	{
		ret |= neogeoReadTransfer(0xe00000+ (offset*2)+1, is_byte_transfer) & 0xff;
	}
	if (mem_mask & 0xff00)
	{
		ret |= neogeoReadTransfer(0xe00000+ (offset*2), is_byte_transfer) << 8;
	}

	return ret;

}

WRITE16_MEMBER(ng_aes_state::neocd_transfer_w)
{
	int is_byte_transfer = 0;
	if (mem_mask != 0xffff) is_byte_transfer = 1;


	if (mem_mask & 0xff00)
	{
		neogeoWriteTransfer(0xe00000+ (offset*2), data>>8, is_byte_transfer);
	}

	if (mem_mask & 0x00ff)
	{
		neogeoWriteTransfer(0xe00000+ (offset*2)+1, data&0xff, is_byte_transfer);
	}


}

/*
 * Handling selectable controller types
 */

READ16_MEMBER(ng_aes_state::aes_in0_r)
{
	UINT32 ret = 0xffff;
	UINT32 ctrl = ioport("CTRLSEL")->read();

	switch(ctrl & 0x0f)
	{
	case 0x00:
		ret = 0xffff;
		break;
	case 0x01:
		ret = ioport("IN0")->read();
		break;
	case 0x02:
		switch (m_controller_select)
		{
			case 0x09: ret = ioport("MJ01_P1")->read(); break;
			case 0x12: ret = ioport("MJ02_P1")->read(); break;
			case 0x1b: ret = ioport("MJ03_P1")->read(); break; /* player 1 normal inputs? */
			case 0x24: ret = ioport("MJ04_P1")->read(); break;
			default:
				ret = ioport("IN0")->read();
				break;
		}
		break;
	}

	return ret;
}

READ16_MEMBER(ng_aes_state::aes_in1_r)
{
	UINT32 ret = 0xffff;
	UINT32 ctrl = ioport("CTRLSEL")->read();

	switch(ctrl & 0xf0)
	{
	case 0x00:
		ret = 0xffff;
		break;
	case 0x10:
		ret = ioport("IN1")->read();
		break;
	case 0x20:
		switch (m_controller_select)
		{
			case 0x09: ret = ioport("MJ01_P2")->read(); break;
			case 0x12: ret = ioport("MJ02_P2")->read(); break;
			case 0x1b: ret = ioport("MJ03_P2")->read(); break; /* player 2 normal inputs? */
			case 0x24: ret = ioport("MJ04_P2")->read(); break;
			default:
				ret = ioport("IN1")->read();
				break;
		}
		break;
	}

	return ret;
}


READ16_MEMBER(ng_aes_state::aes_in2_r)
{
	UINT32 in2 = ioport("IN2")->read();
	UINT32 ret = in2;
	UINT32 sel = ioport("CTRLSEL")->read();

	if((sel & 0x02) && (m_controller_select == 0x24))
		ret ^= 0x0200;

	if((sel & 0x20) && (m_controller_select == 0x24))
		ret ^= 0x0800;

	return ret;
}


/*************************************
 *
 *  Machine initialization
 *
 *************************************/

static void aes_postload(neogeo_state *state)
{
	_set_main_cpu_bank_address(state->machine());
	_set_main_cpu_vector_table_source(state->machine());
	set_audio_cpu_banking(state->machine());
	_set_audio_cpu_rom_source(state->machine().device("maincpu")->memory().space(AS_PROGRAM));
}



static void common_machine_start(running_machine &machine)
{
	neogeo_state *state = machine.driver_data<neogeo_state>();

	/* set the BIOS bank */
	state->membank(NEOGEO_BANK_BIOS)->set_base(state->memregion("mainbios")->base());

	/* set the initial main CPU bank */
	main_cpu_banking_init(machine);

	/* set the initial audio CPU ROM banks */
	audio_cpu_banking_init(machine);

	state->create_interrupt_timers(machine);

	/* irq levels for MVS / AES */
	state->m_vblank_level = 1;
	state->m_raster_level = 2;

	/* start with an IRQ3 - but NOT on a reset */
	state->m_irq3_pending = 1;

	/* get devices */
	state->m_maincpu = machine.device<cpu_device>("maincpu");
	state->m_audiocpu = machine.device<cpu_device>("audiocpu");
	state->m_upd4990a = machine.device("upd4990a");

	/* register state save */
	state->save_item(NAME(state->m_display_position_interrupt_control));
	state->save_item(NAME(state->m_display_counter));
	state->save_item(NAME(state->m_vblank_interrupt_pending));
	state->save_item(NAME(state->m_display_position_interrupt_pending));
	state->save_item(NAME(state->m_irq3_pending));
	state->save_item(NAME(state->m_audio_result));
	state->save_item(NAME(state->m_controller_select));
	state->save_item(NAME(state->m_main_cpu_bank_address));
	state->save_item(NAME(state->m_main_cpu_vector_table_source));
	state->save_item(NAME(state->m_audio_cpu_banks));
	state->save_item(NAME(state->m_audio_cpu_rom_source));
	state->save_item(NAME(state->m_audio_cpu_rom_source_last));
	state->save_item(NAME(state->m_save_ram_unlocked));
	state->save_item(NAME(state->m_output_data));
	state->save_item(NAME(state->m_output_latch));
	state->save_item(NAME(state->m_el_value));
	state->save_item(NAME(state->m_led1_value));
	state->save_item(NAME(state->m_led2_value));
	state->save_item(NAME(state->m_recurse));

	machine.save().register_postload(save_prepost_delegate(FUNC(aes_postload), state));
}

MACHINE_START_MEMBER(ng_aes_state,neogeo)
{
	common_machine_start(machine());

	/* initialize the memcard data structure */
	m_memcard_data = auto_alloc_array_clear(machine(), UINT8, MEMCARD_SIZE);
	save_pointer(NAME(m_memcard_data), 0x0800);
}

MACHINE_START_MEMBER(ng_aes_state,neocd)
{
	UINT8* ROM = machine().root_device().memregion("mainbios")->base();
	UINT8* RAM = machine().root_device().memregion("maincpu")->base();
	UINT8* Z80bios = machine().root_device().memregion("audiobios")->base();
	int x;

	common_machine_start(machine());

	/* irq levels for NEOCD (swapped compared to MVS / AES) */
	m_vblank_level = 2;
	m_raster_level = 1;


	/* initialize the memcard data structure */
	/* NeoCD doesn't have memcard slots, rather, it has a larger internal memory which works the same */
	m_memcard_data = auto_alloc_array_clear(machine(), UINT8, 0x2000);
	save_pointer(NAME(m_memcard_data), 0x2000);

	// copy initial 68k vectors into RAM
	memcpy(RAM,ROM,0x80);

	// copy Z80 code into Z80 space (from 0x20000)
	for(x=0;x<0x10000;x+=2)
	{
		Z80bios[x] = ROM[x+0x20001];
		Z80bios[x+1] = ROM[x+0x20000];
	}


	// for custom vectors
	machine().device("maincpu")->execute().set_irq_acknowledge_callback(neocd_int_callback);

	m_tempcdc->reset_NeoCd();
	
}

//static DEVICE_IMAGE_LOAD(aes_cart)
//{
//  else
//      return IMAGE_INIT_FAIL;

//  return IMAGE_INIT_PASS;
//}

/*************************************
 *
 *  Machine reset
 *
 *************************************/

MACHINE_RESET_MEMBER(ng_aes_state,neogeo)
{
	offs_t offs;
	address_space &space = machine().device("maincpu")->memory().space(AS_PROGRAM);

	/* reset system control registers */
	for (offs = 0; offs < 8; offs++)
		system_control_w(space, offs, 0, 0x00ff);

	machine().device("maincpu")->reset();

	neogeo_reset_rng(machine());

	start_interrupt_timers(machine());

	/* trigger the IRQ3 that was set by MACHINE_START */
	update_interrupts(machine());

	m_recurse = 0;

	/* AES apparently always uses the cartridge's fixed bank mode */
	neogeo_set_fixed_layer_source(machine(),1);

	NeoSpriteRAM = memregion("sprites")->base();
	YM2610ADPCMAROM = memregion("ymsnd")->base();
	NeoZ80ROMActive = memregion("audiocpu")->base();
	NeoTextRAM = memregion("fixed")->base();
	curr_space = &machine().device("maincpu")->memory().space(AS_PROGRAM);
	m_tempcdc->dma_space = curr_space;


	m_tempcdc->NeoCDCommsReset();

	nTransferWriteEnable = 0;

}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, AS_PROGRAM, 16, ng_aes_state )
	AM_RANGE(0x000000, 0x00007f) AM_ROMBANK(NEOGEO_BANK_VECTORS)
	AM_RANGE(0x000080, 0x0fffff) AM_ROM
	AM_RANGE(0x100000, 0x10ffff) AM_MIRROR(0x0f0000) AM_RAM
	/* some games have protection devices in the 0x200000 region, it appears to map to cart space, not surprising, the ROM is read here too */
	AM_RANGE(0x200000, 0x2fffff) AM_ROMBANK(NEOGEO_BANK_CARTRIDGE)
	AM_RANGE(0x2ffff0, 0x2fffff) AM_WRITE(main_cpu_bank_select_w)
	AM_RANGE(0x300000, 0x300001) AM_MIRROR(0x01ff7e) AM_READ(aes_in0_r)
	AM_RANGE(0x300080, 0x300081) AM_MIRROR(0x01ff7e) AM_READ_PORT("IN4")
	AM_RANGE(0x300000, 0x300001) AM_MIRROR(0x01ffe0) AM_READ(neogeo_unmapped_r) AM_WRITENOP	// AES has no watchdog
	AM_RANGE(0x320000, 0x320001) AM_MIRROR(0x01fffe) AM_READ_PORT("IN3") AM_WRITE(audio_command_w)
	AM_RANGE(0x340000, 0x340001) AM_MIRROR(0x01fffe) AM_READ(aes_in1_r)
	AM_RANGE(0x360000, 0x37ffff) AM_READ(neogeo_unmapped_r)
	AM_RANGE(0x380000, 0x380001) AM_MIRROR(0x01fffe) AM_READ(aes_in2_r)
	AM_RANGE(0x380000, 0x38007f) AM_MIRROR(0x01ff80) AM_WRITE(io_control_w)
	AM_RANGE(0x3a0000, 0x3a001f) AM_MIRROR(0x01ffe0) AM_READ(neogeo_unmapped_r) AM_WRITE(system_control_w)
	AM_RANGE(0x3c0000, 0x3c0007) AM_MIRROR(0x01fff8) AM_READ(neogeo_video_register_r)
	AM_RANGE(0x3c0000, 0x3c000f) AM_MIRROR(0x01fff0) AM_WRITE(neogeo_video_register_w)
	AM_RANGE(0x3e0000, 0x3fffff) AM_READ(neogeo_unmapped_r)
	AM_RANGE(0x400000, 0x401fff) AM_MIRROR(0x3fe000) AM_READWRITE(neogeo_paletteram_r, neogeo_paletteram_w)
	AM_RANGE(0x800000, 0x800fff) AM_READWRITE(memcard_r, memcard_w)
	AM_RANGE(0xc00000, 0xc1ffff) AM_MIRROR(0x0e0000) AM_ROMBANK(NEOGEO_BANK_BIOS)
	AM_RANGE(0xd00000, 0xd0ffff) AM_MIRROR(0x0f0000) AM_READ(neogeo_unmapped_r) AM_SHARE("save_ram") //AM_RAM_WRITE(save_ram_w)
	AM_RANGE(0xe00000, 0xffffff) AM_READ(neogeo_unmapped_r)
ADDRESS_MAP_END



static ADDRESS_MAP_START( neocd_main_map, AS_PROGRAM, 16, ng_aes_state )
	AM_RANGE(0x000000, 0x00007f) AM_RAMBANK(NEOGEO_BANK_VECTORS)
	AM_RANGE(0x000080, 0x1fffff) AM_RAM AM_SHARE("neocd_work_ram")
	/* some games have protection devices in the 0x200000 region, it appears to map to cart space, not surprising, the ROM is read here too */
	AM_RANGE(0x200000, 0x2fffff) AM_ROMBANK(NEOGEO_BANK_CARTRIDGE)
	AM_RANGE(0x2ffff0, 0x2fffff) AM_WRITE(main_cpu_bank_select_w)
	AM_RANGE(0x300000, 0x300001) AM_MIRROR(0x01ff7e) AM_READ(aes_in0_r)
	AM_RANGE(0x300080, 0x300081) AM_MIRROR(0x01ff7e) AM_READ_PORT("IN4")
	AM_RANGE(0x300000, 0x300001) AM_MIRROR(0x01ffe0) AM_READ(neogeo_unmapped_r) AM_WRITENOP	// AES has no watchdog
	AM_RANGE(0x320000, 0x320001) AM_MIRROR(0x01fffe) AM_READ_PORT("IN3") AM_WRITE(audio_command_w)
	AM_RANGE(0x340000, 0x340001) AM_MIRROR(0x01fffe) AM_READ(aes_in1_r)
	AM_RANGE(0x360000, 0x37ffff) AM_READ(neogeo_unmapped_r)
	AM_RANGE(0x380000, 0x380001) AM_MIRROR(0x01fffe) AM_READ(aes_in2_r)
	AM_RANGE(0x380000, 0x38007f) AM_MIRROR(0x01ff80) AM_WRITE(io_control_w)
	AM_RANGE(0x3a0000, 0x3a001f) AM_MIRROR(0x01ffe0) AM_READ(neogeo_unmapped_r) AM_WRITE(system_control_w)
	AM_RANGE(0x3c0000, 0x3c0007) AM_MIRROR(0x01fff8) AM_READ(neogeo_video_register_r)
	AM_RANGE(0x3c0000, 0x3c000f) AM_MIRROR(0x01fff0) AM_WRITE(neogeo_video_register_w)
	AM_RANGE(0x3e0000, 0x3fffff) AM_READ(neogeo_unmapped_r)
	AM_RANGE(0x400000, 0x401fff) AM_MIRROR(0x3fe000) AM_READWRITE(neogeo_paletteram_r, neogeo_paletteram_w)
	AM_RANGE(0x800000, 0x803fff) AM_READWRITE(neocd_memcard_r, neocd_memcard_w)
	AM_RANGE(0xc00000, 0xcfffff) AM_ROMBANK(NEOGEO_BANK_BIOS)
	AM_RANGE(0xd00000, 0xd0ffff) AM_MIRROR(0x0f0000) AM_READ(neogeo_unmapped_r) AM_SHARE("save_ram") //AM_RAM_WRITE(save_ram_w)
	AM_RANGE(0xe00000, 0xefffff) AM_READWRITE(neocd_transfer_r,neocd_transfer_w)
	AM_RANGE(0xf00000, 0xfeffff) AM_READ(neogeo_unmapped_r)
	AM_RANGE(0xff0000, 0xff01ff) AM_READWRITE(neocd_control_r, neocd_control_w) // CDROM / DMA
	AM_RANGE(0xff0200, 0xffffff) AM_READ(neogeo_unmapped_r)
ADDRESS_MAP_END


/*************************************
 *
 *  Audio CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( audio_map, AS_PROGRAM, 8, ng_aes_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_MAIN_BANK)
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK3)
	AM_RANGE(0xc000, 0xdfff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK2)
	AM_RANGE(0xe000, 0xefff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK1)
	AM_RANGE(0xf000, 0xf7ff) AM_ROMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK0)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END



/*************************************
 *
 *  Audio CPU port handlers
 *
 *************************************/

static ADDRESS_MAP_START( audio_io_map, AS_IO, 8, ng_aes_state )
  /*AM_RANGE(0x00, 0x00) AM_MIRROR(0xff00) AM_READWRITE(audio_command_r, audio_cpu_clear_nmi_w);*/  /* may not and NMI clear */
	AM_RANGE(0x00, 0x00) AM_MIRROR(0xff00) AM_READ(audio_command_r)
	AM_RANGE(0x04, 0x07) AM_MIRROR(0xff00) AM_DEVREADWRITE_LEGACY("ymsnd", ym2610_r, ym2610_w)
	AM_RANGE(0x08, 0x08) AM_MIRROR(0xff00) /* write - NMI enable / acknowledge? (the data written doesn't matter) */
	AM_RANGE(0x08, 0x08) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_f000_f7ff_r)
	AM_RANGE(0x09, 0x09) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_e000_efff_r)
	AM_RANGE(0x0a, 0x0a) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_c000_dfff_r)
	AM_RANGE(0x0b, 0x0b) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_8000_bfff_r)
	AM_RANGE(0x0c, 0x0c) AM_MIRROR(0xff00) AM_WRITE(audio_result_w)
	AM_RANGE(0x18, 0x18) AM_MIRROR(0xff00) /* write - NMI disable? (the data written doesn't matter) */
ADDRESS_MAP_END



// does the Z80 actually see all of this as RAM on the NeoCD, or is it only actually writable via the transfer areas?
static ADDRESS_MAP_START( neocd_audio_map, AS_PROGRAM, 8, ng_aes_state )
	AM_RANGE(0x0000, 0x7fff) AM_RAMBANK(NEOGEO_BANK_AUDIO_CPU_MAIN_BANK)
	AM_RANGE(0x8000, 0xbfff) AM_RAMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK3)
	AM_RANGE(0xc000, 0xdfff) AM_RAMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK2)
	AM_RANGE(0xe000, 0xefff) AM_RAMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK1)
	AM_RANGE(0xf000, 0xf7ff) AM_RAMBANK(NEOGEO_BANK_AUDIO_CPU_CART_BANK0)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( neocd_audio_io_map, AS_IO, 8, ng_aes_state )
  /*AM_RANGE(0x00, 0x00) AM_MIRROR(0xff00) AM_READWRITE(audio_command_r, audio_cpu_clear_nmi_w);*/  /* may not and NMI clear */
	AM_RANGE(0x00, 0x00) AM_MIRROR(0xff00) AM_READ(audio_command_r)
	AM_RANGE(0x04, 0x07) AM_MIRROR(0xff00) AM_DEVREADWRITE_LEGACY("ymsnd", ym2610_r, ym2610_w)
	AM_RANGE(0x08, 0x08) AM_MIRROR(0xff00) /* write - NMI enable / acknowledge? (the data written doesn't matter) */
//  AM_RANGE(0x08, 0x08) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_f000_f7ff_r)
//  AM_RANGE(0x09, 0x09) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_e000_efff_r)
//  AM_RANGE(0x0a, 0x0a) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_c000_dfff_r)
//  AM_RANGE(0x0b, 0x0b) AM_MIRROR(0xfff0) AM_MASK(0xfff0) AM_READ(audio_cpu_bank_select_8000_bfff_r)
	AM_RANGE(0x0c, 0x0c) AM_MIRROR(0xff00) AM_WRITE(audio_result_w)
	AM_RANGE(0x18, 0x18) AM_MIRROR(0xff00) /* write - NMI disable? (the data written doesn't matter) */
ADDRESS_MAP_END


/*************************************
 *
 *  Audio interface
 *
 *************************************/

static const ym2610_interface ym2610_config =
{
	audio_cpu_irq
};



/*************************************
 *
 *  Standard Neo-Geo DIPs and
 *  input port definition
 *
 *************************************/

#define STANDARD_DIPS																		\
	PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" ) PORT_DIPLOCATION("SW:1")					\
	PORT_DIPSETTING(	  0x0001, DEF_STR( Off ) )											\
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )											\
	PORT_DIPNAME( 0x0002, 0x0002, "Coin Chutes?" ) PORT_DIPLOCATION("SW:2")					\
	PORT_DIPSETTING(	  0x0000, "1?" )													\
	PORT_DIPSETTING(	  0x0002, "2?" )													\
	PORT_DIPNAME( 0x0004, 0x0004, "Autofire (in some games)" ) PORT_DIPLOCATION("SW:3")		\
	PORT_DIPSETTING(	  0x0004, DEF_STR( Off ) )											\
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )											\
	PORT_DIPNAME( 0x0018, 0x0018, "COMM Setting (Cabinet No.)" ) PORT_DIPLOCATION("SW:4,5")	\
	PORT_DIPSETTING(	  0x0018, "1" )														\
	PORT_DIPSETTING(	  0x0008, "2" )														\
	PORT_DIPSETTING(	  0x0010, "3" )														\
	PORT_DIPSETTING(	  0x0000, "4" )														\
	PORT_DIPNAME( 0x0020, 0x0020, "COMM Setting (Link Enable)" ) PORT_DIPLOCATION("SW:6")	\
	PORT_DIPSETTING(	  0x0020, DEF_STR( Off ) )											\
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )											\
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Free_Play ) ) PORT_DIPLOCATION("SW:7")			\
	PORT_DIPSETTING(	  0x0040, DEF_STR( Off ) )											\
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )											\
	PORT_DIPNAME( 0x0080, 0x0080, "Freeze" ) PORT_DIPLOCATION("SW:8")						\
	PORT_DIPSETTING(	  0x0080, DEF_STR( Off ) )											\
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )

#define STANDARD_IN2																				\
	PORT_START("IN2")																				\
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )													\
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_START1 )									\
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON5) PORT_NAME("1P Select") PORT_CODE(KEYCODE_5) PORT_PLAYER(1)	\
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_START2 )									\
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON5) PORT_NAME("2P Select") PORT_CODE(KEYCODE_6) PORT_PLAYER(2)	\
	PORT_BIT( 0x7000, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, ng_aes_state, get_memcard_status, NULL)			\
	PORT_BIT( 0x8000, IP_ACTIVE_HIGH, IPT_UNKNOWN )  /* Matrimelee expects this bit to be active high when on an AES */

#define STANDARD_IN3																				\
	PORT_START("IN3")																				\
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_UNUSED )  /* Coin 1 - AES has no coin slots, it's a console */	\
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNUSED )  /* Coin 2 - AES has no coin slots, it's a console */	\
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNUSED )  /* Service Coin - not used, AES is a console */	\
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_UNKNOWN ) /* having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms */	\
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_UNKNOWN ) /* having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms */	\
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_SPECIAL ) /* what is this? */								\
	PORT_BIT( 0x00c0, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(get_calendar_status, NULL)			\
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(get_audio_result, NULL)


#define STANDARD_IN4																			\
	PORT_START("IN4")																			\
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_UNKNOWN )												\
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_UNKNOWN )												\
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_UNKNOWN )												\
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_UNKNOWN )												\
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_UNKNOWN )												\
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_UNKNOWN )												\
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* what is this? - is 0 for 1 or 2 slot MVS (and AES?)*/							\
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Enter BIOS") PORT_CODE(KEYCODE_F2)	\
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

static INPUT_PORTS_START( controller )
	PORT_START("IN0")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x01)


	PORT_START("IN1")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x10)
INPUT_PORTS_END

static INPUT_PORTS_START( mjpanel )
	PORT_START("MJ01_P1")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_MAHJONG_A ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_MAHJONG_B ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_MAHJONG_C ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_MAHJONG_D ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_MAHJONG_E ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_MAHJONG_F ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_MAHJONG_G ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("MJ02_P1")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_MAHJONG_H ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_MAHJONG_I ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_MAHJONG_J ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_MAHJONG_K ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_MAHJONG_L ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_MAHJONG_M ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_MAHJONG_N ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)

	PORT_START("MJ03_P1")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)

	PORT_START("MJ04_P1")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_MAHJONG_PON ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_MAHJONG_CHI ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_MAHJONG_KAN ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_MAHJONG_RON ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_MAHJONG_REACH ) PORT_PLAYER(1) PORT_CONDITION("CTRLSEL", 0x0f, EQUALS, 0x02)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("MJ01_P2")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_MAHJONG_A ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_MAHJONG_B ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_MAHJONG_C ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_MAHJONG_D ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_MAHJONG_E ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_MAHJONG_F ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_MAHJONG_G ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("MJ02_P2")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_MAHJONG_H ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_MAHJONG_I ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_MAHJONG_J ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_MAHJONG_K ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_MAHJONG_L ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_MAHJONG_M ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_MAHJONG_N ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("MJ03_P2")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)

	PORT_START("MJ04_P2")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_MAHJONG_PON ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_MAHJONG_CHI ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_MAHJONG_KAN ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_MAHJONG_RON ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_MAHJONG_REACH ) PORT_PLAYER(2) PORT_CONDITION("CTRLSEL", 0xf0, EQUALS, 0x20)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

INPUT_PORTS_END

static INPUT_PORTS_START( aes )
// was there anyting in place of dipswitch in the home console?
/*  PORT_START("IN0")
    PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
//  PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" ) PORT_DIPLOCATION("SW:1")
//  PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
//  PORT_DIPNAME( 0x0002, 0x0002, "Coin Chutes?" ) PORT_DIPLOCATION("SW:2")
//  PORT_DIPSETTING(      0x0000, "1?" )
//  PORT_DIPSETTING(      0x0002, "2?" )
//  PORT_DIPNAME( 0x0004, 0x0000, "Mahjong Control Panel (or Auto Fire)" ) PORT_DIPLOCATION("SW:3")
//  PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
//  PORT_DIPNAME( 0x0018, 0x0018, "COMM Setting (Cabinet No.)" ) PORT_DIPLOCATION("SW:4,5")
//  PORT_DIPSETTING(      0x0018, "1" )
//  PORT_DIPSETTING(      0x0008, "2" )
//  PORT_DIPSETTING(      0x0010, "3" )
//  PORT_DIPSETTING(      0x0000, "4" )
//  PORT_DIPNAME( 0x0020, 0x0020, "COMM Setting (Link Enable)" ) PORT_DIPLOCATION("SW:6")
//  PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
//  PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Free_Play ) ) PORT_DIPLOCATION("SW:7")
//  PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
//  PORT_DIPNAME( 0x0080, 0x0080, "Freeze" ) PORT_DIPLOCATION("SW:8")
//  PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
    PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(aes_controller_r, NULL) */

	PORT_START("CTRLSEL")  /* Select Controller Type */
	PORT_CONFNAME( 0x0f, 0x01, "P1 Controller")
	PORT_CONFSETTING(  0x00, "Unconnected" )
	PORT_CONFSETTING(  0x01, "NeoGeo Controller" )
	PORT_CONFSETTING(  0x02, "NeoGeo Mahjong Panel" )
	PORT_CONFNAME( 0xf0, 0x10, "P2 Controller")
	PORT_CONFSETTING(  0x00, "Unconnected" )
	PORT_CONFSETTING(  0x10, "NeoGeo Controller" )
	PORT_CONFSETTING(  0x20, "NeoGeo Mahjong Panel" )

	PORT_INCLUDE( controller )

	PORT_INCLUDE( mjpanel )

// were some of these present in the AES?!?
	STANDARD_IN2

	STANDARD_IN3

	STANDARD_IN4
INPUT_PORTS_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_CONFIG_START( neogeo, ng_aes_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, NEOGEO_MAIN_CPU_CLOCK)
	MCFG_CPU_PROGRAM_MAP(main_map)

	MCFG_CPU_ADD("audiocpu", Z80, NEOGEO_AUDIO_CPU_CLOCK)
	MCFG_CPU_PROGRAM_MAP(audio_map)
	MCFG_CPU_IO_MAP(audio_io_map)

	MCFG_MACHINE_START_OVERRIDE(ng_aes_state, neogeo)
	MCFG_MACHINE_RESET_OVERRIDE(ng_aes_state, neogeo)

	// temporary until things are cleaned up
	MCFG_DEVICE_ADD("tempcdc", LC89510_TEMP, 0) // cd controller
	MCFG_SEGACD_HACK_SET_NEOCD

	/* video hardware */
	MCFG_DEFAULT_LAYOUT(layout_neogeo)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_RAW_PARAMS(NEOGEO_PIXEL_CLOCK, NEOGEO_HTOTAL, NEOGEO_HBEND, NEOGEO_HBSTART, NEOGEO_VTOTAL, NEOGEO_VBEND, NEOGEO_VBSTART)
	MCFG_SCREEN_UPDATE_DRIVER(ng_aes_state, screen_update_neogeo)

	/* audio hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("ymsnd", YM2610, NEOGEO_YM2610_CLOCK)
	MCFG_SOUND_CONFIG(ym2610_config)
	MCFG_SOUND_ROUTE(0, "lspeaker",  0.60)
	MCFG_SOUND_ROUTE(0, "rspeaker", 0.60)
	MCFG_SOUND_ROUTE(1, "lspeaker",  1.0)
	MCFG_SOUND_ROUTE(2, "rspeaker", 1.0)

	/* NEC uPD4990A RTC */
	MCFG_UPD4990A_ADD("upd4990a")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( aes, neogeo )

	MCFG_MEMCARD_HANDLER(neogeo)

	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_LOAD(neo_cartridge)
	MCFG_CARTSLOT_INTERFACE("neo_cart")
	MCFG_CARTSLOT_MANDATORY

	MCFG_SOFTWARE_LIST_ADD("cart_list","neogeo")
	MCFG_SOFTWARE_LIST_FILTER("cart_list","AES")
MACHINE_CONFIG_END



/* NeoCD uses custom vectors on IRQ4 to handle various events from the CDC */

static IRQ_CALLBACK(neocd_int_callback)
{
	ng_aes_state *state = device->machine().driver_data<ng_aes_state>();

	if (irqline==4)
	{
		if (state->m_tempcdc->get_nNeoCDIRQVectorAck()) {
			state->m_tempcdc->set_nNeoCDIRQVectorAck(0);
			return state->m_tempcdc->get_nNeoCDIRQVector();
		}
	}

	return (0x60+irqline*4)/4;
}



struct cdrom_interface neocd_cdrom =
{
	"neocd_cdrom",
	NULL
};

static MACHINE_CONFIG_DERIVED( neocd, neogeo )

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(neocd_main_map)

	MCFG_CPU_MODIFY("audiocpu")
	MCFG_CPU_PROGRAM_MAP(neocd_audio_map)
	MCFG_CPU_IO_MAP(neocd_audio_io_map)


	MCFG_MACHINE_START_OVERRIDE(ng_aes_state,neocd)

	MCFG_CDROM_ADD( "cdrom",neocd_cdrom )
	MCFG_SOFTWARE_LIST_ADD("cd_list","neocd")
MACHINE_CONFIG_END

/*************************************
 *
 *  Driver initalization
 *
 *************************************/

DRIVER_INIT_MEMBER(ng_aes_state,neogeo)
{
}


ROM_START( aes )
	ROM_REGION16_BE( 0x80000, "mainbios", 0 )
	ROM_SYSTEM_BIOS( 0, "jap-aes",   "Japan AES" )
	ROMX_LOAD("neo-po.bin",  0x00000, 0x020000, CRC(16d0c132) SHA1(4e4a440cae46f3889d20234aebd7f8d5f522e22c), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(1))	/* AES Console (Japan) Bios */
	ROM_SYSTEM_BIOS( 1, "asia-aes",   "Asia AES" )
	ROMX_LOAD("neo-epo.bin", 0x00000, 0x020000, CRC(d27a71f1) SHA1(1b3b22092f30c4d1b2c15f04d1670eb1e9fbea07), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(2))	/* AES Console (Asia?) Bios */
//  ROM_SYSTEM_BIOS( 2, "uni-bios_2_3","Universe Bios (Hack, Ver. 2.3)" )
//  ROMX_LOAD( "uni-bios_2_3.rom",  0x00000, 0x020000, CRC(27664eb5) SHA1(5b02900a3ccf3df168bdcfc98458136fd2b92ac0), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(3) ) /* Universe Bios v2.3 (hack) */

	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "audiobios", 0 )
	ROM_LOAD( "sm1.sm1", 0x00000, 0x20000, CRC(94416d67) SHA1(42f9d7ddd6c0931fd64226a60dc73602b2819dcf) )

	ROM_REGION( 0x90000, "audiocpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "zoomy", 0 )
	ROM_LOAD( "000-lo.lo", 0x00000, 0x20000, CRC(5a86cff2) SHA1(5992277debadeb64d1c1c64b0a92d9293eaf7e4a) )

	ROM_REGION( 0x20000, "fixedbios", 0 )
	ROM_LOAD( "sfix.sfix", 0x000000, 0x20000, CRC(c2ea0cfd) SHA1(fd4a618cdcdbf849374f0a50dd8efe9dbab706c3) )

	ROM_REGION( 0x20000, "fixed", ROMREGION_ERASEFF )

	ROM_REGION( 0x1000000, "ymsnd", ROMREGION_ERASEFF )

	ROM_REGION( 0x1000000, "ymsnd.deltat", ROMREGION_ERASEFF )

	ROM_REGION( 0x900000, "sprites", ROMREGION_ERASEFF )
ROM_END

ROM_START( neocd )
	ROM_REGION16_BE( 0x100000, "mainbios", 0 )
	ROM_SYSTEM_BIOS( 0, "top",   "Top loading NeoGeo CD" )
	ROMX_LOAD( "top-sp1.bin",    0x00000, 0x80000, CRC(c36a47c0) SHA1(235f4d1d74364415910f73c10ae5482d90b4274f), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(1))
	ROM_SYSTEM_BIOS( 1, "front",   "Front loading NeoGeo CD" )
	ROMX_LOAD( "front-sp1.bin",    0x00000, 0x80000, CRC(cac62307) SHA1(53bc1f283cdf00fa2efbb79f2e36d4c8038d743a), ROM_GROUPWORD | ROM_REVERSE | ROM_BIOS(2))

	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x100000, "audiobios", ROMREGION_ERASEFF )

	ROM_REGION( 0x90000, "audiocpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "zoomy", 0 )
	ROM_LOAD( "000-lo.lo", 0x00000, 0x20000, CRC(5a86cff2) SHA1(5992277debadeb64d1c1c64b0a92d9293eaf7e4a) )

	ROM_REGION( 0x20000, "fixedbios", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "fixed", ROMREGION_ERASEFF )

	ROM_REGION( 0x400000, "ymsnd", ROMREGION_ERASEFF )

//    NO_DELTAT_REGION

	ROM_REGION( 0x400000, "sprites", ROMREGION_ERASEFF )
ROM_END

ROM_START( neocdz )
	ROM_REGION16_BE( 0x100000, "mainbios", 0 )
	ROM_LOAD16_WORD_SWAP( "neocd.bin",    0x00000, 0x80000, CRC(df9de490) SHA1(7bb26d1e5d1e930515219cb18bcde5b7b23e2eda) )

	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "audiobios", ROMREGION_ERASEFF )

	ROM_REGION( 0x90000, "audiocpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "zoomy", 0 )
	ROM_LOAD( "000-lo.lo", 0x00000, 0x20000, CRC(5a86cff2) SHA1(5992277debadeb64d1c1c64b0a92d9293eaf7e4a) )

	ROM_REGION( 0x20000, "fixedbios", ROMREGION_ERASEFF )

	ROM_REGION( 0x20000, "fixed", ROMREGION_ERASEFF )

	ROM_REGION( 0x400000, "ymsnd", ROMREGION_ERASEFF )

//    NO_DELTAT_REGION

	ROM_REGION( 0x400000, "sprites", ROMREGION_ERASEFF )
ROM_END

/*    YEAR  NAME  PARENT COMPAT MACHINE INPUT  INIT     COMPANY      FULLNAME            FLAGS */
CONS( 1990, aes,    0,		0,   aes,      aes, ng_aes_state,   neogeo,  "SNK", "Neo-Geo AES", 0)

CONS( 1996, neocdz, 0,	    0,   neocd,    aes, ng_aes_state,   neogeo,  "SNK", "Neo-Geo CDZ", GAME_NOT_WORKING ) // the CDZ is the newer slot-loading model, faster drive etc.
CONS( 1994, neocd,  neocdz,	0,   neocd,    aes, ng_aes_state,   neogeo,  "SNK", "Neo-Geo CD", GAME_NOT_WORKING ) // older Top Loading model, ignores disc protections?
