/*

  Merit trivia games

  driver by Pierpaolo Prazzoli

  Games known to be needed:
  - Some other missing categories romsets and new / old revision

  TODO:
  - finish video emulation (colors and flipped tiles?)

Notes: it's important that "user1" is 0xa0000 bytes with empty space filled
       with 0xff, because the built-in roms test checks how many question roms
       the games has and the type of each one.
       The  type is stored in one byte in an offset which change for every game,
       using it in a form of protection.

       Rom type byte legend:
       0 -> 0x02000 bytes rom
       1 -> 0x04000 bytes rom
       2 -> 0x08000 bytes rom
       3 -> 0x10000 bytes rom

       ---------------------------------------------------------------------------

       Trivia Whiz ? Horizontal and Vertical can accept 10 question roms, with
       4 categories, but they only use 3 categories.

       ---------------------------------------------------------------------------
*/

#include "driver.h"
#include "machine/8255ppi.h"
#include "sound/ay8910.h"

static tilemap *bg_tilemap;
static UINT8 *phrcraze_attr;

static int extra_video_bank_bit;

static int question_address = 0;
static int decryption_key;

static READ8_HANDLER( questions_r )
{
	UINT8 *questions = memory_region(machine, "user1");
	int address;

	switch(question_address >> 16)
	{
		case 0x30: address = 0x00000;
			break;

		case 0x31: address = 0x10000;
			break;

		case 0x32: address = 0x20000;
			break;

		case 0x33: address = 0x30000;
			break;

		case 0x34: address = 0x40000;
			break;

		case 0x35: address = 0x50000;
			break;

		case 0x36: address = 0x60000;
			break;

		case 0x37: address = 0x70000;
			break;

		case 0x28: address = 0x80000;
			break;

		case 0x18: address = 0x90000;
			break;

// not used? only 10 roms are tested in service mode
// b0 b1 b2 b3 b4 b5 b6 b7 a8 98
//      case 0xb0: address = 0xa0000;
//          break;

		default: logerror("read unknown question rom: %02X\n",question_address >> 16);
			return 0xff;
	}

	address |= question_address & 0xffff;

	return questions[address];
}

static WRITE8_HANDLER( low_offset_w )
{
	offset = (offset & 0xf0) | ((offset - decryption_key) & 0x0f);
	offset = BITSWAP8(offset,7,6,5,4,0,1,2,3);
	question_address = (question_address & 0xffff00) | offset;
}

static WRITE8_HANDLER( med_offset_w )
{
	offset = (offset & 0xf0) | ((offset - decryption_key) & 0x0f);
	offset = BITSWAP8(offset,7,6,5,4,0,1,2,3);
	question_address = (question_address & 0xff00ff) | (offset << 8);
}

static WRITE8_HANDLER( high_offset_w )
{
	offset = BITSWAP8(offset,7,6,5,4,0,1,2,3);
	question_address = (question_address & 0x00ffff) | (offset << 16);
}

static WRITE8_HANDLER( phrcraze_attr_w )
{
	phrcraze_attr[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap,offset);
}

static WRITE8_HANDLER( phrcraze_bg_w )
{
	videoram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap,offset);
}

// I think this looks more like a colortable than a palette
static WRITE8_HANDLER( palette_w )
{
	int i;

	paletteram[offset] = data & 0x0f;
	for (i = 0; i < 0x100; i++)
	{
//      int bit = (i & 0x300)>>8;
		offset = ((i & 0x7f)<<3) | ((i & 0x80) >> 5);

		palette_set_color_rgb(machine,i,
				pal4bit(BIT(paletteram[offset],0)*0x1 + BIT(paletteram[offset+1],0)*0x2 + BIT(paletteram[offset+2],0)*0x4 + BIT(paletteram[offset+3],0)*0x8),
				pal4bit(BIT(paletteram[offset],1)*0x1 + BIT(paletteram[offset+1],1)*0x2 + BIT(paletteram[offset+2],1)*0x4 + BIT(paletteram[offset+3],1)*0x8),
				pal4bit(BIT(paletteram[offset],2)*0x1 + BIT(paletteram[offset+1],2)*0x2 + BIT(paletteram[offset+2],2)*0x4 + BIT(paletteram[offset+3],2)*0x8));
	}
}

static WRITE8_HANDLER( led1_w )
{
	/* 5 button lamps player 1 */
	set_led_status(0,~data & 0x01);
	set_led_status(1,~data & 0x02);
	set_led_status(2,~data & 0x04);
	set_led_status(3,~data & 0x08);
	set_led_status(4,~data & 0x10);
}

static WRITE8_HANDLER( led2_w )
{
	/* 5 button lamps player 2 */
	set_led_status(5,~data & 0x01);
	set_led_status(6,~data & 0x02);
	set_led_status(7,~data & 0x04);
	set_led_status(8,~data & 0x08);
	set_led_status(9,~data & 0x10);

	/* coin counter */
	coin_counter_w(0,0x80-(data & 0x80));
}

static WRITE8_HANDLER( misc_w )
{
	flip_screen_set(~data & 0x10);
	extra_video_bank_bit = (data & 2) << 8;

	// other bits unknown
}

static ADDRESS_MAP_START( pitboss_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x67ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xa000, 0xa000) AM_READ(input_port_1_r)
	AM_RANGE(0xa001, 0xa001) AM_READ(input_port_0_r)
	AM_RANGE(0xa002, 0xa002) AM_NOP //dips ?
//  AM_RANGE(0xc000, 0xc002) AM_NOP
	AM_RANGE(0xe000, 0xe001) AM_WRITENOP // 6845 crt
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(phrcraze_attr_w) AM_BASE(&phrcraze_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(phrcraze_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(palette_w) AM_BASE(&paletteram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( bigappg_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_ROM
	AM_RANGE(0xa000, 0xafff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xc004, 0xc007) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)
	AM_RANGE(0xc008, 0xc00b) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)
	AM_RANGE(0xe000, 0xe001) AM_WRITENOP // 6845 crt
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(phrcraze_attr_w) AM_BASE(&phrcraze_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(phrcraze_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(palette_w) AM_BASE(&paletteram)
ADDRESS_MAP_END


static ADDRESS_MAP_START( trvwhiz_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x4c00, 0x4cff) AM_READWRITE(questions_r, high_offset_w)
	AM_RANGE(0x5400, 0x54ff) AM_WRITE(low_offset_w)
	AM_RANGE(0x5800, 0x58ff) AM_WRITE(med_offset_w)
	AM_RANGE(0x6000, 0x67ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xa000, 0xa003) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)
	AM_RANGE(0xc000, 0xc003) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)
	AM_RANGE(0xe000, 0xe001) AM_WRITENOP // 6845 crt
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(phrcraze_attr_w) AM_BASE(&phrcraze_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(phrcraze_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(palette_w) AM_BASE(&paletteram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( trvwhiz_io_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x8000, 0x8000) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x8100, 0x8100) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( phrcraze_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xa000, 0xbfff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xc008, 0xc00b) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)
	AM_RANGE(0xc00c, 0xc00f) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)
	AM_RANGE(0xce00, 0xceff) AM_READWRITE(questions_r, high_offset_w)
	AM_RANGE(0xd600, 0xd6ff) AM_WRITE(low_offset_w)
	AM_RANGE(0xda00, 0xdaff) AM_WRITE(med_offset_w)
	AM_RANGE(0xe000, 0xe001) AM_WRITENOP // 6845 crt
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(phrcraze_attr_w) AM_BASE(&phrcraze_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(phrcraze_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(palette_w) AM_BASE(&paletteram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( phrcraze_io_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0xc004, 0xc004) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0xc104, 0xc104) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( tictac_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x9fff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xc004, 0xc007) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)
	AM_RANGE(0xc008, 0xc00b) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)
	AM_RANGE(0xce00, 0xceff) AM_READWRITE(questions_r, high_offset_w)
	AM_RANGE(0xd600, 0xd6ff) AM_WRITE(low_offset_w)
	AM_RANGE(0xda00, 0xdaff) AM_WRITE(med_offset_w)
	AM_RANGE(0xe000, 0xe001) AM_WRITENOP // 6845 crt
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(phrcraze_attr_w) AM_BASE(&phrcraze_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(phrcraze_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(palette_w) AM_BASE(&paletteram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( tictac_io_map, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0xc00c, 0xc00c) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0xc10c, 0xc10c) AM_WRITE(AY8910_write_port_0_w)
ADDRESS_MAP_END


static ADDRESS_MAP_START( trvwhziv_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xa000, 0xbfff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0xc004, 0xc007) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)
	AM_RANGE(0xc008, 0xc00b) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)
	AM_RANGE(0xce00, 0xceff) AM_READWRITE(questions_r, high_offset_w)
	AM_RANGE(0xd600, 0xd6ff) AM_WRITE(low_offset_w)
	AM_RANGE(0xda00, 0xdaff) AM_WRITE(med_offset_w)
	AM_RANGE(0xe000, 0xe001) AM_WRITENOP // 6845 crt
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(phrcraze_attr_w) AM_BASE(&phrcraze_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(phrcraze_bg_w) AM_BASE(&videoram)
	AM_RANGE(0xf800, 0xfbff) AM_WRITE(palette_w) AM_BASE(&paletteram)
ADDRESS_MAP_END



// in service mode:
// keep service test button pressed to clear the coin counter.
// keep it pressed for 10 seconds to clear all the memory.
// to enter hidden test mode enable "Enable Test Mode", enable "Reset High Scores"
static INPUT_PORTS_START( phrcraze )
	PORT_START("IN0")	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, "Enable Test Mode" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, "Reset High Scores" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPSETTING(    0x02, "6" )
	PORT_DIPNAME( 0x04, 0x04, "Topic \"8\"" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0xc0, "Upright 1 Player" )
	PORT_DIPSETTING(    0x00, "Upright 2 Players" )
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )
//  PORT_DIPSETTING(    0x40, "Upright 1 Player" )
INPUT_PORTS_END

// in service mode:
// keep service test button pressed to clear the coin counter.
// keep it pressed for 10 seconds to clear all the memory.
// to enter hidden test mode enable "Enable Test Mode", enable "Reset High Scores"
static INPUT_PORTS_START( phrcrazs )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, "Enable Test Mode" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, "Reset High Scores" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x04, 0x04, "XXX-Rated Sex Topic" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x08, "Bonus Phraze" )
	PORT_DIPSETTING(    0x18, DEF_STR( None ) )
	PORT_DIPSETTING(    0x10, "800K" )
	PORT_DIPSETTING(    0x08, "1M" )
	PORT_DIPSETTING(    0x00, "1.5M" )
	PORT_DIPNAME( 0x20, 0x20, "Random Sex Category" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0xc0, "Upright 1 Player" )
	PORT_DIPSETTING(    0x00, "Upright 2 Players" )
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )
//  PORT_DIPSETTING(    0x40, "Upright 1 Player" )
INPUT_PORTS_END

// keep service test button pressed to clear the coin counter.
// To enter hidden test-mode in service mode:
// enable "Reset High Scores" then press "Service Mode"
static INPUT_PORTS_START( tictac )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, "Reset High Scores" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Lightning Round 1 Credit" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0xc0, "Upright 1 Player" )
	PORT_DIPSETTING(    0x00, "Upright 2 Players" )
	PORT_DIPSETTING(    0x80, DEF_STR( Cocktail ) )
//  PORT_DIPSETTING(    0x40, "Upright 1 Player" )
INPUT_PORTS_END

static INPUT_PORTS_START( trivia )
	PORT_START("IN0")	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x08, 0x08, "Reset High Scores" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "On 0 Points" )
	PORT_DIPSETTING(    0x04, "Continue" )
	PORT_DIPSETTING(    0x00, "Game Over" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

// in service mode:
// keep service test button pressed to clear the coin counter.
// keep it pressed for 10 seconds to clear all the memory.
// to enter hidden test mode enable "Enable Test Mode", enable "Reset High Scores"
static INPUT_PORTS_START( trvwhziv )
	PORT_INCLUDE( trivia )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )	/* no coinage DSW */
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( pitboss )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_DIPNAME( 0x02, 0x02, "0-1" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "0-2" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "0-3" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "0-4" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "0-5" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "0-6" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "0-7" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) //z
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON6 ) //x
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON7 ) //c
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON8 ) //v
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON9 ) //b

	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1 )

	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( bigappg )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, "2" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")
	PORT_DIPNAME( 0x01, 0x01, "3" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN3")
	PORT_DIPNAME( 0x01, 0x01, "4" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static TILE_GET_INFO( get_tile_info_bg )
{
	int attr = phrcraze_attr[tile_index];
	int region = (attr & 0x40) >> 6;
	int code = videoram[tile_index] | ((attr & 0x80) << 1);

	if(region == 0)
		code |= extra_video_bank_bit;

	SET_TILE_INFO(region, code, 0, 0);
}


static VIDEO_START( merit )
{
	bg_tilemap = tilemap_create(get_tile_info_bg,tilemap_scan_rows,8,8,64,32);
}

static VIDEO_UPDATE( merit )
{
	tilemap_draw(bitmap,cliprect,bg_tilemap,0,0);
	return 0;
}

static const gfx_layout tiles8x8x3_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8
};

static const gfx_layout tiles8x8x1_layout =
{
	8,8,
	RGN_FRAC(1,1),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8
};

static GFXDECODE_START( merit )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8x3_layout, 0,  32 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles8x8x1_layout, 0, 128 )
	GFXDECODE_ENTRY( "gfx1", 8, tiles8x8x3_layout, 0,  32 ) // flipped tiles
	GFXDECODE_ENTRY( "gfx2", 8, tiles8x8x1_layout, 0, 128 ) // flipped tiles
GFXDECODE_END

static const ppi8255_interface ppi8255_intf[2] =
{
	{
		input_port_0_r,				/* Port A read */
		input_port_1_r,				/* Port B read */
		input_port_2_r,				/* Port C read */
		NULL,						/* Port A write */
		NULL,						/* Port B write */
		NULL						/* Port C write */
	},
	{
		input_port_3_r,				/* Port A read */
		NULL,						/* Port B read */
		NULL,						/* Port C read */
		NULL,						/* Port A write */
		led1_w,						/* Port B write */
		misc_w						/* Port C write */
	}
};

static const struct AY8910interface merit_ay8912_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	NULL, NULL,
	led2_w, NULL
};


static MACHINE_DRIVER_START( pitboss )
	MDRV_CPU_ADD("main",Z80,2500000)		 /* ?? */
	MDRV_CPU_PROGRAM_MAP(pitboss_map,0)
	MDRV_CPU_IO_MAP(trvwhiz_io_map,0)
	MDRV_CPU_VBLANK_INT("main", irq0_line_hold)

	MDRV_NVRAM_HANDLER(generic_0fill)

	MDRV_DEVICE_ADD( "ppi8255_0", PPI8255 )
	MDRV_DEVICE_CONFIG( ppi8255_intf[0] )

	MDRV_DEVICE_ADD( "ppi8255_1", PPI8255 )
	MDRV_DEVICE_CONFIG( ppi8255_intf[1] )

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0, 64*8-1, 1*8, 32*8-1)

	MDRV_GFXDECODE(merit)
	MDRV_PALETTE_LENGTH(0x100)

	MDRV_VIDEO_START(merit)
	MDRV_VIDEO_UPDATE(merit)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay", AY8910, 2500000)
	MDRV_SOUND_CONFIG(merit_ay8912_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.33)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( bigappg )
	MDRV_IMPORT_FROM(pitboss)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(bigappg_map,0)
	MDRV_CPU_IO_MAP(tictac_io_map,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( tictac )
	MDRV_IMPORT_FROM(pitboss)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(tictac_map,0)
	MDRV_CPU_IO_MAP(tictac_io_map,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( trvwhiz )
	MDRV_IMPORT_FROM(pitboss)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(trvwhiz_map,0)
	MDRV_CPU_IO_MAP(trvwhiz_io_map,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( phrcraze )
	MDRV_IMPORT_FROM(pitboss)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(phrcraze_map,0)
	MDRV_CPU_IO_MAP(phrcraze_io_map,0)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( trvwhziv )
	MDRV_IMPORT_FROM(pitboss)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(trvwhziv_map,0)
	MDRV_CPU_IO_MAP(tictac_io_map,0)
MACHINE_DRIVER_END


ROM_START( pitboss )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5-0c.rom", 0x0000, 0x2000, CRC(d8902656) SHA1(06da829201f6141a6b23afa0e277a3c7a122c26e) )
	ROM_LOAD( "u6-0.rom",  0x2000, 0x2000, CRC(bf903b01) SHA1(1f5f69cfd3eb105bd9bad071016931a79defa16b) )
	ROM_LOAD( "u7-0.rom",  0x4000, 0x2000, CRC(306351b9) SHA1(32cd243aa65571ee7fc72971b6a16beeb4ed9d85) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39.rom",   0x0000, 0x2000, CRC(6662f607) SHA1(6b423f8de011d196700839af0be37effbf87383f) )
	ROM_LOAD( "u38.rom",   0x2000, 0x2000, CRC(a014b44f) SHA1(906d426b1de75f26030c19dcd599b6570909f510) )
	ROM_LOAD( "u37.rom",   0x4000, 0x2000, CRC(cb12e139) SHA1(06fe91281faae5d0c0ae4b3cd8ad103bd3995c38) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40.rom",   0x0000, 0x2000, CRC(52298162) SHA1(79aa6c4ab6bec6450d882615e64f61cfef934153) )
ROM_END

ROM_START( casino5 )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5",           0x0000, 0x2000, CRC(abe240d8) SHA1(296eb3251dd51147d6984a8c08c3be22e5ed8e86) )
	ROM_LOAD( "u6",           0x2000, 0x4000, CRC(4d9f0c57) SHA1(d19b4b4f42d329ea35907d17c15a55b954b07295) )
	ROM_LOAD( "u7",           0x6000, 0x4000, CRC(d3bc510d) SHA1(6222badabf629dd6334591867596f811883aed52) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39",          0x0000, 0x2000, CRC(6662f607) SHA1(6b423f8de011d196700839af0be37effbf87383f) )
	ROM_LOAD( "u38",          0x2000, 0x2000, CRC(a014b44f) SHA1(906d426b1de75f26030c19dcd599b6570909f510) )
	ROM_LOAD( "u37",          0x4000, 0x2000, CRC(cb12e139) SHA1(06fe91281faae5d0c0ae4b3cd8ad103bd3995c38) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40",          0x0000, 0x2000, CRC(b13a3fb1) SHA1(25760aa27c88b8be248a87df724bf8797d179e7a) )
ROM_END

ROM_START( bigappg )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "merit.u5",     0x0000, 0x8000, CRC(47bad6fd) SHA1(87f6c603b52e184f82179869d7b58453cbd34814) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "merit.u39",    0x0000, 0x2000, CRC(506e20b4) SHA1(cd28d4f56f83ed3f7cfa44280975c0a6af474707) )
	ROM_LOAD( "merit.u38",    0x2000, 0x2000, CRC(1924feeb) SHA1(c4656a04e2284a265554ea9774d0201c44864809) )
	ROM_LOAD( "merit.u37",    0x4000, 0x2000, CRC(12a04867) SHA1(8e3a6050d854ccc906ae473a104a16d610e4f1da) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "merit.u40",    0x0000, 0x2000, CRC(ac4983b8) SHA1(a552a15f813c331de67eaae2ed42cc037b26c5bd) )
ROM_END

ROM_START( trvwzh )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "trivia.u5",    0x0000, 0x2000, CRC(731fd5b1) SHA1(1074780321029446da0e6765b9e036b06b067a48) )
	ROM_LOAD( "trivia.u6",    0x2000, 0x2000, CRC(af6886c0) SHA1(48005b921d7ce33ffc0ba160be82053a26382a9d) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "trivia.u39",   0x0000, 0x2000, CRC(f8a5f5fb) SHA1(a511e1a2b5e887ef00dc919e9e664ccec2d36cfa) )
	ROM_LOAD( "trivia.u38",   0x2000, 0x2000, CRC(27621e52) SHA1(a7e88d329e2e774fef9bd8c5cefb4d8f1cfcba4c) )
	ROM_LOAD( "trivia.u37",   0x4000, 0x2000, CRC(f739b5dc) SHA1(fbf469b7f4cab50e06ec2def9344e3b9801a275e) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "trivia.u40",   0x0000, 0x2000, CRC(cea7319f) SHA1(663cd18a4699dfd5ad1d3357094eff247e9b4a47) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "trivia.ent",   0x08000, 0x8000, CRC(ff45d92b) SHA1(10356bc6a04b2c53ecaf76cb0cba3ec70b4ba612) )
	ROM_LOAD( "trivia1.ent",  0x18000, 0x8000, CRC(902e26f7) SHA1(f13b816bfc507fb429fb3f44531de346a82c780d) )
	ROM_LOAD( "trivia.gen",   0x28000, 0x8000, CRC(1d8d353f) SHA1(6bd0cc5c67da81a48737f32bc49cbf235648c4c6) )
	ROM_LOAD( "trivia.02a",   0x3c000, 0x4000, CRC(2000e3c3) SHA1(21737fde3d1a1b22da4590476e4e52ee1bab026f) )
	ROM_LOAD( "trivia.sex",   0x48000, 0x8000, CRC(0be4ef9a) SHA1(c80080f1c853e1043bf7e47bea322540a8ac9195) )
	ROM_LOAD( "trivia.sx2",   0x58000, 0x8000, CRC(32519098) SHA1(d070e02bb10e04964893903599a69a8943f9ac8a) )
ROM_END

ROM_START( trvwzha )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "trivia.u5",    0x0000, 0x2000, CRC(731fd5b1) SHA1(1074780321029446da0e6765b9e036b06b067a48) )
	ROM_LOAD( "trivia.u6",    0x2000, 0x2000, CRC(af6886c0) SHA1(48005b921d7ce33ffc0ba160be82053a26382a9d) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "trivia.u39",   0x0000, 0x2000, CRC(f8a5f5fb) SHA1(a511e1a2b5e887ef00dc919e9e664ccec2d36cfa) )
	ROM_LOAD( "trivia.u38",   0x2000, 0x2000, CRC(27621e52) SHA1(a7e88d329e2e774fef9bd8c5cefb4d8f1cfcba4c) )
	ROM_LOAD( "trivia.u37",   0x4000, 0x2000, CRC(f739b5dc) SHA1(fbf469b7f4cab50e06ec2def9344e3b9801a275e) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "trivia.u40",   0x0000, 0x2000, CRC(cea7319f) SHA1(663cd18a4699dfd5ad1d3357094eff247e9b4a47) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "trivia.spo",   0x08000, 0x8000, CRC(64181d34) SHA1(f84e28fc589b86ca6a596815871ed26602bcc095) )
	ROM_LOAD( "trivia1.spo",  0x18000, 0x8000, CRC(ae111429) SHA1(ff551d7ac7ad367338e908805aeb78c59a747919) )
	ROM_LOAD( "trivia2.spo",  0x28000, 0x8000, CRC(ee9263b3) SHA1(1644ab01f17e3af1e193e509d64dcbb243d3eb80) )
ROM_END

ROM_START( trvwzv )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5.bin",       0x0000, 0x2000, CRC(fd3531ac) SHA1(d11573df65676e704b28cc2d99fb004b48a358a4) )
	ROM_LOAD( "u6.bin",       0x2000, 0x2000, CRC(29e43d0e) SHA1(ad610748fe37436880648078f5d1a305cb147c5d) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39",          0x0000, 0x2000, CRC(b9d9a80e) SHA1(55b6a0d09f8619df93ba936e083835c859a557df) )
	ROM_LOAD( "u38",          0x2000, 0x2000, CRC(8348083e) SHA1(260a4c1ae043e7ceac65a8818c23940d32275879) )
	ROM_LOAD( "u37",          0x4000, 0x2000, CRC(b4d3c9f4) SHA1(dda99549306519c147d275d8c6af672e80a96b67) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40.bin",      0x0000, 0x2000, CRC(1f0ff6e0) SHA1(5a31afde34aeb6f851389d093bb426e5cfdedbf2) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "ent_001_01.bin", 0x08000, 0x8000, CRC(c68ce954) SHA1(bf70fe64f095d5cfcf5347d83651b78c6c8bf05f) )
	ROM_LOAD( "ent_001_02.bin", 0x18000, 0x8000, CRC(aac4ff63) SHA1(d68c4408b4dad976e317a33f2a4eaee39d90dbed) )
	ROM_LOAD( "gen_001_01.bin", 0x28000, 0x8000, CRC(5deb1900) SHA1(b7e9407c37481ef8953e8283d45949d951302e92) )
	ROM_LOAD( "gen_001_02.bin", 0x3c000, 0x4000, CRC(d2b53b6a) SHA1(f75334e47885086e277682daf018818a02ce1026) )
	ROM_LOAD( "sex_001_01.bin", 0x48000, 0x8000, CRC(32519098) SHA1(d070e02bb10e04964893903599a69a8943f9ac8a) )
	ROM_LOAD( "sex_001_02.bin", 0x58000, 0x8000, CRC(0be4ef9a) SHA1(c80080f1c853e1043bf7e47bea322540a8ac9195) )
ROM_END

ROM_START( trvwzva )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5.bin",       0x0000, 0x2000, CRC(fd3531ac) SHA1(d11573df65676e704b28cc2d99fb004b48a358a4) )
	ROM_LOAD( "u6.bin",       0x2000, 0x2000, CRC(29e43d0e) SHA1(ad610748fe37436880648078f5d1a305cb147c5d) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39",          0x0000, 0x2000, CRC(b9d9a80e) SHA1(55b6a0d09f8619df93ba936e083835c859a557df) )
	ROM_LOAD( "u38",          0x2000, 0x2000, CRC(8348083e) SHA1(260a4c1ae043e7ceac65a8818c23940d32275879) )
	ROM_LOAD( "u37",          0x4000, 0x2000, CRC(b4d3c9f4) SHA1(dda99549306519c147d275d8c6af672e80a96b67) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40.bin",      0x0000, 0x2000, CRC(1f0ff6e0) SHA1(5a31afde34aeb6f851389d093bb426e5cfdedbf2) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "spo_001_01.bin", 0x08000, 0x8000, CRC(7b56315d) SHA1(4c8c63b80176bfac9594958a7043627012baada3) )
	ROM_LOAD( "spo_001_02.bin", 0x18000, 0x8000, CRC(148b63ee) SHA1(9f3b222d979f23b313f379cbc06cc00d88d08c56) )
	ROM_LOAD( "spo_001_03.bin", 0x28000, 0x8000, CRC(a6af8e41) SHA1(64f672bfa5fb2c0575103614986e53e238c5984f) )
ROM_END

ROM_START( trvwz2 )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5_0e",        0x0000, 0x2000, CRC(97b8d320) SHA1(573945531113d8aae9418ba1e9a2063052227029) )
	ROM_LOAD( "u6_0e",        0x2000, 0x2000, CRC(2e86288d) SHA1(62c7024d8dfebed9bb05ea91302efe5d18cb7d2a) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39",          0x0000, 0x2000, CRC(b9d9a80e) SHA1(55b6a0d09f8619df93ba936e083835c859a557df) )
	ROM_LOAD( "u38",          0x2000, 0x2000, CRC(8348083e) SHA1(260a4c1ae043e7ceac65a8818c23940d32275879) )
	ROM_LOAD( "u37",          0x4000, 0x2000, CRC(b4d3c9f4) SHA1(dda99549306519c147d275d8c6af672e80a96b67) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40a",         0x0000, 0x2000, CRC(fbfae092) SHA1(b8569819952a5c805f11b6854d64b3ae9c857f97) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "ent_101.01a",  0x08000, 0x8000, CRC(3825ac47) SHA1(d0da047c4d30a26f496b3663cfda77c229279be8) )
	ROM_LOAD( "ent_101.02a",  0x18000, 0x8000, CRC(a0153407) SHA1(e669957a5d4775bfa2c16960a2a909a3505c078b) )
	ROM_LOAD( "ent_101.03a",  0x28000, 0x8000, CRC(755b16ab) SHA1(277ea4110479ecdb2c772299ea04f4918cf7f561) )
	ROM_LOAD( "gen_101.01a",  0x38000, 0x8000, CRC(74d14039) SHA1(54b85581d60fb535d37a051f375e687a933600ea) )
	ROM_LOAD( "gen_101.02a",  0x48000, 0x8000, CRC(b1b930d8) SHA1(57be3ee1c0adcb549088818dc7efda64508b5647) )
	ROM_LOAD( "spo_101.01a",  0x58000, 0x8000, CRC(9dc4ba98) SHA1(4ce2bbbd7436a0ba8140879d5d8614bddbd5a8ec) )
	ROM_LOAD( "spo_101.02a",  0x68000, 0x8000, CRC(9c106ad9) SHA1(1d1a5c91152283e3937a2df17cd57b8fe04072b7) )
	ROM_LOAD( "spo_101.03a",  0x78000, 0x8000, CRC(3d69c3a3) SHA1(9f16d45660f3cb15e44e9fc0d940a7b2b12819e8) )
	ROM_LOAD( "sex-101.01a",  0x88000, 0x8000, CRC(301d65c2) SHA1(48d260077e9c9ed82f6dfa176b1103723dc9e19a) )
	ROM_LOAD( "sex-101.02a",  0x98000, 0x8000, CRC(2596091b) SHA1(7fbb9c2c3f74e12714513928c8cf3769bf29fc8b) )
ROM_END

ROM_START( trvwz3h )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5",           0x0000, 0x2000, CRC(ad4ab519) SHA1(80e99f4e5542115e34074b41bbc69906e01a408f) )
	ROM_LOAD( "u6",           0x2000, 0x2000, CRC(21a44014) SHA1(331f8b4fa3f837de070b68b959c818122aedc68a) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39",          0x0000, 0x2000, CRC(f8a5f5fb) SHA1(a511e1a2b5e887ef00dc919e9e664ccec2d36cfa) )
	ROM_LOAD( "u38",          0x2000, 0x2000, CRC(27621e52) SHA1(a7e88d329e2e774fef9bd8c5cefb4d8f1cfcba4c) )
	ROM_LOAD( "u37",          0x4000, 0x2000, CRC(f739b5dc) SHA1(fbf469b7f4cab50e06ec2def9344e3b9801a275e) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40",          0x0000, 0x2000, CRC(e829473f) SHA1(ba754d9377d955b409970494e1a14dbe1d359ee5) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "7",            0x08000, 0x8000, CRC(974dca96) SHA1(eb4a745c84307a1bbb220659877f97c28cd515ac) )
	ROM_LOAD( "8",            0x18000, 0x8000, CRC(e15ef8d0) SHA1(51c946311ffe507aa9031044bc34e5ae8d3473ab) )
	ROM_LOAD( "9",            0x28000, 0x8000, CRC(503115a1) SHA1(5e6630191465b3d2a590fab08b4f47f7408ecc44) )
	ROM_LOAD( "10",           0x38000, 0x8000, CRC(0e4fe73d) SHA1(9aee22a5837637ec5e360b72e71555942df1d26f) )
	ROM_LOAD( "11",           0x48000, 0x8000, CRC(f56c0935) SHA1(8e16133ad90829bbc0e0f2e9ee9c26e9d0c5057e) )
	ROM_LOAD( "12",           0x58000, 0x8000, CRC(057f6676) SHA1(a93a7a76fc8b8263568a50b00a57f3abe76c9aa3) )
	ROM_LOAD( "13",           0x68000, 0x8000, CRC(1fa46b86) SHA1(16d54d0932fe342399faf303eafa3c0b7ba2e202) )
	ROM_LOAD( "14",           0x78000, 0x8000, CRC(b395cd97) SHA1(a42c7c1687eaba64a725888cd6413568cc90b010) )
	ROM_LOAD( "15",           0x88000, 0x8000, CRC(b064876b) SHA1(588300fb6603f334de41a9685b1fcf8c642b5c16) )
ROM_END

ROM_START( trvwz3v )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5",    0x0000, 0x2000, CRC(97b8d320) SHA1(573945531113d8aae9418ba1e9a2063052227029) )
	ROM_LOAD( "u6",    0x2000, 0x2000, CRC(2e86288d) SHA1(62c7024d8dfebed9bb05ea91302efe5d18cb7d2a) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39",   0x0000, 0x2000, CRC(b9d9a80e) SHA1(55b6a0d09f8619df93ba936e083835c859a557df) )
	ROM_LOAD( "u38",   0x2000, 0x2000, CRC(8348083e) SHA1(260a4c1ae043e7ceac65a8818c23940d32275879) )
	ROM_LOAD( "u37",   0x4000, 0x2000, CRC(b4d3c9f4) SHA1(dda99549306519c147d275d8c6af672e80a96b67) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40",   0x0000, 0x2000, CRC(fbfae092) SHA1(b8569819952a5c805f11b6854d64b3ae9c857f97) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "10",    0x08000, 0x8000, CRC(15d16703) SHA1(9184f63669e9ec93e88276777e1b7f209543c3e3) )
	ROM_LOAD( "8",     0x18000, 0x8000, CRC(647f3394) SHA1(636647ae620fd2f985b82e3516451e3bffd44040) )
	ROM_LOAD( "7",     0x28000, 0x8000, CRC(974dca96) SHA1(eb4a745c84307a1bbb220659877f97c28cd515ac) )
	ROM_LOAD( "6",     0x38000, 0x8000, CRC(e15ef8d0) SHA1(51c946311ffe507aa9031044bc34e5ae8d3473ab) )
	ROM_LOAD( "5",     0x48000, 0x8000, CRC(503115a1) SHA1(5e6630191465b3d2a590fab08b4f47f7408ecc44) )
	ROM_LOAD( "4",     0x58000, 0x8000, CRC(0e4fe73d) SHA1(9aee22a5837637ec5e360b72e71555942df1d26f) )
	ROM_LOAD( "3",     0x68000, 0x8000, CRC(f56c0935) SHA1(8e16133ad90829bbc0e0f2e9ee9c26e9d0c5057e) )
	ROM_LOAD( "2",     0x78000, 0x8000, CRC(057f6676) SHA1(a93a7a76fc8b8263568a50b00a57f3abe76c9aa3) )
	ROM_LOAD( "1",     0x88000, 0x8000, CRC(1fa46b86) SHA1(16d54d0932fe342399faf303eafa3c0b7ba2e202) )
	ROM_LOAD( "0",     0x98000, 0x8000, CRC(b395cd97) SHA1(a42c7c1687eaba64a725888cd6413568cc90b010) )
ROM_END

ROM_START( trvwz4 )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5.bin",       0x0000, 0x8000, CRC(bc23a1ab) SHA1(b9601f316e373c568c5b208de417617094046559) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "u39.bin",      0x0000, 0x2000, CRC(b9d9a80e) SHA1(55b6a0d09f8619df93ba936e083835c859a557df) )
	ROM_LOAD( "u38.bin",      0x2000, 0x2000, CRC(8348083e) SHA1(260a4c1ae043e7ceac65a8818c23940d32275879) )
	ROM_LOAD( "u37.bin",      0x4000, 0x2000, CRC(b4d3c9f4) SHA1(dda99549306519c147d275d8c6af672e80a96b67) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "u40.bin",      0x0000, 0x2000, CRC(fbfae092) SHA1(b8569819952a5c805f11b6854d64b3ae9c857f97) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "ent_4_1.bin",  0x08000, 0x8000, CRC(1b317149) SHA1(94e882e9cc041ac8f292136c1ce2d21340ac5e7f) )
	ROM_LOAD( "ent_4_2.bin",  0x18000, 0x8000, CRC(43d51697) SHA1(7af3f16f9519184ae63d8818bbc52a2ba897f275) )
	ROM_LOAD( "rnp_4_1.bin",  0x28000, 0x8000, CRC(e54fc4bc) SHA1(4607974ed2bf83c475396fc1cbb1e09ad084ace8) )
	ROM_LOAD( "rnp_4_2.bin",  0x38000, 0x8000, CRC(fee2d0b0) SHA1(9c9abec4ce693fc2d3976f3d499213c2ce67c197) )
	ROM_LOAD( "sbt_4_1.bin",  0x48000, 0x8000, CRC(f1560804) SHA1(2ef0d587fbedfc342a12e913fa3c94eb8d67e2c5) )
	ROM_LOAD( "sbt_4_2.bin",  0x58000, 0x8000, CRC(b0d6f6b2) SHA1(b08622d3775d1bb40c3b07ef932f3db4166ee284) )
	ROM_LOAD( "sex_4_1.bin",  0x68000, 0x8000, CRC(976352b0) SHA1(5f89caca410704ba8a90da3167ba18e45fb21d43) )
	ROM_LOAD( "sex_4_2.bin",  0x78000, 0x8000, CRC(5f148bc9) SHA1(2fd2cf819c2f395dcffad59857b3533fe3cce60b) )
	ROM_LOAD( "spo_4_1.bin",  0x88000, 0x8000, CRC(5fe0c6a3) SHA1(17bdb5262ce4edf5f022f075537f6161e1397b46) )
	ROM_LOAD( "spo_4_2.bin",  0x98000, 0x8000, CRC(3f3390e0) SHA1(50bd7b79268438584bb0f497ab0055b4d4864590) )
ROM_END

/*

crt200 rev e-1
1985 merit industries

u39   u37                      z80b
u40   u38
           6845                 u5

      2016                      pb
      2016                      6264

                        ay3-8912
                        8255
                        8255


crt205a

pba pb9 pb8 pb7 pb6 pb5 pb4 pb3 pb2 pb1

*/

ROM_START( tictac )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "merit.u5",     0x00000, 0x8000, CRC(f0dd73f5) SHA1(f2988b84255ce5f7ea6d25150cdbae88b98e1be3) )

	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "merit.u39",    0x00000, 0x2000, CRC(dd79e824) SHA1(d65ee1c758293ddf8a5f4913878a2867ba526e68) )
	ROM_LOAD( "merit.u38",    0x02000, 0x2000, CRC(e1bf0fab) SHA1(291261ea817c42d6e8a19c17a2d3706fed7d78c4) )
	ROM_LOAD( "merit.u37",    0x04000, 0x2000, CRC(94f9c7f8) SHA1(494389983fb62fe2d772c276e659b6b20c531933) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "merit.u40",    0x00000, 0x2000, CRC(ab0088eb) SHA1(23a05a4dc11a8497f4fc7e4a76085af15ff89cea) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "merit.pb1",    0x08000, 0x8000, CRC(d1584173) SHA1(7a2190203f478f446cc70c473c345e7cc332e049) )
	ROM_LOAD( "merit.pb2",    0x18000, 0x8000, CRC(d00ab1fd) SHA1(c94269c8a478e88f71aeca94c6f20fc05a9c62bd) )
	ROM_LOAD( "merit.pb3",    0x28000, 0x8000, CRC(71b398a9) SHA1(5ea07c409afd52c7d08592b30ff0ff3b72c3f8c3) )
	ROM_LOAD( "merit.pb4",    0x38000, 0x8000, CRC(eb34672f) SHA1(c472fc4445fc434029a2740dfc1d9ab9b1ef9f87) )
	ROM_LOAD( "merit.pb5",    0x48000, 0x8000, CRC(8eea30b9) SHA1(fe1d0332106631f56bc6c57a888da9e4e63fa52f) )
	ROM_LOAD( "merit.pb6",    0x58000, 0x8000, CRC(3f45064d) SHA1(de109ac0b19fd1cd7f0020cc174c2da21708108c) )
	ROM_LOAD( "merit.pb7",    0x68000, 0x8000, CRC(f1c446cd) SHA1(9a6f18defbb64e202ae12e1a59502b8f2d6a58a6) )
	ROM_LOAD( "merit.pb8",    0x78000, 0x8000, CRC(206cfc0d) SHA1(78f6b684713459a617096aa3ffe6e9e62583938c) )
	ROM_LOAD( "merit.pb9",    0x88000, 0x8000, CRC(9333dbca) SHA1(dd87e6f69d60580fdb6f979398edbeb1a51be355) )
	ROM_LOAD( "merit.pba",    0x98000, 0x8000, CRC(6eda81f4) SHA1(6d64344691e3e52035a7d30fb3e762f0bd397db7) )
ROM_END

ROM_START( phrcraze )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "phrz.11",      0x00000, 0x8000, CRC(ccd33a0c) SHA1(869b66af4369f3b4bc19336ca2b8104c7f652de7) )

	// probably over-dumped
	ROM_REGION( 0x18000, "gfx1", 0 )
	ROM_LOAD( "phrz.10",      0x00000, 0x8000, CRC(bfa78b67) SHA1(1b51c0e00240f798fe717624e706cb15700bc2f9) )
	ROM_LOAD( "phrz.8",       0x08000, 0x8000, CRC(9ce22cb3) SHA1(b653afb8f13decd993e434aaad69a6e09ab65f83) )
	ROM_LOAD( "phrz.7",       0x10000, 0x8000, CRC(237e221a) SHA1(7aa69375c2b9a9e73e0e4ed207bf595368b2deb2) )

	// probably over-dumped
	ROM_REGION( 0x8000, "gfx2", 0 )
	ROM_LOAD( "phrz.9",       0x00000, 0x8000, CRC(17dcddd4) SHA1(51682bdbfb67cd0ccf20b97e8fa12d72f0fe82ed) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "phrz.1",       0x00000, 0x8000, CRC(0a016c5e) SHA1(1a24ecd7fe59b08c75a1b4575c7fe467cc7f0cf8) )
	ROM_LOAD( "phrz.2",       0x10000, 0x8000, CRC(e67dc49e) SHA1(5265af228531dc16db7f7ee78da6e51ef9a1d772) )
	ROM_LOAD( "phrz.3",       0x20000, 0x8000, CRC(5c79a653) SHA1(85a904465b347564e937074e2b18159604c83e51) )
	ROM_LOAD( "phrz.4",       0x30000, 0x8000, CRC(9837f757) SHA1(01106114b6997fe6432e519101f95c83a1f7cc1e) )
	ROM_LOAD( "phrz.5",       0x40000, 0x8000, CRC(dc9d8682) SHA1(46973da4298d0ed149c651498527c91b8ba57e0a) )
	ROM_LOAD( "phrz.6",       0x50000, 0x8000, CRC(48e24f17) SHA1(f50c85505f6ab2360f0885494001f174224f8575) )
ROM_END

ROM_START( phrcrazs )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "u5.bin",       0x00000, 0x8000, CRC(6122b5bb) SHA1(9952b14334287a992eefefbdc887b9a9215304ef) )

	ROM_REGION( 0xc000, "gfx1", 0 )
	ROM_LOAD( "u39.bin",      0x00000, 0x4000, CRC(adbd2cdc) SHA1(a1e9481bd6ee0f8915cea43eaad3ebdd54438eed) )
	ROM_LOAD( "u38.bin",      0x04000, 0x4000, CRC(3578f00d) SHA1(c6780a6ee1b5eb00258a89bceabbbe380d79d299) )
	ROM_LOAD( "u37.bin",      0x08000, 0x4000, CRC(962f18a3) SHA1(ec1c3e470c59905c0f56fce2703f6ff586849512) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "u40.bin",      0x00000, 0x4000, CRC(493172c8) SHA1(a76ff5d0d3dd56b0ee4352f03c9ce92f107d34ec) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) // questions
	ROM_LOAD( "std1.bin",     0x00000, 0x8000, CRC(0a016c5e) SHA1(1a24ecd7fe59b08c75a1b4575c7fe467cc7f0cf8) )
	ROM_LOAD( "std2.bin",     0x10000, 0x8000, CRC(e67dc49e) SHA1(5265af228531dc16db7f7ee78da6e51ef9a1d772) )
	ROM_LOAD( "std3.bin",     0x20000, 0x8000, CRC(5c79a653) SHA1(85a904465b347564e937074e2b18159604c83e51) )
	ROM_LOAD( "std4.bin",     0x30000, 0x8000, CRC(9837f757) SHA1(01106114b6997fe6432e519101f95c83a1f7cc1e) )
	ROM_LOAD( "std5.bin",     0x40000, 0x8000, CRC(dc9d8682) SHA1(46973da4298d0ed149c651498527c91b8ba57e0a) )
	ROM_LOAD( "std6.bin",     0x50000, 0x8000, CRC(48e24f17) SHA1(f50c85505f6ab2360f0885494001f174224f8575) )
	/* empty space as per instructions for other "sex" category roms */
	//"Sex" questions revision A
	ROM_LOAD( "sex1_2a.bin",  0x80000, 0x8000, CRC(7ef3bca7) SHA1(f25cd01f996882a500e1a800d924759cd1de255d) )
	ROM_LOAD( "sex1_1a.bin",  0x90000, 0x8000, CRC(ed7604b8) SHA1(b1e841b50b8ef6ae95fafac1c34b6d0337a05d18) )
ROM_END

ROM_START( dodge )
	ROM_REGION( 0x10000, "main", 0 )
	ROM_LOAD( "2131-82.u5",     0x0000, 0x8000, CRC(eb82515d) SHA1(d2c15bd633472f50b621ba90598559e345246d01) )

	ROM_REGION( 0x18000, "gfx1", 0 )
    /*
    dodge.u37        1ST AND 2ND HALF IDENTICAL
    dodge.u38        1ST AND 2ND HALF IDENTICAL
    dodge.u39        1ST AND 2ND HALF IDENTICAL */

	ROM_LOAD( "dodge.u39",    0x00000, 0x8000, CRC(3b3376a1) SHA1(6880cdc29686ff7328717c3833ff826c278b023e) )
	ROM_LOAD( "dodge.u38",    0x08000, 0x8000, CRC(654d5b00) SHA1(9e16330b2dc8821fc20a39eb42176fda23085bfc) )
	ROM_LOAD( "dodge.u37",    0x10000, 0x8000, CRC(bc9e63d4) SHA1(2320f5a0545f18e1e42a3a45fedce912c36fbe13) )

	ROM_REGION( 0x2000, "gfx2", ROMREGION_ERASEFF )
    /* socket at position u40 is unpopulated */
ROM_END



static DRIVER_INIT( key_0 )
{
	decryption_key = 0;
}

static DRIVER_INIT( key_2 )
{
	decryption_key = 2;
}

static DRIVER_INIT( key_4 )
{
	decryption_key = 4;
}

static DRIVER_INIT( key_5 )
{
	decryption_key = 5;
}

static DRIVER_INIT( key_7 )
{
	decryption_key = 7;
}

GAME( 1983, pitboss,  0,       pitboss,  pitboss,  0,      ROT0,  "Merit", "Pit Boss",                                    GAME_WRONG_COLORS | GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
GAME( 1983, casino5,  0,       pitboss,  pitboss,  0,      ROT0,  "Merit", "Casino 5",                                    GAME_WRONG_COLORS | GAME_IMPERFECT_GRAPHICS | GAME_NOT_WORKING )
GAME( 1985, trvwzh,   0,       trvwhiz,  trivia,   key_0,  ROT0,  "Merit", "Trivia ? Whiz (Horizontal - Question set 1)", GAME_WRONG_COLORS )
GAME( 1985, trvwzha,  trvwzh,  trvwhiz,  trivia,   key_0,  ROT0,  "Merit", "Trivia ? Whiz (Horizontal - Question set 2)", GAME_WRONG_COLORS )
GAME( 1985, trvwzv,   0,       trvwhiz,  trivia,   key_0,  ROT90, "Merit", "Trivia ? Whiz (Vertical - Question set 1)",   GAME_WRONG_COLORS )
GAME( 1985, trvwzva,  trvwzv,  trvwhiz,  trivia,   key_0,  ROT90, "Merit", "Trivia ? Whiz (Vertical - Question set 2)",   GAME_WRONG_COLORS )
GAME( 1985, trvwz2,   0,       trvwhiz,  trivia,   key_2,  ROT90, "Merit", "Trivia ? Whiz (Edition 2)",                   GAME_WRONG_COLORS | GAME_IMPERFECT_GRAPHICS )
GAME( 1985, trvwz3h,  0,       trvwhiz,  trivia,   key_0,  ROT0,  "Merit", "Trivia ? Whiz (Edition 3 - Horizontal)",      GAME_WRONG_COLORS | GAME_IMPERFECT_GRAPHICS )
GAME( 1985, trvwz3v,  0,       trvwhiz,  trivia,   key_0,  ROT90, "Merit", "Trivia ? Whiz (Edition 3 - Vertical)",        GAME_WRONG_COLORS | GAME_IMPERFECT_GRAPHICS )
GAME( 1985, trvwz4,   0,       trvwhziv, trvwhziv, key_5,  ROT90, "Merit", "Trivia ? Whiz (Edition 4)",                   GAME_WRONG_COLORS | GAME_IMPERFECT_GRAPHICS )
GAME( 1985, tictac,   0,       tictac,   tictac,   key_4,  ROT0,  "Merit", "Tic Tac Trivia",                              GAME_WRONG_COLORS )
GAME( 1986, phrcraze, 0,       phrcraze, phrcraze, key_7,  ROT0,  "Merit", "Phraze Craze",                                GAME_WRONG_COLORS )
GAME( 1986, phrcrazs, 0,       phrcraze, phrcrazs, key_7,  ROT90, "Merit", "Phraze Craze (Sex Kit)",                      GAME_WRONG_COLORS )
GAME( 1986, bigappg,  0,       bigappg,  bigappg,  0,      ROT0,  "Merit", "Big Apple Games ?",                           GAME_WRONG_COLORS | GAME_NOT_WORKING )
GAME( 1986, dodge,    0,       bigappg,  bigappg,  0,      ROT0,  "Merit", "Dodge City",                                  GAME_WRONG_COLORS | GAME_NOT_WORKING )
