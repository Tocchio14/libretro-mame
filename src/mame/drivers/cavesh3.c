/* Cave SH3 ( CAVE CV1000-B ) */
/* skeleton placeholder driver */


#include "emu.h"
#include "cpu/sh4/sh4.h"


class cavesh3_state : public driver_device
{
public:
	cavesh3_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

};


VIDEO_START(cavesh3)
{
}

SCREEN_UPDATE(cavesh3)
{
	return 0;
}


static ADDRESS_MAP_START( cavesh3_map, AS_PROGRAM, 64 )
	AM_RANGE(0x00000000, 0x001fffff) AM_ROM AM_REGION("maincpu", 0)
ADDRESS_MAP_END

static ADDRESS_MAP_START( cavesh3_port, AS_IO, 64 )
ADDRESS_MAP_END


static INPUT_PORTS_START( cavesh3 )
INPUT_PORTS_END


#define CAVE_CPU_CLOCK 133333333/4

static const struct sh4_config sh4cpu_config = {  1,  0,  1,  0,  0,  0,  1,  1,  0, CAVE_CPU_CLOCK };

static MACHINE_CONFIG_START( cavesh3, cavesh3_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SH3LE, CAVE_CPU_CLOCK)
	MCFG_CPU_CONFIG(sh4cpu_config)
	MCFG_CPU_PROGRAM_MAP(cavesh3_map)
	MCFG_CPU_IO_MAP(cavesh3_port)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))  /* not accurate */
	MCFG_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MCFG_SCREEN_SIZE(640, 480)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 0, 480-1)
	MCFG_SCREEN_UPDATE(cavesh3)

	MCFG_PALETTE_LENGTH(0x1000)

	MCFG_VIDEO_START(cavesh3)
MACHINE_CONFIG_END

/**************************************************

All roms are flash roms with no lables, so keep the
 version numbers attached to the roms that differ

**************************************************/

ROM_START( mushisam )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD("u4", 0x000000, 0x200000, CRC(0b5b30b2) SHA1(35fd1bb1561c30b311b4325bc8f4628f2fccd20b) ) /* (2004/10/12 MASTER VER.) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD("u2", 0x000000, 0x8400000, CRC(b1f826dc) SHA1(c287bd9f571d0df03d7fcbcf3c57c74ce564ab05) ) /* (2004/10/12 MASTER VER.) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD("u23", 0x000000, 0x400000, CRC(138e2050) SHA1(9e86489a4e65af5efb5495adf6d4b3e01d5b2816) )
	ROM_LOAD("u24", 0x400000, 0x400000, CRC(e3d05c9f) SHA1(130c3d62317da1729c85bd178bd51500edd73ada) )
ROM_END

ROM_START( mushisama )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD("u4", 0x000000, 0x200000, CRC(9f1c7f51) SHA1(f82ae72ec03687904ca7516887080be92365a5f3) ) /* (2004/10/12 MASTER VER) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD("u2", 0x000000, 0x8400000, CRC(2cd13810) SHA1(40e45e201b60e63a060b68d4cc767eb64cfb99c2) ) /* (2004/10/12 MASTER VER) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD("u23", 0x000000, 0x400000, CRC(138e2050) SHA1(9e86489a4e65af5efb5495adf6d4b3e01d5b2816) )
	ROM_LOAD("u24", 0x400000, 0x400000, CRC(e3d05c9f) SHA1(130c3d62317da1729c85bd178bd51500edd73ada) )
ROM_END

ROM_START( mushitam )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD("u4", 0x000000, 0x200000, CRC(4a23e6c8) SHA1(d44c287bb88e6d413a8d35d75bc1b4928ad52cdf) ) /* (2005/09/09 MASTER VER) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD("u2", 0x000000, 0x8400000, CRC(3f93ff82) SHA1(6f6c250aa7134016ffb288d056bc937ea311f538) ) /* (2005/09/09 MASTER VER) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD("u23", 0x000000, 0x400000, CRC(701a912a) SHA1(85c198946fb693d99928ea2595c84ba4d9dc8157) )
	ROM_LOAD("u24", 0x400000, 0x400000, CRC(6feeb9a1) SHA1(992711c80e660c32f97b343c2ce8184fddd7364e) )
ROM_END

ROM_START( futari10 )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD( "u4", 0x000000, 0x200000, CRC(b127dca7) SHA1(e1f518bc72fc1cdf69aefa89eafa4edaf4e84778) ) /* (2006/10/23 MASTER VER.) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD( "u2", 0x000000, 0x8400000, CRC(78ffcd0c) SHA1(0e2937edec15ce3f5741b72ebd3bbaaefffb556e) ) /* (2006/10/23 MASTER VER.) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD( "u23", 0x000000, 0x400000, CRC(39f1e1f4) SHA1(53d12f59a56df35c705408c76e6e02118da656f1) )
	ROM_LOAD( "u24", 0x400000, 0x400000, CRC(c631a766) SHA1(8bb6934a2f5b8a9841c3dcf85192b1743773dd8b) )
ROM_END

ROM_START( futari15 )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD("u4", 0x000000, 0x200000, CRC(e8c5f128) SHA1(45fb8066fdbecb83fdc2e14555c460d0c652cd5f) ) /* (2006/12/8.MAST VER. 1.54) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD("u2", 0x000000, 0x8400000, CRC(b9eae1fc) SHA1(410f8e7cfcbfd271b41fb4f8d049a13a3191a1f9) ) /* (2006/12/8.MAST VER. 1.54) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD("u23", 0x000000, 0x400000, CRC(39f1e1f4) SHA1(53d12f59a56df35c705408c76e6e02118da656f1) )
	ROM_LOAD("u24", 0x400000, 0x400000, CRC(c631a766) SHA1(8bb6934a2f5b8a9841c3dcf85192b1743773dd8b) )
ROM_END

ROM_START( futari15a )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD("u4", 0x000000, 0x200000, CRC(a609cf89) SHA1(56752fae9f42fa852af8ee2eae79e25ec7f17953) ) /* (2006/12/8 MAST VER 1.54) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD("u2", 0x000000, 0x8400000, CRC(b9d815f9) SHA1(6b6f668b0bbb087ffac65e4f0d8bd9d5b28eeb28) ) /* (2006/12/8 MAST VER 1.54) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD("u23", 0x000000, 0x400000, CRC(39f1e1f4) SHA1(53d12f59a56df35c705408c76e6e02118da656f1) )
	ROM_LOAD("u24", 0x400000, 0x400000, CRC(c631a766) SHA1(8bb6934a2f5b8a9841c3dcf85192b1743773dd8b) )
ROM_END

ROM_START( ibara )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD( "u4", 0x000000, 0x200000, CRC(8e6c155d) SHA1(38ac2107dc7824836e2b4e04c7180d5ae43c9b79) ) /* (2005/03/22 MASTER VER..) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD( "u2", 0x000000, 0x8400000, CRC(55840976) SHA1(4982bdce84f9603adfed7a618f18bc80359ab81e) ) /* (2005/03/22 MASTER VER..) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD( "u23", 0x000000, 0x400000, CRC(ee5e585d) SHA1(7eeba4ee693060e927f8c46b16e39227c6a62392) )
	ROM_LOAD( "u24", 0x400000, 0x400000, CRC(f0aa3cb6) SHA1(f9d137cd879e718811b2d21a0af2a9c6b7dca2f9) )
ROM_END

ROM_START( ibarablk )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD( "u4", 0x000000, 0x200000, CRC(ee1f1f77) SHA1(ac276f3955aa4dde2544af4912819a7ae6bcf8dd) ) /* (2006/02/06. MASTER VER.) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD( "u2", 0x000000, 0x8400000, CRC(5e46be44) SHA1(bed5f1bf452f2cac58747ecabec3c4392566a3a7) ) /* (2006/02/06. MASTER VER.) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD( "u23", 0x000000, 0x400000, CRC(a436bb22) SHA1(0556e771cc02638bf8814315ba671c2d442594f1) ) /* (2006/02/06 MASTER VER.) */
	ROM_LOAD( "u24", 0x400000, 0x400000, CRC(d11ab6b6) SHA1(2132191cbe847e2560423e4545c969f21f8ff825) ) /* (2006/02/06 MASTER VER.) */
ROM_END

ROM_START( ibarablka )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD( "u4", 0x000000, 0x200000, CRC(a9d43839) SHA1(507696e616608c05893c7ac2814b3365e9cb0720) ) /* (2006/02/06 MASTER VER.) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD( "u2", 0x000000, 0x8400000, CRC(33400d96) SHA1(09c22b5431ac3726bf88c56efd970f56793f825a) ) /* (2006/02/06 MASTER VER.) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD( "u23", 0x000000, 0x400000, CRC(a436bb22) SHA1(0556e771cc02638bf8814315ba671c2d442594f1) ) /* (2006/02/06 MASTER VER.) */
	ROM_LOAD( "u24", 0x400000, 0x400000, CRC(d11ab6b6) SHA1(2132191cbe847e2560423e4545c969f21f8ff825) ) /* (2006/02/06 MASTER VER.) */
ROM_END

ROM_START( espgal2 )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD( "u4", 0x000000, 0x200000, CRC(09c908bb) SHA1(7d6031fd3542b3e1d296ff218feb40502fd78694) ) /* (2005/11/14 MASTER VER) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD( "u2", 0x000000, 0x8400000, CRC(222f58c7) SHA1(d47a5085a1debd9cb8c61d88cd39e4f5036d1797) ) /* (2005/11/14 MASTER VER) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD( "u23", 0x000000, 0x400000, CRC(b9a10c22) SHA1(4561f95c6018c9716077224bfe9660e61fb84681) )
	ROM_LOAD( "u24", 0x400000, 0x400000, CRC(c76b1ec4) SHA1(b98a53d41a995d968e0432ed824b0b06d93dcea8) )
ROM_END

ROM_START( deathsml )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF)
	ROM_LOAD( "u4", 0x000000, 0x200000, CRC(1a7b98bf) SHA1(07798a4a846e5802756396b34df47d106895c1f1) ) /* (2007/10/09 MASTER VER) */

	ROM_REGION( 0x8400000, "game", ROMREGION_ERASEFF)
	ROM_LOAD( "u2", 0x000000, 0x8400000, CRC(d45b0698) SHA1(7077b9445f5ed4749c7f683191ccd312180fac38) ) /* (2007/10/09 MASTER VER) */

	ROM_REGION( 0x800000, "samples", ROMREGION_ERASEFF)
	ROM_LOAD( "u23", 0x000000, 0x400000, CRC(aab718c8) SHA1(0e636c46d06151abd6f73232bc479dafcafe5327) )
	ROM_LOAD( "u24", 0x400000, 0x400000, CRC(83881d84) SHA1(6e2294b247dfcbf0ced155dc45c706f29052775d) )
ROM_END


GAME( 2004, mushisam,  0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "Mushihime Sama (2004/10/12 MASTER VER.)",                    GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2004, mushisama, mushisam,   cavesh3,    cavesh3,  0, ROT0, "Cave", "Mushihime Sama (2004/10/12 MASTER VER)",                     GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2005, mushitam,  0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "Mushihime Tama (2005/09/09 MASTER VER)",                     GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2005, espgal2,   0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "EspGaluda II (2005/11/14 MASTER VER)",                       GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2006, futari15,  0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "Mushihime Sama Futari Ver 1.5 (2006/12/8.MASTER VER. 1.54)", GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2006, futari15a, futari15,   cavesh3,    cavesh3,  0, ROT0, "Cave", "Mushihime Sama Futari Ver 1.5 (2006/12/8 MASTER VER 1.54)",  GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2006, futari10,  futari15,   cavesh3,    cavesh3,  0, ROT0, "Cave", "Mushihime Sama Futari Ver 1.0 (2006/10/23 MASTER VER.)",     GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2006, ibara,     0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "Ibara (2005/03/22 MASTER VER..)",                            GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2006, ibarablk,  0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "Ibara Kuro - Black Label (2006/02/06. MASTER VER.)",         GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2006, ibarablka, ibarablk,   cavesh3,    cavesh3,  0, ROT0, "Cave", "Ibara Kuro - Black Label (2006/02/06 MASTER VER.)",          GAME_NOT_WORKING | GAME_NO_SOUND )
GAME( 2007, deathsml,  0,          cavesh3,    cavesh3,  0, ROT0, "Cave", "Death Smiles (2007/10/09 MASTER VER)",                       GAME_NOT_WORKING | GAME_NO_SOUND )

/*

Known versions of games on this hardware (* denotes undumped):

MUSHIHIMESAMA
  "2004/10/12 MASTER VER"  - broken
  "2004/10/12 MASTER VER." - fixed 1
* "2004/10/12.MASTER VER." - fixed 2

IBARA
  "2005/03/22 MASTER VER.."

IBARA BLACK LABEL
  "2006/02/06 MASTER VER."
  "2006/02/06. MASTER VER."

ESPGALUDA II
  "2005/11/14 MASTER VER"

PINK SWEETS
* "2006/04/06 MASTER VER."
* "2006/04/06 MASTER VER..."
* "2006/04/06 MASTER VER...."
* "2006/05/18 MASTER VER."
* "2006/xx/xx MASTER VER"

MUSHIHIMESAMA FUTARI 1.0
* "2006/10/23 MASTER VER"  - Ultra unlockable
  "2006/10/23 MASTER VER." - Ultra unlockable
* "2006/10/23.MASTER VER." - Cannot unlock ultra

MUSHIHIME SAMA FUTARI 1.5
  "2006/12/8 MASTER VER 1.54"
  "2006/12/8.MASTER VER.1.54."

MUSHIHIMESAMA FUTARI BLACK LABEL
* "2007/12/11 BLACK LABEL VER"
* "2009/11/17 INTERNATIONAL BL"  ("Another Ver" on title screen)

MUCHI MUCHI PORK
* "2007/ 4/17 MASTER VER."
* 2 "period" ver, location of the periods unknown

DEATH SMILES
  "2007/10/09 MASTER VER"

DEATH SMILES MEGA BLACK LABEL
* "2008/10/06 MEGABLACK LABEL VER"

DODONPACHI FUKKATSU 1.0
* "2008/05/16 MASTER VER"

DODONPACHI FUKKATSU 1.5
* "2008/06/23 MASTER VER 1.5"

DODONPACHI DAIFUKKATSU BLACK LABEL
* "2010/1/18 BLACK LABEL"

AKAI KATANA
* "2010/ 8/13 MASTER VER."
*  Home/Limited version, unknown date line, different gameplay from regular version, doesn't accept coins - permanent freeplay

MUSHIHIMESAMA 1.5 MATSURI VERSION
* 2011/5/23 CAVEMATSURI VER 1.5

*/
