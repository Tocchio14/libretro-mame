/**********************************************************************

    RCA CDP1861 Video Display Controller emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************
                            _____   _____
                  _CLK   1 |*    \_/     | 24  Vdd
                 _DMAO   2 |             | 23  _CLEAR
                  _INT   3 |             | 22  SC1
                   TPA   4 |             | 21  SC0
                   TPB   5 |             | 20  DI7
            _COMP SYNC   6 |   CDP1861   | 19  DI6
                 VIDEO   7 |             | 18  DI5
                _RESET   8 |             | 17  DI4
                  _EFX   9 |             | 16  DI3
               DISP ON  10 |             | 15  DI2
              DISP OFF  11 |             | 14  DI1
                   Vss  12 |_____________| 13  DI0

**********************************************************************/

#pragma once

#ifndef __CDP1861__
#define __CDP1861__

#include "emu.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define CDP1861_VISIBLE_COLUMNS 64
#define CDP1861_VISIBLE_LINES   128

#define CDP1861_HBLANK_START    14 * 8
#define CDP1861_HBLANK_END      12
#define CDP1861_HSYNC_START     0
#define CDP1861_HSYNC_END       12
#define CDP1861_SCREEN_WIDTH    14 * 8

#define CDP1861_TOTAL_SCANLINES             262

#define CDP1861_SCANLINE_DISPLAY_START      80
#define CDP1861_SCANLINE_DISPLAY_END        208
#define CDP1861_SCANLINE_VBLANK_START       262
#define CDP1861_SCANLINE_VBLANK_END         16
#define CDP1861_SCANLINE_VSYNC_START        16
#define CDP1861_SCANLINE_VSYNC_END          0
#define CDP1861_SCANLINE_INT_START          CDP1861_SCANLINE_DISPLAY_START - 2
#define CDP1861_SCANLINE_INT_END            CDP1861_SCANLINE_DISPLAY_START
#define CDP1861_SCANLINE_EFX_TOP_START      CDP1861_SCANLINE_DISPLAY_START - 4
#define CDP1861_SCANLINE_EFX_TOP_END        CDP1861_SCANLINE_DISPLAY_START
#define CDP1861_SCANLINE_EFX_BOTTOM_START   CDP1861_SCANLINE_DISPLAY_END - 4
#define CDP1861_SCANLINE_EFX_BOTTOM_END     CDP1861_SCANLINE_DISPLAY_END



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_CDP1861_ADD(_tag, _screen_tag, _clock, _irq, _dma_out, _efx) \
	MCFG_DEVICE_ADD(_tag, CDP1861, _clock) \
	downcast<cdp1861_device *>(device)->set_screen_tag(_screen_tag); \
	downcast<cdp1861_device *>(device)->set_irq_callback(DEVCB2_##_irq); \
	downcast<cdp1861_device *>(device)->set_dma_out_callback(DEVCB2_##_dma_out); \
	downcast<cdp1861_device *>(device)->set_efx_callback(DEVCB2_##_efx);

#define MCFG_CDP1861_SCREEN_ADD(_cdptag, _tag, _clock) \
	MCFG_SCREEN_ADD(_tag, RASTER) \
	MCFG_SCREEN_UPDATE_DEVICE(_cdptag, cdp1861_device, screen_update) \
	MCFG_SCREEN_RAW_PARAMS(_clock, CDP1861_SCREEN_WIDTH, CDP1861_HBLANK_END, CDP1861_HBLANK_START, CDP1861_TOTAL_SCANLINES, CDP1861_SCANLINE_VBLANK_END, CDP1861_SCANLINE_VBLANK_START)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> cdp1861_device

class cdp1861_device :  public device_t
{
public:
	// construction/destruction
	cdp1861_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	void set_screen_tag(const char *screen_tag) { m_screen_tag = screen_tag; }
	template<class _irq> void set_irq_callback(_irq irq) { m_write_irq.set_callback(irq); }
	template<class _dma_out> void set_dma_out_callback(_dma_out dma_out) { m_write_dma_out.set_callback(dma_out); }
	template<class _efx> void set_efx_callback(_efx efx) { m_write_efx.set_callback(efx); }

	DECLARE_WRITE8_MEMBER( dma_w );
	DECLARE_WRITE_LINE_MEMBER( disp_on_w );
	DECLARE_WRITE_LINE_MEMBER( disp_off_w );

	UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:
	enum
	{
		TIMER_INT,
		TIMER_EFX,
		TIMER_DMA
	};

	devcb2_write_line m_write_irq;
	devcb2_write_line m_write_dma_out;
	devcb2_write_line m_write_efx;

	const char *m_screen_tag;
	screen_device *m_screen;        // screen
	bitmap_rgb32 m_bitmap;          // bitmap

	int m_disp;                     // display enabled
	int m_dispon;                   // display on latch
	int m_dispoff;                  // display off latch
	int m_dmaout;                   // DMA request active

	// timers
	emu_timer *m_int_timer;         // interrupt timer
	emu_timer *m_efx_timer;         // EFx timer
	emu_timer *m_dma_timer;         // DMA timer
};


// device type definition
extern const device_type CDP1861;



#endif
