/******************************************************************************

  Sega encryption emulation                                 by Nicola Salmoria


  This encryption is an evolution of the one implemented in segacrpt.c.
  It works on more data and address bits but apart from that it's essentially
  the same.

  The encryption affects D0, D2, D4, and D6, and depends on M1, A0, A3, A6, A9,
  A12, and A14.

  The encryption consists of a permutation of the four data bits, which can also
  be inverted. Therefore there are 4! * 2^4 = 384 different possible encryptions.

  An interesting peculiarity is that four games in the list below use an almost
  identical key, just offset by one or more bytes. This leads to believe that
  keys were generated using a PRNG like in other Sega encryptions (MC8123 etc.)
  and the CPU part# used to skip the first N bytes.


  List of encrypted games currently known:

 CPU Part #         Game                   Comments
  315-5162      4D Warriors &          used I'm Sorry for k.p.a.
                Rafflesia &
                Wonder Boy (set 4)
  315-5177      Astro Flash &
                Wonder Boy (set 1)
  315-5178      Wonder Boy (set 2)     unencrypted version available
  315-5179      Robo-Wrestle 2001
  317-5000      Fantasy Zone (Sound CPU)  same key as 315-5177

  The following games seem to use the same algorithm as the above ones, but
  using a key which almost doesn't change

  317-0004      Calorie Kun            unencrypted bootleg available
  317-0005      Space Position
  317-0006      Gardia (set 1)
  317-0007      Gardia (set 2)

******************************************************************************/

#include "emu.h"
#include "segacrp2.h"


static void sega_decode_2(running_machine *machine,const char *cputag,
		const UINT8 xor_table[128],const int swap_table[128])
{
	int A;
	static const UINT8 swaptable[24][4] =
	{
		{ 6,4,2,0 }, { 4,6,2,0 }, { 2,4,6,0 }, { 0,4,2,6 },
		{ 6,2,4,0 }, { 6,0,2,4 }, { 6,4,0,2 }, { 2,6,4,0 },
		{ 4,2,6,0 }, { 4,6,0,2 }, { 6,0,4,2 }, { 0,6,4,2 },
		{ 4,0,6,2 }, { 0,4,6,2 }, { 6,2,0,4 }, { 2,6,0,4 },
		{ 0,6,2,4 }, { 2,0,6,4 }, { 0,2,6,4 }, { 4,2,0,6 },
		{ 2,4,0,6 }, { 4,0,2,6 }, { 2,0,4,6 }, { 0,2,4,6 },
	};


	address_space *space = cputag_get_address_space(machine, cputag, ADDRESS_SPACE_PROGRAM);
	UINT8 *rom = memory_region(machine, cputag);
	UINT8 *decrypted = auto_alloc_array(machine, UINT8, 0x8000);

	space->set_decrypted_region(0x0000, 0x7fff, decrypted);


	for (A = 0x0000;A < 0x8000;A++)
	{
		int row;
		UINT8 src;
		const UINT8 *tbl;


		src = rom[A];

		/* pick the translation table from bits 0, 3, 6, 9, 12 and 14 of the address */
		row = (A & 1) + (((A >> 3) & 1) << 1) + (((A >> 6) & 1) << 2)
				+ (((A >> 9) & 1) << 3) + (((A >> 12) & 1) << 4) + (((A >> 14) & 1) << 5);

		/* decode the opcodes */
		tbl = swaptable[swap_table[2*row]];
		decrypted[A] = BITSWAP8(src,7,tbl[0],5,tbl[1],3,tbl[2],1,tbl[3]) ^ xor_table[2*row];

		/* decode the data */
		tbl = swaptable[swap_table[2*row+1]];
		rom[A] = BITSWAP8(src,7,tbl[0],5,tbl[1],3,tbl[2],1,tbl[3]) ^ xor_table[2*row+1];
	}
}



void sega_315_5162_decode(running_machine *machine, const char *cputag)
{
	static const UINT8 xor_table[128] =
	{
		     0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,0x40,0x10,0x50,0x04,0x44,0x14,0x54,0x01,0x41,0x11,0x51,0x05,0x45,0x15,0x55,
		0x00,
	};

	static const int swap_table[128] =
	{
		    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
		10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
		11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
		12,
	};


	sega_decode_2(machine,cputag,xor_table,swap_table);
}


void sega_315_5177_decode(running_machine *machine, const char *cputag)
{
	static const UINT8 xor_table[128] =
	{
		0x04,0x54,0x51,0x15,0x40,0x44,0x01,0x51,0x55,0x10,0x44,0x41,
		0x05,0x55,0x50,0x14,0x41,0x45,0x00,0x50,0x54,0x11,0x45,0x40,
		0x04,0x54,0x51,0x15,0x40,0x44,0x01,0x51,0x55,0x10,0x44,0x41,
		0x05,0x55,0x50,0x14,0x41,0x45,0x00,0x50,0x54,0x11,0x45,0x40,
		0x04,0x54,0x51,0x15,0x40,0x44,0x01,0x51,0x55,0x10,0x44,0x41,
		0x05,0x55,0x50,0x14,

		0x04,0x54,0x51,0x15,0x40,0x44,0x01,0x51,0x55,0x10,0x44,0x41,
		0x05,0x55,0x50,0x14,0x41,0x45,0x00,0x50,0x54,0x11,0x45,0x40,
		0x04,0x54,0x51,0x15,0x40,0x44,0x01,0x51,0x55,0x10,0x44,0x41,
		0x05,0x55,0x50,0x14,0x41,0x45,0x00,0x50,0x54,0x11,0x45,0x40,
		0x04,0x54,0x51,0x15,0x40,0x44,0x01,0x51,0x55,0x10,0x44,0x41,
		0x05,0x55,0x50,0x14,
	};

	static const int swap_table[128] =
	{
		0,0,0,0,
		1,1,1,1,1,
		2,2,2,2,2,
		3,3,3,3,
		4,4,4,4,4,
		5,5,5,5,5,
		6,6,6,6,6,
		7,7,7,7,7,
		8,8,8,8,
		9,9,9,9,9,
		10,10,10,10,10,
		11,11,11,11,11,
		12,12,12,12,12,
		13,13,

		8,8,8,8,
		9,9,9,9,9,
		10,10,10,10,10,
		11,11,11,11,
		12,12,12,12,12,
		13,13,13,13,13,
		14,14,14,14,14,
		15,15,15,15,15,
		16,16,16,16,
		17,17,17,17,17,
		18,18,18,18,18,
		19,19,19,19,19,
		20,20,20,20,20,
		21,21,
	};


	sega_decode_2(machine,cputag,xor_table,swap_table);
}


void sega_315_5178_decode(running_machine *machine, const char *cputag)
{
	static const UINT8 xor_table[128] =
	{
		0x00,0x55,0x45,0x05,0x11,0x41,0x01,0x14,0x44,0x50,0x10,
		0x00,0x55,0x15,0x05,0x51,0x41,0x01,0x14,0x44,0x04,0x10,
		0x40,0x55,0x15,0x05,0x51,0x11,
		0x01,0x54,0x44,0x04,0x10,0x40,0x00,0x15,0x45,0x51,0x11,
		0x01,0x54,0x14,0x04,0x50,0x40,0x00,0x15,0x45,0x05,0x11,
		0x41,0x54,0x14,0x04,0x50,0x10,
		0x00,0x55,0x45,0x05,0x11,0x41,0x01,0x14,

		0x00,0x55,0x45,0x05,0x11,0x41,0x01,0x14,0x44,0x50,0x10,
		0x00,0x55,0x15,0x05,0x51,0x41,0x01,0x14,0x44,0x04,0x10,
		0x40,0x55,0x15,0x05,0x51,0x11,
		0x01,0x54,0x44,0x04,0x10,0x40,0x00,0x15,0x45,0x51,0x11,
		0x01,0x54,0x14,0x04,0x50,0x40,0x00,0x15,0x45,0x05,0x11,
		0x41,0x54,0x14,0x04,0x50,0x10,
		0x00,0x55,0x45,0x05,0x11,0x41,0x01,0x14,
	};

	static const int swap_table[128] =
	{
		 2,
		 3, 5, 7, 1, 3, 5, 7, 1, 3, 5, 7,
		 0, 2, 4, 6, 0, 2, 4, 6, 0, 2, 4,
		 5, 7, 1, 3, 5, 7, 1, 3, 5, 7, 1, 3,
		 4, 6, 0, 2, 4, 6, 0, 2, 4, 6,
		 8,
		 1, 3, 5, 7, 1, 3, 5, 7, 1, 3, 5,
		 6, 0, 2, 4, 6, 0, 2,

		10,
		11,13,15, 9,11,13,15, 9,11,13,15,
		 8,10,12,14, 8,10,12,14, 8,10,12,
		13,15, 9,11,13,15, 9,11,13,15, 9,11,
		12,14, 8,10,12,14, 8,10,12,14,
		16,
		 9,11,13,15, 9,11,13,15, 9,11,13,
		14, 8,10,12,14, 8,10,
	};


	sega_decode_2(machine,cputag,xor_table,swap_table);
}


void sega_315_5179_decode(running_machine *machine, const char *cputag)
{
	static const UINT8 xor_table[128] =
	{
		0x00,0x45,0x41,0x14,0x10,0x55,0x51,0x01,0x04,0x40,0x45,0x11,0x14,0x50,
		0x00,0x05,0x41,0x44,0x10,0x15,0x51,0x54,0x04,
		0x00,0x45,0x41,0x14,0x10,0x55,0x05,0x01,0x44,0x40,0x15,0x11,0x54,0x50,
		0x00,0x05,0x41,0x44,0x10,0x15,0x51,0x01,0x04,
		0x40,0x45,0x11,0x14,0x50,0x55,0x05,0x01,0x44,0x40,0x15,0x11,0x54,0x04,
		0x00,0x45,0x41,0x14,0x50,
		0x00,0x05,0x41,0x44,0x10,0x15,0x51,0x54,0x04,
		0x00,0x45,0x41,0x14,0x50,0x55,0x05,0x01,0x44,0x40,0x15,0x11,0x54,0x50,
		0x00,0x05,0x41,0x44,0x10,0x55,0x51,0x01,0x04,
		0x40,0x45,0x11,0x14,0x50,0x55,0x05,0x01,0x44,0x40,0x15,0x51,0x54,0x04,
		0x00,0x45,0x41,0x14,0x10,0x55,0x51,0x01,0x04,
		0x40,0x45,0x11,0x54,0x50,0x00,0x05,0x41,
	};

	static const int swap_table[128] =
	{
		 8, 9,11,13,15, 0, 2, 4, 6,
		 8, 9,11,13,15, 1, 2, 4, 6,
		 8, 9,11,13,15, 1, 2, 4, 6,
		 8, 9,11,13,15, 1, 2, 4, 6,
		 8,10,11,13,15, 1, 2, 4, 6,
		 8,10,11,13,15, 1, 2, 4, 6,
		 8,10,11,13,15, 1, 3, 4, 6,
		 8,
		 7, 1, 2, 4, 6, 0, 1, 3, 5,
		 7, 1, 2, 4, 6, 0, 1, 3, 5,
		 7, 1, 2, 4, 6, 0, 2, 3, 5,
		 7, 1, 2, 4, 6, 0, 2, 3, 5,
		 7, 1, 2, 4, 6, 0, 2, 3, 5,
		 7, 1, 3, 4, 6, 0, 2, 3, 5,
		 7, 1, 3, 4, 6, 0, 2, 4, 5,
		 7,
	};


	sega_decode_2(machine,cputag,xor_table,swap_table);
}


/******************************************************************************

  These games (all 317-000x CPUs) use the same algorithm, but the key doesn't
  change much - just a shift in the table.

******************************************************************************/

static void sega_decode_317(running_machine *machine, const char *cputag, int shift)
{
	static const UINT8 xor_table[128+3] =
	{
		0x04,0x54,0x44,0x14,0x15,0x15,0x51,0x41,0x41,0x14,0x10,0x50,0x15,0x55,0x54,0x05,
		0x04,0x41,0x51,0x01,0x05,0x10,0x55,0x51,0x05,0x05,0x54,0x11,0x45,0x05,0x04,0x14,
		0x10,0x55,0x01,0x41,0x51,0x05,0x55,0x04,0x45,0x41,0x55,0x14,0x45,0x10,0x04,0x45,
		0x55,0x50,0x40,0x00,0x11,0x45,0x15,0x00,0x01,0x00,0x40,0x00,0x01,0x45,0x11,0x00,
		0x45,0x00,0x44,0x54,0x40,0x04,0x05,0x15,0x15,0x10,0x15,0x04,0x01,0x05,0x50,0x11,
		0x00,0x44,0x44,0x04,0x04,0x01,0x50,0x05,0x51,0x00,0x45,0x44,0x50,0x15,0x54,0x40,
		0x41,0x45,0x40,0x10,0x14,0x15,0x40,0x51,0x50,0x50,0x45,0x00,0x10,0x15,0x05,0x51,
		0x50,0x44,0x01,0x15,0x40,0x04,0x01,0x44,0x50,0x44,0x50,0x50,0x50,0x10,0x44,0x04,
		0x40,0x04,0x10,
	};

	static const int swap_table[128+3] =
	{
		 7, 7,12, 1,18,11, 8,23,21,17, 0,23,22, 0,21,15,
		13,19,21,20,20,12,13,10,20, 0,14,18, 6,18, 3, 5,
		 5,20,20,13, 8, 0,20,18, 4,14, 8, 5,17, 6,22,10,
		 0,21, 0, 1, 6,11,17, 9,17, 3, 9,21, 0, 4,16, 1,
		13,17,21, 5, 3, 7, 2,16,18,13, 6,19,11,23, 3,20,
		 3, 2,18,10,18,23,19,23, 3,15, 0,10, 5,12, 0, 0,
		11,22, 8,14, 8, 6, 1,15, 7,11, 2,17,10,15, 8,21,
		10, 0, 2, 6, 1, 1, 3, 1,12,18,16, 5, 0,15,17,15,
		10,20, 1,
	};

	sega_decode_2( machine, cputag, xor_table + shift, swap_table + shift );
}

void sega_317_0004_decode(running_machine *machine, const char *cputag)	{ sega_decode_317( machine, cputag, 0 ); }
void sega_317_0005_decode(running_machine *machine, const char *cputag)	{ sega_decode_317( machine, cputag, 1 ); }
void sega_317_0006_decode(running_machine *machine, const char *cputag)	{ sega_decode_317( machine, cputag, 2 ); }
void sega_317_0007_decode(running_machine *machine, const char *cputag)	{ sega_decode_317( machine, cputag, 3 ); }
