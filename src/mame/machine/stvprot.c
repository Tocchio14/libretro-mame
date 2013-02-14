/* ST-V protection stuff */

/* todo:
 figure out properly
 clean up
 fix remaining games
 split them on a per-game basis

 Known Protected ST-V Games

 Astra Superstars (text layer gfx transfer)
 Elandoree (gfx transfer of textures)
 Final Fight Revenge (boot vectors etc.?)
 Radiant Silvergun (game start protection ?)
 Steep Slope Sliders (gfx transfer of character portraits)
 Decathlete (transfer of all gfx data)
 Tecmo World Cup '98 (tecmo logo, player movement?)


 Is this just scrambled, or also compressed somehow?  I've not had much luck
 locating the data some of the games want (for example decathlete)

 RSGun doesn't appear to make use of the data transfer features..

 The protection addresses are in the A-Bus area, this should map to the cartridge slot,
 is there something special in these game cartridges?

 Astra Superstars data were extracted from Saturn version of the game. It is not known if
 protection device has data stored inside, or they are read from roms (using decryption/decompression)

*/

/****************************************************************************************

Protection & cartridge handling

*****************************************************************************************

These are the known ST-V games that uses this area as a valid protection,I have written
the data used by the games in the various circumstances for reference:
-Astra Super Stars [astrass]
 [0]        [1]        [2]        [3]
 0x000y0000 0x00000000 0x06130027 0x01230000 test mode,char transfer (3)
 0x???????? 0x???????? 0x???????? 0x???????? attract mode
 0x000y0000 0x00000000 0x06130027 0x01230000 gameplay,char transfer (3)

-Elan Doree : Legend of Dragon [elandore]
 [0]        [1]        [2]        [3]
 No protection                               test mode
 No protection                               attract mode
 0x000y0000 0x00000000 0x****00** 0xff7f0000 gameplay,VDP-1 write (textures on humans)
 0x000y0000 0x00000000 0x****00** 0xffbf0000 gameplay,VDP-1 write (textures on humans)

 0x000y0000 0x00000000 0x****00** 0xf9ff0000 gameplay,VDP-1 write (textures on dragons)
 0x000y0000 0x00000000 0x****00** 0xfbff0000 gameplay,VDP-1 write (textures on dragons)
 0x000y0000 0x00000000 0x****00** 0xfe7f0000 gameplay,VDP-1 write (textures on dragons)
 0x000y0000 0x00000000 0x****00** 0xfd7f0000 gameplay,VDP-1 write (textures on dragons)
 0x000y0000 0x00000000 0x****00** 0xfeff0000 gameplay,VDP-1 write (textures on dragons)
 0x000y0000 0x00000000 0x****00** 0xf9bf0000 gameplay,VDP-1 write (textures on dragons)

-Final Fight Revenge [ffreveng]
 [0]        [1]        [2]        [3]
 0x000y0000 0x00000000 0x4bcc0013 0x10da0000 test mode,boot vectors at $06080000
 0x000y0000 0x00000000 0x0b780013 0x10d70000 attract mode,boot vectors at $06080000
 0x???????? 0x???????? 0x???????? 0x???????? gameplay

-Radiant Silvergun [rsgun]
 [0]        [1]        [2]        [3]
 No protection                               test mode
 0x000y0000 0x00000000 0x08000010 0x77770000 attract mode,work ram-h $60ff1ec and so on (1)
 0x???????? 0x???????? 0x???????? 0x???????? gameplay

-Steep Slope Sliders [sss]
 [0]        [1]        [2]        [3]
 No protection                               test mode
*0x000y0000 0x00000000 0x000000a6 0x2c5b0000 attract mode,VDP-1 write
*0x000y0000 0x00000000 0x000000a6 0x2c5b0000 gameplay,VDP-1 write character 1 (2)
*0x000y0000 0x00000000 0x0f9800a6 0x47f10000 gameplay,VDP-1 write character 2
*0x000y0000 0x00000000 0x1d4800a6 0xfcda0000 gameplay,VDP-1 write character 3
*0x000y0000 0x00000000 0x29e300a6 0xb5e60000 gameplay,VDP-1 write character 4
*0x000y0000 0x00000000 0x38e900a6 0x392c0000 gameplay,VDP-1 write character 5
*0x000y0000 0x00000000 0x462500a6 0x77c30000 gameplay,VDP-1 write character 6
*0x000y0000 0x00000000 0x555c00a6 0x8a620000 gameplay,VDP-1 write character 7

=========================================================================================
y = setted as a 0,then after the ctrl data is moved is toggled to 1 then again toggled
    to 0 after the reading,this bit is likely to be a "calculate protection values"
    if 1,use normal ram if 0.
* = working checks
[3,low word]AFAIK this is the cartridge area and it's read-only.
(1)That area is usually (but not always) used as system registers.
(2)Same as P.O.S.T. check,it was really simple to look-up because of that.
(3)Wrong offset,or it requires something else like a bitswap?
=========================================================================================
Protection works as a sort of data transfer,it could also be that it uses
encryption on the data used...

For now I'm writing this function with a command basis so I can work better with it.
****************************************************************************************/

#include "emu.h"
#include "stvprot.h"
#include "includes/stv.h"

static UINT32 a_bus[4];
static UINT32 ctrl_index;
static UINT32 internal_counter;
static UINT8 char_offset; //helper to jump the decoding of the NULL chars.

/************************
*
* Tecmo World Cup '98
*
************************/

/*
 0x200214
 0x20de94
 wpset 0x200214,0x20de94-0x200214,r
 dump twcup98.dmp,0x200214,0x20de94-0x200214,4,0,0
 protection tests the data 0x201220 at
 bp 0x6009a9e
 with 0x60651f8
 */

static UINT32 twcup_prot_data[8] =
{
	0x23232323, 0x23232323, 0x4c4c4c4c, 0x4c156301
};

static READ32_HANDLER( twcup98_prot_r )
{
	UINT32 *ROM = (UINT32 *)space.machine().root_device().memregion("abus")->base();

	if(a_bus[0] & 0x00010000)//protection calculation is activated
	{
		if(offset == 3)
		{
			UINT32 res;
			logerror("A-Bus control protection read at %06x with data = %08x\n",space.device().safe_pc(),a_bus[3]);
			#ifdef MAME_DEBUG
			popmessage("Prot read at %06x with data = %08x",space.device().safe_pc(),a_bus[3]);
			#endif
			switch(a_bus[3] >> 16)
			{
				case 0x1212:
					if(ctrl_index & 2)
					{
						res = (ROM[ctrl_index / 4] & 0xffff) << 16;
						res |= (ROM[(ctrl_index+4) / 4] & 0xffff0000) >> 16;
					}
					else
					{
						res = ROM[ctrl_index / 4] & 0xffff0000;
						res |= ROM[ctrl_index / 4] & 0xffff;
					}

					if(ctrl_index >= 0xD215A4+0x100c && ctrl_index < 0xD215A4+0x100c+8*4)
						res = twcup_prot_data[(ctrl_index-(0xD215A4+0x100c))/4];

					ctrl_index+=4;
					return res;
			}

		}
		return a_bus[offset];
	}
	else
	{
		if(a_bus[offset] != 0) return a_bus[offset];
		else return ROM[(0x02fffff0/4)+offset];
	}
}

static WRITE32_HANDLER ( twcup98_prot_w )
{
	COMBINE_DATA(&a_bus[offset]);
	logerror("A-Bus control protection write at %06x: [%02x] <- %08x\n",space.device().safe_pc(),offset,data);

	if(offset == 3)
	{
		int a_bus_vector;

		a_bus_vector = a_bus[2] >> 16;
		a_bus_vector|= (a_bus[2] & 0xffff) << 16;
		a_bus_vector<<= 1;

		//MAIN : 12120000  DATA : 0ad20069 Tecmo logo
		//MAIN : 12120000  DATA : e332006b title screen

		/* TODO: encrypted / compressed data.
		   Both points to a section that has a string ("TECMO" / "TITLE") */

		//printf("MAIN : %08x  DATA : %08x %08x\n",a_bus[3],a_bus[2],a_bus_vector);

		switch(a_bus[3] >> 16)
		{
			case 0x1212:
				ctrl_index = a_bus_vector;
				break;
		}

	}
	//popmessage("%04x %04x",data,offset/4);
}

void install_twcup98_protection(running_machine &machine)
{
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x4fffff0, 0x4ffffff, FUNC(twcup98_prot_r), FUNC(twcup98_prot_w));
}

/**************************
*
* Steep Slope Sliders
*
**************************/

static READ32_HANDLER( sss_prot_r )
{
	UINT32 *ROM = (UINT32 *)space.machine().root_device().memregion("abus")->base();

	if(a_bus[0] & 0x00010000)//protection calculation is activated
	{
		if(offset == 3)
		{
			UINT32 res;

			logerror("A-Bus control protection read at %06x with data = %08x\n",space.device().safe_pc(),a_bus[3]);
			#ifdef MAME_DEBUG
			popmessage("Prot read at %06x with data = %08x",space.device().safe_pc(),a_bus[3]);
			#endif
			switch(a_bus[3]>>16)
			{
				case 0x2c5b:
				case 0x47f1:
				case 0xfcda:
				case 0xb5e6:
				case 0x392c:
				case 0x77c3:
				case 0x8a62:
					if(ctrl_index & 2)
					{
						res = (ROM[ctrl_index / 4] & 0xffff) << 16;
						res |= (ROM[(ctrl_index+4) / 4] & 0xffff0000) >> 16;
					}
					else
					{
						res = ROM[ctrl_index / 4] & 0xffff0000;
						res |= ROM[ctrl_index / 4] & 0xffff;
					}
					ctrl_index+=4;
					return res;
			}
		}
		return a_bus[offset];
	}
	else
	{
		if(a_bus[offset] != 0) return a_bus[offset];
		else return ROM[(0x02fffff0/4)+offset];
	}
}

static WRITE32_HANDLER ( sss_prot_w )
{
	COMBINE_DATA(&a_bus[offset]);
	logerror("A-Bus control protection write at %06x: [%02x] <- %08x\n",space.device().safe_pc(),offset,data);
	if(offset == 3)
	{
		int a_bus_vector;

		a_bus_vector = a_bus[2] >> 16;
		a_bus_vector|= (a_bus[2] & 0xffff) << 16;
		a_bus_vector<<= 1;

		/*
MAIN : 2c5b0000  DATA : 000000a6 014c0000
MAIN : 47f10000  DATA : 0f9800a6 014c1f30
MAIN : fcda0000  DATA : 1d4800a6 014c3a90
MAIN : b5e60000  DATA : 29e300a6 014c53c6
MAIN : 392c0000  DATA : 38e900a6 014c71d2
MAIN : 77c30000  DATA : 462500a6 014c8c4a
MAIN : 8a620000  DATA : 555c00a6 014caab8
		*/

//		printf("MAIN : %08x  DATA : %08x %08x\n",a_bus[3],a_bus[2],a_bus_vector);
		switch(a_bus[3] >> 16)
		{
			/* Note: only the first value is TRUSTED (because it's tested in the code).
			   Others are hand-tuned by checking if there isn't any garbage during display. */
			case 0x2c5b: ctrl_index = (a_bus_vector-0x60054); break;
			case 0x47f1: ctrl_index = (a_bus_vector-0x56498); break;
			case 0xfcda: ctrl_index = (a_bus_vector-0x50b0c); break;
			case 0xb5e6: ctrl_index = (a_bus_vector-0x4af56); break;
			case 0x392c: ctrl_index = (a_bus_vector-0x45876); break;
			case 0x77c3: ctrl_index = (a_bus_vector-0x3fe02); break;
			case 0x8a62: ctrl_index = (a_bus_vector-0x3a784); break;
			default:
				ctrl_index = 0;
				popmessage("Unknown SSS seed %04x, contact MAMEdev",a_bus[3] >> 16);
				break;
		}

//		printf("%08x\n",ctrl_index);
	}
}

void install_sss_protection(running_machine &machine)
{
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x4fffff0, 0x4ffffff, FUNC(sss_prot_r), FUNC(sss_prot_w));
}

/*************************************
*
* Radiant Silvergun
*
*************************************/

static READ32_HANDLER( rsgun_prot_r )
{
	UINT32 *ROM = (UINT32 *)space.machine().root_device().memregion("abus")->base();

	if(a_bus[0] & 0x00010000)//protection calculation is activated
	{
		if(offset == 3)
		{
			logerror("A-Bus control protection read at %06x with data = %08x\n",space.device().safe_pc(),a_bus[3]);
			#ifdef MAME_DEBUG
			popmessage("Prot read at %06x with data = %08x",space.device().safe_pc(),a_bus[3]);
			#endif
			switch(a_bus[3])
			{
				case 0x77770000: {//rsgun
					UINT32 val =
						((ctrl_index & 0xff)<<24) |
						(((ctrl_index+1) & 0xff)<<16) |
						(((ctrl_index+2) & 0xff)<<8) |
						((ctrl_index+3) & 0xff);
					if(ctrl_index & 0x100)
						val &= 0x0f0f0f0f;
					else
						val &= 0xf0f0f0f0;

					ctrl_index += 4;
					return val;
				}
			}
		}
		return a_bus[offset];
	}
	else
	{
		if(a_bus[offset] != 0) return a_bus[offset];
		else return ROM[(0x02fffff0/4)+offset];
	}
}

static WRITE32_HANDLER ( rsgun_prot_w )
{
	COMBINE_DATA(&a_bus[offset]);
	logerror("A-Bus control protection write at %06x: [%02x] <- %08x\n",space.device().safe_pc(),offset,data);
	if(offset == 3)
	{
//		int a_bus_vector;

//		a_bus_vector = a_bus[2] >> 16;
//		a_bus_vector|= (a_bus[2] & 0xffff) << 16;
//		a_bus_vector<<= 1;
//		printf("MAIN : %08x  DATA : %08x %08x\n",a_bus[3],a_bus[2],a_bus_vector);
		switch(a_bus[3])
		{
			case 0x77770000: ctrl_index = 0; break;
		}
	}
	//popmessage("%04x %04x",data,offset/4);
}

void install_rsgun_protection(running_machine &machine)
{
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x4fffff0, 0x4ffffff, FUNC(rsgun_prot_r), FUNC(rsgun_prot_w));
}

/*************************
*
* Elandoree
*
*************************/

static READ32_HANDLER( elandore_prot_r )
{
	UINT32 *ROM = (UINT32 *)space.machine().root_device().memregion("abus")->base();

	if(a_bus[0] & 0x00010000)//protection calculation is activated
	{
		if(offset == 3)
		{
			UINT32 res;
			logerror("A-Bus control protection read at %06x with data = %08x\n",space.device().safe_pc(),a_bus[3]);
			#ifdef MAME_DEBUG
			popmessage("Prot read at %06x with data = %08x",space.device().safe_pc(),a_bus[3]);
			#endif
			switch(a_bus[3] >> 16)
			{
				default:
					if(ctrl_index & 2)
					{
						res = (ROM[ctrl_index / 4] & 0xffff) << 16;
						res |= (ROM[(ctrl_index+4) / 4] & 0xffff0000) >> 16;
					}
					else
					{
						res = ROM[ctrl_index / 4] & 0xffff0000;
						res |= ROM[ctrl_index / 4] & 0xffff;
					}
					ctrl_index+=4;
					return res;
			}
		}
		return a_bus[offset];
	}
	else
	{
		if(a_bus[offset] != 0) return a_bus[offset];
		else return ROM[(0x02fffff0/4)+offset];
	}
}

static WRITE32_HANDLER ( elandore_prot_w )
{
	COMBINE_DATA(&a_bus[offset]);
	logerror("A-Bus control protection write at %06x: [%02x] <- %08x\n",space.device().safe_pc(),offset,data);
	if(offset == 3)
	{
		int a_bus_vector;

		a_bus_vector = a_bus[2] >> 16;
		a_bus_vector|= (a_bus[2] & 0xffff) << 16;
		a_bus_vector<<= 1;

		//printf("MAIN : %08x  DATA : %08x %08x\n",a_bus[3],a_bus[2],a_bus_vector);
		switch(a_bus[3] >> 16)
		{
			default:
				ctrl_index = a_bus_vector;
				break;
		}
	}
	//popmessage("%04x %04x",data,offset/4);
}

void install_elandore_protection(running_machine &machine)
{
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x4fffff0, 0x4ffffff, FUNC(elandore_prot_r), FUNC(elandore_prot_w));
}

/*************************
*
* Final Fight Revenge
*
*************************/

/*
ffreveng protection notes
Global:
R2 is the vector read (where to jump to)
R3 is the vector pointer

Directory listing for Final Fight Revenge (Saturn Version):

In the ST-V version, most of these file names could be found at relative address 0x346a0 (0x22346a0)
Also, there's a table at 0x260000 (0x2260000), this points to offsets to the ROM (and are sent to the protection device),
and the size of it

fad      size     file name   date
000000aa 00003000  2000/2/8
000000aa 00003000  2000/2/8
000000b0 00076080 0;1 2000/2/8
0001799e 014b2000 ALY.RED;1 1999/11/9
00002350 00001900 ALYHRAM.BIN;1 2000/2/8
0000040e 00005204 ANDEND.BIN;1 1999/11/9
000001a0 0000c700 ANDORE.BIN;1 2000/2/8
000003c8 00016938 ANDORE.CRT;1 1999/11/9
000001b9 000f3528 ANDOREM.BIN;1 2000/2/8
00003525 012db000 ATN.RED;1 1999/11/9
000024e6 00001900 ATNHRAM.BIN;1 2000/2/8
00013291 0120d000 ATO.RED;1 1999/11/9
000024a0 00001100 ATOHRAM.BIN;1 2000/2/8
00000419 0000f000 BELGER.BIN;1 2000/2/8
000005f0 00013bdc BELGER.CRT;1 1999/11/9
00000437 000ce990 BELGERM.BIN;1 2000/2/8
00002611 00000894 BURGR.BIN;1 2000/2/8
00002605 00000638 CAFFE.BIN;1 2000/2/8
0000273b 000022e0 CAPCOM.BIN;1 1999/11/9
00001d9a 00012100 CAPHRAM.BIN;1 2000/2/8
00000834 00004cec CDYEND.BIN;1 1999/11/9
0000218a 00005900 CHSHRAM.BIN;1 2000/2/8
0000061a 0000b900 CODY.BIN;1 2000/2/8
00000809 00014e00 CODY.CRT;1 1999/11/9
00002857 000ff412 CODYANIM.BIN;1 1999/11/9
00002a56 00011dbc CODYAPAL.BIN;1 1999/11/9
00000632 000d31cc CODYM.BIN;1 2000/2/8
00002619 0000046c CURRY.BIN;1 2000/2/8
00000b36 00005110 DAMEND.BIN;1 1999/11/9
0000083e 0000b300 DAMND.BIN;1 2000/2/8
00000aa6 000181ae DAMND.CRT;1 1999/11/9
00000855 0010c674 DAMNDM.BIN;1 2000/2/8
00000ad9 000063c0 DDAD.BIN;1 1999/11/9
00000b2c 00004a30 DDBL.BIN;1 1999/11/9
00000b25 000034b4 DDCD.BIN;1 1999/11/9
00000ae6 00004c6c DDDM.BIN;1 1999/11/9
00000b1c 00004660 DDED.BIN;1 1999/11/9
00000b15 00003770 DDEL.BIN;1 1999/11/9
00000b0e 00003268 DDGY.BIN;1 1999/11/9
00000b06 00003c74 DDHG.BIN;1 1999/11/9
00000b00 000029a8 DDPS.BIN;1 1999/11/9
00000af9 000035e4 DDRL.BIN;1 1999/11/9
00000af0 000044a8 DDSD.BIN;1 1999/11/9
0000217a 00007d00 DEMHRAM.BIN;1 2000/2/8
0000278e 00064396 DLOOP.CRT;1 1999/11/9
00000b41 0000e200 EDDIE.BIN;1 2000/2/8
00000d25 00019a44 EDDIE.CRT;1 1999/11/9
00000b5e 000c7758 EDDIEM.BIN;1 2000/2/8
00000d5a 00005204 EDIEND.BIN;1 1999/11/9
00000d65 0000d700 ELGADO.BIN;1 2000/2/8
00001010 00018b2e ELGADO.CRT;1 1999/11/9
00000d80 0012ec64 ELGADOM.BIN;1 2000/2/8
00001043 00005204 ELGEND.BIN;1 1999/11/9
00001d79 0000e5ec ENDING.BIN;1 1999/11/9
00002bc3 0002e8f4 ENDING.CRT;1 1999/11/9
0000019e 0000005d FFEXABS.TXT;1 1999/11/9
0000019f 00000060 FFEXBIB.TXT;1 1999/11/9
0000019d 00000032 FFEXCPY.TXT;1 1999/11/9
00002615 0000062c FRIES.BIN;1 2000/2/8
00001ce1 0004b8b4 GAMEL.BIN;1 1999/11/9
000003f6 000011fc GANDRE.BIN;1 1999/11/9
0000040b 00001246 GBEL.BIN;1 1999/11/9
000003f9 00000af3 GCODY.BIN;1 1999/11/9
000003fb 00000cc4 GDAM.BIN;1 1999/11/9
000003fd 00000f6a GEDDIE.BIN;1 1999/11/9
000003ff 00000c85 GGADO.BIN;1 1999/11/9
00000401 00000d4c GGUY.BIN;1 1999/11/9
00000403 00000f16 GHAG.BIN;1 1999/11/9
00000405 00000a63 GPOISON.BIN;1 1999/11/9
00000407 00000a85 GROL.BIN;1 1999/11/9
00005b71 01156000 GRV.RED;1 1999/11/9
0000243b 00001600 GRVHRAM.BIN;1 2000/2/8
00000409 00000dad GSODOM.BIN;1 1999/11/9
0000104e 0000c300 GUY.BIN;1 2000/2/8
0000123a 00014844 GUY.CRT;1 1999/11/9
00001266 00005204 GUYEND.BIN;1 1999/11/9
00001067 000d14aa GUYM.BIN;1 2000/2/8
00001271 0000c600 HAGGAR.BIN;1 2000/2/8
00001489 00017ad8 HAGGAR.CRT;1 1999/11/9
0000128a 000e46fc HAGGARM.BIN;1 2000/2/8
000014bb 00005204 HGREND.BIN;1 1999/11/9
00002740 0001b468 HISCORE.BIN;1 1999/11/9
00002777 00000a00 HISCP.BIN;1 1999/11/9
00002779 000086c0 HISCTEX.BIN;1 1999/11/9
0000278b 00001680 HSFACE_T.BIN;1 1999/11/9
00002607 00000618 HTDOG.BIN;1 2000/2/8
00015741 010e3800 JNK.RED;1 1999/11/9
000023cd 00001900 JNKHRAM.BIN;1 2000/2/8
00002d0b 0003dfc0 KANJI.FON;1 1999/11/9
00001d98 00000d44 LOAD.BIN;1 1999/11/9
00001dbf 001dd5a8 LOGO.CPK;1 1999/11/9
00002d0a 00000400 LVLHRAM.BIN;1 2000/2/8
0000260a 000005e8 MEAT.BIN;1 2000/2/8
00007eb3 01253800 MLK.RED;1 1999/11/9
00002196 00000d00 MLKHRAM.BIN;1 2000/2/8
0000278a 00000400 NAMEP.BIN;1 1999/11/9
0000a3f0 011b8000 NPK.RED;1 1999/11/9
000022d0 00002700 NPKHRAM.BIN;1 2000/2/8
00010dc0 0121d800 NUK.RED;1 1999/11/9
00002555 00001100 NUKHRAM.BIN;1 2000/2/8
00002c21 00036f94 OVER.CRT;1 1999/11/9
000014c6 0000ab00 POISON.BIN;1 2000/2/8
000017f1 00018880 POISON.CRT;1 1999/11/9
000014dc 0017617c POISONM.BIN;1 2000/2/8
0000c7f6 01166000 PRK.RED;1 1999/11/9
00002268 00001100 PRKHRAM.BIN;1 2000/2/8
00002a82 00003940 PSCBCHR.BIN;1 1999/11/9
00002a8a 00002000 PSCBMAP.BIN;1 1999/11/9
00002a92 00000ac0 PSCRTEX.BIN;1 1999/11/9
00002a7b 000033c0 PSFCCHR.BIN;1 1999/11/9
00001824 000049d4 PSNEND.BIN;1 1999/11/9
00002a8e 00001e54 PSNMCHR.BIN;1 1999/11/9
00002a94 000037c0 PSSBCHR.BIN;1 1999/11/9
00002a9b 00002000 PSSBMAP.BIN;1 1999/11/9
00002a9f 00000200 PSSBPAL.BIN;1 1999/11/9
00002a7a 00000600 PS_PAL.BIN;1 1999/11/9
00002c8f 0003d4c0 RESIDENT.CRT;1 1999/11/9
00001c68 0003ae40 RESTEXT.BIN;1 1999/11/9
00001cde 00001220 RESTEXTB.BIN;1 1999/11/9
00001d97 00000200 RETIMEPA.BIN;1 1999/11/9
0000261b 000004e8 REVNG.BIN;1 1999/11/9
00001a5b 00005204 ROLEND.BIN;1 1999/11/9
0000182e 0000e700 ROLENTO.BIN;1 2000/2/8
00001a2d 00016bfc ROLENTO.CRT;1 1999/11/9
0000184b 000d6bf0 ROLENTOM.BIN;1 2000/2/8
000025c5 000062e4 SDDRVS.TSK;1 1999/11/9
00002eb3 002ee000 SEGA_WRN.DA;1 1998/4/30
00002b82 00020492 SELECT.CRT;1 1999/11/9
00002aa0 00070e04 SODBTEX.BIN;1 1999/11/9
00001c5d 00005154 SODEND.BIN;1 1999/11/9
00001a66 0000b100 SODOM.BIN;1 2000/2/8
00001c29 0001943a SODOM.CRT;1 1999/11/9
00001a7d 000b9e2f SODOMM.BIN;1 2000/2/8
0000260d 000004cc SUSHI.BIN;1 2000/2/8
0000261c 0008f204 TITLE.BIN;1 1999/11/9
0000eb58 010e9000 WHS.RED;1 1999/11/9
000021ff 00001100 WHSHRAM.BIN;1 2000/2/8
000003a0 00009b90 _ADCL00.BIN;1 1999/11/9
000003b4 00009b90 _ADCL01.BIN;1 1999/11/9
000023cb 00000c54 _ALYBMAP.BIN;1 1999/11/9
0000239b 00017ec0 _ALYFCHR.BIN;1 1999/11/9
00002399 00000c1a _ALYFMAP.BIN;1 1999/11/9
00002354 00000600 _ALYPAL.BIN;1 1999/11/9
0000235a 0001f400 _ALYRCHR.BIN;1 1999/11/9
00002355 0000205c _ALYRMAP.BIN;1 1999/11/9
0000252a 00015800 _ATNBCHR.BIN;1 1999/11/9
00002526 00001882 _ATNBMAP.BIN;1 1999/11/9
00002525 00000416 _ATNFMAP.BIN;1 1999/11/9
000024ea 00000600 _ATNPAL.BIN;1 1999/11/9
000024ef 0001af00 _ATNRCHR.BIN;1 1999/11/9
000024eb 00001d10 _ATNRMAP.BIN;1 1999/11/9
000024e3 000017fa _ATOBMAP.BIN;1 1999/11/9
000024e2 00000040 _ATOFCHR.BIN;1 1999/11/9
000024e1 00000006 _ATOFMAP.BIN;1 1999/11/9
000024a3 00000600 _ATOPAL.BIN;1 1999/11/9
000024a9 0001bbc0 _ATORCHR.BIN;1 1999/11/9
000024a4 00002004 _ATORMAP.BIN;1 1999/11/9
00000618 00000a20 _BGUNTX.BIN;1 1999/11/9
000005d5 0000d688 _BLTXURE.BIN;1 1999/11/9
00002613 00000a80 _BURGRTX.BIN;1 1999/11/9
00002606 000006c0 _CAFFETX.BIN;1 1999/11/9
000007d9 0000bdb0 _CDCL00.BIN;1 1999/11/9
000007f1 0000bdb0 _CDCL01.BIN;1 1999/11/9
00000ad7 000008a0 _CHSAWTX.BIN;1 1999/11/9
000025e3 00003500 _CRAT2TX.BIN;1 1999/11/9
000025ea 00002200 _CRAT3TX.BIN;1 1999/11/9
000025d5 00003300 _CRATDTX.BIN;1 1999/11/9
000025dc 00003300 _CRATNTX.BIN;1 1999/11/9
0000261a 00000500 _CURRYTX.BIN;1 1999/11/9
00000a6e 0000d918 _DMCL00.BIN;1 1999/11/9
00000a8a 0000d918 _DMCL01.BIN;1 1999/11/9
000025ef 00001b60 _DRUMTX.BIN;1 1999/11/9
00000ced 0000da3c _EDCL00.BIN;1 1999/11/9
00000d09 0000da3c _EDCL01.BIN;1 1999/11/9
00000fde 0000c404 _ELCL00.BIN;1 1999/11/9
00000ff7 0000c404 _ELCL01.BIN;1 1999/11/9
00002616 000011a0 _FRIESTX.BIN;1 1999/11/9
00001d96 00000660 _GFNTCHR.BIN;1 1999/11/9
00001264 00000880 _GKATATX.BIN;1 1999/11/9
00002488 0000bcc0 _GRVBCHR.BIN;1 1999/11/9
00002486 00000d76 _GRVBMAP.BIN;1 1999/11/9
00002483 000010fc _GRVFMAP.BIN;1 1999/11/9
0000243e 00000600 _GRVPAL.BIN;1 1999/11/9
00002444 0001f4c0 _GRVRCHR.BIN;1 1999/11/9
0000243f 00002004 _GRVRMAP.BIN;1 1999/11/9
0000120a 0000bf44 _GYCL00.BIN;1 1999/11/9
00001222 0000bf44 _GYCL01.BIN;1 1999/11/9
00001453 0000d614 _HGCL00.BIN;1 1999/11/9
0000146e 0000d614 _HGCL01.BIN;1 1999/11/9
00002608 00000880 _HTDOGTX.BIN;1 1999/11/9
000025f3 00002b60 _ICEBTX.BIN;1 1999/11/9
00001042 00000560 _JKNFTX.BIN;1 1999/11/9
00002419 00010940 _JNKBCHR.BIN;1 1999/11/9
00002417 00000982 _JNKBMAP.BIN;1 1999/11/9
00002414 0000113a _JNKFMAP.BIN;1 1999/11/9
000023d1 00000600 _JNKPAL.BIN;1 1999/11/9
000023d6 0001ee80 _JNKRCHR.BIN;1 1999/11/9
000023d2 00001bde _JNKRMAP.BIN;1 1999/11/9
000025d4 000006c0 _KATA2TX.BIN;1 1999/11/9
00001c5c 00000780 _KATANTX.BIN;1 1999/11/9
0000260b 00000cc0 _MEATTX.BIN;1 1999/11/9
000021dd 00010e80 _MLKBCHR.BIN;1 1999/11/9
000021db 00000be6 _MLKBMAP.BIN;1 1999/11/9
000021d9 00000f70 _MLKFMAP.BIN;1 1999/11/9
00002198 00000600 _MLKPAL.BIN;1 1999/11/9
0000219c 0001e440 _MLKRCHR.BIN;1 1999/11/9
00002199 000013de _MLKRMAP.BIN;1 1999/11/9
0000234d 00001416 _NPKBMAP.BIN;1 1999/11/9
0000231d 00017dc0 _NPKFCHR.BIN;1 1999/11/9
0000231a 00001406 _NPKFMAP.BIN;1 1999/11/9
000022d5 00000600 _NPKPAL.BIN;1 1999/11/9
000022db 0001f640 _NPKRCHR.BIN;1 1999/11/9
000022d6 00002202 _NPKRMAP.BIN;1 1999/11/9
0000259a 000153c0 _NUKBCHR.BIN;1 1999/11/9
00002596 00001924 _NUKBMAP.BIN;1 1999/11/9
00002594 00000f6e _NUKFMAP.BIN;1 1999/11/9
00002558 00000600 _NUKPAL.BIN;1 1999/11/9
0000255e 0001adc0 _NUKRCHR.BIN;1 1999/11/9
00002559 00002004 _NUKRMAP.BIN;1 1999/11/9
000025d3 000005a0 _NYOIBTX.BIN;1 1999/11/9
000025f9 000039e0 _OILCTX.BIN;1 1999/11/9
000014b9 00000b40 _PIPETX.BIN;1 1999/11/9
00001823 00000600 _POISNTX.BIN;1 1999/11/9
000022cc 000018fc _PRKBMAP.BIN;1 1999/11/9
000022ac 0000fd00 _PRKFCHR.BIN;1 1999/11/9
000022a8 00001d4a _PRKFMAP.BIN;1 1999/11/9
0000226b 00000600 _PRKPAL.BIN;1 1999/11/9
00002271 0001b0c0 _PRKRCHR.BIN;1 1999/11/9
0000226c 00002202 _PRKRMAP.BIN;1 1999/11/9
000017c9 00009874 _PSCL00.BIN;1 1999/11/9
000017dd 00009874 _PSCL01.BIN;1 1999/11/9
00000d59 00000180 _PSTKTX.BIN;1 1999/11/9
000019f9 0000cb2c _RLCL00.BIN;1 1999/11/9
00001a13 0000cb2c _RLCL01.BIN;1 1999/11/9
00001bf1 0000dd0c _SDCL00.BIN;1 1999/11/9
00001c0d 0000dd0c _SDCL01.BIN;1 1999/11/9
00000833 00000660 _SKNFTX.BIN;1 1999/11/9
000025d2 00000240 _SPIKETX.BIN;1 1999/11/9
0000260e 000011e0 _SUSHITX.BIN;1 1999/11/9
00002601 00001ba0 _TOMBTX.BIN;1 1999/11/9
0000224b 0000e2c0 _WHSBCHR.BIN;1 1999/11/9
00002249 00000a36 _WHSBMAP.BIN;1 1999/11/9
00002245 00001bb8 _WHSFMAP.BIN;1 1999/11/9
00002202 00000600 _WHSPAL.BIN;1 1999/11/9
00002207 0001efc0 _WHSRCHR.BIN;1 1999/11/9
00002203 00001cf4 _WHSRMAP.BIN;1 1999/11/9
*/

static READ32_HANDLER( ffreveng_prot_r )
{
	UINT32 *ROM = (UINT32 *)space.machine().root_device().memregion("abus")->base();

	if(a_bus[0] & 0x00010000)//protection calculation is activated
	{
		if(offset == 3)
		{
			#if 0
			UINT32 res;
			#endif
			logerror("A-Bus control protection read at %06x with data = %08x\n",space.device().safe_pc(),a_bus[3]);
			switch(a_bus[3] >> 16)
			{
				case 0x10da://ffreveng, boot vectors at $6080000,test mode
				case 0x10d7://ffreveng, boot vectors at $6080000,attract mode
					#if 0
					if(ctrl_index & 2)
					{
						res = (ROM[ctrl_index / 4] & 0xffff) << 16;
						res |= (ROM[(ctrl_index+4) / 4] & 0xffff0000) >> 16;
					}
					else
					{
						res = ROM[ctrl_index / 4] & 0xffff0000;
						res |= ROM[ctrl_index / 4] & 0xffff;
					}
					#endif
					ctrl_index+=4;
					return 0;
			}
		}
		return a_bus[offset];
	}
	else
	{
		if(a_bus[offset] != 0) return a_bus[offset];
		else return ROM[(0x02fffff0/4)+offset];
	}
}

static WRITE32_HANDLER ( ffreveng_prot_w )
{
	COMBINE_DATA(&a_bus[offset]);
	logerror("A-Bus control protection write at %06x: [%02x] <- %08x\n",space.device().safe_pc(),offset,data);
	if(offset == 3)
	{
		int a_bus_vector;

		a_bus_vector = a_bus[2] >> 16;
		a_bus_vector|= (a_bus[2] & 0xffff) << 16;
		a_bus_vector<<= 1;

		printf("MAIN : %08x  DATA : %08x %08x\n",a_bus[3],a_bus[2],a_bus_vector);
		switch(a_bus[3] >> 16)
		{
			case 0x10d7: ctrl_index = a_bus_vector; break;
			case 0x10da: ctrl_index = a_bus_vector; break;
			default:
				ctrl_index = 0;
		}
	}
	//popmessage("%04x %04x",data,offset/4);
}

void install_ffreveng_protection(running_machine &machine)
{
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x4fffff0, 0x4ffffff, FUNC(ffreveng_prot_r), FUNC(ffreveng_prot_w));
}

/************************
*
* Astra Super Stars
*
************************/

static READ32_HANDLER(astrass_prot_r)
{
	if ( offset == 3 && ctrl_index != -1 )
	{
		UINT32 data = 0;
		UINT32 *prot_data = (UINT32 *)space.machine().root_device().memregion("user2")->base();

		data = prot_data[ctrl_index++];

		if ( ctrl_index >= space.machine().root_device().memregion("user2")->bytes()/4 )
		{
			ctrl_index = -1;
		}

		return data;
	}
	return a_bus[offset];
}

static WRITE32_HANDLER(astrass_prot_w)
{
	COMBINE_DATA(&a_bus[0 + offset]);
	if ( offset == 3 )
	{
		ctrl_index = 0;
	}
}

void install_astrass_protection(running_machine &machine)
{
	ctrl_index = -1;
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x4fffff0, 0x4ffffff, FUNC(astrass_prot_r), FUNC(astrass_prot_w));
}

/**************************
*
* Decathlete
*
**************************/

/* Decathlete seems to be a variation on this ... not understood */
static UINT32 decathlt_protregs[4];
static UINT32 decathlt_lastcount = 0;
static UINT32 decathlt_part;
static UINT32 decathlt_prot_uploadmode=0;
static UINT32 decathlt_prot_uploadoffset=0;
static UINT16 decathlt_prottable1[24];
static UINT16 decathlt_prottable2[128];

static READ32_HANDLER( decathlt_prot_r )
{
	// the offsets written to the protection device definitely only refer to 2 of the roms
	//  it's a fair assumption to say that only those 2 are connected to the protection device
	UINT8 *ROM = (UINT8 *)space.machine().root_device().memregion("abus")->base()+0x1000000;

	if (offset==2)
	{
		UINT32 retvalue;

		retvalue = ROM[(decathlt_protregs[0]*2)-2]; decathlt_protregs[0]++;
		retvalue <<= 8;
		retvalue |= ROM[(decathlt_protregs[0]*2)+1-2];
		retvalue <<= 8;
		retvalue |= ROM[(decathlt_protregs[0]*2)-2]; decathlt_protregs[0]++;
		retvalue <<= 8;
		retvalue |= ROM[(decathlt_protregs[0]*2)+1-2];


		decathlt_lastcount++;
		logerror("blah_r %08x\n", retvalue);
		return retvalue;
	}
	else
	{
		logerror("%06x Decathlete prot R offset %04x mask %08x regs %08x, %08x, %08x, %08x\n",space.device().safe_pc(), offset, mem_mask, decathlt_protregs[0], decathlt_protregs[1], decathlt_protregs[2], decathlt_protregs[3]);
	}

	return decathlt_protregs[offset];
}


void write_prot_data(UINT32 data, UINT32 mem_mask, int offset, int which)
{
	decathlt_protregs[offset] = (data&mem_mask)|(decathlt_protregs[offset]&~mem_mask);
//  decathlt_protregs[0] = 0x0c00000/4;

	if (offset==0) // seems to set a (scrambled?) source address
	{
		decathlt_part ^=1;

		//if (decathlt_part==0) logerror("%d, last read count was %06x\n",which, decathlt_lastcount*4);
		decathlt_lastcount = 0;
		if (decathlt_part==1) logerror("%d Decathlete prot W offset %04x data %08x, %08x, >>> regs %08x <<<<, %08x, %08x, %08x\n",which, offset, data, decathlt_protregs[0], decathlt_protregs[0]*4, decathlt_protregs[1], decathlt_protregs[2], decathlt_protregs[3]);
	}

	if (offset==1) // uploads 2 tables...
	{
		if (mem_mask==0xffff0000)
		{
			if (data == 0x80000000)
			{
			//  logerror("changed to upload mode 1\n");
				decathlt_prot_uploadmode = 1;
				decathlt_prot_uploadoffset = 0;
			}
			else if (data == 0x80800000)
			{
			//  logerror("changed to upload mode 2\n");
				decathlt_prot_uploadmode = 2;
				decathlt_prot_uploadoffset = 0;
			}
			else
			{
			//  logerror("unknown upload mode\n");
				decathlt_prot_uploadmode = 2;
				decathlt_prot_uploadoffset = 0;
			}

//          logerror("ARGH! %08x %08x\n",mem_mask,data);
		}
		else if (mem_mask==0x0000ffff)
		{
			if (decathlt_prot_uploadmode==1)
			{
				if (decathlt_prot_uploadoffset>=24)
				{
				//  logerror("upload mode 1 error, too big\n");
					return;
				}

				//logerror("uploading table 1 %04x %04x\n",decathlt_prot_uploadoffset, data&0xffff);
				decathlt_prottable1[decathlt_prot_uploadoffset]=data&0xffff;
				decathlt_prot_uploadoffset++;

				{
					/* 0x18 (24) values in this table, rom data is 0x1800000 long, maybe it has
					   something to do with that? or 24-address bits?

					   uploaded values appear to be 12-bit, some are repeated
					*/

					{
						FILE* fp;
						if (which==1) fp = fopen("table1x","wb");
						else fp = fopen("table1","wb");

						{
							fwrite(&decathlt_prottable1,24,2,fp);
						}
						fclose(fp);
					}
				}

			}
			else if (decathlt_prot_uploadmode==2)
			{
				if (decathlt_prot_uploadoffset>=128)
				{
					//logerror("upload mode 2 error, too big\n");
					return;
				}

				//logerror("uploading table 2 %04x %04x\n",decathlt_prot_uploadoffset, data&0xffff);
				decathlt_prottable2[decathlt_prot_uploadoffset]=data&0xffff;
				decathlt_prot_uploadoffset++;

				{
					/* the table uploaded here is a 256 byte table with 256 unique values, remaps something? */

					{
						FILE* fp;
						if (which==1) fp = fopen("table2x","wb");
						else fp = fopen("table2","wb");

						{
							fwrite(&decathlt_prottable2,128,2,fp);
						}
						fclose(fp);
					}
				}
			}
			else
			{
			//  logerror("unknown upload mode!\n");
			}
		}
	}

	if (offset>1)
	{
	//  logerror("higher offset write\n");
	}

}

static WRITE32_HANDLER( decathlt_prot1_w )
{
	write_prot_data(data,mem_mask, offset, 0);

}

static WRITE32_HANDLER( decathlt_prot2_w )
{
	write_prot_data(data,mem_mask, offset, 1);


}

void install_decathlt_protection(running_machine &machine)
{
	/* It uploads 2 tables here, then performs what looks like a number of transfers, setting
	   a source address of some kind (scrambled?) and then making many reads from a single address */
	memset(decathlt_protregs, 0, sizeof(decathlt_protregs));
	decathlt_lastcount = 0;
	decathlt_prot_uploadmode = 0;
	decathlt_prot_uploadoffset = 0;
	decathlt_part = 1;
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x37FFFF0, 0x37FFFFF, FUNC(decathlt_prot_r), FUNC(decathlt_prot1_w));
	/* It accesses the device at this address too, with different tables, for the game textures, should it just act like a mirror, or a secondary device? */
	machine.device("maincpu")->memory().space(AS_PROGRAM).install_legacy_readwrite_handler(0x27FFFF0, 0x27FFFFF, FUNC(decathlt_prot_r), FUNC(decathlt_prot2_w));
}

void stv_register_protection_savestates(running_machine &machine)
{
	state_save_register_global_array(machine, a_bus);
	state_save_register_global(machine, ctrl_index);
	state_save_register_global(machine, internal_counter);
	state_save_register_global(machine, char_offset);
}
