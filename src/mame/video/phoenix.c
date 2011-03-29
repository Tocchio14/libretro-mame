/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "video/resnet.h"
#include "audio/pleiads.h"
#include "includes/phoenix.h"



/***************************************************************************

  Convert the color PROMs into a more useable format.

  Phoenix has two 256x4 palette PROMs, one containing the high bits and the
  other the low bits (2x2x2 color space).
  The palette PROMs are connected to the RGB output this way:

  bit 3 --
        -- 270 ohm resistor  -- GREEN
        -- 270 ohm resistor  -- BLUE
  bit 0 -- 270 ohm resistor  -- RED

  bit 3 --
        -- GREEN
        -- BLUE
  bit 0 -- RED

  plus 270 ohm pullup and pulldown resistors on all lines

***************************************************************************/

static const res_net_decode_info phoenix_decode_info =
{
	2,		// there may be two proms needed to construct color
	0,		// start at 0
	255,	// end at 255
	//  R,   G,   B,   R,   G,   B
	{   0,   0,   0, 256, 256, 256},		// offsets
	{   0,   2,   1,  -1,   1,   0},		// shifts
	{0x01,0x01,0x01,0x02,0x02,0x02}			// masks
};

static const res_net_info phoenix_net_info =
{
	RES_NET_VCC_5V | RES_NET_VBIAS_5V | RES_NET_VIN_OPEN_COL,
	{
		{ RES_NET_AMP_NONE, 270, 270, 2, { 270, 1 } },
		{ RES_NET_AMP_NONE, 270, 270, 2, { 270, 1 } },
		{ RES_NET_AMP_NONE, 270, 270, 2, { 270, 1 } }
	}
};

static const res_net_info pleiades_net_info =
{
	RES_NET_VCC_5V | RES_NET_VBIAS_5V | RES_NET_VIN_OPEN_COL,
	{
		{ RES_NET_AMP_NONE, 150, 270, 2, { 270, 1 } },
		{ RES_NET_AMP_NONE, 150, 270, 2, { 270, 1 } },
		{ RES_NET_AMP_NONE, 150, 270, 2, { 270, 1 } }
	}
};

static const res_net_info survival_net_info =
{
	RES_NET_VCC_5V | RES_NET_VBIAS_5V | RES_NET_VIN_OPEN_COL,
	{
		{ RES_NET_AMP_NONE, 270, 270, 2, { 180, 1 } },
		{ RES_NET_AMP_NONE, 270, 270, 2, { 180, 1 } },
		{ RES_NET_AMP_NONE, 270, 270, 2, { 180, 1 } }
	}
};

PALETTE_INIT( phoenix )
{
	int i;
	rgb_t	*rgb;

	rgb = compute_res_net_all(machine, color_prom, &phoenix_decode_info, &phoenix_net_info);
	/* native order */
	for (i=0;i<256;i++)
	{
		int col;
		col = ((i << 3 ) & 0x18) | ((i>>2) & 0x07) | (i & 0x60);
		palette_set_color(machine,i,rgb[col]);
	}
	palette_normalize_range(machine.palette, 0, 255, 0, 255);
	auto_free(machine, rgb);
}

PALETTE_INIT( survival )
{
	int i;
	rgb_t	*rgb;

	rgb = compute_res_net_all(machine, color_prom, &phoenix_decode_info, &survival_net_info);
	/* native order */
	for (i=0;i<256;i++)
	{
		int col;
		col = ((i << 3 ) & 0x18) | ((i>>2) & 0x07) | (i & 0x60);
		palette_set_color(machine,i,rgb[col]);
	}
	palette_normalize_range(machine.palette, 0, 255, 0, 255);
	auto_free(machine, rgb);
}

PALETTE_INIT( pleiads )
{
	int i;
	rgb_t	*rgb;

	rgb = compute_res_net_all(machine, color_prom, &phoenix_decode_info, &pleiades_net_info);
	/* native order */
	for (i=0;i<256;i++)
	{
		int col;
		col = ((i << 3 ) & 0x18) | ((i>>2) & 0x07) | (i & 0xE0);
		palette_set_color(machine,i,rgb[col]);
	}
	palette_normalize_range(machine.palette, 0, 255, 0, 255);
	auto_free(machine, rgb);
}

/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

static TILE_GET_INFO( get_fg_tile_info )
{
	phoenix_state *state = machine.driver_data<phoenix_state>();
	int code, col;

	code = state->videoram_pg[state->videoram_pg_index][tile_index];
	col = (code >> 5);
	col = col | 0x08 | (state->palette_bank << 4);
	SET_TILE_INFO(
			1,
			code,
			col,
			0);
}

static TILE_GET_INFO( get_bg_tile_info )
{
	phoenix_state *state = machine.driver_data<phoenix_state>();
	int code, col;

	code = state->videoram_pg[state->videoram_pg_index][tile_index + 0x800];
	col = (code >> 5);
	col = col | 0x00 | (state->palette_bank << 4);
	SET_TILE_INFO(
			0,
			code,
			col,
			0);
}

/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( phoenix )
{
	phoenix_state *state = machine.driver_data<phoenix_state>();
	state->videoram_pg[0] = auto_alloc_array(machine, UINT8, 0x1000);
	state->videoram_pg[1] = auto_alloc_array(machine, UINT8, 0x1000);

	memory_configure_bank(machine, "bank1", 0, 1, state->videoram_pg[0], 0);
	memory_configure_bank(machine, "bank1", 1, 1, state->videoram_pg[1], 0);
	memory_set_bank(machine, "bank1", 0);

	state->videoram_pg_index = 0;
	state->palette_bank = 0;
	state->cocktail_mode = 0;

	state->fg_tilemap = tilemap_create(machine, get_fg_tile_info,tilemap_scan_rows,8,8,32,32);
	state->bg_tilemap = tilemap_create(machine, get_bg_tile_info,tilemap_scan_rows,8,8,32,32);

	tilemap_set_transparent_pen(state->fg_tilemap,0);

	tilemap_set_scrolldx(state->fg_tilemap, 0, (HTOTAL - HBSTART));
	tilemap_set_scrolldx(state->bg_tilemap, 0, (HTOTAL - HBSTART));
	tilemap_set_scrolldy(state->fg_tilemap, 0, (VTOTAL - VBSTART));
	tilemap_set_scrolldy(state->bg_tilemap, 0, (VTOTAL - VBSTART));

	state_save_register_global_pointer(machine, state->videoram_pg[0], 0x1000);
	state_save_register_global_pointer(machine, state->videoram_pg[1], 0x1000);
	state_save_register_global(machine, state->videoram_pg_index);
	state_save_register_global(machine, state->palette_bank);
	state_save_register_global(machine, state->cocktail_mode);

	/* some more candidates */
	state->pleiads_protection_question = 0;
	state->survival_protection_value = 0;
	state->survival_sid_value = 0;
	state->survival_input_readc = 0;
	state->survival_input_latches[0] = 0;
	state->survival_input_latches[1] = 0;

	state_save_register_global(machine, state->pleiads_protection_question);
	state_save_register_global(machine, state->survival_protection_value);
	state_save_register_global(machine, state->survival_sid_value);
	state_save_register_global(machine, state->survival_input_readc);
	state_save_register_global_array(machine, state->survival_input_latches);

}

/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE8_HANDLER( phoenix_videoram_w )
{
	phoenix_state *state = space->machine().driver_data<phoenix_state>();
	UINT8 *rom = space->machine().region("maincpu")->base();

	state->videoram_pg[state->videoram_pg_index][offset] = data;

	if ((offset & 0x7ff) < 0x340)
	{
		if (offset & 0x800)
			tilemap_mark_tile_dirty(state->bg_tilemap,offset & 0x3ff);
		else
			tilemap_mark_tile_dirty(state->fg_tilemap,offset & 0x3ff);
	}

	/* as part of the protecion, Survival executes code from $43a4 */
	rom[offset + 0x4000] = data;
}


WRITE8_HANDLER( phoenix_videoreg_w )
{
	phoenix_state *state = space->machine().driver_data<phoenix_state>();
	if (state->videoram_pg_index != (data & 1))
	{
		/* set memory bank */
		state->videoram_pg_index = data & 1;
		memory_set_bank(space->machine(), "bank1", state->videoram_pg_index);

		state->cocktail_mode = state->videoram_pg_index && (input_port_read(space->machine(), "CAB") & 0x01);

		tilemap_set_flip_all(space->machine(), state->cocktail_mode ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
		tilemap_mark_all_tiles_dirty_all(space->machine());
	}

	/* Phoenix has only one palette select effecting both layers */
	if (state->palette_bank != ((data >> 1) & 1))
	{
		state->palette_bank = (data >> 1) & 1;

		tilemap_mark_all_tiles_dirty_all(space->machine());
	}
}

WRITE8_HANDLER( pleiads_videoreg_w )
{
	phoenix_state *state = space->machine().driver_data<phoenix_state>();
	if (state->videoram_pg_index != (data & 1))
	{
		/* set memory bank */
		state->videoram_pg_index = data & 1;
		memory_set_bank(space->machine(), "bank1", state->videoram_pg_index);

		state->cocktail_mode = state->videoram_pg_index && (input_port_read(space->machine(), "CAB") & 0x01);

		tilemap_set_flip_all(space->machine(), state->cocktail_mode ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
		tilemap_mark_all_tiles_dirty_all(space->machine());
	}


	/* the palette table is at $0420-$042f and is set by $06bc.
       Four palette changes by level.  The palette selection is
       wrong, but the same paletter is used for both layers. */

	if (state->palette_bank != ((data >> 1) & 3))
	{
		state->palette_bank = ((data >> 1) & 3);

		tilemap_mark_all_tiles_dirty_all(space->machine());

		logerror("Palette: %02X\n", (data & 0x06) >> 1);
	}

	state->pleiads_protection_question = data & 0xfc;

	/* send two bits to sound control C (not sure if they are there) */
	pleiads_sound_control_c_w(space->machine().device("cust"), offset, data);
}


WRITE8_HANDLER( phoenix_scroll_w )
{
	phoenix_state *state = space->machine().driver_data<phoenix_state>();
	tilemap_set_scrollx(state->bg_tilemap,0,data);
}


CUSTOM_INPUT( player_input_r )
{
	phoenix_state *state = field->port->machine().driver_data<phoenix_state>();
	if (state->cocktail_mode)
		return (input_port_read(field->port->machine(), "CTRL") & 0xf0) >> 4;
	else
		return (input_port_read(field->port->machine(), "CTRL") & 0x0f) >> 0;
}

CUSTOM_INPUT( pleiads_protection_r )
{
	phoenix_state *state = field->port->machine().driver_data<phoenix_state>();
	/* handle Pleiads protection */
	switch (state->pleiads_protection_question)
	{
	case 0x00:
	case 0x20:
		/* Bit 3 is 0 */
		return 0;
	case 0x0c:
	case 0x30:
		/* Bit 3 is 1 */
		return 1;
	default:
		logerror("%s:Unknown protection question %02X\n", field->port->machine().describe_context(), state->pleiads_protection_question);
		return 0;
	}
}

/*
    Protection.  There is a 14 pin part connected to the 8910 Port B D0 labeled DL57S22

    Inputs are demangled at 0x1ae6-0x1b04 using the table at 0x1b26
    and bit 0 of the data from the AY8910 port B. The equation is:
    input = map[input] + ay_data + b@437c
    (b@437c is set and cleared elsewhere in the code, but is
    always 0 during the demangling.)

    A routine at 0x2f31 checks for incorrect AY8910 port B data.
    Incorrect values increment an error counter at 0x4396 which
    causes bad sprites and will kill the game after a specified
    number of errors. For input & 0xf0 == 0 or 2 or 4, AY8910
    port B must have bit 0 cleared. For all other joystick bits,
    it must be set.

    Another  routine at 0x02bc checks for bad SID data, and
    increments the same error counter and cancels certain joystick input.

    The hiscore data entry routine at 0x2fd8 requires unmangled inputs
    at 0x3094. This could explain the significance of the loop where
    the joystick inputs are read for gameplay at 0x2006-0x202a. The
    code waits here for two consecutive identical reads from the AY8910.
    This probably means there's a third read of raw data with some or all
    of the otherwise unused bits 1-7 on the AY8910 port B set to
    distinguish it from a gameplay read.
*/

#define REMAP_JS(js) ((ret & 0xf) | ( (js & 0xf)  << 4))
READ8_HANDLER( survival_input_port_0_r )
{
	phoenix_state *state = space->machine().driver_data<phoenix_state>();
	UINT8 ret = ~input_port_read(space->machine(), "IN0");

	if( state->survival_input_readc++ == 2 )
	{
		state->survival_input_readc = 0;
		state->survival_protection_value = 0;
		return ~ret;
	}

	// Any value that remaps the joystick input to 0,2,4 must clear bit 0
	// on the AY8910 port B. All other remaps must set bit 0.

	state->survival_protection_value = 0xff;
	state->survival_sid_value = 0;

	switch( ( ret >> 4) & 0xf )
	{
		case 0: // js_nop = 7 + 1
			ret = REMAP_JS( 7 );
			break;
		case 1: // js_n = 1 + 1
			ret = REMAP_JS( 8 );
			break;
		case 2: // js_e = 0 + 0
			state->survival_sid_value = 0x80;
			state->survival_protection_value = 0xfe;
			ret = REMAP_JS( 2 );
			break;
		case 3: // js_ne = 0 + 1;
			state->survival_sid_value = 0x80;
			ret = REMAP_JS( 0xa );
			break;
		case 4: // js_w = 4 + 0
			state->survival_sid_value = 0x80;
			state->survival_protection_value = 0xfe;
			ret = REMAP_JS( 4 );
			break;
		case 5: // js_nw = 2 + 1
			state->survival_sid_value = 0x80;
			ret = REMAP_JS( 0xc );
			break;
		case 8: // js_s = 5 + 1
			ret = REMAP_JS( 1 );
			break;
		case 0xa: // js_se = 6 + 1
			state->survival_sid_value = 0x80;
			ret = REMAP_JS( 3 );
			break;
		case 0xc: // js_sw = 4 + 1
			state->survival_sid_value = 0x80;
			ret = REMAP_JS( 5 );
			break;
		default:
			break;
	}

	state->survival_input_latches[0] = state->survival_input_latches[1];
	state->survival_input_latches[1] = ~ret;

	return state->survival_input_latches[0];
}

READ8_DEVICE_HANDLER( survival_protection_r )
{
	phoenix_state *state = device->machine().driver_data<phoenix_state>();
	return state->survival_protection_value;
}

READ_LINE_DEVICE_HANDLER( survival_sid_callback )
{
	phoenix_state *state = device->machine().driver_data<phoenix_state>();
	return state->survival_sid_value;
}


/***************************************************************************

  Display refresh

***************************************************************************/

SCREEN_UPDATE( phoenix )
{
	phoenix_state *state = screen->machine().driver_data<phoenix_state>();
	tilemap_draw(bitmap,cliprect,state->bg_tilemap,0,0);
	tilemap_draw(bitmap,cliprect,state->fg_tilemap,0,0);
	return 0;
}
