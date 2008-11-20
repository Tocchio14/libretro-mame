#include "driver.h"
#include "megadriv.h"

/*

CPU
Main cpu 68000P10
Work ram 64kb (62256 x2)
Sound cpu z80B
Sound ram 8kb (76c88-6264 x1)
Sound ic Ym2612 (identified by pins,code was been erased.Named on board as TA07)

Other ics
Microchip PIC16C57 (probably it contains the MD modified bios)
Osc 50 Mhz
There are present 3 flat-pack chips with code erased again and named TA04,TA05,TA06 on board,which i have
identified (generically) by looking the pcb as:
TA04-Intercommunication and sync generator chip
TA05-Input controller
TA06-VDP (probably MD clone) Uses 2x D41264 SIL package as video ram

ROMs

M3,M4 main program
M1,M2 graphics
All eproms are 27c040

Notes:

Dip-switch 8 x1

------------------------

This romset comes from a bootleg pcb.The game is a coin-op conversion of the one developed for the Megadrive
console.I cannot know gameplay differences since pcb is faulty.

However,hardware is totally different.It seems to be based on Sega Mega Drive hardware with cpu clock increased,
and since exists an "unlicensed" porting of the game for this system probably the "producers" are the same.


Stephh's notes (based on the game M68000 code and some tests) :

1) Useful addresses

  - 0xffff88.b = number of credits (range 0x00-0xff but display limited to 9)
  - 0xff7e3c.b = lives (range 0x30-0x39)
  - 0xffeffa.b = energy (range 0x00-0x08)
  - 0xffefe0.w = apples (range 0x30-0x39 * 2 , MSB first)
  - 0xffefe2.w = jewels (range 0x30-0x39 * 2 , MSB first)
  - 0xff7e29.b to 0xff7e2e.b = score (range 0x30-0x39 * 6) - MSDigit first
  - 0xff7e12.b to 0xff7e15.b = ??? (range 0x30-0x33 * 3 + 0x00) - MSDigit first - see below
  - 0xff7e16.b to 0xff7e19.b = ??? (range 0x30-0x33 * 3 + 0x00) - MSDigit first - see below
  - 0xff7e21.b = difficulty (range 0x00-0x02) - see below

2) Adresses notes

  - I can't tell what adresses 0xff7r12.l and 0xff7e16.l are supposed to be designed for :
    they are written once at the begining of each level (code at 0x1a9030) but I haven't found
    when they were read back (I've only played the 2 first levels though as well as the bonus level,
    but I also watched all demo levels till the end after the games full credits).
    I guess they were originally designed for bonus lives (additional and first), but no evidence.
  - 0xff7e21.b affects contents of 0xff7e3c.b, 0xffefe0.w, 0xff7e12.l and 0xff7e16.l :

                         Easy             Normal             Hard
                    0xff7e21.b=0x00   0xff7e21.b=0x01   0xff7e21.b=0x02
      0xff7e3c.b      0x32              0x31              0x30
      0xffefe0.w      0x3035            0x3032            0x3030
      0xff7e12.l      0x30313000        0x30313200        0x30313400
      0xff7e16.l      0x30303900        0x30313200        0x30313500

3) MCU notes

  - As I don't know how it is on real hardware, MCU simulation is more a guess than anything;
    anyway, the game now runs correctly (coins are handled and settings change)
  - Difficulty Dip Switches are correct (see code at 0x1b2680)
  - Coinage Dip Switches might be wrong because I don't know what the possible values can be,
    but setting them the way I did isn't that bad (see code at 0x1b2a50)
  - It's possible that writes to 0x220000 are in fact a mask for what is read back from 0x330000,
    but I haven't found any formula (thus the "lame" read/write handlers you may investigate)

4) Controls notes

  - This game is a one player only game (same as the MegaDrive version);
    that's why I've "blanked" player 2 inputs which are never read.
  - I've labelled the buttons the same way as in 'g_aladj' with default options.

5) MegaDrive comparaison ('g_aladj' in HazeMD)

  - There is no "OPTIONS" menu as the difficulty is handled via the MCU / Dip Switches.
    Some code has been patched but most is still there (see the texts in the ROM ares);
    Unfortunately, there seems to be no way to access them (no "service" button).
  - Even with the same settings (same value for 0xff7e21.b), lives and apples,
    as well as contents of 0xff7e12.l and 0xff7e16.l are really different) !
    Here is the same data as above for 'g_aladj' :

                         Easy             Normal             Hard
                    0xff7e21.b=0x00   0xff7e21.b=0x01   0xff7e21.b=0x02
      0xff7e3c.b      0x35              0x33              0x32
      0xffefe0.w      0x3135            0x3130            0x3035
      0xff7e12.l      0x30303600        0x30303800        0x30313000
      0xff7e16.l      0x30303300        0x30303600        0x30303900

    But what makes the arcade version much harder is how energy is handled : in 'g_aladj', you can
    be hit 8 times before you lose a life, while in 'aladbl', you lose a life as soon as you are hit !
    This is done via code change at 0x1aee3c and patched code at 0x1afc00 :

      diff aladbl.asm g_aladj.asm

      < 1AEE3C: 4EB9 001A FC00             jsr     $1afc00.l
      > 1AEE3C: 5339 00FF EFFA             subq.b  #1, $ffeffa.l

      < 1AFC00: 0C39 0001 00FF F57C        cmpi.b  #$1, $fff57c.l
      < 1AFC08: 6700 000A                  beq     $1afc14
      < 1AFC0C: 4239 00FF EFFA             clr.b   $ffeffa.l
      < 1AFC12: 4E75                       rts
      < 1AFC14: 5339 00FF EFFA             subq.b  #1, $ffeffa.l
      < 1AFC1A: 4E75                       rts

    Surprisingly, when you are in "demo mode", player can be again be hit 8 times
    before losing a life (this is the purpose of the 0xfff57c "flag") !

*/

ROM_START( aladbl )
	ROM_REGION( 0x400000, "main", 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "m1.bin", 0x000001, 0x080000,  CRC(5e2671e4) SHA1(54705c7614fc7b5a1065478fa41f51dd1d8045b7) )
	ROM_LOAD16_BYTE( "m2.bin", 0x000000, 0x080000,  CRC(142a0366) SHA1(6c94aa9936cd11ccda503b52019a6721e64a32f0) )
	ROM_LOAD16_BYTE( "m3.bin", 0x100001, 0x080000,  CRC(0feeeb19) SHA1(bd567a33077ab9997871d21736066140d50e3d70) )
	ROM_LOAD16_BYTE( "m4.bin", 0x100000, 0x080000,  CRC(bc712661) SHA1(dfd554d000399e17b4ddc69761e572195ed4e1f0))
ROM_END

static WRITE16_HANDLER( aladbl_w )
{
    /*
    Values returned from the log file :
      - aladbl_w : 1b2a6c - data = 6600 (each time a coin is inserted)
      - aladbl_w : 1b2a82 - data = 0000 (each time a coin is inserted)
      - aladbl_w : 1b2d18 - data = aa00 (only once on reset)
      - aladbl_w : 1b2d42 - data = 0000 (only once on reset)
    */
	logerror("aladbl_w : %06x - data = %04x\n",cpu_get_pc(space->cpu),data);
}

static READ16_HANDLER( aladbl_r )
{
	if (cpu_get_pc(space->cpu)==0x1b2a56) return (input_port_read(space->machine, "MCU") & 0xff0f);             // coins
	if (cpu_get_pc(space->cpu)==0x1b2a72) return 0x0000;
	if (cpu_get_pc(space->cpu)==0x1b2d24) return (input_port_read(space->machine, "MCU") & 0x00f0) | 0x1200;    // difficulty
	if (cpu_get_pc(space->cpu)==0x1b2d4e) return 0x0000;

	logerror("aladbl_r : %06x\n",cpu_get_pc(space->cpu));

	return 0x0000;
}

static DRIVER_INIT( aladbl )
{
	// 220000 = writes to mcu? 330000 = reads?
	memory_install_write16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x220000, 0x220001, 0, 0, aladbl_w);
	memory_install_read16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x330000, 0x330001, 0, 0, aladbl_r);
	DRIVER_INIT_CALL(megadrij);
}

GAME( 1993, aladbl ,  0,   megadriv,    aladbl,       aladbl,  ROT0,   "bootleg / Sega", "Aladdin (bootleg of Japanese Megadrive version)", 0)
