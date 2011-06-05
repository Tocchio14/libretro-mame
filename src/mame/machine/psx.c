/***************************************************************************

  machine/psx.c

***************************************************************************/

#include "emu.h"
#include "cpu/psx/psx.h"
#include "video/psx.h"
#include "includes/psx.h"

#define VERBOSE_LEVEL ( 0 )

INLINE void ATTR_PRINTF(3,4) verboselog( psx_state *p_psx, int n_level, const char *s_fmt, ... )
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror( "%s: %s", p_psx->machine().describe_context(), buf );
	}
}

void psx_driver_init( running_machine &machine )
{
	psx_state *p_psx = machine.driver_data<psx_state>();

	p_psx->b_need_sianniv_vblank_hack = !strcmp(machine.system().name, "sianniv");

	p_psx->m_p_n_psxram = (UINT32 *)memory_get_shared(machine, "share1", p_psx->m_n_psxramsize);
}

WRITE32_HANDLER( psx_com_delay_w )
{
	psx_state *p_psx = space->machine().driver_data<psx_state>();

	COMBINE_DATA( &p_psx->n_com_delay );
	verboselog( p_psx, 1, "psx_com_delay_w( %08x %08x )\n", data, mem_mask );
}

READ32_HANDLER( psx_com_delay_r )
{
	psx_state *p_psx = space->machine().driver_data<psx_state>();

	verboselog( p_psx, 1, "psx_com_delay_r( %08x )\n", mem_mask );
	return p_psx->n_com_delay;
}

/* IRQ */

void psx_irq_set( running_machine &machine, UINT32 data )
{
	psxcpu_device::irq_set( *machine.device("maincpu"), "maincpu", data );
}

/* SIO */

void psx_sio_install_handler( running_machine &machine, int n_port, psx_sio_handler p_f_sio_handler )
{
	psxcpu_device::install_sio_handler( *machine.device("maincpu"), "maincpu", n_port, p_f_sio_handler );
}

void psx_sio_input( running_machine &machine, int n_port, int n_mask, int n_data )
{
	psxcpu_device::sio_input( *machine.device("maincpu"), "maincpu", n_port, n_mask, n_data );
}

/* GPU */

PALETTE_INIT( psx )
{
	UINT32 n_colour;

	for( n_colour = 0; n_colour < 0x10000; n_colour++ )
	{
		palette_set_color_rgb( machine, n_colour, pal5bit(n_colour >> 0), pal5bit(n_colour >> 5), pal5bit(n_colour >> 10) );
	}
}

SCREEN_UPDATE( psx )
{
	psxgpu_device *gpu = downcast<psxgpu_device *>( screen->machine().device("gpu") );
	gpu->update_screen( bitmap, cliprect );
	return 0;
}

READ32_HANDLER( psx_gpu_r )
{
	psxgpu_device *gpu = downcast<psxgpu_device *>( space->machine().device("gpu") );
	return gpu->read( *space, offset, mem_mask );
}

WRITE32_HANDLER( psx_gpu_w )
{
	psxgpu_device *gpu = downcast<psxgpu_device *>( space->machine().device("gpu") );
	gpu->write( *space, offset, data, mem_mask );
}

void psx_lightgun_set( running_machine &machine, int n_x, int n_y )
{
	psxgpu_device *gpu = downcast<psxgpu_device *>( machine.device("gpu") );
	gpu->lightgun_set( n_x, n_y );
}

INTERRUPT_GEN( psx_vblank )
{
	psxgpu_device *gpu = downcast<psxgpu_device *>( device->machine().device("gpu") );
	psx_state *p_psx = device->machine().driver_data<psx_state>();

	if(p_psx->b_need_sianniv_vblank_hack)
	{
		UINT32 pc = cpu_get_pc(device);
		if((pc >= 0x80010018 && pc <= 0x80010028) || pc == 0x8002a4f0)
			return;
	}

	gpu->vblank();
}

