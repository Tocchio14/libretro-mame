/* MayGay M2 hardware
  SWP (Skill With Prizes) Video based games */

/*
    Guniess Book of Records by Maygay 1996 - M2 Hardware

    CPU - MC68306FC16 (On M2 Active CPU Board)
    Sound - OKIM6650 (ON MPEG Sound Board)
    Graphics - CL-GD5429-86QC-C (On Video Card)
    QUARTZ OSCILLATORS AT 3.6864Mhz (by 16c64), 16.0000Mhz (by MC68306FC16), 4.00000Mhz (by OKIM6650), 4.0000Mhz (by 16c55) and 14.31818Mhz (by CL-GD5429-86QC-C)
    RAM HY6264A x 2 (On M2 Active CPU Board), HM514260CJ7 x3 (On Video Card)

    Other

    M62405P (ON MPEG Sound Board)
    MC14514BCP (On M2 Active I/O Board)
    MAX1232 (On M2 Active CPU Board)
    MC1488P and MC1489P (On M2 Active CPU Board)

    Custom

    MAYGAY MACHINES EP840150 5261ES-A46528A ASG9605 84PIN PLCC?

    PIC's

    16c64.u6 - PIC16C64-20/P M2 VER2.2 I/O CPU PIC  CRC32 6AE364A2 SHA1 56DDE3D270C2CF81D9592C7C2284767188409B56 (On M2 Active CPU Board)

    16C55.u5 - PIC16C55-XT/P M2 Sound PIC version 1 CRC32 0BD92C3E SHA1 596F4D0A83EBC879EC64BA3038D2E9448D2F8901 (ON MPEG Sound Board)


    Other

    0819a.u17 - GAL16V8B Not Dumped (On Video Memory Board)
    0190a.u6 - GAL16V8B Not Dumped (On Video Card)
    45a.u12 - GAL16V8B Not Dumped (On Video Card)
    o146a.u4 - GAL16V8B Not Dumped (On MPEG Sound Board)

    Roms

    Video Memory Board

    dg70014.u01 - TMS27C040 CRC32 EA24D687 SHA1 6B2A069681236EED67F54ED6B1117416DB7DE9AA

    dg70014.u02 - TMS27C040 CRC32 31E151AA SHA1 2DD99973D829910A6AF54D0A46AB37BA36B159B1

    dg70014.u03 - TMS27C040 CRC32 18EA3CF1 SHA1 5A20597906B0209666BF937E0A4975A250EAACED

    dg70014.u04 - TMS27C040 CRC32 96A05459 SHA1 CDC12733DFD9AE50C2D33397A2CF3831C1B275BF

    da70014.u05 - TMS27C040 CRC32 5ACEB7D6 SHA1 C3099D3C83BE2F5DD3E474557CA17A2C5385ED1F

    dq70014.u06 - TMS27C040 CRC32 568AD7C7 SHA1 078FCEEF241AC4C74B20A07718837973EE6402A5

    dg70014.u07 - TMS27C040 CRC32 602D79F1 SHA1 EA873C42E93FAFF6DF8C05A98B52B79E52CAA28B

    dg70014.u08 - TMS27C040 CRC32 EB12693D SHA1 8D894A56BD7E280AB3D4456EF180C42EE87D6F3B

    dg70014.u09 - TMS27C040 CRC32 85534E69 SHA1 1165E43C02DCE8FE048DFB863B9EB61BE7F68A48

    dg70014.u10 - TMS27C040 CRC32 6174F684 SHA1 33EF0E28F69B810F3DF899B15AC36577025B7B59

    da70014.u11 - TMS27C040 CRC32 6CD1ADCD SHA1 A803A4B9945498CC1A772B6B9D6948669782149D

    dq70014.u12 - TMS27C040 CRC32 7BD17C0B SHA1 395A0F72DB64648BD5D953868AA42ED6A4C9E2EC


    Video Card

    std961d.u27 - TMS27C010A CRC32 7BD17C0B SHA1 395A0F72DB64648BD5D953868AA42ED6A4C9E2EC

    std951d.u28 - TMS27C010A CRC32 D4A8686A SHA1 D25D4C7ED32874F33DA787DFAE1F661D06531359


    MPEG Sound Board

    dig1127.u3 - TMS27C040 CRC32 45A2275D SHA1 8AF08B5C007BB2BFE927DF53BD167EA6045D8694

    dig1127.u2 - TMS27C040 CRC32 220B38FE SHA1 8FF20A9353736CDA44EC11A99AC94B36F9DB7430

    Dumped By Dang_Spot 18/02/04

----------------------------------------------

    Complete dumps?

    Guiness Book of Records
    Risk
    London Underground
    Big Break (except the PICs)
    Aladdins Cave (except the PICs)

    Incomplete dumps?

    everything else :-(

 */



#include "emu.h"
#include "cpu/m68000/m68000.h"

class maygayew_state : public driver_device
{
public:
	maygayew_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }
};



static ADDRESS_MAP_START( maygayew_map, AS_PROGRAM, 16, maygayew_state )
	AM_RANGE(0x000000, 0x03ffff) AM_ROM AM_REGION("mainrom",0)
	AM_RANGE(0x100000, 0x13ffff) AM_ROM AM_REGION("mainrom",0)
	AM_RANGE(0x200000, 0x23ffff) AM_ROM AM_REGION("mainrom",0)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM
ADDRESS_MAP_END


static INPUT_PORTS_START( maygayew )
INPUT_PORTS_END


static MACHINE_CONFIG_START( maygayew, maygayew_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000,8000000)	// MC68306FC16 - standard 68000 core + peripherals
	MCFG_CPU_PROGRAM_MAP(maygayew_map)
MACHINE_CONFIG_END

ROM_START( mg_gbr )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "std961d.u27", 0x00000, 0x020000, CRC(f52fa8da) SHA1(80168d379d396b1fd76200caa203b0fd0f280b78) )
	ROM_LOAD16_BYTE( "std951d.u28", 0x00001, 0x020000, CRC(d4a8686a) SHA1(d25d4c7ed32874f33da787dfae1f661d06531359) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw8-146.u27", 0x00000, 0x020000, CRC(af217e4e) SHA1(5fcf72cc186f29641c7f9f50a35ad89dfc76d56f) )
	ROM_LOAD16_BYTE( "sw8-146.u28", 0x00001, 0x020000, CRC(68c66f9b) SHA1(76dc54e506dc05e66ce9d52a2e2cd2af0a365854) )
	ROM_LOAD16_BYTE( "sw8-147.u27", 0x00000, 0x020000, CRC(9c428128) SHA1(39d76d307bee7b4dce90ad4aa52a14a460b5ba35) )
	ROM_LOAD16_BYTE( "sw8-147.u28", 0x00001, 0x020000, CRC(384a945d) SHA1(761b35130574c108b8da40b0b1779cbda5797172) )

	ROM_REGION( 0x800000, "dataroms", 0 )
	ROM_LOAD16_BYTE( "dg70014.u01", 0x000000, 0x080000, CRC(ea24d687) SHA1(6b2a069681236eed67f54ed6b1117416db7de9aa) )
	ROM_LOAD16_BYTE( "dg70014.u07", 0x000001, 0x080000, CRC(602d79f1) SHA1(ea873c42e93faff6df8c05a98b52b79e52caa28b) )
	ROM_LOAD16_BYTE( "dg70014.u02", 0x100000, 0x080000, CRC(31e151aa) SHA1(2dd99973d829910a6af54d0a46ab37ba36b159b1) )
	ROM_LOAD16_BYTE( "dg70014.u08", 0x100001, 0x080000, CRC(eb12693d) SHA1(8d894a56bd7e280ab3d4456ef180c42ee87d6f3b) )
	ROM_LOAD16_BYTE( "dg70014.u03", 0x200000, 0x080000, CRC(18ea3cf1) SHA1(5a20597906b0209666bf937e0a4975a250eaaced) )
	ROM_LOAD16_BYTE( "dg70014.u09", 0x200001, 0x080000, CRC(85534e69) SHA1(1165e43c02dce8fe048dfb863b9eb61be7f68a48) )
	ROM_LOAD16_BYTE( "dg70014.u04", 0x300000, 0x080000, CRC(96a05459) SHA1(cdc12733dfd9ae50c2d33397a2cf3831c1b275bf) )
	ROM_LOAD16_BYTE( "dg70014.u10", 0x300001, 0x080000, CRC(6174f684) SHA1(33ef0e28f69b810f3df899b15ac36577025b7b59) )
	ROM_LOAD16_BYTE( "da70014.u05", 0x400000, 0x080000, CRC(5aceb7d6) SHA1(c3099d3c83be2f5dd3e474557ca17a2c5385ed1f) )
	ROM_LOAD16_BYTE( "da70014.u11", 0x400001, 0x080000, CRC(6cd1adcd) SHA1(a803a4b9945498cc1a772b6b9d6948669782149d) )
	ROM_LOAD16_BYTE( "dq70014.u06", 0x500000, 0x080000, CRC(568ad7c7) SHA1(078fceef241ac4c74b20a07718837973ee6402a5) )
	ROM_LOAD16_BYTE( "dq70014.u12", 0x500001, 0x080000, CRC(7bd17c0b) SHA1(395a0f72db64648bd5d953868aa42ed6a4c9e2ec) )

	ROM_REGION( 0x100000, "snd", 0 )
	ROM_LOAD( "dig1127.u3", 0x000000, 0x080000, CRC(45a2275d) SHA1(8af08b5c007bb2bfe927df53bd167ea6045d8694) )
	ROM_LOAD( "dig1127.u2", 0x080000, 0x080000, CRC(220b38fe) SHA1(8ff20a9353736cda44ec11a99ac94b36f9db7430) )

	ROM_REGION( 0x100000, "sound_16c55", 0 )	// PIC dump?
	ROM_LOAD( "16c55.u5", 0x0000, 0x0023ff, CRC(0bd92c3e) SHA1(596f4d0a83ebc879ec64ba3038d2e9448d2f8901) )
	ROM_REGION( 0x100000, "io_16c64", 0 )	// PIC dump?
	ROM_LOAD( "16c64.u6", 0x0000, 0x008fff, CRC(6ae364a2) SHA1(56dde3d270c2cf81d9592c7c2284767188409b56) )
ROM_END

ROM_START( mg_risk )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "u27_std.u27", 0x00000, 0x020000, CRC(0f16ec39) SHA1(3eef3563b0c23bd70f8e8cd24fe8cad771e98b4e) )
	ROM_LOAD16_BYTE( "u28_std.u28", 0x00001, 0x020000, CRC(71b0e758) SHA1(d84696b2a5d2e3e9afc8fc5037481b6cb162d7b7) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw9-025.u27", 0x00000, 0x020000, CRC(80777622) SHA1(48dfeb39b443d4b1d8052c44ea3a42f25feddba0) )
	ROM_LOAD16_BYTE( "sw9-025.u28", 0x00001, 0x020000, CRC(d635d214) SHA1(2979ab5aebda2a96ec8bbe926ab5c4d3899691c4) )
	ROM_LOAD16_BYTE( "sw9-026.u27", 0x00000, 0x020000, CRC(78552a94) SHA1(90ad61882aa89e90e2733ee4a0eb6750c06bcdc2) )
	ROM_LOAD16_BYTE( "sw9-026.u28", 0x00001, 0x020000, CRC(0ed451dd) SHA1(9778718a9a2bd13b1ad1500f7cb0c08a68766609) )

	ROM_REGION( 0x800000, "dataroms", 0 )
	ROM_LOAD16_BYTE( "dg7-001-4-u01.u1",  0x000000, 0x080000, CRC(7de3a78b) SHA1(c267d039d75a9ebda2591cfd61b4a89b0ce46be3) )
	ROM_LOAD16_BYTE( "dg7-001-4-u07.u7",  0x000001, 0x080000, CRC(628bc52e) SHA1(d6278a2f4b0c7c6adcbe72c6479525546b2f19e9) )
	ROM_LOAD16_BYTE( "dg7-001-4-u02.u2",  0x100000, 0x080000, CRC(89de1c91) SHA1(5517c9e8e469fa36626c2f662c644318c37ebec5) )
	ROM_LOAD16_BYTE( "dg7-001-4-u08.u8",  0x100001, 0x080000, CRC(bb42bbaf) SHA1(6bd24cbac55395b088b9b90e2c59be549d76ba71) )
	ROM_LOAD16_BYTE( "dg7-001-4-u03.u3",  0x200000, 0x080000, CRC(fbe5049a) SHA1(8418d29ecf688f365bc59f26730b651743d33c7d) )
	ROM_LOAD16_BYTE( "dg7-001-4-u09.u9",  0x200001, 0x080000, CRC(556328bc) SHA1(98685beae3ecb2a0918968dd734ee9060485f750) )
	ROM_LOAD16_BYTE( "dg7-001-4-u04.u4",  0x300000, 0x080000, CRC(331b354e) SHA1(e459da23e422feded74057e8bf567703fa61532b) )
	ROM_LOAD16_BYTE( "dg7-001-4-u10.u10", 0x300001, 0x080000, CRC(911e105a) SHA1(ba50666dd10bda41f67a8e89dcc522a6afa073d0) )
	/* no 05/11 pair? - unpopulated? (probably, the 2nd half of the 4/10 pair before this is empty) */
	ROM_LOAD16_BYTE( "dg7-001-4-u06.u6",  0x500000, 0x080000, CRC(dca01eb3) SHA1(48ac890c6439924f7e98a9b6f050af508b4f2927) )
	ROM_LOAD16_BYTE( "dg7-001-4-u12.u12", 0x500001, 0x080000, CRC(467c101c) SHA1(33ee1ca481b1a138c9e53bae4ef4ee4a093fabbb) )

	ROM_REGION( 0x100000, "snd", 0 )
	ROM_LOAD( "d1g1-145.u3", 0x000000, 0x080000, CRC(a428be45) SHA1(ccce7274ea25423e34c5be35706271501781bd08) )
	ROM_LOAD( "d1g1-145.u2", 0x080000, 0x080000, CRC(6a3a6e26) SHA1(449eadbbee291c94a0f1f32a860dafd64e6143bb) )

	ROM_REGION( 0x100000, "sound_16c55", 0 )	// PIC dump?
	ROM_LOAD( "pic16c55.u5", 0x0000, 0x00040a, CRC(c1c0bd5b) SHA1(a1364de27f747d5531cb57757852f75cbb0cd520) )
	ROM_REGION( 0x100000, "io_16c64", 0 )	// PIC dump?
	ROM_LOAD( "pic16c64.u6", 0x0000, 0x004010, CRC(64eca658) SHA1(77e9aa586a16cf1e88da4bb53866242ab1ece3cd) )
ROM_END


ROM_START( mg_bb )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "bigbreaku27603asw6-618.bin", 0x00000, 0x020000, CRC(2c91fe59) SHA1(320127facb8ac8ea5adbf1eb66938b9211c0ba21) )
	ROM_LOAD16_BYTE( "bigbreaku28gfdbsa6-618std.bin", 0x00001, 0x020000, CRC(100aebe6) SHA1(360967d692e590271e4975b2203b5545c7af3540) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "656u27", 0x00000, 0x020000, CRC(b9992edb) SHA1(7a5a0e029b6aaa94c32560f1170bdee0d42ddd62) )
	ROM_LOAD16_BYTE( "656u28", 0x00001, 0x020000, CRC(19004505) SHA1(992d66d2e35c2f232cc01aaca656cccbd55bc401) )
	ROM_LOAD16_BYTE( "657u27", 0x00000, 0x020000, CRC(26532336) SHA1(cdf21b6d20b93065a38868834fd49642c3f87443) )
	ROM_LOAD16_BYTE( "657u28", 0x00001, 0x020000, CRC(8d1406bf) SHA1(570a208152c51e647eb9cef61009e848cb144ae7) )

	ROM_REGION( 0x800000, "dataroms", 0 )
	ROM_LOAD16_BYTE( "bigbreakdg6-001-4-u01.bin", 0x000000, 0x080000, CRC(9a271e70) SHA1(68478367156acf7ed9b343e3130d365f59dafa3e) )
	ROM_LOAD16_BYTE( "bigbreakdg6-001-4-u07.bin", 0x000001, 0x080000, CRC(b9029538) SHA1(4809867015d4525e2b8f012e48038a4e8b59cae8) )
	ROM_LOAD16_BYTE( "bigbreakdg6-001-4-u02.bin", 0x100000, 0x080000, CRC(5882625d) SHA1(5d4b2e8ce8b25e87a71b44bd5536e970872cbc08) )
	ROM_LOAD16_BYTE( "bigbreakdg6-001-4-u08.bin", 0x100001, 0x080000, CRC(eac30ceb) SHA1(93c56c18908701e702819cdb806da9d27892f5be) )
	ROM_LOAD16_BYTE( "bigbreakdg6-001-4-u03.bin", 0x200000, 0x080000, CRC(eb163259) SHA1(cd5ed150ee040b374fd5a09cd52a6d19681c01a9) )
	ROM_LOAD16_BYTE( "bigbreakdg6-001-4-u09.bin", 0x200001, 0x080000, CRC(cc81e702) SHA1(5768d67fdf75a9d6ad09bcf0d35dc52e4b8dd2c8) )
	ROM_LOAD16_BYTE( "bigbreakda8-001-4-u04.bin", 0x300000, 0x080000, CRC(78267f8d) SHA1(fb5deeabcee2c32e5c7ec645bbeaae6fcf8c2c7b) )
	ROM_LOAD16_BYTE( "bigbreakda6-001-4-u10.bin", 0x300001, 0x080000, CRC(9a0460a5) SHA1(f39a41895c6cadbaf1984ee2b31cac40c3d9ed6e) )
	/* no 05/11 pair? - unpopulated? */
	ROM_LOAD16_BYTE( "bigbreakdq6-001-4-u06.bin", 0x500000, 0x080000, CRC(a57ea42a) SHA1(7946f1f3b9ac7d9fab4b76b03c56fafc33571e1b) )
	ROM_LOAD16_BYTE( "bigbreakdq6-001-4-u12.bin", 0x500001, 0x080000, CRC(fb15d7ad) SHA1(44c1bbfce527bcffabf14d7735ad08ce292af745) )

	ROM_REGION( 0x100000, "snd", 0 )
	ROM_LOAD( "bigbreaksoudnv3dig1-106.bin", 0x0000, 0x080000, CRC(65790dfa) SHA1(5dbcec93a77dd96e5f4b42db8ab25afed9a27c9e) )

	ROM_REGION( 0x100000, "sound_16c55", 0 )	// PIC dump?
	ROM_LOAD( "pic16c55.u5", 0x0000, 0x00040a, NO_DUMP )
	ROM_REGION( 0x100000, "io_16c64", 0 )	// PIC dump?
	ROM_LOAD( "pic16c64.u6", 0x0000, 0x004010, NO_DUMP )
ROM_END


ROM_START( mg_alad )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sw7-536s.u27", 0x00000, 0x020000, CRC(a16bb22b) SHA1(19a0f74cf6558a33895dd8079221dfaf64a397c3) )
	ROM_LOAD16_BYTE( "sw7-536s.u28", 0x00001, 0x020000, CRC(5dfe53fa) SHA1(6b71ae36204bdde8354e78c9ecbc4c35f46bd529) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw8-323.u27", 0x00000, 0x020000, CRC(9a159893) SHA1(7289261f9956392e9823791bf9c979c47d7def44) )
	ROM_LOAD16_BYTE( "sw8-323.u28", 0x00001, 0x020000, CRC(0d439b33) SHA1(12f8e9e466da52e91208363000f8271293f08c8a) )
	ROM_LOAD16_BYTE( "sw8-322.u27", 0x00000, 0x020000, CRC(9a159893) SHA1(7289261f9956392e9823791bf9c979c47d7def44) )
	ROM_LOAD16_BYTE( "sw8-322.u28", 0x00001, 0x020000, CRC(5dcf60f5) SHA1(b3f5a1bcdf95c2476b4ab57293fd676e1a31f8f6) )
	ROM_LOAD16_BYTE( "al_c_swp.u27", 0x00000, 0x020000, CRC(8edd6aa0) SHA1(85adbe00c2925dc87cfe80e457ba671f46272ab2) )
	ROM_LOAD16_BYTE( "al_c_swp.u28", 0x00001, 0x020000, CRC(02662b47) SHA1(4d94fbca0b1fafc482be009faec7749551a8da24) )

	ROM_REGION( 0x800000, "dataroms", 0 )
	ROM_LOAD( "dg-001-4.u01", 0x0000, 0x080000, CRC(7796ca47) SHA1(6afb22e7934a60712e5dbae862b63032d61d6095) )
	ROM_LOAD( "dg-001-4.u07", 0x0000, 0x080000, CRC(2b1d7818) SHA1(f580b7f488d919580049b448720e76cc6b8d35ff) )
	ROM_LOAD( "dg-001-4.u02", 0x0000, 0x080000, CRC(c18935ad) SHA1(d1a09591cee49f6e163257fdbfdb4c163fa68959) )
	ROM_LOAD( "dg-001-4.u08", 0x0000, 0x080000, CRC(8214a2f1) SHA1(31358b416e14400a75a327ca4b83b94cd18d6365) )

	ROM_REGION( 0x100000, "snd", 0 )
	ROM_LOAD( "d1g1-144.u3", 0x0000, 0x080000, CRC(30941180) SHA1(cafcddcec359c217016bfd86b84b358fc0b1e618) )

	ROM_REGION( 0x100000, "sound_16c55", 0 )
	ROM_LOAD( "sound_16c55.u5", 0x0000, 0x080000, NO_DUMP )
	ROM_REGION( 0x100000, "io_16c64", 0 )
	ROM_LOAD( "io_16c64.u6", 0x0000, 0x080000, NO_DUMP )

ROM_END

/*

MAYGAY M2 - London Underground SWP game
---------------------------------------

Dumped by Andy Welburn on a sunny morning 10/03/07

*************************************************
**Do not seperate this text file from the roms.**
*************************************************

filename:   label:          location    type
============================================================
M2_U9.bin   Final 4 M2 I/O      IOB U9      27C512
VMB_U6.bin  DQ8-002-4 U6        VMB U6 even 27C040
VMB_U12.bin DQ8-002-4 U12       VMB U12 odd 27C040
VMB_U4.bin  DG8-001-4 U4        VMB U4 even 27C040
VMB_U10.bin DG8-001-4 U10       VMB U10 odd 27C040
VMB_U3.bin  DG8-001-4 U3        VMB U3 even 27C040
VMB_U9.bin  DG8-001-4 U9        VMB U9 odd  27C040
VMB_U2.bin  DG8-001-4 U2        VMB U2 even     27C040
VMB_U1.bin  DG8-001-4 U1        VMB U1 even     27C040
VMB_U7.bin  DG8-001-4 U7        VMB U7 odd  27C040
VC_U27.bin  SW8-232 NON DATA U27    VC U27      27C010
VC_U28.bin  SW8-232 NON DATA U28    VC U28      27C010
MSB_U3.bin  DIG1-155 U3     MSB U3      27C040
MSB_U2.bin  DIG1-155 U2     MSB U2      27C040
MSB_U5.bin  M2 SOUND PIC Ver 1.0    MSB U5      PIC16C55
CPU_U6.bin  M2 CPU I/O PIC Ver 2.2  CPU U6      PIC16C64


location:
IOB = M2 Active I/O Board
VMB = Video Memory Board
VC = Video Card
MSB = MPEG Sound Board
CPU = M2 Active CPU Board


NOTES:
- Video Memory Board and MPEG Sound Board rom labels were prefixed
  with "L/UNDERGROUND", these have been omitted in the table above
  to keep it brief.
- All Videocard roms were prefixed with LONDON UNDERGROUND
- se enclosed jpegs for pictures of the pcb and roms in situ.

enjoy..

Andy

*/



ROM_START( mg_lug )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sw8-232.u27", 0x00000, 0x020000, CRC(38c91eb5) SHA1(5a9a7d97999e8e261854fb9e8cf4ff056c351e87) )
	ROM_LOAD16_BYTE( "sw8-232.u28", 0x00001, 0x020000, CRC(5766291f) SHA1(715a1269576a8b83a75be4922ef92976c96e70f7) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw8-233.u27", 0x00000, 0x020000, CRC(13fca198) SHA1(8e259ffaba795bcbe7097a44049966b63e743413) )
	ROM_LOAD16_BYTE( "sw8-233.u28", 0x00001, 0x020000, CRC(72f12dae) SHA1(d0f41a164647f4b375baa35a929c5d1e9c9b82f4) )

	ROM_REGION( 0x1000000, "io", 0 ) // none of the others have this?
	ROM_LOAD( "m2.u9", 0x0000, 0x010000, CRC(1466debb) SHA1(9e403d8427031fcb07bc109138ada8f83af9c9dd) )

	ROM_REGION( 0x800000, "dataroms", 0 )
	ROM_LOAD16_BYTE( "vmb.u1", 0x000000, 0x080000, CRC(12bacc08) SHA1(091c5eacdf18e1b6e6a20ae0a6b8f0627dae7d14) )
	ROM_LOAD16_BYTE( "vmb.u7", 0x000001, 0x080000, CRC(89c8e923) SHA1(575274126176c25015bfeb1da4e0586050c03e4c) )
	ROM_LOAD16_BYTE( "vmb.u2", 0x100000, 0x080000, CRC(18505bc3) SHA1(910e258700568f76459b6ebe65debacf258c2ee6) )
	ROM_LOAD16_BYTE( "vmb.u8", 0x100001, 0x080000, CRC(8a126378) SHA1(279beb550106c4fe91a41f2ad0b8930c4f99402e) )
	ROM_LOAD16_BYTE( "vmb.u3", 0x200000, 0x080000, CRC(3a6a98a0) SHA1(aef73c9526ac7849e192b4a6f140336746d17bb9) )
	ROM_LOAD16_BYTE( "vmb.u9", 0x200001, 0x080000, CRC(7f79cc2b) SHA1(e8303be1ce08ac879525c49429f5248d8f684e58) )
	ROM_LOAD16_BYTE( "vmb.u4", 0x300000, 0x080000, CRC(e94282fb) SHA1(a9c494d43ac2905b3689c940a4013be7dd3fe166) )
	ROM_LOAD16_BYTE( "vmb.u10",0x300001, 0x080000, CRC(e8e17be9) SHA1(99cf5ed63614b772107d38eb9a2028860d275a4f) )
	/* no 05/11 pair? - unpopulated? */
	ROM_LOAD16_BYTE( "vmb.u6", 0x500000, 0x080000, CRC(6a409048) SHA1(cd76c144afcf5eba6080f34770db4081a168201b) )
	ROM_LOAD16_BYTE( "vmb.u12",0x500001, 0x080000, CRC(85d1ebad) SHA1(85f80c962f95d3d1af1efa7af9e8d06482146b95) )

	ROM_REGION( 0x100000, "snd", 0 )
	ROM_LOAD( "msb.u3", 0x000000, 0x080000, CRC(18fbf244) SHA1(20c6b6b644d24dc477c859a300e357914f2cfe72) )
	ROM_LOAD( "msb.u2", 0x080000, 0x080000, CRC(d802d345) SHA1(2b5acce2922fee3da924dc1291c2778de947218e) )

	ROM_REGION( 0x100000, "sound_16c55", 0 )	// PIC dump?
	ROM_LOAD( "msb.u5", 0x0000, 0x080000, CRC(e0335ce9) SHA1(a4a6d7cc79eaceab8949767860c7849d8b24d7d5) )
	ROM_REGION( 0x100000, "io_16c64", 0 )	// PIC dump?
	ROM_LOAD( "cpu.u6", 0x0000, 0x080000, CRC(24e25be1) SHA1(241f9c217e73586ab590f33ae4c8ec554e312f8c) )
ROM_END




ROM_START( mg_ewg )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sa6-127.u1", 0x00000, 0x020000, CRC(ffea277b) SHA1(e0f4b3b613a4545f8d5e7ec9a422af4c718243fb) )
	ROM_LOAD16_BYTE( "sa6-127.u2", 0x00001, 0x020000, CRC(01879da7) SHA1(4b3fa537af22a81af28305158378979cd4973a69) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sa6-128.u1", 0x00001, 0x020000, CRC(ffea277b) SHA1(e0f4b3b613a4545f8d5e7ec9a422af4c718243fb) )
	ROM_LOAD16_BYTE( "sa6-128.u2", 0x00000, 0x020000, CRC(1894b102) SHA1(46cf6cfa96710d76f0b776b06931bdfa5eff140e) )
	ROM_LOAD16_BYTE( "sa6-129.u1", 0x00001, 0x020000, CRC(f39ba9e9) SHA1(cf339c9f42e36b962da91f8cc68e556d32fab3f8) )
	ROM_LOAD16_BYTE( "sa6-129.u2", 0x00000, 0x020000, CRC(369e90f0) SHA1(e4400a0394c873ea41854beb2036465a28f9acca) )
	ROM_LOAD16_BYTE( "sa6-130.u1", 0x00001, 0x020000, CRC(a51b562b) SHA1(1998ea0e299fc72d0a39eaf87f0c5d7748a978d9) )
	ROM_LOAD16_BYTE( "sa6-130.u2", 0x00000, 0x020000, CRC(cb3d17e7) SHA1(7b941921193ee9218231e5739e7e7f1e0a1fb1af) )
	ROM_LOAD16_BYTE( "sa6-281.u1", 0x00001, 0x020000, CRC(6c0019ea) SHA1(4c8e791e8f748f866c0f364d4c6f97fcfaf27e29) )
	ROM_LOAD16_BYTE( "sa6-281.u2", 0x00000, 0x020000, CRC(68dfb7aa) SHA1(6c6046f2d33f262e4fbde11318d171823ac2c61c) )
	ROM_LOAD16_BYTE( "sa6-282.u1", 0x00001, 0x020000, CRC(6c0019ea) SHA1(4c8e791e8f748f866c0f364d4c6f97fcfaf27e29) )
	ROM_LOAD16_BYTE( "sa6-282.u2", 0x00000, 0x020000, CRC(6733d944) SHA1(bd62653cfc7ab5122e6d5397cd1b75e18285441d) )
	ROM_LOAD16_BYTE( "sa6-283.u1", 0x00001, 0x020000, CRC(af26391d) SHA1(066819cbb38feda47165a1f0560cc0c7de41d3ec) )
	ROM_LOAD16_BYTE( "sa6-283.u2", 0x00000, 0x020000, CRC(49b59728) SHA1(b9728e63a5453b66469af2457e258761dbc21927) )
	ROM_LOAD16_BYTE( "sa6-284.u1", 0x00001, 0x020000, CRC(bd16cd1e) SHA1(fcb9314f83d60d84ed4ff17d2c02c29f20e15fdf) )
	ROM_LOAD16_BYTE( "sa6-284.u2", 0x00000, 0x020000, CRC(fc40c076) SHA1(825fec1f768fd2442a5de89d627609f3b133dc5b) )
ROM_END

ROM_START( mg_jv )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sa6-608.u1", 0x00000, 0x020000, CRC(40d376c9) SHA1(8d2ac145760ec272b4875c8a5cff5298ca7a8259) )
	ROM_LOAD16_BYTE( "sa6-608.u2", 0x00001, 0x020000, CRC(c75a0135) SHA1(bb4aba84894d95458720f26703622622efbffb7d) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sa6-609.u1", 0x0000, 0x020000, CRC(40d376c9) SHA1(8d2ac145760ec272b4875c8a5cff5298ca7a8259) )
	ROM_LOAD16_BYTE( "sa6-609.u2", 0x0000, 0x020000, CRC(bb3cbd6f) SHA1(1642712bc3afbf9675639280667d27a74833863d) )
	ROM_LOAD16_BYTE( "sa6-610.u1", 0x0000, 0x020000, CRC(a0f9d83e) SHA1(aff8d1815832e19a67cca490206366a333aac641) )
	ROM_LOAD16_BYTE( "sa6-610.u2", 0x0000, 0x020000, CRC(bb3cbd6f) SHA1(1642712bc3afbf9675639280667d27a74833863d) )
	ROM_LOAD16_BYTE( "sa6-611.u1", 0x0000, 0x020000, CRC(c65af93d) SHA1(bd4a83d3405be39fb61d8d5b59e19c40b81a841f) )
	ROM_LOAD16_BYTE( "sa6-611.u2", 0x0000, 0x020000, CRC(c75a0135) SHA1(bb4aba84894d95458720f26703622622efbffb7d) )
ROM_END

ROM_START( mg_kf )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sw6-099.u27", 0x00000, 0x020000, CRC(ea5b6583) SHA1(70f1fd73d6e422cf0e9d39c8fcd9650085801953) )
	ROM_LOAD16_BYTE( "sw6-099.u28", 0x00001, 0x020000, CRC(bb5a36b8) SHA1(71562eba7975cefd5868ba1db6a9eeac27666cef) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw6-100.u27", 0x0000, 0x020000, CRC(ea5b6583) SHA1(70f1fd73d6e422cf0e9d39c8fcd9650085801953) )
	ROM_LOAD16_BYTE( "sw6-100.u28", 0x0000, 0x020000, CRC(a90f949b) SHA1(4d8d92e78da69628b12e418e61a073f17d97ed8a) )
ROM_END


ROM_START( mg_pbw )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sw8-148.u27", 0x00000, 0x020000, CRC(d416dbd8) SHA1(114d8529807f2895123e9438c6a3b3a0a3be5d4a) )
	ROM_LOAD16_BYTE( "sw8-148.u28", 0x00001, 0x020000, CRC(e51287bc) SHA1(526a01376f836bff5f00bb8e8130e3022752c280) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw8-149.u27", 0x00000, 0x020000, CRC(4f9745f4) SHA1(358119937423ee9687bf99ce8a473f4d88bf0699) )
	ROM_LOAD16_BYTE( "sw8-149.u28", 0x00001, 0x020000, CRC(0a9f0fd6) SHA1(7841cbb4997e0f244164072790b8936d49910879) )
	ROM_LOAD16_BYTE( "pwizu27", 0x00000, 0x080000, CRC(84af9df8) SHA1(f31fd5721cac97f17476fb71bb0071ad6c44091b) )
	ROM_LOAD16_BYTE( "pwizu28", 0x00001, 0x080000, CRC(ba00fecd) SHA1(9e78626b2f611ecd3b62fa5035041b231d53c26f) )
ROM_END


ROM_START( mg_scl )
	ROM_REGION( 0x040000, "mainrom", 0 )
	ROM_LOAD16_BYTE( "sw8-152.u27", 0x00000, 0x020000, CRC(e3bf141f) SHA1(04869f7bec38fa93b9c81946d15c7f94987704a5) )
	ROM_LOAD16_BYTE( "sw8-152.u28", 0x00001, 0x020000, CRC(da716d37) SHA1(be87bcf660e385fd543d62a382d149e743c4433a) )

	ROM_REGION( 0x1000000, "altrevs", 0 )
	ROM_LOAD16_BYTE( "sw8-153.u27", 0x00000, 0x020000, CRC(e3bf141f) SHA1(04869f7bec38fa93b9c81946d15c7f94987704a5) )
	ROM_LOAD16_BYTE( "sw8-153.u28", 0x00001, 0x020000, CRC(8afd96f1) SHA1(db3b4ef58c293cddddefeed9e3ba8b936d682dc4) )
	ROM_LOAD16_BYTE( "sclue.u27", 0x00000, 0x080000, CRC(1296124a) SHA1(502de898fee639fa7917a607ce451bc3a3374c5b) )
	ROM_LOAD16_BYTE( "sclue.u28", 0x00001, 0x080000, CRC(1330b949) SHA1(11736865f7524a1de7da235d85c4aafd7199ed62) )
ROM_END

// complete(?) dump
GAME( 199?, mg_gbr		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Guinness Book Of Records (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_risk		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Risk (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_bb		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Big Break (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_lug		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "London Underground (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_alad		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Aladdin's Cave (Maygay M2)", GAME_IS_SKELETON )

// incomplete dumps
GAME( 199?, mg_ewg		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Each Way Gambler (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_jv		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Jack & Vera (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_pbw		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Pinball Wizard (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_scl		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Super Clue (Maygay M2)", GAME_IS_SKELETON )
GAME( 199?, mg_kf		, 0			, maygayew, maygayew, driver_device, 0, ROT0, "Maygay", "Krypton Factor (Maygay M2)", GAME_IS_SKELETON )
