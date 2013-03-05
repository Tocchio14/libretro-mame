/*****************************************************************************

    Casio Loopy (c) 1995 Casio

    skeleton driver

    TODO:
    - Identify what actually is the NEC CDT-109 CPU, it should contain a program
      controller for the thermal printer device

    Note:
    - just a placeholder for any HW discovery, until we decap/trojan the BIOS,
      the idea is to understand the HW enough to extract the SH-1 internal BIOS
      data via a trojan;

	ASM notes:
	- first vector is almost certainly VBR value.
	- [VBR + 0x2c] irq for i/o?
	- [VBR + 0x140] points to an internal BIOS routine, at 0x6238
	  (Nigaoe Artist has a direct 0x648c instead)
	- 0x0604 is probably a BRA -2 / NOP (some games puts that as a null irq vector)
	- Nigaoe Artist jumps to 0x668 at some point.


===============================================================================

Casio Loopy PCB Layout
----------------------

JCM631-MA1M C
|---------------------------------------------------------|
|    CB    CC              CD         CE       CF      CG |
|--|                                                      |
   |                                        BA10339F      |
|--| 15218  |--|     CXA1645M                           CH|
|           |  |                A1603C                    |
|    15218  |  |                                          |
|           |  |                                          |
|BIOS.LSI352|  |                                          |
|           |  |                      21MHz               |
| |--------||  |   |------|                 SW1           |
| |NEC     ||  |   |SH7021|      |----------|             |
| |CDT109  ||CA|   |      |      |          |             |
| |        ||  |   |------|      |CASIO     |             |
| |--------||  |                 |RH-7500   |             |
|           |  |                 |5C315     |          |--|
| |-------| |  |                 |          |          |
| |CASIO  | |  |                 |----------|          |--|
| |RH-7501| |  |  HM514260                                |
| |5C350  | |  |                               HM62256    |
| |-------| |  |                                          |
| 6379      |--|    SW301                      HM62256    /
|--------|                        HM538123               /
         |                                              /
         |                                             /
         |--------------------------------------------/

Notes:
      Connectors
      ----------
      CA - Cartridge connector
      CB - Power Input connector
      CC - Composite Video and Audio Out connector
      CD - Printer Cassette Motor connector
      CE - Printer Data connector
      CF - Printer Head connector
      CG - Paper Sensor connector
      CH - Joystick connector
      Connectors on the back of the main unit include RCA audio (left/right), RCA composite video,
      24V DC power input and contrast knob.
      On top of the main unit, there is a reset button, on/off slide switch, a big eject button, a
      button to cut off stickers after they're printed, a button to open the hatch where the sticker
      cassette is located and a red LED for power.

      IC's
      ----
      BIOS2.LSI352- Hitachi HN62434 512k x8 (4MBit) maskROM (SOP40)
      CDT-109     - NEC CDT109 (QFP120). This is some kind of CPU, the package looks a bit
                    like a V60. The BIOS is tied directly to it.
      RH-7500     - Casio RH-7500 5C315 (QFP208). This is the graphics generator chip.
      RH-7501     - Casio RH-7501 5C350 (QFP64). This is probably the sound chip.
      SH7021      - Hitachi HD6437021TE20 SuperH RISC Engine SH-2A CPU with 32k internal maskROM (TQFP100)
                    The internal ROM (BIOS1) is not dumped. A SH-2A software programming manual is available here...
                    http://documentation.renesas.com/eng/products/mpumcu/rej09b0051_sh2a.pdf
      CXA1645M    - Sony CXA1645M RGB Encoder (RGB -> Composite Video) (SOIC24)
      A1603C      - NEC uPA1603C Compound Field Effect Power Transistor Array (DIP16)
      HM514260    - Hitachi HM514260 256k x 16 DRAM (SOJ40)
      HM538123    - Hitachi HM538123 128k x8 multi-port Video DRAM with 256-word x8 serial access memory (SOJ40)
      HM62256     - Hitachi HM62256 32k x8 SRAM (SOP28)
      BA10339F    - Rohm BA10339F Quad Comparitor (SOIC14)
      6379        - NEC uPD6379 2-channel 16-bit D/A convertor for digital audio signal demodulation (SOIC8)
      15218       - Rohm BA15218 Dual High Slew Rate, Low Noise Operational Amplifier (SOIC8)

      Other
      -----
      SW1        - Reset Switch
      SW301      - ON/OFF Slide Switch


Carts
-----
There are reports of 11 existing carts.
Only 6 are available so far.

XK-401: Anime Land
XK-402: HARIHARI Seal Paradise
XK-403: Dream Change
XK-404: Nigaoe Artist
XK-501: Wanwan Aijou Monogatari
XK-502: PC Collection

The rest are not dumped yet.....


Lupiton's Wonder Palette
Magical Shop
Chakra-kun no Omajinai Paradise
XK-503: Little Romance
XK-504: I want a room in Loopy Town



Inside the carts
----------------

Carts 401 - 404:
PCB 'JCM632-AN1M C'
1x 16M maskROM (SOP44)
1x 8k x8 SRAM (SOP28)
1x 3V coin battery (CR2032)

Cart 501:
PCB 'Z544-1 A240427-1'
1x 16M maskROM (SOP44)
1x 8k x8 SRAM (SOP28)
1x OKI MSM6653A Voice Synthesis IC with 544Kbits internal maskROM (SOP24)
1x Rohm BA15218 High Slew Rate, Low Noise, Dual Operational Amplifier (SOIC8)
1x 74HC273 logic chip
1x 3V coin battery (CR2032)

Cart 502:
PCB 'Z545-1 A240570-1'
1x 16M maskROM (SOP44)
1x 32k x8 SRAM (SOP28)
1x 74HC00 logic chip
1x 3V coin battery (CR2032)

******************************************************************************/

#include "emu.h"
#include "cpu/sh2/sh2.h"
//#include "cpu/v60/v60.h"
#include "imagedev/cartslot.h"


class casloopy_state : public driver_device
{
public:
	casloopy_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_bios_rom(*this, "bios_rom"){ }

	required_shared_ptr<UINT32> m_bios_rom;
	UINT16 *m_paletteram;
	UINT8 *m_vram;
	int m_gfx_index;
	DECLARE_DRIVER_INIT(casloopy);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_casloopy(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_READ16_MEMBER(casloopy_vregs_r);
	DECLARE_WRITE16_MEMBER(casloopy_vregs_w);
	DECLARE_READ16_MEMBER(casloopy_pal_r);
	DECLARE_WRITE16_MEMBER(casloopy_pal_w);
	DECLARE_READ8_MEMBER(casloopy_vram_r);
	DECLARE_WRITE8_MEMBER(casloopy_vram_w);
};


static const gfx_layout casloopy_tile_layout =
{
	8,8,
	0x400,
	4,
	{ 0, 1, 2, 3 },
	{ STEP8(0, 4) },
	{ STEP8(0, 4*8) },
	4*8*8
};

void casloopy_state::video_start()
{
	/* TODO: proper sizes */
	m_paletteram = auto_alloc_array(machine(), UINT16, 0x1000);
	m_vram = auto_alloc_array(machine(), UINT8, 0x4000);

	for (m_gfx_index = 0; m_gfx_index < MAX_GFX_ELEMENTS; m_gfx_index++)
		if (machine().gfx[m_gfx_index] == 0)
			break;

	machine().gfx[m_gfx_index] = auto_alloc(machine(), gfx_element(machine(), casloopy_tile_layout, m_vram, 0x10, 0));
}

UINT32 casloopy_state::screen_update_casloopy(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	gfx_element *gfx = machine().gfx[m_gfx_index];
	int x,y;
	int count;

	count = 0;

	for (y=0;y<32;y++)
	{
		for (x=0;x<32;x++)
		{
			UINT16 tile = (m_vram[count+1])|(m_vram[count]<<8);

			tile &= 0x3ff; //???
			tile |= 0x100;

			drawgfx_opaque(bitmap,cliprect,gfx,tile,7,0,0,x*8,y*8);

			count+=2;
		}
	}

	return 0;
}

READ16_MEMBER(casloopy_state::casloopy_vregs_r)
{
	if(offset == 4/2)
	{
		return (machine().primary_screen->vblank() << 8); /*| (machine().primary_screen->vpos() & 0xff);*/
	}

	printf("%08x\n",offset*2);

	return 0xffff;
}

WRITE16_MEMBER(casloopy_state::casloopy_vregs_w)
{
	printf("%08x %08x\n",offset*2,data);
}

READ16_MEMBER(casloopy_state::casloopy_pal_r)
{
	return m_paletteram[offset];
}

WRITE16_MEMBER(casloopy_state::casloopy_pal_w)
{
	int r,g,b;
	COMBINE_DATA(&m_paletteram[offset]);

	b = ((m_paletteram[offset])&0x001f)>>0;
	g = ((m_paletteram[offset])&0x03e0)>>5;
	r = ((m_paletteram[offset])&0x7c00)>>10;

	palette_set_color_rgb(machine(), offset, pal5bit(r), pal5bit(g), pal5bit(b));
}

READ8_MEMBER(casloopy_state::casloopy_vram_r)
{
	return m_vram[offset];
}

WRITE8_MEMBER(casloopy_state::casloopy_vram_w)
{
	m_vram[offset] = data;

	machine().gfx[m_gfx_index]->mark_dirty(offset/256);
}


static ADDRESS_MAP_START( casloopy_map, AS_PROGRAM, 32, casloopy_state )
	AM_RANGE(0x00000000, 0x00007fff) AM_RAM AM_SHARE("bios_rom")
	AM_RANGE(0x01000000, 0x0107ffff) AM_RAM // stack pointer points here
	AM_RANGE(0x04040000, 0x04043fff) AM_READWRITE8(casloopy_vram_r,casloopy_vram_w,0xffffffff) // tilemap + PCG
	AM_RANGE(0x04051000, 0x040510ff) AM_READWRITE16(casloopy_pal_r,casloopy_pal_w,0xffffffff)
	AM_RANGE(0x04058000, 0x04058007) AM_READWRITE16(casloopy_vregs_r,casloopy_vregs_w,0xffffffff)
//	AM_RANGE(0x05ffff00, 0x05ffffff) - SH7021 internal i/o
	AM_RANGE(0x06000000, 0x061fffff) AM_ROM AM_REGION("rom_cart",0)
	AM_RANGE(0x07fff000, 0x07ffffff) AM_RAM
	AM_RANGE(0x0e000000, 0x0e1fffff) AM_ROM AM_REGION("rom_cart",0)
ADDRESS_MAP_END

#if 0
static ADDRESS_MAP_START( casloopy_sub_map, AS_PROGRAM, 16, casloopy_state )
	AM_RANGE(0xf80000, 0xffffff) AM_ROM AM_REGION("subcpu",0)
ADDRESS_MAP_END
#endif

static INPUT_PORTS_START( casloopy )
INPUT_PORTS_END

/* TODO: move this into DEVICE_IMAGE_LOAD_MEMBER */
void casloopy_state::machine_start()
{
	UINT8 *SRC = memregion("cart")->base();
	UINT8 *DST = memregion("rom_cart")->base();

	// fix endianness
	for (int i=0;i<0x200000;i+=4)
	{
		UINT8 tempa = SRC[i+0];
		UINT8 tempb = SRC[i+1];
		DST[i+0] = SRC[i+2];
		DST[i+1] = SRC[i+3];
		DST[i+2] = tempa;
		DST[i+3] = tempb;
	}
}

void casloopy_state::machine_reset()
{
	//machine().device("maincpu")->execute().set_input_line(INPUT_LINE_HALT, ASSERT_LINE); //halt the CPU until we find enough data to proceed

}

static MACHINE_CONFIG_START( casloopy, casloopy_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",SH2,8000000)
	MCFG_CPU_PROGRAM_MAP(casloopy_map)

//  MCFG_CPU_ADD("subcpu",V60,8000000)
//  MCFG_CPU_PROGRAM_MAP(casloopy_sub_map)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(casloopy_state, screen_update_casloopy)

	MCFG_PALETTE_LENGTH(512)

	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_EXTENSION_LIST("ic1,bin")
	MCFG_CARTSLOT_MANDATORY
	MCFG_CARTSLOT_INTERFACE("loopy_cart")

	/* software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list","casloopy")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( casloopy )
	ROM_REGION( 0x80000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bios1", 0x0000, 0x8000, NO_DUMP ) // SH7021 uses 32 KB

	ROM_REGION( 0x80000, "subcpu", 0) //NEC CDT-109
	ROM_LOAD( "bios2.lsi352", 0x0000, 0x80000, CRC(8f51fa17) SHA1(99f50be06b083fdb07e08f30b0b26d9037afc869) )

	ROM_REGION( 0x200000, "cart", 0 )
	ROM_CART_LOAD("cart",    0x00000, 0x200000, ROM_NOMIRROR)

	ROM_REGION( 0x200000, "rom_cart", ROMREGION_ERASE00 )
ROM_END

DRIVER_INIT_MEMBER(casloopy_state,casloopy)
{
	/* load hand made bios data*/
	m_bios_rom[0/4] = 0x6000480;//0x600af3c;//0x6000964; //SPC
	m_bios_rom[4/4] = 0x0000000; //SSP

	for(int i=0x400/4;i<0x8000/4;i++)
		m_bios_rom[i] = 0x000b0009; // RTS + NOP
}

GAME( 1995, casloopy,  0,   casloopy,  casloopy, casloopy_state,  casloopy, ROT0, "Casio", "Loopy", GAME_NOT_WORKING | GAME_NO_SOUND )
