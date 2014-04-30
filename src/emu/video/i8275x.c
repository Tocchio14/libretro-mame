// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Intel 8275 Programmable CRT Controller emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

/*

    TODO:

	- character attributes
    - double spaced rows

*/

#include "i8275x.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define LOG 0


const int DMA_BURST_SPACING[] = { 0, 7, 15, 23, 31, 39, 47, 55 };


#define DOUBLE_SPACED_ROWS \
	BIT(m_param[REG_SCN1], 7)

#define CHARACTERS_PER_ROW \
	((m_param[REG_SCN1] & 0x7f) + 1)

#define VRTC_ROW_COUNT \
	((m_param[REG_SCN2] >> 5) + 1)

#define CHARACTER_ROWS_PER_FRAME \
	((m_param[REG_SCN2] & 0x3f) + 1)

#define UNDERLINE \
	(m_param[REG_SCN3] >> 4)

#define SCANLINES_PER_ROW \
	((m_param[REG_SCN3] & 0x0f) + 1)

#define OFFSET_LINE_COUNTER \
	BIT(m_param[REG_SCN4], 7)

#define VISIBLE_FIELD_ATTRIBUTE \
	BIT(m_param[REG_SCN4], 6)

#define CURSOR_FORMAT \
	((m_param[REG_SCN4] >> 4) & 0x03)

#define HRTC_COUNT \
	(((m_param[REG_SCN4] & 0x0f) + 1) * 2)

#define DMA_BURST_COUNT \
	(1 << (m_param[REG_DMA] & 0x03))

#define DMA_BURST_SPACE \
	DMA_BURST_SPACING[(m_param[REG_DMA] >> 2) & 0x07]



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

// device type definition
const device_type I8275x = &device_creator<i8275x_device>;



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  i8275x_device - constructor
//-------------------------------------------------

i8275x_device::i8275x_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, I8275x, "I8275", tag, owner, clock, "i8275x", __FILE__),
	device_video_interface(mconfig, *this),
	m_write_irq(*this),
	m_write_drq(*this),
	m_write_hrtc(*this),
	m_write_vrtc(*this),
	m_status(0),
	m_param_idx(0),
	m_param_end(0),
	m_buffer_idx(0),
	m_fifo_next(false),
	m_buffer_dma(0),
	m_lpen(0),
	m_hlgt(0),
	m_vsp(0),
	m_gpa(0),
	m_rvv(0),
	m_lten(0),
	m_scanline(0),
	m_du(false),
	m_cursor_blink(0),
	m_char_blink(0)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void i8275x_device::device_start()
{
	// get the screen device
	m_screen->register_screen_bitmap(m_bitmap);

	// resolve callbacks
	m_display_cb.bind_relative_to(*owner());
	m_write_drq.resolve_safe();
	m_write_irq.resolve_safe();
	m_write_hrtc.resolve_safe();
	m_write_vrtc.resolve_safe();

	// allocate timers
	m_hrtc_on_timer = timer_alloc(TIMER_HRTC_ON);
	m_drq_on_timer = timer_alloc(TIMER_DRQ_ON);
	m_scanline_timer = timer_alloc(TIMER_SCANLINE);

	// state saving
	save_item(NAME(m_status));
	save_item(NAME(m_param));
	save_item(NAME(m_param_idx));
	save_item(NAME(m_param_end));
	save_item(NAME(m_buffer[0]));
	save_item(NAME(m_buffer[1]));
	save_item(NAME(m_buffer_idx));
	save_item(NAME(m_fifo_idx));
	save_item(NAME(m_fifo_next));
	save_item(NAME(m_buffer_dma));
	save_item(NAME(m_lpen));
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void i8275x_device::device_reset()
{
	memset(m_buffer, 0, sizeof(m_buffer));

	m_status &= ~ST_IE;

	m_write_irq(CLEAR_LINE);
}


//-------------------------------------------------
//  device_timer - handle timer events
//-------------------------------------------------

void i8275x_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	//int y = m_screen->vpos();
	//int x = m_screen->hpos();
	int rc = m_scanline / SCANLINES_PER_ROW;
	int lc = m_scanline % SCANLINES_PER_ROW;

	switch (id)
	{
	case TIMER_HRTC_ON:
		//if (LOG) logerror("I8275 '%s' y %u x %u HRTC 1\n", tag(), y, x);
		m_write_hrtc(1);
		break;

	case TIMER_DRQ_ON:
		//if (LOG) logerror("I8275 '%s' y %u x %u DRQ 1\n", tag(), y, x);
		m_write_drq(1);
		break;

	case TIMER_SCANLINE:
		if (!(m_status & ST_VE)) break;

		//if (LOG) logerror("I8275 '%s' y %u x %u HRTC 0\n", tag(), y, x);
		m_write_hrtc(0);

		if (m_scanline == 0)
		{
			//if (LOG) logerror("I8275 '%s' y %u x %u VRTC 0\n", tag(), y, x);
			m_write_vrtc(0);
		}
		else if (m_scanline == m_irq_scanline)
		{
			if (m_status & ST_IE)
			{
				//if (LOG) logerror("I8275 '%s' y %u x %u IRQ 1\n", tag(), y, x);
				m_status |= ST_IR;
				m_write_irq(ASSERT_LINE);
			}
		}
		else if (m_scanline == m_vrtc_scanline)
		{
			//if (LOG) logerror("I8275 '%s' y %u x %u VRTC 1\n", tag(), y, x);
			m_write_vrtc(1);

			// reset field attributes
			m_hlgt = 0;
			m_vsp = 0;
			m_gpa = 0;
			m_rvv = 0,
			m_lten = 0;

			m_du = false;

			m_cursor_blink++;
			m_cursor_blink &= 0x1f;

			m_char_blink++;
			m_char_blink &= 0x3f;
		}

		if (lc == 0)
		{
			if ((m_scanline < m_vrtc_scanline - SCANLINES_PER_ROW) && (m_buffer_idx < CHARACTERS_PER_ROW) && !m_du)
			{
				m_status |= ST_DU;
				m_du = true;
				//if (LOG) logerror("I8275 '%s' y %u x %u DMA Underrun\n", tag(), y, x);
				m_write_drq(0);
			}

			// swap line buffers
			m_buffer_dma = !m_buffer_dma;
			m_buffer_idx = 0;
			m_fifo_idx = 0;

			if ((!m_du && (m_scanline < m_vrtc_scanline - SCANLINES_PER_ROW)) || (m_scanline == m_vrtc_drq_scanline))
			{
				// start DMA burst
				m_drq_on_timer->adjust(clocks_to_attotime(DMA_BURST_SPACE));
			}
		}

		if (m_scanline < m_vrtc_scanline)
		{
			int line_counter = OFFSET_LINE_COUNTER ? ((lc - 1) % SCANLINES_PER_ROW) : lc;

			for (int sx = 0; sx < CHARACTERS_PER_ROW; sx++)
			{
				int m_lineattr = 0;
				int lten = 0;
				int vsp = 0;

				UINT8 data = m_buffer[!m_buffer_dma][sx];

				if (data & 0x80)
				{
					if ((data & 0xc0) == 0x80)
					{
						// field attribute code
						m_hlgt = (data & FAC_H) ? 1 : 0;
						m_vsp = (data & FAC_B) ? 1 : 0;
						m_gpa = (data & FAC_GG) >> 2;
						m_rvv = (data & FAC_R) ? 1 : 0;
						m_lten = (data & FAC_U) ? 1 : 0;

						if (!VISIBLE_FIELD_ATTRIBUTE)
						{
							int fifo_idx = 0;

							data = m_fifo[!m_buffer_dma][fifo_idx];

							fifo_idx++;
							fifo_idx &= 0xf;
						}
						else
						{
							vsp = 1;
						}
					}
					else
					{
						// character attribute code
					}
				}

				if (!vsp && m_vsp)
				{
					vsp = (m_char_blink < 32) ? 1 : 0;
				}

				if ((rc == m_param[REG_CUR_ROW]) && (sx == m_param[REG_CUR_COL]))
				{
					int vis = 1;

					if (!(CURSOR_FORMAT & 0x02))
					{
						vis = (m_cursor_blink < 16) ? 1 : 0;
					}

					if (CURSOR_FORMAT & 0x01)
					{
						lten = (lc == UNDERLINE) ? vis : 0;
					}
					else
					{
						lten = vis;
					}
				}

				if (!m_display_cb.isnull())
				m_display_cb(m_bitmap,
					sx * m_hpixels_per_column, // x position on screen of starting point
					m_scanline, // y position on screen
					line_counter, // current line of char
					(data & 0x7f),  // char code to be displayed
					m_lineattr,  // line attribute code
					lten | m_lten,  // light enable signal
					m_rvv,  // reverse video signal
					vsp, // video suppression
					m_gpa,  // general purpose attribute code
					m_hlgt  // highlight
				);
			}
		}

		m_scanline++;
		m_scanline %= ((CHARACTER_ROWS_PER_FRAME + VRTC_ROW_COUNT) * SCANLINES_PER_ROW);
		break;
	}
}


//-------------------------------------------------
//  read -
//-------------------------------------------------

READ8_MEMBER( i8275x_device::read )
{
	UINT8 data = 0;

	if (offset & 0x01)
	{
		data = m_status;

		if (m_status & ST_IR)
		{
			//if (LOG) logerror("I8275 '%s' IRQ 0\n", tag());
			m_write_irq(CLEAR_LINE);
		}

		m_status &= ~(ST_IR | ST_LP | ST_IC | ST_DU | ST_FO);
	}
	else
	{
		data = m_param[m_param_idx];
		m_param_idx++;

		if (m_param_idx > m_param_end)
		{
			m_status |= ST_IC;
		}
	}

	return data;
}


//-------------------------------------------------
//  write -
//-------------------------------------------------

WRITE8_MEMBER( i8275x_device::write )
{
	if (offset & 0x01)
	{
		if (LOG) logerror("I8275 '%s' Command %02x\n", tag(), data);

		switch (data >> 5)
		{
		case CMD_RESET:
			if (LOG) logerror("I8275 '%s' Reset\n", tag());

			m_status &= ~ST_IE;
			if (LOG) logerror("I8275 '%s' IRQ 0\n", tag());
			m_write_irq(CLEAR_LINE);
			m_write_drq(0);

			m_param_idx = REG_SCN1;
			m_param_end = REG_SCN4;
			break;

		case CMD_START_DISPLAY:
			{
				m_param[REG_DMA] = data;
				if (LOG) logerror("I8275 '%s' Start Display %u %u\n", tag(), DMA_BURST_COUNT, DMA_BURST_SPACE);
				m_status |= (ST_IE | ST_VE);
			}
			break;

		case CMD_STOP_DISPLAY:
			if (LOG) logerror("I8275 '%s' Stop Display\n", tag());
			m_status &= ~ST_VE;
			break;

		case CMD_READ_LIGHT_PEN:
			if (LOG) logerror("I8275 '%s' Read Light Pen\n", tag());
			m_param_idx = REG_LPEN_COL;
			m_param_end = REG_LPEN_ROW;
			break;

		case CMD_LOAD_CURSOR:
			if (LOG) logerror("I8275 '%s' Load Cursor\n", tag());
			m_param_idx = REG_CUR_COL;
			m_param_end = REG_CUR_ROW;
			break;

		case CMD_ENABLE_INTERRUPT:
			if (LOG) logerror("I8275 '%s' Enable Interrupt\n", tag());
			m_status |= ST_IE;
			break;

		case CMD_DISABLE_INTERRUPT:
			if (LOG) logerror("I8275 '%s' Disable Interrupt\n", tag());
			m_status &= ~ST_IE;
			break;

		case CMD_PRESET_COUNTERS:
			if (LOG) logerror("I8275 '%s' Preset Counters\n", tag());
			m_scanline = 0;
			break;
		}
	}
	else
	{
		if (LOG) logerror("I8275 '%s' Parameter %02x\n", tag(), data);

		m_param[m_param_idx] = data;

		if (m_param_idx == REG_SCN4)
		{
			recompute_parameters();
		}

		m_param_idx++;
	}
}


//-------------------------------------------------
//  dack_w -
//-------------------------------------------------

WRITE8_MEMBER( i8275x_device::dack_w )
{
	//int y = m_screen->vpos();
	//int x = m_screen->hpos();
	//if (LOG) logerror("I8275 '%s' y %u x %u DACK %04x:%02x %u\n", tag(), y, x, offset, data, m_buffer_idx);

	m_write_drq(0);

	if (m_fifo_next)
	{
		if (m_fifo_idx == 16)
		{
			m_fifo_idx = 0;
			m_status |= ST_FO;
		}

		m_fifo[m_buffer_dma][m_fifo_idx++] = data;

		m_fifo_next = false;
	}
	else
	{
		m_buffer[m_buffer_dma][m_buffer_idx++] = data;

		if (!VISIBLE_FIELD_ATTRIBUTE && ((data & 0xc0) == 0x80))
		{
			m_fifo_next = true;
		}

		if (m_buffer_idx == CHARACTERS_PER_ROW)
		{
			// stop DMA
		}
		else if (!(m_buffer_idx % DMA_BURST_COUNT))
		{
			m_drq_on_timer->adjust(clocks_to_attotime(DMA_BURST_SPACE));
		}
		else
		{
			m_drq_on_timer->adjust(attotime::zero);	
		}
	}
}


//-------------------------------------------------
//  lpen_w -
//-------------------------------------------------

WRITE_LINE_MEMBER( i8275x_device::lpen_w )
{
	if (!m_lpen && state)
	{
		m_param[REG_LPEN_COL] = m_screen->hpos() / m_hpixels_per_column;
		m_param[REG_LPEN_ROW] = m_screen->vpos() / SCANLINES_PER_ROW;

		m_status |= ST_LP;
	}

	m_lpen = state;
}


//-------------------------------------------------
//  screen_update -
//-------------------------------------------------

UINT32 i8275x_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	if (!(m_status & ST_VE))
	{
		m_bitmap.fill(rgb_t::black);
	}

	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);

	return 0;
}


//-------------------------------------------------
//  recompute_parameters -
//-------------------------------------------------

void i8275x_device::recompute_parameters()
{
	int y = m_screen->vpos();

	int horiz_pix_total = (CHARACTERS_PER_ROW + HRTC_COUNT) * m_hpixels_per_column;
	int vert_pix_total = (CHARACTER_ROWS_PER_FRAME + VRTC_ROW_COUNT) * SCANLINES_PER_ROW;
	attoseconds_t refresh = m_screen->frame_period().attoseconds;
	int max_visible_x = (CHARACTERS_PER_ROW * m_hpixels_per_column) - 1;
	int max_visible_y = (CHARACTER_ROWS_PER_FRAME * SCANLINES_PER_ROW) - 1;

	if (LOG) logerror("width %u height %u max_x %u max_y %u refresh %f\n", horiz_pix_total, vert_pix_total, max_visible_x, max_visible_y, 1 / ATTOSECONDS_TO_DOUBLE(refresh));

	rectangle visarea;
	visarea.set(0, max_visible_x, 0, max_visible_y);
	m_screen->configure(horiz_pix_total, vert_pix_total, visarea, refresh);

	int hrtc_on_pos = CHARACTERS_PER_ROW * m_hpixels_per_column;
	m_hrtc_on_timer->adjust(m_screen->time_until_pos(y, hrtc_on_pos), 0, m_screen->scan_period());

	m_irq_scanline = (CHARACTER_ROWS_PER_FRAME - 1) * SCANLINES_PER_ROW;
	m_vrtc_scanline = CHARACTER_ROWS_PER_FRAME * SCANLINES_PER_ROW;
	m_vrtc_drq_scanline = vert_pix_total - SCANLINES_PER_ROW;

	if (LOG) logerror("irq_y %u vrtc_y %u drq_y %u\n", m_irq_scanline, m_vrtc_scanline, m_vrtc_drq_scanline);

	m_scanline_timer->adjust(m_screen->time_until_pos(0, 0), 0, m_screen->scan_period());
}
