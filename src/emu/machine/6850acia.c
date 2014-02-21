/*********************************************************************

    6850acia.c

    6850 ACIA code

*********************************************************************/

#include "emu.h"
#include "6850acia.h"


/***************************************************************************
    MACROS
***************************************************************************/

#define LOG 0

/***************************************************************************
    LOCAL VARIABLES
***************************************************************************/

const int acia6850_device::counter_divide_select[4] =
{
	1,
	16,
	64,
	0
};

const int acia6850_device::word_select[8][3] =
{
	// word length, parity, stop bits
	{ 7+1, EVEN, 2 },
	{ 7+1, ODD,  2 },
	{ 7+1, EVEN, 1 },
	{ 7+1, ODD,  1 },
	{ 8,   NONE, 2 },
	{ 8,   NONE, 1 },
	{ 8+1, EVEN, 1 },
	{ 8+1, ODD,  1 }
};

const int acia6850_device::transmitter_control[4][3] =
{
	// rts, brk, tx irq
	{ 0, 0, 0 },
	{ 0, 0, 1 },
	{ 1, 0, 0 },
	{ 0, 1, 0 }
};


/***************************************************************************
    LIVE DEVICE
***************************************************************************/

// device type definition
const device_type ACIA6850 = &device_creator<acia6850_device>;

//-------------------------------------------------
//  acia6850_device - constructor
//-------------------------------------------------

acia6850_device::acia6850_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, ACIA6850, "6850 ACIA", tag, owner, clock, "acia6850", __FILE__),
	m_txd_handler(*this),
	m_rts_handler(*this),
	m_irq_handler(*this),
	m_status(ACIA6850_STATUS_TDRE),
	m_first_master_reset(true),
	m_dcd_irq_pending(false),
	m_overrun_pending(false),
	m_divide(0),
	m_rts(0),
	m_dcd(0),
	m_irq(0),
	m_txc(0),
	m_txd(0),
	m_tx_irq_enable(false),
	m_rxc(0),
	m_rxd(1),
	m_rx_irq_enable(false)
{
}

acia6850_device::acia6850_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
	: device_t(mconfig, type, name, tag, owner, clock, shortname, source),
	m_txd_handler(*this),
	m_rts_handler(*this),
	m_irq_handler(*this),
	m_status(ACIA6850_STATUS_TDRE),
	m_first_master_reset(true),
	m_dcd_irq_pending(false),
	m_overrun_pending(false),
	m_divide(0),
	m_rts(0),
	m_dcd(0),
	m_irq(0),
	m_txc(0),
	m_txd(0),
	m_tx_irq_enable(false),
	m_rxc(0),
	m_rxd(1),
	m_rx_irq_enable(false)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void acia6850_device::device_start()
{
	// resolve callbacks
	m_txd_handler.resolve_safe();
	m_rts_handler.resolve_safe();
	m_irq_handler.resolve_safe();

	save_item(NAME(m_status));
	save_item(NAME(m_tdr));
	save_item(NAME(m_rdr));

	save_item(NAME(m_first_master_reset));
	save_item(NAME(m_dcd_irq_pending));
	save_item(NAME(m_overrun_pending));

	save_item(NAME(m_divide));
	save_item(NAME(m_bits));
	save_item(NAME(m_stopbits));
	save_item(NAME(m_parity));
	save_item(NAME(m_brk));

	save_item(NAME(m_rts));
	save_item(NAME(m_dcd));
	save_item(NAME(m_irq));

	save_item(NAME(m_txc));
	save_item(NAME(m_txd));
	save_item(NAME(m_tx_state));
	save_item(NAME(m_tx_bits));
	save_item(NAME(m_tx_shift));
	save_item(NAME(m_tx_parity));
	save_item(NAME(m_tx_counter));
	save_item(NAME(m_tx_irq_enable));

	save_item(NAME(m_rxc));
	save_item(NAME(m_rxd));
	save_item(NAME(m_rx_state));
	save_item(NAME(m_rx_bits));
	save_item(NAME(m_rx_shift));
	save_item(NAME(m_rx_parity));
	save_item(NAME(m_rx_counter));
	save_item(NAME(m_rx_irq_enable));

	output_txd(1);
	output_rts(1);
	output_irq(1);
}

READ8_MEMBER( acia6850_device::status_r )
{
	UINT8 status = m_status;

	if (status & ACIA6850_STATUS_CTS)
	{
		status &= ~ACIA6850_STATUS_TDRE;
	}

	if (m_dcd_irq_pending == DCD_IRQ_READ_STATUS)
	{
		m_dcd_irq_pending = DCD_IRQ_READ_DATA;
	}

	return status;
}

WRITE8_MEMBER( acia6850_device::control_w )
{
	if (LOG) logerror("MC6850 '%s' Control: %02x\n", tag(), data);

	// CR0 and CR1
	int counter_divide_select_bits = (data >> 0) & 3;
	m_divide = counter_divide_select[counter_divide_select_bits];

	// CR2, CR3 and CR4
	int word_select_bits = (data >> 2) & 7;
	m_bits = word_select[word_select_bits][0];
	m_parity = word_select[word_select_bits][1];
	m_stopbits = word_select[word_select_bits][2];

	// CR5 & CR6
	int transmitter_control_bits = (data >> 5) & 3;
	int rts = transmitter_control[transmitter_control_bits][0];
	m_brk = transmitter_control[transmitter_control_bits][1];
	m_tx_irq_enable = transmitter_control[transmitter_control_bits][2];

	// Receive Interrupt Enable Bit (CR7)
	m_rx_irq_enable = (data >> 7) & 1;

	if (m_divide == 0)
	{
		if (m_first_master_reset)
		{
			/// TODO: find out whether you need to leave master reset before it stops counting as the first
			rts = 1;
			m_first_master_reset = false;
		}

		m_dcd_irq_pending = DCD_IRQ_NONE;
		m_overrun_pending = false;

		m_rx_state = START;
		m_rx_counter = 0;

		m_tx_state = START;
		output_txd(1);

		m_status &= ACIA6850_STATUS_CTS;

		/// TODO: find out if this should be set as data sheet says status is cleared apart from cts & dcd
		m_status |= ACIA6850_STATUS_TDRE;

		if (m_dcd)
		{
			m_status |= ACIA6850_STATUS_DCD;
		}
	}

	output_rts(rts);

	update_irq();
}


int acia6850_device::calculate_txirq()
{
	return !(m_tx_irq_enable && ((m_status & ACIA6850_STATUS_TDRE) && !(m_status & ACIA6850_STATUS_CTS)));
}

int acia6850_device::calculate_rxirq()
{
	return !(m_rx_irq_enable && ((m_status & ACIA6850_STATUS_RDRF) || m_dcd_irq_pending != DCD_IRQ_NONE));
}

void acia6850_device::update_irq()
{
	output_irq(calculate_txirq() && calculate_rxirq());
}

WRITE8_MEMBER( acia6850_device::data_w )
{
	if (LOG) logerror("MC6850 '%s' Data: %02x\n", tag(), data);

	/// TODO: find out if data stored during master reset is sent after divider is set
	if (m_divide == 0)
	{
		logerror("%s:ACIA %p: Data write while in reset!\n", machine().describe_context(), this);
	}

	/// TODO: find out what happens if TDRE is already clear when you write
	m_tdr = data;
	m_status &= ~ACIA6850_STATUS_TDRE;

	update_irq();
}

READ8_MEMBER( acia6850_device::data_r )
{
	if (m_overrun_pending)
	{
		m_status |= ACIA6850_STATUS_OVRN;
		m_overrun_pending = false;
	}
	else
	{
		m_status &= ~ACIA6850_STATUS_OVRN;
		m_status &= ~ACIA6850_STATUS_RDRF;
	}

	if (m_dcd_irq_pending == DCD_IRQ_READ_DATA)
	{
		m_dcd_irq_pending = DCD_IRQ_NONE;
	}

	update_irq();

	return m_rdr;
}

DECLARE_WRITE_LINE_MEMBER( acia6850_device::write_cts )
{
	if (state)
	{
		m_status |= ACIA6850_STATUS_CTS;
	}
	else
	{
		m_status &= ~ACIA6850_STATUS_CTS;
	}
}

DECLARE_WRITE_LINE_MEMBER( acia6850_device::write_dcd )
{
	m_dcd = state;
}

WRITE_LINE_MEMBER( acia6850_device::write_rxc )
{
	if (m_rxc != state)
	{
		m_rxc = state;

		if (state && m_divide > 0)
		{
			if (m_dcd)
			{
				if (!(m_status & ACIA6850_STATUS_DCD))
				{
					m_status |= ACIA6850_STATUS_DCD;
					m_dcd_irq_pending = DCD_IRQ_READ_STATUS;
				}

				m_rx_state = START;
				m_rx_counter = 0;
			}
			else
			{
				if (m_dcd_irq_pending == DCD_IRQ_NONE)
				{
					m_status &= ~ACIA6850_STATUS_DCD;
				}
	
				m_rx_counter++;

				switch (m_rx_state)
				{
				case START:
					if (m_rxd == 0)
					{
						if (m_rx_counter == 1)
						{
							if (LOG) logerror("MC6850 '%s': RX START BIT\n", tag());
						}

						if (m_rx_counter >= m_divide / 2)
						{
							m_rx_state = DATA;
							m_rx_counter = 0;
							m_rx_shift = 0;
							m_rx_parity = 0;
							m_rx_bits = 0;
						}
					}
					else
					{
						m_rx_counter = 0;

						if (LOG) logerror("MC6850 '%s': RX FALSE START BIT\n", tag());
					}
					break;

				case DATA:
					if (m_rx_counter == m_divide)
					{
						m_rx_counter = 0;
						m_rx_bits++;

						if (m_rx_bits != m_bits || m_parity == NONE)
						{
							if (LOG) logerror("MC6850 '%s': RX DATA BIT %d %d\n", tag(), m_rx_bits - 1, m_rxd);
						}
						else
						{
							if (LOG) logerror("MC6850 '%s': RX PARITY BIT %x\n", tag(), m_rxd);
						}

						if (m_rxd)
						{
							m_rx_shift |= 1 << m_bits;
						}

						m_rx_shift >>= 1;

						m_rx_parity ^= m_rxd;

						if (m_rx_bits == m_bits)
						{
							if (m_status & ACIA6850_STATUS_RDRF)
							{
								m_overrun_pending = true;
							}
							else
							{
								/// TODO: find out if this is the correct place to calculate parity
								if (m_parity == ODD)
								{
									m_rx_parity = !m_rx_parity;
								}

								if (m_parity != NONE && !m_rx_parity)
								{
									m_status |= ACIA6850_STATUS_PE;
								}
								else
								{
									m_status &= ~ACIA6850_STATUS_PE;
								}

								m_rdr = m_rx_shift;

								if (m_bits == 8 && m_parity != NONE)
								{
									m_rdr &= 0x7f;
								}

								m_status |= ACIA6850_STATUS_RDRF;
							}

							m_rx_state = STOP;
						}
					}
					break;

				case STOP:
					if (m_rx_counter == m_divide)
					{
						m_rx_counter = 0;

						if (LOG) logerror("MC6850 '%s': RX STOP BIT\n", tag());
		
						if (m_rxd != 0)
						{
							m_status |= ACIA6850_STATUS_FE;
						}
						else
						{
							m_status &= ~ACIA6850_STATUS_FE;
						}

						/// TODO: find out if 6850 only waits for 1 STOP bit when receiving
						m_rx_state = START;
					}
					break;
				}
			}

			update_irq();
		}
	}
}

DECLARE_WRITE_LINE_MEMBER( acia6850_device::write_rxd )
{
	m_rxd = state;
}

WRITE_LINE_MEMBER( acia6850_device::write_txc )
{
	if (m_txc != state)
	{
		m_txc = state;

		if (!state && m_divide > 0)
		{
			m_tx_counter++;

			/// TODO: check txd is correctly generated, check atarist mcu is reading data, start checking receive data.
			switch (m_tx_state)
			{
			case START:
				m_tx_counter = 0;

				if (!(m_status & ACIA6850_STATUS_TDRE) && !(m_status & ACIA6850_STATUS_CTS))
				{
					if (LOG) logerror("MC6850 '%s': TX DATA %x\n", tag(), m_tdr);

					m_tx_state = DATA;
					m_tx_shift = m_tdr;
					m_tx_bits = 0;
					m_tx_parity = 0;
					m_status |= ACIA6850_STATUS_TDRE;

					if (LOG) logerror("MC6850 '%s': TX START BIT\n", tag());

					output_txd(0);
				}
				else
				{
					/// TODO: find out if break stops transmitter
					output_txd(!m_brk);
				}
				break;

			case DATA:
				if (m_tx_counter == m_divide)
				{
					m_tx_counter = 0;
					m_tx_bits++;

					if (m_tx_bits != m_bits || m_parity == NONE)
					{
						output_txd(m_tx_shift & 1);
						m_tx_parity ^= m_txd;
						m_tx_shift >>= 1;

						if (LOG) logerror("MC6850 '%s': TX DATA BIT %d %d\n", tag(), m_tx_bits, m_txd);
					}
					else
					{
						/// TODO: find out if this is the correct place to calculate parity
						if (m_parity == ODD)
						{
							m_tx_parity = !m_tx_parity;
						}

						output_txd(m_tx_parity);

						if (LOG) logerror("MC6850 '%s': TX PARITY BIT %d\n", tag(), m_txd);
					}

					if (m_tx_bits == m_bits)
					{
						m_tx_state = STOP;
						m_tx_bits = 0;
					}
				}
				break;

			case STOP:
				if (m_tx_counter == m_divide)
				{
					m_tx_bits++;

					if (LOG) logerror("MC6850 '%s': TX STOP BIT %d\n", tag(), m_tx_bits);

					output_txd(1);

					if (m_tx_bits == m_stopbits)
					{
						m_tx_state = START;
					}
				}
				break;
			}
		}

		update_irq();
	}
}

void acia6850_device::output_txd(int txd)
{
	if (m_txd != txd)
	{
		m_txd = txd;

		m_txd_handler(m_txd);
	}
}

void acia6850_device::output_rts(int rts)
{
	if (m_rts != rts)
	{
		m_rts = rts;
		m_rts_handler(m_rts);
	}
}

void acia6850_device::output_irq(int irq)
{
	if (m_irq != irq)
	{
		m_irq = irq;

		if (irq)
		{
			m_status &= ~ACIA6850_STATUS_IRQ;
		}
		else
		{
			m_status |= ACIA6850_STATUS_IRQ;
		}

		m_irq_handler(!m_irq);
	}
}
