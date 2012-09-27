/***************************************************************************

  RIOT 6532 emulation

The timer seems to follow these rules:
- When the timer flag changes from 0 to 1 the timer continues to count
  down at a 1 cycle rate.
- When the timer is being read or written the timer flag is reset.
- When the timer flag is set and the timer contents are 0, the counting
  stops.

***************************************************************************/

#include "emu.h"
#include "6532riot.h"


//**************************************************************************
//  CONSTANTS
//**************************************************************************

// device type definition
const device_type RIOT6532 = &device_creator<riot6532_device>;

enum
{
	TIMER_IDLE,
	TIMER_COUNTING,
	TIMER_FINISHING
};

#define TIMER_FLAG		0x80
#define PA7_FLAG		0x40




/***************************************************************************
    INTERNAL FUNCTIONS
***************************************************************************/

/*-------------------------------------------------
    update_irqstate - update the IRQ state
    based on interrupt enables
-------------------------------------------------*/

void riot6532_device::update_irqstate()
{
	int irq = (m_irqstate & m_irqenable) ? ASSERT_LINE : CLEAR_LINE;

	if (m_irq != irq)
	{
		m_irq_func(irq);
		m_irq = irq;
	}
}


/*-------------------------------------------------
    apply_ddr - combine inputs and outputs
    according to the DDR
-------------------------------------------------*/

UINT8 riot6532_device::apply_ddr(const riot6532_port *port)
{
	return (port->m_out & port->m_ddr) | (port->m_in & ~port->m_ddr);
}


/*-------------------------------------------------
    update_pa7_state - see if PA7 has changed
    and signal appropriately
-------------------------------------------------*/

void riot6532_device::update_pa7_state()
{
	UINT8 data = apply_ddr(&m_port[0]) & 0x80;

	/* if the state changed in the correct direction, set the PA7 flag and update IRQs */
	if ((m_pa7prev ^ data) && (m_pa7dir ^ data) == 0)
	{
		m_irqstate |= PA7_FLAG;
		update_irqstate();
	}
	m_pa7prev = data;
}


/*-------------------------------------------------
    get_timer - return the current timer value
-------------------------------------------------*/

UINT8 riot6532_device::get_timer()
{
	/* if idle, return 0 */
	if (m_timerstate == TIMER_IDLE)
	{
		return 0;
	}

	/* if counting, return the number of ticks remaining */
	else if (m_timerstate == TIMER_COUNTING)
	{
		return m_timer->remaining().as_ticks(clock()) >> m_timershift;
	}

	/* if finishing, return the number of ticks without the shift */
	else
	{
		return m_timer->remaining().as_ticks(clock());
	}
}



/*-------------------------------------------------
    timer_end_callback - callback to process the
    timer
-------------------------------------------------*/

TIMER_CALLBACK( riot6532_device::timer_end_callback )
{
	riot6532_device *via = reinterpret_cast<riot6532_device *>(ptr);
	via->timer_end();
}

void riot6532_device::timer_end()
{
	assert(m_timerstate != TIMER_IDLE);

	/* if we finished counting, switch to the finishing state */
	if(m_timerstate == TIMER_COUNTING)
	{
		m_timerstate = TIMER_FINISHING;
		m_timer->adjust(attotime::from_ticks(256, clock()));

		/* signal timer IRQ as well */
		m_irqstate |= TIMER_FLAG;
		update_irqstate();
	}

	/* if we finished finishing, keep spinning */
	else if (m_timerstate == TIMER_FINISHING)
	{
		m_timer->adjust(attotime::from_ticks(256, clock()));
	}
}



/***************************************************************************
    I/O ACCESS
***************************************************************************/

/*-------------------------------------------------
    riot6532_w - master I/O write access
-------------------------------------------------*/

WRITE8_DEVICE_HANDLER( riot6532_w )
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	via->reg_w(offset, data);
}

WRITE8_MEMBER( riot6532_device::write )
{
	reg_w(offset, data);
}

void riot6532_device::reg_w(UINT8 offset, UINT8 data)
{
	/* if A4 == 1 and A2 == 1, we are writing to the timer */
	if ((offset & 0x14) == 0x14)
	{
		static const UINT8 timershift[4] = { 0, 3, 6, 10 };
		attotime curtime = machine().time();
		INT64 target;

		/* A0-A1 contain the timer divisor */
		m_timershift = timershift[offset & 3];

		/* A3 contains the timer IRQ enable */
		if (offset & 8)
			m_irqenable |= TIMER_FLAG;
		else
			m_irqenable &= ~TIMER_FLAG;

		/* writes here clear the timer flag */
		if (m_timerstate != TIMER_FINISHING || get_timer() != 0xff)
		{
			m_irqstate &= ~TIMER_FLAG;
		}
		update_irqstate();

		/* update the timer */
		m_timerstate = TIMER_COUNTING;
		target = curtime.as_ticks(clock()) + 1 + (data << m_timershift);
		m_timer->adjust(attotime::from_ticks(target, clock()) - curtime);
	}

	/* if A4 == 0 and A2 == 1, we are writing to the edge detect control */
	else if ((offset & 0x14) == 0x04)
	{
		/* A1 contains the A7 IRQ enable */
		if (offset & 2)
		{
			m_irqenable |= PA7_FLAG;
		}
		else
		{
			m_irqenable &= ~PA7_FLAG;
		}

		/* A0 specifies the edge detect direction: 0=negative, 1=positive */
		m_pa7dir = (offset & 1) << 7;
	}

	/* if A4 == anything and A2 == 0, we are writing to the I/O section */
	else
	{
		/* A1 selects the port */
		riot6532_port *port = &m_port[(offset >> 1) & 1];

		/* if A0 == 1, we are writing to the port's DDR */
		if (offset & 1)
		{
			port->m_ddr = data;
		}

		/* if A0 == 0, we are writing to the port's output */
		else
		{
			port->m_out = data;
			port->m_out_func(0, data);
		}

		/* writes to port A need to update the PA7 state */
		if (port == &m_port[0])
		{
			update_pa7_state();
		}
	}
}


/*-------------------------------------------------
    riot6532_r - master I/O read access
-------------------------------------------------*/

READ8_DEVICE_HANDLER( riot6532_r )
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	return via->reg_r(offset);
}

READ8_MEMBER( riot6532_device::read )
{
	return reg_r(offset);
}

UINT8 riot6532_device::reg_r(UINT8 offset)
{
	UINT8 val = 0;

	/* if A2 == 1 and A0 == 1, we are reading interrupt flags */
	if ((offset & 0x05) == 0x05)
	{
		val = m_irqstate;

		/* implicitly clears the PA7 flag */
		m_irqstate &= ~PA7_FLAG;
		update_irqstate();
	}

	/* if A2 == 1 and A0 == 0, we are reading the timer */
	else if ((offset & 0x05) == 0x04)
	{
		val = get_timer();

		/* A3 contains the timer IRQ enable */
		if (offset & 8)
		{
			m_irqenable |= TIMER_FLAG;
		}
		else
		{
			m_irqenable &= ~TIMER_FLAG;
		}

		/* implicitly clears the timer flag */
		if (m_timerstate != TIMER_FINISHING || val != 0xff)
		{
			m_irqstate &= ~TIMER_FLAG;
		}
		update_irqstate();
	}

	/* if A2 == 0 and A0 == anything, we are reading from ports */
	else
	{
		/* A1 selects the port */
		riot6532_port *port = &m_port[(offset >> 1) & 1];

		/* if A0 == 1, we are reading the port's DDR */
		if (offset & 1)
		{
			val = port->m_ddr;
		}

		/* if A0 == 0, we are reading the port as an input */
		else
		{
			/* call the input callback if it exists */
			if (!port->m_in_func.isnull())
			{
				port->m_in = port->m_in_func(0);

				/* changes to port A need to update the PA7 state */
				if (port == &m_port[0])
				{
					update_pa7_state();
				}
			}

			/* apply the DDR to the result */
			val = apply_ddr(port);
		}
	}
	return val;
}


/*-------------------------------------------------
    porta_in_set - set port A input value
-------------------------------------------------*/

void riot6532_porta_in_set(device_t *device, UINT8 data, UINT8 mask)
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	via->porta_in_set(data, mask);
}

void riot6532_device::porta_in_set(UINT8 data, UINT8 mask)
{
	m_port[0].m_in = (m_port[0].m_in & ~mask) | (data & mask);
	update_pa7_state();
}


/*-------------------------------------------------
    portb_in_set - set port B input value
-------------------------------------------------*/

void riot6532_portb_in_set(device_t *device, UINT8 data, UINT8 mask)
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	via->portb_in_set(data, mask);
}

void riot6532_device::portb_in_set(UINT8 data, UINT8 mask)
{
	m_port[1].m_in = (m_port[1].m_in & ~mask) | (data & mask);
}


/*-------------------------------------------------
    porta_in_get - return port A input value
-------------------------------------------------*/

UINT8 riot6532_porta_in_get(device_t *device)
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	return via->porta_in_get();
}

UINT8 riot6532_device::porta_in_get()
{
	return m_port[0].m_in;
}


/*-------------------------------------------------
    portb_in_get - return port B input value
-------------------------------------------------*/

UINT8 riot6532_portb_in_get(device_t *device)
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	return via->portb_in_get();
}

UINT8 riot6532_device::portb_in_get()
{
	return m_port[1].m_in;
}


/*-------------------------------------------------
    porta_in_get - return port A output value
-------------------------------------------------*/

UINT8 riot6532_porta_out_get(device_t *device)
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	return via->porta_out_get();
}

UINT8 riot6532_device::porta_out_get()
{
	return m_port[0].m_out;
}


/*-------------------------------------------------
    portb_in_get - return port B output value
-------------------------------------------------*/

UINT8 riot6532_portb_out_get(device_t *device)
{
	riot6532_device *via = downcast<riot6532_device *>(device);
	return via->portb_out_get();
}

UINT8 riot6532_device::portb_out_get()
{
	return m_port[1].m_out;
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  riot6532_device - constructor
//-------------------------------------------------

riot6532_device::riot6532_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, RIOT6532, "6532 (RIOT)", tag, owner, clock),
	  m_irq(CLEAR_LINE)
{
}


//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void riot6532_device::device_config_complete()
{
	// inherit a copy of the static data
	const riot6532_interface *intf = reinterpret_cast<const riot6532_interface *>(static_config());
	if (intf != NULL)
	{
		*static_cast<riot6532_interface *>(this) = *intf;
	}

	// or initialize to defaults if none provided
	else
	{
		memset(&m_in_a_cb, 0, sizeof(m_in_a_cb));
		memset(&m_in_b_cb, 0, sizeof(m_in_b_cb));
		memset(&m_out_a_cb, 0, sizeof(m_out_a_cb));
		memset(&m_out_b_cb, 0, sizeof(m_out_b_cb));
		memset(&m_irq_cb, 0, sizeof(m_irq_cb));
	}
}


/*-------------------------------------------------
    device_start - device-specific startup
-------------------------------------------------*/

void riot6532_device::device_start()
{
	/* configure the ports */
	m_port[0].m_in_func.resolve(m_in_a_cb, *this);
	m_port[0].m_out_func.resolve(m_out_a_cb, *this);
	m_port[1].m_in_func.resolve(m_in_b_cb, *this);
	m_port[1].m_out_func.resolve(m_out_b_cb, *this);

	/* resolve irq func */
	m_irq_func.resolve(m_irq_cb, *this);

	/* allocate timers */
	m_timer = machine().scheduler().timer_alloc(FUNC(timer_end_callback), (void *)this);

	/* register for save states */
	save_item(NAME(m_port[0].m_in));
	save_item(NAME(m_port[0].m_out));
	save_item(NAME(m_port[0].m_ddr));
	save_item(NAME(m_port[1].m_in));
	save_item(NAME(m_port[1].m_out));
	save_item(NAME(m_port[1].m_ddr));

	save_item(NAME(m_irqstate));
	save_item(NAME(m_irqenable));
	save_item(NAME(m_irq));

	save_item(NAME(m_pa7dir));
	save_item(NAME(m_pa7prev));

	save_item(NAME(m_timershift));
	save_item(NAME(m_timerstate));
}



/*-------------------------------------------------
    device_reset - device-specific reset
-------------------------------------------------*/

void riot6532_device::device_reset()
{
	/* reset I/O states */
	m_port[0].m_out = 0;
	m_port[0].m_ddr = 0;
	m_port[1].m_out = 0;
	m_port[1].m_ddr = 0;

	/* reset IRQ states */
	m_irqenable = 0;
	m_irqstate = 0;
	update_irqstate();

	/* reset PA7 states */
	m_pa7dir = 0;
	m_pa7prev = 0;

	/* reset timer states */
	m_timershift = 0;
	m_timerstate = TIMER_IDLE;
	m_timer->adjust(attotime::never);
}
