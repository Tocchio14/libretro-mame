/***************************************************************************

    Sega System 32/Multi 32 hardware

****************************************************************************

    Still to do:
        * fix protection
        * fix jpark correctly

****************************************************************************

Slip Stream (950515 HISPANIC)
Sega, 1995

This game runs on Sega System 32 hardware.

PCB Layout
----------
837-7428  171-5964E (C) SEGA 1990
|-------------------------------------------------------------------------------------------|
| TDA1518                                                               8464      8464      |
|             HM65256                                                   8464      8464      |
|  TL064      HM65256             |---------|                             |---------|       |
|   LC7881  |-----|               |SEGA     |                             |SEGA     |       |
|           |ASSP |               |315-5385 |                             |315-5388 |       |
|CND        |5C105|               |         |                             |         |       |
|           |1FD07|               |         |             |---------|     |         |       |
|           |-----|               |---------|        CNF  |SEGA     |     |---------|       |
|                                                         |315-5242 |                       |
|-|  TL062   TL064                  84256                 |         |                       |
  |                                 84256                 |         |                    CNI|
|-|                                                       |---------|      |---------|      |
|             YM3438                                                       |SEGA     |      |
|                             |-----------|                                |315-5387 |      |
|             YM3438          |NEC        |       315-5441                 |         |      |
|                             |D70616R-16 |                                |         |      |
|J              BAT_5.5V      |9105M9 V60 |                                |---------|      |
|A   BAT_3.3V                 |(C)NEC 1986|                                                 |
|M                            |           |                                  D42264         |
|M            8464            |-----------|                                  D42264         |
|A                                                                           D42264         |
|             Z80                                                            D42264         |
|                            HM53461ZP-12   HM53461ZP-12                                    |
|                            HM53461ZP-12   HM53461ZP-12                                    |
|                            HM53461ZP-12   HM53461ZP-12                                    |
|                            HM53461ZP-12   HM53461ZP-12                |-----------|       |
|-|                          HM53461ZP-12   HM53461ZP-12                | SEGA      |    CNE|
  |      |-------|           HM53461ZP-12   HM53461ZP-12                | 315-5386A |       |
|-|      |SEGA   |           HM53461ZP-12   HM53461ZP-12                |           |       |
|        |315-   |           HM53461ZP-12   HM53461ZP-12                |           |       |
|        |5296   |                                                      |           |       |
|        |-------|        3771   3771                                   |-----------|       |
|         93C46                                                              D42264         |
|CNB                         SW2                                             D42264         |
|                            SW1                                             D42264         |
|       CNA                  DSW1    32MHz  50MHz                            D42264         |
|-------------------------------------------------------------------------------------------|

Notes:
      V60 CPU running at 16.00MHz [32/2]
      Z80 CPU running at 8.000MHz [32/4]
      YM3438 running at 8.000MHz [32/4]
      CNE/F/I - Multi-pin connectors for connection of ROM Board
      CND     - 4 pin connector for 2nd Speaker for Stereo Output
      CNA     - 30 pin connector for extra controls PCB
      CNB     - 26 pin connector (purpose unknown)
      SW1     - push-button TEST switch
      SW2     - push button SERVICE switch
      DSW1    - 4-position DIP Switch
      HM53461 - Hitachi 256k Dual Port RAM
      D42264  - NEC 256k Dual Port RAM

      SEGA Custom ICs:
                      315-5441  Lattice GAL16V8A (DIP20)
                      315-5386A (QFP184)
                      315-5388  (QFP160)
                      315-5387  (QFP160)
                      315-5242  (custom ceramic DIP32 containing a small QFP IC and some capacitors/resistors etc)
                      315-5296  (QFP100)
                      ASSP 5C105 (QFP80)
                      315-5385  (QFP128)


ROM Board
---------
837-7429-01
|-------------------------------------------------------------------------------------------|
|JP10-JP27                     CNJ                    CNH                      JP1-JP9      |
|              IC6                                                                          |
| IC7                                                                                       |
|              IC13                                                                         |
| IC14                                                                                      |
|            315-5442                                                                       |
| IC22                                 IC25                IC24               IC23          |
|                                                                                           |
| IC26                                 IC29                IC28               IC27          |
|                                                                                           |
| IC31            CNK                  IC34                IC23               IC32          |
|                                                                                           |
| IC35                                 IC38                IC37               IC36          |
|             CNG       JP28-JP32                                                           |
|-------------------------------------------------------------------------------------------|

Notes:
CNG/H/J  - Multi-pin connectors (below PCB) for connection of ROM Board to Main Board
CNK      - Custom Sega connector for connection of protection daughterboard (not used)
315-5442 - MMI PAL16L8ACN (DIP20)

Jumpers: (to configure ROM sizes used on Slip Stream. Other games)
         (may vary depending on ROM types used)
JP1: 2-3           JP17: 1-2
JP2: 2-3           JP18: 2-3
JP3: 2-3           JP19: 1-2
JP4: 1-2           JP20: 2-3
JP5: 2-3           JP21: 1-2
JP6: 2-3           JP22: 2-3
JP7: 1-2           JP23: 1-2
JP8: 2-3           JP24: 1-2
JP9: 1-2           JP25: 2-3
JP10: 2-3          JP26: 1-2
JP11: 1-2          JP27: 1-2
JP12: 1-2          JP28: 2-3
JP13: 1-2          JP29: 1-2
JP14: 2-3          JP30: 2-3
JP15: 2-3          JP31: 1-2
JP16: 2-3          JP32: 1-2

ROM Locations
-------------
PCB Label  CPU P0 CPU P1    CPU D0   CPU D1   SND0     SND1     SND2 SND3 SCR0     SCR1     SCR2     SCR3     OBJ0     OBJ1     OBJ2     OBJ3     OBJ4     OBJ5     OBJ6     OBJ7
IC#        IC13   IC6       IC7      IC14     IC35     IC31     -    -    IC38     IC34     IC29     IC25     IC36     IC32     IC27     IC23     IC37     IC33     IC28     IC24
IC Type    -      27C4002   27C4000  27C4000  27C010   27C4000  -    -    27C4000  27C4000  27C4000  27C4000  27C4000  27C4000  27C4000  27C4000  27C4000  27C4000  27C4000  27C4000
ROM Label  -      S32HPRG01 S32DAT00 S32DAT01 S32SND00 S32SND01 -    -    S32SCR00 S32SCR01 S32SCR02 S32SCR03 S32OBJ00 S32OBJ01 S32OBJ02 S32OBJ03 S32OBJ04 S32OBJ05 S32OBJ06 S32OBJ07


Extra Controls PCB
------------------
837-7536
837-7536-91 (sticker)
|------------------------|
|                        |
|                 74HC74 |
|         OKI_M6253      |
|  74HC4053              |
| DAP601          74HC139|
| DAN601                 |
|CN2              CN1    |
|------------------------|
Notes:
CN2 - Multi-pin connector for controls
CN1 - Connector joining to CNA on main PCB


*********************************************************************************************


Title Fight (Export Revision)
Sega, 1992

This game runs on Sega System Multi32 hardware.

PCB Layout
----------
837-8676  171-6253C (C) SEGA 1992
834-9324-02 (STICKER)
|-------------------------------------------------------------------------------------------|
|             CNF                                     CNH                          8464     |
|8464   HM53461ZP-12 (x16)      |-----------|    HM53461ZP-12 (x16)                8464     |
|8464                           | SEGA      |                                      8464     |
|8464                           | 315-5386A |                                      8464     |
|8464                           |           |                                               |
|                               |           |                                               |
|                               |           |                                               |
|   |---------|  |---------|    |-----------|                    |---------|  |---------|   |
|   |SEGA     |  |SEGA     |                                     |SEGA     |  |SEGA     |   |
|   |315-5388 |  |315-5387 |  HM53461ZP-12 (x8)                  |315-5388 |  |315-5242 |   |
|   |         |  |         |                                     |         |  |         |   |
|   |         |  |         |                                     |         |  |         |   |
|   |---------|  |---------|                                     |---------|  |---------|   |
|             CND                                     CNE                                   |
| |---------| MB3771 MB3771                      315-5596              |-------| M5M5278P-25|
| |SEGA     |                                                          |SEGA   |            |
| |315-5242 |      8464                                                |315-   | M5M5278P-25|
| |         |BAT_3.3V                                                  |5591   |            |
| |         |BAT_5.5V                     |---------|                  |-------| M5M5278P-25|
| |---------|         Z80    50MHz        |SEGA     |     |-----------|                 DSW1|
|                            32MHz        |315-5385 |     |NEC        |          M5M5278P-25|
| |------|                                |         |     |D70632R-20 |                 SW4 |
| |SEGA  |                   |-------|    |         |     |9314X9 V70 |    40MHz        SW3 |
| |315-  |                   |SEGA   |    |---------|     |(C)NEC 1986|                     |
| |5560  | YM3438            |315-   |                    |           |                     |
| |------| LC78820           |5296   |                    |-----------|              93C45  |
|TDA1518                     |-------|                                   |-------|          |
|                                       SW1    SW2                       |SEGA   |       CNM|
|  VOL                                                                   |315-   |          |
|                                                                        |5296   |          |
| UPC844 UPC4064                                                         |-------|          |
|                                                                                           |
|CNC                                                      LED                               |
--|            JAMMA             |----|     CNJ      |----|            JAMMA             |--|
  |------------------------------|    |--------------|    |------------------------------|
Notes:
      V70 CPU running at 20.00MHz [40/2]
      Z80 CPU running at 8.000MHz [32/4]
      YM3438 running at 8.000MHz [32/4]
      315-5560 running at 8.000MHz [32/4]
      CND/E/F/H: Multi-pin connectors for connection of ROM Board
      CNC      : 4 pin connector for 2nd Speaker for Stereo Output
      CNJ      : 32 pin connector (purpose unknown)
      CNM      : 30 pin connector (purpose unknown)
      SW1      : push-button TEST switch
      SW2      : push button SERVICE switch
      SW3/4    : push button switches (purpose unknown)
      DSW1     : 4-position DIP Switch

      SEGA Custom ICs:
                      315-5596  Lattice GAL16V8A (DIP20)
                      315-5386A (QFP184)
                      315-5388  (x2, QFP160)
                      315-5387  (QFP160)
                      315-5242  (x2, custom ceramic DIP32 containing a small QFP IC and some capacitors/resistors etc)
                      315-5296  (x2, QFP100)
                      315-5560  (QFP80)
                      315-5385  (QFP128)
                      315-5591  (QFP100)


ROM Board
---------
837-8890
|-------------------------------------------------------------------------------------------|
|             CN3                                     CN4                                   |
|                                                                                           |
|             IC3                          IC14            IC15              IC10           |
| IC1   IC2                   JP1-5                                                         |
|                                                                                           |
|             IC11            JP6-10       IC22            IC23              IC18           |
|                                                                                           |
|                                                                                           |
|                                          IC36            IC37                             |
|             315-5598                                                       IC38           |
|                  CN5                                                                      |
| IC30  IC31                               IC39            IC40                             |
|                                                                CN6         IC41           |
|JP11-16      CN1                                     CN2                                   |
|-------------------------------------------------------------------------------------------|

Notes:
CN1/2/3/4 : Multi-pin connectors (below PCB) for connection of ROM Board to Main Board
CN5       : Custom Sega connector for connection of protection daughterboard (not used)
CN6       : Custom Sega connector (purpose unknown)
315-5598  : Lattice GAL16V8A (DIP20)

Jumpers: (to configure ROM sizes used on Title Fight. Other games may vary depending on ROM types used.)
JP1: 1-2
JP2: 1-2
JP3: 2-3
JP4: 2-3
JP5: 2-3
JP6: 1-2
JP7: 2-3
JP8: 2-3
JP9: 1-2
JP10: 1-2
JP11: OPEN
JP12: OPEN
JP13: OPEN
JP14: OPEN
JP15: 1-2
JP16: 2-3

ROM Locations (All known System Multi32 games shown for reference)
-------------
PCB Label     CPU P0    CPU P1    CPU D0    CPU D1    SOUND     SCR0      SCR1      OBJ0      OBJ1      OBJ2      OBJ3      OBJ4      OBJ5      OBJ6      OBJ7      PCM D0    PCM D1    PCM EP/S
IC#           IC37      IC40      IC36      IC39      IC31      IC3       IC11      IC14      IC22      IC15      IC23      IC10      IC18      IC38      IC41      IC1       IC2       IC30

IC Type       27C2048   27C2048   -         -         27C1000   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   -         -
Title Fight   EPR15388  EPR15389  -         -         EPR15384  MPR15381  MPR15382  MPR15379  MPR15380  MPR15375  MPR15376  MPR15371  MPR15372  MPR15373  MPR15374  MPR15385  -         -

IC Type       27C1024   27C1024   27C4200   27C4200   27C040    8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   -
OutRunners    EPR15618  EPR15619  EPR15538  EPR15539  EPR15550  MPR15548  MPR15549  MPR15540  MPR15541  MPR15542  MPR15543  MPR15544  MPR15545  MPR15546  MPR15547  MPR15551  MPR15552  -

IC Type       27C2048   27C2048   -         -         27C1000   534200    534200    8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   8316200   -
Hard Dunk     EPR16512  EPR16509  -         -         EPR16505  MPR16503  MPR16504  MPR16495  MPR16496  MPR16497  MPR16498  MPR16499  MPR16500  MPR16501  MPR16502  MPR16506  MPR16507  -
Hard Dunk (J) EPR16508  EPR16509  -         -         EPR16505  MPR16503  MPR16504  MPR16495  MPR16496  MPR16497  MPR16498  MPR16499  MPR16500  MPR16501  MPR16502  MPR16506  MPR16507  -

IC Type       27C2048   27C2048   27C4200   27C4200   27C1000   27C160    27C160    27C160    27C160    27C160    27C160    27C160    27C160    27C160    27C160    27C160    27C160    -
Stadium Cross EPR15093  EPR15094  EPR15018  EPR15019  EPR15192  EPR15020  EPR15021  EPR15022  EPR15023  EPR15024  EPR15025  EPR15026  EPR15027  EPR15028  EPR15029  EPR15031  EPR15032  -


***************************************************************************/

#include "driver.h"
#include "rendlay.h"
#include "segas32.h"
#include "machine/eeprom.h"
#include "sound/2612intf.h"
#include "sound/rf5c68.h"
#include "sound/multipcm.h"



/*************************************
 *
 *  Constants
 *
 *************************************/

#define MASTER_CLOCK		32215900
#define RFC_CLOCK			50000000
#define MULTI32_CLOCK		40000000

#define TIMER_0_CLOCK		((MASTER_CLOCK/2)/2048)	/* confirmed */
#define TIMER_1_CLOCK		((RFC_CLOCK/16)/256)	/* confirmed */

#define MAIN_IRQ_VBSTART	0
#define MAIN_IRQ_VBSTOP		1
#define MAIN_IRQ_SOUND		2
#define MAIN_IRQ_TIMER0		3
#define MAIN_IRQ_TIMER1		4

#define SOUND_IRQ_YM3438	0
#define SOUND_IRQ_V60		1



/*************************************
 *
 *  Global variables
 *
 *************************************/

UINT8 *ga2_dpram;
UINT16 *system32_workram;
UINT16 *system32_protram;



/*************************************
 *
 *  Statics
 *
 *************************************/

static const UINT8 *system32_default_eeprom;
static UINT8 *z80_shared_ram;

/* V60 interrupt controller */
static UINT8 v60_irq_control[0x10];
static emu_timer *v60_irq_timer[2];

/* sound interrupt controller */
static UINT8 sound_irq_control[4];
static UINT8 sound_irq_input;
static UINT8 sound_dummy_value;
static UINT16 sound_bank;

/* I/O chips and custom I/O */
static UINT8 misc_io_data[2][0x10];
static read16_machine_func custom_io_r[2];
static write16_machine_func custom_io_w[2];
static UINT8 analog_bank;
static UINT8 analog_value[4];
static UINT8 sonic_last[6];

static void (*system32_prot_vblank)(void);



/*************************************
 *
 *  Prototypes
 *
 *************************************/

static void signal_v60_irq(running_machine *machine, int data);
static TIMER_CALLBACK( signal_v60_irq_callback );
static void signal_sound_irq(running_machine *machine, int which);



/*************************************
 *
 *  Machine init
 *
 *************************************/

static MACHINE_RESET( system32 )
{
	/* initialize the interrupt controller */
	memset(v60_irq_control, 0xff, sizeof(v60_irq_control));

	/* allocate timers */
	v60_irq_timer[0] = timer_alloc(signal_v60_irq_callback, NULL);
	v60_irq_timer[1] = timer_alloc(signal_v60_irq_callback, NULL);

	/* clear IRQ lines */
	cpu_set_input_line(machine->cpu[0], 0, CLEAR_LINE);
}



/*************************************
 *
 *  Interrupt controller
 *
 *************************************/

static void update_irq_state(running_machine *machine)
{
	UINT8 effirq = v60_irq_control[7] & ~v60_irq_control[6] & 0x1f;
	int vector;

	/* loop over interrupt vectors, finding the highest priority one with */
	/* an unmasked interrupt pending */
	for (vector = 0; vector < 5; vector++)
		if (effirq & (1 << vector))
		{
			cpu_set_input_line_and_vector(machine->cpu[0], 0, ASSERT_LINE, vector);
			break;
		}

	/* if we didn't find any, clear the interrupt line */
	if (vector == 5)
		cpu_set_input_line(machine->cpu[0], 0, CLEAR_LINE);
}


static void signal_v60_irq(running_machine *machine, int which)
{
	int i;

	/* see if this interrupt input is mapped to any vectors; if so, mark them */
	for (i = 0; i < 5; i++)
		if (v60_irq_control[i] == which)
			v60_irq_control[7] |= 1 << i;
	update_irq_state(machine);
}


static TIMER_CALLBACK( signal_v60_irq_callback )
{
	signal_v60_irq(machine, param);
}


static void int_control_w(const address_space *space, int offset, UINT8 data)
{
	int duration;

//  logerror("%06X:int_control_w(%X) = %02X\n", cpu_get_pc(machine->activecpu), offset, data);
	switch (offset)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:			/* vectors */
			v60_irq_control[offset] = data;
			break;

		case 5:			/* unknown */
			v60_irq_control[offset] = data;
			break;

		case 6:			/* mask */
			v60_irq_control[offset] = data;
			update_irq_state(space->machine);
			break;

		case 7:			/* acknowledge */
			v60_irq_control[offset] &= data;
			update_irq_state(space->machine);
			break;

		case 8:
		case 9:			/* timer 0 count */
			v60_irq_control[offset] = data;
			duration = v60_irq_control[8] + ((v60_irq_control[9] << 8) & 0xf00);
			if (duration)
			{
				attotime period = attotime_make(0, attotime_to_attoseconds(ATTOTIME_IN_HZ(TIMER_0_CLOCK)) * duration);
				timer_adjust_oneshot(v60_irq_timer[0], period, MAIN_IRQ_TIMER0);
			}
			break;

		case 10:
		case 11:		/* timer 1 count */
			v60_irq_control[offset] = data;
			duration = v60_irq_control[10] + ((v60_irq_control[11] << 8) & 0xf00);
			if (duration)
			{
				attotime period = attotime_make(0, attotime_to_attoseconds(ATTOTIME_IN_HZ(TIMER_1_CLOCK)) * duration);
				timer_adjust_oneshot(v60_irq_timer[1], period, MAIN_IRQ_TIMER1);
			}
			break;

		case 12:
		case 13:
		case 14:
		case 15:		/* signal IRQ to sound CPU */
			signal_sound_irq(space->machine, SOUND_IRQ_V60);
			break;
	}
}


static READ16_HANDLER( interrupt_control_16_r )
{
	switch (offset)
	{
		case 8/2:
			/* fix me - should return timer count down value */
			break;

		case 10/2:
			/* fix me - should return timer count down value */
			break;
	}

	/* return all F's for everything except timer values */
	return 0xffff;
}


static WRITE16_HANDLER( interrupt_control_16_w )
{
	if (ACCESSING_BITS_0_7)
		int_control_w(space, offset*2+0, data);
	if (ACCESSING_BITS_8_15)
		int_control_w(space, offset*2+1, data >> 8);
}


static READ32_HANDLER( interrupt_control_32_r )
{
	switch (offset)
	{
		case 8/4:
			/* fix me - should return timer count down value */
			break;
	}

	/* return all F's for everything except timer values */
	return 0xffffffff;
}


static WRITE32_HANDLER( interrupt_control_32_w )
{
	if (ACCESSING_BITS_0_7)
		int_control_w(space, offset*4+0, data);
	if (ACCESSING_BITS_8_15)
		int_control_w(space, offset*4+1, data >> 8);
	if (ACCESSING_BITS_16_23)
		int_control_w(space, offset*4+2, data >> 16);
	if (ACCESSING_BITS_24_31)
		int_control_w(space, offset*4+3, data >> 24);
}


static TIMER_CALLBACK( end_of_vblank_int )
{
	signal_v60_irq(machine, MAIN_IRQ_VBSTOP);
	system32_set_vblank(0);
}


static INTERRUPT_GEN( start_of_vblank_int )
{
	signal_v60_irq(device->machine, MAIN_IRQ_VBSTART);
	system32_set_vblank(1);
	timer_set(video_screen_get_time_until_pos(device->machine->primary_screen, 0, 0), NULL, 0, end_of_vblank_int);
	if (system32_prot_vblank)
		(*system32_prot_vblank)();
}



/*************************************
 *
 *  I/O chip
 *
 *************************************/

static UINT16 common_io_chip_r(const address_space *space, int which, offs_t offset, UINT16 mem_mask)
{
	static const char *const portnames[2][8] =
			{
				{ "P1_A", "P2_A", "PORTC_A", "PORTD_A", "SERVICE12_A", "SERVICE34_A", "PORTG_A", "PORTH_A" },
				{ "P1_B", "P2_B", "PORTC_B", "PORTD_B", "SERVICE12_B", "SERVICE34_B", "PORTG_B", "PORTH_B" },
			};
	offset &= 0x1f/2;

	switch (offset)
	{
		/* I/O ports */
		case 0x00/2:
		case 0x02/2:
		case 0x04/2:
		case 0x06/2:
		case 0x08/2:
		case 0x0a/2:
		case 0x0c/2:
		case 0x0e/2:
			/* if the port is configured as an output, return the last thing written */
			if (misc_io_data[which][0x1e/2] & (1 << offset))
				return misc_io_data[which][offset];

			/* otherwise, return an input port */
			return input_port_read_safe(space->machine, portnames[which][offset], 0xffff);

		/* 'SEGA' protection */
		case 0x10/2:
			return 'S';
		case 0x12/2:
			return 'E';
		case 0x14/2:
			return 'G';
		case 0x16/2:
			return 'A';

		/* CNT register & mirror */
		case 0x18/2:
		case 0x1c/2:
			return misc_io_data[which][0x1c/2];

		/* port direction register & mirror */
		case 0x1a/2:
		case 0x1e/2:
			return misc_io_data[which][0x1e/2];
	}
	return 0xffff;
}


static void common_io_chip_w(const address_space *space, int which, offs_t offset, UINT16 data, UINT16 mem_mask)
{
	UINT8 old;

	/* only LSB matters */
	if (!ACCESSING_BITS_0_7)
		return;

	/* generic implementation */
	offset &= 0x1f/2;
	old = misc_io_data[which][offset];
	misc_io_data[which][offset] = data;

	switch (offset)
	{
		/* I/O ports */
		case 0x00/2:
		case 0x02/2:
		case 0x04/2:
		case 0x08/2:
		case 0x0a/2:
		case 0x0c/2:
			break;

		/* miscellaneous output */
		case 0x06/2:
			if (which == 0)
			{
				eeprom_write_bit(data & 0x80);
				eeprom_set_cs_line((data & 0x20) ? CLEAR_LINE : ASSERT_LINE);
				eeprom_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
			}
/*            coin_lockout_w(1 + 2*which, data & 0x08);
            coin_lockout_w(0 + 2*which, data & 0x04);*/
			coin_counter_w(1 + 2*which, data & 0x02);
			coin_counter_w(0 + 2*which, data & 0x01);
			break;

		/* tile banking */
		case 0x0e/2:
			if (which == 0)
				system32_tilebank_external = data;
			else
			{
				/* multi-32 EEPROM access */
				eeprom_write_bit(data & 0x80);
				eeprom_set_cs_line((data & 0x20) ? CLEAR_LINE : ASSERT_LINE);
				eeprom_set_clock_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
			}
			break;

		/* CNT register */
		case 0x1c/2:
			system32_displayenable[which] = (data & 0x02);
			if (which == 0)
				cpu_set_input_line(space->machine->cpu[1], INPUT_LINE_RESET, (data & 0x04) ? CLEAR_LINE : ASSERT_LINE);
			break;
	}
}


static READ16_HANDLER( io_chip_r )
{
	return common_io_chip_r(space, 0, offset, mem_mask);
}


static WRITE16_HANDLER( io_chip_w )
{
	common_io_chip_w(space, 0, offset, data, mem_mask);
}


static READ32_HANDLER( io_chip_0_r )
{
	return common_io_chip_r(space, 0, offset*2+0, mem_mask) |
	      (common_io_chip_r(space, 0, offset*2+1, mem_mask >> 16) << 16);
}


static WRITE32_HANDLER( io_chip_0_w )
{
	if (ACCESSING_BITS_0_15)
		common_io_chip_w(space, 0, offset*2+0, data, mem_mask);
	if (ACCESSING_BITS_16_31)
		common_io_chip_w(space, 0, offset*2+1, data >> 16, mem_mask >> 16);
}


static READ32_HANDLER( io_chip_1_r )
{
	return common_io_chip_r(space, 1, offset*2+0, mem_mask) |
	      (common_io_chip_r(space, 1, offset*2+1, mem_mask >> 16) << 16);
}


static WRITE32_HANDLER( io_chip_1_w )
{
	if (ACCESSING_BITS_0_15)
		common_io_chip_w(space, 1, offset*2+0, data, mem_mask);
	if (ACCESSING_BITS_16_31)
		common_io_chip_w(space, 1, offset*2+1, data >> 16, mem_mask >> 16);
}



/*************************************
 *
 *  I/O expansion range
 *
 *************************************/

static READ16_HANDLER( io_expansion_r )
{
	if (custom_io_r[0])
		return (*custom_io_r[0])(space, offset, mem_mask);
	else
		logerror("%06X:io_expansion_r(%X)\n", cpu_get_pc(space->cpu), offset);
	return 0xffff;
}


static WRITE16_HANDLER( io_expansion_w )
{
	/* only LSB matters */
	if (!ACCESSING_BITS_0_7)
		return;

	if (custom_io_w[0])
		(*custom_io_w[0])(space, offset, data, mem_mask);
	else
		logerror("%06X:io_expansion_w(%X) = %02X\n", cpu_get_pc(space->cpu), offset, data & 0xff);
}


static READ32_HANDLER( io_expansion_0_r )
{
	if (custom_io_r[0])
		return (*custom_io_r[0])(space, offset*2+0, mem_mask) |
			  ((*custom_io_r[0])(space, offset*2+1, mem_mask >> 16) << 16);
	else
		logerror("%06X:io_expansion_r(%X)\n", cpu_get_pc(space->cpu), offset);
	return 0xffffffff;
}


static WRITE32_HANDLER( io_expansion_0_w )
{
	/* only LSB matters */
	if (ACCESSING_BITS_0_7)
	{
		if (custom_io_w[0])
			(*custom_io_w[0])(space, offset*2+0, data, mem_mask);
		else
			logerror("%06X:io_expansion_w(%X) = %02X\n", cpu_get_pc(space->cpu), offset, data & 0xff);
	}
	if (ACCESSING_BITS_16_23)
	{
		if (custom_io_w[0])
			(*custom_io_w[0])(space, offset*2+1, data >> 16, mem_mask >> 16);
		else
			logerror("%06X:io_expansion_w(%X) = %02X\n", cpu_get_pc(space->cpu), offset, data & 0xff);
	}
}


static READ32_HANDLER( io_expansion_1_r )
{
	if (custom_io_r[1])
		return (*custom_io_r[1])(space, offset*2+0, mem_mask) |
			  ((*custom_io_r[1])(space, offset*2+1, mem_mask >> 16) << 16);
	else
		logerror("%06X:io_expansion_r(%X)\n", cpu_get_pc(space->cpu), offset);
	return 0xffffffff;
}


static WRITE32_HANDLER( io_expansion_1_w )
{
	/* only LSB matters */
	if (ACCESSING_BITS_0_7)
	{
		if (custom_io_w[1])
			(*custom_io_w[1])(space, offset*2+0, data, mem_mask);
		else
			logerror("%06X:io_expansion_w(%X) = %02X\n", cpu_get_pc(space->cpu), offset, data & 0xff);
	}
	if (ACCESSING_BITS_16_23)
	{
		if (custom_io_w[1])
			(*custom_io_w[1])(space, offset*2+1, data >> 16, mem_mask >> 16);
		else
			logerror("%06X:io_expansion_w(%X) = %02X\n", cpu_get_pc(space->cpu), offset, data & 0xff);
	}
}



/*************************************
 *
 *  Game-specific custom I/O
 *
 *************************************/

static READ16_HANDLER( analog_custom_io_r )
{
	UINT16 result;
	switch (offset)
	{
		case 0x10/2:
		case 0x12/2:
		case 0x14/2:
		case 0x16/2:
			result = analog_value[offset & 3] | 0x7f;
			analog_value[offset & 3] <<= 1;
			return result;
	}
	logerror("%06X:unknown analog_custom_io_r(%X) & %04X\n", cpu_get_pc(space->cpu), offset*2, mem_mask);
	return 0xffff;
}


static WRITE16_HANDLER( analog_custom_io_w )
{
	static const char *const names[] = { "ANALOG1", "ANALOG2", "ANALOG3", "ANALOG4" };
	switch (offset)
	{
		case 0x10/2:
		case 0x12/2:
		case 0x14/2:
		case 0x16/2:
			analog_value[offset & 3] = input_port_read_safe(space->machine, names[offset & 3], 0);
			return;
	}
	logerror("%06X:unknown analog_custom_io_w(%X) = %04X & %04X\n", cpu_get_pc(space->cpu), offset*2, data, mem_mask);
}


static READ16_HANDLER( extra_custom_io_r )
{
	static const char *const names[] = { "EXTRA1", "EXTRA2", "EXTRA3", "EXTRA4" };
	switch (offset)
	{
		case 0x20/2:
		case 0x22/2:
		case 0x24/2:
		case 0x26/2:
			return input_port_read_safe(space->machine, names[offset & 3], 0xffff);
	}

	logerror("%06X:unknown extra_custom_io_r(%X) & %04X\n", cpu_get_pc(space->cpu), offset*2, mem_mask);
	return 0xffff;
}


static WRITE16_HANDLER( orunners_custom_io_w )
{
	static const char *const names[] = { "ANALOG1", "ANALOG2", "ANALOG3", "ANALOG4", "ANALOG5", "ANALOG6", "ANALOG7", "ANALOG8" };
	switch (offset)
	{
		case 0x10/2:
		case 0x12/2:
		case 0x14/2:
		case 0x16/2:
			analog_value[offset & 3] = input_port_read_safe(space->machine, names[analog_bank * 4 + (offset & 3)], 0);
			return;

		case 0x20/2:
			analog_bank = data & 1;
			return;
	}
	logerror("%06X:unknown orunners_custom_io_w(%X) = %04X & %04X\n", cpu_get_pc(space->cpu), offset*2, data, mem_mask);
}


static READ16_HANDLER( sonic_custom_io_r )
{
	static const char *const names[] = { "TRACKX1", "TRACKY1", "TRACKX2", "TRACKY2", "TRACKX3", "TRACKY3" };

	switch (offset)
	{
		case 0x00/2:
		case 0x04/2:
		case 0x08/2:
		case 0x0c/2:
		case 0x10/2:
		case 0x14/2:
			return (UINT8)(input_port_read(space->machine, names[offset/2]) - sonic_last[offset/2]);
	}

	logerror("%06X:unknown sonic_custom_io_r(%X) & %04X\n", cpu_get_pc(space->cpu), offset*2, mem_mask);
	return 0xffff;
}


static WRITE16_HANDLER( sonic_custom_io_w )
{
	static const char *const names[] = { "TRACKX1", "TRACKY1", "TRACKX2", "TRACKY2", "TRACKX3", "TRACKY3" };

	switch (offset)
	{
		case 0x00/2:
		case 0x08/2:
		case 0x10/2:
			sonic_last[offset/2 + 0] = input_port_read(space->machine, names[offset/2 + 0]);
			sonic_last[offset/2 + 1] = input_port_read(space->machine, names[offset/2 + 1]);
			return;
	}

	logerror("%06X:unknown sonic_custom_io_w(%X) = %04X & %04X\n", cpu_get_pc(space->cpu), offset*2, data, mem_mask);
}



/*************************************
 *
 *  Random number generator
 *
 *************************************/

static WRITE16_HANDLER( random_number_16_w )
{
//  mame_printf_debug("%06X:random_seed_w(%04X) = %04X & %04X\n", cpu_get_pc(space->cpu), offset*2, data, mem_mask);
}

static READ16_HANDLER( random_number_16_r )
{
	return mame_rand(space->machine);
}

static WRITE32_HANDLER( random_number_32_w )
{
//  mame_printf_debug("%06X:random_seed_w(%04X) = %04X & %04X\n", cpu_get_pc(space->cpu), offset*2, data, mem_mask);
}

static READ32_HANDLER( random_number_32_r )
{
	return mame_rand(space->machine) ^ (mame_rand(space->machine) << 16);
}



/*************************************
 *
 *  Sound communications
 *
 *************************************/

static READ16_HANDLER( shared_ram_16_r )
{
	return z80_shared_ram[offset*2+0] | (z80_shared_ram[offset*2+1] << 8);
}


static WRITE16_HANDLER( shared_ram_16_w )
{
	if (ACCESSING_BITS_0_7)
		z80_shared_ram[offset*2+0] = data;
	if (ACCESSING_BITS_8_15)
		z80_shared_ram[offset*2+1] = data >> 8;
}


static READ32_HANDLER( shared_ram_32_r )
{
	return z80_shared_ram[offset*4+0] | (z80_shared_ram[offset*4+1] << 8) |
	      (z80_shared_ram[offset*4+2] << 16) | (z80_shared_ram[offset*4+3] << 24);
}


static WRITE32_HANDLER( shared_ram_32_w )
{
	if (ACCESSING_BITS_0_7)
		z80_shared_ram[offset*4+0] = data;
	if (ACCESSING_BITS_8_15)
		z80_shared_ram[offset*4+1] = data >> 8;
	if (ACCESSING_BITS_16_23)
		z80_shared_ram[offset*4+2] = data >> 16;
	if (ACCESSING_BITS_24_31)
		z80_shared_ram[offset*4+3] = data >> 24;
}



/*************************************
 *
 *  Sound interrupt controller
 *
 *************************************/

static void update_sound_irq_state(running_machine *machine)
{
	UINT8 effirq = sound_irq_input & ~sound_irq_control[3] & 0x07;
	int vector;

	/* loop over interrupt vectors, finding the highest priority one with */
	/* an unmasked interrupt pending */
	for (vector = 0; vector < 3; vector++)
		if (effirq & (1 << vector))
		{
			cpu_set_input_line_and_vector(machine->cpu[1], 0, ASSERT_LINE, 2 * vector);
			break;
		}

	/* if we didn't find any, clear the interrupt line */
	if (vector == 3)
		cpu_set_input_line(machine->cpu[1], 0, CLEAR_LINE);
}


static void signal_sound_irq(running_machine *machine, int which)
{
	int i;

	/* see if this interrupt input is mapped to any vectors; if so, mark them */
	for (i = 0; i < 3; i++)
		if (sound_irq_control[i] == which)
			sound_irq_input |= 1 << i;
	update_sound_irq_state(machine);
}


static void clear_sound_irq(running_machine *machine, int which)
{
	int i;
	for (i = 0; i < 3; i++)
		if (sound_irq_control[i] == which)
			sound_irq_input &= ~(1 << i);
	update_sound_irq_state(machine);
}


static WRITE8_HANDLER( sound_int_control_lo_w )
{
	/* odd offsets are interrupt acks */
	if (offset & 1)
	{
		sound_irq_input &= data;
		update_sound_irq_state(space->machine);
	}

	/* high offsets signal an IRQ to the v60 */
	if (offset & 4)
		signal_v60_irq(space->machine, MAIN_IRQ_SOUND);
}


static WRITE8_HANDLER( sound_int_control_hi_w )
{
	sound_irq_control[offset] = data;
	update_sound_irq_state(space->machine);
}


static void ym3438_irq_handler(running_machine *machine, int state)
{
	if (state)
		signal_sound_irq(machine, SOUND_IRQ_YM3438);
	else
		clear_sound_irq(machine, SOUND_IRQ_YM3438);
}



/*************************************
 *
 *  Sound banking
 *
 *************************************/

static WRITE8_HANDLER( sound_bank_lo_w )
{
	sound_bank = (sound_bank & ~0x3f) | (data & 0x3f);
	memory_set_bankptr(1, memory_region(space->machine, "sound") + 0x100000 + 0x2000 * sound_bank);
}


static WRITE8_HANDLER( sound_bank_hi_w )
{
	sound_bank = (sound_bank & 0x3f) | ((data & 0x04) << 4) | ((data & 0x03) << 7);
	memory_set_bankptr(1, memory_region(space->machine, "sound") + 0x100000 + 0x2000 * sound_bank);
}


static WRITE8_HANDLER( multipcm_bank_w )
{
	multi_pcm_set_bank(0, 0x80000 * ((data >> 3) & 7), 0x80000 * (data & 7));
}


static WRITE8_HANDLER( scross_bank_w )
{
	multi_pcm_set_bank(0, 0x80000 * (data & 7), 0x80000 * (data & 7));
}



/*************************************
 *
 *  Sound hack (not protection)
 *
 *************************************/

static READ8_HANDLER( sound_dummy_r )
{
	return sound_dummy_value;
}


static WRITE8_HANDLER( sound_dummy_w )
{
	sound_dummy_value = data;
}



/*************************************
 *
 *  NVRAM handler
 *
 *************************************/

static NVRAM_HANDLER( system32 )
{
	if (read_or_write)
		eeprom_save(file);
	else
	{
		eeprom_init(&eeprom_interface_93C46);

		if (file)
			eeprom_load(file);
		else if (system32_default_eeprom != NULL)
			eeprom_set_data(system32_default_eeprom, 0x80);
	}
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( system32_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x000000, 0x1fffff) AM_ROM
	AM_RANGE(0x200000, 0x20ffff) AM_MIRROR(0x0f0000) AM_RAM	AM_BASE(&system32_workram)
	AM_RANGE(0x300000, 0x31ffff) AM_MIRROR(0x0e0000) AM_READWRITE(system32_videoram_r, system32_videoram_w) AM_BASE(&system32_videoram)
	AM_RANGE(0x400000, 0x41ffff) AM_MIRROR(0x0e0000) AM_READWRITE(system32_spriteram_r, system32_spriteram_w) AM_BASE(&system32_spriteram)
	AM_RANGE(0x500000, 0x50000f) AM_MIRROR(0x0ffff0) AM_READWRITE(system32_sprite_control_r, system32_sprite_control_w)
	AM_RANGE(0x600000, 0x60ffff) AM_MIRROR(0x0e0000) AM_READWRITE(system32_paletteram_r, system32_paletteram_w) AM_BASE(&system32_paletteram[0])
	AM_RANGE(0x610000, 0x61007f) AM_MIRROR(0x0eff80) AM_READWRITE(system32_mixer_r, system32_mixer_w)
	AM_RANGE(0x700000, 0x701fff) AM_MIRROR(0x0fe000) AM_READWRITE(shared_ram_16_r, shared_ram_16_w)
	AM_RANGE(0xc00000, 0xc0001f) AM_MIRROR(0x0fff80) AM_READWRITE(io_chip_r, io_chip_w)
	AM_RANGE(0xc00040, 0xc0007f) AM_MIRROR(0x0fff80) AM_READWRITE(io_expansion_r, io_expansion_w)
	AM_RANGE(0xd00000, 0xd0000f) AM_MIRROR(0x07fff0) AM_READWRITE(interrupt_control_16_r, interrupt_control_16_w)
	AM_RANGE(0xd80000, 0xdfffff) AM_READWRITE(random_number_16_r, random_number_16_w)
	AM_RANGE(0xf00000, 0xffffff) AM_ROM AM_REGION("main", 0)
ADDRESS_MAP_END


static ADDRESS_MAP_START( multi32_map, ADDRESS_SPACE_PROGRAM, 32 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xffffff)
	AM_RANGE(0x000000, 0x1fffff) AM_ROM
	AM_RANGE(0x200000, 0x21ffff) AM_MIRROR(0x0e0000) AM_RAM
	AM_RANGE(0x300000, 0x31ffff) AM_MIRROR(0x0e0000) AM_READWRITE(multi32_videoram_r, multi32_videoram_w) AM_BASE((UINT32 **)&system32_videoram)
	AM_RANGE(0x400000, 0x41ffff) AM_MIRROR(0x0e0000) AM_READWRITE(multi32_spriteram_r, multi32_spriteram_w) AM_BASE((UINT32 **)&system32_spriteram)
	AM_RANGE(0x500000, 0x50000f) AM_MIRROR(0x0ffff0) AM_READWRITE(multi32_sprite_control_r, multi32_sprite_control_w)
	AM_RANGE(0x600000, 0x60ffff) AM_MIRROR(0x060000) AM_READWRITE(multi32_paletteram_0_r, multi32_paletteram_0_w) AM_BASE((UINT32 **)&system32_paletteram[0])
	AM_RANGE(0x610000, 0x61007f) AM_MIRROR(0x06ff80) AM_WRITE(multi32_mixer_0_w)
	AM_RANGE(0x680000, 0x68ffff) AM_MIRROR(0x060000) AM_READWRITE(multi32_paletteram_1_r, multi32_paletteram_1_w) AM_BASE((UINT32 **)&system32_paletteram[1])
	AM_RANGE(0x690000, 0x69007f) AM_MIRROR(0x06ff80) AM_WRITE(multi32_mixer_1_w)
	AM_RANGE(0x700000, 0x701fff) AM_MIRROR(0x0fe000) AM_READWRITE(shared_ram_32_r, shared_ram_32_w)
	AM_RANGE(0xc00000, 0xc0001f) AM_MIRROR(0x07ff80) AM_READWRITE(io_chip_0_r, io_chip_0_w)
	AM_RANGE(0xc00040, 0xc0007f) AM_MIRROR(0x07ff80) AM_READWRITE(io_expansion_0_r, io_expansion_0_w)
	AM_RANGE(0xc80000, 0xc8001f) AM_MIRROR(0x07ff80) AM_READWRITE(io_chip_1_r, io_chip_1_w)
	AM_RANGE(0xc80040, 0xc8007f) AM_MIRROR(0x07ff80) AM_READWRITE(io_expansion_1_r, io_expansion_1_w)
	AM_RANGE(0xd00000, 0xd0000f) AM_MIRROR(0x07fff0) AM_READWRITE(interrupt_control_32_r, interrupt_control_32_w)
	AM_RANGE(0xd80000, 0xdfffff) AM_READWRITE(random_number_32_r, random_number_32_w)
	AM_RANGE(0xf00000, 0xffffff) AM_ROM AM_REGION("main", 0)
ADDRESS_MAP_END



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( system32_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x9fff) AM_ROM AM_REGION("sound", 0x100000)
	AM_RANGE(0xa000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xc000, 0xc00f) AM_MIRROR(0x0ff0) AM_WRITE(rf5c68_reg_w)
	AM_RANGE(0xd000, 0xdfff) AM_READWRITE(rf5c68_r, rf5c68_w)
	AM_RANGE(0xe000, 0xffff) AM_RAM AM_BASE(&z80_shared_ram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( system32_sound_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x80, 0x80) AM_MIRROR(0x0c) AM_READWRITE(ym3438_status_port_0_a_r, ym3438_control_port_0_a_w)
	AM_RANGE(0x81, 0x81) AM_MIRROR(0x0c) AM_WRITE(ym3438_data_port_0_a_w)
	AM_RANGE(0x82, 0x82) AM_MIRROR(0x0c) AM_WRITE(ym3438_control_port_0_b_w)
	AM_RANGE(0x83, 0x83) AM_MIRROR(0x0c) AM_WRITE(ym3438_data_port_0_b_w)
	AM_RANGE(0x90, 0x90) AM_MIRROR(0x0c) AM_READWRITE(ym3438_status_port_1_a_r, ym3438_control_port_1_a_w)
	AM_RANGE(0x91, 0x91) AM_MIRROR(0x0c) AM_WRITE(ym3438_data_port_1_a_w)
	AM_RANGE(0x92, 0x92) AM_MIRROR(0x0c) AM_WRITE(ym3438_control_port_1_b_w)
	AM_RANGE(0x93, 0x93) AM_MIRROR(0x0c) AM_WRITE(ym3438_data_port_1_b_w)
	AM_RANGE(0xa0, 0xaf) AM_WRITE(sound_bank_lo_w)
	AM_RANGE(0xb0, 0xbf) AM_WRITE(sound_bank_hi_w)
	AM_RANGE(0xc0, 0xcf) AM_WRITE(sound_int_control_lo_w)
	AM_RANGE(0xd0, 0xd3) AM_MIRROR(0x04) AM_WRITE(sound_int_control_hi_w)
	AM_RANGE(0xf1, 0xf1) AM_READWRITE(sound_dummy_r, sound_dummy_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( multi32_sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x9fff) AM_ROM AM_REGION("sound", 0x100000)
	AM_RANGE(0xa000, 0xbfff) AM_ROMBANK(1)
	AM_RANGE(0xc000, 0xdfff) AM_READWRITE(multi_pcm_reg_0_r, multi_pcm_reg_0_w)
	AM_RANGE(0xe000, 0xffff) AM_RAM AM_BASE(&z80_shared_ram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( multi32_sound_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x80, 0x80) AM_MIRROR(0x0c) AM_READWRITE(ym3438_status_port_0_a_r, ym3438_control_port_0_a_w)
	AM_RANGE(0x81, 0x81) AM_MIRROR(0x0c) AM_WRITE(ym3438_data_port_0_a_w)
	AM_RANGE(0x82, 0x82) AM_MIRROR(0x0c) AM_WRITE(ym3438_control_port_0_b_w)
	AM_RANGE(0x83, 0x83) AM_MIRROR(0x0c) AM_WRITE(ym3438_data_port_0_b_w)
	AM_RANGE(0xa0, 0xaf) AM_WRITE(sound_bank_lo_w)
	AM_RANGE(0xb0, 0xbf) AM_WRITE(multipcm_bank_w)
	AM_RANGE(0xc0, 0xcf) AM_WRITE(sound_int_control_lo_w)
	AM_RANGE(0xd0, 0xd3) AM_MIRROR(0x04) AM_WRITE(sound_int_control_hi_w)
	AM_RANGE(0xf1, 0xf1) AM_READWRITE(sound_dummy_r, sound_dummy_w)
ADDRESS_MAP_END



/*************************************
 *
 *  GA2 Protection CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( ga2_v25_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x00000, 0x0ffff) AM_ROM
	AM_RANGE(0x10000, 0x1ffff) AM_RAM AM_BASE(&ga2_dpram)
	AM_RANGE(0xf0000, 0xfffff) AM_ROM AM_REGION("mcu", 0)
ADDRESS_MAP_END



/*************************************
 *
 *  Generic port definitions
 *
 *************************************/

static INPUT_PORTS_START( system32_generic )
	PORT_START("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY

	PORT_START("P2_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)

	PORT_START("PORTC_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTD_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("SERVICE12_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SERVICE34_A")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE3 )	/* sometimes mirrors SERVICE1 */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE4 )	/* tends to also work as a test switch */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(eeprom_bit_r, NULL)

	PORT_START("PORTG_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTH_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START( multi32_generic )
	PORT_INCLUDE( system32_generic )

	PORT_START("P1_B")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P2_B")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTC_B")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTD_B")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("SERVICE12_B")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SERVICE34_B")
	PORT_BIT( 0x7f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(eeprom_bit_r, NULL)

	PORT_START("PORTG_B")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("PORTH_B")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



/*************************************
 *
 *  Game-specific port definitions
 *
 *************************************/

static INPUT_PORTS_START( arescue )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x38, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE34_A")
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_REVERSE

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Z ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( alien3 )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START("ANALOG4")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( arabfgt )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("EXTRA1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)

	PORT_START("EXTRA2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)

	PORT_START("EXTRA3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( arabfgtu )
	PORT_INCLUDE( arabfgt )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN3 )

	PORT_MODIFY("EXTRA3")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
INPUT_PORTS_END


static INPUT_PORTS_START( brival )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("EXTRA2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_PLAYER(1)
INPUT_PORTS_END


static INPUT_PORTS_START( darkedge )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)

	PORT_START("EXTRA2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1)
INPUT_PORTS_END


static INPUT_PORTS_START( dbzvrvs )
	PORT_INCLUDE( system32_generic )
INPUT_PORTS_END


static INPUT_PORTS_START( f1en )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Gear Up")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_CODE(KEYCODE_LSHIFT) PORT_NAME("Gear Down")
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE34_A")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
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
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( f1lap )
	PORT_INCLUDE( f1en )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Gear Up")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_CODE(KEYCODE_LSHIFT) PORT_NAME("Gear Down")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_CODE(KEYCODE_Z)
INPUT_PORTS_END


static INPUT_PORTS_START( ga2 )
	PORT_INCLUDE( system32_generic )

	PORT_START("EXTRA1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)

	PORT_START("EXTRA2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(4)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)

	PORT_START("EXTRA3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( ga2u )
	PORT_INCLUDE( ga2 )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN3 )

	PORT_MODIFY("EXTRA3")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
INPUT_PORTS_END


static INPUT_PORTS_START( harddunk )
	PORT_INCLUDE( multi32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("P1_B")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(4)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)

	PORT_MODIFY("P2_B")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(5)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(5)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(5)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(5)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(5)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(5)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(5)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(5)

	PORT_MODIFY("SERVICE12_B")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START5 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("EXTRA1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)

	PORT_START("EXTRA2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(6)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(6)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(6)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(6)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(6)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(6)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(6)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(6)

	PORT_START("EXTRA3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START6 )
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static INPUT_PORTS_START( holo )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START( jpark )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START("ANALOG4")
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( orunners )
	PORT_INCLUDE( multi32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_SPACE)	/* shift up */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_LSHIFT)	/* shift down */
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_Z)		/* DJ/music */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_X)		/* << */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_C)		/* >> */
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("P1_B")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_CODE(KEYCODE_Q)		/* shift up */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_CODE(KEYCODE_W)		/* shift down */
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("P2_B")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)								/* DJ/music */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)								/* << */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2)								/* >> */
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("ANALOG4")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START("ANALOG7")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START("ANALOG8")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( radm )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Light")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_CODE(KEYCODE_LSHIFT) PORT_NAME("Wiper")
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE34_A")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
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
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( radr )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Gear Change") PORT_TOGGLE
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE34_A")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Transmission" )
	PORT_DIPSETTING(    0x08, "Manual" )
	PORT_DIPSETTING(    0x00, "Automatic" )
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( scross )
	PORT_INCLUDE( multi32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_SPACE)	/*  */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_LSHIFT)	/*  */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_CODE(KEYCODE_LALT)		/* brake */
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("P1_B")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_CODE(KEYCODE_Q)		/*  */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_CODE(KEYCODE_W)		/*  */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_CODE(KEYCODE_S)		/* brake */
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(2)

	PORT_START("ANALOG4")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( slipstrm )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Gear Change") PORT_TOGGLE
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE34_A")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
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
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("ANALOG1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)

	PORT_START("ANALOG3")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(30) PORT_KEYDELTA(10)
INPUT_PORTS_END


static INPUT_PORTS_START( sonic )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START3 )

	PORT_START("TRACKX1")
	PORT_BIT( 0xff, 0, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_REVERSE PORT_PLAYER(1)

	PORT_START("TRACKY1")
	PORT_BIT( 0xff, 0, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_PLAYER(1)

	PORT_START("TRACKX2")
	PORT_BIT( 0xff, 0, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_REVERSE PORT_PLAYER(2)

	PORT_START("TRACKY2")
	PORT_BIT( 0xff, 0, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_PLAYER(2)

	PORT_START("TRACKX3")
	PORT_BIT( 0xff, 0, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_REVERSE PORT_PLAYER(3)

	PORT_START("TRACKY3")
	PORT_BIT( 0xff, 0, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_PLAYER(3)
INPUT_PORTS_END


static INPUT_PORTS_START( spidman )
	PORT_INCLUDE( system32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("EXTRA1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)

	PORT_START("EXTRA2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x0c, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)

	PORT_START("EXTRA3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START( spidmanu )
	PORT_INCLUDE( spidman )

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN3 )

	PORT_MODIFY("EXTRA3")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
INPUT_PORTS_END


static INPUT_PORTS_START( svf )
	PORT_INCLUDE( system32_generic )
INPUT_PORTS_END


static INPUT_PORTS_START( titlef )
	PORT_INCLUDE( multi32_generic )

	PORT_MODIFY("P1_A")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_PLAYER(1)

	PORT_MODIFY("P2_A")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_PLAYER(1)

	PORT_MODIFY("SERVICE12_A")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("P1_B")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKLEFT_LEFT ) PORT_PLAYER(2)

	PORT_MODIFY("P2_B")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICKRIGHT_LEFT ) PORT_PLAYER(2)

	PORT_MODIFY("SERVICE12_B")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout bgcharlayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 0, 4, 16, 20, 8, 12, 24, 28, 32, 36, 48, 52, 40, 44, 56, 60  },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
	  8*64, 9*64,10*64,11*64,12*64,13*64,14*64,15*64 },
	16*64
};


static GFXDECODE_START( segas32 )
	GFXDECODE_ENTRY( "gfx1", 0, bgcharlayout,   0x00, 0x3ff  )
GFXDECODE_END



/*************************************
 *
 *  Sound interfaces
 *
 *************************************/

static const ym3438_interface ym3438_config =
{
	ym3438_irq_handler
};



/*************************************
 *
 *  Dual PCB shared memory comms
 *
 *************************************/

// Both arescue and f1en appear to use an identical shared RAM system.

static UINT16* dual_pcb_comms;

static WRITE16_HANDLER( dual_pcb_comms_w )
{
	COMBINE_DATA(&dual_pcb_comms[offset]);
}

static READ16_HANDLER( dual_pcb_comms_r )
{
	return dual_pcb_comms[offset];
}


/* There must be something on the comms board for this?
   Probably not a dip/solder link/trace cut, but maybe
   just whichever way the cables are plugged in?
   Both f1en and arescue master units try to set bit 1... */
static READ16_HANDLER( dual_pcb_masterslave )
{
	return 0; // 0/1 master/slave
}



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( system32 )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", V60, MASTER_CLOCK/2)
	MDRV_CPU_PROGRAM_MAP(system32_map,0)
	MDRV_CPU_VBLANK_INT("main", start_of_vblank_int)

	MDRV_CPU_ADD("sound", Z80, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(system32_sound_map,0)
	MDRV_CPU_IO_MAP(system32_sound_portmap,0)

	MDRV_MACHINE_RESET(system32)
	MDRV_NVRAM_HANDLER(system32)

	/* video hardware */
	MDRV_GFXDECODE(segas32)
	MDRV_PALETTE_LENGTH(0x4000)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_SIZE(52*8, 262)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 52*8-1, 0*8, 28*8-1)

	MDRV_VIDEO_START(system32)
	MDRV_VIDEO_UPDATE(system32)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD("ym1", YM3438, MASTER_CLOCK/4)
	MDRV_SOUND_CONFIG(ym3438_config)
	MDRV_SOUND_ROUTE(0, "left", 0.40)
	MDRV_SOUND_ROUTE(1, "right", 0.40)

	MDRV_SOUND_ADD("ym2", YM3438, MASTER_CLOCK/4)
	MDRV_SOUND_ROUTE(0, "left", 0.40)
	MDRV_SOUND_ROUTE(1, "right", 0.40)

	MDRV_SOUND_ADD("rf", RF5C68, RFC_CLOCK/4)
	MDRV_SOUND_ROUTE(0, "left", 0.55)
	MDRV_SOUND_ROUTE(1, "right", 0.55)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( system32_v25 )
	MDRV_IMPORT_FROM(system32)

	/* add a V25 for protection */
	MDRV_CPU_ADD("mcu", V25, 10000000)
	MDRV_CPU_PROGRAM_MAP(ga2_v25_map,0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( multi32 )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", V70, MULTI32_CLOCK/2)
	MDRV_CPU_PROGRAM_MAP(multi32_map,0)
	MDRV_CPU_VBLANK_INT("left", start_of_vblank_int)

	MDRV_CPU_ADD("sound", Z80, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(multi32_sound_map,0)
	MDRV_CPU_IO_MAP(multi32_sound_portmap,0)

	MDRV_MACHINE_RESET(system32)
	MDRV_NVRAM_HANDLER(system32)

	/* video hardware */
	MDRV_GFXDECODE(segas32)
	MDRV_PALETTE_LENGTH(0x8000)
	MDRV_DEFAULT_LAYOUT(layout_dualhsxs)

	MDRV_SCREEN_ADD("left", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_SIZE(52*8, 262)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 52*8-1, 0*8, 28*8-1)

	MDRV_SCREEN_ADD("right", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_SIZE(52*8, 262)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 52*8-1, 0*8, 28*8-1)

	MDRV_VIDEO_START(multi32)
	MDRV_VIDEO_UPDATE(multi32)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD("ym", YM3438, MASTER_CLOCK/4)
	MDRV_SOUND_CONFIG(ym3438_config)
	MDRV_SOUND_ROUTE(1, "left", 0.40)
	MDRV_SOUND_ROUTE(0, "right", 0.40)

	MDRV_SOUND_ADD("sega", MULTIPCM, MASTER_CLOCK/4)
	MDRV_SOUND_ROUTE(1, "left", 1.0)
	MDRV_SOUND_ROUTE(0, "right", 1.0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

#define ROM_LOAD_x2(name, base, length, crc) \
	ROM_LOAD( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 1 * length, length )

#define ROM_LOAD_x4(name, base, length, crc) \
	ROM_LOAD( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 1 * length, length ) \
	ROM_RELOAD(     base + 2 * length, length ) \
	ROM_RELOAD(     base + 3 * length, length )

#define ROM_LOAD_x8(name, base, length, crc) \
	ROM_LOAD( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 1 * length, length ) \
	ROM_RELOAD(     base + 2 * length, length ) \
	ROM_RELOAD(     base + 3 * length, length ) \
	ROM_RELOAD(     base + 4 * length, length ) \
	ROM_RELOAD(     base + 5 * length, length ) \
	ROM_RELOAD(     base + 6 * length, length ) \
	ROM_RELOAD(     base + 7 * length, length )

#define ROM_LOAD_x16(name, base, length, crc) \
	ROM_LOAD( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 1 * length, length ) \
	ROM_RELOAD(     base + 2 * length, length ) \
	ROM_RELOAD(     base + 3 * length, length ) \
	ROM_RELOAD(     base + 4 * length, length ) \
	ROM_RELOAD(     base + 5 * length, length ) \
	ROM_RELOAD(     base + 6 * length, length ) \
	ROM_RELOAD(     base + 7 * length, length ) \
	ROM_RELOAD(     base + 8 * length, length ) \
	ROM_RELOAD(     base + 9 * length, length ) \
	ROM_RELOAD(     base + 10 * length, length ) \
	ROM_RELOAD(     base + 11 * length, length ) \
	ROM_RELOAD(     base + 12 * length, length ) \
	ROM_RELOAD(     base + 13 * length, length ) \
	ROM_RELOAD(     base + 14 * length, length ) \
	ROM_RELOAD(     base + 15 * length, length )

#define ROM_LOAD16_BYTE_x2(name, base, length, crc) \
	ROM_LOAD16_BYTE( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 2 * length, length )

#define ROM_LOAD16_BYTE_x4(name, base, length, crc) \
	ROM_LOAD16_BYTE( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 2 * length, length ) \
	ROM_RELOAD(     base + 4 * length, length ) \
	ROM_RELOAD(     base + 6 * length, length )

#define ROM_LOAD32_WORD_x2(name, base, length, crc) \
	ROM_LOAD32_WORD( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 2 * length, length )

#define ROM_LOAD32_WORD_x4(name, base, length, crc) \
	ROM_LOAD32_WORD( name, base + 0 * length, length, crc ) \
	ROM_RELOAD(     base + 2 * length, length ) \
	ROM_RELOAD(     base + 4 * length, length ) \
	ROM_RELOAD(     base + 6 * length, length )



/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Air Rescue
    requires 2 linked system32 pcbs
    requires additional math DSP to be emulated
*/
ROM_START( arescue )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr14540.13",     0x000000, 0x020000, CRC(c2b4e5d0) SHA1(69f8ddded5095df9012663d0ded61b78f1692a8d) )
	ROM_LOAD_x4( "epr14539.6",      0x080000, 0x020000, CRC(1a1b5532) SHA1(f3651470222036703b7ecedb6e91e4cdb3d20df7) )
	ROM_LOAD16_BYTE( "epr14509.14", 0x100000, 0x080000, CRC(daa5a356) SHA1(ca87242c59de5ab5f9406635bee758a855fe20bc) )
	ROM_LOAD16_BYTE( "epr14508.7",  0x100001, 0x080000, CRC(6702c14d) SHA1(dc9324f16a3e3238f5ccdade9451d6823a50b563) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr14513.35", 0x100000, 0x40000, CRC(f9a884cd) SHA1(73010fff5e0257355e08e78838c74af86ed364ce) )
	ROM_LOAD_x2( "mpr14512.31", 0x200000, 0x80000, CRC(9da48051) SHA1(2d41148d089a75362ed0fde577eca919213ac666) )
	ROM_LOAD_x2( "mpr14511.26", 0x300000, 0x80000, CRC(074c53cc) SHA1(9c89843bbe8058123c25b7f8f86de754ddbca2bb) )
	ROM_LOAD_x2( "mpr14510.22", 0x400000, 0x80000, CRC(5ea6d52d) SHA1(d424082468940bb91ab424ac7812839792ed4e88) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "mpr14496.25", 0x000003, 0x080000, CRC(737da16c) SHA1(52247d9bc2924e90d040bef484a541b1f4a9026f) )
	ROM_LOAD32_BYTE( "mpr14497.29", 0x000001, 0x080000, CRC(ebd7ed17) SHA1(2307dc28501965432d2ff55a21698efdce014401) )
	ROM_LOAD32_BYTE( "mpr14498.34", 0x000002, 0x080000, CRC(d4a764bd) SHA1(8434a9225ed1e01e8b1cfe169268e42cd3ce6ee3) )
	ROM_LOAD32_BYTE( "mpr14499.38", 0x000000, 0x080000, CRC(fadc4b2b) SHA1(01c02a4dfad1ab19bac8b81b61d37fdc035bc5c5) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14500.24", 0x000007, 0x100000, CRC(0a064e9b) SHA1(264761f4aacaeeac9426528caf180404cd7f6e18) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14501.28", 0x000006, 0x100000, CRC(4662bb41) SHA1(80774e680468e9ba9c5dd5eeaa4791fa3b3722fd) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14502.33", 0x000005, 0x100000, CRC(988555a9) SHA1(355e44319fd51358329cc7cd226e4c4725e045cb) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14503.37", 0x000004, 0x100000, CRC(90556aca) SHA1(24df62af55048db66d50c7034c5460330d231bf5) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14504.23", 0x000003, 0x100000, CRC(46dd038d) SHA1(9530a52e2e7388437c20ebcb19bf84c8b3b5086b) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14505.27", 0x000002, 0x100000, CRC(be142c1f) SHA1(224631e00c2458c39c6a2ef7978c2b1131fb4da2) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14506.32", 0x000001, 0x100000, CRC(5dd8fb6b) SHA1(7d21cacb2c9dba5db2547b6d8e89397e0424ee8e) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14507.36", 0x000000, 0x100000, CRC(db3f59ec) SHA1(96dcb3827354773fc2911c62260a27e90dcbe96a) , ROM_SKIP(7) )

	ROM_REGION( 0x20000, "user2", 0 ) /* NEC uPD77P25 DSP Internal ROM */
	ROM_LOAD( "d7725.01", 0x000000, 0x002800, CRC(a7ec5644) SHA1(e9b05c70b639ee289e557dfd9a6c724b36338e2b) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Alien 3: The Gun
    not protected
    Rom PCB No: 837-9878-02
    Main PCB: 837-7428-03
*/
ROM_START( alien3 )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "15943.bin",     0x000000, 0x040000, CRC(ac4591aa) SHA1(677155a3ebdac6602525e06adb25d287eaf9e089) )
	ROM_LOAD_x2( "15942.bin",     0x080000, 0x040000, CRC(a1e1d0ec) SHA1(10d8d2235a67a4ba475fe98124c6a4a5311592b5) )
	ROM_LOAD16_BYTE( "15855.bin", 0x100000, 0x080000, CRC(a6fadabe) SHA1(328bbb54651eef197ba13f1bd9228f3f4de7ee5e) )
	ROM_LOAD16_BYTE( "15854.bin", 0x100001, 0x080000, CRC(d1aec392) SHA1(f48804fe0151e83ad45e912b55db8ae8ddebd2ad) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "15859.bin", 0x100000, 0x040000, CRC(91b55bd0) SHA1(23b85a006a91c2a5eb1cee14172fd0d8b7732518) )
	ROM_LOAD( "15858.bin",    0x200000, 0x100000, CRC(2eb64c10) SHA1(b2dbe86b82e889f4a9850cf4aa6596a139c1c3d6) )
	ROM_LOAD( "15857.bin",    0x300000, 0x100000, CRC(915c56df) SHA1(7031f937c826af17caf7ec8cbb6155d0a55bd38a) )
	ROM_LOAD( "15856.bin",    0x400000, 0x100000, CRC(a5ef4f1f) SHA1(e8da7a995955e80872a25bd75465c590b649cfab) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "15863.bin", 0x000000, 0x200000, CRC(9d36b645) SHA1(2977047780b615b64c3b4aec78fef0643d40490e) )
	ROM_LOAD16_BYTE( "15862.bin", 0x000001, 0x200000, CRC(9e277d25) SHA1(9f191484a42391268306a8d2d95c340ce8b2d6cd) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "15864.bin", 0x000000, 0x200000, CRC(58207157) SHA1(d1b0c7edac8b89b1322398d4cd3a976a88bc0b56) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15866.bin", 0x000002, 0x200000, CRC(9c53732c) SHA1(9aa5103cc10b4927c16e0cf102b64a15dd038756) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15868.bin", 0x000004, 0x200000, CRC(62d556e8) SHA1(d70cab0881784a3d4dd06d0c99587ca6054c2dc4) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15870.bin", 0x000006, 0x200000, CRC(d31c0400) SHA1(44c1b2e5236d894d31ff72552a7ad50270dd2fad) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15865.bin", 0x800000, 0x200000, CRC(dd64f87b) SHA1(cfa96c5f2b1221706552f5cef4aa7c61ebe21e39) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15867.bin", 0x800002, 0x200000, CRC(8cf9cb11) SHA1(a77399fccee3f258a5716721edd69a33f94f8daf) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15869.bin", 0x800004, 0x200000, CRC(dd4b137f) SHA1(7316dce32d35bf468defae5e6ed86910a37a2457) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15871.bin", 0x800006, 0x200000, CRC(58eb10ae) SHA1(23f2a72dc7b2d7b5c8a979952f81608296805745) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

ROM_START( alien3u )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "epr-15941.bin",  0x000000, 0x040000, CRC(bf8c257f) SHA1(d08b77d2e3f5af0da7e7d8727fbe7fc0eb1153ff) )
	ROM_LOAD_x2( "epr-15940a.bin", 0x080000, 0x040000, CRC(8840b51e) SHA1(0aa6945000676b1adc535b1557a1455d62aed9f5) )
	ROM_LOAD16_BYTE( "15855.bin",  0x100000, 0x080000, CRC(a6fadabe) SHA1(328bbb54651eef197ba13f1bd9228f3f4de7ee5e) )
	ROM_LOAD16_BYTE( "15854.bin",  0x100001, 0x080000, CRC(d1aec392) SHA1(f48804fe0151e83ad45e912b55db8ae8ddebd2ad) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "15859.bin", 0x100000, 0x040000, CRC(91b55bd0) SHA1(23b85a006a91c2a5eb1cee14172fd0d8b7732518) )
	ROM_LOAD( "15858.bin",    0x200000, 0x100000, CRC(2eb64c10) SHA1(b2dbe86b82e889f4a9850cf4aa6596a139c1c3d6) )
	ROM_LOAD( "15857.bin",    0x300000, 0x100000, CRC(915c56df) SHA1(7031f937c826af17caf7ec8cbb6155d0a55bd38a) )
	ROM_LOAD( "15856.bin",    0x400000, 0x100000, CRC(a5ef4f1f) SHA1(e8da7a995955e80872a25bd75465c590b649cfab) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "15863.bin", 0x000000, 0x200000, CRC(9d36b645) SHA1(2977047780b615b64c3b4aec78fef0643d40490e) )
	ROM_LOAD16_BYTE( "15862.bin", 0x000001, 0x200000, CRC(9e277d25) SHA1(9f191484a42391268306a8d2d95c340ce8b2d6cd) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "15864.bin", 0x000000, 0x200000, CRC(58207157) SHA1(d1b0c7edac8b89b1322398d4cd3a976a88bc0b56) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15866.bin", 0x000002, 0x200000, CRC(9c53732c) SHA1(9aa5103cc10b4927c16e0cf102b64a15dd038756) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15868.bin", 0x000004, 0x200000, CRC(62d556e8) SHA1(d70cab0881784a3d4dd06d0c99587ca6054c2dc4) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15870.bin", 0x000006, 0x200000, CRC(d31c0400) SHA1(44c1b2e5236d894d31ff72552a7ad50270dd2fad) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15865.bin", 0x800000, 0x200000, CRC(dd64f87b) SHA1(cfa96c5f2b1221706552f5cef4aa7c61ebe21e39) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15867.bin", 0x800002, 0x200000, CRC(8cf9cb11) SHA1(a77399fccee3f258a5716721edd69a33f94f8daf) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15869.bin", 0x800004, 0x200000, CRC(dd4b137f) SHA1(7316dce32d35bf468defae5e6ed86910a37a2457) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15871.bin", 0x800006, 0x200000, CRC(58eb10ae) SHA1(23f2a72dc7b2d7b5c8a979952f81608296805745) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Arabian Fight
    protected via a custom V20 with encrypted code
*/

ROM_START( arabfgt )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr14609.8",         0x000000, 0x020000, CRC(6a43c7fb) SHA1(70e9f9fa5f867f0455d62ff2690ad19055d79363) )
	ROM_LOAD16_BYTE_x2( "epr14592.18", 0x100000, 0x040000, CRC(f7dff316) SHA1(338690a1404dde6e7e66067f23605a247c7d0f5b) )
	ROM_LOAD16_BYTE_x2( "epr14591.9",  0x100001, 0x040000, CRC(bbd940fb) SHA1(99c17aba890935eaf7ea468492da03103288eb1b) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU + banks */
	ROM_LOAD_x8( "epr14596.36", 0x100000, 0x020000, CRC(bd01faec) SHA1(c909dcb8ef2672c4b0060d911d295e445ca311eb) )
	ROM_LOAD( "mpr14595f.35",   0x200000, 0x100000, CRC(5173d1af) SHA1(dccda644488d0c561c8ff7fa9619bd9504d8d9c6) )
	ROM_LOAD( "mpr14594f.34",   0x300000, 0x100000, CRC(01777645) SHA1(7bcbe7687bd80b94bd3b2b3099cdd036bf7e0cd3) )
	ROM_LOAD( "mpr14593f.24",   0x400000, 0x100000, CRC(aa037047) SHA1(5cb1cfb235bbbf875d2b07ac4a9130ba13d47e57) )

	ROM_REGION( 0x100000, "cpu2", 0 ) /* Protection CPU (simulated, not emulated) */
	ROM_LOAD( "14468-01.3", 0x00000, 0x10000, CRC(c3c591e4) SHA1(53e48066e85b61d0c456618d14334a509b354cb3) )
	ROM_RELOAD(             0xf0000, 0x10000             )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14599f.14", 0x000000, 0x200000, CRC(94f1cf10) SHA1(34ec86487bcb6726c025149c319f00a854eb7a1d) )
	ROM_LOAD16_BYTE( "mpr14598f.5",  0x000001, 0x200000, CRC(010656f3) SHA1(31619c022cba4f250ce174f186d3e34444f60faf) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14600f.32", 0x000000, 0x200000, CRC(e860988a) SHA1(328581877c0890519c854f75f0976b0e9c4560f8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14602.30",  0x000002, 0x200000, CRC(64524e4d) SHA1(86246185ab5ab638a73991c9e3aeb07c6d51be4f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14604.28",  0x000004, 0x200000, CRC(5f8d5167) SHA1(1b08495e5a4cc2530c2895e47abd0e0b75496c68) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14606.26",  0x000006, 0x200000, CRC(7047f437) SHA1(e806a1cd73c96b33e8edc64e41d99bf7798103e0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14601f.31", 0x800000, 0x200000, CRC(a2f3bb32) SHA1(1a60975dead5faf08ad4e9a96a00f98664d5e5ec) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14603.29",  0x800002, 0x200000, CRC(f6ce494b) SHA1(b3117e34913e855c035ebe37fbfbe0f7466f94f0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14605.27",  0x800004, 0x200000, CRC(aaf52697) SHA1(b502a37ae68fc08b60cdf0e2b744898b3474d3b9) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14607.25",  0x800006, 0x200000, CRC(b70b0735) SHA1(9ef2da6f710bc5c2c7ee30dc144409a61dbe6646) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

ROM_START( arabfgtu )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "mpr14608.8",         0x000000, 0x020000, CRC(cd5efba9) SHA1(a7daf8e95d31359753c984c447e93d40f43a179d) )
	ROM_LOAD16_BYTE_x2( "epr14592.18", 0x100000, 0x040000, CRC(f7dff316) SHA1(338690a1404dde6e7e66067f23605a247c7d0f5b) )
	ROM_LOAD16_BYTE_x2( "epr14591.9",  0x100001, 0x040000, CRC(bbd940fb) SHA1(99c17aba890935eaf7ea468492da03103288eb1b) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU + banks */
	ROM_LOAD_x8( "epr14596.36", 0x100000, 0x020000, CRC(bd01faec) SHA1(c909dcb8ef2672c4b0060d911d295e445ca311eb) )
	ROM_LOAD( "mpr14595f.35",   0x200000, 0x100000, CRC(5173d1af) SHA1(dccda644488d0c561c8ff7fa9619bd9504d8d9c6) )
	ROM_LOAD( "mpr14594f.34",   0x300000, 0x100000, CRC(01777645) SHA1(7bcbe7687bd80b94bd3b2b3099cdd036bf7e0cd3) )
	ROM_LOAD( "mpr14593f.24",   0x400000, 0x100000, CRC(aa037047) SHA1(5cb1cfb235bbbf875d2b07ac4a9130ba13d47e57) )

	ROM_REGION( 0x100000, "cpu2", 0 ) /* Protection CPU (simulated, not emulated) */
	ROM_LOAD( "14468-01.3", 0x00000, 0x10000, CRC(c3c591e4) SHA1(53e48066e85b61d0c456618d14334a509b354cb3) )
	ROM_RELOAD(             0xf0000, 0x10000             )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14599f.14", 0x000000, 0x200000, CRC(94f1cf10) SHA1(34ec86487bcb6726c025149c319f00a854eb7a1d) )
	ROM_LOAD16_BYTE( "mpr14598f.5",  0x000001, 0x200000, CRC(010656f3) SHA1(31619c022cba4f250ce174f186d3e34444f60faf) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14600f.32", 0x000000, 0x200000, CRC(e860988a) SHA1(328581877c0890519c854f75f0976b0e9c4560f8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14602.30",  0x000002, 0x200000, CRC(64524e4d) SHA1(86246185ab5ab638a73991c9e3aeb07c6d51be4f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14604.28",  0x000004, 0x200000, CRC(5f8d5167) SHA1(1b08495e5a4cc2530c2895e47abd0e0b75496c68) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14606.26",  0x000006, 0x200000, CRC(7047f437) SHA1(e806a1cd73c96b33e8edc64e41d99bf7798103e0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14601f.31", 0x800000, 0x200000, CRC(a2f3bb32) SHA1(1a60975dead5faf08ad4e9a96a00f98664d5e5ec) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14603.29",  0x800002, 0x200000, CRC(f6ce494b) SHA1(b3117e34913e855c035ebe37fbfbe0f7466f94f0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14605.27",  0x800004, 0x200000, CRC(aaf52697) SHA1(b502a37ae68fc08b60cdf0e2b744898b3474d3b9) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14607.25",  0x800006, 0x200000, CRC(b70b0735) SHA1(9ef2da6f710bc5c2c7ee30dc144409a61dbe6646) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Arabian Fight (Japan)
    protected via a custom V20 with encrypted code
*/
ROM_START( arabfgtj )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr14597.8",         0x000000, 0x020000, CRC(7a6fe222) SHA1(f730e9d44ad20dbaa59f6311a077c889e0aec8e4) )
	ROM_LOAD16_BYTE_x2( "epr14592.18", 0x100000, 0x040000, CRC(f7dff316) SHA1(338690a1404dde6e7e66067f23605a247c7d0f5b) )
	ROM_LOAD16_BYTE_x2( "epr14591.9",  0x100001, 0x040000, CRC(bbd940fb) SHA1(99c17aba890935eaf7ea468492da03103288eb1b) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU + banks */
	ROM_LOAD_x8( "epr14596.36",  0x100000, 0x020000, CRC(bd01faec) SHA1(c909dcb8ef2672c4b0060d911d295e445ca311eb) )
	ROM_LOAD( "mpr14595f.35",    0x200000, 0x100000, CRC(5173d1af) SHA1(dccda644488d0c561c8ff7fa9619bd9504d8d9c6) )
	ROM_LOAD( "mpr14594f.34",    0x300000, 0x100000, CRC(01777645) SHA1(7bcbe7687bd80b94bd3b2b3099cdd036bf7e0cd3) )
	ROM_LOAD( "mpr14593f.24",    0x400000, 0x100000, CRC(aa037047) SHA1(5cb1cfb235bbbf875d2b07ac4a9130ba13d47e57) )

	ROM_REGION( 0x100000, "cpu2", 0 ) /* Protection CPU */
	ROM_LOAD( "14468-01.3", 0x00000, 0x10000, CRC(c3c591e4) SHA1(53e48066e85b61d0c456618d14334a509b354cb3) )
	ROM_RELOAD(             0xf0000, 0x10000             )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14599f.14", 0x000000, 0x200000, CRC(94f1cf10) SHA1(34ec86487bcb6726c025149c319f00a854eb7a1d) )
	ROM_LOAD16_BYTE( "mpr14598f.5",  0x000001, 0x200000, CRC(010656f3) SHA1(31619c022cba4f250ce174f186d3e34444f60faf) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14600f.32", 0x000000, 0x200000, CRC(e860988a) SHA1(328581877c0890519c854f75f0976b0e9c4560f8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14602.30",  0x000002, 0x200000, CRC(64524e4d) SHA1(86246185ab5ab638a73991c9e3aeb07c6d51be4f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14604.28",  0x000004, 0x200000, CRC(5f8d5167) SHA1(1b08495e5a4cc2530c2895e47abd0e0b75496c68) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14606.26",  0x000006, 0x200000, CRC(7047f437) SHA1(e806a1cd73c96b33e8edc64e41d99bf7798103e0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14601f.31", 0x800000, 0x200000, CRC(a2f3bb32) SHA1(1a60975dead5faf08ad4e9a96a00f98664d5e5ec) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14603.29",  0x800002, 0x200000, CRC(f6ce494b) SHA1(b3117e34913e855c035ebe37fbfbe0f7466f94f0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14605.27",  0x800004, 0x200000, CRC(aaf52697) SHA1(b502a37ae68fc08b60cdf0e2b744898b3474d3b9) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14607.25",  0x800006, 0x200000, CRC(b70b0735) SHA1(9ef2da6f710bc5c2c7ee30dc144409a61dbe6646) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Burning Rival
    protected via FD1149 317-0212
*/
ROM_START( brival )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr15722.8",      0x000000, 0x020000, CRC(138141c0) SHA1(aca2a46ee1008e91c65a09e79c76d5175e7df5e8) )
	ROM_LOAD16_BYTE( "epr15723.18", 0x100000, 0x080000, CRC(4ff40d39) SHA1(b33a656f976ec7a1a2268e7b9a81d5b84f3d9ca3) )
	ROM_LOAD16_BYTE( "epr15724.9",  0x100001, 0x080000, CRC(3ff8a052) SHA1(f484a8e15a022f9ff290e662ab27f96f9f0ad24e) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr15725.36", 0x100000, 0x020000, CRC(ea1407d7) SHA1(68b571341f032278e87a38739ba8084b7a6062d3) )
	ROM_LOAD( "mpr15627.35",    0x200000, 0x100000, CRC(8a8388c5) SHA1(7ee03feb975cc576a3d8651fd41976ca87d60894) )
	ROM_LOAD( "mpr15626.34",    0x300000, 0x100000, CRC(83306d1e) SHA1(feb08902b51c0013d9417832cdf198e36cdfc28c) )
	ROM_LOAD( "mpr15625.24",    0x400000, 0x100000, CRC(3ce82932) SHA1(f2107bc2591f46a51c9f0d706933b1ae69db91f9) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr15629.14", 0x000000, 0x200000, CRC(2c8dd96d) SHA1(4a42a30485c19eb4f4a9d518a3dff3ae11911d01) )
	ROM_LOAD16_BYTE( "mpr15628.5",  0x000001, 0x200000, CRC(58d4ca40) SHA1(b1633acc803bba7e8283a9663b49abeda662a74d) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr15637.32", 0x000000, 0x200000, CRC(b6cf2f05) SHA1(a308d40ce5165e03fccf7fcd615ee111f7840fdc) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15635.30", 0x000002, 0x200000, CRC(70f2eb2b) SHA1(9868c8b0dd8ce810a0e32f51e702eee7e1c9a967) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15633.28", 0x000004, 0x200000, CRC(005dfed5) SHA1(f555620d75d3886a890307be9df9c0879bcda695) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15631.26", 0x000006, 0x200000, CRC(c35e2f21) SHA1(37935aa2eaa1769e57fb58f47f9797ae153d7496) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15636.31", 0x800000, 0x200000, CRC(d81ca97b) SHA1(a8e64e6cbe822f18ce20f50c8ddb8f1d5ed8b783) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15634.29", 0x800002, 0x200000, CRC(b0c6c52a) SHA1(04dd7344ca82e38f9d796a764c9e5a631a89aaac) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15632.27", 0x800004, 0x200000, CRC(8476e52b) SHA1(e89748d34febcaf362580cdae30a5c570e56899a) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15630.25", 0x800006, 0x200000, CRC(bf7dd2f6) SHA1(ab3fbe9e2b9b57424fb2a147f32b0f573c0b11b8) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Burning Rival (Japan)
    protected via FD1149 317-0212
*/
ROM_START( brivalj )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr15720.8",      0x000000, 0x020000, CRC(0d182d78) SHA1(53e9e5898869ea4a354dc3e9a78d8b8e9a7274c9) )
	ROM_LOAD16_BYTE( "epr15723.18", 0x100000, 0x080000, CRC(4ff40d39) SHA1(b33a656f976ec7a1a2268e7b9a81d5b84f3d9ca3) )
	ROM_LOAD16_BYTE( "epr15724.9",  0x100001, 0x080000, CRC(3ff8a052) SHA1(f484a8e15a022f9ff290e662ab27f96f9f0ad24e) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr15725.36", 0x100000, 0x020000, CRC(ea1407d7) SHA1(68b571341f032278e87a38739ba8084b7a6062d3) )
	ROM_LOAD( "mpr15627.35",    0x200000, 0x100000, CRC(8a8388c5) SHA1(7ee03feb975cc576a3d8651fd41976ca87d60894) )
	ROM_LOAD( "mpr15626.34",    0x300000, 0x100000, CRC(83306d1e) SHA1(feb08902b51c0013d9417832cdf198e36cdfc28c) )
	ROM_LOAD( "mpr15625.24",    0x400000, 0x100000, CRC(3ce82932) SHA1(f2107bc2591f46a51c9f0d706933b1ae69db91f9) )

	/* the 10 roms below may be bad dumps ... mp14598 / 99 have corrupt tiles when compared to the roms
       in the parent set, but Sega did change the part numbers so they might be correct, the others
       are suspicious, the changes are very similar but the part numbers haven't changed.  We really
       need a 3rd board to verify */
	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14599f.14", 0x000000, 0x200000, CRC(1de17e83) SHA1(04ee14b863f93b42a5bd1b6da71cff54ef11d4b7) )
	ROM_LOAD16_BYTE( "mpr14598f.5",  0x000001, 0x200000, CRC(cafb0de9) SHA1(94c6bfc7a4081dee373e9466a7b6f80889696087) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "brivalj_mp15637.32", 0x000000, 0x200000, CRC(f39844c0) SHA1(c48dc8cccdd9d3756cf99a983c6a89ed43fcda22) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15635.30", 0x000002, 0x200000, CRC(263cf6d1) SHA1(7accd214502fd050edc0901c9929d6069dae4d00) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15633.28", 0x000004, 0x200000, CRC(44e9a88b) SHA1(57a930b9c3b83c889df54de60c90f847c2dcb614) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15631.26", 0x000006, 0x200000, CRC(e93cf9c9) SHA1(17786cd3ccaef613216db724e923861841c52b45) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15636.31", 0x800000, 0x200000, CRC(079ff77f) SHA1(bdd41acef58c39ba58cf85d307229622877dbdf9) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15634.29", 0x800002, 0x200000, CRC(1edc14cd) SHA1(80a281c904560b364fe9f2b8987b7a254220a29f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15632.27", 0x800004, 0x200000, CRC(796215f2) SHA1(d7b393781dbba59c9b1cd600d27e6d91e36ea771) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "brivalj_mp15630.25", 0x800006, 0x200000, CRC(8dabb501) SHA1(c5af2187d00e0b9732a82441f9758b303fecbb2c) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Dark Edge
    protected via FD1149 317-0204
*/
ROM_START( darkedge )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "epr15246.8", 0x000000, 0x080000, CRC(c0bdceeb) SHA1(9cf670cf9a8691f259c75c1d9c6cb14e8a70bb72) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr15243.36", 0x100000, 0x020000, CRC(08ca5f11) SHA1(c2c48d2f02770941a93794f82cb407d6264904d2) )
	ROM_LOAD( "mpr15242.35",    0x200000, 0x100000, CRC(ffb7d917) SHA1(bfeae1a2bd7250edb695b7034f6b1f851f6fd48a) )
	ROM_LOAD( "mpr15241.34",    0x300000, 0x100000, CRC(8eccc4fe) SHA1(119724b9b6d2b51ad4f065ebf74d200960090e68) )
	ROM_LOAD( "mpr15240.24",    0x400000, 0x100000, CRC(867d59e8) SHA1(fb1c0d26dbb1bde9d8bc86419cd911b8e37bf923) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr15248", 0x000000, 0x080000, CRC(185b308b) SHA1(a49c1b752b3c4355560e0cd712fb9a096140e37b) )
	ROM_LOAD16_BYTE( "mpr15247", 0x000001, 0x080000, CRC(be21548c) SHA1(2e315aadc2a0b781c3ee3fe71c75eb1f43514eff) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr15249.32", 0x000000, 0x200000, CRC(2b4371a8) SHA1(47f448bfbc068f2d0cdedd81bcd280823d5758a3) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15251.30", 0x000002, 0x200000, CRC(efe2d689) SHA1(af22153ea3afdde3732f881087c642170f91d745) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15253.28", 0x000004, 0x200000, CRC(8356ed01) SHA1(a28747813807361c7d0c722a94e194caea8bfab6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15255.26", 0x000006, 0x200000, CRC(ff04a5b0) SHA1(d4548f9da014ba5249c2f75d654a2a88c095aaf8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15250.31", 0x800000, 0x200000, CRC(c5cab71a) SHA1(111c69c40a39c3fceef38f5876e1dcf7ac2fbee2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15252.29", 0x800002, 0x200000, CRC(f8885fea) SHA1(ef944df5f6fd64813734056ad2a150f518c75459) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15254.27", 0x800004, 0x200000, CRC(7765424b) SHA1(7cd4c275f6333beeea62dd65a769e11650c68923) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15256.25", 0x800006, 0x200000, CRC(44c36b62) SHA1(4c7f2cc4347ef2126dcbf43e8dce8500e52b5f8e) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Dark Edge (Japan)
    protected via FD1149 317-0204
*/
ROM_START( darkedgj )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "epr15244.8", 0x000000, 0x080000, CRC(0db138cb) SHA1(79ccb754e0d816b395b536a6d9c5a6e93168a913) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr15243.36", 0x100000, 0x020000, CRC(08ca5f11) SHA1(c2c48d2f02770941a93794f82cb407d6264904d2) )
	ROM_LOAD( "mpr15242.35",    0x200000, 0x100000, CRC(ffb7d917) SHA1(bfeae1a2bd7250edb695b7034f6b1f851f6fd48a) )
	ROM_LOAD( "mpr15241.34",    0x300000, 0x100000, CRC(8eccc4fe) SHA1(119724b9b6d2b51ad4f065ebf74d200960090e68) )
	ROM_LOAD( "mpr15240.24",    0x400000, 0x100000, CRC(867d59e8) SHA1(fb1c0d26dbb1bde9d8bc86419cd911b8e37bf923) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr15248", 0x000000, 0x080000, CRC(185b308b) SHA1(a49c1b752b3c4355560e0cd712fb9a096140e37b) )
	ROM_LOAD16_BYTE( "mpr15247", 0x000001, 0x080000, CRC(be21548c) SHA1(2e315aadc2a0b781c3ee3fe71c75eb1f43514eff) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr15249.32", 0x000000, 0x200000, CRC(2b4371a8) SHA1(47f448bfbc068f2d0cdedd81bcd280823d5758a3) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15251.30", 0x000002, 0x200000, CRC(efe2d689) SHA1(af22153ea3afdde3732f881087c642170f91d745) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15253.28", 0x000004, 0x200000, CRC(8356ed01) SHA1(a28747813807361c7d0c722a94e194caea8bfab6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15255.26", 0x000006, 0x200000, CRC(ff04a5b0) SHA1(d4548f9da014ba5249c2f75d654a2a88c095aaf8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15250.31", 0x800000, 0x200000, CRC(c5cab71a) SHA1(111c69c40a39c3fceef38f5876e1dcf7ac2fbee2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15252.29", 0x800002, 0x200000, CRC(f8885fea) SHA1(ef944df5f6fd64813734056ad2a150f518c75459) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15254.27", 0x800004, 0x200000, CRC(7765424b) SHA1(7cd4c275f6333beeea62dd65a769e11650c68923) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15256.25", 0x800006, 0x200000, CRC(44c36b62) SHA1(4c7f2cc4347ef2126dcbf43e8dce8500e52b5f8e) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Dragon Ball Z, VRVS
    protected via FD1149 317-0215/0217
*/
ROM_START( dbzvrvs )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD( "16543",   0x000000, 0x080000, CRC(7b9bc6f5) SHA1(556fd8471bf471e41fc6a50471c2be1bd6b98697) )
	ROM_LOAD( "16542.a", 0x080000, 0x080000, CRC(6449ab22) SHA1(03e6cdacf77f2ff80dd6798094deac5486f2c840) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "16541", 0x100000, 0x040000, CRC(1d61d836) SHA1(c6b1b54d41d2650abeaf69a31aa76c4462531880) )
	ROM_LOAD( "16540",    0x200000, 0x100000, CRC(b6f9bb43) SHA1(823f29a2fc4b9315e8c58616dbd095d45d366c8b) )
	ROM_LOAD( "16539",    0x300000, 0x100000, CRC(38c26418) SHA1(2442933e13c83209e904c1dec677aeda91b75290) )
	ROM_LOAD( "16538",    0x400000, 0x100000, CRC(4d402c31) SHA1(2df160fd7e70f3d7b52fef2a2082e68966fd1535) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "16545", 0x000000, 0x100000, CRC(51748bac) SHA1(b1cae16b62a8d29117c0adb140eb09c1092f6c37) )
	ROM_LOAD16_BYTE( "16544", 0x000001, 0x100000, CRC(f6c93dfc) SHA1(a006cedb7d0151ccc8d22e6588b1c39e099da182) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "16546", 0x000000, 0x200000, CRC(96f4be31) SHA1(ce3281630180d91de7850e9b1062382817fe0b1d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16548", 0x000002, 0x200000, CRC(00377f59) SHA1(cf0f808d7730f334c5ac80d3171fa457be9ac88e) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16550", 0x000004, 0x200000, CRC(168e8966) SHA1(a18ec30f1358b09bcde6d8d2dbe0a82bea3bdae9) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16552", 0x000006, 0x200000, CRC(a31dae31) SHA1(2da2c391f29b5fdb87e3f95d9dabd50370fafa5a) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16547", 0x800000, 0x200000, CRC(50d328ed) SHA1(c4795299f5d7c9f3a847d684d8cde7012d4486f0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16549", 0x800002, 0x200000, CRC(a5802e9f) SHA1(4cec3ed85a21aaf99b73013795721f212019e619) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16551", 0x800004, 0x200000, CRC(dede05fc) SHA1(51e092579e2b81fb68a9cc54165f80026fe71796) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16553", 0x800006, 0x200000, CRC(c0a43009) SHA1(e4f73768de512046b3e25c4238da811dcc2dde0b) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    F1 Exhaust Note
    to display the title screen this will require 2 linked system32 boards to be emulated
*/
ROM_START( f1en )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr14452a.006",       0x000000, 0x020000, CRC(b5b4a9d9) SHA1(6699c15dc1155c3cee33a06d320acbff0ab5ad11) )
	ROM_LOAD16_BYTE_x2( "epr14445.014", 0x100000, 0x040000, CRC(d06261ab) SHA1(6e1c4ce4e49a142fd5b1ecac98145960d7afd567) )
	ROM_LOAD16_BYTE_x2( "epr14444.007", 0x100001, 0x040000, CRC(07724354) SHA1(9d7f64a80553c4ae0e9cf716478fd5c4b8277470) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr14449.035", 0x100000, 0x020000, CRC(2d29699c) SHA1(cae02e5533a0edd3b3b4a54a1a43321285e06416) )
	ROM_LOAD_x2( "epr14448.031", 0x200000, 0x080000, CRC(87ca1e8d) SHA1(739274171c13983a60d061176095645419dade49) )
	ROM_LOAD_x2( "epr14447.026", 0x300000, 0x080000, CRC(db1cfcbd) SHA1(c76eb2ced5571a548ad00709097dd272747127a2) )
	ROM_LOAD_x2( "epr14446.022", 0x400000, 0x080000, CRC(646ec2cb) SHA1(67e453f128ae227e22c68f55d0d3f5831fbeb2f9) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "mpr14362", 0x000000, 0x040000, CRC(fb1c4e79) SHA1(38ee23763b9e5bb62bbc54cab95041415404f0c4) )
	ROM_LOAD32_BYTE( "mpr14361", 0x000002, 0x040000, CRC(e3204bda) SHA1(34157e80edd6d685bd5a5e23b1e0130a5f3d138a) )
	ROM_LOAD32_BYTE( "mpr14360", 0x000001, 0x040000, CRC(c5e8da79) SHA1(662a6c146fe3d0b8763d845379c06d0ee6ced1ed) )
	ROM_LOAD32_BYTE( "mpr14359", 0x000003, 0x040000, CRC(70305c68) SHA1(7a6a1bf7381eba8cc1c3897497b32ca63316972a) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14370", 0x000000, 0x080000, CRC(fda78289) SHA1(3740affdcc738c50d07ff3e5b592bdf8a8b6be15) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14369", 0x000001, 0x080000, CRC(7765116d) SHA1(9493148aa84adc90143cf638265d4c55bfb43990) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14368", 0x000002, 0x080000, CRC(5744a30e) SHA1(98544fb234a8e93716e951d5414a490845e213c5) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14367", 0x000003, 0x080000, CRC(77bb9003) SHA1(6370fdeab4967976840d752577cd860b9ce8efca) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14366", 0x000004, 0x080000, CRC(21078e83) SHA1(f35f643c28aad3bf18cb9906b114c4f49b7b4cd1) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14365", 0x000005, 0x080000, CRC(36913790) SHA1(4a447cffb44b023fe1441277db1e411d4cd119eb) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14364", 0x000006, 0x080000, CRC(0fa12ecd) SHA1(6a34c7718edffbeddded8786e11cac181b485ebd) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14363", 0x000007, 0x080000, CRC(f3427a56) SHA1(6a99d7432dfff35470ddcca5cfde36689a77e706) , ROM_SKIP(7) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    F1 Super Lap
    Requires 2 linked system 32 boards to function
    protected via FD1149 317-0210
*/
ROM_START( f1lap )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "15598",        0x000000, 0x020000, CRC(9feab7cd) SHA1(2a14c0df39e7bdae12a34679fabc6abb7618e27d) )
	ROM_LOAD_x4( "15599",        0x080000, 0x020000, CRC(5c5ac112) SHA1(2c071946e33f0700a832c7aad36f639acd35f555) )
	ROM_LOAD16_BYTE_x2( "15596", 0x100000, 0x040000, CRC(20e92909) SHA1(b974c79e11bfbd1cee61f9041cf79971fd96db3a) )
	ROM_LOAD16_BYTE_x2( "15597", 0x100001, 0x040000, CRC(cd1ccddb) SHA1(ff0371a8010141d1ab81b5eba555ae7c64e5da37) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "15592", 0x100000, 0x020000, CRC(7c055cc8) SHA1(169beb83dfae86dd408aa92b3c214b8f607825fc) )
	ROM_LOAD( "15593",    0x200000, 0x100000, CRC(e7300441) SHA1(33c264f0e6326689ba75026932c0932868e83b25) )
	ROM_LOAD( "15594",    0x300000, 0x100000, CRC(7f4ca3bb) SHA1(dc53a1857d619e574acb4c0587a6ba844df2d283) )
	ROM_LOAD( "15595",    0x400000, 0x100000, CRC(3fbdad9a) SHA1(573ea2242f79c7d3b6bf0e6745f6b07a621834ac) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "15608", 0x000000, 0x200000, CRC(64462c69) SHA1(9501e83c52e3e16f73b94cef975b5a31b2ee5476) )
	ROM_LOAD16_BYTE( "15609", 0x000001, 0x200000, CRC(d586e455) SHA1(aea190d31c590216eb19766ba749b1e9b710bdce) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "15600", 0x000000, 0x200000, CRC(d2698d23) SHA1(996fbcc1d0814e6f14fa7e4870ece077ecda54e6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15602", 0x000002, 0x200000, CRC(1674764d) SHA1(bc39757a5d25df1a088f874ca2442854eb551e48) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15604", 0x000004, 0x200000, CRC(1552bbb9) SHA1(77edd3f9d8dec87fa0445d264309e6164eba9313) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15606", 0x000006, 0x200000, CRC(2b4f5265) SHA1(48b4ccdedb52fbf80661ff380e5a273201fc0a12) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15601", 0x800000, 0x200000, CRC(31a8f40a) SHA1(62798346750dea87e43c8a8b01c33bf886bb50f4) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15603", 0x800002, 0x200000, CRC(3805ecbc) SHA1(54d29250441160f282c70adfd515adb21d2cda33) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15605", 0x800004, 0x200000, CRC(cbdbf35e) SHA1(a1c0900ac3210e72f5848561a6c4a77c804782c6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "15607", 0x800006, 0x200000, CRC(6c8817c9) SHA1(f5d493ed4237caf5042e95373bf9abd1fd16f873) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x20000, "user2", 0 ) /*  comms board  */
	ROM_LOAD( "15612", 0x00000, 0x20000, CRC(9d204617) SHA1(8db57121065f5d1ac52fcfb88459bdbdc30e645b) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Golden Axe: The Return of Death Adder
    protected via a custom V20 with encrypted code
*/
ROM_START( ga2 )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr14961.b",        0x000000, 0x020000, CRC(d9cd8885) SHA1(dc9d1f01770bd23ba5959e300badbc5093a149bc) )
	ROM_LOAD_x4( "epr14958.b",        0x080000, 0x020000, CRC(0be324a3) SHA1(5e5f457548906453eaa8d326c353b47353eab73d) )
	ROM_LOAD16_BYTE_x2( "epr15148.b", 0x100000, 0x040000, CRC(c477a9fd) SHA1(a9d60f801c12fd067e5ad1801a92c84edd13bd08) )
	ROM_LOAD16_BYTE_x2( "epr15147.b", 0x100001, 0x040000, CRC(1bb676ea) SHA1(125ffd13204f48be23e20b281c42c2307888c40b) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU + banks */
	ROM_LOAD_x16( "epr14945", 0x100000, 0x010000, CRC(4781d4cb) SHA1(bd1b774b3cd0c3e0290c55e426f66d6820d21d0f) )
	ROM_LOAD( "mpr14944",     0x200000, 0x100000, CRC(fd4d4b86) SHA1(e14b9cd6004bf9ecd902e37b433b828241361b46) )
	ROM_LOAD( "mpr14943",     0x300000, 0x100000, CRC(24d40333) SHA1(38faf8f3eac317a163e93bd2247fe98189b13d2d) )
	ROM_LOAD( "mpr14942",     0x400000, 0x100000, CRC(a89b0e90) SHA1(e14c62418eb7f9a2deb2a6dcf635bedc1c73c253) )

	ROM_REGION( 0x100000, "mcu", 0 ) /* Protection CPU */
	ROM_LOAD( "epr14468", 0x00000, 0x10000, CRC(77634daa) SHA1(339169d164b9ed7dc3787b084d33effdc8e9efc1) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14948", 0x000000, 0x200000, CRC(75050d4a) SHA1(51d6bc9935abcf30af438e69c2cf4e09f57a803f) )
	ROM_LOAD16_BYTE( "mpr14947", 0x000001, 0x200000, CRC(b53e62f4) SHA1(5aa0f198e6eb070b77b0d180d30c0228a9bc691e) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14949", 0x000000, 0x200000, CRC(152c716c) SHA1(448d16ea036b66e886119c00af543dfa5e53fd84) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14951", 0x000002, 0x200000, CRC(fdb1a534) SHA1(3126b595bf69bf9952fedf8f9c6743eb10489dc6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14953", 0x000004, 0x200000, CRC(33bd1c15) SHA1(4e16562e3357d4db54b20543073e8f1fd6f74b1f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14955", 0x000006, 0x200000, CRC(e42684aa) SHA1(12e0f18a11edb46f09e2e8c5c4ba14170d0cf00d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14950", 0x800000, 0x200000, CRC(15fd0026) SHA1(e918984bd60ad63172fe273b31cc9019100228c8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14952", 0x800002, 0x200000, CRC(96f96613) SHA1(4c9808866032dab0401de322c28242e8a8775457) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14954", 0x800004, 0x200000, CRC(39b2ac9e) SHA1(74f4c81d85ab9b4c5e8ae4b4d2c6dff766c482ca) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14956", 0x800006, 0x200000, CRC(298fca50) SHA1(16e09b19cc7be3dfc8e82b45348e6d1cf2ed5621) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Golden Axe: The Return of Death Adder (U.S.)
    protected via a custom V20 with encrypted code
*/
ROM_START( ga2u )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr14960.b",        0x000000, 0x020000, CRC(87182fea) SHA1(bb669ea7091f1ea34589a565490effa934ca44a3) )
	ROM_LOAD_x4( "epr14957.b",        0x080000, 0x020000, CRC(ab787cf4) SHA1(7e19bb3e5d587b5009efc9f9fa52aecaef0eedc4) )
	ROM_LOAD16_BYTE_x2( "epr15146.b", 0x100000, 0x040000, CRC(7293d5c3) SHA1(535a8b4b4a05546b321cee8de6733edfc1f71589) )
	ROM_LOAD16_BYTE_x2( "epr15145.b", 0x100001, 0x040000, CRC(0da61782) SHA1(f0302d747e5d55663095bb38732af423104c33ea) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU + banks */
	ROM_LOAD_x16( "epr14945", 0x100000, 0x010000, CRC(4781d4cb) SHA1(bd1b774b3cd0c3e0290c55e426f66d6820d21d0f) )
	ROM_LOAD( "mpr14944",     0x200000, 0x100000, CRC(fd4d4b86) SHA1(e14b9cd6004bf9ecd902e37b433b828241361b46) )
	ROM_LOAD( "mpr14943",     0x300000, 0x100000, CRC(24d40333) SHA1(38faf8f3eac317a163e93bd2247fe98189b13d2d) )
	ROM_LOAD( "mpr14942",     0x400000, 0x100000, CRC(a89b0e90) SHA1(e14c62418eb7f9a2deb2a6dcf635bedc1c73c253) )

	ROM_REGION( 0x100000, "mcu", 0 ) /* Protection CPU */
	ROM_LOAD( "epr14468", 0x00000, 0x10000, CRC(77634daa) SHA1(339169d164b9ed7dc3787b084d33effdc8e9efc1) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14948", 0x000000, 0x200000, CRC(75050d4a) SHA1(51d6bc9935abcf30af438e69c2cf4e09f57a803f) )
	ROM_LOAD16_BYTE( "mpr14947", 0x000001, 0x200000, CRC(b53e62f4) SHA1(5aa0f198e6eb070b77b0d180d30c0228a9bc691e) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14949", 0x000000, 0x200000, CRC(152c716c) SHA1(448d16ea036b66e886119c00af543dfa5e53fd84) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14951", 0x000002, 0x200000, CRC(fdb1a534) SHA1(3126b595bf69bf9952fedf8f9c6743eb10489dc6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14953", 0x000004, 0x200000, CRC(33bd1c15) SHA1(4e16562e3357d4db54b20543073e8f1fd6f74b1f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14955", 0x000006, 0x200000, CRC(e42684aa) SHA1(12e0f18a11edb46f09e2e8c5c4ba14170d0cf00d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14950", 0x800000, 0x200000, CRC(15fd0026) SHA1(e918984bd60ad63172fe273b31cc9019100228c8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14952", 0x800002, 0x200000, CRC(96f96613) SHA1(4c9808866032dab0401de322c28242e8a8775457) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14954", 0x800004, 0x200000, CRC(39b2ac9e) SHA1(74f4c81d85ab9b4c5e8ae4b4d2c6dff766c482ca) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14956", 0x800006, 0x200000, CRC(298fca50) SHA1(16e09b19cc7be3dfc8e82b45348e6d1cf2ed5621) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Golden Axe: The Return of Death Adder (Japan)
    protected via a custom V20 with encrypted code
*/
ROM_START( ga2j )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr14956.17",        0x000000, 0x020000, CRC(f1929177) SHA1(7dc39c40eff9fb46c2e51d1e83478cd6970e3951) )
	ROM_LOAD_x4( "epr14946.8",         0x080000, 0x020000, CRC(eacafe94) SHA1(d41a7e1ee2df9e053b559be0a1a6d2ae520fd3e4) )
	ROM_LOAD16_BYTE_x2( "epr14941.18", 0x100000, 0x040000, CRC(0ffb8203) SHA1(b27dce634d203af8abb6ddfb656d4c48eb54af01) )
	ROM_LOAD16_BYTE_x2( "epr14940.9",  0x100001, 0x040000, CRC(3b5b3084) SHA1(ea17f6b7fd413fe3808f822cec84c993c9b75aa2) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU + banks */
	ROM_LOAD_x16( "epr14945", 0x100000, 0x010000, CRC(4781d4cb) SHA1(bd1b774b3cd0c3e0290c55e426f66d6820d21d0f) )
	ROM_LOAD( "mpr14944",     0x200000, 0x100000, CRC(fd4d4b86) SHA1(e14b9cd6004bf9ecd902e37b433b828241361b46) )
	ROM_LOAD( "mpr14943",     0x300000, 0x100000, CRC(24d40333) SHA1(38faf8f3eac317a163e93bd2247fe98189b13d2d) )
	ROM_LOAD( "mpr14942",     0x400000, 0x100000, CRC(a89b0e90) SHA1(e14c62418eb7f9a2deb2a6dcf635bedc1c73c253) )

	ROM_REGION( 0x100000, "mcu", 0 ) /* Protection CPU */
	ROM_LOAD( "epr1446802.3", 0x00000, 0x10000, CRC(77634daa) SHA1(339169d164b9ed7dc3787b084d33effdc8e9efc1) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr14948", 0x000000, 0x200000, CRC(75050d4a) SHA1(51d6bc9935abcf30af438e69c2cf4e09f57a803f) )
	ROM_LOAD16_BYTE( "mpr14947", 0x000001, 0x200000, CRC(b53e62f4) SHA1(5aa0f198e6eb070b77b0d180d30c0228a9bc691e) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14949", 0x000000, 0x200000, CRC(152c716c) SHA1(448d16ea036b66e886119c00af543dfa5e53fd84) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14951", 0x000002, 0x200000, CRC(fdb1a534) SHA1(3126b595bf69bf9952fedf8f9c6743eb10489dc6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14953", 0x000004, 0x200000, CRC(33bd1c15) SHA1(4e16562e3357d4db54b20543073e8f1fd6f74b1f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14955", 0x000006, 0x200000, CRC(e42684aa) SHA1(12e0f18a11edb46f09e2e8c5c4ba14170d0cf00d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14950", 0x800000, 0x200000, CRC(15fd0026) SHA1(e918984bd60ad63172fe273b31cc9019100228c8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14952", 0x800002, 0x200000, CRC(96f96613) SHA1(4c9808866032dab0401de322c28242e8a8775457) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14954", 0x800004, 0x200000, CRC(39b2ac9e) SHA1(74f4c81d85ab9b4c5e8ae4b4d2c6dff766c482ca) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr14956", 0x800006, 0x200000, CRC(298fca50) SHA1(16e09b19cc7be3dfc8e82b45348e6d1cf2ed5621) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Hard Dunk (Multi-32)
    not protected
*/
ROM_START( harddunk )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x2( "ep16512.37", 0x000000, 0x40000, CRC(1a7de085) SHA1(2e0dac1f7715089b7f6b1035c859ffe2d674932f) )
	/* the following is the same as 16509.40 but with a different name, unusual for Sega */
	ROM_LOAD32_WORD_x2( "ep16513.40", 0x000002, 0x40000, CRC(603dee75) SHA1(32ae964a4b57d470b4900cca6e06329f1a75a6e6) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "16505", 0x100000, 0x20000, CRC(eeb90a07) SHA1(d1c2132897994b2e85fd5a97222b9fcd61bc421e) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "16503", 0x000000, 0x080000, CRC(ac1b6f1a) SHA1(56482931adf7fe551acf796b74cd8af3773d4fef) )
	ROM_LOAD16_BYTE( "16504", 0x000001, 0x080000, CRC(7c61fcd8) SHA1(ca4354f90fada752bf11ee22a7798a8aa22b1c61) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "16495", 0x000000, 0x200000, CRC(6e5f26be) SHA1(146761072bbed08f4a9df8a474b34fab61afaa4f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16497", 0x000002, 0x200000, CRC(42ab5859) SHA1(f50c51eb81186aec5f747ecab4c5c928f8701afc) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16499", 0x000004, 0x200000, CRC(a290ea36) SHA1(2503b44174f23a9d323caab86553977d1d6d9c94) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16501", 0x000006, 0x200000, CRC(f1566620) SHA1(bcf31d11ee669d5afc7dc22c42fa59f4e48c1f50) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16496", 0x800000, 0x200000, CRC(d9d27247) SHA1(d211623478516ed1b89ab16a7fc7969954c5e353) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16498", 0x800002, 0x200000, CRC(c022a991) SHA1(a660a20692f4d9ba7be73577328f69f109be5e47) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16500", 0x800004, 0x200000, CRC(452c0be3) SHA1(af87ce4618bae2d791c1baed34ba7f853af664ff) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16502", 0x800006, 0x200000, CRC(ffc3147e) SHA1(12d882dec3098674d27058a8009e8778555f477a) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	ROM_LOAD( "mp16506.1", 0x000000, 0x200000, CRC(e779f5ed) SHA1(462d1bbe8bb12a0c5a6d6c613c720b26ec21cb25) )
	ROM_LOAD( "mp16507.2", 0x200000, 0x200000, CRC(31e068d3) SHA1(9ac88b15af441fb3b31ce759c565b60a09039571) )
ROM_END

/**************************************************************************************************************************
    Hard Dunk (Japan) (Multi-32)
    not protected
*/
ROM_START( harddunj )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x2( "16508.37", 0x000000, 0x40000, CRC(b3713be5) SHA1(8123638a838e41fcc0d32e14382421b521eff94f) )
	ROM_LOAD32_WORD_x2( "16509.40", 0x000002, 0x40000, CRC(603dee75) SHA1(32ae964a4b57d470b4900cca6e06329f1a75a6e6) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "16505", 0x100000, 0x20000, CRC(eeb90a07) SHA1(d1c2132897994b2e85fd5a97222b9fcd61bc421e) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "16503", 0x000000, 0x080000, CRC(ac1b6f1a) SHA1(56482931adf7fe551acf796b74cd8af3773d4fef) )
	ROM_LOAD16_BYTE( "16504", 0x000001, 0x080000, CRC(7c61fcd8) SHA1(ca4354f90fada752bf11ee22a7798a8aa22b1c61) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "16495", 0x000000, 0x200000, CRC(6e5f26be) SHA1(146761072bbed08f4a9df8a474b34fab61afaa4f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16497", 0x000002, 0x200000, CRC(42ab5859) SHA1(f50c51eb81186aec5f747ecab4c5c928f8701afc) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16499", 0x000004, 0x200000, CRC(a290ea36) SHA1(2503b44174f23a9d323caab86553977d1d6d9c94) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16501", 0x000006, 0x200000, CRC(f1566620) SHA1(bcf31d11ee669d5afc7dc22c42fa59f4e48c1f50) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16496", 0x800000, 0x200000, CRC(d9d27247) SHA1(d211623478516ed1b89ab16a7fc7969954c5e353) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16498", 0x800002, 0x200000, CRC(c022a991) SHA1(a660a20692f4d9ba7be73577328f69f109be5e47) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16500", 0x800004, 0x200000, CRC(452c0be3) SHA1(af87ce4618bae2d791c1baed34ba7f853af664ff) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "16502", 0x800006, 0x200000, CRC(ffc3147e) SHA1(12d882dec3098674d27058a8009e8778555f477a) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	ROM_LOAD( "mp16506.1", 0x000000, 0x200000, CRC(e779f5ed) SHA1(462d1bbe8bb12a0c5a6d6c613c720b26ec21cb25) )
	ROM_LOAD( "mp16507.2", 0x200000, 0x200000, CRC(31e068d3) SHA1(9ac88b15af441fb3b31ce759c565b60a09039571) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Holosseum
    not protected
*/
ROM_START( holo )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr14977.a",      0x000000, 0x020000, CRC(e0d7e288) SHA1(3126041ba73f21fac0207bf5c63230c61180f564) )
	ROM_LOAD_x4( "epr14976.a",      0x080000, 0x020000, CRC(e56f13be) SHA1(3d9e7add8feaa35c4c2e8bda84ae251087bd5e40) )
	ROM_LOAD16_BYTE_x4( "epr15011", 0x100000, 0x020000, CRC(b9f59f59) SHA1(f8c91fa877cf53153bec3d7850eab38227cc18ba) )
	ROM_LOAD16_BYTE_x4( "epr15010", 0x100001, 0x020000, CRC(0c09c57b) SHA1(028a9fe1c625be218ba90906308d25d69d4de4c4) )

    ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr14965", 0x100000, 0x020000, CRC(3a918cfe) SHA1(f43ecbc9e774873e868bc921321541b308ea1a3c) )
	ROM_LOAD( "mpr14964",    0x200000, 0x100000, CRC(7ff581d5) SHA1(ab81bd70937319e4edc8924bdb493d5ef1ec096a) )
	ROM_LOAD( "mpr14963",    0x300000, 0x100000, CRC(0974a60e) SHA1(87d770edcee9c9e8f37d36ab28c5aa5d685ea849) )
	ROM_LOAD( "mpr14962",    0x400000, 0x100000, CRC(6b2e694e) SHA1(7874bdfd534231c7756e0e0d9fc7a3d5bdba74d3) )

	ROM_REGION( 0x000100, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	/* game doesn't use bg tilemaps */

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr14973", 0x000000, 0x100000, CRC(b3c3ff6b) SHA1(94e8dbfae37a5b122ee3d471aad1f758e4a39b9e) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14972", 0x000001, 0x100000, CRC(0c161374) SHA1(413ab45deb687ecdbdc06ae98aa32ad8a0d80e0c) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14971", 0x000002, 0x100000, CRC(dfcf6fdf) SHA1(417291b54010be20dd6738a70d372b580615a8bb) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14970", 0x000003, 0x100000, CRC(cae3a745) SHA1(b6cc1f4abb460cda4714967e880928dc727ecf0a) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14969", 0x000004, 0x100000, CRC(c06b7c15) SHA1(8b97a899e6eacf798b9f55af8df95e12ccacadec) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14968", 0x000005, 0x100000, CRC(f413894a) SHA1(d65f57b1e55199e901c7c2f701589c46eeab739a) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14967", 0x000006, 0x100000, CRC(5377fce0) SHA1(baf8f82ab851b24202938fc7213d72324b9b92c0) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr14966", 0x000007, 0x100000, CRC(dffba2e9) SHA1(b97e47e78abb8302bc2c87681643382203bd76eb) , ROM_SKIP(7) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Jurassic Park
    not protected
*/
ROM_START( jpark )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "epr16402a.8",     0x000000, 0x80000, CRC(c70db239) SHA1(fd79dfd1ce194fcc8ccb58117bc845cdfe9943b1) )
	ROM_LOAD16_BYTE( "epr16395.18", 0x100000, 0x80000, CRC(ac5a01d6) SHA1(df6bffdf5723cb8790a9c1c0ab271989a758bdd8) )
	ROM_LOAD16_BYTE( "epr16394.9",  0x100001, 0x80000, CRC(c08c3a8a) SHA1(923cf256d863656336401fa75103b42298cb3822) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr16399.36", 0x100000, 0x040000, CRC(b09b2fe3) SHA1(bf8d646bab65fcc4ece8c2bd9a3df389e5860ed6) )
	ROM_LOAD( "mpr16398.35",    0x200000, 0x100000, CRC(fa710ca6) SHA1(1fd625070eef5f99d7be07606aeeff9282e32532) )
	ROM_LOAD( "mpr16397.34",    0x300000, 0x100000, CRC(6e96e0be) SHA1(422b783b72127b80a23043b2dd1c04f5772f436e) )
	ROM_LOAD( "mpr16396.24",    0x400000, 0x100000, CRC(f69a2dc4) SHA1(3f02b10976852916c58e852f3161a857784fe36b) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr16404.14", 0x000000, 0x200000, CRC(11283807) SHA1(99e465c3fc31e640740b8257a349e203f026754a) )
	ROM_LOAD16_BYTE( "mpr16403.5",  0x000001, 0x200000, CRC(02530a9b) SHA1(b43e1b47f74c801bfc599cbe893fb8dc13453dd0) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr16405.32", 0x000000, 0x200000, CRC(b425f182) SHA1(66c6bd29dd3450db816b895c4c9c5208a66aae67) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16407.30", 0x000002, 0x200000, CRC(bc49ffd9) SHA1(a50ba7ddccfdfd7638c4041978b39c1559afbbb4) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16409.28", 0x000004, 0x200000, CRC(fe73660d) SHA1(ec1a3ea5303d2ccb9e327da18476969953626e1c) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16411.26", 0x000006, 0x200000, CRC(71cabbc5) SHA1(9760f57ef43eb251488dadd37711d5682d902434) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16406.31", 0x800000, 0x200000, CRC(b9ed73d6) SHA1(0dd22e7a21e95d84fc91acd742c737f96529f515) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16408.29", 0x800002, 0x200000, CRC(7b2f476b) SHA1(da99a9911982ba8aaef8c9aecc2977c9fd6da094) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16410.27", 0x800004, 0x200000, CRC(49c8f952) SHA1(f26b818711910b10bf520e5f849a1478a6b1d6e6) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16412.25", 0x800006, 0x200000, CRC(105dc26e) SHA1(fd2ef8c9fe1a78b4f9cc891a6fbd060184e58a1f) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x10000, "cpu2", 0 ) /* unused */
	ROM_LOAD( "epr13908.xx", 0x00000, 0x8000, CRC(6228c1d2) SHA1(bd37fe775534fb94c9af80546948ce5f9c47bbf5) ) /* cabinet movement */
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Kokoroji 2
    Sega System32 + CD - Sega 1993

    Rom Board is 837-8393 16Mb ROM board (Same as godenaxe2 or Arabian Fight)

    SCSI CD board is 839-0572-01. It use a Fujitsu MB89352AP for SCSI + a Sony CXD1095Q for I/O + 8Mhz quartz
*/
ROM_START( kokoroj2 )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr16186.8",     0x000000,  0x20000, CRC(8c3afb6e) SHA1(68c3c066a943b1ea8e3bee06c7c8279b5a12e7f7) )
	ROM_LOAD16_BYTE( "epr16183.18", 0x100000, 0x80000, CRC(4844432f) SHA1(b127169d0f108e0b99ec81814a5c3c45bb82e0b1) )
	ROM_LOAD16_BYTE( "epr16182.9",  0x100001, 0x80000, CRC(a27f5f5f) SHA1(c2bbd1632bce0851cf8ab45d3ccbec1076e67f5e) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr16185.36", 0x100000, 0x020000, CRC(afb97c4d) SHA1(f6e77d932824f93d89559a9cb3b2d678d5fc6940) )
	ROM_LOAD( "mpr16184.35",    0x200000, 0x080000, CRC(dbd44a85) SHA1(e7341d2ef27c580bff365b5c546da2adb72faee8) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr16188.14", 0x000000, 0x200000, CRC(83a450ab) SHA1(1d0b45512d784ed1d82135b84c7c540f92d789f7) )
	ROM_LOAD16_BYTE( "mpr16187.5",  0x000001, 0x200000, CRC(98b62f8b) SHA1(eaf98efd9eac7b7c385138a8a4dbc94b0ca38df5) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr16189.32", 0x000000, 0x200000, CRC(0937f713) SHA1(4b2b09ec8ed97794ad3824d1c57eae7f7e01379c) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16191.30", 0x000002, 0x200000, CRC(cfef4aaa) SHA1(bc8a252dcbdb8facdd91eda7aed0f56fe7529d15) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16193.28", 0x000004, 0x200000, CRC(a0706e4e) SHA1(1f36d952971c05db4190b229aa4957db3e5224f1) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16195.26", 0x000006, 0x200000, CRC(a4ddcd61) SHA1(90ef40f1fc84d1e4d4f78c33b8f0d1f56e04bf90) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16190.31", 0x800000, 0x200000, CRC(528d408e) SHA1(89f8a2cfc8b59377d6a65555c3172e457b131502) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16192.29", 0x800002, 0x200000, CRC(efaa93d1) SHA1(2947eaf7fc358ced1c04e7abe7a3f3066c73f2d0) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16194.27", 0x800004, 0x200000, CRC(39b5efe7) SHA1(2039909a2dd46951d442f1b6377f365525f9f2f1) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16196.25", 0x800006, 0x200000, CRC(b8e22e05) SHA1(dd667e2c5d421cba356421825e6aca9b5ca0af45) , ROM_SKIP(6)|ROM_GROUPWORD )

	/* AUDIO CD -- verify that this is an acceptable read and convert to CHD */
//  ROM_REGION( 0x0017eb, "user1", 0 )
//  ROM_LOAD( "segaa.cue",     0x0,  0x0017eb, CRC(7dae0d4b) SHA1(05c84e3a2b0fd0630b84b42e8d4c4ffbbb1cf531) )
//  ROM_REGION( 0x179ce890, "user2", 0 )
//  ROM_LOAD( "segaa.bin",     0x0,  0x179ce890, CRC(4d6937a4) SHA1(637be0a388e9bde8b12cdf6a0bfb7a0528e763b3) )
ROM_END

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Outrunners (Multi-32)
    not protected
*/
ROM_START( orunners )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x4( "epr15620.37", 0x000000, 0x020000, CRC(84f5ad92) SHA1(1f9cb04b42b2d450be93400d9979a7910eaf05d1) )
	ROM_LOAD32_WORD_x4( "epr15621.40", 0x000002, 0x020000, CRC(d98b765a) SHA1(b58567e976228267a86af53de2135bc0b247a44a) )
	ROM_LOAD32_WORD( "mpr15538.bin",   0x100000, 0x080000, CRC(93958820) SHA1(e19b6f18a5707dbb64ae009d63c05eac5bac4a81) )
	ROM_LOAD32_WORD( "mpr15539.bin",   0x100002, 0x080000, CRC(219760fa) SHA1(bd62a83de9c9542f6da454a87dc4947492f65c52) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr15550.bin", 0x100000, 0x80000, CRC(0205d2ed) SHA1(3475479e1a45fe96eefbe53842758898db7accbf) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr15548.bin", 0x000000, 0x200000, CRC(b6470a66) SHA1(e1544590c02d41f62f82a4d771b893fb0f2734c7) )
	ROM_LOAD16_BYTE( "mpr15549.bin", 0x000001, 0x200000, CRC(81d12520) SHA1(1555893941e832f00ad3d0b3ad0c34a0d3a1c58a) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr15540.bin", 0x000000, 0x200000, CRC(a10d72b4) SHA1(6d9d5e20be6721b53ce49df4d5a1bbd91f5b3aed) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15542.bin", 0x000002, 0x200000, CRC(40952374) SHA1(c669ef52508bc2f49cf812dc86ac98fb535471fa) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15544.bin", 0x000004, 0x200000, CRC(39e3df45) SHA1(38a7b21617b45613b05509dda388f8f7770b186c) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15546.bin", 0x000006, 0x200000, CRC(e3fcc12c) SHA1(1cf7e05c7873f68789a27a91cddf471df40d7907) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15541.bin", 0x800000, 0x200000, CRC(a2003c2d) SHA1(200a2c7d78d3f5f28909267fdcdbddd58c5f5fa2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15543.bin", 0x800002, 0x200000, CRC(933e8e7b) SHA1(0d53286f524f47851a483569dc37e9f6d34cc5f4) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15545.bin", 0x800004, 0x200000, CRC(53dd0235) SHA1(4aee5ae1820ff933b6bd8a54bdbf989c0bc95c1a) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15547.bin", 0x800006, 0x200000, CRC(edcb2a43) SHA1(f0bcfcc749ca0267f85bf9838164869912944d00) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	ROM_LOAD( "mpr15551.bin", 0x000000, 0x200000, CRC(4894bc73) SHA1(351f5c03fb430fd87df915dfe3a377b5ada622c4) )
	ROM_LOAD( "mpr15552.bin", 0x200000, 0x200000, CRC(1c4b5e73) SHA1(50a8e9a200575a3522a51bf094aa0e87b90bb0a3) )
ROM_END

ROM_START( orunneru )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x4( "epr15618.37", 0x000000, 0x020000, CRC(25647f76) SHA1(9f882921ebb2f078350295c322b263f75812c053) )
	ROM_LOAD32_WORD_x4( "epr15619.40", 0x000002, 0x020000, CRC(2a558f95) SHA1(616ec0a7b251da61a49b933c58895b1a4d39417a) )
	ROM_LOAD32_WORD( "mpr15538.bin",   0x100000, 0x080000, CRC(93958820) SHA1(e19b6f18a5707dbb64ae009d63c05eac5bac4a81) )
	ROM_LOAD32_WORD( "mpr15539.bin",   0x100002, 0x080000, CRC(219760fa) SHA1(bd62a83de9c9542f6da454a87dc4947492f65c52) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr15550.bin", 0x100000, 0x80000, CRC(0205d2ed) SHA1(3475479e1a45fe96eefbe53842758898db7accbf) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr15548.bin", 0x000000, 0x200000, CRC(b6470a66) SHA1(e1544590c02d41f62f82a4d771b893fb0f2734c7) )
	ROM_LOAD16_BYTE( "mpr15549.bin", 0x000001, 0x200000, CRC(81d12520) SHA1(1555893941e832f00ad3d0b3ad0c34a0d3a1c58a) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr15540.bin", 0x000000, 0x200000, CRC(a10d72b4) SHA1(6d9d5e20be6721b53ce49df4d5a1bbd91f5b3aed) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15542.bin", 0x000002, 0x200000, CRC(40952374) SHA1(c669ef52508bc2f49cf812dc86ac98fb535471fa) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15544.bin", 0x000004, 0x200000, CRC(39e3df45) SHA1(38a7b21617b45613b05509dda388f8f7770b186c) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15546.bin", 0x000006, 0x200000, CRC(e3fcc12c) SHA1(1cf7e05c7873f68789a27a91cddf471df40d7907) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15541.bin", 0x800000, 0x200000, CRC(a2003c2d) SHA1(200a2c7d78d3f5f28909267fdcdbddd58c5f5fa2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15543.bin", 0x800002, 0x200000, CRC(933e8e7b) SHA1(0d53286f524f47851a483569dc37e9f6d34cc5f4) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15545.bin", 0x800004, 0x200000, CRC(53dd0235) SHA1(4aee5ae1820ff933b6bd8a54bdbf989c0bc95c1a) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15547.bin", 0x800006, 0x200000, CRC(edcb2a43) SHA1(f0bcfcc749ca0267f85bf9838164869912944d00) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	ROM_LOAD( "mpr15551.bin", 0x000000, 0x200000, CRC(4894bc73) SHA1(351f5c03fb430fd87df915dfe3a377b5ada622c4) )
	ROM_LOAD( "mpr15552.bin", 0x200000, 0x200000, CRC(1c4b5e73) SHA1(50a8e9a200575a3522a51bf094aa0e87b90bb0a3) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Rad Mobile
    not protected

    GAME BD NO. 833-7738-02
     1. ROM BD. 834-7739-02
     2. MAIN BD. 837-7428
    A/D BD NO. 837-7536
*/
ROM_START( radm )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr-13693.ic21",     0x000000, 0x020000, CRC(3f09a211) SHA1(e0e011d7069745e9bf0395bc1375d0f8b9c46dab) )
	ROM_LOAD16_BYTE( "epr-13525.ic37", 0x100000, 0x080000, CRC(62ad83a0) SHA1(b537176ebca15d91db04d5d7ab36aa967d41288e) )
	ROM_LOAD16_BYTE( "epr-13526.ic38", 0x100001, 0x080000, CRC(59ea372a) SHA1(e7a5d59586652c59c23e07e0a99ecc740fb6144d) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr-13527.ic9",  0x100000, 0x020000, CRC(a2e3fbbe) SHA1(2787bbef696ab3f2b7855ac991867837d3de54cd) )
	ROM_LOAD_x2( "epr-13523.ic14", 0x200000, 0x080000, CRC(d5563697) SHA1(eb3fd3dbfea383ac1bb5d2e1552723994cb4693d) )
	ROM_LOAD_x2( "epr-13699.ic20", 0x300000, 0x080000, CRC(33fd2913) SHA1(60b664559b4989446b1c7d875432e53a36fe27df) )
	ROM_LOAD_x2( "epr-13523.ic14", 0x400000, 0x080000, CRC(d5563697) SHA1(eb3fd3dbfea383ac1bb5d2e1552723994cb4693d) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "mpr-13519.ic3",  0x000000, 0x080000, CRC(bedc9534) SHA1(7b3f7a47b6c0ca6707dc3c1167f3564d43adb32f) )
	ROM_LOAD32_BYTE( "mpr-13520.ic7",  0x000002, 0x080000, CRC(3532e91a) SHA1(669c8d27b4b48e1ab9d6d30b0994f5a4e5169118) )
	ROM_LOAD32_BYTE( "mpr-13521.ic12", 0x000001, 0x080000, CRC(e9bca903) SHA1(18a73c830b9755262a1c525e3ad5ae084117b64d) )
	ROM_LOAD32_BYTE( "mpr-13522.ic18", 0x000003, 0x080000, CRC(25e04648) SHA1(617e794e8f7aa2a435bac917b8968699fe88dafb) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr-13511.ic1",  0x000000, 0x100000, CRC(f8f15b11) SHA1(da6c2b8c3a94c4c263583f046823eaea818aff7c) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13512.ic5",  0x000001, 0x100000, CRC(d0be34a6) SHA1(b42a63e30f0f7a94de8a825ca93cf8efdb7a7648) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13513.ic10", 0x000002, 0x100000, CRC(feef1982) SHA1(bdf906317079a12c48ef4fca5bef0d437e9bf050) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13514.ic16", 0x000003, 0x100000, CRC(d0f9ebd1) SHA1(510ebd3d7a52bcab2debea61591770d1dff172a1) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13515.ic2",  0x000004, 0x100000, CRC(77bf2387) SHA1(7215dde5618e238edbe16b3007ede790785fe25f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13516.ic6",  0x000005, 0x100000, CRC(8c4bc62d) SHA1(3206f623ec0b7558413d063404103b183f26b488) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13517.ic11", 0x000006, 0x100000, CRC(1d7d84a7) SHA1(954cfccfc7250a5bead2eeba42e655d5ac82955f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13518.ic17", 0x000007, 0x100000, CRC(9ea4b15d) SHA1(7dcfd6d42bb945beca8344cf92e7bd53903a824b) , ROM_SKIP(7) )

	ROM_REGION( 0x8000, "user2", 0 ) /* unused (cabinet motor?) */
	ROM_LOAD( "epr-13686.bin", 0x00000, 0x8000, CRC(317a2857) SHA1(e0788dc7a7d214d9c4d26b24e44c1a0dc9ae477c) ) /* cabinet movement */
ROM_END

/**************************************************************************************************************************
    Rad Mobile (U.S.)
    not protected

    GAME BD NO. 833-7738-01
     1. ROM BD. 834-7739-01
     2. MAIN BD. 837-7428
    A/D BD NO. 837-7536
*/
ROM_START( radmu )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr-13690.ic21",     0x000000, 0x020000, CRC(21637dec) SHA1(b9921effb10a72f3bdca4d540149c7f46662b716) )
	ROM_LOAD16_BYTE( "epr-13525.ic37", 0x100000, 0x080000, CRC(62ad83a0) SHA1(b537176ebca15d91db04d5d7ab36aa967d41288e) )
	ROM_LOAD16_BYTE( "epr-13526.ic38", 0x100001, 0x080000, CRC(59ea372a) SHA1(e7a5d59586652c59c23e07e0a99ecc740fb6144d) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr-13527.ic9",  0x100000, 0x020000, CRC(a2e3fbbe) SHA1(2787bbef696ab3f2b7855ac991867837d3de54cd) )
	ROM_LOAD_x2( "epr-13523.ic14", 0x200000, 0x080000, CRC(d5563697) SHA1(eb3fd3dbfea383ac1bb5d2e1552723994cb4693d) )
	ROM_LOAD_x2( "epr-13699.ic20", 0x300000, 0x080000, CRC(33fd2913) SHA1(60b664559b4989446b1c7d875432e53a36fe27df) )
	ROM_LOAD_x2( "epr-13523.ic14", 0x400000, 0x080000, CRC(d5563697) SHA1(eb3fd3dbfea383ac1bb5d2e1552723994cb4693d) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "mpr-13519.ic3",  0x000000, 0x080000, CRC(bedc9534) SHA1(7b3f7a47b6c0ca6707dc3c1167f3564d43adb32f) )
	ROM_LOAD32_BYTE( "mpr-13520.ic7",  0x000002, 0x080000, CRC(3532e91a) SHA1(669c8d27b4b48e1ab9d6d30b0994f5a4e5169118) )
	ROM_LOAD32_BYTE( "mpr-13521.ic12", 0x000001, 0x080000, CRC(e9bca903) SHA1(18a73c830b9755262a1c525e3ad5ae084117b64d) )
	ROM_LOAD32_BYTE( "mpr-13522.ic18", 0x000003, 0x080000, CRC(25e04648) SHA1(617e794e8f7aa2a435bac917b8968699fe88dafb) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr-13511.ic1",  0x000000, 0x100000, CRC(f8f15b11) SHA1(da6c2b8c3a94c4c263583f046823eaea818aff7c) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13512.ic5",  0x000001, 0x100000, CRC(d0be34a6) SHA1(b42a63e30f0f7a94de8a825ca93cf8efdb7a7648) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13513.ic10", 0x000002, 0x100000, CRC(feef1982) SHA1(bdf906317079a12c48ef4fca5bef0d437e9bf050) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13514.ic16", 0x000003, 0x100000, CRC(d0f9ebd1) SHA1(510ebd3d7a52bcab2debea61591770d1dff172a1) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13515.ic2",  0x000004, 0x100000, CRC(77bf2387) SHA1(7215dde5618e238edbe16b3007ede790785fe25f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13516.ic6",  0x000005, 0x100000, CRC(8c4bc62d) SHA1(3206f623ec0b7558413d063404103b183f26b488) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13517.ic11", 0x000006, 0x100000, CRC(1d7d84a7) SHA1(954cfccfc7250a5bead2eeba42e655d5ac82955f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr-13518.ic17", 0x000007, 0x100000, CRC(9ea4b15d) SHA1(7dcfd6d42bb945beca8344cf92e7bd53903a824b) , ROM_SKIP(7) )

	ROM_REGION( 0x8000, "user2", 0 ) /* unused (cabinet motor?) */
	ROM_LOAD( "epr-13686.bin", 0x00000, 0x8000, CRC(317a2857) SHA1(e0788dc7a7d214d9c4d26b24e44c1a0dc9ae477c) ) /* cabinet movement */
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Rad Rally
    not protected
*/
ROM_START( radr )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr14241.06",     0x000000, 0x020000, CRC(59a5f63d) SHA1(325a26a09475ddc828de71e71a1d3043f3959cec) )
	ROM_LOAD16_BYTE( "epr14106.14", 0x100000, 0x080000, CRC(e73c63bf) SHA1(30fb68eaa7d02a232c873bd7751cac7d0fa08e44) )
	ROM_LOAD16_BYTE( "epr14107.07", 0x100001, 0x080000, CRC(832f797a) SHA1(b0c16ef7bd8d37f592975052ba9da3da70a2fc79) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr14108.35", 0x100000, 0x020000, CRC(38a99b4d) SHA1(b6455e6b29bfef41c5e0ebe3a8064889b7e5f5fd) )
	ROM_LOAD_x2( "epr14109.31", 0x200000, 0x080000, CRC(b42e5833) SHA1(da94ce7c1d7a581a1aa6b79b323c67a919918808) )
	ROM_LOAD_x2( "epr14110.26", 0x300000, 0x080000, CRC(b495e7dc) SHA1(b4143fcee10e0649378fdb1e3f5a0a2c585414ec) )
	ROM_LOAD_x2( "epr14237.22", 0x400000, 0x080000, CRC(0a4b4b29) SHA1(98447a587f903ba03e17d6a145b7c8bfddf25c4d) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "epr14102.38", 0x000000, 0x040000, CRC(5626e80f) SHA1(9844817295a8cd8a9b09da6681b0c1fbfe82618e) )
	ROM_LOAD32_BYTE( "epr14103.34", 0x000002, 0x040000, CRC(08c7e804) SHA1(cf45b1934edc43cb3a0ed72159949cb0dd00d701) )
	ROM_LOAD32_BYTE( "epr14104.29", 0x000001, 0x040000, CRC(b0173646) SHA1(1ba4edc033e0e4f5a1e02987e9f6b8b1650b46d7) )
	ROM_LOAD32_BYTE( "epr14105.25", 0x000003, 0x040000, CRC(614843b6) SHA1(d4f2cd3b024f7152d6e89237f0da06adea2efe57) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr13511.36", 0x000000, 0x100000, CRC(f8f15b11) SHA1(da6c2b8c3a94c4c263583f046823eaea818aff7c) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13512.32", 0x000001, 0x100000, CRC(d0be34a6) SHA1(b42a63e30f0f7a94de8a825ca93cf8efdb7a7648) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13513.27", 0x000002, 0x100000, CRC(feef1982) SHA1(bdf906317079a12c48ef4fca5bef0d437e9bf050) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13514.23", 0x000003, 0x100000, CRC(d0f9ebd1) SHA1(510ebd3d7a52bcab2debea61591770d1dff172a1) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13515.37", 0x000004, 0x100000, CRC(77bf2387) SHA1(7215dde5618e238edbe16b3007ede790785fe25f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13516.33", 0x000005, 0x100000, CRC(8c4bc62d) SHA1(3206f623ec0b7558413d063404103b183f26b488) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13517.28", 0x000006, 0x100000, CRC(1d7d84a7) SHA1(954cfccfc7250a5bead2eeba42e655d5ac82955f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13518.24", 0x000007, 0x100000, CRC(9ea4b15d) SHA1(7dcfd6d42bb945beca8344cf92e7bd53903a824b) , ROM_SKIP(7) )

	ROM_REGION( 0x8000, "user2", 0 ) /* unused */
	ROM_LOAD( "epr14084.17", 0x00000, 0x8000, CRC(f14ed074) SHA1(e1bb23eac85e3236046527c5c7688f6f23d43aef) ) /* cabinet link */
ROM_END

/**************************************************************************************************************************
    Rad Rally (U.S.)
    not protected
*/
ROM_START( radru )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x8( "epr14240.bin",    0x000000, 0x020000, CRC(8473e7ab) SHA1(fbd883dc804d7de6ce239b68c6e6ae3a54e9e03c) )
	ROM_LOAD16_BYTE( "epr14106.14", 0x100000, 0x080000, CRC(e73c63bf) SHA1(30fb68eaa7d02a232c873bd7751cac7d0fa08e44) )
	ROM_LOAD16_BYTE( "epr14107.07", 0x100001, 0x080000, CRC(832f797a) SHA1(b0c16ef7bd8d37f592975052ba9da3da70a2fc79) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr14108.35", 0x100000, 0x020000, CRC(38a99b4d) SHA1(b6455e6b29bfef41c5e0ebe3a8064889b7e5f5fd) )
	ROM_LOAD_x2( "epr14109.31", 0x200000, 0x080000, CRC(b42e5833) SHA1(da94ce7c1d7a581a1aa6b79b323c67a919918808) )
	ROM_LOAD_x2( "epr14110.26", 0x300000, 0x080000, CRC(b495e7dc) SHA1(b4143fcee10e0649378fdb1e3f5a0a2c585414ec) )
	ROM_LOAD_x2( "epr14237.22", 0x400000, 0x080000, CRC(0a4b4b29) SHA1(98447a587f903ba03e17d6a145b7c8bfddf25c4d) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "epr14102.38", 0x000000, 0x040000, CRC(5626e80f) SHA1(9844817295a8cd8a9b09da6681b0c1fbfe82618e) )
	ROM_LOAD32_BYTE( "epr14103.34", 0x000002, 0x040000, CRC(08c7e804) SHA1(cf45b1934edc43cb3a0ed72159949cb0dd00d701) )
	ROM_LOAD32_BYTE( "epr14104.29", 0x000001, 0x040000, CRC(b0173646) SHA1(1ba4edc033e0e4f5a1e02987e9f6b8b1650b46d7) )
	ROM_LOAD32_BYTE( "epr14105.25", 0x000003, 0x040000, CRC(614843b6) SHA1(d4f2cd3b024f7152d6e89237f0da06adea2efe57) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr13511.36", 0x000000, 0x100000, CRC(f8f15b11) SHA1(da6c2b8c3a94c4c263583f046823eaea818aff7c) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13512.32", 0x000001, 0x100000, CRC(d0be34a6) SHA1(b42a63e30f0f7a94de8a825ca93cf8efdb7a7648) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13513.27", 0x000002, 0x100000, CRC(feef1982) SHA1(bdf906317079a12c48ef4fca5bef0d437e9bf050) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13514.23", 0x000003, 0x100000, CRC(d0f9ebd1) SHA1(510ebd3d7a52bcab2debea61591770d1dff172a1) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13515.37", 0x000004, 0x100000, CRC(77bf2387) SHA1(7215dde5618e238edbe16b3007ede790785fe25f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13516.33", 0x000005, 0x100000, CRC(8c4bc62d) SHA1(3206f623ec0b7558413d063404103b183f26b488) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13517.28", 0x000006, 0x100000, CRC(1d7d84a7) SHA1(954cfccfc7250a5bead2eeba42e655d5ac82955f) , ROM_SKIP(7) )
	ROMX_LOAD( "mpr13518.24", 0x000007, 0x100000, CRC(9ea4b15d) SHA1(7dcfd6d42bb945beca8344cf92e7bd53903a824b) , ROM_SKIP(7) )

	ROM_REGION( 0x8000, "user2", 0 ) /* unused */
	ROM_LOAD( "epr14084.17", 0x00000, 0x8000, CRC(f14ed074) SHA1(e1bb23eac85e3236046527c5c7688f6f23d43aef) ) /* cabinet link */
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Slipstream

    GAME BD NO. 833-7429-01
     1. ROM BD. 837-7429-01
     2. MAIN BD. 837-7428
    A/D BD NO. 837-7536
*/
ROM_START( slipstrm )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "s32b_prg01.ic6",    0x000000, 0x080000, CRC(7d066307) SHA1(d87e04167263b435b77830db02ed58651ccc020c) )
	ROM_LOAD16_BYTE( "s32_dat00.ic14",0x100000, 0x080000, CRC(c3ff6309) SHA1(dcc857736fe0f15aa7909c3ee88a7e239c8f0228) )
	ROM_LOAD16_BYTE( "s32_dat01.ic7", 0x100001, 0x080000, CRC(0e605c81) SHA1(47c64195cab9a07b234d5a375d26168e53ffaa17) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "s32_snd00.ic35", 0x100000, 0x020000, CRC(0fee2278) SHA1(7533a03c3fc46d65dfdd07bddf1e6e0bbc368752) )
	ROM_LOAD_x2( "s32_snd01.ic31", 0x200000, 0x080000, CRC(ae7be5f2) SHA1(ba089355e64864435bcc3b0c208e4bce1ea66295) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "s32_scr00.ic38", 0x000000, 0x080000, CRC(3cbb2d0b) SHA1(b94006347b72cd60a889b0e279f62f677cedfd2e) )
	ROM_LOAD32_BYTE( "s32_scr01.ic34", 0x000002, 0x080000, CRC(4167be55) SHA1(96b34d311b318c00c3fad917e341589a70ba0a15) )
	ROM_LOAD32_BYTE( "s32_scr02.ic29", 0x000001, 0x080000, CRC(52c4bb85) SHA1(4fbee1072a19c75c25b5fd269acc75640923d69c) )
	ROM_LOAD32_BYTE( "s32_scr03.ic25", 0x000003, 0x080000, CRC(4948604a) SHA1(d5a1b9781fef7976a59a0af9b755a04fcacf9381) )

	ROM_REGION32_BE( 0x400000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "s32_obj00.ic36", 0x000000, 0x80000, CRC(cffe9e0d) SHA1(5272d54ff142de927a9abd61f3646e963c7d22c4) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj01.ic32", 0x000001, 0x80000, CRC(4ebd1383) SHA1(ce35f4d15e7904bfde55e58cdde925cba8002763) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj02.ic27", 0x000002, 0x80000, CRC(b3cf4fe2) SHA1(e13199522e1e3e8b9cfe72cc29b33f25dad542ef) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj03.ic23", 0x000003, 0x80000, CRC(c6345391) SHA1(155758097911ffca0c5c0b2a24a8033339dcfcbb) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj04.ic37", 0x000004, 0x80000, CRC(2de4288e) SHA1(8e794f79f506293edb7609187a7908516ce76849) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj05.ic33", 0x000005, 0x80000, CRC(6cfb74fb) SHA1(b74c886959910cd069427418525b23300a9b7b18) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj06.ic28", 0x000006, 0x80000, CRC(53234bf4) SHA1(1eca538dcb86e44c31310ab1ab42a2b66b69c8fe) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj07.ic24", 0x000007, 0x80000, CRC(22c129cf) SHA1(0f64680511a357038f6a556253c13fbb5417dd1a) , ROM_SKIP(7) )
ROM_END

ROM_START( slipstrh )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x2( "s32h_prg01.ic6",    0x000000, 0x080000, CRC(ab778297) SHA1(e440d44b20f2f7478ef7d86af90af5eb7b9a545a) )
	ROM_LOAD16_BYTE( "s32_dat00.ic14",0x100000, 0x080000, CRC(c3ff6309) SHA1(dcc857736fe0f15aa7909c3ee88a7e239c8f0228) )
	ROM_LOAD16_BYTE( "s32_dat01.ic7", 0x100001, 0x080000, CRC(0e605c81) SHA1(47c64195cab9a07b234d5a375d26168e53ffaa17) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "s32_snd00.ic35", 0x100000, 0x020000, CRC(0fee2278) SHA1(7533a03c3fc46d65dfdd07bddf1e6e0bbc368752) )
	ROM_LOAD_x2( "s32_snd01.ic31", 0x200000, 0x080000, CRC(ae7be5f2) SHA1(ba089355e64864435bcc3b0c208e4bce1ea66295) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "s32_scr00.ic38", 0x000000, 0x080000, CRC(3cbb2d0b) SHA1(b94006347b72cd60a889b0e279f62f677cedfd2e) )
	ROM_LOAD32_BYTE( "s32_scr01.ic34", 0x000002, 0x080000, CRC(4167be55) SHA1(96b34d311b318c00c3fad917e341589a70ba0a15) )
	ROM_LOAD32_BYTE( "s32_scr02.ic29", 0x000001, 0x080000, CRC(52c4bb85) SHA1(4fbee1072a19c75c25b5fd269acc75640923d69c) )
	ROM_LOAD32_BYTE( "s32_scr03.ic25", 0x000003, 0x080000, CRC(4948604a) SHA1(d5a1b9781fef7976a59a0af9b755a04fcacf9381) )

	ROM_REGION32_BE( 0x400000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "s32_obj00.ic36", 0x000000, 0x80000, CRC(cffe9e0d) SHA1(5272d54ff142de927a9abd61f3646e963c7d22c4) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj01.ic32", 0x000001, 0x80000, CRC(4ebd1383) SHA1(ce35f4d15e7904bfde55e58cdde925cba8002763) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj02.ic27", 0x000002, 0x80000, CRC(b3cf4fe2) SHA1(e13199522e1e3e8b9cfe72cc29b33f25dad542ef) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj03.ic23", 0x000003, 0x80000, CRC(c6345391) SHA1(155758097911ffca0c5c0b2a24a8033339dcfcbb) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj04.ic37", 0x000004, 0x80000, CRC(2de4288e) SHA1(8e794f79f506293edb7609187a7908516ce76849) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj05.ic33", 0x000005, 0x80000, CRC(6cfb74fb) SHA1(b74c886959910cd069427418525b23300a9b7b18) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj06.ic28", 0x000006, 0x80000, CRC(53234bf4) SHA1(1eca538dcb86e44c31310ab1ab42a2b66b69c8fe) , ROM_SKIP(7) )
	ROMX_LOAD( "s32_obj07.ic24", 0x000007, 0x80000, CRC(22c129cf) SHA1(0f64680511a357038f6a556253c13fbb5417dd1a) , ROM_SKIP(7) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Sonic the Hedgehog
    protected
*/
ROM_START( sonic )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr-c-87.17",        0x000000, 0x020000, CRC(25e3c27e) SHA1(8f173cd5c7c817dcccdcad9be5781cfaa081d73e) )
	ROM_LOAD_x4( "epr-c-86.8",         0x080000, 0x020000, CRC(efe9524c) SHA1(8020e734704a8f989919ee5ad92f70035de717f0) )
	ROM_LOAD16_BYTE_x2( "epr-c-81.18", 0x100000, 0x040000, CRC(65b06c25) SHA1(9f524012a7adbc71737f90fc556f0ce9adc2bcf8) )
	ROM_LOAD16_BYTE_x2( "epr-c-80.9",  0x100001, 0x040000, CRC(2db66fd2) SHA1(54582c0d5977649a38fc3a2c0fe4d7b1959abc76) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr15785.36", 0x100000, 0x040000, CRC(0fe7422e) SHA1(b7eaf4736ba155965317bb4ef3b33fc122635151) )
	ROM_LOAD( "mpr15784.35",    0x200000, 0x100000, CRC(42f06714) SHA1(30e45bb2d9b492f0c1acc4fbe1e5869f0559300b) )
	ROM_LOAD( "mpr15783.34",    0x300000, 0x100000, CRC(e4220eea) SHA1(a546c8bfc24e0695cf79c49e1a867d2595a1ed7f) )
	ROM_LOAD( "mpr15782.33",    0x400000, 0x100000, CRC(cf56b5a0) SHA1(5786228aab120c3361524ba93b418b24fd5b8ffb) ) // (this is the only rom unchanged from the prototype)

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr15789.14", 0x000000, 0x100000, CRC(4378f12b) SHA1(826e0550a3c5f2b6e59c6531ac03658a4f826651) )
	ROM_LOAD16_BYTE( "mpr15788.5",  0x000001, 0x100000, CRC(a6ed5d7a) SHA1(d30f26b452d380e7657e044e144f7dbbc4dc13e5) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr15790.32", 0x000000, 0x200000, CRC(c69d51b1) SHA1(7644fb64457855f9ed87ca25ddc28c21bcb61fd9) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15792.30", 0x000002, 0x200000, CRC(1006bb67) SHA1(38c752e634aa94b1a23c09c4dba6388b7d0358af) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15794.28", 0x000004, 0x200000, CRC(8672b480) SHA1(61659e3856cdff0b2bca190a7e60c81b86ea2089) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15796.26", 0x000006, 0x200000, CRC(95b8011c) SHA1(ebc56ae49a76d04de60b0f81506769219a9885a7) , ROM_SKIP(6)|ROM_GROUPWORD )

	// NOTE: these last 4 are in fact 16 megabit ROMs,
	// but they were dumped as 8 because the top half
	// is "FF" in all of them.
	ROMX_LOAD( "mpr15791.31", 0x800000, 0x100000, CRC(42217066) SHA1(46d14c632da1bed02bc0a637e34ab9cbf356c5de) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15793.29", 0x800002, 0x100000, CRC(75bafe55) SHA1(ad33dae062c4bdf8d17d3f6f7c333aa2e7da260e) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15795.27", 0x800004, 0x100000, CRC(7f3dad30) SHA1(84be1c31df35e1c7fef77e83d6d135378789a1ef) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr15797.25", 0x800006, 0x100000, CRC(013c6cab) SHA1(eb9b77d28815d2e225b0882706084a52b11c48ea) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Sonic the Hedgehog (prototype)
    not protected
*/
ROM_START( sonicp )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "sonpg0.bin",        0x000000, 0x20000, CRC(da05dcbb) SHA1(c2ced1f3aee92b0e531d5cd7611d4811f2ae95e7) )
	ROM_LOAD_x4( "sonpg1.bin",        0x080000, 0x20000, CRC(c57dc5c5) SHA1(5741bdd52ee7181d883129885838b36f4af8a04c) )
	ROM_LOAD16_BYTE_x2( "sonpd0.bin", 0x100000, 0x40000, CRC(a7da7546) SHA1(0a10573b21cd38d58380698bc18b0256dbb24044) )
	ROM_LOAD16_BYTE_x2( "sonpd1.bin", 0x100001, 0x40000, CRC(c30e4c70) SHA1(897b6f62921694fe3c63677908f76eaf38b7b92f) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "sonsnd0.bin", 0x100000, 0x040000, CRC(569c8d4b) SHA1(9f1f6da6adbea043cc5ad853806fcb7bf683c832) )
	ROM_LOAD( "sonsnd1.bin",    0x200000, 0x100000, CRC(f4fa5a21) SHA1(14a364ba7744ff0b44423d8d6bab990fe534ff29) )
	ROM_LOAD( "sonsnd2.bin",    0x300000, 0x100000, CRC(e1bd45a5) SHA1(b411757853d61588e5223b48b5124cc00b3d65dd) )
	ROM_LOAD( "sonsnd3.bin",    0x400000, 0x100000, CRC(cf56b5a0) SHA1(5786228aab120c3361524ba93b418b24fd5b8ffb) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "sonscl0.bin", 0x000000, 0x080000, CRC(445e31b9) SHA1(5678dfda74a09b5ac673448b222d11df4ca23aff) )
	ROM_LOAD32_BYTE( "sonscl1.bin", 0x000002, 0x080000, CRC(3d234181) SHA1(2e8c14ad36be76f5f5fc6a3ee152f1abc8bf0ddd) )
	ROM_LOAD32_BYTE( "sonscl2.bin", 0x000001, 0x080000, CRC(a5de28b2) SHA1(49a16ac10cf01b5b8802b8b015a2e403086c206a) )
	ROM_LOAD32_BYTE( "sonscl3.bin", 0x000003, 0x080000, CRC(7ce7554b) SHA1(8def3acae6baafbe9e350f18e245a9a833df5cc4) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "sonobj0.bin", 0x000000, 0x100000, CRC(ceea18e3) SHA1(f902a7e2f8e126fd7a7862c55de32ce6352a7716) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj1.bin", 0x000001, 0x100000, CRC(6bbc226b) SHA1(5ef4256b6a93891daf1349def6db3bc428e5f4f3) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj2.bin", 0x000002, 0x100000, CRC(fcd5ef0e) SHA1(e3e50d4838ac3cce41d69ee6cd31981fbe422a4b) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj3.bin", 0x000003, 0x100000, CRC(b99b42ab) SHA1(60d91dc4e8e0adc62809cd2e71833c658124fbfc) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj4.bin", 0x000004, 0x100000, CRC(c7ec1456) SHA1(d866b9dff546bd6feb43e317328ac0a2324303b9) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj5.bin", 0x000005, 0x100000, CRC(bd5da27f) SHA1(ab3043190a32b555513a29a70e01547daf698cf8) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj6.bin", 0x000006, 0x100000, CRC(313c92d1) SHA1(a5134750667502811fd755cc0974a744cdb785e1) , ROM_SKIP(7) )
	ROMX_LOAD( "sonobj7.bin", 0x000007, 0x100000, CRC(3784c507) SHA1(8ea58c52b09b84643218e26f1ec1fa0ea864346e) , ROM_SKIP(7) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Spiderman
    not protected
*/
ROM_START( spidman )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "14307",        0x000000, 0x020000, CRC(d900219c) SHA1(d59654db1fc0ec4d5f8cda9000ab4bd3bb36cdfc) )
	ROM_LOAD_x4( "14306",        0x080000, 0x020000, CRC(64379dc6) SHA1(7efc7175351186c54f141161a395e63b1cc7e7a5) )
	ROM_LOAD16_BYTE_x4( "14281", 0x100000, 0x020000, CRC(8a746c42) SHA1(fa3729ec3aa4b3c59322408146ce2cfbf5a11b98) )
	ROM_LOAD16_BYTE_x4( "14280", 0x100001, 0x020000, CRC(3c8148f7) SHA1(072b7982bb95e7a9ab77844b59020146c262488d) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "14285", 0x100000, 0x040000, CRC(25aefad6) SHA1(10153f4e773a0f55378f869eb1d85156e85f893f) )
	ROM_LOAD_x2( "14284", 0x200000, 0x080000, CRC(760542d4) SHA1(dcac73869c02fefd328bd6bdbcbdb3b68b0647da) )
	ROM_LOAD_x2( "14283", 0x300000, 0x080000, CRC(c863a91c) SHA1(afdc76bbb9b207cfcb47d437248a757d03212f4e) )
	ROM_LOAD_x2( "14282", 0x400000, 0x080000, CRC(ea20979e) SHA1(9b70ef055da8c7c56da54b7edef2379678e7c50f) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "14291", 0x000000, 0x100000, CRC(490f95a1) SHA1(f220788670b76164ac414ed9b16a422f719be267) )
	ROM_LOAD32_BYTE( "14290", 0x000002, 0x100000, CRC(a144162d) SHA1(d43f12dd9f690cdfcebb6c7b515ff7dc7dcaa377) )
	ROM_LOAD32_BYTE( "14289", 0x000001, 0x100000, CRC(38570582) SHA1(a9d810a02a1f5a6849c79d65fbebff21a4b82b59) )
	ROM_LOAD32_BYTE( "14288", 0x000003, 0x100000, CRC(3188b636) SHA1(bc0adeeca5040caa563ee1e0eded9c323ca23446) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "14299", 0x000000, 0x100000, CRC(ce59231b) SHA1(bcb1f11b74935694d0617ec8df66db2cc57b6219) , ROM_SKIP(7) )
	ROMX_LOAD( "14298", 0x000001, 0x100000, CRC(2745c84c) SHA1(5a0528c921cba7a1047d3a2ece79925103d719a1) , ROM_SKIP(7) )
	ROMX_LOAD( "14297", 0x000002, 0x100000, CRC(29cb9450) SHA1(7dc38d23a2f0cee2f4edde05c1a6f0dc83f331db) , ROM_SKIP(7) )
	ROMX_LOAD( "14296", 0x000003, 0x100000, CRC(9d8cbd31) SHA1(55a9f9ec9029157da033e69664b58e694a28db47) , ROM_SKIP(7) )
	ROMX_LOAD( "14295", 0x000004, 0x100000, CRC(29591f50) SHA1(1ac4ceaf74892e30f210ad77024eb441c5e4a959) , ROM_SKIP(7) )
	ROMX_LOAD( "14294", 0x000005, 0x100000, CRC(fa86b794) SHA1(7b6907e5734fbf2fba7bcc213a8551fec5e9f3d5) , ROM_SKIP(7) )
	ROMX_LOAD( "14293", 0x000006, 0x100000, CRC(52899269) SHA1(ff809ff88701109e0ca79e785a61402d97335cec) , ROM_SKIP(7) )
	ROMX_LOAD( "14292", 0x000007, 0x100000, CRC(41f71443) SHA1(351d40d6159cb5b792519bce5d16490965800cfb) , ROM_SKIP(7) )
ROM_END

/**************************************************************************************************************************
    Spiderman (U.S.)
    not protected
*/
ROM_START( spidmanu )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "14303",        0x000000, 0x020000, CRC(7f1bd28f) SHA1(cff57e66d09682baf44aace99d698ad305f6a3d5) )
	ROM_LOAD_x4( "14302",        0x080000, 0x020000, CRC(d954c40a) SHA1(436c81779274861de79dc6ce2c0fcc65bfd52098) )
	ROM_LOAD16_BYTE_x4( "14281", 0x100000, 0x020000, CRC(8a746c42) SHA1(fa3729ec3aa4b3c59322408146ce2cfbf5a11b98) )
	ROM_LOAD16_BYTE_x4( "14280", 0x100001, 0x020000, CRC(3c8148f7) SHA1(072b7982bb95e7a9ab77844b59020146c262488d) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "14285", 0x100000, 0x040000, CRC(25aefad6) SHA1(10153f4e773a0f55378f869eb1d85156e85f893f) )
	ROM_LOAD_x2( "14284", 0x200000, 0x080000, CRC(760542d4) SHA1(dcac73869c02fefd328bd6bdbcbdb3b68b0647da) )
	ROM_LOAD_x2( "14283", 0x300000, 0x080000, CRC(c863a91c) SHA1(afdc76bbb9b207cfcb47d437248a757d03212f4e) )
	ROM_LOAD_x2( "14282", 0x400000, 0x080000, CRC(ea20979e) SHA1(9b70ef055da8c7c56da54b7edef2379678e7c50f) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD32_BYTE( "14291", 0x000000, 0x100000, CRC(490f95a1) SHA1(f220788670b76164ac414ed9b16a422f719be267) )
	ROM_LOAD32_BYTE( "14290", 0x000002, 0x100000, CRC(a144162d) SHA1(d43f12dd9f690cdfcebb6c7b515ff7dc7dcaa377) )
	ROM_LOAD32_BYTE( "14289", 0x000001, 0x100000, CRC(38570582) SHA1(a9d810a02a1f5a6849c79d65fbebff21a4b82b59) )
	ROM_LOAD32_BYTE( "14288", 0x000003, 0x100000, CRC(3188b636) SHA1(bc0adeeca5040caa563ee1e0eded9c323ca23446) )

	ROM_REGION32_BE( 0x800000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "14299", 0x000000, 0x100000, CRC(ce59231b) SHA1(bcb1f11b74935694d0617ec8df66db2cc57b6219) , ROM_SKIP(7) )
	ROMX_LOAD( "14298", 0x000001, 0x100000, CRC(2745c84c) SHA1(5a0528c921cba7a1047d3a2ece79925103d719a1) , ROM_SKIP(7) )
	ROMX_LOAD( "14297", 0x000002, 0x100000, CRC(29cb9450) SHA1(7dc38d23a2f0cee2f4edde05c1a6f0dc83f331db) , ROM_SKIP(7) )
	ROMX_LOAD( "14296", 0x000003, 0x100000, CRC(9d8cbd31) SHA1(55a9f9ec9029157da033e69664b58e694a28db47) , ROM_SKIP(7) )
	ROMX_LOAD( "14295", 0x000004, 0x100000, CRC(29591f50) SHA1(1ac4ceaf74892e30f210ad77024eb441c5e4a959) , ROM_SKIP(7) )
	ROMX_LOAD( "14294", 0x000005, 0x100000, CRC(fa86b794) SHA1(7b6907e5734fbf2fba7bcc213a8551fec5e9f3d5) , ROM_SKIP(7) )
	ROMX_LOAD( "14293", 0x000006, 0x100000, CRC(52899269) SHA1(ff809ff88701109e0ca79e785a61402d97335cec) , ROM_SKIP(7) )
	ROMX_LOAD( "14292", 0x000007, 0x100000, CRC(41f71443) SHA1(351d40d6159cb5b792519bce5d16490965800cfb) , ROM_SKIP(7) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Stadium Cross (Multi-32)
    not protected
*/
ROM_START( scross )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x2( "epr-15093.bin", 0x000000, 0x040000, CRC(2adc7a4b) SHA1(dca71f00d94898c0758394704d819e13482bf120) )
	ROM_LOAD32_WORD_x2( "epr-15094.bin", 0x000002, 0x040000, CRC(bbb0ae73) SHA1(0d8837706405f301adf8fa85c8d4813d7600af98) )
	ROM_LOAD32_WORD( "epr-15018.bin",    0x100000, 0x080000, CRC(3a98385e) SHA1(8088d337655030c28e290da4bbf44cb647dab66c) )
	ROM_LOAD32_WORD( "epr-15019.bin",    0x100002, 0x080000, CRC(8bf4ac83) SHA1(e594d9d9b42d0765ed8a20a40b7dd92b75124d34) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr-15192.bin", 0x100000, 0x20000, CRC(7524290b) SHA1(ee58be2c0c4293ee19622b96ca493f4ce4da0038) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	/* 1ST AND 2ND HALF IDENTICAL (all roms) */
	ROM_LOAD16_BYTE( "epr-15020.bin", 0x000000, 0x200000, CRC(65afea2f) SHA1(ad573727398bfac8e94f321be84b60e5690bfba6) )
	ROM_LOAD16_BYTE( "epr-15021.bin", 0x000001, 0x200000, CRC(27bc6969) SHA1(d6bb446becb2d36b73bca5055357a43b837afc0a) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	/* 1ST AND 2ND HALF IDENTICAL (all roms) */
	ROMX_LOAD( "epr-15022.bin", 0x000000, 0x200000, CRC(09ca9608) SHA1(cbd0138c1c7811d42b051fed6a7e3526cc4e457f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15024.bin", 0x000002, 0x200000, CRC(0dc920eb) SHA1(d24d637aa0dcd3bae779ef7e12663df81667dbf7) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15026.bin", 0x000004, 0x200000, CRC(67637c37) SHA1(7c250e7e9dd5c07da4fa35bacdfcecd5e8fa4ec7) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15028.bin", 0x000006, 0x200000, CRC(9929abdc) SHA1(34b6624ddd3a0aedec0a2b433643a37f745ec66d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15023.bin", 0x800000, 0x200000, CRC(0e42a2bb) SHA1(503214caf5fa9a2324b61e04f378fd1a790322df) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15025.bin", 0x800002, 0x200000, CRC(0c677fc6) SHA1(fc2207008417072e7ee91f722797d827e150ce2d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15027.bin", 0x800004, 0x200000, CRC(d6d077f9) SHA1(928cefae9ae58239fbffb1dcee282c6ac1e661fe) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15029.bin", 0x800006, 0x200000, CRC(707af749) SHA1(fae5325c983df3cf198878220ad88d47339ac512) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	/* 1ST AND 2ND HALF IDENTICAL (all roms, are these OK?) */
	ROM_LOAD("epr-15031.bin", 0x000000, 0x200000, CRC(663a7fd2) SHA1(b4393a687225b075db21960d19a6ddd7a9d7d086) )
	ROM_LOAD("epr-15032.bin", 0x200000, 0x200000, CRC(cb709f3d) SHA1(3962c8b5907d1f8f611f58ddac693cc47364a79c) )
ROM_END

ROM_START( scrossu )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x2( "epr-15091.bin", 0x000000, 0x040000, CRC(2c572293) SHA1(6377a6eb6084f7332ce6eeaaf0c37200da792d0c) )
	ROM_LOAD32_WORD_x2( "epr-15092.bin", 0x000002, 0x040000, CRC(6e3e175a) SHA1(feaca0720646e2a4b78b376e99dc86788adb98e7) )
	ROM_LOAD32_WORD( "epr-15018.bin",    0x100000, 0x080000, CRC(3a98385e) SHA1(8088d337655030c28e290da4bbf44cb647dab66c) )
	ROM_LOAD32_WORD( "epr-15019.bin",    0x100002, 0x080000, CRC(8bf4ac83) SHA1(e594d9d9b42d0765ed8a20a40b7dd92b75124d34) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr-15192.bin", 0x100000, 0x20000, CRC(7524290b) SHA1(ee58be2c0c4293ee19622b96ca493f4ce4da0038) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	/* 1ST AND 2ND HALF IDENTICAL (all roms) */
	ROM_LOAD16_BYTE( "epr-15020.bin", 0x000000, 0x200000, CRC(65afea2f) SHA1(ad573727398bfac8e94f321be84b60e5690bfba6) )
	ROM_LOAD16_BYTE( "epr-15021.bin", 0x000001, 0x200000, CRC(27bc6969) SHA1(d6bb446becb2d36b73bca5055357a43b837afc0a) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	/* 1ST AND 2ND HALF IDENTICAL (all roms) */
	ROMX_LOAD( "epr-15022.bin", 0x000000, 0x200000, CRC(09ca9608) SHA1(cbd0138c1c7811d42b051fed6a7e3526cc4e457f) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15024.bin", 0x000002, 0x200000, CRC(0dc920eb) SHA1(d24d637aa0dcd3bae779ef7e12663df81667dbf7) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15026.bin", 0x000004, 0x200000, CRC(67637c37) SHA1(7c250e7e9dd5c07da4fa35bacdfcecd5e8fa4ec7) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15028.bin", 0x000006, 0x200000, CRC(9929abdc) SHA1(34b6624ddd3a0aedec0a2b433643a37f745ec66d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15023.bin", 0x800000, 0x200000, CRC(0e42a2bb) SHA1(503214caf5fa9a2324b61e04f378fd1a790322df) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15025.bin", 0x800002, 0x200000, CRC(0c677fc6) SHA1(fc2207008417072e7ee91f722797d827e150ce2d) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15027.bin", 0x800004, 0x200000, CRC(d6d077f9) SHA1(928cefae9ae58239fbffb1dcee282c6ac1e661fe) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "epr-15029.bin", 0x800006, 0x200000, CRC(707af749) SHA1(fae5325c983df3cf198878220ad88d47339ac512) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	/* 1ST AND 2ND HALF IDENTICAL (all roms, are these OK?) */
	ROM_LOAD("epr-15031.bin", 0x000000, 0x200000, CRC(663a7fd2) SHA1(b4393a687225b075db21960d19a6ddd7a9d7d086) )
	ROM_LOAD("epr-15032.bin", 0x200000, 0x200000, CRC(cb709f3d) SHA1(3962c8b5907d1f8f611f58ddac693cc47364a79c) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Super Visual Football
    protected via FD1149 317-0222
*/
ROM_START( svf )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr16872a.17",    0x000000, 0x020000, CRC(1f383b00) SHA1(c3af01743de5ff09ada19879902842efdbceb595) )
	ROM_LOAD_x4( "epr16871a.8",     0x080000, 0x020000, CRC(f7061bd7) SHA1(b46f4f2ecda8f521c0a91f2f2c2445b72cbc2874) )
	ROM_LOAD16_BYTE( "epr16865.18", 0x100000, 0x080000, CRC(9198ca9f) SHA1(0f6271ce8a07e4ab7fdce38964055510f2ebfd4e) )
	ROM_LOAD16_BYTE( "epr16864.9",  0x100001, 0x080000, CRC(201a940e) SHA1(e19d76141844dbdedee0698ea50edbb898ab55e9) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x8( "epr16866.36", 0x100000, 0x020000, CRC(74431350) SHA1(d3208b595423b5b0f25ee90db213112a09906f8f) )
	ROM_LOAD( "mpr16779.35",    0x200000, 0x100000, CRC(7055e859) SHA1(cde27fa4aaf0ee54063ee68794e9a6075581fff5) )
	ROM_LOAD( "mpr16778.34",    0x300000, 0x100000, CRC(feedaecf) SHA1(25c14ccb85c467dc0c8e85b61f8f86f4396c0cc7) )
	ROM_LOAD( "mpr16777.24",    0x400000, 0x100000, CRC(14b5d5df) SHA1(1b0b0a31294b1bbc16d2046b374d584a1b00a78c) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr16784.14", 0x000000, 0x100000, CRC(4608efe2) SHA1(9b41aa28f50af770e854ef9fdff1a55da7b7b131) )
	ROM_LOAD16_BYTE( "mpr16783.5", 0x000001, 0x100000, CRC(042eabe7) SHA1(a11df5c21d85f0c96dbdcaf57be37a79658ad648) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr16785.32", 0x000000, 0x200000, CRC(51f775ce) SHA1(125b40bf47304d37b92e81df5081c81d9af6c8a2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16787.30", 0x000002, 0x200000, CRC(dee7a204) SHA1(29acff4d5dd68609ac46853860788206d18262ab) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16789.28", 0x000004, 0x200000, CRC(6b6c8ad3) SHA1(97b0078c851845c31dcf0fe4b2a88393dcdf8988) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16791.26", 0x000006, 0x200000, CRC(4f7236da) SHA1(d1c29f6aa82d60a626217f1430bc8a76ef012007) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16860.31", 0x800000, 0x200000, CRC(578a7325) SHA1(75a066841fa24952d8ed5ac2d988609295f437a8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16861.29", 0x800002, 0x200000, CRC(d79c3f73) SHA1(e4360efb0964a92cfad8c458a5568709fcc81339) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16862.27", 0x800004, 0x200000, CRC(00793354) SHA1(3b37a89c5100d5f92a3567fc8d2003bc9d6fe0cd) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16863.25", 0x800006, 0x200000, CRC(42338226) SHA1(106636408d5648fb95fbaee06074c57f6a535a82) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    Super Visual Soccer
    protected via FD1149 317-0222
*/
ROM_START( svs )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr16883a.17",    0x000000, 0x020000, CRC(e1c0c3ce) SHA1(12dd8d9d1a2c2c7bf1ab652a6a6f947384d79577) )
	ROM_LOAD_x4( "epr16882a.8",     0x080000, 0x020000, CRC(1161bbbe) SHA1(3cfeed9ea947eed79aeb5674d54de45d15fb6e1f) )
	ROM_LOAD16_BYTE( "epr16865.18", 0x100000, 0x080000, CRC(9198ca9f) SHA1(0f6271ce8a07e4ab7fdce38964055510f2ebfd4e) )
	ROM_LOAD16_BYTE( "epr16864.9",  0x100001, 0x080000, CRC(201a940e) SHA1(e19d76141844dbdedee0698ea50edbb898ab55e9) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr16868.36", 0x100000, 0x040000, CRC(47aa4ec7) SHA1(baea18aaac0314f769f1e36fdbe8aedf62862544) ) /* same as jleague but with a different part number */
	ROM_LOAD( "mpr16779.35",    0x200000, 0x100000, CRC(7055e859) SHA1(cde27fa4aaf0ee54063ee68794e9a6075581fff5) )
	ROM_LOAD( "mpr16778.34",    0x300000, 0x100000, CRC(feedaecf) SHA1(25c14ccb85c467dc0c8e85b61f8f86f4396c0cc7) )
	ROM_LOAD( "mpr16777.24",    0x400000, 0x100000, CRC(14b5d5df) SHA1(1b0b0a31294b1bbc16d2046b374d584a1b00a78c) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr16784.14", 0x000000, 0x100000, CRC(4608efe2) SHA1(9b41aa28f50af770e854ef9fdff1a55da7b7b131) )
	ROM_LOAD16_BYTE( "mpr16783.5", 0x000001, 0x100000, CRC(042eabe7) SHA1(a11df5c21d85f0c96dbdcaf57be37a79658ad648) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr16785.32", 0x000000, 0x200000, CRC(51f775ce) SHA1(125b40bf47304d37b92e81df5081c81d9af6c8a2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16787.30", 0x000002, 0x200000, CRC(dee7a204) SHA1(29acff4d5dd68609ac46853860788206d18262ab) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16789.28", 0x000004, 0x200000, CRC(6b6c8ad3) SHA1(97b0078c851845c31dcf0fe4b2a88393dcdf8988) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16791.26", 0x000006, 0x200000, CRC(4f7236da) SHA1(d1c29f6aa82d60a626217f1430bc8a76ef012007) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16860.31", 0x800000, 0x200000, CRC(578a7325) SHA1(75a066841fa24952d8ed5ac2d988609295f437a8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16861.29", 0x800002, 0x200000, CRC(d79c3f73) SHA1(e4360efb0964a92cfad8c458a5568709fcc81339) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16862.27", 0x800004, 0x200000, CRC(00793354) SHA1(3b37a89c5100d5f92a3567fc8d2003bc9d6fe0cd) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16863.25", 0x800006, 0x200000, CRC(42338226) SHA1(106636408d5648fb95fbaee06074c57f6a535a82) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END

/**************************************************************************************************************************
    The JLeague '94
    protected via FD1149 317-0222
*/
ROM_START( jleague )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code + data */
	ROM_LOAD_x4( "epr16782.17",     0x000000, 0x020000, CRC(f0278944) SHA1(49e3842231ee5abdd6205b598309153d6b4ddc02) )
	ROM_LOAD_x4( "epr16781.8",      0x080000, 0x020000, CRC(7df9529b) SHA1(de3633f4941ff3877c4cb8b53e080eccea19f22e) )
	ROM_LOAD16_BYTE( "epr16776.18", 0x100000, 0x080000, CRC(e8694626) SHA1(d4318a9a6b1cc5c719bff9c25b7398dd2ea1e18b) )
	ROM_LOAD16_BYTE( "epr16775.9",  0x100001, 0x080000, CRC(e81e2c3d) SHA1(2900710f1dec6cf71875c82a56584ba45ed3a545) )

	ROM_REGION( 0x500000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr16780.36", 0x100000, 0x040000, CRC(47aa4ec7) SHA1(baea18aaac0314f769f1e36fdbe8aedf62862544) )
	ROM_LOAD( "mpr16779.35",    0x200000, 0x100000, CRC(7055e859) SHA1(cde27fa4aaf0ee54063ee68794e9a6075581fff5) )
	ROM_LOAD( "mpr16778.34",    0x300000, 0x100000, CRC(feedaecf) SHA1(25c14ccb85c467dc0c8e85b61f8f86f4396c0cc7) )
	ROM_LOAD( "mpr16777.24",    0x400000, 0x100000, CRC(14b5d5df) SHA1(1b0b0a31294b1bbc16d2046b374d584a1b00a78c) )

	ROM_REGION( 0x200000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr16784.14", 0x000000, 0x100000, CRC(4608efe2) SHA1(9b41aa28f50af770e854ef9fdff1a55da7b7b131) )
	ROM_LOAD16_BYTE( "mpr16783.5",  0x000001, 0x100000, CRC(042eabe7) SHA1(a11df5c21d85f0c96dbdcaf57be37a79658ad648) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr16785.32", 0x000000, 0x200000, CRC(51f775ce) SHA1(125b40bf47304d37b92e81df5081c81d9af6c8a2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16787.30", 0x000002, 0x200000, CRC(dee7a204) SHA1(29acff4d5dd68609ac46853860788206d18262ab) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16789.28", 0x000004, 0x200000, CRC(6b6c8ad3) SHA1(97b0078c851845c31dcf0fe4b2a88393dcdf8988) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16791.26", 0x000006, 0x200000, CRC(4f7236da) SHA1(d1c29f6aa82d60a626217f1430bc8a76ef012007) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16786.31", 0x800000, 0x200000, CRC(d08a2d32) SHA1(baac63cbacbe38e057684174040384a7152eb523) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16788.29", 0x800002, 0x200000, CRC(cd9d3605) SHA1(7c4402be1a1ddde6766cfdd5fe7e2a088c4a59e8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16790.27", 0x800004, 0x200000, CRC(2ea75746) SHA1(c91e5d678917886cc23fbef7a577c5a70665c7b2) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr16792.25", 0x800006, 0x200000, CRC(9f416072) SHA1(18107cf64f918888aa5a54432f8e9137910101b8) , ROM_SKIP(6)|ROM_GROUPWORD )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Title Fight (Multi-32)
    not protected

    GAME BOARD NO. 834-9324-02
     1. ROM BD NO. 834-9413-02
     2. MAIN BD NO. 837-8676 (SYSTEM MULTI 32)
*/
ROM_START( titlef )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x2( "epr-15388.ic37", 0x000000, 0x40000, CRC(db1eefbd) SHA1(7059a1d5c9364d836c1d922071a108cbde661e0a) )
	ROM_LOAD32_WORD_x2( "epr-15389.ic40", 0x000002, 0x40000, CRC(da9f60a3) SHA1(87a7bea04e51e3c241871e83ff7322c6a07bd106) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr-15384.ic31", 0x100000, 0x20000, CRC(0f7d208d) SHA1(5425120480f813210fae28951e8bfd5acb08ca53) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr-15381.ic3",  0x000000, 0x200000, CRC(162cc4d6) SHA1(2369d3d76ab5ef8f033aa45530ab957f0e5ff028) )
	ROM_LOAD16_BYTE( "mpr-15382.ic11", 0x000001, 0x200000, CRC(fd03a130) SHA1(040c36383ef5d8298af714958cd5b0a4c7556ae7) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr-15379.ic14", 0x000000, 0x200000, CRC(e5c74b11) SHA1(67e4460efe5dcd88ffc12024b255efc843e6a8b5) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15375.ic15", 0x000002, 0x200000, CRC(046a9b50) SHA1(2b4c53f2a0264835cb7197daa9b3461c212541e8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15371.ic10", 0x000004, 0x200000, CRC(999046c6) SHA1(37ce4e8aaf537b5366eacabaf36e4477b5624121) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15373.ic38", 0x000006, 0x200000, CRC(9b3294d9) SHA1(19542f14ce09753385a44098dfd1aaf331e7af0e) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15380.ic22", 0x800000, 0x200000, CRC(6ea0e58d) SHA1(1c4b761522157b0b9d086181ba6f6994879d8fdf) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15376.ic23", 0x800002, 0x200000, CRC(de3e05c5) SHA1(cac0d04ecd37e5836d246c0809bcfc11430df591) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15372.ic18", 0x800004, 0x200000, CRC(c187c36a) SHA1(bb55c2a768a43ef19a7847a4aa113523fee26c20) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15374.ic41", 0x800006, 0x200000, CRC(e026aab0) SHA1(75dfaef6d50c3d1d7f27aa5e44fcbc0ff2173c6f) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	ROM_LOAD( "mpr-15385.ic1", 0x000000, 0x200000, CRC(5a9b0aa0) SHA1(d208aa165f9eea05e3b8c3f406ff44374e4f6887) )
ROM_END

/**************************************************************************************************************************
    Title Fight (US)
    not protected

    GAME BOARD NO. 834-9324-01
     1. ROM BD NO. 834-9413-01
     2. MAIN BD NO. 837-8676 (SYSTEM MULTI 32)
*/
ROM_START( titlefu )
	ROM_REGION( 0x200000, "main", 0 ) /* v60 code */
	ROM_LOAD32_WORD_x4( "epr-15386.ic37", 0x000000, 0x40000, CRC(e36e2516) SHA1(b6a73d6136ff8a13831b6db0fbc8a585f2acf254) )
	ROM_LOAD32_WORD_x4( "epr-15387.ic40", 0x000002, 0x40000, CRC(e63406d3) SHA1(cd105862b2267d1d3af588cda70e6e4c1cca1da2) )

	ROM_REGION( 0x180000, "sound", 0 ) /* sound CPU */
	ROM_LOAD_x4( "epr-15384.ic31", 0x100000, 0x20000, CRC(0f7d208d) SHA1(5425120480f813210fae28951e8bfd5acb08ca53) )

	ROM_REGION( 0x400000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD16_BYTE( "mpr-15381.ic3",  0x000000, 0x200000, CRC(162cc4d6) SHA1(2369d3d76ab5ef8f033aa45530ab957f0e5ff028) )
	ROM_LOAD16_BYTE( "mpr-15382.ic11", 0x000001, 0x200000, CRC(fd03a130) SHA1(040c36383ef5d8298af714958cd5b0a4c7556ae7) )

	ROM_REGION32_BE( 0x1000000, "gfx2", 0 ) /* sprites */
	ROMX_LOAD( "mpr-15379.ic14", 0x000000, 0x200000, CRC(e5c74b11) SHA1(67e4460efe5dcd88ffc12024b255efc843e6a8b5) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15375.ic15", 0x000002, 0x200000, CRC(046a9b50) SHA1(2b4c53f2a0264835cb7197daa9b3461c212541e8) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15371.ic10", 0x000004, 0x200000, CRC(999046c6) SHA1(37ce4e8aaf537b5366eacabaf36e4477b5624121) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15373.ic38", 0x000006, 0x200000, CRC(9b3294d9) SHA1(19542f14ce09753385a44098dfd1aaf331e7af0e) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15380.ic22", 0x800000, 0x200000, CRC(6ea0e58d) SHA1(1c4b761522157b0b9d086181ba6f6994879d8fdf) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15376.ic23", 0x800002, 0x200000, CRC(de3e05c5) SHA1(cac0d04ecd37e5836d246c0809bcfc11430df591) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15372.ic18", 0x800004, 0x200000, CRC(c187c36a) SHA1(bb55c2a768a43ef19a7847a4aa113523fee26c20) , ROM_SKIP(6)|ROM_GROUPWORD )
	ROMX_LOAD( "mpr-15374.ic41", 0x800006, 0x200000, CRC(e026aab0) SHA1(75dfaef6d50c3d1d7f27aa5e44fcbc0ff2173c6f) , ROM_SKIP(6)|ROM_GROUPWORD )

	ROM_REGION( 0x400000, "sega", 0 ) /* Sega PCM sound data */
	ROM_LOAD( "mpr-15385.ic1", 0x000000, 0x200000, CRC(5a9b0aa0) SHA1(d208aa165f9eea05e3b8c3f406ff44374e4f6887) )
ROM_END



/*************************************
 *
 *  Common driver inits
 *
 *************************************/

static void segas32_common_init(read16_machine_func custom_r, write16_machine_func custom_w, const UINT8 *default_eeprom)
{
	/* reset the custom handlers and other pointers */
	custom_io_r[0] = custom_r;
	custom_io_w[0] = custom_w;
	system32_default_eeprom = default_eeprom;
	system32_prot_vblank = NULL;
}



/*************************************
 *
 *  Game-specific driver inits
 *
 *************************************/

static DRIVER_INIT( alien3 )
{
	/* alien 3 with the gun calibrated, it doesn't prompt you if its not */
	static const UINT8 alien3_default_eeprom[128] =
	{
		0x33, 0x53, 0x41, 0x32, 0x00, 0x35, 0x00, 0x00, 0x8B, 0xE8, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00,
		0x01, 0x01, 0x03, 0x00, 0x01, 0x08, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x8B, 0xE8, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x03, 0x00,
		0x01, 0x08, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	segas32_common_init(analog_custom_io_r, analog_custom_io_w, alien3_default_eeprom);
}

static READ16_HANDLER( arescue_handshake_r )
{
	return 0;
}

static READ16_HANDLER( arescue_slavebusy_r )
{
	return 0x100; // prevents master trying to synch to slave.
}

static DRIVER_INIT( arescue )
{
	segas32_common_init(analog_custom_io_r, analog_custom_io_w, NULL);
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00000, 0xa00007, 0, 0, arescue_dsp_r, arescue_dsp_w);

	dual_pcb_comms = auto_malloc(0x1000);
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x810000, 0x810fff, 0, 0, dual_pcb_comms_r, dual_pcb_comms_w);
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x818000, 0x818003, 0, 0, dual_pcb_masterslave);

	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x810000, 0x810001, 0, 0, arescue_handshake_r);
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x81000e, 0x81000f, 0, 0, arescue_slavebusy_r);
}


static DRIVER_INIT( arabfgt )
{
	segas32_common_init(extra_custom_io_r, NULL, NULL);

	/* install protection handlers */
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00100, 0xa0011f, 0, 0, arf_wakeup_protection_r);
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00000, 0xa00fff, 0, 0, arabfgt_protection_r, arabfgt_protection_w);
}


static DRIVER_INIT( brival )
{
	segas32_common_init(extra_custom_io_r, NULL, NULL);

	/* install protection handlers */
	system32_protram = auto_malloc (0x1000);
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x20ba00, 0x20ba07, 0, 0, brival_protection_r);
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00000, 0xa00fff, 0, 0, brival_protection_w);
}


static DRIVER_INIT( darkedge )
{
	segas32_common_init(extra_custom_io_r, NULL, NULL);

	/* install protection handlers */
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00000, 0xa7ffff, 0, 0, darkedge_protection_r, darkedge_protection_w);
	system32_prot_vblank = darkedge_fd1149_vblank;
}

static DRIVER_INIT( dbzvrvs )
{
	segas32_common_init(NULL, NULL, NULL);

	/* install protection handlers */
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00000, 0xa7ffff, 0, 0, dbzvrvs_protection_r, dbzvrvs_protection_w);
}

static WRITE16_HANDLER( f1en_comms_echo_w )
{
	// pretend that slave is following master op, enables attract mode video with sound
	if (ACCESSING_BITS_0_7)
		program_write_byte( 0x810049, data );
}

static DRIVER_INIT( f1en )
{
	segas32_common_init(analog_custom_io_r, analog_custom_io_w, NULL);

	dual_pcb_comms = auto_malloc(0x1000);
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x810000, 0x810fff, 0, 0, dual_pcb_comms_r, dual_pcb_comms_w);
	memory_install_read16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x818000, 0x818003, 0, 0, dual_pcb_masterslave);

	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x810048, 0x810049, 0, 0, f1en_comms_echo_w);
}


static DRIVER_INIT( f1lap )
{
	segas32_common_init(analog_custom_io_r, analog_custom_io_w, NULL);
}


static DRIVER_INIT( ga2 )
{
	segas32_common_init(extra_custom_io_r, NULL, NULL);

	decrypt_ga2_protrom(machine);
	memory_install_readwrite16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0xa00000, 0xa00fff, 0, 0, ga2_dpram_r, ga2_dpram_w);
}


static DRIVER_INIT( harddunk )
{
	segas32_common_init(extra_custom_io_r, NULL, NULL);
}


static DRIVER_INIT( holo )
{
	segas32_common_init(NULL, NULL, NULL);
}


static DRIVER_INIT( jpark )
{
	/* Temp. Patch until we emulate the 'Drive Board', thanks to Malice */
	UINT16 *pROM = (UINT16 *)memory_region(machine, "main");

	segas32_common_init(analog_custom_io_r, analog_custom_io_w, NULL);

	pROM[0xC15A8/2] = 0xCD70;
	pROM[0xC15AA/2] = 0xD8CD;
}


static DRIVER_INIT( orunners )
{
	segas32_common_init(analog_custom_io_r, orunners_custom_io_w, NULL);
}


static DRIVER_INIT( radm )
{
	/* put radmobile in 'upright' mode since we don't emulate the motor */
	static const UINT8 radm_default_eeprom[128] =
	{
		0x45, 0x53, 0x41, 0x47, 0x83, 0x01, 0x00, 0x01, 0x03, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0xFF, 0x01, 0x01, 0x01, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x60, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x45, 0x53, 0x41, 0x47, 0x83, 0x01, 0x00, 0x01, 0x03, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0xFF, 0x01, 0x01, 0x01, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x60, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00
	};

	segas32_common_init(analog_custom_io_r, analog_custom_io_w, radm_default_eeprom);
}


static DRIVER_INIT( radr )
{
	/* set rad rally to 1 coin 1 credit otherwise it'll boot into freeplay by default which isn't ideal ;-) */
	static const UINT8 radr_default_eeprom[128] =
	{
		0x45, 0x53, 0x41, 0x47, 0x00, 0x03, 0x00, 0x01, 0x04, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0xFF, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x75, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
		0x45, 0x53, 0x41, 0x47, 0x00, 0x03, 0x00, 0x01, 0x04, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x01, 0xFF, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x75, 0x07, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00
	};

	segas32_common_init(analog_custom_io_r, analog_custom_io_w, radr_default_eeprom);
}


static DRIVER_INIT( scross )
{
	segas32_common_init(analog_custom_io_r, analog_custom_io_w, NULL);
	memory_install_write8_handler(machine, 1, ADDRESS_SPACE_PROGRAM, 0xb0, 0xbf, 0, 0, scross_bank_w);
}


static DRIVER_INIT( slipstrm )
{
	segas32_common_init(analog_custom_io_r, analog_custom_io_w, NULL);
}


static DRIVER_INIT( sonic )
{
	segas32_common_init(sonic_custom_io_r, sonic_custom_io_w, NULL);

	/* install protection handlers */
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x20E5C4, 0x20E5C5, 0, 0, sonic_level_load_protection);
}


static DRIVER_INIT( sonicp )
{
	segas32_common_init(sonic_custom_io_r, sonic_custom_io_w, NULL);
}


static DRIVER_INIT( spidman )
{
	segas32_common_init(extra_custom_io_r, NULL, NULL);
}


static DRIVER_INIT( svf )
{
	segas32_common_init(NULL, NULL, NULL);
}


static DRIVER_INIT( jleague )
{
	segas32_common_init(NULL, NULL, NULL);
	memory_install_write16_handler(machine, 0, ADDRESS_SPACE_PROGRAM, 0x20F700, 0x20F705, 0, 0, jleague_protection_w);
}


static DRIVER_INIT( titlef )
{
	segas32_common_init(NULL, NULL, NULL);
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1992, arescue,  0,        system32,     arescue,  arescue,  ROT0, "Sega",   "Air Rescue", GAME_IMPERFECT_GRAPHICS )
GAME( 1993, alien3,   0,        system32,     alien3,   alien3,   ROT0, "Sega",   "Alien3: The Gun (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1993, alien3u,  alien3,   system32,     alien3,   alien3,   ROT0, "Sega",   "Alien3: The Gun (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, arabfgt,  0,        system32,     arabfgt,  arabfgt,  ROT0, "Sega",   "Arabian Fight (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, arabfgtu, arabfgt,  system32,     arabfgtu, arabfgt,  ROT0, "Sega",   "Arabian Fight (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, arabfgtj, arabfgt,  system32,     arabfgt,  arabfgt,  ROT0, "Sega",   "Arabian Fight (Japan)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, brival,   0,        system32,     brival,   brival,   ROT0, "Sega",   "Burning Rival (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, brivalj,  brival,   system32,     brival,   brival,   ROT0, "Sega",   "Burning Rival (Japan)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, darkedge, 0,        system32,     darkedge, darkedge, ROT0, "Sega",   "Dark Edge (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, darkedgj, darkedge, system32,     darkedge, darkedge, ROT0, "Sega",   "Dark Edge (Japan)", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, dbzvrvs,  0,        system32,     dbzvrvs,  dbzvrvs,  ROT0, "Sega / Banpresto", "Dragon Ball Z V.R.V.S.", GAME_IMPERFECT_GRAPHICS)
GAME( 1991, f1en,     0,        system32,     f1en,     f1en,     ROT0, "Sega",   "F1 Exhaust Note", GAME_IMPERFECT_GRAPHICS )
GAME( 1993, f1lap,    0,        system32,     f1lap,    f1lap,    ROT0, "Sega",   "F1 Super Lap", GAME_NOT_WORKING )
GAME( 1992, ga2,      0,        system32_v25, ga2,      ga2,      ROT0, "Sega",   "Golden Axe: The Revenge of Death Adder (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, ga2u,     ga2,      system32_v25, ga2u,     ga2,      ROT0, "Sega",   "Golden Axe: The Revenge of Death Adder (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, ga2j,     ga2,      system32_v25, ga2,      ga2,      ROT0, "Sega",   "Golden Axe: The Revenge of Death Adder (Japan)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, holo,     0,        system32,     holo,     holo,     ORIENTATION_FLIP_Y, "Sega",   "Holosseum (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, jpark,    0,        system32,     jpark,    jpark,    ROT0, "Sega",   "Jurassic Park", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, kokoroj2, 0,        system32,     radr,     radr,     ROT0, "Sega",   "Kokoroji 2", GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING) // uses an Audio CD
GAME( 1991, radm,     0,        system32,     radm,     radm,     ROT0, "Sega",   "Rad Mobile (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1991, radmu,    radm,     system32,     radm,     radm,     ROT0, "Sega",   "Rad Mobile (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1991, radr,     0,        system32,     radr,     radr,     ROT0, "Sega",   "Rad Rally (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1991, radru,    radr,     system32,     radr,     radr,     ROT0, "Sega",   "Rad Rally (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1995, slipstrm, 0,        system32,     slipstrm, slipstrm, ROT0, "Capcom", "Slipstream (950515 BRAZIL)", GAME_IMPERFECT_GRAPHICS )
GAME( 1995, slipstrh, slipstrm, system32,     slipstrm, slipstrm, ROT0, "Capcom", "Slipstream (950515 HISPANIC)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, sonic,    0,        system32,     sonic,    sonic,    ROT0, "Sega",   "SegaSonic The Hedgehog (Japan, rev. C)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, sonicp,   sonic,    system32,     sonic,    sonicp,   ROT0, "Sega",   "SegaSonic The Hedgehog (Japan, prototype)", GAME_IMPERFECT_GRAPHICS )
GAME( 1991, spidman,  0,        system32,     spidman,  spidman,  ROT0, "Sega",   "Spider-Man: The Videogame (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1991, spidmanu, spidman,  system32,     spidmanu, spidman,  ROT0, "Sega",   "Spider-Man: The Videogame (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, svf,      0,        system32,     svf,      svf,      ROT0, "Sega",   "Super Visual Football: European Sega Cup", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, svs,      svf,      system32,     svf,      svf,      ROT0, "Sega",   "Super Visual Soccer: Sega Cup (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, jleague,  svf,      system32,     svf,      jleague,  ROT0, "Sega",   "The J.League 1994 (Japan)", GAME_IMPERFECT_GRAPHICS )

GAME( 1994, harddunk, 0,        multi32,      harddunk, harddunk, ROT0, "Sega",   "Hard Dunk (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1994, harddunj, harddunk, multi32,      harddunk, harddunk, ROT0, "Sega",   "Hard Dunk (Japan)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, orunners, 0,        multi32,      orunners, orunners, ROT0, "Sega",   "OutRunners (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, orunneru, orunners, multi32,      orunners, orunners, ROT0, "Sega",   "OutRunners (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, scross,   0,        multi32,      scross,   scross,   ROT0, "Sega",   "Stadium Cross (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, scrossu,  scross,   multi32,      scross,   scross,   ROT0, "Sega",   "Stadium Cross (US)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, titlef,   0,        multi32,      titlef,   titlef,   ROT0, "Sega",   "Title Fight (World)", GAME_IMPERFECT_GRAPHICS )
GAME( 1992, titlefu,  titlef,   multi32,      titlef,   titlef,   ROT0, "Sega",   "Title Fight (US)", GAME_IMPERFECT_GRAPHICS )
