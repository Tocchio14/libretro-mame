/***************************************************************************

    Arcadia System - (c) 1988 Arcadia Systems

    Driver by Ernesto Corvi and Mariusz Wojcieszek

    Games supported:

        SportTime Bowling
        Leader Board
        Ninja Mission
        Road Wars
        Sidewinder
        Space Ranger
        SportTime Table Hockey
        Spot
        Magic Johnson's Fast Break
        World Darts
        Xenon
        World Trophy Soccer

    Other Arcadia games (not dumped):

        Aaargh!
        Blasta Ball
        N.Y. Warriors
        Pool
        Rockford

    Hardware description (from targets.mame.net):

        In the late 80s, Arcadia collaborated with Mastertronic to create their own
        ten-interchangeable-game arcade platform called the Arcadia Multi Select system,
        using the same hardware as the beloved Commodore Amiga computer.

        (In fact, the Multi Select's main PCB is an A500 motherboard, to which the ROM
        cage is attached through the external expansion port).

        Reportedly the system was also (or was originally) supposed to have been released
        in two five-game Super Select versions--"Arcade Action" and "Sports Simulation"
        -- but no specimens of these have ever been seen.

    NOTES and TODO:
        - To get into service mode, hold down F2 before pressing a button after
        the 'INITIALIZATION OK' message. Pressing F2 during game brings service
        mode also.

***************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "includes/amiga.h"
#include "machine/6526cia.h"
#include "machine/nvram.h"
#include "machine/amigafdc.h"


// arcadia_state was also defined in mess/includes/arcadia.h
class arcadia_amiga_state : public amiga_state
{
public:
	arcadia_amiga_state(const machine_config &mconfig, device_type type, const char *tag)
		: amiga_state(mconfig, type, tag) { }

	UINT8 coin_counter[2];
	DECLARE_WRITE16_MEMBER(arcadia_multibios_change_game);
	DECLARE_CUSTOM_INPUT_MEMBER(coin_counter_r);
	DECLARE_INPUT_CHANGED_MEMBER(coin_changed_callback);
	DECLARE_WRITE8_MEMBER(arcadia_cia_0_porta_w);
	DECLARE_WRITE8_MEMBER(arcadia_cia_0_portb_w);
	DECLARE_DRIVER_INIT(xeon);
	DECLARE_DRIVER_INIT(sdwr);
	DECLARE_DRIVER_INIT(dart);
	DECLARE_DRIVER_INIT(bowl);
	DECLARE_DRIVER_INIT(none);
	DECLARE_DRIVER_INIT(sprg);
	DECLARE_DRIVER_INIT(rdwr);
	DECLARE_DRIVER_INIT(ninj);
	DECLARE_DRIVER_INIT(airh);
	DECLARE_DRIVER_INIT(ldrba);
	DECLARE_DRIVER_INIT(socc);
};



/*************************************
 *
 *  Dynamic installation of ROM board
 *
 *************************************/

WRITE16_MEMBER(arcadia_amiga_state::arcadia_multibios_change_game)
{
	if (data == 0)
		space.install_read_bank(0x800000, 0x97ffff, "bank2");
	else
		space.nop_read(0x800000, 0x97ffff);
}



/*************************************
 *
 *  CIA-A port A access:
 *
 *  PA7 = game port 1, pin 6 (fire)
 *  PA6 = game port 0, pin 6 (fire)
 *  PA5 = /RDY (disk ready)
 *  PA4 = /TK0 (disk track 00)
 *  PA3 = /WPRO (disk write protect)
 *  PA2 = /CHNG (disk change)
 *  PA1 = /LED (LED, 0=bright / audio filter control)
 *  PA0 = OVL (ROM/RAM overlay bit)
 *
 *************************************/

WRITE8_MEMBER(arcadia_amiga_state::arcadia_cia_0_porta_w)
{
	/* switch banks as appropriate */
	machine().root_device().membank("bank1")->set_entry(data & 1);

	/* swap the write handlers between ROM and bank 1 based on the bit */
	if ((data & 1) == 0)
		/* overlay disabled, map RAM on 0x000000 */
		machine().device("maincpu")->memory().space(AS_PROGRAM)->install_write_bank(0x000000, 0x07ffff, "bank1");

	else
		/* overlay enabled, map Amiga system ROM on 0x000000 */
		machine().device("maincpu")->memory().space(AS_PROGRAM)->unmap_write(0x000000, 0x07ffff);

	/* bit 2 = Power Led on Amiga */
	set_led_status(machine(), 0, (data & 2) ? 0 : 1);
}



/*************************************
 *
 *  CIA-A port B access:
 *
 *  PB7 = parallel data 7
 *  PB6 = parallel data 6
 *  PB5 = parallel data 5
 *  PB4 = parallel data 4
 *  PB3 = parallel data 3
 *  PB2 = parallel data 2
 *  PB1 = parallel data 1
 *  PB0 = parallel data 0
 *
 *************************************/

WRITE8_MEMBER(arcadia_amiga_state::arcadia_cia_0_portb_w)
{
	/* writing a 0 in the low bit clears one of the coins */
	if ((data & 1) == 0)
	{

		if (coin_counter[0] > 0)
			coin_counter[0]--;
		else if (coin_counter[1] > 0)
			coin_counter[1]--;
	}
}



/*************************************
 *
 *  Coin counters
 *
 *************************************/

CUSTOM_INPUT_MEMBER(arcadia_amiga_state::coin_counter_r)
{
	int coin = (FPTR)param;

	/* return coin counter values */
	return coin_counter[coin] & 3;
}


INPUT_CHANGED_MEMBER(arcadia_amiga_state::coin_changed_callback)
{
	int coin = (FPTR)param;

	/* check for a 0 -> 1 transition */
	if (!oldval && newval && coin_counter[coin] < 3)
		coin_counter[coin] += 1;
}


static void arcadia_reset_coins(running_machine &machine)
{
	UINT8 *coin_counter = machine.driver_data<arcadia_amiga_state>()->coin_counter;

	/* reset coin counters */
	coin_counter[0] = coin_counter[1] = 0;
}



/*************************************
 *
 *  Memory map
 *
 *************************************/

static ADDRESS_MAP_START( amiga_map, AS_PROGRAM, 16, arcadia_amiga_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x000000, 0x07ffff) AM_RAMBANK("bank1") AM_SHARE("chip_ram")
	AM_RANGE(0xbfd000, 0xbfefff) AM_READWRITE_LEGACY(amiga_cia_r, amiga_cia_w)
	AM_RANGE(0xc00000, 0xdfffff) AM_READWRITE_LEGACY(amiga_custom_r, amiga_custom_w) AM_SHARE("custom_regs")
	AM_RANGE(0xe80000, 0xe8ffff) AM_READWRITE_LEGACY(amiga_autoconfig_r, amiga_autoconfig_w)
	AM_RANGE(0xf80000, 0xffffff) AM_ROM AM_REGION("user1", 0)		/* Kickstart BIOS */

	AM_RANGE(0x800000, 0x97ffff) AM_ROMBANK("bank2") AM_REGION("user3", 0)
	AM_RANGE(0x980000, 0x9fbfff) AM_ROM AM_REGION("user2", 0)
	AM_RANGE(0x9fc000, 0x9ffffd) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0x9ffffe, 0x9fffff) AM_WRITE(arcadia_multibios_change_game)
	AM_RANGE(0xf00000, 0xf7ffff) AM_ROM AM_REGION("user2", 0)
ADDRESS_MAP_END



/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( arcadia )
	PORT_START("CIA0PORTA")
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_SPECIAL )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL

	PORT_START("CIA0PORTB")
	PORT_DIPNAME( 0x01, 0x01, "DSW1 1" )
	PORT_DIPSETTING(    0x01, "Reset" )
	PORT_DIPSETTING(    0x00, "Set" )
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x30, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, arcadia_amiga_state,coin_counter_r, 0)
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, arcadia_amiga_state,coin_counter_r, 1)

	PORT_START("JOY0DAT")
	PORT_BIT( 0x0303, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, arcadia_amiga_state,amiga_joystick_convert, "P1JOY")
	PORT_BIT( 0xfcfc, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("JOY1DAT")
	PORT_BIT( 0x0303, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, arcadia_amiga_state,amiga_joystick_convert, "P2JOY")
	PORT_BIT( 0xfcfc, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("POTGO")
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0xaaff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P1JOY")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)

	PORT_START("P2JOY")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)

	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, arcadia_amiga_state,coin_changed_callback, 0)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, arcadia_amiga_state,coin_changed_callback, 1)
INPUT_PORTS_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

static const mos6526_interface cia_0_intf =
{
	0,														/* tod_clock */
	DEVCB_LINE(amiga_cia_0_irq),										/* irq_func */
	DEVCB_NULL,	/* pc_func */
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_INPUT_PORT("CIA0PORTA"),
	DEVCB_DRIVER_MEMBER(arcadia_amiga_state,arcadia_cia_0_porta_w),	/* port A */
	DEVCB_INPUT_PORT("CIA0PORTB"),
	DEVCB_DRIVER_MEMBER(arcadia_amiga_state,arcadia_cia_0_portb_w)	/* port B */
};

static const mos6526_interface cia_1_intf =
{
	0,														/* tod_clock */
	DEVCB_LINE(amiga_cia_1_irq),										/* irq_func */
	DEVCB_NULL,	/* pc_func */
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

static MACHINE_CONFIG_START( arcadia, arcadia_amiga_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, AMIGA_68000_NTSC_CLOCK)
	MCFG_CPU_PROGRAM_MAP(amiga_map)

	MCFG_MACHINE_RESET(amiga)
	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_UPDATE_BEFORE_VBLANK)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(59.997)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(512*2, 262)
	MCFG_SCREEN_VISIBLE_AREA((129-8)*2, (449+8-1)*2, 44-8, 244+8-1)
	MCFG_SCREEN_UPDATE_STATIC(amiga)

	MCFG_PALETTE_LENGTH(4096)
	MCFG_PALETTE_INIT(amiga)

	MCFG_VIDEO_START(amiga)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("amiga", AMIGA, 3579545)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.50)
	MCFG_SOUND_ROUTE(2, "rspeaker", 0.50)
	MCFG_SOUND_ROUTE(3, "lspeaker", 0.50)

	/* cia */
	MCFG_MOS8520_ADD("cia_0", AMIGA_68000_NTSC_CLOCK / 10, cia_0_intf)
	MCFG_MOS8520_ADD("cia_1", AMIGA_68000_NTSC_CLOCK / 10, cia_1_intf)

	/* fdc */
	MCFG_AMIGA_FDC_ADD("fdc", AMIGA_68000_NTSC_CLOCK)
MACHINE_CONFIG_END



/*************************************
 *
 *  BIOS definition
 *
 *************************************/

#define ROM_LOAD16_BYTE_BIOS(bios,name,offset,length,hash)     ROMX_LOAD(name, offset, length, hash, ROM_SKIP(1) | ROM_BIOS(bios+1))

#define ARCADIA_BIOS \
	ROM_REGION16_BE(0x80000, "user1", 0 ) \
	ROM_LOAD16_WORD( "kick12.rom", 0x000000, 0x040000, CRC(a6ce1636) SHA1(11f9e62cf299f72184835b7b2a70a16333fc0d88) ) \
	ROM_COPY( "user1", 0x000000, 0x040000, 0x040000 ) \
	\
	ROM_REGION16_BE( 0x80000, "user2", 0 ) \
	ROM_SYSTEM_BIOS(0, "onep300", "OnePlay 3.00") \
	ROM_LOAD16_BYTE_BIOS( 0, "scpav3_0.1h", 0x000000, 0x10000, CRC(2d8e1a06) SHA1(be187f34624aeda110017c4a09242f7c00ef56a4) ) \
	ROM_LOAD16_BYTE_BIOS( 0, "scpav3_0.1l", 0x000001, 0x10000, CRC(e4f38fab) SHA1(01c2eb5965070893be6734eb1372576727716476) ) \
	ROM_SYSTEM_BIOS(1, "onep211", "OnePlay 2.11" ) \
	ROM_LOAD16_BYTE_BIOS( 1, "scpa211h", 0x000000, 0x10000, CRC(be9dbdc5) SHA1(1554da09f051ec53937d65d4e451de51bc0c69e5) ) \
	ROM_LOAD16_BYTE_BIOS( 1, "scpa211l", 0x000001, 0x10000, CRC(95b84504) SHA1(99999fc40909001b37aa1b543918118becc81800) ) \
	ROM_SYSTEM_BIOS(2, "tenp211", "TenPlay 2.11" ) \
	ROM_LOAD16_BYTE_BIOS( 2, "gcp-1-hi", 0x000000, 0x10000, CRC(67d44523) SHA1(f3e3699132cdf741518accb890c04d17374c4049) ) \
	ROM_LOAD16_BYTE_BIOS( 2, "gcp-1-lo", 0x000001, 0x10000, CRC(65d9b9cf) SHA1(5c60a0dd4a0a7d9b938ce6b0446a6ad2ecaf07ec) ) \
	ROM_LOAD16_BYTE_BIOS( 2, "gcp-2-hi", 0x020000, 0x10000, CRC(1d7594ae) SHA1(6173bbfecf18d7d9ee6bc2b6753ca9d42fabd781) ) \
	ROM_LOAD16_BYTE_BIOS( 2, "gcp-2-lo", 0x020001, 0x10000, CRC(e776198d) SHA1(694ca4cc99ed84a95d18201c94a3332f8599654f) ) \
	ROM_LOAD16_BYTE_BIOS( 2, "gcp-3-hi", 0x040000, 0x10000, CRC(3e7364be) SHA1(26e10d0ddc031a891138db36ce4f1732722e6847) ) \
	ROM_LOAD16_BYTE_BIOS( 2, "gcp-3-lo", 0x040001, 0x10000, CRC(87229e0d) SHA1(0b18544801e529f954b9e03226bd2e5475f36351) ) \
	ROM_SYSTEM_BIOS(3, "onep220", "OnePlay 2.20" ) \
	ROM_LOAD16_BYTE_BIOS( 3, "scpa_v220_1h.u12", 0x000000, 0x10000, CRC(79450b4b) SHA1(a1e508a89fc62e1c4994064f2786f491b1dc8bc6) ) \
	ROM_LOAD16_BYTE_BIOS( 3, "scpa_v220_1l.u16", 0x000001, 0x10000, CRC(d2825511) SHA1(747a37c6073224472bf261ae376ac9abfbe07554) )



ROM_START( ar_bios )
	ARCADIA_BIOS

	ROM_REGION16_BE(0x180000, "user3", ROMREGION_ERASEFF)
ROM_END


/*************************************
 *
 *  Specific games
 *
 *************************************/

/* AIRH */
ROM_START( ar_airh )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "airh_1h.bin", 0x00000, 0x10000, CRC(290e8e9e) SHA1(9215e36f02adf4064934aab99accefcb17ea6d3f) )
	ROM_LOAD16_BYTE( "airh_1l.bin", 0x00001, 0x10000, CRC(155452b6) SHA1(aeaa67ea9cc543c9a43094545450159e4784fb5c) )
ROM_END

ROM_START( ar_airh2 )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "arcadia4.u10", 0x00000, 0x10000, CRC(baf8d886) SHA1(77efbc27c1cf717dfee2686009a957029eb1b113) )
	ROM_LOAD16_BYTE( "arcadia4.u6",  0x00001, 0x10000, CRC(ccff38ee) SHA1(ae89dbc9533358c80423b2dc21f101816730be7c) )

	ROM_REGION( 0x104, "misc", ROMREGION_ERASEFF )
	ROM_LOAD( "arcadia.u14.bin",  0x000, 0x104, CRC(1af35582) SHA1(a78aa61a56dea9b5c9df8b734f99adb0383d135b) ) // bad/protected?
ROM_END


/* BOWL V 2.1 */
ROM_START( ar_bowl )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "bowl_1h.bin", 0x00000, 0x10000, CRC(c0c20422) SHA1(3576df08e7a4cdadaf9dea5da0770efe5f461b07) )
	ROM_LOAD16_BYTE( "bowl_1l.bin", 0x00001, 0x10000, CRC(1c7fe75c) SHA1(b1830b91b53ec24d4b072898acac02552e2eae97) )
	ROM_LOAD16_BYTE( "bowl_2h.bin", 0x20000, 0x10000, CRC(a1e497d8) SHA1(4b4885c6937b7cfb24921e84a80d6d4f56844a73) )
	ROM_LOAD16_BYTE( "bowl_2l.bin", 0x20001, 0x10000, CRC(ce23aa34) SHA1(4b17a8447286aeb775c4edb1968978e281422421) )
	ROM_LOAD16_BYTE( "bowl_3h.bin", 0x40000, 0x10000, CRC(0c55da71) SHA1(db8a1494fca3aa044da27ea1d3acf68be415be23) )
	ROM_LOAD16_BYTE( "bowl_3l.bin", 0x40001, 0x10000, CRC(5ce00809) SHA1(d7f336df28a033b38b5296537826d164aaf5e8c9) )
ROM_END


/* DART V 2.1 */
ROM_START( ar_dart )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "dart_1h.bin", 0x00000, 0x10000, CRC(4d6a33e2) SHA1(1a227b5b0b4aca40d46af62e44deebca60582363) )
	ROM_LOAD16_BYTE( "dart_1l.bin", 0x00001, 0x10000, CRC(3fa66973) SHA1(66b59870f8ed1bb5b46b2887d99a6a71ff0514ba) )
	ROM_LOAD16_BYTE( "dart_2h.bin", 0x20000, 0x10000, CRC(3a30426a) SHA1(bf9226d2bfd1fb2d70e55e30aa3dde953baf5792) )
	ROM_LOAD16_BYTE( "dart_2l.bin", 0x20001, 0x10000, CRC(479c0b73) SHA1(2ad958f4f2d902635d030cf3f466097da3cc421c) )
	ROM_LOAD16_BYTE( "dart_3h.bin", 0x40000, 0x10000, CRC(dd217562) SHA1(80e21112a87259785e5d172249dfe8058970fd4d) )
	ROM_LOAD16_BYTE( "dart_3l.bin", 0x40001, 0x10000, CRC(12cff829) SHA1(3826e5442bb125dff4f10ef8b0b65a2d5b8d9985) )
	ROM_LOAD16_BYTE( "dart_4h.bin", 0x60000, 0x10000, CRC(98b27f13) SHA1(eb4fe813be4f202badfb947291e75ec0df915c25) )
	ROM_LOAD16_BYTE( "dart_4l.bin", 0x60001, 0x10000, CRC(a059204c) SHA1(01fb21175957fa8e92f918ea560ceecc809ed0b7) )
	ROM_LOAD16_BYTE( "dart_5h.bin", 0x80000, 0x10000, CRC(38f4c236) SHA1(1a5501ed8e94cff584f40c3b984aff7aea9ec956) )
	ROM_LOAD16_BYTE( "dart_5l.bin", 0x80001, 0x10000, CRC(df4103cc) SHA1(c792cc52148afa7bde6458704d9de2550b6eb636) )
	ROM_LOAD16_BYTE( "dart_6h.bin", 0xa0000, 0x10000, CRC(e21cc8be) SHA1(04280eef26f4a97c2280bdec19b1bc586fceffb0) )
	ROM_LOAD16_BYTE( "dart_6l.bin", 0xa0001, 0x10000, CRC(21112d4e) SHA1(95e49aa2f23c6d005a0de3cf96a1c06adeacf2a9) )
ROM_END

ROM_START( ar_dart2 )
	ARCADIA_BIOS

	ROM_REGION16_BE(0x200000, "user3", 0)
	ROM_LOAD16_BYTE( "arcadia3.u10", 0x00000, 0x10000, CRC(7166c736) SHA1(9892725b4d0aac6486950f8153083ba3f6982ad8) )
	ROM_LOAD16_BYTE( "arcadia3.u6",  0x00001, 0x10000, CRC(f226137e) SHA1(61540885ff83420f5adc1087547e0ece33383ffd) )
	ROM_LOAD16_BYTE( "arcadia3.u11", 0x20000, 0x10000, CRC(5d9a7804) SHA1(acd8eb7fd28594e8008eb953f843a72f12782ea2) )
	ROM_LOAD16_BYTE( "arcadia3.u7",  0x20001, 0x10000, CRC(a4876410) SHA1(668d120f688d31d86645fa67e01cd7e6356ca340) )
	ROM_LOAD16_BYTE( "arcadia3.u12", 0x40000, 0x10000, CRC(0cb77441) SHA1(cadd5656d2a97a852bcc231c7df22965f751a80a) )
	ROM_LOAD16_BYTE( "arcadia3.u8",  0x40001, 0x10000, CRC(97caab85) SHA1(9fd1b90d9cce073db81724ddd62f3e4ec610792b) )
	ROM_LOAD16_BYTE( "arcadia3.u13", 0x60000, 0x10000, CRC(9ccf347e) SHA1(9c2b5276543d20006e115459ac5451e2e807b28f) )
	ROM_LOAD16_BYTE( "arcadia3.u9",  0x60001, 0x10000, CRC(39c6675a) SHA1(6b79c2afab5b42b83d673836b94d7baa7a4e3214) )
	ROM_LOAD16_BYTE( "arcadia3.u19", 0x80000, 0x10000, CRC(2495a19e) SHA1(2cfb4c1ebb67b534d7c6df28512c80b52931e244) )
	ROM_LOAD16_BYTE( "arcadia3.u15", 0x80001, 0x10000, BAD_DUMP CRC(6d7006df) SHA1(eb6a0c9df3ec1bf5d0a180e138a591f374555f1c) ) // FIXED BITS (0000x000)
	ROM_LOAD16_BYTE( "arcadia3.u20", 0xa0000, 0x10000, CRC(efb0f2e2) SHA1(7ba1d85ac573db9bbd1ef04d0770c7c1277bc10e) )
	ROM_LOAD16_BYTE( "arcadia3.u16", 0xa0001, 0x10000, CRC(a9c5e939) SHA1(75de7c0fb4654b6738ecd0c170589f3a46012f33) )

	ROM_REGION( 0x104, "misc", ROMREGION_ERASEFF )
	ROM_LOAD( "arcadia.u14.bin",  0x000, 0x104, CRC(1af35582) SHA1(a78aa61a56dea9b5c9df8b734f99adb0383d135b) ) // bad/protected?
ROM_END


/*
    Magic Johnson's Fast Break

    Piggyback 1.5mb rom board

             3h       7h       x
             2h       6h       x
             1h       5h       x
             scpa1h   4h       8h
             3l       7l       x
             2l       6l       x
     DS1220Y   1l       5l       x
     sec-scpa  scpa1l   4l       8l
*/
ROM_START( ar_fast )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "fastv28.1h", 0x000000, 0x10000, CRC(091e4533) SHA1(61a16deecd32b386d62aab95e8d4a61bddcd8af4) )
	ROM_LOAD16_BYTE( "fastv28.1l", 0x000001, 0x10000, CRC(8f7685c1) SHA1(b379c1a47618401cfbfcc7bd2d13ae51f5e73e46) )
	ROM_LOAD16_BYTE( "fastv28.2h", 0x020000, 0x10000, CRC(3a3dd931) SHA1(7be3316e2acf6b14b29ef2e36d8f76999d5d4e94) )
	ROM_LOAD16_BYTE( "fastv28.2l", 0x020001, 0x10000, CRC(4838d7e5) SHA1(d2ae5b8f25df51936937ddf62001347fccdf830a) )
	ROM_LOAD16_BYTE( "fastv28.3h", 0x040000, 0x10000, CRC(db94fa62) SHA1(4fe79a4226161b15ecdda9d85c1ad84cf31b6a30) )
	ROM_LOAD16_BYTE( "fastv28.3l", 0x040001, 0x10000, CRC(a400367d) SHA1(a4362beeb35fa0c9020883eab0a71194f3a90b9a) )
	ROM_LOAD16_BYTE( "fastv28.4h", 0x060000, 0x10000, CRC(c0a021dd) SHA1(c4c40c05050a2831b55683d85ee39b8870e0bf88) )
	ROM_LOAD16_BYTE( "fastv28.4l", 0x060001, 0x10000, CRC(870e60f1) SHA1(0f0566da96dfc898dbbc35dfaba489d1fc9ab435) )
	ROM_LOAD16_BYTE( "fastv28.5h", 0x080000, 0x10000, CRC(6daf4817) SHA1(ca0bf79e77a3e878da1f97ff9a64107e8c112aee) )
	ROM_LOAD16_BYTE( "fastv28.5l", 0x080001, 0x10000, CRC(f489da29) SHA1(5e70183acfd0d849ae9691b312ca98698b1a2252) )
	ROM_LOAD16_BYTE( "fastv28.6h", 0x0a0000, 0x10000, CRC(b23dbcfd) SHA1(67495235016e4bcbf6251e4073d6938a3c5b0eea) )
	ROM_LOAD16_BYTE( "fastv28.6l", 0x0a0001, 0x10000, CRC(4e23e807) SHA1(69c910d70fb85d037257b19a1be9e99c617bf1c4) )
	ROM_LOAD16_BYTE( "fastv28.7h", 0x0c0000, 0x10000, CRC(74d598eb) SHA1(9434169d316fc2802e7790e5b09be086fccab351) )
	ROM_LOAD16_BYTE( "fastv28.7l", 0x0c0001, 0x10000, CRC(b0649050) SHA1(a8efdfc82a63fc16ee2103b4c96b92d6f9e7afc6) )
	ROM_LOAD16_BYTE( "fastv28.8h", 0x0e0000, 0x10000, CRC(3650aaf0) SHA1(cc37aa94360159f45076eafaae8140a661bd52f6) )
	ROM_LOAD16_BYTE( "fastv28.8l", 0x0e0001, 0x10000, CRC(82603f68) SHA1(8affe73e97b966b8e63bff2c7914fb5ead7b60ff) )

	ROM_REGION( 0x0200, "plds", 0 )
	ROM_LOAD( "pal16l8-sec-scpa.bin", 0x0000, 0x0104, NO_DUMP ) /* PAL is read protected */
ROM_END


/* LDRB */
ROM_START( ar_ldrb )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "lbg2401h", 0x00000, 0x10000, CRC(fe1287e9) SHA1(34088416d970614b31b25e982ef40fd950080b3e) )
	ROM_LOAD16_BYTE( "lbg2401l", 0x00001, 0x10000, CRC(7c7bb9ee) SHA1(5c76445732ab139db82fe21c16f49e2609bb03aa) )
	ROM_LOAD16_BYTE( "lbg2402h", 0x20000, 0x10000, CRC(64e5fbae) SHA1(0dde0d05b05f232aac9ad44398cedd8c7627f146) )
	ROM_LOAD16_BYTE( "lbg2402l", 0x20001, 0x10000, CRC(bb115e1c) SHA1(768cf51661f630b1c0a4b83b9f6124c78a517d0a) )
	ROM_LOAD16_BYTE( "lbg2403h", 0x40000, 0x10000, CRC(1d290e28) SHA1(0d589628fe59de9d7e2a57ddeabca991d1c79fdf) )
	ROM_LOAD16_BYTE( "lbg2403l", 0x40001, 0x10000, CRC(b1352a77) SHA1(ac7337a3778442d444002f730e2880f61f32cf2a) )
	ROM_LOAD16_BYTE( "lbg2404h", 0x60000, 0x10000, CRC(b621c688) SHA1(f2a50ebfc50725cdef77bb8a4864405dbb203784) )
	ROM_LOAD16_BYTE( "lbg2404l", 0x60001, 0x10000, CRC(13f9c4b0) SHA1(08a1fab271307191c5caa108c4ae284f92c270e4) )
	ROM_LOAD16_BYTE( "lbg2405h", 0x80000, 0x10000, CRC(71273172) SHA1(2b6204fdf03268e920b5948c999aa725fc66cac6) )
	ROM_LOAD16_BYTE( "lbg2405l", 0x80001, 0x10000, CRC(d9028183) SHA1(009b496da31f67b11de54e50254a9897ea68cd92) )
	ROM_LOAD16_BYTE( "lbg2406h", 0xa0000, 0x10000, CRC(a6ce61a4) SHA1(6cd64b7d589c91aeee06293f473fd1b3c56b19e0) )
	ROM_LOAD16_BYTE( "lbg2406l", 0xa0001, 0x10000, CRC(13c71422) SHA1(93e6dca2b28e1b5235b922f064be96eed0bedd8c) )
	ROM_LOAD16_BYTE( "lbg2407h", 0xc0000, 0x10000, CRC(61807fa9) SHA1(9d7097b921cf4026bb2828780e3fb87e0a3a24a0) )
	ROM_LOAD16_BYTE( "lbg2407l", 0xc0001, 0x10000, CRC(c62dae9f) SHA1(59b8e1c2469edd57024a4f3ca4222811442fa077) )
	ROM_LOAD16_BYTE( "lbg2408h", 0xe0000, 0x10000, CRC(b5911807) SHA1(b2995b308b2618f312005f130048e73c151311ae) )
	ROM_LOAD16_BYTE( "lbg2408l", 0xe0001, 0x10000, CRC(1f1ea828) SHA1(4af463bc6d58d64d4f082971c71654a6bb0c26bc) )
ROM_END


/* LDRB V 2.5 */
ROM_START( ar_ldrba )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "ldrb_1h.bin", 0x00000, 0x10000, CRC(0236511c) SHA1(22b2ee076ed57ba38413c16a52510383d8488e25) )
	ROM_LOAD16_BYTE( "ldrb_1l.bin", 0x00001, 0x10000, CRC(786d34b9) SHA1(5fd6ef94f65c6fd503d3682154b576d6509a3aa9) )
	ROM_LOAD16_BYTE( "ldrb_2h.bin", 0x20000, 0x10000, CRC(64e5fbae) SHA1(0dde0d05b05f232aac9ad44398cedd8c7627f146) )
	ROM_LOAD16_BYTE( "ldrb_2l.bin", 0x20001, 0x10000, CRC(bb115e1c) SHA1(768cf51661f630b1c0a4b83b9f6124c78a517d0a) )
	ROM_LOAD16_BYTE( "ldrb_3h.bin", 0x40000, 0x10000, CRC(1d290e28) SHA1(0d589628fe59de9d7e2a57ddeabca991d1c79fdf) )
	ROM_LOAD16_BYTE( "ldrb_3l.bin", 0x40001, 0x10000, CRC(b1352a77) SHA1(ac7337a3778442d444002f730e2880f61f32cf2a) )
	ROM_LOAD16_BYTE( "ldrb_4h.bin", 0x60000, 0x10000, CRC(b621c688) SHA1(f2a50ebfc50725cdef77bb8a4864405dbb203784) )
	ROM_LOAD16_BYTE( "ldrb_4l.bin", 0x60001, 0x10000, CRC(13f9c4b0) SHA1(08a1fab271307191c5caa108c4ae284f92c270e4) )
	ROM_LOAD16_BYTE( "ldrb_5h.bin", 0x80000, 0x10000, CRC(71273172) SHA1(2b6204fdf03268e920b5948c999aa725fc66cac6) )
	ROM_LOAD16_BYTE( "ldrb_5l.bin", 0x80001, 0x10000, CRC(d9028183) SHA1(009b496da31f67b11de54e50254a9897ea68cd92) )
	ROM_LOAD16_BYTE( "ldrb_6h.bin", 0xa0000, 0x10000, CRC(a6ce61a4) SHA1(6cd64b7d589c91aeee06293f473fd1b3c56b19e0) )
	ROM_LOAD16_BYTE( "ldrb_6l.bin", 0xa0001, 0x10000, CRC(13c71422) SHA1(93e6dca2b28e1b5235b922f064be96eed0bedd8c) )
	ROM_LOAD16_BYTE( "ldrb_7h.bin", 0xc0000, 0x10000, CRC(4ebb8d12) SHA1(c328a26139ba0792cab1020b32eb4b8e39d51a22) )
	ROM_LOAD16_BYTE( "ldrb_7l.bin", 0xc0001, 0x10000, CRC(1afa9a4f) SHA1(3e5ca56e03d693a72424b9ad0717494ea8eb561e) )
	ROM_LOAD16_BYTE( "ldrb_8h.bin", 0xe0000, 0x10000, CRC(fbdca9af) SHA1(9612eb777a00ba4153f40eaefd162ca5b5efdb54) )
	ROM_LOAD16_BYTE( "ldrb_8l.bin", 0xe0001, 0x10000, CRC(322f52eb) SHA1(3033eb753fb8b3bf56b152377bf567b06a0c8144) )
ROM_END

/* LDRB */
ROM_START( ar_ldrbb )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "ldrb_1h.u11", 0x00000, 0x10000, CRC(97dcde78) SHA1(d324415b853de17646b5266581bea27e571fa08f) )
	ROM_LOAD16_BYTE( "ldrb_1l_gcp_22.u15", 0x00001, 0x10000, CRC(b51d17f7) SHA1(56add2f69c35e5082926bd59be7f98a6a223c549) )
	ROM_LOAD16_BYTE( "ldrb_2h.u10", 0x20000, 0x10000, CRC(64e5fbae) SHA1(0dde0d05b05f232aac9ad44398cedd8c7627f146) )
	ROM_LOAD16_BYTE( "ldrb_2l.u14", 0x20001, 0x10000, CRC(bb115e1c) SHA1(768cf51661f630b1c0a4b83b9f6124c78a517d0a) )
	ROM_LOAD16_BYTE( "ldrb_3h.u9",  0x40000, 0x10000, CRC(1d290e28) SHA1(0d589628fe59de9d7e2a57ddeabca991d1c79fdf) )
	ROM_LOAD16_BYTE( "ldrb_3l.u13", 0x40001, 0x10000, CRC(b1352a77) SHA1(ac7337a3778442d444002f730e2880f61f32cf2a) )
	ROM_LOAD16_BYTE( "ldrb_4h.u20", 0x60000, 0x10000, CRC(b621c688) SHA1(f2a50ebfc50725cdef77bb8a4864405dbb203784) )
	ROM_LOAD16_BYTE( "ldrb_4l.u24", 0x60001, 0x10000, CRC(13f9c4b0) SHA1(08a1fab271307191c5caa108c4ae284f92c270e4) )
	ROM_LOAD16_BYTE( "ldrb_5h.u19", 0x80000, 0x10000, CRC(71273172) SHA1(2b6204fdf03268e920b5948c999aa725fc66cac6) )
	ROM_LOAD16_BYTE( "ldrb_5l.u23", 0x80001, 0x10000, CRC(d9028183) SHA1(009b496da31f67b11de54e50254a9897ea68cd92) )
	ROM_LOAD16_BYTE( "ldrb_6h.u18", 0xa0000, 0x10000, CRC(a6ce61a4) SHA1(6cd64b7d589c91aeee06293f473fd1b3c56b19e0) )
	ROM_LOAD16_BYTE( "ldrb_6l.u22", 0xa0001, 0x10000, CRC(13c71422) SHA1(93e6dca2b28e1b5235b922f064be96eed0bedd8c) )
	ROM_LOAD16_BYTE( "ldrb_7h.u17", 0xc0000, 0x10000, CRC(4ebb8d12) SHA1(c328a26139ba0792cab1020b32eb4b8e39d51a22) )
	ROM_LOAD16_BYTE( "ldrb_7l.u21", 0xc0001, 0x10000, CRC(1afa9a4f) SHA1(3e5ca56e03d693a72424b9ad0717494ea8eb561e) )
	ROM_LOAD16_BYTE( "ldrb_8h.u28", 0xe0000, 0x10000, CRC(701f50ba) SHA1(4ea6be00aa2d32d739fa6ec70ec8bce470b28f90) )
	ROM_LOAD16_BYTE( "ldrb_8l.u32", 0xe0001, 0x10000, CRC(80642c1d) SHA1(fc2101b749db3ebb3499c8870026c05acf46fa4d) )
ROM_END

/* NINJ V 2.5 */
ROM_START( ar_ninj )
	ARCADIA_BIOS

	ROM_REGION16_BE(0x200000, "user3", 0)
	ROM_LOAD16_BYTE( "ninj_1h.bin", 0x00000, 0x10000, CRC(53b07b4d) SHA1(4852005adf60fe63f2da880dd32740d18fd31169) )
	ROM_LOAD16_BYTE( "ninj_1l.bin", 0x00001, 0x10000, CRC(3337a6c1) SHA1(be9719f0cd5872b51f4c6d32fcac2638c0dedaf4) )
	ROM_LOAD16_BYTE( "ninj_2h.bin", 0x20000, 0x10000, CRC(e28a5fa8) SHA1(150e26aea24706b72d2e6612280d5dddc527061b) )
	ROM_LOAD16_BYTE( "ninj_2l.bin", 0x20001, 0x10000, CRC(4f52c008) SHA1(c26bf9a7a21a5b78697a684bada90ff70160f868) )
	ROM_LOAD16_BYTE( "ninj_3h.bin", 0x40000, 0x10000, CRC(c6e4dd36) SHA1(a8dcea97e0eb1da462ad55fd543c637544bfd059) )
	ROM_LOAD16_BYTE( "ninj_3l.bin", 0x40001, 0x10000, CRC(1dca7ea5) SHA1(2950ea2e9267d27e0ebe785a08e2d6627ae5eb17) )
	ROM_LOAD16_BYTE( "ninj_4h.bin", 0x60000, 0x10000, CRC(dc1a21d4) SHA1(76463837e0da8fd61de334e00adb807c7ef92523) )
	ROM_LOAD16_BYTE( "ninj_4l.bin", 0x60001, 0x10000, CRC(64660b15) SHA1(9e9c5f61add1439613400fee0c2376dc4000e6c6) )
	ROM_LOAD16_BYTE( "ninj_5h.bin", 0x80000, 0x10000, CRC(49cda31b) SHA1(e9579b9d47f7e638f933b8ce659bc63c8bdeb0a4) )
	ROM_LOAD16_BYTE( "ninj_5l.bin", 0x80001, 0x10000, CRC(1c5ef815) SHA1(7e88c1545ee15efd928220989f8b29207a8fec7e) )
	ROM_LOAD16_BYTE( "ninj_6h.bin", 0xa0000, 0x10000, CRC(b647f31e) SHA1(18367b96418ab950ba97d656e1466234af3bca80) )
	ROM_LOAD16_BYTE( "ninj_6l.bin", 0xa0001, 0x10000, CRC(9e5407e3) SHA1(85a8383573f3cd120f323e867c7fa2b6badd5aad) )
ROM_END

ROM_START( ar_ninj2 )
	ARCADIA_BIOS

	ROM_REGION16_BE(0x200000, "user3", 0)
	ROM_LOAD16_BYTE( "arcadia5.u10", 0x00000, 0x10000, CRC(217cb8eb) SHA1(7cb1da4d5d5b5af5f42c10848c7535bdeebbcd94) )
	ROM_LOAD16_BYTE( "arcadia5.u6",  0x00001, 0x10000, CRC(009bee8f) SHA1(ff61a0770643400ecddc70e603ca6589424b6831) )
	ROM_LOAD16_BYTE( "arcadia5.u11", 0x20000, 0x10000, CRC(f5c84e48) SHA1(8149a225a406ffb7c9faaa48af4947c2f66f750d) )
	ROM_LOAD16_BYTE( "arcadia5.u7",  0x20001, 0x10000, CRC(62d32d21) SHA1(3b80e0a4543d64f905139224f333a913cde60f30) )
	ROM_LOAD16_BYTE( "arcadia5.u12", 0x40000, 0x10000, CRC(c6e4dd36) SHA1(a8dcea97e0eb1da462ad55fd543c637544bfd059) )
	ROM_LOAD16_BYTE( "arcadia5.u8",  0x40001, 0x10000, CRC(1dca7ea5) SHA1(2950ea2e9267d27e0ebe785a08e2d6627ae5eb17) )
	ROM_LOAD16_BYTE( "arcadia5.u13", 0x60000, 0x10000, CRC(1bca3231) SHA1(57bd0516c30c4b7d8b4ca27580f770732b1a56bd) )
	ROM_LOAD16_BYTE( "arcadia5.u9",  0x60001, 0x10000, CRC(df2f98ae) SHA1(cbad0371109aab9fa6c72a022a496e9b85c5fcd4) )
	ROM_LOAD16_BYTE( "arcadia5.u19", 0x80000, 0x10000, CRC(49cda31b) SHA1(e9579b9d47f7e638f933b8ce659bc63c8bdeb0a4) )
	ROM_LOAD16_BYTE( "arcadia5.u15", 0x80001, 0x10000, CRC(1c5ef815) SHA1(7e88c1545ee15efd928220989f8b29207a8fec7e) )
	ROM_LOAD16_BYTE( "arcadia5.u20", 0xa0000, 0x10000, CRC(7359920b) SHA1(72c7438f9f5ef5d6a23f11fc58d32f8e1ff0ae44) )
	ROM_LOAD16_BYTE( "arcadia5.u16", 0xa0001, 0x10000, CRC(85a639bb) SHA1(22bfadfe6c8fd366e45ec172c070d9811e1ea8a9) )

	ROM_REGION( 0x104, "misc", ROMREGION_ERASEFF )
	ROM_LOAD( "arcadia.u14.bin",  0x000, 0x104, CRC(1af35582) SHA1(a78aa61a56dea9b5c9df8b734f99adb0383d135b) ) // bad/protected?
ROM_END



/* RDWR V 2.3 */
ROM_START( ar_rdwr )
	ARCADIA_BIOS

	ROM_REGION16_BE(0x200000, "user3", 0)
	ROM_LOAD16_BYTE( "rdwr_1h.bin", 0x00000, 0x10000, CRC(f52cb704) SHA1(cce8c7484ae8c3a3d14b2e79a981780a277c9b1c) )
	ROM_LOAD16_BYTE( "rdwr_1l.bin", 0x00001, 0x10000, CRC(fde0de6d) SHA1(7f62ce854a040775548c5ba3b05e6a4dcb0d7cfb) )
	ROM_LOAD16_BYTE( "rdwr_2h.bin", 0x20000, 0x10000, CRC(8f3c1a2c) SHA1(e473e55457c04ebd597375e9936aeb0473507ed7) )
	ROM_LOAD16_BYTE( "rdwr_2l.bin", 0x20001, 0x10000, CRC(21865e15) SHA1(be4b0e77a17edeb77f6a9d4bec6d49d4a46242ea) )
	ROM_LOAD16_BYTE( "rdwr_3h.bin", 0x40000, 0x10000, CRC(0cb3bc66) SHA1(5e22abcd38fc74f472cc5090b7c2893aaabc37bd) )
	ROM_LOAD16_BYTE( "rdwr_3l.bin", 0x40001, 0x10000, CRC(d863a958) SHA1(d27b8ff2daa51319d5c44700c6dd74e4bc8d99a4) )
	ROM_LOAD16_BYTE( "rdwr_4h.bin", 0x60000, 0x10000, CRC(466fe771) SHA1(1cc65887e097302bd504b8c4da5f7d2b760d7f74) )
	ROM_LOAD16_BYTE( "rdwr_4l.bin", 0x60001, 0x10000, CRC(fff39238) SHA1(05b4a70e1f808254e1fb20a15c460655d14d4216) )
ROM_END


/* SDWR V 2.1 */
ROM_START( ar_sdwr )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "sdwr_1h.bin", 0x00000, 0x10000, CRC(aef3eea8) SHA1(4bf7619f52395fcbde3c8f7af3fd7da4af03c673) )
	ROM_LOAD16_BYTE( "sdwr_1l.bin", 0x00001, 0x10000, CRC(daed4add) SHA1(a9404a87f1958d7ab829fbb48855d2deb64c5aec) )
	ROM_LOAD16_BYTE( "sdwr_2h.bin", 0x20000, 0x10000, CRC(d67ba564) SHA1(2afba72a77806e3925c9ca1e13c16c442a6cfc3a) )
	ROM_LOAD16_BYTE( "sdwr_2l.bin", 0x20001, 0x10000, CRC(97f58a6d) SHA1(161bc8b3e14e5efca7b988f80cc16345280ca4bd) )
	ROM_LOAD16_BYTE( "sdwr_3h.bin", 0x40000, 0x10000, CRC(b31ad2b2) SHA1(66003bd331f61d1bd2e8f4d595b61503dad4e4b8) )
	ROM_LOAD16_BYTE( "sdwr_3l.bin", 0x40001, 0x10000, CRC(af929620) SHA1(5fde0f199016abf8fd9db821ee492feeba21b604) )
	ROM_LOAD16_BYTE( "sdwr_4h.bin", 0x60000, 0x10000, CRC(7502a271) SHA1(aa318619c0b98873b435b5bbf7feb2d5d51198f9) )
	ROM_LOAD16_BYTE( "sdwr_4l.bin", 0x60001, 0x10000, CRC(942d50b4) SHA1(eb0c9057ffd0d03dc2cde1158ce9f07de8ea6905) )
	ROM_LOAD16_BYTE( "sdwr_5h.bin", 0x80000, 0x10000, CRC(c25ac91d) SHA1(da4d46a2c987e2be2e31c081557b2de1744fa237) )
	ROM_LOAD16_BYTE( "sdwr_5l.bin", 0x80001, 0x10000, CRC(ecd1fbd3) SHA1(0b859d608859ccbff03db655219dfea4e609454d) )
	ROM_LOAD16_BYTE( "sdwr_6h.bin", 0xa0000, 0x10000, CRC(ea3c8ab3) SHA1(95cb5b9dd29c19862a2659867474cbf49192f830) )
	ROM_LOAD16_BYTE( "sdwr_6l.bin", 0xa0001, 0x10000, CRC(2544ccd7) SHA1(953aa00f2610ecd31db6e36964cbe7c2866050b9) )
ROM_END

ROM_START( ar_sdwr2 )
	ARCADIA_BIOS

	ROM_REGION16_BE(0x200000, "user3", 0)
	ROM_LOAD16_BYTE( "arcadia1.u10", 0x00000, 0x10000, CRC(30949f1f) SHA1(270cd449f994eda76afd5532018bad636ac4cf68) )
	ROM_LOAD16_BYTE( "arcadia1.u6",  0x00001, 0x10000, CRC(c760d1c4) SHA1(7d311e8b192e493da9501755e096599e1e8e8d3e) )
	ROM_LOAD16_BYTE( "arcadia1.u11", 0x20000, 0x10000, CRC(d67ba564) SHA1(2afba72a77806e3925c9ca1e13c16c442a6cfc3a) )
	ROM_LOAD16_BYTE( "arcadia1.u7",  0x20001, 0x10000, CRC(97f58a6d) SHA1(161bc8b3e14e5efca7b988f80cc16345280ca4bd) )
	ROM_LOAD16_BYTE( "arcadia1.u12", 0x40000, 0x10000, CRC(b31ad2b2) SHA1(66003bd331f61d1bd2e8f4d595b61503dad4e4b8) )
	ROM_LOAD16_BYTE( "arcadia1.u8",  0x40001, 0x10000, CRC(af929620) SHA1(5fde0f199016abf8fd9db821ee492feeba21b604) )
	ROM_LOAD16_BYTE( "arcadia1.u13", 0x60000, 0x10000, CRC(360b17a6) SHA1(db7e1ce1c545139142804ce68ce5232d153fb0f2) )
	ROM_LOAD16_BYTE( "arcadia1.u9",  0x60001, 0x10000, CRC(d724e563) SHA1(0012bc6354642a761fc223627401542ae48aea6e) )
	ROM_LOAD16_BYTE( "arcadia1.u19", 0x80000, 0x10000, CRC(c25ac91d) SHA1(da4d46a2c987e2be2e31c081557b2de1744fa237) )
	ROM_LOAD16_BYTE( "arcadia1.u15", 0x80001, 0x10000, CRC(ecd1fbd3) SHA1(0b859d608859ccbff03db655219dfea4e609454d) )
	ROM_LOAD16_BYTE( "arcadia1.u20", 0xa0000, 0x10000, CRC(5791440b) SHA1(fc9297343ddc2e6e1e22b7ed9a986777519061db) )
	ROM_LOAD16_BYTE( "arcadia1.u16", 0xa0001, 0x10000, CRC(e63e1679) SHA1(0762bff0018e006905a2e58141fdf05910f06e29) )

	ROM_REGION( 0x104, "misc", ROMREGION_ERASEFF )
	ROM_LOAD( "arcadia.u14.bin",  0x000, 0x104, CRC(1af35582) SHA1(a78aa61a56dea9b5c9df8b734f99adb0383d135b) ) // bad/protected?
ROM_END


/* SOCC */
ROM_START( ar_socc )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "socc30.1hi", 0x00000, 0x10000, CRC(b4df41cf) SHA1(681cecef390f82be420cd9f4d32afddace3da3c8) )
	ROM_LOAD16_BYTE( "socc30.1lo", 0x00001, 0x10000, CRC(28b5e119) SHA1(3fff3252b855e6ca1aceff89ec0c61688c954d88) )
	ROM_LOAD16_BYTE( "socc30.2hi", 0x20000, 0x10000, CRC(b3c14026) SHA1(be4e05cbf5b14a1dc77eff70bb44c8cdae57e59a) )
	ROM_LOAD16_BYTE( "socc30.2lo", 0x20001, 0x10000, CRC(f7f9a734) SHA1(064adbfb919c7422190b5e4146fbc335a2a97091) )
	ROM_LOAD16_BYTE( "socc30.3hi", 0x40000, 0x10000, CRC(2a2bd2a0) SHA1(9a9a0463ecf99941f88a163dcb111bf8c3508d78) )
	ROM_LOAD16_BYTE( "socc30.3lo", 0x40001, 0x10000, CRC(f335bb8b) SHA1(a15ee6b27d6e2aab25f3260edd0803fe1fa05f0e) )
	ROM_LOAD16_BYTE( "socc30.4hi", 0x60000, 0x10000, CRC(4f2f28dc) SHA1(cd419c80e7112163fd182d2cc58148d3674ccd8c) )
	ROM_LOAD16_BYTE( "socc30.4lo", 0x60001, 0x10000, CRC(b326d36c) SHA1(5ac2df36754c97065f74695a9c46e2a558e8e112) )
	ROM_LOAD16_BYTE( "socc30.5hi", 0x80000, 0x10000, CRC(4fcaec4a) SHA1(1286c57a81358a69573df6f6cedf44c564476320) )
	ROM_LOAD16_BYTE( "socc30.5lo", 0x80001, 0x10000, CRC(f131115e) SHA1(a9050878cbc1923bb88d46ecdad8f5bd69d389ea) )
	ROM_LOAD16_BYTE( "socc30.6hi", 0xa0000, 0x10000, CRC(9380644f) SHA1(01da02f9397c7b8fd44a78a533f09264eb4f3d0c) )
	ROM_LOAD16_BYTE( "socc30.6lo", 0xa0001, 0x10000, CRC(b93e13ea) SHA1(0fac9cb54099f0d0ad61eb6f5bd3686c27a045b9) )
ROM_END


/* SPOT */
ROM_START( ar_spot )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "spotv2.1h", 0x00000, 0x10000, CRC(a8440838) SHA1(8d64ddb603754c85aad47bd079d0a7d80d57b36c) )
	ROM_LOAD16_BYTE( "spotv2.1l", 0x00001, 0x10000, CRC(2abd2835) SHA1(b419da47c6390334ed8af56bc21430e5b43d6d58) )
	ROM_LOAD16_BYTE( "spotv2.2h", 0x20000, 0x10000, CRC(f4c95f77) SHA1(46c70755e3c6d06bec4b1bd164a586292a59249d) )
	ROM_LOAD16_BYTE( "spotv2.2l", 0x20001, 0x10000, CRC(58d7bf54) SHA1(0da63d32d738f8ed3675c6d14b2d12039af5ff21) )
	ROM_LOAD16_BYTE( "spotv2.3h", 0x40000, 0x10000, CRC(c9d2f3b7) SHA1(1b4693bcde14dc5eefe7456d4d613e6cb674c972) )
	ROM_LOAD16_BYTE( "spotv2.3l", 0x40001, 0x10000, CRC(adf94e81) SHA1(5ad56044008236edea0a44393daee06e572b1cc2) )
	ROM_LOAD16_BYTE( "spotv2.4h", 0x60000, 0x10000, CRC(cdea2feb) SHA1(4bb24b8cb5dd1e88d3f468979e2f350568414668) )
	ROM_LOAD16_BYTE( "spotv2.4l", 0x60001, 0x10000, CRC(214c353b) SHA1(819283248eac2a516f9fcdda060284ffe9c39bc8) )
	ROM_LOAD16_BYTE( "spotv2.5h", 0x80000, 0x10000, CRC(809d0f5c) SHA1(d1bae86090db8e5cc066afb76203704e7d217fde) )
	ROM_LOAD16_BYTE( "spotv2.5l", 0x80001, 0x10000, CRC(b86d8153) SHA1(42a564fa608e806d04052e67263afc4a5a417d40) )
	ROM_LOAD16_BYTE( "spotv2.6h", 0xa0000, 0x10000, CRC(8c221a34) SHA1(8f246bbcb79f5e508932d776fbfa648392f7f78d) )
	ROM_LOAD16_BYTE( "spotv2.6l", 0xa0001, 0x10000, CRC(821fa69a) SHA1(f037853be96158b8a6dd5f34e15ddfc16b6410c3) )
	ROM_LOAD16_BYTE( "spotv2.7h", 0xc0000, 0x10000, CRC(054355db) SHA1(6f4a46230b6dfd4727816737c31bce9483d3a3f7) )
	ROM_LOAD16_BYTE( "spotv2.7l", 0xc0001, 0x10000, CRC(30d396d8) SHA1(2a56727554a823f56b37b9e8d324e9f53524eb02) )
	ROM_LOAD16_BYTE( "spotv2.8h", 0xe0000, 0x10000, CRC(94dbb239) SHA1(0c475c8e102cc835d01e3de4604c1323219048f1) )
	ROM_LOAD16_BYTE( "spotv2.8l", 0xe0001, 0x10000, CRC(4d7f8f05) SHA1(04690717cec5912cd12ccb7135614842f5597898) )
ROM_END


/* SPRG */
ROM_START( ar_sprg )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "sprg_1h.bin", 0x00000, 0x10000, CRC(90b45dc5) SHA1(7cf1fc27e95bf207ed94cc5c20cf0c0ae7799d83) )
	ROM_LOAD16_BYTE( "sprg_1l.bin", 0x00001, 0x10000, CRC(e5ce68e9) SHA1(dfda2e0bffc499a497865bc214450653880eccf2) )
	ROM_LOAD16_BYTE( "sprg_2h.bin", 0x20000, 0x10000, CRC(02ef780f) SHA1(d21d6e8a379a2b38de7f3ec0540f67dd6425cbc9) )
	ROM_LOAD16_BYTE( "sprg_2l.bin", 0x20001, 0x10000, CRC(fa1f5b23) SHA1(4f808d8ee9cd672061995d0bfab65851bf1c01d3) )
	ROM_LOAD16_BYTE( "sprg_3h.bin", 0x40000, 0x10000, CRC(48130e6e) SHA1(479555c4a5c041c04135f01fbdd5c5f78f4422cf) )
	ROM_LOAD16_BYTE( "sprg_3l.bin", 0x40001, 0x10000, CRC(4b968cc6) SHA1(fbf3bcb5803dbe75e5a9bcde39c98c9c30bd1699) )
	ROM_LOAD16_BYTE( "sprg_4h.bin", 0x60000, 0x10000, CRC(23c8f667) SHA1(da75def3a34f6e7d48f2c6cefff608348c09cf70) )
	ROM_LOAD16_BYTE( "sprg_4l.bin", 0x60001, 0x10000, CRC(13ba011f) SHA1(75da2fbbfe2e957992b2a73609737d777fe9a151) )
ROM_END


/* XEON V 2.3 */
ROM_START( ar_xeon )
	ARCADIA_BIOS

	ROM_REGION16_BE( 0x180000, "user3", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "xeon_1h.bin", 0x00000, 0x10000, CRC(ca422811) SHA1(fa6f82e1d91b48d58b61f916d5b04dc1a13774fb) )
	ROM_LOAD16_BYTE( "xeon_1l.bin", 0x00001, 0x10000, CRC(97edf967) SHA1(57fca524e01ba21f7420472f14aaf3fa63a326fa) )
	ROM_LOAD16_BYTE( "xeon_2h.bin", 0x20000, 0x10000, CRC(8078c10e) SHA1(599995374b23da7187556e2f4f285b60d818f885) )
	ROM_LOAD16_BYTE( "xeon_2l.bin", 0x20001, 0x10000, CRC(a8845d8f) SHA1(2d54dc25af68c46bbbdf8f9ed8014ae7d8564e09) )
	ROM_LOAD16_BYTE( "xeon_3h.bin", 0x40000, 0x10000, CRC(9d013152) SHA1(7a3bec56d564efbca9721d308b3eddc76763ec41) )
	ROM_LOAD16_BYTE( "xeon_3l.bin", 0x40001, 0x10000, CRC(331b1449) SHA1(0e282d04b2c7e68051e5ea1671737b11dfb71521) )
	ROM_LOAD16_BYTE( "xeon_4h.bin", 0x60000, 0x10000, CRC(fbf43d5c) SHA1(6d335b7b1d3b75887526cb8ea3518661b5554774) )
	ROM_LOAD16_BYTE( "xeon_4l.bin", 0x60001, 0x10000, CRC(47b60bf5) SHA1(10d8addc090ad3fa2663c40e22f736ac3522b177) )
ROM_END



/*************************************
 *
 *  Low byte ROM decoding
 *
 *************************************/

INLINE void generic_decode(running_machine &machine, const char *tag, int bit7, int bit6, int bit5, int bit4, int bit3, int bit2, int bit1, int bit0)
{
	UINT16 *rom = (UINT16 *)machine.root_device().memregion(tag)->base();
	int i;

	/* only the low byte of ROMs are encrypted in these games */
	for (i = 0; i < 0x20000/2; i++)
		rom[i] = BITSWAP16(rom[i], 15,14,13,12,11,10,9,8, bit7,bit6,bit5,bit4,bit3,bit2,bit1,bit0);

	#if 0
	{
		UINT8 *ROM = machine.root_device().memregion(tag)->base();
		int size = machine.root_device().memregion(tag)->bytes();

		FILE *fp;
		char filename[256];
		sprintf(filename,"decrypted_%s", machine.system().name);
		fp=fopen(filename, "w+b");
		if (fp)
		{
			fwrite(ROM, size, 1, fp);
			fclose(fp);
		}
	}
	#endif
}



/*************************************
 *
 *  Generic driver init
 *
 *************************************/

static void arcadia_init(running_machine &machine)
{
	arcadia_amiga_state *state = machine.driver_data<arcadia_amiga_state>();
	static const amiga_machine_interface arcadia_intf =
	{
		ANGUS_CHIP_RAM_MASK,
		NULL, NULL, NULL,
		NULL,
		NULL,  arcadia_reset_coins,
		NULL,
		0
	};
	UINT16 *biosrom;

	/* configure our Amiga setup */
	amiga_machine_config(machine, &arcadia_intf);

	/* set up memory */
	state->membank("bank1")->configure_entry(0, state->m_chip_ram);
	state->membank("bank1")->configure_entry(1, machine.root_device().memregion("user1")->base());

	/* OnePlay bios is encrypted, TenPlay is not */
	biosrom = (UINT16 *)machine.root_device().memregion("user2")->base();
	if (biosrom[0] != 0x4afc)
		generic_decode(machine, "user2", 6, 1, 0, 2, 3, 4, 5, 7);
}



/*************************************
 *
 *  Per-game decryption
 *
 *************************************/

DRIVER_INIT_MEMBER(arcadia_amiga_state,none)   { arcadia_init(machine()); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,airh)   { arcadia_init(machine()); generic_decode(machine(), "user3", 5, 0, 2, 4, 7, 6, 1, 3); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,bowl)   { arcadia_init(machine()); generic_decode(machine(), "user3", 7, 6, 0, 1, 2, 3, 4, 5); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,dart)   { arcadia_init(machine()); generic_decode(machine(), "user3", 4, 0, 7, 6, 3, 1, 2, 5); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,ldrba)  { arcadia_init(machine()); generic_decode(machine(), "user3", 2, 3, 4, 1, 0, 7, 5, 6); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,ninj)   { arcadia_init(machine()); generic_decode(machine(), "user3", 1, 6, 5, 7, 4, 2, 0, 3); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,rdwr)   { arcadia_init(machine()); generic_decode(machine(), "user3", 3, 1, 6, 4, 0, 5, 2, 7); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,sdwr)   { arcadia_init(machine()); generic_decode(machine(), "user3", 6, 3, 4, 5, 2, 1, 0, 7); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,socc)   { arcadia_init(machine()); generic_decode(machine(), "user3", 0, 7, 1, 6, 5, 4, 3, 2); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,sprg)   { arcadia_init(machine()); generic_decode(machine(), "user3", 4, 7, 3, 0, 6, 5, 2, 1); }
DRIVER_INIT_MEMBER(arcadia_amiga_state,xeon)   { arcadia_init(machine()); generic_decode(machine(), "user3", 3, 1, 2, 4, 0, 5, 6, 7); }

/*************************************
 *
 *  Game drivers
 *
 *************************************/

/* BIOS */
GAME( 1988, ar_bios,	0, arcadia, arcadia, arcadia_amiga_state, none,  ROT0, "Arcadia Systems", "Arcadia System BIOS", GAME_IS_BIOS_ROOT )


GAME( 1988, ar_airh,	ar_bios, arcadia, arcadia, arcadia_amiga_state, airh,  ROT0, "Arcadia Systems", "SportTime Table Hockey (Arcadia, set 1, V 2.1)", 0 )
GAME( 1988, ar_airh2,	ar_airh, arcadia, arcadia, arcadia_amiga_state, airh,  ROT0, "Arcadia Systems", "SportTime Table Hockey (Arcadia, set 2)", 0 )

GAME( 1988, ar_bowl,	ar_bios, arcadia, arcadia, arcadia_amiga_state, bowl,  ROT0, "Arcadia Systems", "SportTime Bowling (Arcadia, V 2.1)", 0 )

GAME( 1987, ar_dart,	ar_bios, arcadia, arcadia, arcadia_amiga_state, dart,  ROT0, "Arcadia Systems", "World Darts (Arcadia, set 1, V 2.1)", 0 )
GAME( 1987, ar_dart2,	ar_dart, arcadia, arcadia, arcadia_amiga_state, dart,  ROT0, "Arcadia Systems", "World Darts (Arcadia, set 2)", GAME_NOT_WORKING ) // bad dump

GAME( 1988, ar_fast,	ar_bios, arcadia, arcadia, arcadia_amiga_state, none,  ROT0, "Arcadia Systems", "Magic Johnson's Fast Break (Arcadia, V 2.8)", 0 )

GAME( 1988, ar_ldrb,	ar_bios, arcadia, arcadia, arcadia_amiga_state, none,  ROT0, "Arcadia Systems", "Leader Board (Arcadia, set 1, V 2.4?)", 0 )
GAME( 1988, ar_ldrba,	ar_ldrb, arcadia, arcadia, arcadia_amiga_state, ldrba, ROT0, "Arcadia Systems", "Leader Board (Arcadia, set 2, V 2.5)", 0 )
GAME( 1988, ar_ldrbb,	ar_ldrb, arcadia, arcadia, arcadia_amiga_state, none,  ROT0, "Arcadia Systems", "Leader Board (Arcadia, set 3)", 0 )

GAME( 1987, ar_ninj,	ar_bios, arcadia, arcadia, arcadia_amiga_state, ninj,  ROT0, "Arcadia Systems", "Ninja Mission (Arcadia, set 1, V 2.5)", 0 )
GAME( 1987, ar_ninj2,   ar_ninj, arcadia, arcadia, arcadia_amiga_state, ninj,  ROT0, "Arcadia Systems", "Ninja Mission (Arcadia, set 2)", 0 )

GAME( 1988, ar_rdwr,	ar_bios, arcadia, arcadia, arcadia_amiga_state, rdwr,  ROT0, "Arcadia Systems", "RoadWars (Arcadia, V 2.3)", 0  )

GAME( 1988, ar_sdwr,	ar_bios, arcadia, arcadia, arcadia_amiga_state, sdwr,  ROT0, "Arcadia Systems", "Sidewinder (Arcadia, set 1, V 2.1)", 0 )
GAME( 1988, ar_sdwr2,   ar_sdwr, arcadia, arcadia, arcadia_amiga_state, sdwr,  ROT0, "Arcadia Systems", "Sidewinder (Arcadia, set 2)", 0 )

GAME( 1989, ar_socc,	ar_bios, arcadia, arcadia, arcadia_amiga_state, socc,  ROT0, "Arcadia Systems", "World Trophy Soccer (Arcadia, V 3.0)", 0 )

GAME( 1990, ar_spot,	ar_bios, arcadia, arcadia, arcadia_amiga_state, none,  ROT0, "Arcadia Systems", "Spot (Arcadia)", 0 )

GAME( 1987, ar_sprg,	ar_bios, arcadia, arcadia, arcadia_amiga_state, sprg,  ROT0, "Arcadia Systems", "Space Ranger (Arcadia, V 2.0)", 0 )

GAME( 1988, ar_xeon,	ar_bios, arcadia, arcadia, arcadia_amiga_state, xeon,  ROT0, "Arcadia Systems", "Xenon (Arcadia, V 2.3)", 0 )
