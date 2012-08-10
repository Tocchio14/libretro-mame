/* Unkonwn JPM Platform */
/* seems to be Coldfire based */
/* only Ker - Chinq has sound roms, they seem to map in cpu space, but are missing from the rest? */
/* Could be Pluto 6? */



#include "emu.h"
#include "cpu/m68000/m68000.h"

class jpmsys7_state : public driver_device
{
public:
	jpmsys7_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_maincpu(*this, "maincpu")
	{ }

protected:

	// devices
	required_device<cpu_device> m_maincpu;
};


static ADDRESS_MAP_START( jpmsys7_map, AS_PROGRAM, 32, jpmsys7_state )
	AM_RANGE(0x00000000, 0x002fffff) AM_ROM
	AM_RANGE(0x10000000, 0x1000ffff) AM_RAM
	AM_RANGE(0x20000018, 0x2000001b) AM_WRITENOP // large data upload like astra/pluto?
	AM_RANGE(0x50000000, 0x50001fff) AM_RAM

//  AM_RANGE(0xf0000000, 0xf0000fff) AM_RAM

ADDRESS_MAP_END

static INPUT_PORTS_START(  jpmsys7 )
INPUT_PORTS_END

static MACHINE_CONFIG_START( jpmsys7, jpmsys7_state )
	MCFG_CPU_ADD("maincpu", MCF5206E, 40000000)	 // seems to be a Coldfire of some kind
	MCFG_CPU_PROGRAM_MAP(jpmsys7_map)

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	/* unknown sound (probably DMA driven DAC) */
MACHINE_CONFIG_END





ROM_START( j7bmagic )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "bm30cz06_2.b8", 0x00000, 0x100000, CRC(b9938925) SHA1(9b19dbede67c049a963d49a77734b0653fbe87a5) )
	ROM_LOAD16_BYTE( "bm30cz06_1.b8", 0x00001, 0x100000, CRC(c1af689c) SHA1(a42cd22dc7a58bb41338f8fc80cb31749904cab6) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "bm75cz06_1.b8", 0x0000, 0x100000, CRC(34e39ccc) SHA1(d31755b065c65cafa10a40d4f6c802419372e1fe) )
ROM_END



ROM_START( j7cexprs )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "2b42", 0x00000, 0x100000, CRC(96414594) SHA1(5e557c0102c6109bf20995754fad1cf8527ba04e) )
	ROM_LOAD16_BYTE( "b15b", 0x00001, 0x100000, CRC(b4b9c8f6) SHA1(ee369909554565f2c26a79cdda4a6c52322ca4dc) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "b17a", 0x0000, 0x100000, CRC(32452105) SHA1(9ff30856afd6568e56cecff920c795b2ec438f9b) )
	ROM_LOAD16_BYTE( "b192", 0x0000, 0x100000, CRC(05a28d8e) SHA1(cd0529f12d6253dce5ee5412604db23d20638810) )
	ROM_LOAD16_BYTE( "b19a", 0x0000, 0x100000, CRC(6673504c) SHA1(20bf3b678e9fd58474532af57a694b04555691c2) )
	ROM_LOAD16_BYTE( "b19b", 0x0000, 0x100000, CRC(1cd52a64) SHA1(675d31d083c9889ffb739ad517d4db5136883771) )
ROM_END





ROM_START( j7crztrl )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "1658", 0x00000, 0x100000, CRC(536c435d) SHA1(edcdd21ef1efb58f3214fc74beefb20d514e06f0) )
	ROM_LOAD16_BYTE( "b59a", 0x00001, 0x100000, CRC(51d0ccf3) SHA1(2c960b1d5e7490b07e77c94110992f8865533411) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "b5b9", 0x0000, 0x100000, CRC(d72c2500) SHA1(0aee84362c838aa6f0c2446bbeb902b20e3e3c68) )
	ROM_LOAD16_BYTE( "b5d1", 0x0000, 0x100000, CRC(e0cb898b) SHA1(6c7099f570784feb032d8ba927799c78ba3a66cf) )
	ROM_LOAD16_BYTE( "b5d9", 0x0000, 0x100000, CRC(831a5449) SHA1(7c94654481b222becea54982199c42adb7e56604) )
	ROM_LOAD16_BYTE( "b5da", 0x0000, 0x100000, CRC(f9bc2e61) SHA1(4c0d371e5a7bed89e3ef9dd46101f4705c83aeb8) )
ROM_END




ROM_START( j7fantaz )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "38c9", 0x00000, 0x100000, CRC(32f41d51) SHA1(0dd29004c9d1f3ff0cb2aedb698c26ff88983dff) )
	ROM_LOAD16_BYTE( "7cd2", 0x00001, 0x100000, CRC(75e4e09f) SHA1(5c66c9d57889696bdeb623ffa4d748552f8458b1) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "7cf1", 0x0000, 0x100000, CRC(f318096c) SHA1(35bba0688b1835bce2fc370e4dcb3d31187a8c08) )
	ROM_LOAD16_BYTE( "7d11", 0x0000, 0x100000, CRC(a72e7825) SHA1(62769230647076274e563ebd444f325136f59586) )
	ROM_LOAD16_BYTE( "7d12", 0x0000, 0x100000, CRC(dd88020d) SHA1(0738f8505fea1e3f8662ab3eef3fc8eed7fbc32d) )
ROM_END





ROM_START( j7kerchn )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "kerching-u1.bin", 0x000001, 0x100000, CRC(25347eff) SHA1(3e5c950993c3cb393fdfd7691637a35f9e41ca3f) )
//  ROM_LOAD16_BYTE( "9236", 0x00001, 0x100000, CRC(25347eff) SHA1(3e5c950993c3cb393fdfd7691637a35f9e41ca3f) )
	ROM_LOAD16_BYTE( "kerching-u2.bin", 0x000000, 0x100000, CRC(64bcf49e) SHA1(1f2755e005b7f47ce0c75b74a50ac41ed9220028) )
//  ROM_LOAD16_BYTE( "67f6", 0x00000, 0x100000, CRC(64bcf49e) SHA1(1f2755e005b7f47ce0c75b74a50ac41ed9220028) )
	ROM_LOAD16_BYTE( "kerching-u3.bin", 0x200001, 0x100000, CRC(9b068f6a) SHA1(99e1b52c98485bf972bf32f0d373182b39e33e07) ) // sound
	ROM_LOAD16_BYTE( "kerching-u4.bin", 0x200000, 0x100000, CRC(7ead4175) SHA1(abbf01da479027cad6f01d5089c68599973704c5) ) // sound

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "9216", 0x0000, 0x100000, CRC(71020fb6) SHA1(808baf0957d6d29331298fecba9e2e1ae775b5cf) )
	ROM_LOAD16_BYTE( "922b", 0x0000, 0x100000, CRC(c90f2d45) SHA1(a8c520f61ea708a01154b3089c8a4ee4a36acebc) )
	ROM_LOAD16_BYTE( "9233", 0x0000, 0x100000, CRC(aadef087) SHA1(7f899086e441c434edd5d4c89bc631bf2f396e9e) )
	ROM_LOAD16_BYTE( "9235", 0x0000, 0x100000, CRC(5f9204d7) SHA1(3b0b4ed0caafe56cebd7f6bd68c28a771a916f00) )
ROM_END


ROM_START( j7clbmag )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "cm500uk1.b8", 0x000001, 0x100000, CRC(5c3b8a01) SHA1(2d63008fca7afd8b08c42b4780d10971d5f092e6) )
	ROM_LOAD16_BYTE( "cm500uk2.b8", 0x000000, 0x100000, CRC(88dcaecf) SHA1(7abb9fe461e56678e4659e3f93956d07bcad0011) )
	ROM_LOAD16_BYTE( "cm_snd1.b8",  0x200001, 0x100000, CRC(bc8ea44e) SHA1(bc65420c9a7936e7b90b0d15fd386f19a347b6a2) )
	ROM_LOAD16_BYTE( "cm_snd2.b8",  0x200000, 0x100000, CRC(eae2b5be) SHA1(cd9fbf8cd754ac272ee23ed6d85b92a90f3edadb) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD( "cmfe_1.b8", 0x0000, 0x100000, CRC(080dfb48) SHA1(dc05f54bbd81927fe4bf3fa4461c38236189a87e) )
	ROM_LOAD( "cmpr_1.b8", 0x0000, 0x100000, CRC(269df029) SHA1(715d5fc235ac91df8c59b532f1f04af212a5831b) )
	ROM_LOAD( "cmsh_1.b8", 0x0000, 0x100000, CRC(d7934964) SHA1(56d7dae960246081f3ee19dce7cc8bfcac490a79) )
ROM_END


ROM_START( j7razzma )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "d971", 0x00000, 0x100000, CRC(ad739bda) SHA1(52c8e5de49209f05e266414210e6002f702efb95) )
	ROM_LOAD16_BYTE( "6129", 0x00001, 0x100000, CRC(ab19863a) SHA1(54804af810f99e27b9321eae84905a1b841e6bb3) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "6148", 0x00001, 0x100000, CRC(2de56fc9) SHA1(a0f6a9e30a4a80da0f6b1d8a5f59c81c9082b570) )
	ROM_LOAD16_BYTE( "6160", 0x00001, 0x100000, CRC(1a02c342) SHA1(2cd3a374702a004558a1bce99c0813ff251d0e67) )
	ROM_LOAD16_BYTE( "6168", 0x00001, 0x100000, CRC(79d31e80) SHA1(27007392a5ca048cb004905987a75811d070fee1) )
	ROM_LOAD16_BYTE( "6169", 0x00001, 0x100000, CRC(037564a8) SHA1(827a1d5956691e2d6b0581656ddbd7eedcf3d91c) )
ROM_END

ROM_START( j7razzmaa )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "f70c", 0x00000, 0x100000, CRC(f4d67a8a) SHA1(2a58435d01200e6efecb010e87199f14a23439c9) )
	ROM_LOAD16_BYTE( "4dd4", 0x00001, 0x100000, CRC(9fbdffe6) SHA1(77c2ab8a5f9a29fd4bc686d3604d20789714b626) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "4dec", 0x00001, 0x100000, CRC(a85a536d) SHA1(fe8f4bdd7b3f150250d70bb5b21ef707fa7daff9) )
	ROM_LOAD16_BYTE( "4df4", 0x00001, 0x100000, CRC(cb8b8eaf) SHA1(4921d44c01c91125d97180e95530c16db3662c66) )
	ROM_LOAD16_BYTE( "4df5", 0x00001, 0x100000, CRC(b12df487) SHA1(a6d833db891e66d0014dba79d8cfb258dca47dad) )
	ROM_LOAD16_BYTE( "4db5", 0x00001, 0x100000, CRC(19411615) SHA1(e3e0ec634a62a195a4a764a23d39c62b637aa4be) )
ROM_END

ROM_START( j7r2roll )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "7746", 0x00000, 0x100000, CRC(72aa8557) SHA1(95bd58c18866919b032fc96316f78da67686c54d) )
	ROM_LOAD16_BYTE( "7dbd", 0x00001, 0x100000, CRC(28f6117e) SHA1(6535e76b3e239903b56562936fc6d2722ef101dd) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "7dd5", 0x0000, 0x100000, CRC(1f11bdf5) SHA1(9c611ccfad0071e5c100404f2fe790f31a857801) )
	ROM_LOAD16_BYTE( "7ddb", 0x0000, 0x100000, CRC(898c9467) SHA1(0cac1d70c277a0bdba5fa59d1cbe6ad8e63286f5) )
	ROM_LOAD16_BYTE( "7ddc", 0x0000, 0x100000, CRC(06661a1f) SHA1(03c121bc82c62d1e5227c6faf3ab97d10b0f74c6) )
	ROM_LOAD16_BYTE( "7ddd", 0x0000, 0x100000, CRC(7cc06037) SHA1(c52c747851c5755eb031ebd7debaef3e8f987fb5) )
ROM_END



ROM_START( j7tubgld )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "tg30cz02_2.b8", 0x00000, 0x100000, CRC(c9162be1) SHA1(a37eef9c069ea30e7b1e50ab1d0ad21fc4874ddd) )
	ROM_LOAD16_BYTE( "tg30cz02_1.b8", 0x00001, 0x100000, CRC(de3884ae) SHA1(ca9ac57ab34b07fc24c39286c47a90786d05baf6) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "tg75cz02_1.b8", 0x0000, 0x100000, CRC(2b7470fe) SHA1(bdd69432f11f0580cacc96f4aeee2d2eaf07b66a) )
ROM_END

ROM_START( j7wldwkd )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "ea06", 0x00000, 0x100000, CRC(a87f1156) SHA1(08dbc39595013acb921cc4d027af3d4a3fd6b02a) )
	ROM_LOAD16_BYTE( "116b", 0x00001, 0x100000, CRC(1993c09a) SHA1(80d3bd99ab79019f7db14641e6ef06efef64ba07) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "114b", 0x0000, 0x100000, CRC(4da5b1d3) SHA1(d05601d6880c9a6f7452bd3e08e984ecdd21040a) )
	ROM_LOAD16_BYTE( "1162", 0x0000, 0x100000, CRC(00e46770) SHA1(a92c0979a13652cfb040ea69dbcc99bb1b8c43a5) )
	ROM_LOAD16_BYTE( "1169", 0x0000, 0x100000, CRC(ecdf34ca) SHA1(50166f92308a8349064e4e28675bc75e75d965a6) )
	ROM_LOAD16_BYTE( "116a", 0x0000, 0x100000, CRC(6335bab2) SHA1(59da7dc212c44230bb674d455470c4355d46980e) )
ROM_END

ROM_START( j7bullio )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "7b7d", 0x00000, 0x100000, CRC(ebe6028b) SHA1(0dabe2178556632a9ce994e04173ff0b1b52765b) )
	ROM_LOAD16_BYTE( "0482", 0x00001, 0x100000, CRC(dcb8dd8f) SHA1(8a703213b6a85dc1c257e3a9110cbfb7c381571e) )

	ROM_REGION( 0x400000, "altrevs", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "04a1", 0x0000, 0x100000, CRC(5a44347c) SHA1(de7b1de4620dadbb06eda7adc63f26577f402f3f) )
	ROM_LOAD16_BYTE( "04b9", 0x0000, 0x100000, CRC(6da398f7) SHA1(566be31549b66fdf4b3466fc50af1fcd3bc02af0) )
	ROM_LOAD16_BYTE( "04c1", 0x0000, 0x100000, CRC(0e724535) SHA1(c77e0ab67ce25421e7c94bf38733af6fd4148ba3) )
	ROM_LOAD16_BYTE( "04c2", 0x0000, 0x100000, CRC(74d43f1d) SHA1(a75c41928727c2752bfa6f04460fa003441ca0a6) )
ROM_END


GAME( 200?, j7bmagic		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Black Magic (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7cexprs		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Cash Xpress (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7crztrl		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Crazy Trails (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7fantaz		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Fantaztec (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7kerchn		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Ker - Chinq (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7razzma		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Razzamataz (Jpm) (set 1)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7razzmaa		,j7razzma,	jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Razzamataz (Jpm) (set 2)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7r2roll		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Ready To Roll (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7tubgld		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Turbo Gold (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7wldwkd		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Jpm","Wild 'N' Wicked (Jpm)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7bullio		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Ace","Bullionaire (Ace)", GAME_IS_SKELETON_MECHANICAL )
GAME( 200?, j7clbmag		,0,			jpmsys7, jpmsys7, driver_device, 0, ROT0, "Qps","Club Magic (Jpm)", GAME_IS_SKELETON_MECHANICAL )
