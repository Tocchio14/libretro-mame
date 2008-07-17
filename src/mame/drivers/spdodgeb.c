/***************************************************************************

Super Dodgeball / Nekketsu Koukou Dodgeball Bu

briver by Paul Hampson and Nicola Salmoria

TODO:
- sprite lag (the real game has quite a bit of lag too)
- double-tap tolerance (find a way to dump + emulate MCU?)

Notes:
- there's probably a 63701 on the board, used for protection. It is checked
  on startup and then just used to read the input ports. It doesn't return
  the ports verbatim, it adds further processing, setting flags when the
  player double-taps in one direction to run.(updated to edge-triggering)

- video timing is probably similar to Double Dragon and other Technos games
  of that era.  The rowscroll of the title bar is done with raster IRQs, I've
  removed the 'scroll value buffer' hack that the driver was using before and
  used partial updates instead. (DH, 29 Sept 07)

***************************************************************************/

#include "driver.h"
#include "deprecat.h"
#include "cpu/m6809/m6809.h"
#include "sound/3812intf.h"
#include "sound/msm5205.h"

extern UINT8 *spdodgeb_videoram;

PALETTE_INIT( spdodgeb );
VIDEO_START( spdodgeb );
VIDEO_UPDATE( spdodgeb );
INTERRUPT_GEN( spdodgeb_interrupt );
WRITE8_HANDLER( spdodgeb_scrollx_lo_w );
WRITE8_HANDLER( spdodgeb_ctrl_w );
WRITE8_HANDLER( spdodgeb_videoram_w );


/* private globals */
static int toggle=0;//, soundcode = 0;
static int adpcm_pos[2],adpcm_end[2],adpcm_idle[2];
/* end of private globals */


static WRITE8_HANDLER( sound_command_w )
{
	soundlatch_w(machine,offset,data);
	cpunum_set_input_line(machine, 1,M6809_IRQ_LINE,HOLD_LINE);
}

static WRITE8_HANDLER( spd_adpcm_w )
{
	int chip = offset & 1;

	switch (offset/2)
	{
		case 3:
			adpcm_idle[chip] = 1;
			MSM5205_reset_w(chip,1);
			break;

		case 2:
			adpcm_pos[chip] = (data & 0x7f) * 0x200;
			break;

		case 1:
			adpcm_end[chip] = (data & 0x7f) * 0x200;
			break;

		case 0:
			adpcm_idle[chip] = 0;
			MSM5205_reset_w(chip,0);
			break;
	}
}

static void spd_adpcm_int(running_machine *machine, int chip)
{
	static int adpcm_data[2] = { -1, -1 };

	if (adpcm_pos[chip] >= adpcm_end[chip] || adpcm_pos[chip] >= 0x10000)
	{
		adpcm_idle[chip] = 1;
		MSM5205_reset_w(chip,1);
	}
	else if (adpcm_data[chip] != -1)
	{
		MSM5205_data_w(chip,adpcm_data[chip] & 0x0f);
		adpcm_data[chip] = -1;
	}
	else
	{
		UINT8 *ROM = memory_region(machine, REGION_SOUND1) + 0x10000 * chip;

		adpcm_data[chip] = ROM[adpcm_pos[chip]++];
		MSM5205_data_w(chip,adpcm_data[chip] >> 4);
	}
}


static int mcu63701_command;
static int inputs[4];

#if 0	// default - more sensitive (state change and timing measured on real board?)
static void mcu63705_update_inputs(running_machine *machine)
{
	static int running[2],jumped[2];
	int buttons[2];
	int p,j;

	/* update running state */
	for (p = 0; p <= 1; p++)
	{
		static int prev[2][2],countup[2][2],countdown[2][2];
		int curr[2][2];

		curr[p][0] = input_port_read(machine, p ? "P2" : "P1") & 0x01;
		curr[p][1] = input_port_read(machine, p ? "P2" : "P1") & 0x02;

		for (j = 0;j <= 1;j++)
		{
			if (curr[p][j] == 0)
			{
				if (prev[p][j] != 0)
					countup[p][j] = 0;
				if (curr[p][j^1])
					countup[p][j] = 100;
				countup[p][j]++;
				running[p] &= ~(1 << j);
			}
			else
			{
				if (prev[p][j] == 0)
				{
					if (countup[p][j] < 10 && countdown[p][j] < 5)
						running[p] |= 1 << j;
					countdown[p][j] = 0;
				}
				countdown[p][j]++;
			}
		}

		prev[p][0] = curr[p][0];
		prev[p][1] = curr[p][1];
	}

	/* update jumping and buttons state */
	for (p = 0; p <= 1; p++)
	{
		static int prev[2];
		int curr[2];

		curr[p] = input_port_read(machine, p ? "P2" : "P1") & 0x30;

		if (jumped[p]) buttons[p] = 0;	/* jump only momentarily flips the buttons */
		else buttons[p] = curr[p];

		if (buttons[p] == 0x30) jumped[p] = 1;
		if (curr[p] == 0x00) jumped[p] = 0;

		prev[p] = curr[p];
	}

	inputs[0] = input_port_read(machine, "P1") & 0xcf;
	inputs[1] = input_port_read(machine, "P2") & 0x0f;
	inputs[2] = running[0] | buttons[0];
	inputs[3] = running[1] | buttons[1];
}
#else	// alternate - less sensitive
static void mcu63705_update_inputs(running_machine *machine)
{
#define DBLTAP_TOLERANCE 5

#define R 0x01
#define L 0x02
#define A 0x10
#define D 0x20

	static UINT8 tapc[4] = {0,0,0,0};	// R1, R2, L1, L2
	static UINT8 last_port[2] = {0,0};
	static UINT8 last_dash[2] = {0,0};
	UINT8 curr_port[2];
	UINT8 curr_dash[2];
	int p;

	for (p=0; p<=1; p++)
	{
		curr_port[p] = input_port_read(machine, p ? "P2" : "P1");
		curr_dash[p] = 0;

		if (curr_port[p] & R)
		{
			if (!(last_port[p] & R))
			{
				if (tapc[p]) curr_dash[p] |= R; else tapc[p] = DBLTAP_TOLERANCE;
			}
			else if (last_dash[p] & R) curr_dash[p] |= R;
		}
		else if (curr_port[p] & L)
		{
			if (!(last_port[p] & L))
			{
				if (tapc[p+2]) curr_dash[p] |= L; else tapc[p+2] = DBLTAP_TOLERANCE;
			}
			else if (last_dash[p] & L) curr_dash[p] |= L;
		}

		if (curr_port[p] & A && !(last_port[p] & A)) curr_dash[p] |= A;
		if (curr_port[p] & D && !(last_port[p] & D)) curr_dash[p] |= D;

		last_port[p] = curr_port[p];
		last_dash[p] = curr_dash[p];

		if (tapc[p  ]) tapc[p  ]--;
		if (tapc[p+2]) tapc[p+2]--;
	}

	inputs[0] = curr_port[0] & 0xcf;
	inputs[1] = curr_port[1] & 0x0f;
	inputs[2] = curr_dash[0];
	inputs[3] = curr_dash[1];

#undef DBLTAP_TOLERANCE
#undef R
#undef L
#undef A
#undef D
}
#endif

static READ8_HANDLER( mcu63701_r )
{
//  logerror("CPU #0 PC %04x: read from port %02x of 63701 data address 3801\n",activecpu_get_pc(),offset);

	if (mcu63701_command == 0) return 0x6a;
	else switch (offset)
	{
		default:
		case 0: return inputs[0];
		case 1: return inputs[1];
		case 2: return inputs[2];
		case 3: return inputs[3];
		case 4: return input_port_read(machine, "IN1");
	}
}

static WRITE8_HANDLER( mcu63701_w )
{
//  logerror("CPU #0 PC %04x: write %02x to 63701 control address 3800\n",activecpu_get_pc(),data);
	mcu63701_command = data;
	mcu63705_update_inputs(machine);
}


static READ8_HANDLER( port_0_r )
{
	int port = input_port_read(machine, "IN0");

	toggle^=0x02;	/* mcu63701_busy flag */

	return (port | toggle);
}



static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(SMH_RAM)
	AM_RANGE(0x2000, 0x2fff) AM_READ(SMH_RAM)
	AM_RANGE(0x3000, 0x3000) AM_READ(port_0_r)
	AM_RANGE(0x3001, 0x3001) AM_READ_PORT("DSW")	/* DIPs */
	AM_RANGE(0x3801, 0x3805) AM_READ(mcu63701_r)
	AM_RANGE(0x4000, 0x7fff) AM_READ(SMH_BANK1)
	AM_RANGE(0x8000, 0xffff) AM_READ(SMH_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(SMH_RAM)
	AM_RANGE(0x1000, 0x10ff) AM_WRITE(SMH_RAM) AM_BASE(&spriteram) AM_SIZE(&spriteram_size)
	AM_RANGE(0x2000, 0x2fff) AM_WRITE(spdodgeb_videoram_w) AM_BASE(&spdodgeb_videoram)
//  AM_RANGE(0x3000, 0x3000) AM_WRITE(SMH_RAM)
//  AM_RANGE(0x3001, 0x3001) AM_WRITE(SMH_RAM)
	AM_RANGE(0x3002, 0x3002) AM_WRITE(sound_command_w)
//  AM_RANGE(0x3003, 0x3003) AM_WRITE(SMH_RAM)
	AM_RANGE(0x3004, 0x3004) AM_WRITE(spdodgeb_scrollx_lo_w)
//  AM_RANGE(0x3005, 0x3005) AM_WRITE(SMH_RAM)			/* mcu63701_output_w */
	AM_RANGE(0x3006, 0x3006) AM_WRITE(spdodgeb_ctrl_w)	/* scroll hi, flip screen, bank switch, palette select */
	AM_RANGE(0x3800, 0x3800) AM_WRITE(mcu63701_w)
	AM_RANGE(0x4000, 0xffff) AM_WRITE(SMH_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_READ(SMH_RAM)
	AM_RANGE(0x1000, 0x1000) AM_READ(soundlatch_r)
	AM_RANGE(0x8000, 0xffff) AM_READ(SMH_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_WRITE(SMH_RAM)
	AM_RANGE(0x2800, 0x2800) AM_WRITE(YM3812_control_port_0_w)
	AM_RANGE(0x2801, 0x2801) AM_WRITE(YM3812_write_port_0_w)
	AM_RANGE(0x3800, 0x3807) AM_WRITE(spd_adpcm_w)
	AM_RANGE(0x8000, 0xffff) AM_WRITE(SMH_ROM)
ADDRESS_MAP_END


static INPUT_PORTS_START( spdodgeb )
	PORT_START_TAG("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_SPECIAL )	/* mcu63701_busy flag */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ))
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ))
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ))
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Hard ))

	PORT_START_TAG("DSW")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START_TAG("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START2 )

	PORT_START_TAG("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START_TAG("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x80, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 2, 4, 6 },
	{ 1, 0, 64+1, 64+0, 128+1, 128+0, 192+1, 192+0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	32*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0,4 },
	{ 3, 2, 1, 0, 16*8+3, 16*8+2, 16*8+1, 16*8+0,
		  32*8+3, 32*8+2, 32*8+1, 32*8+0, 48*8+3, 48*8+2, 48*8+1, 48*8+0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		  8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	64*8
};

static GFXDECODE_START( spdodgeb )
	GFXDECODE_ENTRY( REGION_GFX1, 0, charlayout,   0x000, 32 )	/* colors 0x000-0x1ff */
	GFXDECODE_ENTRY( REGION_GFX2, 0, spritelayout, 0x200, 32 )	/* colors 0x200-0x3ff */
GFXDECODE_END


static void irq_handler(running_machine *machine, int irq)
{
	cpunum_set_input_line(machine,1,M6809_FIRQ_LINE,irq ? ASSERT_LINE : CLEAR_LINE);
}

static const struct YM3812interface ym3812_interface =
{
	irq_handler
};

static const struct MSM5205interface msm5205_interface =
{
	spd_adpcm_int,	/* interrupt function */
	MSM5205_S48_4B	/* 8kHz? */
};



static MACHINE_DRIVER_START( spdodgeb )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502,12000000/6)	/* 2MHz ? */
	MDRV_CPU_PROGRAM_MAP(readmem,writemem)
	MDRV_CPU_VBLANK_INT_HACK(spdodgeb_interrupt,33)	/* 1 IRQ every 8 visible scanlines, plus NMI for vblank */

	MDRV_CPU_ADD(M6809,12000000/6)
	/* audio CPU */	/* 2MHz ? */
	MDRV_CPU_PROGRAM_MAP(sound_readmem,sound_writemem)

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(500) /* not accurate */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(1*8, 31*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(spdodgeb)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_PALETTE_INIT(spdodgeb)
	MDRV_VIDEO_START(spdodgeb)
	MDRV_VIDEO_UPDATE(spdodgeb)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM3812, 3000000)
	MDRV_SOUND_CONFIG(ym3812_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.0)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.0)

	MDRV_SOUND_ADD(MSM5205, 384000)
	MDRV_SOUND_CONFIG(msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)

	MDRV_SOUND_ADD(MSM5205, 384000)
	MDRV_SOUND_CONFIG(msm5205_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.50)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.50)
MACHINE_DRIVER_END



ROM_START( spdodgeb )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "22a-04.139",	  0x10000, 0x08000, CRC(66071fda) SHA1(4a239295900e6234a2a693321ca821671747a58e) )  /* Two banks */
	ROM_CONTINUE(             0x08000, 0x08000 )		 /* Static code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* audio cpu */
	ROM_LOAD( "22j5-0.33",    0x08000, 0x08000, CRC(c31e264e) SHA1(0828a2094122e3934b784ec9ad7c2b89d91a83bb) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* I/O mcu */
	ROM_LOAD( "63701.bin",    0xc000, 0x4000, NO_DUMP )	/* missing */

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE ) /* text */
	ROM_LOAD( "22a-4.121",    0x00000, 0x20000, CRC(acc26051) SHA1(445224238cce420990894824d95447e3f63a9ef0) )
	ROM_LOAD( "22a-3.107",    0x20000, 0x20000, CRC(10bb800d) SHA1(265a3d67669034d17713b505ef55cd1c90f8d205) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "22a-1.2",      0x00000, 0x20000, CRC(3bd1c3ec) SHA1(40f61552ea6f7a81915fe3e13f75dc1dc69da33e) )
	ROM_LOAD( "22a-2.35",     0x20000, 0x20000, CRC(409e1be1) SHA1(35a77fc8fe6fc212734e2f452dbde9b8cf696f61) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* adpcm samples */
	ROM_LOAD( "22j6-0.83",    0x00000, 0x10000, CRC(744a26e3) SHA1(519f22f1e5cc417cb8f9ced97e959d23c711283b) )
	ROM_LOAD( "22j7-0.82",    0x10000, 0x10000, CRC(2fa1de21) SHA1(e8c7af6057b64ecadd3473b82abd8e9f873082fd) )

	ROM_REGION( 0x0800, REGION_PROMS, 0 )	/* color PROMs */
	ROM_LOAD( "mb7132e.158",  0x0000, 0x0400, CRC(7e623722) SHA1(e1fe60533237bd0aba5c8de9775df620ed5227c0) )
	ROM_LOAD( "mb7122e.159",  0x0400, 0x0400, CRC(69706e8d) SHA1(778ee88ff566aa38c80e0e61bb3fe8458f0e9450) )
ROM_END

/*

Nekketsu Koukou Dodgeball Bu
(c)1987 Technos Japan

TA-0022-P1-04
M6100293A (PCB manufactured by Taito)

CPU: 6502 (Labeled TJC-706002)
Sound: 68A09, YM3812, M5205x2
OSC: 12.000MHz

ROMs:
22J4-0.139 - Main program
22J5-0.33  - Sound program
22JA-0.162 - HD63701Y0P (no dump)

TJ22J4-0.121 - Text
TJ22J3-0.107 /

TJ22J1-0.2  - Graphics
TJ22J2-0.35 /

22J6-0.83 - ADPCM Samples
22J7-0.82 /

22J8-0.158 (7132)
22J9-0.159 (7122)

*/

ROM_START( nkdodge )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "22j4-0.139",	  0x10000, 0x08000, CRC(aa674fd8) SHA1(4e8d3e07b54d23b221cb39cf10389bc7a56c4021) )  /* Two banks */
	ROM_CONTINUE(             0x08000, 0x08000 )		 /* Static code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* audio cpu */
	ROM_LOAD( "22j5-0.33",    0x08000, 0x08000, CRC(c31e264e) SHA1(0828a2094122e3934b784ec9ad7c2b89d91a83bb) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* I/O mcu */
	ROM_LOAD( "63701.bin",    0xc000, 0x4000, NO_DUMP )	/* missing */

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE ) /* text */
	ROM_LOAD( "tj22j4-0.121",    0x00000, 0x20000, CRC(d2922b3f) SHA1(30ad37f8355c732b545017c2fc56879256b650be) )
	ROM_LOAD( "tj22j3-0.107",    0x20000, 0x20000, CRC(79cd1315) SHA1(2d7a877e59f704b10b5f609e60fa565c68f5fdb0) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "tj22j1-0.2",      0x00000, 0x20000, CRC(9ed27a8d) SHA1(d80d275bbe91f3e1bd0495a2d7a3be0280a7cda1) )
	ROM_LOAD( "tj22j2-0.35",     0x20000, 0x20000, CRC(768934f9) SHA1(922f3154dcfb29c2e5c1bebc53247136160f1229) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* adpcm samples */
	ROM_LOAD( "22j6-0.83",    0x00000, 0x10000, CRC(744a26e3) SHA1(519f22f1e5cc417cb8f9ced97e959d23c711283b) )
	ROM_LOAD( "22j7-0.82",    0x10000, 0x10000, CRC(2fa1de21) SHA1(e8c7af6057b64ecadd3473b82abd8e9f873082fd) )

	ROM_REGION( 0x0800, REGION_PROMS, 0 )	/* color PROMs */
	ROM_LOAD( "22j8-0.158",  0x0000, 0x0400, CRC(c368440f) SHA1(39762d102a42211f24db16bc721b01230df1c4d6) )
	ROM_LOAD( "22j9-0.159",  0x0400, 0x0400, CRC(6059f401) SHA1(280b1bda3a55f2d8c2fd4552c4dcec7100f0170f) )
ROM_END

/* the bootleg just seems to have the gfx roms in a different format, program is identical */

ROM_START( nkdodgeb )
	ROM_REGION( 0x18000, REGION_CPU1, 0 )
	ROM_LOAD( "12.bin",	      0x10000, 0x08000, CRC(aa674fd8) SHA1(4e8d3e07b54d23b221cb39cf10389bc7a56c4021) )  /* Two banks */
	ROM_CONTINUE(             0x08000, 0x08000 )		 /* Static code */

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* audio cpu */
	ROM_LOAD( "22j5-0.33",    0x08000, 0x08000, CRC(c31e264e) SHA1(0828a2094122e3934b784ec9ad7c2b89d91a83bb) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* I/O mcu */
	ROM_LOAD( "63701.bin",    0xc000, 0x4000, NO_DUMP )	/* missing */

	ROM_REGION( 0x40000, REGION_GFX1, ROMREGION_DISPOSE ) /* text */
	ROM_LOAD( "10.bin",       0x00000, 0x10000, CRC(442326fd) SHA1(e0e9e1dfdca3edd6e2522f55c191b40b81b8eaff) )
	ROM_LOAD( "11.bin",       0x10000, 0x10000, CRC(2140b070) SHA1(7a9d89eb6130b1dd21236fefaeb09a29c7f0d208) )
	ROM_LOAD( "9.bin",        0x20000, 0x10000, CRC(18660ac1) SHA1(be6a47eea9649d7b9ff8b30a4de643522c9869e6) )
	ROM_LOAD( "8.bin",        0x30000, 0x10000, CRC(5caae3c9) SHA1(f81a1c4ce2117d41e81542d417ff3573ea0f5313) )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "2.bin",        0x00000, 0x10000, CRC(1271583e) SHA1(98a597f2be1abdac6c4de811cfa8a53549bc6904) )
	ROM_LOAD( "1.bin",        0x10000, 0x10000, CRC(5ae6cccf) SHA1(6bd385d6559b54c681d05eed2e91bfc2aa3e6844) )
	ROM_LOAD( "4.bin",        0x20000, 0x10000, CRC(f5022822) SHA1(fa67b1f70da80365f14776b712df6f656e603fb0) )
	ROM_LOAD( "3.bin",        0x30000, 0x10000, CRC(05a71179) SHA1(7e5ed81b37ac458d7a40e89f83f1efb742e797a8) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* adpcm samples */
	ROM_LOAD( "22j6-0.83",    0x00000, 0x10000, CRC(744a26e3) SHA1(519f22f1e5cc417cb8f9ced97e959d23c711283b) )
	ROM_LOAD( "22j7-0.82",    0x10000, 0x10000, CRC(2fa1de21) SHA1(e8c7af6057b64ecadd3473b82abd8e9f873082fd) )

	ROM_REGION( 0x0800, REGION_PROMS, 0 )	/* color PROMs */
	ROM_LOAD( "27s191.bin",  0x0000, 0x0800, CRC(317e42ea) SHA1(59caacc02fb7fb11604bd177f790fd68830ca7c1) )
	ROM_LOAD( "82s137.bin",  0x0400, 0x0400, CRC(6059f401) SHA1(280b1bda3a55f2d8c2fd4552c4dcec7100f0170f) )
ROM_END



GAME( 1987, spdodgeb, 0,        spdodgeb, spdodgeb, 0, ROT0, "Technos", "Super Dodge Ball (US)", 0 )
GAME( 1987, nkdodge,  spdodgeb, spdodgeb, spdodgeb, 0, ROT0, "Technos", "Nekketsu Koukou Dodgeball Bu (Japan)", 0 )
GAME( 1987, nkdodgeb, spdodgeb, spdodgeb, spdodgeb, 0, ROT0, "Technos", "Nekketsu Koukou Dodgeball Bu (Japan, bootleg)", 0 )
