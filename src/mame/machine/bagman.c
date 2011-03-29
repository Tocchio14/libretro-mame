/***************************************************************************

  machine.c

  Functions to emulate general aspects of the machine (RAM, ROM, interrupts,
  I/O ports)

***************************************************************************/

#include "emu.h"
#include "includes/bagman.h"


/*Creation date: 98-02-18 */
/*  A few words of comment:
**
**   What's inside of this file is a PAL16R6 emulator. Maybe someday we will
**need to use it for some other game too. We will need to make it more exact
**then (some of the functionality of this chip IS NOT implemented). However I
**have bought a book about PALs and I'm able to improve it. Just LMK.
**  Jarek Burczynski
**  bujar at mame dot net
*/


/*      64 rows x 32 columns
**  1 - fuse blown: disconnected from input (equal to 1)
**  0 - fuse not blown: connected to input (ie. x, not x, q, not q accordingly)
*/
static const UINT8 fusemap[64*32]=
{
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,1,0,1,1,0,1,1,1,0,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1,
1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,
1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


static void update_pal(bagman_state *state)
{
UINT16 rowoffs;
UINT8 row, column, val;

/*calculate all rows ANDs*/
	for (row = 0; row < 64; row++)
	{
		rowoffs = row*32;
		val = 1; /*prepare for AND */
		for (column = 0; column < 32; column++)
		{
			if ( fusemap[ rowoffs + column ] == 0 )
				val &= state->columnvalue[column];
		}
		state->andmap[row] = val;
	}

/* I/O pin #19 */
	val = 0; /*prepare for OR*/
	for (row = 1; row < 8; row++)
		val |= state->andmap[row];
	if (state->andmap[0] == 1)
	{
		state->columnvalue[2] = 1-val;
		state->columnvalue[3] = val;
		state->outvalue[0]    = 1-val;
	}
	else
	{
		/*pin is in INPUT configuration so it doesn't create output...*/
		state->columnvalue[2] = 0;
		state->columnvalue[3] = 1;
	}

/* O pin #18 (D1) */
	val = 0; /*prepare for OR*/
	for (row = 8; row < 16; row++)
		val |= state->andmap[row];
	state->columnvalue[6] = 1-val;
	state->columnvalue[7] = val;
	state->outvalue[1]    = 1-val;

/* O pin #17 (D2) */
	val = 0; /*prepare for OR*/
	for (row = 16; row < 24; row++)
		val |= state->andmap[row];
	state->columnvalue[10] = 1-val;
	state->columnvalue[11] = val;
	state->outvalue[2]     = 1-val;

/* O pin #16 (D3) */
	val = 0; /*prepare for OR*/
	for (row = 24; row < 32; row++)
		val |= state->andmap[row];
	state->columnvalue[14] = 1-val;
	state->columnvalue[15] = val;
	state->outvalue[3]     = 1-val;

/* O pin #15 (D4) */
	val = 0; /*prepare for OR*/
	for (row = 32; row < 40; row++)
		val |= state->andmap[row];
	state->columnvalue[18] = 1-val;
	state->columnvalue[19] = val;
	state->outvalue[4]     = 1-val;

/* O pin #14 (D5) */
	val = 0; /*prepare for OR*/
	for (row = 40; row < 48; row++)
		val |= state->andmap[row];
	state->columnvalue[22] = 1-val;
	state->columnvalue[23] = val;
	state->outvalue[5]     = 1-val;

/* O pin #13 (D6) */
	val = 0; /*prepare for OR*/
	for (row = 48; row < 56; row++)
		val |= state->andmap[row];
	state->columnvalue[26] = 1-val;
	state->columnvalue[27] = val;
	state->outvalue[6]     = 1-val;

/* I/O pin #12 */
	val = 0; /*prepare for OR*/
	for (row = 57; row < 64; row++)
		val |= state->andmap[row];
	if (state->andmap[56] == 1)
	{
		state->columnvalue[30] = 1-val;
		state->columnvalue[31] = val;
		state->outvalue[7]     = 1-val;
	}
	else
	{
		/*pin is in INPUT configuration so it doesn't create output...*/
		state->columnvalue[30] = 0;
		state->columnvalue[31] = 1;
	}

}


WRITE8_HANDLER( bagman_pal16r6_w )
{
	bagman_state *state = space->machine().driver_data<bagman_state>();
UINT8 line;

	line = offset * 4;
	state->columnvalue[line    ] = data & 1;
	state->columnvalue[line + 1] = 1 - (data & 1);
}

MACHINE_RESET( bagman )
{
	bagman_state *state = machine.driver_data<bagman_state>();
	address_space *space = machine.device("maincpu")->memory().space(AS_PROGRAM);
	bagman_pal16r6_w(space, 0, 1);	/*pin 2*/
	bagman_pal16r6_w(space, 1, 1);	/*pin 3*/
	bagman_pal16r6_w(space, 2, 1);	/*pin 4*/
	bagman_pal16r6_w(space, 3, 1);	/*pin 5*/
	bagman_pal16r6_w(space, 4, 1);	/*pin 6*/
	bagman_pal16r6_w(space, 5, 1);	/*pin 7*/
	bagman_pal16r6_w(space, 6, 1);	/*pin 8*/
	bagman_pal16r6_w(space, 7, 1);	/*pin 9*/
	update_pal(state);
}

READ8_HANDLER( bagman_pal16r6_r )
{
	bagman_state *state = space->machine().driver_data<bagman_state>();
	update_pal(state);
	return	(state->outvalue[6]) + (state->outvalue[5] << 1) + (state->outvalue[4] << 2) +
		(state->outvalue[3] << 3) + (state->outvalue[2] << 4) + (state->outvalue[1] << 5);

/* Bagman schematics show that this is right mapping order of PAL outputs to bits.
** This is the PAL 16R6 shown almost in the middle of the schematics.
** The /RD4 line goes low (active) whenever CPU reads from memory address a000.
*/
}
