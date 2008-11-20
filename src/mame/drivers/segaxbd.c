/***************************************************************************

    Sega X-board hardware

    Special thanks to Charles MacDonald for his priceless assistance

****************************************************************************

    Known bugs:
        * gprider has a hack to make it work
        * extra sound boards etc. in some smgp sets not hooked up
        * rachero doesn't like IC17/IC108 (divide chips) in self-test
          due to testing an out-of-bounds value
        * abcop doesn't like IC41/IC108 (divide chips) in self-test
          due to testing an out-of-bounds value
        * smgp sound communication is messed up, causing incorrect
          voice samples to be played; this game seems to need VERY
          tight synchronization between the 68000 and Z80

***************************************************************************/

#include "driver.h"
#include "deprecat.h"
#include "system16.h"
#include "cpu/m68000/m68000.h"
#include "machine/segaic16.h"
#include "sound/2151intf.h"
#include "sound/segapcm.h"


#define MASTER_CLOCK			50000000
#define SOUND_CLOCK				16000000



/*************************************
 *
 *  Statics
 *
 *************************************/

static UINT8 iochip_regs[2][8];
static UINT8 iochip_force_input;
static UINT8 (*iochip_custom_io_r[2])(offs_t offset, UINT8 portdata);
static void (*iochip_custom_io_w[2])(offs_t offset, UINT8 data);

static UINT8 adc_reverse[8];

static UINT8 vblank_irq_state;
static UINT8 timer_irq_state;
static UINT8 gprider_hack;

static UINT16 *backupram1, *backupram2;



/*************************************
 *
 *  Configuration
 *
 *************************************/

static void xboard_generic_init(running_machine *machine)
{
	/* init the FD1094 */
	fd1094_driver_init(machine, NULL);

	/* set the default road priority */
	xboard_set_road_priority(1);

	/* reset the custom handlers and other pointers */
	memset(iochip_custom_io_r, 0, sizeof(iochip_custom_io_r));
	memset(iochip_custom_io_w, 0, sizeof(iochip_custom_io_w));
	memset(adc_reverse, 0, sizeof(adc_reverse));

	gprider_hack = 0;
}



/*************************************
 *
 *  Initialization & interrupts
 *
 *************************************/

static void update_main_irqs(running_machine *machine)
{
	int irq = 0;

	if (timer_irq_state)
		irq |= 2;
	else
		cpu_set_input_line(machine->cpu[0], 2, CLEAR_LINE);

	if (vblank_irq_state)
		irq |= 4;
	else
		cpu_set_input_line(machine->cpu[0], 4, CLEAR_LINE);

	if (gprider_hack && irq > 4)
		irq = 4;

	if (!(irq==6))
		cpu_set_input_line(machine->cpu[0], 6, CLEAR_LINE);

	if (irq)
	{
		cpu_set_input_line(machine->cpu[0], irq, ASSERT_LINE);
		cpuexec_boost_interleave(machine, attotime_zero, ATTOTIME_IN_USEC(100));
	}
}


static TIMER_CALLBACK( scanline_callback )
{
	int scanline = param;
	int next_scanline = (scanline + 2) % 262;
	int update = 0;

	/* clock the timer and set the IRQ if something happened */
	if ((scanline % 2) != 0 && segaic16_compare_timer_clock(0))
		timer_irq_state = update = 1;

	/* set VBLANK on scanline 223 */
	if (scanline == 223)
	{
		vblank_irq_state = update = 1;
		cpu_set_input_line(machine->cpu[1], 4, ASSERT_LINE);
		next_scanline = scanline + 1;
	}

	/* clear VBLANK on scanline 224 */
	else if (scanline == 224)
	{
		vblank_irq_state = 0;
		update = 1;
		cpu_set_input_line(machine->cpu[1], 4, CLEAR_LINE);
		next_scanline = scanline + 1;
	}

	/* update IRQs on the main CPU */
	if (update)
		update_main_irqs(machine);

	/* come back in 2 scanlines */
	timer_set(video_screen_get_time_until_pos(machine->primary_screen, next_scanline, 0), NULL, next_scanline, scanline_callback);
}


static void timer_ack_callback(running_machine *machine)
{
	/* clear the timer IRQ */
	timer_irq_state = 0;
	update_main_irqs(machine);
}



/*************************************
 *
 *  Sound communication
 *
 *************************************/

static TIMER_CALLBACK( delayed_sound_data_w )
{
	const address_space *space = cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM);

	soundlatch_w(space, 0, param);
	cpu_set_input_line(machine->cpu[2], INPUT_LINE_NMI, ASSERT_LINE);
}


static void sound_data_w(running_machine *machine, UINT8 data)
{
	timer_call_after_resynch(NULL, data, delayed_sound_data_w);
}


static void sound_cpu_irq(running_machine *machine, int state)
{
	cpu_set_input_line(machine->cpu[2], 0, state);
}


static READ8_HANDLER( sound_data_r )
{
	cpu_set_input_line(space->machine->cpu[2], INPUT_LINE_NMI, CLEAR_LINE);
	return soundlatch_r(space, offset);
}



/*************************************
 *
 *  Basic machine setup
 *
 *************************************/

static void xboard_reset(const device_config *device)
{
	cpu_set_input_line(device->machine->cpu[1], INPUT_LINE_RESET, PULSE_LINE);
	cpuexec_boost_interleave(device->machine, attotime_zero, ATTOTIME_IN_USEC(100));
}


static MACHINE_RESET( xboard )
{
	fd1094_machine_init(machine->cpu[0]);
	segaic16_tilemap_reset(0);

	/* hook the RESET line, which resets CPU #1 */
	cpu_set_info_fct(machine->cpu[0], CPUINFO_PTR_M68K_RESET_CALLBACK, (genf *)xboard_reset);

	/* set up the compare/timer chip */
	segaic16_compare_timer_init(0, sound_data_w, timer_ack_callback);
	segaic16_compare_timer_init(1, NULL, NULL);

	/* start timers to track interrupts */
	timer_set(video_screen_get_time_until_pos(machine->primary_screen, 1, 0), NULL, 1, scanline_callback);
}



/*************************************
 *
 *  Input handlers
 *
 *************************************/

static READ16_HANDLER( adc_r )
{
	static const char *const ports[] = { "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6", "ADC7" };
	int which = (iochip_regs[0][2] >> 2) & 7;

	/* on the write, latch the selected input port and stash the value */
	int value = input_port_read_safe(space->machine, ports[which], 0x0010);

	/* reverse some port values */
	if (adc_reverse[which])
		value = 255 - value;

	/* return the previously latched value */
	return value;
}


static WRITE16_HANDLER( adc_w )
{
}


INLINE UINT16 iochip_r(int which, int port, int inputval)
{
	UINT16 result = iochip_regs[which][port];

	/* if there's custom I/O, do that to get the input value */
	if (iochip_custom_io_r[which])
		inputval = (*iochip_custom_io_r[which])(port, inputval);

	/* for ports 0-3, the direction is controlled 4 bits at a time by register 6 */
	if (port <= 3)
	{
		if (iochip_force_input || ((iochip_regs[which][6] >> (2*port+0)) & 1))
			result = (result & ~0x0f) | (inputval & 0x0f);
		if (iochip_force_input || ((iochip_regs[which][6] >> (2*port+1)) & 1))
			result = (result & ~0xf0) | (inputval & 0xf0);
	}

	/* for port 4, the direction is controlled 1 bit at a time by register 7 */
	else
	{
		if ((iochip_regs[which][7] >> 0) & 1)
			result = (result & ~0x01) | (inputval & 0x01);
		if ((iochip_regs[which][7] >> 1) & 1)
			result = (result & ~0x02) | (inputval & 0x02);
		if ((iochip_regs[which][7] >> 2) & 1)
			result = (result & ~0x04) | (inputval & 0x04);
		if ((iochip_regs[which][7] >> 3) & 1)
			result = (result & ~0x08) | (inputval & 0x08);
		result &= 0x0f;
	}
	return result;
}


static READ16_HANDLER( iochip_0_r )
{
	switch (offset)
	{
		case 0:
			/* Input port:
                D7: (Not connected)
                D6: /INTR of ADC0804
                D5-D0: CN C pin 24-19 (switch state 0= open, 1= closed)
            */
			return iochip_r(0, 0, input_port_read(space->machine, "IO0PORTA"));

		case 1:
			/* I/O port: CN C pins 17,15,13,11,9,7,5,3 */
			return iochip_r(0, 1, input_port_read(space->machine, "IO0PORTB"));

		case 2:
			/* Output port */
			return iochip_r(0, 2, 0);

		case 3:
			/* Output port */
			return iochip_r(0, 3, 0);

		case 4:
			/* Unused */
			return iochip_r(0, 4, 0);
	}

	/* everything else returns 0 */
	return 0;
}


static WRITE16_HANDLER( iochip_0_w )
{
	UINT8 oldval;

	/* access is via the low 8 bits */
	if (!ACCESSING_BITS_0_7)
		return;

	data &= 0xff;

	/* swap in the new value and remember the previous value */
	oldval = iochip_regs[0][offset];
	iochip_regs[0][offset] = data;

	/* certain offsets have common effects */
	switch (offset)
	{
		case 2:
			/* Output port:
                D7: (Not connected)
                D6: (/WDC) - watchdog reset
                D5: Screen display (1= blanked, 0= displayed)
                D4-D2: (ADC2-0)
                D1: (CONT) - affects sprite hardware
                D0: Sound section reset (1= normal operation, 0= reset)
            */
			if (((oldval ^ data) & 0x40) && !(data & 0x40)) watchdog_reset_w(space,0,0);
			segaic16_set_display_enable(space->machine, data & 0x20);
			cpu_set_input_line(space->machine->cpu[2], INPUT_LINE_RESET, (data & 0x01) ? CLEAR_LINE : ASSERT_LINE);
			return;

		case 3:
			/* Output port:
                D7: Amplifier mute control (1= sounding, 0= muted)
                D6-D0: CN D pin A17-A23 (output level 1= high, 0= low)
            */
			sound_global_enable(data & 0x80);
			return;
	}

	if (offset <= 4)
		logerror("I/O chip 0, port %c write = %02X\n", 'A' + offset, data);
}


static READ16_HANDLER( iochip_1_r )
{
	switch (offset)
	{
		case 0:
			/* Input port: switches, CN D pin A1-8 (switch state 1= open, 0= closed) */
			return iochip_r(1, 0, input_port_read(space->machine, "IO1PORTA"));

		case 1:
			/* Input port: switches, CN D pin A9-16 (switch state 1= open, 0= closed) */
			return iochip_r(1, 1, input_port_read(space->machine, "IO1PORTB"));

		case 2:
			/* Input port: DIP switches (1= off, 0= on) */
			return iochip_r(1, 2, input_port_read(space->machine, "IO1PORTC"));

		case 3:
			/* Input port: DIP switches (1= off, 0= on) */
			return iochip_r(1, 3, input_port_read(space->machine, "IO1PORTD"));

		case 4:
			/* Unused */
			return iochip_r(1, 4, 0);
	}

	/* everything else returns 0 */
	return 0;
}


static WRITE16_HANDLER( iochip_1_w )
{
	/* access is via the low 8 bits */
	if (!ACCESSING_BITS_0_7)
		return;

	data &= 0xff;

	/* swap in the new value and remember the previous value */
	iochip_regs[1][offset] = data;

	if (offset <= 4)
		logerror("I/O chip 1, port %c write = %02X\n", 'A' + offset, data);
}


static WRITE16_HANDLER( iocontrol_w )
{
	if (ACCESSING_BITS_0_7)
	{
		logerror("I/O chip force input = %d\n", data & 1);
		/* Racing Hero and ABCop set this and fouls up their output ports */
		/*iochip_force_input = data & 1;*/
	}
}



/*************************************
 *
 *  After Burner II Custom I/O
 *
 *************************************/

static WRITE16_HANDLER( aburner2_iochip_0_D_w )
{
	/* access is via the low 8 bits */
	if (!ACCESSING_BITS_0_7)
		return;

	iochip_regs[0][3] = data;

	output_set_lamp_value(2, (data >> 1) & 0x01);	/* altitude warning lamp */
	output_set_led_value(0, (data >> 2) & 0x01);	/* start lamp */
	coin_counter_w(0, (data >> 4) & 0x01);
	output_set_lamp_value(0, (data >> 5) & 0x01);	/* lock on lamp */
	output_set_lamp_value(1, (data >> 6) & 0x01);	/* danger lamp */
	sound_global_enable((data >> 7) & 0x01);
}



/*************************************
 *
 *  Line of Fire Custom I/O
 *
 *************************************/

static UINT16 *loffire_sync;

static WRITE16_HANDLER( loffire_sync0_w )
{
	COMBINE_DATA(&loffire_sync[offset]);
	cpuexec_boost_interleave(space->machine, attotime_zero, ATTOTIME_IN_USEC(10));
}



/*************************************
 *
 *  SMGP external access
 *
 *************************************/

static READ16_HANDLER( smgp_excs_r )
{
	logerror("%06X:smgp_excs_r(%04X)\n", cpu_get_pc(space->cpu), offset*2);
	return 0xffff;
}


static WRITE16_HANDLER( smgp_excs_w )
{
	logerror("%06X:smgp_excs_w(%04X) = %04X & %04X\n", cpu_get_pc(space->cpu), offset*2, data, mem_mask);
}



/*************************************
 *
 *  Capacitor-backed RAM
 *
 *************************************/

static NVRAM_HANDLER( xboard )
{
	if (read_or_write)
	{
		mame_fwrite(file, backupram1, 0x4000);
		mame_fwrite(file, backupram2, 0x4000);
	}
	else if (file)
	{
		mame_fread(file, backupram1, 0x4000);
		mame_fread(file, backupram2, 0x4000);
	}
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0x3fffff)
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x080000, 0x083fff) AM_MIRROR(0x01c000) AM_RAM AM_SHARE(1) AM_BASE(&backupram1)
	AM_RANGE(0x0a0000, 0x0a3fff) AM_MIRROR(0x01c000) AM_RAM AM_SHARE(2) AM_BASE(&backupram2)
	AM_RANGE(0x0c0000, 0x0cffff) AM_RAM_WRITE(segaic16_tileram_0_w) AM_BASE(&segaic16_tileram_0)
	AM_RANGE(0x0d0000, 0x0d0fff) AM_MIRROR(0x00f000) AM_RAM_WRITE(segaic16_textram_0_w) AM_BASE(&segaic16_textram_0)
	AM_RANGE(0x0e0000, 0x0e0007) AM_MIRROR(0x003ff8) AM_READWRITE(segaic16_multiply_0_r, segaic16_multiply_0_w)
	AM_RANGE(0x0e4000, 0x0e401f) AM_MIRROR(0x003fe0) AM_READWRITE(segaic16_divide_0_r, segaic16_divide_0_w)
	AM_RANGE(0x0e8000, 0x0e801f) AM_MIRROR(0x003fe0) AM_READWRITE(segaic16_compare_timer_0_r, segaic16_compare_timer_0_w)
	AM_RANGE(0x100000, 0x100fff) AM_MIRROR(0x00f000) AM_RAM AM_BASE(&segaic16_spriteram_0)
	AM_RANGE(0x110000, 0x11ffff) AM_WRITE(segaic16_sprites_draw_0_w)
	AM_RANGE(0x120000, 0x123fff) AM_MIRROR(0x00c000) AM_RAM_WRITE(segaic16_paletteram_w) AM_BASE(&paletteram16)
	AM_RANGE(0x130000, 0x13ffff) AM_READWRITE(adc_r, adc_w)
	AM_RANGE(0x140000, 0x14000f) AM_MIRROR(0x00fff0) AM_READWRITE(iochip_0_r, iochip_0_w)
	AM_RANGE(0x150000, 0x15000f) AM_MIRROR(0x00fff0) AM_READWRITE(iochip_1_r, iochip_1_w)
	AM_RANGE(0x160000, 0x16ffff) AM_WRITE(iocontrol_w)
	AM_RANGE(0x200000, 0x27ffff) AM_ROM AM_REGION("sub", 0x00000)
	AM_RANGE(0x280000, 0x283fff) AM_MIRROR(0x01c000) AM_RAM AM_SHARE(3)
	AM_RANGE(0x2a0000, 0x2a3fff) AM_MIRROR(0x01c000) AM_RAM AM_SHARE(4)
	AM_RANGE(0x2e0000, 0x2e0007) AM_MIRROR(0x003ff8) AM_READWRITE(segaic16_multiply_1_r, segaic16_multiply_1_w)
	AM_RANGE(0x2e4000, 0x2e401f) AM_MIRROR(0x003fe0) AM_READWRITE(segaic16_divide_1_r, segaic16_divide_1_w)
	AM_RANGE(0x2e8000, 0x2e800f) AM_MIRROR(0x003ff0) AM_READWRITE(segaic16_compare_timer_1_r, segaic16_compare_timer_1_w)
	AM_RANGE(0x2ec000, 0x2ecfff) AM_MIRROR(0x001000) AM_RAM AM_SHARE(5) AM_BASE(&segaic16_roadram_0)
	AM_RANGE(0x2ee000, 0x2effff) AM_READWRITE(segaic16_road_control_0_r, segaic16_road_control_0_w)
//  AM_RANGE(0x2f0000, 0x2f3fff) AM_READWRITE(excs_r, excs_w)
	AM_RANGE(0x3f8000, 0x3fbfff) AM_RAM AM_SHARE(1)
	AM_RANGE(0x3fc000, 0x3fffff) AM_RAM AM_SHARE(2)
ADDRESS_MAP_END



/*************************************
 *
 *  Sub CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sub_map, ADDRESS_SPACE_PROGRAM, 16 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xfffff)
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x080000, 0x083fff) AM_MIRROR(0x01c000) AM_RAM AM_SHARE(3)
	AM_RANGE(0x0a0000, 0x0a3fff) AM_MIRROR(0x01c000) AM_RAM AM_SHARE(4)
	AM_RANGE(0x0e0000, 0x0e0007) AM_MIRROR(0x003ff8) AM_READWRITE(segaic16_multiply_1_r, segaic16_multiply_1_w)
	AM_RANGE(0x0e4000, 0x0e401f) AM_MIRROR(0x003fe0) AM_READWRITE(segaic16_divide_1_r, segaic16_divide_1_w)
	AM_RANGE(0x0e8000, 0x0e800f) AM_MIRROR(0x003ff0) AM_READWRITE(segaic16_compare_timer_1_r, segaic16_compare_timer_1_w)
	AM_RANGE(0x0ec000, 0x0ecfff) AM_MIRROR(0x001000) AM_RAM AM_SHARE(5)
	AM_RANGE(0x0ee000, 0x0effff) AM_READWRITE(segaic16_road_control_0_r, segaic16_road_control_0_w)
//  AM_RANGE(0x0f0000, 0x0f3fff) AM_READWRITE(excs_r, excs_w)
ADDRESS_MAP_END



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0xefff) AM_ROM
	AM_RANGE(0xf000, 0xf0ff) AM_MIRROR(0x0700) AM_READWRITE(sega_pcm_r, sega_pcm_w)
	AM_RANGE(0xf800, 0xffff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_MIRROR(0x3e) AM_WRITE(ym2151_register_port_0_w)
	AM_RANGE(0x01, 0x01) AM_MIRROR(0x3e) AM_READWRITE(ym2151_status_port_0_r, ym2151_data_port_0_w)
	AM_RANGE(0x40, 0x40) AM_MIRROR(0x3f) AM_READ(sound_data_r)
ADDRESS_MAP_END



/*************************************
 *
 *  Misc CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( smgp_comm_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x3fff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( smgp_comm_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
ADDRESS_MAP_END



/*************************************
 *
 *  Generic port definitions
 *
 *************************************/

/*
    aburner chip 0, port A: motor status (R)
            chip 0, port B: motor power (W)
            chip 0, port C: unknown (W)
            chip 0, port D: lamp (W)
            chip 1, port A: buttons
            chip 1, port B: ---
            chip 2, port C: DIPs
            chip 3, port D: DIPs
*/

static INPUT_PORTS_START( xboard_generic )
	PORT_START("IO0PORTA")
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SPECIAL )	/* /INTR of ADC0804 */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IO0PORTB")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IO1PORTA")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* button? not used by any game we have */
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* cannon trigger or shift down */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* missile button or shift up */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("IO1PORTB")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IO1PORTC")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SWA:1,2,3,4")
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, "6 Coins/4 Credits" )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, "5 Coins/6 Credits" )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_5C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SWA:5,6,7,8")
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x50, "6 Coins/4 Credits" )
	PORT_DIPSETTING(    0x40, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, "5 Coins/6 Credits" )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_5C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )

	PORT_START("IO1PORTD")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) ) PORT_DIPLOCATION("SWB:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



/*************************************
 *
 *  Game-specific port definitions
 *
 *************************************/

static INPUT_PORTS_START( aburner )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Vulcan")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Missile")

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x03, 0x01, "Cabinet Type" ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving Deluxe" )
	PORT_DIPSETTING(    0x02, "Moving Standard" )
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	// According to the manual, SWB:4 sets 3 or 4 lives, but it doesn't actually do that.
	// However, it does on Afterburner II.  Maybe there's another version of Afterburner
	// that behaves as the manual suggests.
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Lives ) ) PORT_DIPLOCATION("SWB:5")
	PORT_DIPSETTING(    0x10, "3" )
	PORT_DIPSETTING(    0x00, "3x Credits" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START("ADC0")	/* stick X */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START("ADC1")	/* stick Y */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_MINMAX(0x40,0xc0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC2")	/* throttle */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Z ) PORT_SENSITIVITY(100) PORT_KEYDELTA(79)

	PORT_START("ADC3")	/* motor Y */
	PORT_BIT( 0xff, (0xb0+0x50)/2, IPT_SPECIAL )

	PORT_START("ADC4")	/* motor X */
	PORT_BIT( 0xff, (0xb0+0x50)/2, IPT_SPECIAL )
INPUT_PORTS_END


static INPUT_PORTS_START( aburner2 )
	PORT_INCLUDE( aburner )

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x03, 0x01, "Cabinet Type" ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Moving Deluxe" )
	PORT_DIPSETTING(    0x02, "Moving Standard" )
	PORT_DIPSETTING(    0x01, "Upright 1" )
	PORT_DIPSETTING(    0x00, "Upright 2" )
	PORT_DIPNAME( 0x04, 0x04, "Throttle Lever" ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Lives ) ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x18, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x08, "3x Credits" )
	PORT_DIPSETTING(    0x00, "4x Credits" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( thndrbld )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Cannon")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Missile")

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x01, 0x01, "Cabinet Type" ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, "Econ Upright" )
	PORT_DIPSETTING(    0x00, "Mini Upright" )	// see note about inputs below
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "Time" ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, "30 sec" )
	PORT_DIPSETTING(    0x00, "0 sec" )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Lives ) ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x18, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	/*  These inputs are valid for the "Econ Upright" and "Deluxe" cabinets.
        On the "Standing" cabinet, the joystick Y axis is reversed.
        On the "Mini Upright" cabinet, the inputs conform to After Burner II:
        the X axis is (un-)reversed, and the throttle and Y axis switch places */
	PORT_START("ADC0")	/* stick X */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC1")	/* "slottle" */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Z ) PORT_SENSITIVITY(100) PORT_KEYDELTA(79)

	PORT_START("ADC2")	/* stick Y */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)
INPUT_PORTS_END


static INPUT_PORTS_START( thndrbd1 )
	PORT_INCLUDE( thndrbld )

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x01, 0x01, "Cabinet Type" ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, "Deluxe" )
	PORT_DIPSETTING(    0x00, "Standing" )	// see note about inputs above
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "Time" ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, "30 sec" )
	PORT_DIPSETTING(    0x00, "0 sec" )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Lives ) ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x18, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( loffire )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x30, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("IO1PORTB")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Language ) ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Japanese ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, "Cockpit" )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPNAME( 0x04, 0x04, "2 Credits to Start" ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:4,5")
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x18, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:6")
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Coin Chute" ) PORT_DIPLOCATION("SWB:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x00, "Twin" )

	PORT_START("ADC0")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ADC1")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(5)

	PORT_START("ADC2")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)

	PORT_START("ADC3")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(5) PORT_PLAYER(2)
INPUT_PORTS_END


static INPUT_PORTS_START( rachero )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Move to Center")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Suicide")

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x01, 0x01, "Credits" ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, "1 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x00, "2 to Start, 1 to Continue" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x30, 0x30, "Time" )  PORT_DIPLOCATION("SWB:5,6")
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Hard ) )

	PORT_START("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END


static INPUT_PORTS_START( smgp )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Shift Down") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Shift Up") PORT_CODE(KEYCODE_Z)

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x07, 0x07, "Machine ID" ) PORT_DIPLOCATION("SWB:1,2,3")
	PORT_DIPSETTING(    0x07, "1" )
	PORT_DIPSETTING(    0x06, "2" )
	PORT_DIPSETTING(    0x05, "3" )
	PORT_DIPSETTING(    0x04, "4" )
	PORT_DIPSETTING(    0x03, "5" )
	PORT_DIPSETTING(    0x02, "6" )
	PORT_DIPSETTING(    0x01, "7" )
	PORT_DIPSETTING(    0x00, "8" )
	PORT_DIPNAME( 0x38, 0x38, "Number of Machines" ) PORT_DIPLOCATION("SWB:4,5,6")
	PORT_DIPSETTING(    0x38, "1" )
	PORT_DIPSETTING(    0x30, "2" )
	PORT_DIPSETTING(    0x28, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPSETTING(    0x18, "5" )
	PORT_DIPSETTING(    0x10, "6" )
	PORT_DIPSETTING(    0x08, "7" )
	PORT_DIPSETTING(    0x00, "8" )
	PORT_DIPNAME( 0xc0, 0x40, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0xc0, "Deluxe" )
	PORT_DIPSETTING(    0x80, "Cockpit" )
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )
//  PORT_DIPSETTING(    0x00, "Deluxe" )

	PORT_START("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x38,0xc8) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x38, IPT_PEDAL ) PORT_MINMAX(0x38,0xb8) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x28, IPT_PEDAL2 ) PORT_MINMAX(0x28,0xa8) PORT_SENSITIVITY(100) PORT_KEYDELTA(40)
INPUT_PORTS_END


static INPUT_PORTS_START( abcop )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Jump")

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x01, 0x01, "Credits" ) PORT_DIPLOCATION("SWB:1")
	PORT_DIPSETTING(    0x01, "1 to Start, 1 to Continue" )
	PORT_DIPSETTING(    0x00, "2 to Start, 1 to Continue" )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SWB:3")
	PORT_DIPSETTING(    0x04, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x30, 0x30, "Time" ) PORT_DIPLOCATION("SWB:5,6")
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x20,0xe0) PORT_SENSITIVITY(100) PORT_KEYDELTA(4) PORT_REVERSE

	PORT_START("ADC1")	/* accelerator */
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
INPUT_PORTS_END


static INPUT_PORTS_START( gprider )
	PORT_INCLUDE( xboard_generic )

	PORT_MODIFY("IO1PORTA")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Shift Down") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Shift Up") PORT_CODE(KEYCODE_Z)

	PORT_MODIFY("IO1PORTD")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SWB:1,2")
	PORT_DIPSETTING(    0x03, "Ride On" )
	PORT_DIPSETTING(    0x02, DEF_STR( Upright ) )
//  PORT_DIPSETTING(    0x01, DEF_STR( Unused ) )
//  PORT_DIPSETTING(    0x00, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x08, 0x08, "ID No." ) PORT_DIPLOCATION("SWB:4")
	PORT_DIPSETTING(    0x08, "Main" )
	PORT_DIPSETTING(    0x00, "Slave" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SWB:5")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SWB:7,8")
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )

	PORT_START("ADC0")	/* steering */
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x01,0xff) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)

	PORT_START("ADC1")	/* gas pedal */
	PORT_BIT( 0xff, 0x10, IPT_PEDAL ) PORT_MINMAX(0x10,0xef) PORT_SENSITIVITY(100) PORT_KEYDELTA(20) PORT_REVERSE

	PORT_START("ADC2")	/* brake */
	PORT_BIT( 0xff, 0x10, IPT_PEDAL2 ) PORT_MINMAX(0x10,0xef) PORT_SENSITIVITY(100) PORT_KEYDELTA(40) PORT_REVERSE
INPUT_PORTS_END



/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static const ym2151_interface ym2151_config =
{
	sound_cpu_irq
};


static const sega_pcm_interface segapcm_interface =
{
	BANK_512
};



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static GFXDECODE_START( segaxbd )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,	0, 1024 )
GFXDECODE_END



/*************************************
 *
 *  Generic machine drivers
 *
 *************************************/

static MACHINE_DRIVER_START( xboard )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(main_map,0)

	MDRV_CPU_ADD("sub", M68000, MASTER_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(sub_map,0)

	MDRV_CPU_ADD("sound", Z80, SOUND_CLOCK/4)
	MDRV_CPU_PROGRAM_MAP(sound_map,0)
	MDRV_CPU_IO_MAP(sound_portmap,0)

	MDRV_MACHINE_RESET(xboard)
	MDRV_NVRAM_HANDLER(xboard)
	MDRV_INTERLEAVE(100)

	/* video hardware */
	MDRV_GFXDECODE(segaxbd)
	MDRV_PALETTE_LENGTH(8192*3)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(MASTER_CLOCK/8, 400, 0, 320, 262, 0, 224)

	MDRV_VIDEO_START(xboard)
	MDRV_VIDEO_UPDATE(xboard)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD("2151", YM2151, SOUND_CLOCK/4)
	MDRV_SOUND_CONFIG(ym2151_config)
	MDRV_SOUND_ROUTE(0, "left", 0.43)
	MDRV_SOUND_ROUTE(1, "right", 0.43)

	MDRV_SOUND_ADD("pcm", SEGAPCM, SOUND_CLOCK/4)
	MDRV_SOUND_CONFIG(segapcm_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.0)
	MDRV_SOUND_ROUTE(1, "right", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( smgp )
	MDRV_IMPORT_FROM(xboard)

	MDRV_CPU_ADD("comm", Z80, 4000000)
	MDRV_CPU_PROGRAM_MAP(smgp_comm_map,0)
	MDRV_CPU_IO_MAP(smgp_comm_portmap,0)
MACHINE_DRIVER_END



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Afterburner, Sega X-board
    CPU: 68000 (317-????)

    Missing the Deluxe version rom set
*/
ROM_START( aburner )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr10940.bin", 0x00000, 0x20000, CRC(4d132c4e) SHA1(007af52167c369177b86fc0f8b007ebceba2a30c) )
	ROM_LOAD16_BYTE( "epr10941.bin", 0x00001, 0x20000, CRC(136ea264) SHA1(606ac67db53a6002ed1bd71287aed2e3e720cdf4) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr10927.bin", 0x00000, 0x20000, CRC(66d36757) SHA1(c7f6d653fb6bfd629bb62057010d41f3ccfccc4d) )
	ROM_LOAD16_BYTE( "epr10928.bin", 0x00001, 0x20000, CRC(7c01d40b) SHA1(d95b4702a9c813db8bc24c8cd7e0933cbe54a573) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr10926.bin", 0x00000, 0x10000, CRC(ed8bd632) SHA1(d5bbd5e257ebef8cfb3baf5fa530b189d9cddb57) )
	ROM_LOAD( "epr10925.bin", 0x10000, 0x10000, CRC(4ef048cc) SHA1(3b386b3bfa600f114dbc19796bb6864a88ff4562) )
	ROM_LOAD( "epr10924.bin", 0x20000, 0x10000, CRC(50c15a6d) SHA1(fc202cc583fc6804647abc884fdf332e72ea3100) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "10932.125",    0x000000, 0x20000, CRC(cc0821d6) SHA1(22e84419a585209bbda1466d2180504c316a9b7f) )
	ROM_LOAD32_BYTE( "10934.129",    0x000001, 0x20000, CRC(4a51b1fa) SHA1(2eed018a5a1e935bb72b6f440a794466a1397dc5) )
	ROM_LOAD32_BYTE( "10936.133",    0x000002, 0x20000, CRC(ada70d64) SHA1(ba6203b0fdb4c4998b7be5b446eb8354751d553a) )
	ROM_LOAD32_BYTE( "10938.102",    0x000003, 0x20000, CRC(e7675baf) SHA1(aa979319a44c0b18c462afb5ca9cdeed2292c76a) )
	ROM_LOAD32_BYTE( "10933.126",    0x080000, 0x20000, CRC(c8efb2c3) SHA1(ba31da93f929f2c457e60b2099d5a1ba6b5a9f48) )
	ROM_LOAD32_BYTE( "10935.130",    0x080001, 0x20000, CRC(c1e23521) SHA1(5e95f3b6ff9f4caca676eaa6c84f1200315218ea) )
	ROM_LOAD32_BYTE( "10937.134",    0x080002, 0x20000, CRC(f0199658) SHA1(cd67504fef53f637a3b1c723c4a04148f88028d2) )
	ROM_LOAD32_BYTE( "10939.103",    0x080003, 0x20000, CRC(a0d49480) SHA1(6c4234456bc09ae771beec284d7aa21ebe474f6f) )
	ROM_LOAD32_BYTE( "epr10942.bin", 0x100000, 0x20000, CRC(5ce10b8c) SHA1(c6c189143762b0ef473d5d31d66226820c5cf080) )
	ROM_LOAD32_BYTE( "epr10943.bin", 0x100001, 0x20000, CRC(b98294dc) SHA1(a4161af23f9a67b4ed81308c73e72e1797cce894) )
	ROM_LOAD32_BYTE( "epr10944.bin", 0x100002, 0x20000, CRC(17be8f67) SHA1(371f0dd1914a98695cb86f921fe8e82b49e69a4a) )
	ROM_LOAD32_BYTE( "epr10945.bin", 0x100003, 0x20000, CRC(df4d4c4f) SHA1(24075a6709869d9acf9082b6b4ad96bc6f8b1932) )
	ROM_LOAD32_BYTE( "epr10946.bin", 0x180000, 0x20000, CRC(d7d485f4) SHA1(d843aefb4d99e0dff8d62ee6bd0c3aa6aa6c941b) )
	ROM_LOAD32_BYTE( "epr10947.bin", 0x180001, 0x20000, CRC(08838392) SHA1(84f7ff3bff31c0738dead7bc00219ede834eb0e0) )
	ROM_LOAD32_BYTE( "epr10948.bin", 0x180002, 0x20000, CRC(64284761) SHA1(9594c671900f7f49d8fb965bc17b4380ce2c68d5) )
	ROM_LOAD32_BYTE( "epr10949.bin", 0x180003, 0x20000, CRC(d8437d92) SHA1(480291358c3d197645d7bd149bdfe5d41071d52d) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	ROM_LOAD( "epr-10922.ic40", 0x000000, 0x10000, CRC(b49183d4) SHA1(71d87bfbce858049ccde9597ab15575b3cdba892) )

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-10923.ic17",0x00000, 0x10000, CRC(6888eb8f) SHA1(8f8fffb214842a5d356e33f5a97099bc6407384f) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "10931.11",    0x00000, 0x20000, CRC(9209068f) SHA1(01f3dda1c066d00080c55f2c86c506b6b2407f98) )
	ROM_LOAD( "10930.12",    0x20000, 0x20000, CRC(6493368b) SHA1(328aff19ff1d1344e9115f519d3962390c4e5ba4) )
	ROM_LOAD( "11102.13",    0x40000, 0x20000, CRC(6c07c78d) SHA1(3868b1824f43e4f2b4fbcd9274bfb3000c889d12) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Afterburner II, Sega X-board
    CPU: 68000 (317-????)
*/
ROM_START( aburner2 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "11107.58",  0x00000, 0x20000, CRC(6d87bab7) SHA1(ab34fe78f1f216037b3e3dca3e61f1b31c05cedf) )
	ROM_LOAD16_BYTE( "11108.104", 0x00001, 0x20000, CRC(202a3e1d) SHA1(cf2018bbad366de4b222eae35942636ca68aa581) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "11109.20", 0x00000, 0x20000, CRC(85a0fe07) SHA1(5a3a8fda6cb4898cfece4ec865b81b9b60f9ad55) )
	ROM_LOAD16_BYTE( "11110.29", 0x00001, 0x20000, CRC(f3d6797c) SHA1(17487b89ddbfbcc32a0b52268259f1c8d10fd0b2) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "11115.154", 0x00000, 0x10000, CRC(e8e32921) SHA1(30a96e6b514a475c778296228ba5b6fb96b211b0) )
	ROM_LOAD( "11114.153", 0x10000, 0x10000, CRC(2e97f633) SHA1(074125c106dd00785903b2e10cd7e28d5036eb60) )
	ROM_LOAD( "11113.152", 0x20000, 0x10000, CRC(36058c8c) SHA1(52befe6c6c53f10b6fd4971098abc8f8d3eef9d4) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "10932.125", 0x000000, 0x20000, CRC(cc0821d6) SHA1(22e84419a585209bbda1466d2180504c316a9b7f) )
	ROM_LOAD32_BYTE( "10934.129", 0x000001, 0x20000, CRC(4a51b1fa) SHA1(2eed018a5a1e935bb72b6f440a794466a1397dc5) )
	ROM_LOAD32_BYTE( "10936.133", 0x000002, 0x20000, CRC(ada70d64) SHA1(ba6203b0fdb4c4998b7be5b446eb8354751d553a) )
	ROM_LOAD32_BYTE( "10938.102", 0x000003, 0x20000, CRC(e7675baf) SHA1(aa979319a44c0b18c462afb5ca9cdeed2292c76a) )
	ROM_LOAD32_BYTE( "10933.126", 0x080000, 0x20000, CRC(c8efb2c3) SHA1(ba31da93f929f2c457e60b2099d5a1ba6b5a9f48) )
	ROM_LOAD32_BYTE( "10935.130", 0x080001, 0x20000, CRC(c1e23521) SHA1(5e95f3b6ff9f4caca676eaa6c84f1200315218ea) )
	ROM_LOAD32_BYTE( "10937.134", 0x080002, 0x20000, CRC(f0199658) SHA1(cd67504fef53f637a3b1c723c4a04148f88028d2) )
	ROM_LOAD32_BYTE( "10939.103", 0x080003, 0x20000, CRC(a0d49480) SHA1(6c4234456bc09ae771beec284d7aa21ebe474f6f) )
	ROM_LOAD32_BYTE( "11103.127", 0x100000, 0x20000, CRC(bdd60da2) SHA1(01673837c5ad84fa087728a05549ac01542ef4e9) )
	ROM_LOAD32_BYTE( "11104.131", 0x100001, 0x20000, CRC(06a35fce) SHA1(c39ae02fc8246e883c4f4c320f668ce6ca9c845a) )
	ROM_LOAD32_BYTE( "11105.135", 0x100002, 0x20000, CRC(027b0689) SHA1(c704c79faadb5e445fd3bd9281683b09831782d2) )
	ROM_LOAD32_BYTE( "11106.104", 0x100003, 0x20000, CRC(9e1fec09) SHA1(6cc47d86852b988bfcd64cb4ed7d832c683e3114) )
	ROM_LOAD32_BYTE( "11116.128", 0x180000, 0x20000, CRC(49b4c1ba) SHA1(5419f49f091e386eead4ccf5e03f12769e278179) )
	ROM_LOAD32_BYTE( "11117.132", 0x180001, 0x20000, CRC(821fbb71) SHA1(be2366d7b4a3a2543ba5024f0e258f1bc43caec8) )
	ROM_LOAD32_BYTE( "11118.136", 0x180002, 0x20000, CRC(8f38540b) SHA1(1fdfb157d1aca96cb635bd3d64f94545eb88c133) )
	ROM_LOAD32_BYTE( "11119.105", 0x180003, 0x20000, CRC(d0343a8e) SHA1(8c0c0addb6dfd0ea04c3900a9f7f7c731ca6e9ea) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	ROM_LOAD( "epr-10922.ic40", 0x000000, 0x10000, CRC(b49183d4) SHA1(71d87bfbce858049ccde9597ab15575b3cdba892) )

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-11112.ic17",    0x00000, 0x10000, CRC(d777fc6d) SHA1(46ce1c3875437044c0a172960d560d6acd6eaa92) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "10931.11",    0x00000, 0x20000, CRC(9209068f) SHA1(01f3dda1c066d00080c55f2c86c506b6b2407f98) )
	ROM_LOAD( "10930.12",    0x20000, 0x20000, CRC(6493368b) SHA1(328aff19ff1d1344e9115f519d3962390c4e5ba4) )
	ROM_LOAD( "11102.13",    0x40000, 0x20000, CRC(6c07c78d) SHA1(3868b1824f43e4f2b4fbcd9274bfb3000c889d12) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Line of Fire, Sega X-board
    CPU: FD1094 (317-0136)
*/
ROM_START( loffire )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12849.rom", 0x000000, 0x20000, CRC(61cfd2fe) SHA1(b47ae9cdf741574ab9128dd3556b1ef35e81a149) )
	ROM_LOAD16_BYTE( "epr12850.rom", 0x000001, 0x20000, CRC(14598f2a) SHA1(13a51529ed32acefd733d9f638621c3e023dbd6d) )

	/*
    It's not possible to determine the original value with just the available
    ROM data. The choice was between 47, 56 and 57, which decrypt correctly all
    the code at the affected addresses (2638, 6638 and so on).
    I chose 57 because it's the only one that has only 1 bit different from the
    bad value in the old dump (77).

    Nicola Salmoria
    */

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0136.key", 0x0000, 0x2000, BAD_DUMP CRC(344bfe0c) SHA1(f6bb8045b46f90f8abadf1dc2e1ae1d7cef9c810) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12804.rom", 0x000000, 0x20000, CRC(b853480e) SHA1(de0889e99251da7ea50316282ebf6f434cc2db11) )
	ROM_LOAD16_BYTE( "epr12805.rom", 0x000001, 0x20000, CRC(4a7200c3) SHA1(3e6febed36a55438e0d24441b68f2b7952791584) )
	ROM_LOAD16_BYTE( "epr12802.rom", 0x040000, 0x20000, CRC(d746bb39) SHA1(08dc8cf565997c7e52329961bf7a229a15900cff) )
	ROM_LOAD16_BYTE( "epr12803.rom", 0x040001, 0x20000, CRC(c1d9e751) SHA1(98b3d0b3b31702f6234b5fea2b82d512fc5d3ad2) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr12791.rom", 0x00000, 0x10000, CRC(acfa69ba) SHA1(353c43dda6c2282a785646b0a58c90cfd173cd7b) )
	ROM_LOAD( "opr12792.rom", 0x10000, 0x10000, CRC(e506723c) SHA1(d04dc29686fe348f8f715d14c027de0e508c770f) )
	ROM_LOAD( "opr12793.rom", 0x20000, 0x10000, CRC(0ce8cce3) SHA1(1a6b1af2b0b9e8240e681f7b15e9d08595753fe6) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr12787.rom", 0x000000, 0x20000, CRC(6431a3a6) SHA1(63a732b7dfd2b83fe7684d47fea26063c4ece099) )
	ROM_LOAD32_BYTE( "epr12788.rom", 0x000001, 0x20000, CRC(1982a0ce) SHA1(e4756f31b0094e0e9ddb2df53a5c938ac5559230) )
	ROM_LOAD32_BYTE( "epr12789.rom", 0x000002, 0x20000, CRC(97d03274) SHA1(b4b9921db53949bc8e91f8a2992e89c172fe8893) )
	ROM_LOAD32_BYTE( "epr12790.rom", 0x000003, 0x20000, CRC(816e76e6) SHA1(34d2a662af96f40f40a77497cbc0a3374fe9a34f) )
	ROM_LOAD32_BYTE( "epr12783.rom", 0x080000, 0x20000, CRC(c13feea9) SHA1(c0c3097903079deec22b0f8de76927f7570ac0f6) )
	ROM_LOAD32_BYTE( "epr12784.rom", 0x080001, 0x20000, CRC(39b94c65) SHA1(4deae3bf7bb4e04b011d23292a0c68471758e7ec) )
	ROM_LOAD32_BYTE( "epr12785.rom", 0x080002, 0x20000, CRC(05ed0059) SHA1(b7404a0f4f15ffdbd08673683cea22340de3f5f9) )
	ROM_LOAD32_BYTE( "epr12786.rom", 0x080003, 0x20000, CRC(a4123165) SHA1(024597dcfbd3be932626b84dbd6e7d38a7a0195d) )
	ROM_LOAD32_BYTE( "epr12779.rom", 0x100000, 0x20000, CRC(ae58af7c) SHA1(8c57f2d0b6584dd606afc5ecff039479e5068420) )
	ROM_LOAD32_BYTE( "epr12780.rom", 0x100001, 0x20000, CRC(ee670c1e) SHA1(8a9e0808d40e210abf6c49ef5c0774d8c0d6602b) )
	ROM_LOAD32_BYTE( "epr12781.rom", 0x100002, 0x20000, CRC(538f6bc5) SHA1(4f294ef0aa9c7e2ac7e92518d938f0870f2e46d1) )
	ROM_LOAD32_BYTE( "epr12782.rom", 0x100003, 0x20000, CRC(5acc34f7) SHA1(ef27ab818f50e59a122b9fc65b13442d9fee307c) )
	ROM_LOAD32_BYTE( "epr12775.rom", 0x180000, 0x20000, CRC(693056ec) SHA1(82d10d960441811b9369295bbb60fa7bfc5457a3) )
	ROM_LOAD32_BYTE( "epr12776.rom", 0x180001, 0x20000, CRC(61efbdfd) SHA1(67f267e0673c64ce77669826ea1d11cb79d0ccc1) )
	ROM_LOAD32_BYTE( "epr12777.rom", 0x180002, 0x20000, CRC(29d5b953) SHA1(0c932a67e2aecffa7a1dbaa587c96214e1a2cc7f) )
	ROM_LOAD32_BYTE( "epr12778.rom", 0x180003, 0x20000, CRC(2fb68e07) SHA1(8685e72aed115cbc9c6c7511217996a573b30d16) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12798.rom",	 0x00000, 0x10000, CRC(0587738d) SHA1(24c79b0c73616d5532a49a2c9121dfabe3a80c7d) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr12799.rom",    0x00000, 0x20000, CRC(bc60181c) SHA1(3c89161348db7cafb5636ab4eaba91fbd3541f90) )
	ROM_LOAD( "epr12800.rom",    0x20000, 0x20000, CRC(1158c1a3) SHA1(e1d664a203eed5a0130b39ced7bea8328f06f107) )
	ROM_LOAD( "epr12801.rom",    0x40000, 0x20000, CRC(2d6567c4) SHA1(542be9d8e91cf2df18d95f4e259cfda0560697cb) )
ROM_END

/**************************************************************************************************************************
    Line of Fire, Sega X-board
    CPU: FD1094 (317-0135)
*/
ROM_START( loffireu )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12847a.bin", 0x000000, 0x20000, CRC(c50eb4ed) SHA1(18a46c97aec2fefd160338c1760b6ee367dcb57f) )
	ROM_LOAD16_BYTE( "epr12848a.bin", 0x000001, 0x20000, CRC(f8ff8640) SHA1(193bb8f42f3c5011ad1fbf87215f012de5e950fb) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0135.key", 0x0000, 0x2000, CRC(c53ad019) SHA1(7e6dc2b35ebfeefb507d4d03f5a59574944177d1) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12804.rom", 0x000000, 0x20000, CRC(b853480e) SHA1(de0889e99251da7ea50316282ebf6f434cc2db11) )
	ROM_LOAD16_BYTE( "epr12805.rom", 0x000001, 0x20000, CRC(4a7200c3) SHA1(3e6febed36a55438e0d24441b68f2b7952791584) )
	ROM_LOAD16_BYTE( "epr12802.rom", 0x040000, 0x20000, CRC(d746bb39) SHA1(08dc8cf565997c7e52329961bf7a229a15900cff) )
	ROM_LOAD16_BYTE( "epr12803.rom", 0x040001, 0x20000, CRC(c1d9e751) SHA1(98b3d0b3b31702f6234b5fea2b82d512fc5d3ad2) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr12791.rom", 0x00000, 0x10000, CRC(acfa69ba) SHA1(353c43dda6c2282a785646b0a58c90cfd173cd7b) )
	ROM_LOAD( "opr12792.rom", 0x10000, 0x10000, CRC(e506723c) SHA1(d04dc29686fe348f8f715d14c027de0e508c770f) )
	ROM_LOAD( "opr12793.rom", 0x20000, 0x10000, CRC(0ce8cce3) SHA1(1a6b1af2b0b9e8240e681f7b15e9d08595753fe6) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr12787.rom", 0x000000, 0x20000, CRC(6431a3a6) SHA1(63a732b7dfd2b83fe7684d47fea26063c4ece099) )
	ROM_LOAD32_BYTE( "epr12788.rom", 0x000001, 0x20000, CRC(1982a0ce) SHA1(e4756f31b0094e0e9ddb2df53a5c938ac5559230) )
	ROM_LOAD32_BYTE( "epr12789.rom", 0x000002, 0x20000, CRC(97d03274) SHA1(b4b9921db53949bc8e91f8a2992e89c172fe8893) )
	ROM_LOAD32_BYTE( "epr12790.rom", 0x000003, 0x20000, CRC(816e76e6) SHA1(34d2a662af96f40f40a77497cbc0a3374fe9a34f) )
	ROM_LOAD32_BYTE( "epr12783.rom", 0x080000, 0x20000, CRC(c13feea9) SHA1(c0c3097903079deec22b0f8de76927f7570ac0f6) )
	ROM_LOAD32_BYTE( "epr12784.rom", 0x080001, 0x20000, CRC(39b94c65) SHA1(4deae3bf7bb4e04b011d23292a0c68471758e7ec) )
	ROM_LOAD32_BYTE( "epr12785.rom", 0x080002, 0x20000, CRC(05ed0059) SHA1(b7404a0f4f15ffdbd08673683cea22340de3f5f9) )
	ROM_LOAD32_BYTE( "epr12786.rom", 0x080003, 0x20000, CRC(a4123165) SHA1(024597dcfbd3be932626b84dbd6e7d38a7a0195d) )
	ROM_LOAD32_BYTE( "epr12779.rom", 0x100000, 0x20000, CRC(ae58af7c) SHA1(8c57f2d0b6584dd606afc5ecff039479e5068420) )
	ROM_LOAD32_BYTE( "epr12780.rom", 0x100001, 0x20000, CRC(ee670c1e) SHA1(8a9e0808d40e210abf6c49ef5c0774d8c0d6602b) )
	ROM_LOAD32_BYTE( "epr12781.rom", 0x100002, 0x20000, CRC(538f6bc5) SHA1(4f294ef0aa9c7e2ac7e92518d938f0870f2e46d1) )
	ROM_LOAD32_BYTE( "epr12782.rom", 0x100003, 0x20000, CRC(5acc34f7) SHA1(ef27ab818f50e59a122b9fc65b13442d9fee307c) )
	ROM_LOAD32_BYTE( "epr12775.rom", 0x180000, 0x20000, CRC(693056ec) SHA1(82d10d960441811b9369295bbb60fa7bfc5457a3) )
	ROM_LOAD32_BYTE( "epr12776.rom", 0x180001, 0x20000, CRC(61efbdfd) SHA1(67f267e0673c64ce77669826ea1d11cb79d0ccc1) )
	ROM_LOAD32_BYTE( "epr12777.rom", 0x180002, 0x20000, CRC(29d5b953) SHA1(0c932a67e2aecffa7a1dbaa587c96214e1a2cc7f) )
	ROM_LOAD32_BYTE( "epr12778.rom", 0x180003, 0x20000, CRC(2fb68e07) SHA1(8685e72aed115cbc9c6c7511217996a573b30d16) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12798.rom",	 0x00000, 0x10000, CRC(0587738d) SHA1(24c79b0c73616d5532a49a2c9121dfabe3a80c7d) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr12799.rom",    0x00000, 0x20000, CRC(bc60181c) SHA1(3c89161348db7cafb5636ab4eaba91fbd3541f90) )
	ROM_LOAD( "epr12800.rom",    0x20000, 0x20000, CRC(1158c1a3) SHA1(e1d664a203eed5a0130b39ced7bea8328f06f107) )
	ROM_LOAD( "epr12801.rom",    0x40000, 0x20000, CRC(2d6567c4) SHA1(542be9d8e91cf2df18d95f4e259cfda0560697cb) )
ROM_END

/**************************************************************************************************************************
    Line of Fire, Sega X-board
    CPU: FD1094 (317-0134)
*/
ROM_START( loffirej )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	/* repaired using data from the loffire set since they are mostly identical
       when decrypted, they pass the rom check so are assumed to be ok but double
       checking them when possible never hurts */
	ROM_LOAD16_BYTE( "epr12794.bin", 0x000000, 0x20000, CRC(1e588992) SHA1(fe7107e83c12643e7d22fd4b4cd0c7bcff0d84c3) )
	ROM_LOAD16_BYTE( "epr12795.bin", 0x000001, 0x20000, CRC(d43d7427) SHA1(ecbd425bab6aa65ffbd441d6a0936ac055d5f06d) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0134.key", 0x0000, 0x2000, CRC(732626d4) SHA1(75ed7ca417758dd62afb4edbb9daee754932c392) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12804.rom", 0x000000, 0x20000, CRC(b853480e) SHA1(de0889e99251da7ea50316282ebf6f434cc2db11) )
	ROM_LOAD16_BYTE( "epr12805.rom", 0x000001, 0x20000, CRC(4a7200c3) SHA1(3e6febed36a55438e0d24441b68f2b7952791584) )
	ROM_LOAD16_BYTE( "epr12802.rom", 0x040000, 0x20000, CRC(d746bb39) SHA1(08dc8cf565997c7e52329961bf7a229a15900cff) )
	ROM_LOAD16_BYTE( "epr12803.rom", 0x040001, 0x20000, CRC(c1d9e751) SHA1(98b3d0b3b31702f6234b5fea2b82d512fc5d3ad2) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr12791.rom", 0x00000, 0x10000, CRC(acfa69ba) SHA1(353c43dda6c2282a785646b0a58c90cfd173cd7b) )
	ROM_LOAD( "opr12792.rom", 0x10000, 0x10000, CRC(e506723c) SHA1(d04dc29686fe348f8f715d14c027de0e508c770f) )
	ROM_LOAD( "opr12793.rom", 0x20000, 0x10000, CRC(0ce8cce3) SHA1(1a6b1af2b0b9e8240e681f7b15e9d08595753fe6) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr12787.rom", 0x000000, 0x20000, CRC(6431a3a6) SHA1(63a732b7dfd2b83fe7684d47fea26063c4ece099) )
	ROM_LOAD32_BYTE( "epr12788.rom", 0x000001, 0x20000, CRC(1982a0ce) SHA1(e4756f31b0094e0e9ddb2df53a5c938ac5559230) )
	ROM_LOAD32_BYTE( "epr12789.rom", 0x000002, 0x20000, CRC(97d03274) SHA1(b4b9921db53949bc8e91f8a2992e89c172fe8893) )
	ROM_LOAD32_BYTE( "epr12790.rom", 0x000003, 0x20000, CRC(816e76e6) SHA1(34d2a662af96f40f40a77497cbc0a3374fe9a34f) )
	ROM_LOAD32_BYTE( "epr12783.rom", 0x080000, 0x20000, CRC(c13feea9) SHA1(c0c3097903079deec22b0f8de76927f7570ac0f6) )
	ROM_LOAD32_BYTE( "epr12784.rom", 0x080001, 0x20000, CRC(39b94c65) SHA1(4deae3bf7bb4e04b011d23292a0c68471758e7ec) )
	ROM_LOAD32_BYTE( "epr12785.rom", 0x080002, 0x20000, CRC(05ed0059) SHA1(b7404a0f4f15ffdbd08673683cea22340de3f5f9) )
	ROM_LOAD32_BYTE( "epr12786.rom", 0x080003, 0x20000, CRC(a4123165) SHA1(024597dcfbd3be932626b84dbd6e7d38a7a0195d) )
	ROM_LOAD32_BYTE( "epr12779.rom", 0x100000, 0x20000, CRC(ae58af7c) SHA1(8c57f2d0b6584dd606afc5ecff039479e5068420) )
	ROM_LOAD32_BYTE( "epr12780.rom", 0x100001, 0x20000, CRC(ee670c1e) SHA1(8a9e0808d40e210abf6c49ef5c0774d8c0d6602b) )
	ROM_LOAD32_BYTE( "epr12781.rom", 0x100002, 0x20000, CRC(538f6bc5) SHA1(4f294ef0aa9c7e2ac7e92518d938f0870f2e46d1) )
	ROM_LOAD32_BYTE( "epr12782.rom", 0x100003, 0x20000, CRC(5acc34f7) SHA1(ef27ab818f50e59a122b9fc65b13442d9fee307c) )
	ROM_LOAD32_BYTE( "epr12775.rom", 0x180000, 0x20000, CRC(693056ec) SHA1(82d10d960441811b9369295bbb60fa7bfc5457a3) )
	ROM_LOAD32_BYTE( "epr12776.rom", 0x180001, 0x20000, CRC(61efbdfd) SHA1(67f267e0673c64ce77669826ea1d11cb79d0ccc1) )
	ROM_LOAD32_BYTE( "epr12777.rom", 0x180002, 0x20000, CRC(29d5b953) SHA1(0c932a67e2aecffa7a1dbaa587c96214e1a2cc7f) )
	ROM_LOAD32_BYTE( "epr12778.rom", 0x180003, 0x20000, CRC(2fb68e07) SHA1(8685e72aed115cbc9c6c7511217996a573b30d16) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12798.rom",	 0x00000, 0x10000, CRC(0587738d) SHA1(24c79b0c73616d5532a49a2c9121dfabe3a80c7d) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr12799.rom",    0x00000, 0x20000, CRC(bc60181c) SHA1(3c89161348db7cafb5636ab4eaba91fbd3541f90) )
	ROM_LOAD( "epr12800.rom",    0x20000, 0x20000, CRC(1158c1a3) SHA1(e1d664a203eed5a0130b39ced7bea8328f06f107) )
	ROM_LOAD( "epr12801.rom",    0x40000, 0x20000, CRC(2d6567c4) SHA1(542be9d8e91cf2df18d95f4e259cfda0560697cb) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Thunder Blade, Sega X-board
    CPU: FD1094 (317-0056)

    GAME BD NO. 834-6493-03 (Uses "MPR" mask roms) or 834-6493-05 (Uses "EPR" eproms)
*/
ROM_START( thndrbld )
	ROM_REGION( 0x100000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-11405.ic58", 0x000000, 0x20000, CRC(e057dd5a) SHA1(4c032db4752dfb44dba3def5ee5377fffd94b79c) )
	ROM_LOAD16_BYTE( "epr-11406.ic63", 0x000001, 0x20000, CRC(c6b994b8) SHA1(098b2ae30c4aafea35222369d60f8e89f87639eb) )
	ROM_LOAD16_BYTE( "epr-11306.ic57", 0x040000, 0x20000, CRC(4b95f2b4) SHA1(9e0ff898a2af05c35db3551e52c7485748698c28) )
	ROM_LOAD16_BYTE( "epr-11307.ic62", 0x040001, 0x20000, CRC(2d6833e4) SHA1(b39a744370014237121f0010d18897e63f7058cf) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0056.key", 0x0000, 0x2000, CRC(b40cd2c5) SHA1(865e70bce4f55f6702960d6eaa780b7b1f880e41) )

	ROM_REGION( 0x100000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr-11390.ic20", 0x000000, 0x20000, CRC(ed988fdb) SHA1(b809b0b7dabd5cb29f5387522c6dfb993d1d0271) )
	ROM_LOAD16_BYTE( "epr-11391.ic29", 0x000001, 0x20000, CRC(12523bc1) SHA1(54635d6c4cc97cf4148dcac3bb2056fc414252f7) )
	ROM_LOAD16_BYTE( "epr-11310.ic21", 0x040000, 0x20000, CRC(5d9fa02c) SHA1(0ca71e35cf9740e38a52960f7d1ef96e7e1dda94) )
	ROM_LOAD16_BYTE( "epr-11311.ic30", 0x040001, 0x20000, CRC(483de21b) SHA1(871f0e856dcc81dcef1d9846261b3c011fa26dde) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr-11314.ic154", 0x00000, 0x10000, CRC(d4f954a9) SHA1(93ee8cf8fcf4e1d0dd58329bba9b594431193449) )
	ROM_LOAD( "epr-11315.ic153", 0x10000, 0x10000, CRC(35813088) SHA1(ea1ec982d1509efb26e7b6a150825a6a905efed9) )
	ROM_LOAD( "epr-11316.ic152", 0x20000, 0x10000, CRC(84290dff) SHA1(c13fb6ef12a991f79a95072f953a02b5c992aa2d) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr-11323.ic90",  0x000000, 0x20000, CRC(27e40735) SHA1(284ddb88efe741fb78199ea619c9b230ee689803) )
	ROM_LOAD32_BYTE( "epr-11322.ic94",  0x000001, 0x20000, CRC(10364d74) SHA1(393b19a972b5d8817ffd438f13ded73cd58ebe56) )
	ROM_LOAD32_BYTE( "epr-11321.ic98",  0x000002, 0x20000, CRC(8e738f58) SHA1(9f2dceebf01e582cf60f072ae411000d8503894b) )
	ROM_LOAD32_BYTE( "epr-11320.ic102", 0x000003, 0x20000, CRC(a95c76b8) SHA1(cda62f3c25b9414a523c2fc5d109031ed560069e) )
	ROM_LOAD32_BYTE( "epr-11327.ic91",  0x080000, 0x20000, CRC(deae90f1) SHA1(c73c23bab949041242302cec13d653dcc71bb944) )
	ROM_LOAD32_BYTE( "epr-11326.ic95",  0x080001, 0x20000, CRC(29198403) SHA1(3ecf315a0e6b3ed5005f8bdcb2e2a884c8b176c7) )
	ROM_LOAD32_BYTE( "epr-11325.ic99",  0x080002, 0x20000, CRC(b9e98ae9) SHA1(c4932e2590b10d54fa8ded94593dc4203fccc60d) )
	ROM_LOAD32_BYTE( "epr-11324.ic103", 0x080003, 0x20000, CRC(9742b552) SHA1(922032264d469e943dfbcaaf57464efc638fcf73) )
	ROM_LOAD32_BYTE( "epr-11331.ic92",  0x100000, 0x20000, CRC(3a2c042e) SHA1(c296ff222d156d3bdcb42bef321831f502830fd6) )
	ROM_LOAD32_BYTE( "epr-11330.ic96",  0x100001, 0x20000, CRC(aa7c70c5) SHA1(b6fea17392b7821b8b3bba78002f9c1604f09edc) )
	ROM_LOAD32_BYTE( "epr-11329.ic100", 0x100002, 0x20000, CRC(31b20257) SHA1(7ce10a94bce67b2d15d7b576b0f7d47389dc8948) )
	ROM_LOAD32_BYTE( "epr-11328.ic104", 0x100003, 0x20000, CRC(da39e89c) SHA1(526549ce9112754c82743552eeebec63fe7ad968) )
	ROM_LOAD32_BYTE( "epr-11395.ic93",  0x180000, 0x20000, CRC(90775579) SHA1(15a86071a105da40ec9c0c0074e342231fc030d0) ) //
	ROM_LOAD32_BYTE( "epr-11394.ic97",  0x180001, 0x20000, CRC(5f2783be) SHA1(424510153a91902901f321f39738a862d6fba8e7) ) // different numbers?
	ROM_LOAD32_BYTE( "epr-11393.ic101", 0x180002, 0x20000, CRC(525e2e1d) SHA1(6fd09f775e7e6cad8078513d1af0a8ff40fb1360) ) // replaced from original rev?
	ROM_LOAD32_BYTE( "epr-11392.ic105", 0x180003, 0x20000, CRC(b4a382f7) SHA1(c03a05ba521f654db1a9c5f5717b7a15e5a29d4e) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* Road Data */
	ROM_LOAD( "epr-11313.ic29", 0x00000, 0x10000, CRC(6a56c4c3) SHA1(c1b8023cb2ba4e96be052031c24b6ae424225c71) )

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-11396.ic17", 0x00000, 0x10000, CRC(d37b54a4) SHA1(c230fe7241a1f13ca13506d1492f348f506c40a7) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr-11317.ic11", 0x00000, 0x20000, CRC(d4e7ac1f) SHA1(ec5d6e4949938adf56e5613801ae56ff2c3dede5) )
	ROM_LOAD( "epr-11318.ic12", 0x20000, 0x20000, CRC(70d3f02c) SHA1(391aac2bc5673e06150de27e19c7c6359da8ca82) )
	ROM_LOAD( "epr-11319.ic13", 0x40000, 0x20000, CRC(50d9242e) SHA1(a106371bf680c3088ec61f07fc5c4ce467973c15) )
ROM_END

/**************************************************************************************************************************
    Thunder Blade (Japan), Sega X-board
    CPU: MC68000

    GAME BD NO. 834-6493-03 (Uses "MPR" mask roms) or 834-6493-05 (Uses "EPR" eproms)
*/
ROM_START( thndrbd1 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-11304.ic58", 0x000000, 0x20000, CRC(a90630ef) SHA1(8f29e020119b2243b1c95e15546af1773327ae85) ) // patched?
	ROM_LOAD16_BYTE( "epr-11305.ic63", 0x000001, 0x20000, CRC(9ba3ef61) SHA1(f75748b37ce35b0ef881804f73417643068dfbb2) ) // patched?
	ROM_LOAD16_BYTE( "epr-11306.ic57", 0x040000, 0x20000, CRC(4b95f2b4) SHA1(9e0ff898a2af05c35db3551e52c7485748698c28) )
	ROM_LOAD16_BYTE( "epr-11307.ic62", 0x040001, 0x20000, CRC(2d6833e4) SHA1(b39a744370014237121f0010d18897e63f7058cf) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr-11308.ic20", 0x00000, 0x20000, CRC(7956c238) SHA1(4608225cfd6ea3d38317cbe970f26a5fc2f8e320) )
	ROM_LOAD16_BYTE( "epr-11309.ic29", 0x00001, 0x20000, CRC(c887f620) SHA1(644c47cc2cf75cbe489ea084c13c59d94631e83f) )
	ROM_LOAD16_BYTE( "epr-11310.ic21", 0x040000, 0x20000, CRC(5d9fa02c) SHA1(0ca71e35cf9740e38a52960f7d1ef96e7e1dda94) )
	ROM_LOAD16_BYTE( "epr-11311.ic30", 0x040001, 0x20000, CRC(483de21b) SHA1(871f0e856dcc81dcef1d9846261b3c011fa26dde) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr-11314.ic154", 0x00000, 0x10000, CRC(d4f954a9) SHA1(93ee8cf8fcf4e1d0dd58329bba9b594431193449) )
	ROM_LOAD( "epr-11315.ic153", 0x10000, 0x10000, CRC(35813088) SHA1(ea1ec982d1509efb26e7b6a150825a6a905efed9) )
	ROM_LOAD( "epr-11316.ic152", 0x20000, 0x10000, CRC(84290dff) SHA1(c13fb6ef12a991f79a95072f953a02b5c992aa2d) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr-11323.ic90",  0x000000, 0x20000, CRC(27e40735) SHA1(284ddb88efe741fb78199ea619c9b230ee689803) )
	ROM_LOAD32_BYTE( "epr-11322.ic94",  0x000001, 0x20000, CRC(10364d74) SHA1(393b19a972b5d8817ffd438f13ded73cd58ebe56) )
	ROM_LOAD32_BYTE( "epr-11321.ic98",  0x000002, 0x20000, CRC(8e738f58) SHA1(9f2dceebf01e582cf60f072ae411000d8503894b) )
	ROM_LOAD32_BYTE( "epr-11320.ic102", 0x000003, 0x20000, CRC(a95c76b8) SHA1(cda62f3c25b9414a523c2fc5d109031ed560069e) )
	ROM_LOAD32_BYTE( "epr-11327.ic91",  0x080000, 0x20000, CRC(deae90f1) SHA1(c73c23bab949041242302cec13d653dcc71bb944) )
	ROM_LOAD32_BYTE( "epr-11326.ic95",  0x080001, 0x20000, CRC(29198403) SHA1(3ecf315a0e6b3ed5005f8bdcb2e2a884c8b176c7) )
	ROM_LOAD32_BYTE( "epr-11325.ic99",  0x080002, 0x20000, CRC(b9e98ae9) SHA1(c4932e2590b10d54fa8ded94593dc4203fccc60d) )
	ROM_LOAD32_BYTE( "epr-11324.ic103", 0x080003, 0x20000, CRC(9742b552) SHA1(922032264d469e943dfbcaaf57464efc638fcf73) )
	ROM_LOAD32_BYTE( "epr-11331.ic92",  0x100000, 0x20000, CRC(3a2c042e) SHA1(c296ff222d156d3bdcb42bef321831f502830fd6) )
	ROM_LOAD32_BYTE( "epr-11330.ic96",  0x100001, 0x20000, CRC(aa7c70c5) SHA1(b6fea17392b7821b8b3bba78002f9c1604f09edc) )
	ROM_LOAD32_BYTE( "epr-11329.ic100", 0x100002, 0x20000, CRC(31b20257) SHA1(7ce10a94bce67b2d15d7b576b0f7d47389dc8948) )
	ROM_LOAD32_BYTE( "epr-11328.ic104", 0x100003, 0x20000, CRC(da39e89c) SHA1(526549ce9112754c82743552eeebec63fe7ad968) )
	ROM_LOAD32_BYTE( "epr-11335.ic93",  0x180000, 0x20000, CRC(f19b3e86) SHA1(40e8ba10cd5020782b82279974d13330a9c015e5) )
	ROM_LOAD32_BYTE( "epr-11334.ic97",  0x180001, 0x20000, CRC(348f91c7) SHA1(03da6a4fee1fdea76058be4bc5ffcde7a79e5948) )
	ROM_LOAD32_BYTE( "epr-11333.ic101", 0x180002, 0x20000, CRC(05a2333f) SHA1(70f213945fa7fe056fe17a02558638e87f2c001e) )
	ROM_LOAD32_BYTE( "epr-11332.ic105", 0x180003, 0x20000, CRC(dc089ec6) SHA1(d72390c45138a507e79af112addbc015560fc248) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* Road Data */
	ROM_LOAD( "epr-11313.ic29", 0x00000, 0x10000, CRC(6a56c4c3) SHA1(c1b8023cb2ba4e96be052031c24b6ae424225c71) )

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-11312.ic17",   0x00000, 0x10000, CRC(3b974ed2) SHA1(cf18a2d0f01643c747a884bf00e5b7037ba2e64a) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr-11317.ic11", 0x00000, 0x20000, CRC(d4e7ac1f) SHA1(ec5d6e4949938adf56e5613801ae56ff2c3dede5) )
	ROM_LOAD( "epr-11318.ic12", 0x20000, 0x20000, CRC(70d3f02c) SHA1(391aac2bc5673e06150de27e19c7c6359da8ca82) )
	ROM_LOAD( "epr-11319.ic13", 0x40000, 0x20000, CRC(50d9242e) SHA1(a106371bf680c3088ec61f07fc5c4ce467973c15) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    Racing Hero, Sega X-board
    CPU: FD1094 (317-0144)
*/
ROM_START( rachero )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-13129.ic58", 0x00000, 0x20000,CRC(ad9f32e7) SHA1(dbcb3436782bee88dcac05d4f59c97f170a7387d) )
	ROM_LOAD16_BYTE( "epr-13130.ic63", 0x00001, 0x20000,CRC(6022777b) SHA1(965c76565d740be3355c4b403a1629cffb9fcd78) )
	ROM_LOAD16_BYTE( "epr-12855.ic57", 0x40000, 0x20000,CRC(cecf1e73) SHA1(3f8631379f32dbfda7720ef345276f9be23ada06) )
	ROM_LOAD16_BYTE( "epr-12856.ic62", 0x40001, 0x20000,CRC(da900ebb) SHA1(595ed65248185ddf8666b3f30ad6329162116448) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0144.key", 0x0000, 0x2000, CRC(8740bbff) SHA1(de96e606c04a09258b966532fb01a6b4d4db86a6) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr-12857.ic20", 0x00000, 0x20000, CRC(8a2328cc) SHA1(c34498428ddfb3eeb986f4153a6165a685d8fc8a) )
	ROM_LOAD16_BYTE( "epr-12858.ic29", 0x00001, 0x20000, CRC(38a248b7) SHA1(a17672123665403c1c56fedab6c8abf44b1131f9) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr-12879.ic154", 0x00000, 0x10000, CRC(c1a9de7a) SHA1(2425456a9d4ba92e1f2da6c2f164a6d5a5dee7c7) )
	ROM_LOAD( "epr-12880.ic153", 0x10000, 0x10000, CRC(27ff04a5) SHA1(b554a6e060f4803100be8efa52977b503eb0f31d) )
	ROM_LOAD( "epr-12881.ic152", 0x20000, 0x10000, CRC(72f14491) SHA1(b7a6cbd08470a5edda77cdd0337abd502c4905fd) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr-12872.ic90",  0x000000, 0x20000, CRC(68d56139) SHA1(b5f32edbda10c31d52f90defea2bae226676069f) )
	ROM_LOAD32_BYTE( "epr-12873.ic94",  0x000001, 0x20000, CRC(3d3ec450) SHA1(ac96ad8c7b365478bd1e5826a073e242f1208247) )
	ROM_LOAD32_BYTE( "epr-12874.ic98",  0x000002, 0x20000, CRC(7d6bde23) SHA1(88b12ec6386cdad60b0028b72033a0037a0cdbdb) )
	ROM_LOAD32_BYTE( "epr-12875.ic102", 0x000003, 0x20000, CRC(e33092bf) SHA1(31e211e25adac0a98befb459093f23c905fbc1e6) )
	ROM_LOAD32_BYTE( "epr-12868.ic91",  0x080000, 0x20000, CRC(96289583) SHA1(4d37e67860bc0e6ef69f0a0775c28f6f2fd6875e) )
	ROM_LOAD32_BYTE( "epr-12869.ic95",  0x080001, 0x20000, CRC(2ef0de02) SHA1(11ee3d77df2cddd3156da52e50565505f95f4cd4) )
	ROM_LOAD32_BYTE( "epr-12870.ic99",  0x080002, 0x20000, CRC(c76630e1) SHA1(7b76e4819990e147639d6b930b17b6fa10df191c) )
	ROM_LOAD32_BYTE( "epr-12871.ic103", 0x080003, 0x20000, CRC(23401b1a) SHA1(eaf465ffda84bdb83cc85daf781275bada396aab) )
	ROM_LOAD32_BYTE( "epr-12864.ic92",  0x100000, 0x20000, CRC(77d6cff4) SHA1(1e625204801d03369311844efb26d22216253ac4) )
	ROM_LOAD32_BYTE( "epr-12865.ic96",  0x100001, 0x20000, CRC(1e7e685b) SHA1(532fe361357383aa9dada833cbe31716c58001e5) )
	ROM_LOAD32_BYTE( "epr-12866.ic100", 0x100002, 0x20000, CRC(fdf31329) SHA1(9c229a0f9d8b8114acfe4f17b45a9b8640560b3e) )
	ROM_LOAD32_BYTE( "epr-12867.ic104", 0x100003, 0x20000, CRC(b25e37fd) SHA1(fef5bfe4690b3203b83fd565d883b2c63f439633) )
	ROM_LOAD32_BYTE( "epr-12860.ic93",  0x180000, 0x20000, CRC(86b64119) SHA1(d39aedad0f05e500e33af888126bd2fc22539141) )
	ROM_LOAD32_BYTE( "epr-12861.ic97",  0x180001, 0x20000, CRC(bccff19b) SHA1(32c3f7802a12be02a114b78cd898c46fcb1c0a61) )
	ROM_LOAD32_BYTE( "epr-12862.ic101", 0x180002, 0x20000, CRC(7d4c3b05) SHA1(4e25a077b403549c681c5047912d0e28f4c07720) )
	ROM_LOAD32_BYTE( "epr-12863.ic105", 0x180003, 0x20000, CRC(85095053) SHA1(f93194ecc0300956280cc0515b3e3ba2c9f71364) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* ground data */
	/* none */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-12859.ic17",    0x00000, 0x10000, CRC(d57881da) SHA1(75b7f331ea8c2e33d6236e0c8fc8dabe5eef8160) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr-12876.ic11",    0x00000, 0x20000, CRC(f72a34a0) SHA1(28f7d077c24352557da3a91a7e49b0c5b79f2a2e) )
	ROM_LOAD( "epr-12877.ic12",    0x20000, 0x20000, CRC(18c1b6d2) SHA1(860cbb96999ab76c40ce96996bba70c42d845abc) )
	ROM_LOAD( "epr-12878.ic13",    0x40000, 0x20000, CRC(7c212c15) SHA1(360b332d2fb32d88949ff8b357a863ffaaca39c2) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************

Super Monaco GP
Sega, 1989

This game runs on Sega X-Board hardware.

PCB Layouts
-----------

Main Board

Top    : 834-6335
Bottom : 171-5494
Sticker: 834-7088-01 REV. B  SUPER MONACO GP
|-----------------------------------------------------------------------------|
|IC67 IC66 IC65 IC64 IC58 IC57 IC56 IC55             16MHz                    |
|IC71 IC70 IC69 IC68                      IC107  IC15   IC11    IC8           |
|IC75 IC74 IC73 IC72                             IC16   IC12                  |
|IC79 IC78 IC77 IC76 IC63 IC62 IC61 IC60  IC108  IC17   IC13   IC10 IC9   IC3 |
|                                                IC18   IC14                  |
|                                                                             |
|                                                                             |
|     IC84              IC81                                                  |
|                                                                IC23         |
|                                                                IC22         |
|                                       IC109                        IC21 IC20|
|                     IC125      IC118              IC28             IC30 IC29|
|IC93 IC92 IC91 IC90  IC126                                      IC31         |
|                                             IC53               IC32         |
|                     IC32                               IC40  IC38           |
|                     IC33                                     IC39           |
|IC97 IC96 IC95 IC94        IC127       IC117                                 |
|                     IC134                                                   |
|                     IC135                   50MHz                           |
|                                                                     IC37    |
|IC101 IC100 IC99 IC98  IC148                                                 |
|                                        IC165                                |
|                                                              IC42           |
|                                 IC150                               IC41    |
|IC105 IC104 IC103 IC102                                                      |
|                    IC154  IC152                   IC160   IC159   DSWB  DSWA|
|                       IC153     IC151  IC149                                |
|-----------------------------------------------------------------------------|
Notes:
      ROMs:
           IC58 : EPR12561A.58 (27C010 EPROM)
           IC57 : not populated
           IC63 : EPR12562A.63 (27C010 EPROM)
           IC62 : not populated

           IC11 : MPR12437.11  (831000 MASKROM)
           IC12 : MPR12438.12  (831000 MASKROM)
           IC13 : MPR12439.13  (831000 MASKROM)
           IC17 : EPR12436.17  (27C512 EPROM)

           IC21 : not populated
           IC20 : EPR12574A.20 (27C010 EPROM)
           IC30 : not populated
           IC29 : EPR12575A.29 (27C010 EPROM)

           IC40 : not populated

           IC90 : MPR12425.90  (831000 MASKROM)
           IC91 : MPR12421.91  (831000 MASKROM)
           IC92 : MPR12417.92  (831000 MASKROM)
           IC93 : EPR12609.93  (27C010 EPROM)

           IC94 : MPR12426.94  (831000 MASKROM)
           IC95 : MPR12422.95  (831000 MASKROM)
           IC96 : MPR12418.96  (831000 MASKROM)
           IC97 : EPR12610.97  (27C010 EPROM)

           IC98 : MPR12427.98  (831000 MASKROM)
           IC99 : MPR12423.99  (831000 MASKROM)
           IC100: MPR12419.100 (831000 MASKROM)
           IC101: EPR12611.101 (27C010 EPROM)

           IC102: MPR12428.102 (831000 MASKROM)
           IC103: MPR12424.103 (831000 MASKROM)
           IC104: MPR12420.104 (831000 MASKROM)
           IC105: EPR12612.105 (27C010 EPROM)

           IC154: EPR12429.154 (27C512 EPROM)
           IC153: EPR12430.153 (27C512 EPROM)
           IC152: EPR12431.152 (27C512 EPROM)

      PALs:
           IC18 : 315-5280
           IC84 : 315-5278
           IC109: 315-5290
           IC117: 315-5291
           IC127: 315-5304

      RAM:
          IC9  : 6116    (2K x8 SRAM)
          IC10 : 6116    (2K x8 SRAM)
          IC16 : 6116    (2K x8 SRAM)
          IC22 : 6264    (8K x8 SRAM)
          IC23 : 6264    (8K x8 SRAM)
          IC31 : 6264    (8K x8 SRAM)
          IC32 : 6116    (2K x8 SRAM)
          IC32 : 6264    (8K x8 SRAM)
          IC33 : 6116    (2K x8 SRAM)
          IC38 : 6264    (8K x8 SRAM)
          IC39 : 6264    (8K x8 SRAM)
          IC55 : 6264    (8K x8 SRAM)
          IC56 : 6264    (8K x8 SRAM)
          IC60 : 6264    (8K x8 SRAM)
          IC61 : 6264    (8K x8 SRAM)
          IC64 : TC51832 (32K x8 SRAM)
          IC65 : TC51832 (32K x8 SRAM)
          IC66 : TC51832 (32K x8 SRAM)
          IC67 : TC51832 (32K x8 SRAM)
          IC68 : TC51832 (32K x8 SRAM)
          IC69 : TC51832 (32K x8 SRAM)
          IC70 : TC51832 (32K x8 SRAM)
          IC71 : TC51832 (32K x8 SRAM)
          IC72 : TC51832 (32K x8 SRAM)
          IC73 : TC51832 (32K x8 SRAM)
          IC74 : TC51832 (32K x8 SRAM)
          IC75 : TC51832 (32K x8 SRAM)
          IC76 : TC51832 (32K x8 SRAM)
          IC77 : TC51832 (32K x8 SRAM)
          IC78 : TC51832 (32K x8 SRAM)
          IC79 : TC51832 (32K x8 SRAM)
          IC125: MB81C78 (8K x8 SRAM ?)
          IC126: MB81C78 (8K x8 SRAM ?)
          IC134: 62256   (32K x8 SRAM)
          IC135: 62256   (32K x8 SRAM)
          IC150: 6264    (8K x8 SRAM)
          IC151: 6264    (8K x8 SRAM)

      SEGA Customs:
                   IC8  : 315-5218  (QFP100)
                   IC37 : 315-5248  (QFP100)
                   IC41 : 315-5249  (QFP120)
                   IC42 : 315-5275  (QFP100, located underneath the PCB)
                   IC53 : 315-5250  (QFP120)
                   IC81 : 315-5211A (PGA179)
                   IC107: 315-5248  (QFP100)
                   IC108: 315-5249  (QFP120)
                   IC148: 315-5197  (PGA135)
                   IC149: 315-5242  (Custom ceramic DIP package. contains a QFP44 and some smt resistors/caps etc)

      OTHER:
            IC14 : Z80 CPU (DIP40)
            IC15 : YM2151 (DIP24)
            IC28 : 68000 CPU (DIP64)
            IC118: Hitachi FD1094 Encrypted 68000 CPU
            IC159: SONY CXD1095 (QFP64)
            IC160: SONY CXD1095 (QFP64)
            IC165: ADC0804 (DIP20)


Network Board

Top    : 834-6780
Bottom : 171-5729-01
Sticker: 834-7112

|---------| |--| |----------------------|
|         RX   TX            315-5336   |
|             315-5337                  |
|                                       |
|            16MHz      6264            |
|                     EPR12587.14       |
| MB89372P-SH     Z80E        MB8421    |
|---------------------------------------|
Notes:
      PALs     : 315-5337, 315-5336
      Z80 clock: 8.000MHz
      6264     : 8K x8 SRAM
      MB8421   : Manufactured by Fujitsu, SDIP52
      MB89372  : Manufactured by Fujitsu, SDIP64
      EPR12587 : 27C256 EEPROM

***********************************************************************

    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0126a)

    This set is coming from a twin.

    This set has an extra link board (834-7112) or 171-5729-01 under the main board with a Z80

    Xtal is 16.000 Mhz.

    It has also one eprom (Epr 12587.14) two pal 16L8 (315-5336 and 315-5337) and two
    fujitsu IC MB89372P and MB8421-12LP

    Main Board : (834-8180-02)

    Epr12576A.20 (68000)
    Epr12577A.29 (68000)
    Epr12563B.58 FD1094 317-0126A
    Epr12564B.63 FD1094 317-0126A
    Epr12609.93
    Epr12610.97
    Epr12611.101
    Epr12612.105
    Mpr12417.92
    Mpr12418.96
    Mpr12419.100
    Mpr12420.104
    Mpr12421.91
    Mpr12422.95
    Mpr12423.99
    Mpr12424.103
    Mpr12425.90
    Mpr12426.94
    Mpr12427.98
    Mpr12428.102
    Epr12429.154
    Epr12430.153
    Epr12431.152
    Epr12436.17
    Mpr12437.11
    Mpr12438.12
    Mpr12439.13

    Link Board :

    Ep12587.14
*/

ROM_START( smgp )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12563b.58", 0x00000, 0x20000, CRC(baf1f333) SHA1(f91a7a311237b9940a44b815716d4226a7ae1e8b) )
	ROM_LOAD16_BYTE( "epr12564b.63", 0x00001, 0x20000, CRC(b5191af0) SHA1(d6fb19552e4816eefe751907ec55a2e07ad24879) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0126a.key", 0x0000, 0x2000, CRC(2abc1982) SHA1(cc4c36e6ba52431df17c6e36ba08d3a89be7b7e7) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12576a.20", 0x00000, 0x20000, CRC(2c9599c1) SHA1(79206f38c2976bd9299ed37bf62ac26dd3fba801) )
	ROM_LOAD16_BYTE( "epr12577a.29", 0x00001, 0x20000, CRC(abf9a50b) SHA1(e367b305cd45900aae4849af4904543f05456dc6) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12609.93",  0x180000, 0x20000, CRC(a867812f) SHA1(f8950bf794b6c2ec767ffff837d28917b636dbe7) ) //
	ROM_LOAD32_BYTE( "epr12610.97",  0x180001, 0x20000, CRC(53b99417) SHA1(ab72d35c88695c777d24c5557e5d3ea2d446e51b) ) //
	ROM_LOAD32_BYTE( "epr12611.101", 0x180002, 0x20000, CRC(bd5c6ab0) SHA1(7632dc4daa8eabe74769369856a8ba451e5bd420) ) // these differ from japan set
	ROM_LOAD32_BYTE( "epr12612.105", 0x180003, 0x20000, CRC(ac86e890) SHA1(7720c1c8df6de5de50254e97772c15161b796031) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0126a)
*/
/* this set contained only prg roms */
ROM_START( smgp6 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12563a.58", 0x00000, 0x20000, CRC(2e64b10e) SHA1(2be1ffb3120e4af6a61880e2a2602db07a73f373) )
	ROM_LOAD16_BYTE( "epr12564a.63", 0x00001, 0x20000, CRC(5baba3e7) SHA1(37194d5a4d3ee48a276f6aeb35b2f20a7661caa2) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0126a.key", 0x0000, 0x2000, CRC(2abc1982) SHA1(cc4c36e6ba52431df17c6e36ba08d3a89be7b7e7) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12576a.20", 0x00000, 0x20000, CRC(2c9599c1) SHA1(79206f38c2976bd9299ed37bf62ac26dd3fba801) )
	ROM_LOAD16_BYTE( "epr12577a.29", 0x00001, 0x20000, CRC(abf9a50b) SHA1(e367b305cd45900aae4849af4904543f05456dc6) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12609.93",  0x180000, 0x20000, CRC(a867812f) SHA1(f8950bf794b6c2ec767ffff837d28917b636dbe7) ) //
	ROM_LOAD32_BYTE( "epr12610.97",  0x180001, 0x20000, CRC(53b99417) SHA1(ab72d35c88695c777d24c5557e5d3ea2d446e51b) ) //
	ROM_LOAD32_BYTE( "epr12611.101", 0x180002, 0x20000, CRC(bd5c6ab0) SHA1(7632dc4daa8eabe74769369856a8ba451e5bd420) ) // these differ from japan set
	ROM_LOAD32_BYTE( "epr12612.105", 0x180003, 0x20000, CRC(ac86e890) SHA1(7720c1c8df6de5de50254e97772c15161b796031) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0126)

    SEGA Monaco G.P. by SEGA 1989

    This set is coming from a sitdown "air drive" version.

    This set has an extra sound board (837-7000) under the main board with a Z80
    and a few eproms, some of those eproms are already on the main board !

    It has also an "air drive" board with a Z80 and one eprom.

    Main Board : (834-7016-05)

    Epr12576.20 (68000)
    Epr12577.29 (68000)
    Epr12563.58 FD1094 317-0126
    Epr12564.63 FD1094 317-0126
    Epr12413.93
    Epr12414.97
    Epr12415.101
    Epr12416.105
    Mpr12417.92
    Mpr12418.96
    Mpr12419.100
    Mpr12420.104
    Mpr12421.91
    Mpr12422.95
    Mpr12423.99
    Mpr12424.103
    Mpr12425.90
    Mpr12426.94
    Mpr12427.98
    Mpr12428.102
    Epr12429.154
    Epr12430.153
    Epr12431.152
    Epr12436.17
    Mpr12437.11
    Mpr12438.12
    IC 13 is not used !

    Sound Board :

    Epr12535.8
    Mpr12437.20
    Mpr12438.21
    Mpr12439.22

    Air Drive Board :

    Ep12505.8
*/
ROM_START( smgp5 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12563.58", 0x00000, 0x20000, CRC(6d7325ae) SHA1(bf88ceddc49dab5b439080d5bf0e7e084a79546c) )
	ROM_LOAD16_BYTE( "epr12564.63", 0x00001, 0x20000, CRC(adfbf921) SHA1(f3321e03dc37b14db065b85d63e321810e4ea797) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0126.key", 0x0000, 0x2000, CRC(4d917996) SHA1(17232c0e35d439a12db3d966064cf00104088903) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12576.20", 0x00000, 0x20000, CRC(23266b26) SHA1(240b9bf198fd2975851e769766566ec4e8379f87) )
	ROM_LOAD16_BYTE( "epr12577.29", 0x00001, 0x20000, CRC(d5b53211) SHA1(b11f5c5094eb7ea9578f15489b00d8bbac1edee6) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12413.93",  0x180000, 0x20000, CRC(2f1693df) SHA1(ba1e654a1b5fae661b0dae4a8ed04ff50fb546a2) )
	ROM_LOAD32_BYTE( "epr12414.97",  0x180001, 0x20000, CRC(c78f3d45) SHA1(665750907ed11c89c2ea5c410eac2808445131ae) )
	ROM_LOAD32_BYTE( "epr12415.101", 0x180002, 0x20000, CRC(6080e9ed) SHA1(eb1b871453f76e6a65d20fa9d4bddc1c9f940b4d) )
	ROM_LOAD32_BYTE( "epr12416.105", 0x180003, 0x20000, CRC(6f1f2769) SHA1(d00d26cd1052d4b46c432b6b69cb2d83179d52a6) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
//  ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) ) // not here on this set

	ROM_REGION( 0x10000, "comm", ROMREGION_ERASE00 ) /* comms */
	/* no comms? */

	ROM_REGION( 0x10000, "cpu4", 0 ) /* z80 on extra sound board */
	ROM_LOAD( "epr12535.8",    0x00000, 0x10000, CRC(80453597) SHA1(d3fee7bb4a8964f5cf1cdae80fc3dde06c947839) )

	ROM_REGION( 0x80000, "pcm2", ROMREGION_ERASEFF ) /* Sega PCM sound data on extra sound board (same as on main board..) */
	ROM_LOAD( "mpr12437.20",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.21",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.22",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "cpu5", 0 ) /* z80 on air board */
	ROM_LOAD( "epr12505.8",    0x00000, 0x8000, CRC(5020788a) SHA1(ed6d1dfb8b6a62d17469e3d09a5b5b864c6b486c) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0125a)
*/
ROM_START( smgpu )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12561c.58", 0x00000, 0x20000, CRC(a5b0f3fe) SHA1(17103e56f822fdb52e72f597c01415ed375aa102) )
	ROM_LOAD16_BYTE( "epr12562c.63", 0x00001, 0x20000, CRC(799e55f4) SHA1(2e02cdc63bda47b087c81021018287cfa961c083) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0125a.key", 0x0000, 0x2000, CRC(3ecdb120) SHA1(c484198e4509d79214e78d4a47e9a7e339f7a2ed) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12574a.20", 0x00000, 0x20000, CRC(f8b5c38b) SHA1(0184d5a1b71fb42d33dbaaad99d2c0fbc5750e7e) )
	ROM_LOAD16_BYTE( "epr12575a.29", 0x00001, 0x20000, CRC(248b1d17) SHA1(22f1e0d0d698abdf0cb1954f1f6382432a12c186) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12609.93",  0x180000, 0x20000, CRC(a867812f) SHA1(f8950bf794b6c2ec767ffff837d28917b636dbe7) ) //
	ROM_LOAD32_BYTE( "epr12610.97",  0x180001, 0x20000, CRC(53b99417) SHA1(ab72d35c88695c777d24c5557e5d3ea2d446e51b) ) //
	ROM_LOAD32_BYTE( "epr12611.101", 0x180002, 0x20000, CRC(bd5c6ab0) SHA1(7632dc4daa8eabe74769369856a8ba451e5bd420) ) // these differ from japan set
	ROM_LOAD32_BYTE( "epr12612.105", 0x180003, 0x20000, CRC(ac86e890) SHA1(7720c1c8df6de5de50254e97772c15161b796031) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0125a)
*/
ROM_START( smgpu1 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12561b.58", 0x00000, 0x20000, CRC(80a32655) SHA1(fe1ffa8af9f1ca175ba90b24a0853329b08d19af) )
	ROM_LOAD16_BYTE( "epr12562b.63", 0x00001, 0x20000, CRC(d525f2a8) SHA1(f3241e11485c7428cd9f081ec6768fda39ae3250) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0125a.key", 0x0000, 0x2000, CRC(3ecdb120) SHA1(c484198e4509d79214e78d4a47e9a7e339f7a2ed) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12574a.20", 0x00000, 0x20000, CRC(f8b5c38b) SHA1(0184d5a1b71fb42d33dbaaad99d2c0fbc5750e7e) )
	ROM_LOAD16_BYTE( "epr12575a.29", 0x00001, 0x20000, CRC(248b1d17) SHA1(22f1e0d0d698abdf0cb1954f1f6382432a12c186) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12609.93",  0x180000, 0x20000, CRC(a867812f) SHA1(f8950bf794b6c2ec767ffff837d28917b636dbe7) ) //
	ROM_LOAD32_BYTE( "epr12610.97",  0x180001, 0x20000, CRC(53b99417) SHA1(ab72d35c88695c777d24c5557e5d3ea2d446e51b) ) //
	ROM_LOAD32_BYTE( "epr12611.101", 0x180002, 0x20000, CRC(bd5c6ab0) SHA1(7632dc4daa8eabe74769369856a8ba451e5bd420) ) // these differ from japan set
	ROM_LOAD32_BYTE( "epr12612.105", 0x180003, 0x20000, CRC(ac86e890) SHA1(7720c1c8df6de5de50254e97772c15161b796031) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0125a)
*/
ROM_START( smgpu2 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12561a.58", 0x00000, 0x20000, CRC(e505eb89) SHA1(bfb9a7a8b13ae454a92349e57215562477cd2cd2) )
	ROM_LOAD16_BYTE( "epr12562a.63", 0x00001, 0x20000, CRC(c3af4215) SHA1(c46829e08d5492515de5d3269b0e899705d0b108) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0125a.key", 0x0000, 0x2000, CRC(3ecdb120) SHA1(c484198e4509d79214e78d4a47e9a7e339f7a2ed) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12574a.20", 0x00000, 0x20000, CRC(f8b5c38b) SHA1(0184d5a1b71fb42d33dbaaad99d2c0fbc5750e7e) )
	ROM_LOAD16_BYTE( "epr12575a.29", 0x00001, 0x20000, CRC(248b1d17) SHA1(22f1e0d0d698abdf0cb1954f1f6382432a12c186) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12609.93",  0x180000, 0x20000, CRC(a867812f) SHA1(f8950bf794b6c2ec767ffff837d28917b636dbe7) ) //
	ROM_LOAD32_BYTE( "epr12610.97",  0x180001, 0x20000, CRC(53b99417) SHA1(ab72d35c88695c777d24c5557e5d3ea2d446e51b) ) //
	ROM_LOAD32_BYTE( "epr12611.101", 0x180002, 0x20000, CRC(bd5c6ab0) SHA1(7632dc4daa8eabe74769369856a8ba451e5bd420) ) // these differ from japan set
	ROM_LOAD32_BYTE( "epr12612.105", 0x180003, 0x20000, CRC(ac86e890) SHA1(7720c1c8df6de5de50254e97772c15161b796031) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0125a)
*/
/* very first US version with demo sound on by default */
ROM_START( smgpu3 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12561.58", 0x00000, 0x20000, CRC(80a32655) SHA1(fe1ffa8af9f1ca175ba90b24a0853329b08d19af) )
	ROM_LOAD16_BYTE( "epr12562.63", 0x00001, 0x20000, CRC(d525f2a8) SHA1(f3241e11485c7428cd9f081ec6768fda39ae3250) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0125a.key", 0x0000, 0x2000, CRC(3ecdb120) SHA1(c484198e4509d79214e78d4a47e9a7e339f7a2ed) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12574a.20", 0x00000, 0x20000, CRC(f8b5c38b) SHA1(0184d5a1b71fb42d33dbaaad99d2c0fbc5750e7e) )
	ROM_LOAD16_BYTE( "epr12575a.29", 0x00001, 0x20000, CRC(248b1d17) SHA1(22f1e0d0d698abdf0cb1954f1f6382432a12c186) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12609.93",  0x180000, 0x20000, CRC(a867812f) SHA1(f8950bf794b6c2ec767ffff837d28917b636dbe7) ) //
	ROM_LOAD32_BYTE( "epr12610.97",  0x180001, 0x20000, CRC(53b99417) SHA1(ab72d35c88695c777d24c5557e5d3ea2d446e51b) ) //
	ROM_LOAD32_BYTE( "epr12611.101", 0x180002, 0x20000, CRC(bd5c6ab0) SHA1(7632dc4daa8eabe74769369856a8ba451e5bd420) ) // these differ from japan set
	ROM_LOAD32_BYTE( "epr12612.105", 0x180003, 0x20000, CRC(ac86e890) SHA1(7720c1c8df6de5de50254e97772c15161b796031) ) //

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0124a)
*/
ROM_START( smgpj )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12432b.58", 0x00000, 0x20000, CRC(c1a29db1) SHA1(0122d366899f98f7a60b0c9bddeece7995cebf83) )
	ROM_LOAD16_BYTE( "epr12433b.63", 0x00001, 0x20000, CRC(97199eb1) SHA1(3baccf8159821d4b4d5caedf5eb691f07372be93) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0124a.key", 0x0000, 0x2000, CRC(022a8a16) SHA1(4fd80105cb85ccba77cf1e76a21d6e245d5d2e7d) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12441a.20", 0x00000, 0x20000, CRC(2c9599c1) SHA1(79206f38c2976bd9299ed37bf62ac26dd3fba801) )
	ROM_LOAD16_BYTE( "epr12442a.29", 0x00001, 0x20000, CRC(77a5ec16) SHA1(b8cf6a3f12689d89bbdd9fb39d1cb7d1a3c10602) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12413.93",  0x180000, 0x20000, CRC(2f1693df) SHA1(ba1e654a1b5fae661b0dae4a8ed04ff50fb546a2) )
	ROM_LOAD32_BYTE( "epr12414.97",  0x180001, 0x20000, CRC(c78f3d45) SHA1(665750907ed11c89c2ea5c410eac2808445131ae) )
	ROM_LOAD32_BYTE( "epr12415.101", 0x180002, 0x20000, CRC(6080e9ed) SHA1(eb1b871453f76e6a65d20fa9d4bddc1c9f940b4d) )
	ROM_LOAD32_BYTE( "epr12416.105", 0x180003, 0x20000, CRC(6f1f2769) SHA1(d00d26cd1052d4b46c432b6b69cb2d83179d52a6) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END

/**************************************************************************************************************************
    Super Monaco GP, Sega X-board
    CPU: FD1094 (317-0124a)
*/
ROM_START( smgpja )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr12432a.58", 0x00000, 0x20000, CRC(22517672) SHA1(db9ac40e83e9786bc9dad70f62c2080d3df694ee) )
	ROM_LOAD16_BYTE( "epr12433a.63", 0x00001, 0x20000, CRC(a46b5d13) SHA1(3a7de5cb6f3e6d726f0ea886a87125dedc6f849f) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0124a.key", 0x0000, 0x2000, CRC(022a8a16) SHA1(4fd80105cb85ccba77cf1e76a21d6e245d5d2e7d) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr12441a.20", 0x00000, 0x20000, CRC(2c9599c1) SHA1(79206f38c2976bd9299ed37bf62ac26dd3fba801) )
	ROM_LOAD16_BYTE( "epr12442a.29", 0x00001, 0x20000, CRC(77a5ec16) SHA1(b8cf6a3f12689d89bbdd9fb39d1cb7d1a3c10602) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr12429.154", 0x00000, 0x10000, CRC(5851e614) SHA1(3dc97237ede2c6125e92ea6efc68a748d0ec69be) )
	ROM_LOAD( "epr12430.153", 0x10000, 0x10000, CRC(05e00134) SHA1(8baaa80815d5dabd38dc8600e357975b96d23b95) )
	ROM_LOAD( "epr12431.152", 0x20000, 0x10000, CRC(35572f4a) SHA1(d66456ecf7b59f81736fb873c553926b56bb3977))

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "mpr12425.90",  0x000000, 0x20000, CRC(14bf2a15) SHA1(84db3ac09e4a8fe470ac051d8d5de1814b48bc72) )
	ROM_LOAD32_BYTE( "mpr12426.94",  0x000001, 0x20000, CRC(28b60dc0) SHA1(ad69d449434853445a076319a55a29014217a100) )
	ROM_LOAD32_BYTE( "mpr12427.98",  0x000002, 0x20000, CRC(0a367928) SHA1(bcb558b7c23906397e66a7f046b09eb5036c0888) )
	ROM_LOAD32_BYTE( "mpr12428.102", 0x000003, 0x20000, CRC(efa80ad5) SHA1(9bc7c3fb60cc076f29a0af487d58e5b48f1c4b06) )
	ROM_LOAD32_BYTE( "mpr12421.91",  0x080000, 0x20000, CRC(25f46140) SHA1(ea75e364cf52636d100158f79be627e36da8c327) )
	ROM_LOAD32_BYTE( "mpr12422.95",  0x080001, 0x20000, CRC(cb51c8f6) SHA1(5af56ae1916c3212b8d5b9e4bccbbe1916694f89) )
	ROM_LOAD32_BYTE( "mpr12423.99",  0x080002, 0x20000, CRC(0be9818e) SHA1(637a8201416e73d53f7e2502ea0a5277e43c167d) )
	ROM_LOAD32_BYTE( "mpr12424.103", 0x080003, 0x20000, CRC(0ce00dfc) SHA1(3b1990977ec7ad4c3bea66527707cff2cd8d5a98) )
	ROM_LOAD32_BYTE( "mpr12417.92",  0x100000, 0x20000, CRC(a806eabf) SHA1(1a61a2135d92b42ee131fd3240bc8a17a96696ab) )
	ROM_LOAD32_BYTE( "mpr12418.96",  0x100001, 0x20000, CRC(ed1a0f2b) SHA1(1aa87292ca0465fa129d6be81d95dbb77332ecab) )
	ROM_LOAD32_BYTE( "mpr12419.100", 0x100002, 0x20000, CRC(ce4568cb) SHA1(1ed66e74ce94d41593b498827d9cc243f775d4ba) )
	ROM_LOAD32_BYTE( "mpr12420.104", 0x100003, 0x20000, CRC(679442eb) SHA1(f88ef0219497f955d8db6783f3636dad52928f46) )
	ROM_LOAD32_BYTE( "epr12413.93",  0x180000, 0x20000, CRC(2f1693df) SHA1(ba1e654a1b5fae661b0dae4a8ed04ff50fb546a2) )
	ROM_LOAD32_BYTE( "epr12414.97",  0x180001, 0x20000, CRC(c78f3d45) SHA1(665750907ed11c89c2ea5c410eac2808445131ae) )
	ROM_LOAD32_BYTE( "epr12415.101", 0x180002, 0x20000, CRC(6080e9ed) SHA1(eb1b871453f76e6a65d20fa9d4bddc1c9f940b4d) )
	ROM_LOAD32_BYTE( "epr12416.105", 0x180003, 0x20000, CRC(6f1f2769) SHA1(d00d26cd1052d4b46c432b6b69cb2d83179d52a6) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr12436.17",    0x00000, 0x10000, CRC(16ec5f0a) SHA1(307b7388b5c36fd4bc2a61f7941db44858e03c5c) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "mpr12437.11",    0x00000, 0x20000, CRC(a1c7e712) SHA1(fa7fa8c39690ae5dab8b28af5aeed5ffae2cd6de) )
	ROM_LOAD( "mpr12438.12",    0x20000, 0x20000, CRC(6573d46b) SHA1(c4a4a0ea35250eff28a5bfd5e9cd372f52fd1308) )
	ROM_LOAD( "mpr12439.13",    0x40000, 0x20000, CRC(13bf6de5) SHA1(92228a05ec33d606491a1da98c4989f69cddbb49) )

	ROM_REGION( 0x10000, "comm", 0 ) /* comms */
	ROM_LOAD( "epr12587.14",    0x00000, 0x8000, CRC(2afe648b) SHA1(b5bf86f3acbcc23c136185110acecf2c971294fa) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    AB Cop, Sega X-board
    CPU: FD1094 (317-0169b)
*/
ROM_START( abcop )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-13568b.ic58", 0x00000, 0x20000, CRC(f88db35b) SHA1(7d85c1194a2aa08427333d2ffc2a8d4f7e1beff0) )
	ROM_LOAD16_BYTE( "epr-13556b.ic63", 0x00001, 0x20000, CRC(337bf32e) SHA1(dafb9d9b3baf79ca76355278e8a14294f186790a) )
	ROM_LOAD16_BYTE( "epr-13559.ic57",  0x40000, 0x20000, CRC(4588bf19) SHA1(6a8b3d4450ac0bc41b46e6a4e1b44d82112fcd64) )
	ROM_LOAD16_BYTE( "epr-13558.ic62",  0x40001, 0x20000, CRC(11259ed4) SHA1(e7de174a0bdb1d1111e5e419f1d501ab5be1d32d) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0169b.key", 0x0000, 0x2000, CRC(058da36e) SHA1(ab3f68a90725063c68fc5d0f8dbece1f8940dc7d) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr-13566.ic20", 0x00000, 0x20000, CRC(22e52f32) SHA1(c67a4ccb88becc58dddcbfea0a1ac2017f7b2929) )
	ROM_LOAD16_BYTE( "epr-13565.ic29", 0x00001, 0x20000, CRC(a21784bd) SHA1(b40ba0ef65bbfe514625253f6aeec14bf4bcf08c) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "opr-13553.ic154", 0x00000, 0x10000, CRC(8c418837) SHA1(e325db39fae768865e20d2cd1ee2b91a9b0165f5) )
	ROM_LOAD( "opr-13554.ic153", 0x10000, 0x10000, CRC(4e3df9f0) SHA1(8b481c2cd25c58612ac8ac3ffb7eeae9ca247d2e) )
	ROM_LOAD( "opr-13555.ic152", 0x20000, 0x10000, CRC(6c4a1d42) SHA1(6c37b045b21173f1e2f7bd19d01c00979b8107fb) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "opr-13552.ic90",  0x000000, 0x20000, CRC(cc2cf706) SHA1(ad39c22e652ebcd90ffb5e17ae35985645f93c71) )
	ROM_LOAD32_BYTE( "opr-13551.ic94",  0x000001, 0x20000, CRC(d6f276c1) SHA1(9ec68157ea460e09ef4b69aa8ea17687dc47ea59) )
	ROM_LOAD32_BYTE( "opr-13550.ic98",  0x000002, 0x20000, CRC(f16518dd) SHA1(a5f1785cd28f03069cb238ac92c6afb5a26cbd37) )
	ROM_LOAD32_BYTE( "opr-13549.ic102", 0x000003, 0x20000, CRC(cba407a7) SHA1(e7684d3b40baa6d832b887fd85ad67fbad8aa7de) )
	ROM_LOAD32_BYTE( "opr-13548.ic91",  0x080000, 0x20000, CRC(080fd805) SHA1(e729565815a3a37462cfee460b7392d2f08e96e5) )
	ROM_LOAD32_BYTE( "opr-13547.ic95",  0x080001, 0x20000, CRC(42d4dd68) SHA1(6ae1f3585ebb20fd2908456d6fa41a893261277e) )
	ROM_LOAD32_BYTE( "opr-13546.ic99",  0x080002, 0x20000, CRC(ca6fbf3d) SHA1(49c3516d87f1546fa7efe785fc5c064d90b1cb8e) )
	ROM_LOAD32_BYTE( "opr-13545.ic103", 0x080003, 0x20000, CRC(c9e58dd2) SHA1(ace2e1630d8df2454183ffdbe26d8cb6d199e940) )
	ROM_LOAD32_BYTE( "opr-13544.ic92",  0x100000, 0x20000, CRC(9c1436d9) SHA1(5156e1b5c7461f6dc0d449b86b6b72153b290a4c) )
	ROM_LOAD32_BYTE( "opr-13543.ic96",  0x100001, 0x20000, CRC(2c1c8f0e) SHA1(19c9fd4272a3db18381f435ed6cd01f994c655e7) )
	ROM_LOAD32_BYTE( "opr-13542.ic100", 0x100002, 0x20000, CRC(01fd52b8) SHA1(b4ab13c7b2b2ffcfdab37d8e4855d5ef8823f1cc) )
	ROM_LOAD32_BYTE( "opr-13541.ic104", 0x100003, 0x20000, CRC(a45c547b) SHA1(d93aaa850d14a7699a1b0411e823088a9bce7553) )
	ROM_LOAD32_BYTE( "opr-13540.ic93",  0x180000, 0x20000, CRC(84b42ab0) SHA1(d24ba7fe23463fc5813ef26e0395951559d6d162) )
	ROM_LOAD32_BYTE( "opr-13539.ic97",  0x180001, 0x20000, CRC(cd6e524f) SHA1(e6df2552a84b2da95301486379c78679b0297634) )
	ROM_LOAD32_BYTE( "opr-13538.ic101", 0x180002, 0x20000, CRC(bf9a4586) SHA1(6013dee83375d72d262c8c04c2e668afea2e216c) )
	ROM_LOAD32_BYTE( "opr-13537.ic105", 0x180003, 0x20000, CRC(fa14ed3e) SHA1(d684496ade2517696a56c1423dd4686d283c133f) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* ground data */
	ROM_LOAD( "opr-13564.ic40",	 0x00000, 0x10000, CRC(e70ba138) SHA1(85eb6618f408642227056d278f10dec8dcc5a80d) )

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-13560.ic17",    0x00000, 0x10000, CRC(83050925) SHA1(118710e5789c7999bb7326df4d7bd207cbffdfd4) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "opr-13563.ic11",    0x00000, 0x20000, CRC(4083e74f) SHA1(e48c7ce0aa3406af0bbf79c169a8157693c97041) )
	ROM_LOAD( "opr-13562.ic12",    0x20000, 0x20000, CRC(3cc3968f) SHA1(d25647f6a3fa939ba30e03e7334362ef0749b23a) )
	ROM_LOAD( "opr-13561.ic13",    0x40000, 0x20000, CRC(80a7c02a) SHA1(7e8c1b9ba270d8657dbe90ed8be2e4b6463e5928) )
ROM_END


/**************************************************************************************************************************
 **************************************************************************************************************************
 **************************************************************************************************************************
    GP Rider (World), Sega X-board
    CPU: FD1094 (317-0163)
    Custom Chip 315-5304 (IC 127)
    IC BD Number: 834-7626-03 (roms are "MPR") / 834-7626-05 (roms are "EPR")
*/
ROM_START( gprider )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-13408.ic63", 0x00001, 0x20000, NO_DUMP )	/* we have a key for this set but no program roms */
	ROM_LOAD16_BYTE( "epr-13409.ic58", 0x00000, 0x20000, NO_DUMP )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0163.key", 0x0000, 0x2000, CRC(c1d4d207) SHA1(c35b0a49fb6a1e0e9a1c087f0ccd190ad5c2bb2c) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr-13395.ic20", 0x00000, 0x20000,CRC(d6ccfac7) SHA1(9287ab08600163a0d9bd33618c629f99391316bd) )
	ROM_LOAD16_BYTE( "epr-13394.ic29", 0x00001, 0x20000,CRC(914a55ec) SHA1(84fe1df12478990418b46b6800425e5599e9eff9) )
	ROM_LOAD16_BYTE( "epr-13393.ic21", 0x40000, 0x20000,CRC(08d023cc) SHA1(d008d57e494f484a1a84896065d53fb9b1d8d60e) )
	ROM_LOAD16_BYTE( "epr-13392.ic30", 0x40001, 0x20000,CRC(f927cd42) SHA1(67eab328c1fb878fe3d086d0639f5051b135a037) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr-13383.ic154", 0x00000, 0x10000, CRC(24f897a7) SHA1(68ba17067d90f07bb5a549017be4773b33ae81d0) )
	ROM_LOAD( "epr-13384.ic153", 0x10000, 0x10000, CRC(fe8238bd) SHA1(601910bd86536e6b08f5308b298c8f01fa60f233) )
	ROM_LOAD( "epr-13385.ic152", 0x20000, 0x10000, CRC(6df1b995) SHA1(5aab19b87a9ef162c30ccf5974cb795e37dba91f) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr-13382.ic90",  0x000000, 0x20000, CRC(01dac209) SHA1(4c6b03308193c472f6cdbcede306f8ce6db0cc4b) )
	ROM_LOAD32_BYTE( "epr-13381.ic94",  0x000001, 0x20000, CRC(3a50d931) SHA1(9d9cb1793f3b8f562ce0ea49f2afeef099f20859) )
	ROM_LOAD32_BYTE( "epr-13380.ic98",  0x000002, 0x20000, CRC(ad1024c8) SHA1(86e941424b2e2e00940886e5daed640a78ed7403) )
	ROM_LOAD32_BYTE( "epr-13379.ic102", 0x000003, 0x20000, CRC(1ac17625) SHA1(7aefd382041dd3f97936ecb8738a3f2c9780c58f) )
	ROM_LOAD32_BYTE( "epr-13378.ic91",  0x080000, 0x20000, CRC(50c9b867) SHA1(dd9702b369ea8abd50da22ce721b7040428e9d4b) )
	ROM_LOAD32_BYTE( "epr-13377.ic95",  0x080001, 0x20000, CRC(9b12f5c0) SHA1(2060420611b3354974c49bc80f556f945512570b) )
	ROM_LOAD32_BYTE( "epr-13376.ic99",  0x080002, 0x20000, CRC(449ac518) SHA1(0438a72e53a7889d39ea7e2530e49a2594d97e90) )
	ROM_LOAD32_BYTE( "epr-13375.ic103", 0x080003, 0x20000, CRC(5489a9ff) SHA1(c458cb55d957edae340535f54189438296f3ec2f) )
	ROM_LOAD32_BYTE( "epr-13374.ic92",  0x100000, 0x20000, CRC(6a319e4f) SHA1(d9f92b15f4baa14745048073205add35b7d42d27) )
	ROM_LOAD32_BYTE( "epr-13373.ic96",  0x100001, 0x20000, CRC(eca5588b) SHA1(11def0c293868193d457958fe7459fd8c31dbd2b) )
	ROM_LOAD32_BYTE( "epr-13372.ic100", 0x100002, 0x20000, CRC(0b45a433) SHA1(82fa2b208eaf70b70524681fbc3ec70085e70d83) )
	ROM_LOAD32_BYTE( "epr-13371.ic104", 0x100003, 0x20000, CRC(b68f4cff) SHA1(166f2a685cbc230c098fdc1646b6e632dd2b09dd) )
	ROM_LOAD32_BYTE( "epr-13370.ic93",  0x180000, 0x20000, CRC(78276620) SHA1(2c4505c57a1e765f9cfd48fb1637d67d199a2f1d) )
	ROM_LOAD32_BYTE( "epr-13369.ic97",  0x180001, 0x20000, CRC(8625bf0f) SHA1(0ae70bc0d54e25eecf4a11cf0600225dca35914d) )
	ROM_LOAD32_BYTE( "epr-13368.ic101", 0x180002, 0x20000, CRC(0f50716c) SHA1(eb4c7f47e11c58fe0d58f67e6dafabc6291eabb8) )
	ROM_LOAD32_BYTE( "epr-13367.ic105", 0x180003, 0x20000, CRC(4b1bb51f) SHA1(17fd5ac9e18dd6097a015e9d7b6815826f9c53f1) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-13388.ic17",    0x00000, 0x10000, CRC(706581e4) SHA1(51c9dbf2bf0d6b8826de24cd33596f5c95136870) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr-13391.ic11",    0x00000, 0x20000, CRC(8c30c867) SHA1(0d735291b1311890938f8a1143fae6af9feb2a69) )
	ROM_LOAD( "epr-13390.ic12",    0x20000, 0x20000, CRC(8c93cd05) SHA1(bb08094abac6c104eddf14f634e9791f03122946) )
	ROM_LOAD( "epr-13389.ic13",    0x40000, 0x20000, CRC(4e4c758e) SHA1(181750dfcdd6d5b28b063c980c251991163d9474) )
ROM_END

/**************************************************************************************************************************
    GP Rider (US), Sega X-board
    CPU: FD1094 (317-0162)
    Custom Chip 315-5304 (IC 127)
    IC BD Number: 834-7626-01 (roms are "MPR") / 834-7626-04 (roms are "EPR")
*/
ROM_START( gprider1 )
	ROM_REGION( 0x80000, "main", 0 ) /* 68000 code */
	ROM_LOAD16_BYTE( "epr-13406.ic63", 0x00001, 0x20000, CRC(122c711f) SHA1(2bcc51347e771a7e7f770e68b24d82497d24aa2e) )
	ROM_LOAD16_BYTE( "epr-13407.ic58", 0x00000, 0x20000, CRC(03553ebd) SHA1(041a71a2dce2ad56360f500cb11e29a629020160) )

	ROM_REGION( 0x2000, "user1", 0 )	/* decryption key */
	ROM_LOAD( "317-0162.key", 0x0000, 0x2000, CRC(8067de53) SHA1(e8cd1dfbad94856c6bd51569557667e72f0a5dd4) )

	ROM_REGION( 0x80000, "sub", 0 ) /* 2nd 68000 code */
	ROM_LOAD16_BYTE( "epr-13395.ic20", 0x00000, 0x20000,CRC(d6ccfac7) SHA1(9287ab08600163a0d9bd33618c629f99391316bd) )
	ROM_LOAD16_BYTE( "epr-13394.ic29", 0x00001, 0x20000,CRC(914a55ec) SHA1(84fe1df12478990418b46b6800425e5599e9eff9) )
	ROM_LOAD16_BYTE( "epr-13393.ic21", 0x40000, 0x20000,CRC(08d023cc) SHA1(d008d57e494f484a1a84896065d53fb9b1d8d60e) )
	ROM_LOAD16_BYTE( "epr-13392.ic30", 0x40001, 0x20000,CRC(f927cd42) SHA1(67eab328c1fb878fe3d086d0639f5051b135a037) )

	ROM_REGION( 0x30000, "gfx1", ROMREGION_DISPOSE ) /* tiles */
	ROM_LOAD( "epr-13383.ic154", 0x00000, 0x10000, CRC(24f897a7) SHA1(68ba17067d90f07bb5a549017be4773b33ae81d0) )
	ROM_LOAD( "epr-13384.ic153", 0x10000, 0x10000, CRC(fe8238bd) SHA1(601910bd86536e6b08f5308b298c8f01fa60f233) )
	ROM_LOAD( "epr-13385.ic152", 0x20000, 0x10000, CRC(6df1b995) SHA1(5aab19b87a9ef162c30ccf5974cb795e37dba91f) )

	ROM_REGION32_LE( 0x200000, "gfx2", 0 ) /* sprites */
	ROM_LOAD32_BYTE( "epr-13382.ic90",  0x000000, 0x20000, CRC(01dac209) SHA1(4c6b03308193c472f6cdbcede306f8ce6db0cc4b) )
	ROM_LOAD32_BYTE( "epr-13381.ic94",  0x000001, 0x20000, CRC(3a50d931) SHA1(9d9cb1793f3b8f562ce0ea49f2afeef099f20859) )
	ROM_LOAD32_BYTE( "epr-13380.ic98",  0x000002, 0x20000, CRC(ad1024c8) SHA1(86e941424b2e2e00940886e5daed640a78ed7403) )
	ROM_LOAD32_BYTE( "epr-13379.ic102", 0x000003, 0x20000, CRC(1ac17625) SHA1(7aefd382041dd3f97936ecb8738a3f2c9780c58f) )
	ROM_LOAD32_BYTE( "epr-13378.ic91",  0x080000, 0x20000, CRC(50c9b867) SHA1(dd9702b369ea8abd50da22ce721b7040428e9d4b) )
	ROM_LOAD32_BYTE( "epr-13377.ic95",  0x080001, 0x20000, CRC(9b12f5c0) SHA1(2060420611b3354974c49bc80f556f945512570b) )
	ROM_LOAD32_BYTE( "epr-13376.ic99",  0x080002, 0x20000, CRC(449ac518) SHA1(0438a72e53a7889d39ea7e2530e49a2594d97e90) )
	ROM_LOAD32_BYTE( "epr-13375.ic103", 0x080003, 0x20000, CRC(5489a9ff) SHA1(c458cb55d957edae340535f54189438296f3ec2f) )
	ROM_LOAD32_BYTE( "epr-13374.ic92",  0x100000, 0x20000, CRC(6a319e4f) SHA1(d9f92b15f4baa14745048073205add35b7d42d27) )
	ROM_LOAD32_BYTE( "epr-13373.ic96",  0x100001, 0x20000, CRC(eca5588b) SHA1(11def0c293868193d457958fe7459fd8c31dbd2b) )
	ROM_LOAD32_BYTE( "epr-13372.ic100", 0x100002, 0x20000, CRC(0b45a433) SHA1(82fa2b208eaf70b70524681fbc3ec70085e70d83) )
	ROM_LOAD32_BYTE( "epr-13371.ic104", 0x100003, 0x20000, CRC(b68f4cff) SHA1(166f2a685cbc230c098fdc1646b6e632dd2b09dd) )
	ROM_LOAD32_BYTE( "epr-13370.ic93",  0x180000, 0x20000, CRC(78276620) SHA1(2c4505c57a1e765f9cfd48fb1637d67d199a2f1d) )
	ROM_LOAD32_BYTE( "epr-13369.ic97",  0x180001, 0x20000, CRC(8625bf0f) SHA1(0ae70bc0d54e25eecf4a11cf0600225dca35914d) )
	ROM_LOAD32_BYTE( "epr-13368.ic101", 0x180002, 0x20000, CRC(0f50716c) SHA1(eb4c7f47e11c58fe0d58f67e6dafabc6291eabb8) )
	ROM_LOAD32_BYTE( "epr-13367.ic105", 0x180003, 0x20000, CRC(4b1bb51f) SHA1(17fd5ac9e18dd6097a015e9d7b6815826f9c53f1) )

	ROM_REGION( 0x10000, "gfx3", ROMREGION_ERASE00 ) /* road gfx */
	/* none?? */

	ROM_REGION( 0x10000, "sound", 0 ) /* sound CPU */
	ROM_LOAD( "epr-13388.ic17",    0x00000, 0x10000, CRC(706581e4) SHA1(51c9dbf2bf0d6b8826de24cd33596f5c95136870) )

	ROM_REGION( 0x80000, "pcm", ROMREGION_ERASEFF ) /* Sega PCM sound data */
	ROM_LOAD( "epr-13391.ic11",    0x00000, 0x20000, CRC(8c30c867) SHA1(0d735291b1311890938f8a1143fae6af9feb2a69) )
	ROM_LOAD( "epr-13390.ic12",    0x20000, 0x20000, CRC(8c93cd05) SHA1(bb08094abac6c104eddf14f634e9791f03122946) )
	ROM_LOAD( "epr-13389.ic13",    0x40000, 0x20000, CRC(4e4c758e) SHA1(181750dfcdd6d5b28b063c980c251991163d9474) )
ROM_END



/*************************************
 *
 *  Generic driver initialization
 *
 *************************************/

static DRIVER_INIT( generic_xboard )
{
	xboard_generic_init(machine);
}



/*************************************
 *
 *  Game-specific driver inits
 *
 *************************************/

static DRIVER_INIT( aburner2 )
{
	xboard_generic_init(machine);
	xboard_set_road_priority(0);

	memory_install_write16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x140006, 0x140007, 0, 0x00fff0, aburner2_iochip_0_D_w);
}


static DRIVER_INIT( aburner )
{
	xboard_generic_init(machine);
	xboard_set_road_priority(0);
}


static DRIVER_INIT( loffire )
{
	xboard_generic_init(machine);
	adc_reverse[1] = adc_reverse[3] = 1;

	/* install extra synchronization on core shared memory */
	loffire_sync = memory_install_write16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x29c000, 0x29c011, 0, 0, loffire_sync0_w);
}


static DRIVER_INIT( smgp )
{
	xboard_generic_init(machine);
	memory_install_readwrite16_handler(cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM), 0x2f0000, 0x2f3fff, 0, 0, smgp_excs_r, smgp_excs_w);
}


static DRIVER_INIT( gprider )
{
	xboard_generic_init(machine);
	gprider_hack = 1;
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1987, aburner2, 0,        xboard,  aburner2, aburner2,       ROT0, "Sega", "After Burner II", 0 )
GAME( 1987, aburner,  aburner2, xboard,  aburner,  aburner,        ROT0, "Sega", "After Burner (Japan)", 0 )
GAME( 1987, thndrbld, 0,        xboard,  thndrbld, generic_xboard, ROT0, "Sega", "Thunder Blade (upright, FD1094 317-0056)", 0 )
GAME( 1987, thndrbd1, thndrbld, xboard,  thndrbd1, generic_xboard, ROT0, "Sega", "Thunder Blade (deluxe/standing, unprotected)", 0 )
GAME( 1989, loffire,  0,        xboard,  loffire,  loffire,        ROT0, "Sega", "Line of Fire / Bakudan Yarou (World, FD1094 317-0136)", 0 )
GAME( 1989, loffireu, loffire,  xboard,  loffire,  loffire,        ROT0, "Sega", "Line of Fire / Bakudan Yarou (US, FD1094 317-0135)", 0 )
GAME( 1989, loffirej, loffire,  xboard,  loffire,  loffire,        ROT0, "Sega", "Line of Fire / Bakudan Yarou (Japan, FD1094 317-0134)", 0 )
GAME( 1989, rachero,  0,        xboard,  rachero,  generic_xboard, ROT0, "Sega", "Racing Hero (FD1094 317-0144)", 0 )
GAME( 1989, smgp,     0,        smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 9, World, Rev B, 'Twin', FD1094 317-0126a)", 0 )
GAME( 1989, smgp6,    smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 8, World, Rev A, FD1094 317-0126a)", 0 )
GAME( 1989, smgp5,    smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 7, World, 'Air Drive Cabinet', FD1094 317-0126)", 0 )
GAME( 1989, smgpu,    smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 6, US, Rev C, FD1094 317-0125a)", 0 )
GAME( 1989, smgpu1,   smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 5, US, Rev B, FD1094 317-0125a)", 0 )
GAME( 1989, smgpu2,   smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 4, US, Rev A, FD1094 317-0125a)", 0 )
GAME( 1989, smgpu3,   smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 3, US, FD1094 317-0125a)", 0 )
GAME( 1989, smgpj,    smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 2, Japan, Rev B, FD1094 317-0124a)", 0 )
GAME( 1989, smgpja,   smgp,     smgp,    smgp,     smgp,           ROT0, "Sega", "Super Monaco GP (set 1, Japan, Rev A, FD1094 317-0124a)", 0 )
GAME( 1990, abcop,    0,        xboard,  abcop,    generic_xboard, ROT0, "Sega", "A.B. Cop (FD1094 317-0169b)", 0 )
GAME( 1990, gprider,  0,        xboard,  gprider,  gprider,        ROT0, "Sega", "GP Rider (set 2, World, FD1094 317-0163)", GAME_NOT_WORKING ) // no prg roms
GAME( 1990, gprider1, gprider,  xboard,  gprider,  gprider,        ROT0, "Sega", "GP Rider (set 1, US, FD1094 317-0162)", 0 )
