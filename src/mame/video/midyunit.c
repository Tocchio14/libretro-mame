/*************************************************************************

    Williams/Midway Y/Z-unit system

**************************************************************************/

#include "emu.h"
#include "profiler.h"
#include "cpu/tms34010/tms34010.h"
#include "includes/midyunit.h"


/* compile-time options */
#define LOG_DMA				0		/* DMAs are logged if the 'L' key is pressed */


/* constants for the DMA chip */
enum
{
	DMA_COMMAND = 0,
	DMA_ROWBYTES,
	DMA_OFFSETLO,
	DMA_OFFSETHI,
	DMA_XSTART,
	DMA_YSTART,
	DMA_WIDTH,
	DMA_HEIGHT,
	DMA_PALETTE,
	DMA_COLOR
};



/*************************************
 *
 *  Video startup
 *
 *************************************/

static VIDEO_START( common )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	/* allocate memory */
	state->cmos_ram = auto_alloc_array(machine, UINT16, (0x2000 * 4)/2);
	state->local_videoram = auto_alloc_array_clear(machine, UINT16, 0x80000/2);
	state->pen_map = auto_alloc_array(machine, pen_t, 65536);

	machine->device<nvram_device>("nvram")->set_base(state->cmos_ram, 0x2000 * 4);

	/* reset all the globals */
	state->cmos_page = 0;
	state->autoerase_enable = 0;
	state->yawdim_dma = 0;

	/* reset DMA state */
	memset(state->dma_register, 0, sizeof(state->dma_register));
	memset(&state->dma_state, 0, sizeof(state->dma_state));

	/* register for state saving */
	state_save_register_global(machine, state->autoerase_enable);
	state_save_register_global_pointer(machine, state->local_videoram, 0x80000/2);
	state_save_register_global_pointer(machine, state->cmos_ram, (0x2000 * 4)/2);
	state_save_register_global(machine, state->videobank_select);
	state_save_register_global_array(machine, state->dma_register);
}


VIDEO_START( midyunit_4bit )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	int i;

	VIDEO_START_CALL(common);

	/* init for 4-bit */
	for (i = 0; i < 65536; i++)
		state->pen_map[i] = ((i & 0xf000) >> 8) | (i & 0x000f);
	state->palette_mask = 0x00ff;
}


VIDEO_START( midyunit_6bit )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	int i;

	VIDEO_START_CALL(common);

	/* init for 6-bit */
	for (i = 0; i < 65536; i++)
		state->pen_map[i] = ((i & 0xc000) >> 8) | (i & 0x0f3f);
	state->palette_mask = 0x0fff;
}


VIDEO_START( mkyawdim )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	VIDEO_START_CALL(midyunit_6bit);
	state->yawdim_dma = 1;
}


VIDEO_START( midzunit )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	int i;

	VIDEO_START_CALL(common);

	/* init for 8-bit */
	for (i = 0; i < 65536; i++)
		state->pen_map[i] = i & 0x1fff;
	state->palette_mask = 0x1fff;
}



/*************************************
 *
 *  Banked graphics ROM access
 *
 *************************************/

READ16_HANDLER( midyunit_gfxrom_r )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	offset *= 2;
	if (state->palette_mask == 0x00ff)
		return state->gfx_rom[offset] | (state->gfx_rom[offset] << 4) |
				(state->gfx_rom[offset + 1] << 8) | (state->gfx_rom[offset + 1] << 12);
	else
		return state->gfx_rom[offset] | (state->gfx_rom[offset + 1] << 8);
}



/*************************************
 *
 *  Video/color RAM read/write
 *
 *************************************/

WRITE16_HANDLER( midyunit_vram_w )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	offset *= 2;
	if (state->videobank_select)
	{
		if (ACCESSING_BITS_0_7)
			state->local_videoram[offset] = (data & 0x00ff) | (state->dma_register[DMA_PALETTE] << 8);
		if (ACCESSING_BITS_8_15)
			state->local_videoram[offset + 1] = (data >> 8) | (state->dma_register[DMA_PALETTE] & 0xff00);
	}
	else
	{
		if (ACCESSING_BITS_0_7)
			state->local_videoram[offset] = (state->local_videoram[offset] & 0x00ff) | (data << 8);
		if (ACCESSING_BITS_8_15)
			state->local_videoram[offset + 1] = (state->local_videoram[offset + 1] & 0x00ff) | (data & 0xff00);
	}
}


READ16_HANDLER( midyunit_vram_r )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	offset *= 2;
	if (state->videobank_select)
		return (state->local_videoram[offset] & 0x00ff) | (state->local_videoram[offset + 1] << 8);
	else
		return (state->local_videoram[offset] >> 8) | (state->local_videoram[offset + 1] & 0xff00);
}



/*************************************
 *
 *  Shift register read/write
 *
 *************************************/

void midyunit_to_shiftreg(address_space *space, UINT32 address, UINT16 *shiftreg)
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	memcpy(shiftreg, &state->local_videoram[address >> 3], 2 * 512 * sizeof(UINT16));
}


void midyunit_from_shiftreg(address_space *space, UINT32 address, UINT16 *shiftreg)
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	memcpy(&state->local_videoram[address >> 3], shiftreg, 2 * 512 * sizeof(UINT16));
}



/*************************************
 *
 *  Y/Z-unit control register
 *
 *************************************/

WRITE16_HANDLER( midyunit_control_w )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	/*
     * Narc system register
     * ------------------
     *
     *   | Bit              | Use
     * --+-FEDCBA9876543210-+------------
     *   | xxxxxxxx-------- |   7 segment led on CPU board
     *   | --------xx------ |   CMOS page
     *   | ----------x----- | - OBJ PAL RAM select
     *   | -----------x---- | - autoerase enable
     *   | ---------------- | - watchdog
     *
     */

	if (ACCESSING_BITS_0_7)
	{
		/* CMOS page is bits 6-7 */
		state->cmos_page = ((data >> 6) & 3) * 0x1000;

		/* video bank select is bit 5 */
		state->videobank_select = (data >> 5) & 1;

		/* handle autoerase disable (bit 4) */
		state->autoerase_enable = ((data & 0x10) == 0);
	}
}



/*************************************
 *
 *  Palette handlers
 *
 *************************************/

WRITE16_HANDLER( midyunit_paletteram_w )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	int newword;

	COMBINE_DATA(&space->machine->generic.paletteram.u16[offset]);
	newword = space->machine->generic.paletteram.u16[offset];
	palette_set_color_rgb(space->machine, offset & state->palette_mask, pal5bit(newword >> 10), pal5bit(newword >> 5), pal5bit(newword >> 0));
}



/*************************************
 *
 *  DMA drawing routines
 *
 *************************************/

static void dma_draw(running_machine *machine, UINT16 command)
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	struct dma_state_t &dma_state = state->dma_state;
	int dx = (command & 0x10) ? -1 : 1;
	int height = dma_state.height;
	int width = dma_state.width;
	UINT8 *base = state->gfx_rom;
	UINT32 offset = dma_state.offset >> 3;
	UINT16 pal = dma_state.palette;
	UINT16 color = pal | dma_state.color;
	int x, y;

	/* we only need the low 4 bits of the command */
	command &= 0x0f;

	/* loop over the height */
	for (y = 0; y < height; y++)
	{
		int tx = dma_state.xpos;
		int ty = dma_state.ypos;
		UINT32 o = offset;
		UINT16 *dest;

		/* determine Y position */
		ty = (ty + y) & 0x1ff;
		offset += dma_state.rowbytes;

		/* determine destination pointer */
		dest = &state->local_videoram[ty * 512];

		/* check for overruns if they are relevant */
		if (o >= 0x06000000 && command < 0x0c)
			continue;

		/* switch off the zero/non-zero options */
		switch (command)
		{
			case 0x00:	/* draw nothing */
				break;

			case 0x01:	/* draw only 0 pixels */
				for (x = 0; x < width; x++, tx += dx)
					if (base[o++] == 0)
						dest[tx] = pal;
				break;

			case 0x02:	/* draw only non-0 pixels */
				for (x = 0; x < width; x++, tx += dx)
				{
					int pixel = base[o++];
					if (pixel != 0)
						dest[tx] = pal | pixel;
				}
				break;

			case 0x03:	/* draw all pixels */
				for (x = 0; x < width; x++, tx += dx)
					dest[tx] = pal | base[o++];
				break;

			case 0x04:	/* color only 0 pixels */
			case 0x05:	/* color only 0 pixels */
				for (x = 0; x < width; x++, tx += dx)
					if (base[o++] == 0)
						dest[tx] = color;
				break;

			case 0x06:	/* color only 0 pixels, copy the rest */
			case 0x07:	/* color only 0 pixels, copy the rest */
				for (x = 0; x < width; x++, tx += dx)
				{
					int pixel = base[o++];
					dest[tx] = (pixel == 0) ? color : (pal | pixel);
				}
				break;

			case 0x08:	/* color only non-0 pixels */
			case 0x0a:	/* color only non-0 pixels */
				for (x = 0; x < width; x++, tx += dx)
					if (base[o++] != 0)
						dest[tx] = color;
				break;

			case 0x09:	/* color only non-0 pixels, copy the rest */
			case 0x0b:	/* color only non-0 pixels, copy the rest */
				for (x = 0; x < width; x++, tx += dx)
				{
					int pixel = base[o++];
					dest[tx] = (pixel != 0) ? color : (pal | pixel);
				}
				break;

			case 0x0c:	/* color all pixels */
			case 0x0d:	/* color all pixels */
			case 0x0e:	/* color all pixels */
			case 0x0f:	/* color all pixels */
				for (x = 0; x < width; x++, tx += dx)
					dest[tx] = color;
				break;
		}
	}
}



/*************************************
 *
 *  DMA finished callback
 *
 *************************************/

static TIMER_CALLBACK( dma_callback )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	state->dma_register[DMA_COMMAND] &= ~0x8000; /* tell the cpu we're done */
	cputag_set_input_line(machine, "maincpu", 0, ASSERT_LINE);
}



/*************************************
 *
 *  DMA reader
 *
 *************************************/

READ16_HANDLER( midyunit_dma_r )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	return state->dma_register[offset];
}



/*************************************
 *
 *  DMA write handler
 *
 *************************************/

/*
 * DMA registers
 * ------------------
 *
 *  Register | Bit              | Use
 * ----------+-FEDCBA9876543210-+------------
 *     0     | x--------------- | trigger write (or clear if zero)
 *           | ---184-1-------- | unknown
 *           | ----------x----- | flip y
 *           | -----------x---- | flip x
 *           | ------------x--- | blit nonzero pixels as color
 *           | -------------x-- | blit zero pixels as color
 *           | --------------x- | blit nonzero pixels
 *           | ---------------x | blit zero pixels
 *     1     | xxxxxxxxxxxxxxxx | width offset
 *     2     | xxxxxxxxxxxxxxxx | source address low word
 *     3     | xxxxxxxxxxxxxxxx | source address high word
 *     4     | xxxxxxxxxxxxxxxx | detination x
 *     5     | xxxxxxxxxxxxxxxx | destination y
 *     6     | xxxxxxxxxxxxxxxx | image columns
 *     7     | xxxxxxxxxxxxxxxx | image rows
 *     8     | xxxxxxxxxxxxxxxx | palette
 *     9     | xxxxxxxxxxxxxxxx | color
 */

WRITE16_HANDLER( midyunit_dma_w )
{
	midyunit_state *state = space->machine->driver_data<midyunit_state>();
	struct dma_state_t &dma_state = state->dma_state;
	UINT32 gfxoffset;
	int command;

	/* blend with the current register contents */
	COMBINE_DATA(&state->dma_register[offset]);

	/* only writes to DMA_COMMAND actually cause actions */
	if (offset != DMA_COMMAND)
		return;

	/* high bit triggers action */
	command = state->dma_register[DMA_COMMAND];
	cputag_set_input_line(space->machine, "maincpu", 0, CLEAR_LINE);
	if (!(command & 0x8000))
		return;

if (LOG_DMA)
{
	if (input_code_pressed(space->machine, KEYCODE_L))
	{
		logerror("----\n");
		logerror("DMA command %04X: (xflip=%d yflip=%d)\n",
				command, (command >> 4) & 1, (command >> 5) & 1);
		logerror("  offset=%08X pos=(%d,%d) w=%d h=%d rb=%d\n",
				state->dma_register[DMA_OFFSETLO] | (state->dma_register[DMA_OFFSETHI] << 16),
				(INT16)state->dma_register[DMA_XSTART], (INT16)state->dma_register[DMA_YSTART],
				state->dma_register[DMA_WIDTH], state->dma_register[DMA_HEIGHT], (INT16)state->dma_register[DMA_ROWBYTES]);
		logerror("  palette=%04X color=%04X\n",
				state->dma_register[DMA_PALETTE], state->dma_register[DMA_COLOR]);
	}
}

	g_profiler.start(PROFILER_USER1);

	/* fill in the basic data */
	dma_state.rowbytes = (INT16)state->dma_register[DMA_ROWBYTES];
	dma_state.xpos = (INT16)state->dma_register[DMA_XSTART];
	dma_state.ypos = (INT16)state->dma_register[DMA_YSTART];
	dma_state.width = state->dma_register[DMA_WIDTH];
	dma_state.height = state->dma_register[DMA_HEIGHT];
	dma_state.palette = state->dma_register[DMA_PALETTE] << 8;
	dma_state.color = state->dma_register[DMA_COLOR] & 0xff;

	/* determine the offset and adjust the rowbytes */
	gfxoffset = state->dma_register[DMA_OFFSETLO] | (state->dma_register[DMA_OFFSETHI] << 16);
	if (command & 0x10)
	{
		if (!state->yawdim_dma)
		{
			gfxoffset -= (dma_state.width - 1) * 8;
			dma_state.rowbytes = (dma_state.rowbytes - dma_state.width + 3) & ~3;
		}
		else
			dma_state.rowbytes = (dma_state.rowbytes + dma_state.width + 3) & ~3;
		dma_state.xpos += dma_state.width - 1;
	}
	else
		dma_state.rowbytes = (dma_state.rowbytes + dma_state.width + 3) & ~3;

	/* apply Y clipping */
	if (dma_state.ypos < 0)
	{
		dma_state.height -= -dma_state.ypos;
		dma_state.offset += (-dma_state.ypos * dma_state.rowbytes) << 3;
		dma_state.ypos = 0;
	}
	if (dma_state.ypos + dma_state.height > 512)
		dma_state.height = 512 - dma_state.ypos;

	/* apply X clipping */
	if (!(command & 0x10))
	{
		if (dma_state.xpos < 0)
		{
			dma_state.width -= -dma_state.xpos;
			dma_state.offset += -dma_state.xpos << 3;
			dma_state.xpos = 0;
		}
		if (dma_state.xpos + dma_state.width > 512)
			dma_state.width = 512 - dma_state.xpos;
	}
	else
	{
		if (dma_state.xpos >= 512)
		{
			dma_state.width -= dma_state.xpos - 511;
			dma_state.offset += (dma_state.xpos - 511) << 3;
			dma_state.xpos = 511;
		}
		if (dma_state.xpos - dma_state.width < 0)
			dma_state.width = dma_state.xpos;
	}

	/* determine the location and draw */
	if (gfxoffset < 0x02000000)
		gfxoffset += 0x02000000;
	{
		dma_state.offset = gfxoffset - 0x02000000;
		dma_draw(space->machine, command);
	}

	/* signal we're done */
	space->machine->scheduler().timer_set(attotime::from_nsec(41 * dma_state.width * dma_state.height), FUNC(dma_callback));

	g_profiler.stop();
}



/*************************************
 *
 *  Core refresh routine
 *
 *************************************/

static TIMER_CALLBACK( autoerase_line )
{
	midyunit_state *state = machine->driver_data<midyunit_state>();
	int scanline = param;

	if (state->autoerase_enable && scanline >= 0 && scanline < 510)
		memcpy(&state->local_videoram[512 * scanline], &state->local_videoram[512 * (510 + (scanline & 1))], 512 * sizeof(UINT16));
}


void midyunit_scanline_update(screen_device &screen, bitmap_t *bitmap, int scanline, const tms34010_display_params *params)
{
	midyunit_state *state = screen.machine->driver_data<midyunit_state>();
	UINT16 *src = &state->local_videoram[(params->rowaddr << 9) & 0x3fe00];
	UINT16 *dest = BITMAP_ADDR16(bitmap, scanline, 0);
	int coladdr = params->coladdr << 1;
	int x;

	/* adjust the display address to account for ignored bits */
	for (x = params->heblnk; x < params->hsblnk; x++)
		dest[x] = state->pen_map[src[coladdr++ & 0x1ff]];

	/* handle autoerase on the previous line */
	autoerase_line(screen.machine, NULL, params->rowaddr - 1);

	/* if this is the last update of the screen, set a timer to clear out the final line */
	/* (since we update one behind) */
	if (scanline == screen.visible_area().max_y)
		screen.machine->scheduler().timer_set(screen.time_until_pos(scanline + 1), FUNC(autoerase_line), params->rowaddr);
}
