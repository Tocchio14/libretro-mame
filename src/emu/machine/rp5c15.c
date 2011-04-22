/**********************************************************************

    Ricoh RP5C15 Real Time Clock emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

/*

    TODO:

    - 12 hour clock
    - test register
    - timer reset

*/

#include "rp5c15.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define LOG 0


// registers
enum
{
	REGISTER_1_SECOND = 0, REGISTER_CLOCK_OUTPUT = REGISTER_1_SECOND,
	REGISTER_10_SECOND, REGISTER_ADJUST = REGISTER_10_SECOND,
	REGISTER_1_MINUTE,
	REGISTER_10_MINUTE,
	REGISTER_1_HOUR,
	REGISTER_10_HOUR,
	REGISTER_DAY_OF_THE_WEEK,
	REGISTER_1_DAY,
	REGISTER_10_DAY,
	REGISTER_1_MONTH,
	REGISTER_10_MONTH, REGISTER_12_24_SELECT = REGISTER_10_MONTH,
	REGISTER_1_YEAR, REGISTER_LEAP_YEAR = REGISTER_1_YEAR,
	REGISTER_10_YEAR,
	REGISTER_MODE,
	REGISTER_TEST,
	REGISTER_RESET
};


// clock output select
enum
{
	CLKOUT_Z = 0,
	CLKOUT_16384_HZ,
	CLKOUT_1024_HZ,
	CLKOUT_128_HZ,
	CLKOUT_16_HZ,
	CLKOUT_1_HZ,
	CLKOUT_1_DIV_60_HZ,
	CLKOUT_L
};


// register write mask
static const int REGISTER_WRITE_MASK[2][16] =
{
	{ 0xf, 0x7, 0xf, 0x7, 0xf, 0x3, 0x7, 0xf, 0x3, 0xf, 0x1, 0xf, 0xf, 0xf, 0xf, 0xf },
	{ 0x3, 0x1, 0xf, 0x7, 0xf, 0x3, 0x7, 0xf, 0x3, 0x0, 0x1, 0x3, 0x0, 0xf, 0xf, 0xf }
};


// days per month
static const int DAYS_PER_MONTH[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


// modes
enum
{
	MODE00 = 0,
	MODE01
};


// mode register
#define MODE_TIMER_EN		0x08
#define MODE_ALARM_EN		0x04
#define MODE_MASK			0x01


// test register
#define TEST_3				0x08
#define TEST_2				0x04
#define TEST_1				0x02
#define TEST_0				0x01


// reset register
#define RESET_ALARM			0x08
#define RESET_TIMER			0x04
#define RESET_16_HZ			0x02
#define RESET_1_HZ			0x01



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// devices
const device_type RP5C15 = rp5c15_device_config::static_alloc_device_config;



//**************************************************************************
//  DEVICE CONFIGURATION
//**************************************************************************

//-------------------------------------------------
//  rp5c15_device_config - constructor
//-------------------------------------------------

rp5c15_device_config::rp5c15_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock)
	: device_config(mconfig, static_alloc_device_config, "RP5C15", tag, owner, clock),
	  device_config_rtc_interface(mconfig, *this)
{
}


//-------------------------------------------------
//  static_alloc_device_config - allocate a new
//  configuration object
//-------------------------------------------------

device_config *rp5c15_device_config::static_alloc_device_config(const machine_config &mconfig, const char *tag, const device_config *owner, UINT32 clock)
{
	return global_alloc(rp5c15_device_config(mconfig, tag, owner, clock));
}


//-------------------------------------------------
//  alloc_device - allocate a new device object
//-------------------------------------------------

device_t *rp5c15_device_config::alloc_device(running_machine &machine) const
{
	return auto_alloc(machine, rp5c15_device(machine, *this));
}


//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void rp5c15_device_config::device_config_complete()
{
	// inherit a copy of the static data
	const rp5c15_interface *intf = reinterpret_cast<const rp5c15_interface *>(static_config());
	if (intf != NULL)
		*static_cast<rp5c15_interface *>(this) = *intf;

	// or initialize to defaults if none provided
	else
	{
		memset(&m_out_alarm_func, 0, sizeof(m_out_alarm_func));
		memset(&m_out_clkout_func, 0, sizeof(m_out_clkout_func));
	}
}



//**************************************************************************
//  INLINE HELPERS
//**************************************************************************

//-------------------------------------------------
//  set_alarm_line -
//-------------------------------------------------

inline void rp5c15_device::set_alarm_line()
{
	int alarm = ((m_mode & MODE_ALARM_EN) ? m_alarm_on : 1) &
				((m_reset & RESET_16_HZ) ? 1 : m_16hz) &
				((m_reset & RESET_1_HZ) ? 1 : m_1hz);

	if (m_alarm != alarm)
	{
		if (LOG) logerror("RP5C15 '%s' Alarm %u\n", tag(), alarm);

		devcb_call_write_line(&m_out_alarm_func, alarm);
		m_alarm = alarm;
	}
}


//-------------------------------------------------
//  read_counter -
//-------------------------------------------------

inline int rp5c15_device::read_counter(int counter)
{
	return (m_reg[MODE00][counter + 1] * 10) + m_reg[MODE00][counter];
}


//-------------------------------------------------
//  write_counter -
//-------------------------------------------------

inline void rp5c15_device::write_counter(int counter, int value)
{
	m_reg[MODE00][counter] = value % 10;
	m_reg[MODE00][counter + 1] = value / 10;
}


//-------------------------------------------------
//  advance_seconds -
//-------------------------------------------------

inline void rp5c15_device::advance_seconds()
{
	int seconds = read_counter(REGISTER_1_SECOND);

	seconds++;

	if (seconds > 59)
	{
		seconds = 0;

		advance_minutes();
	}

	write_counter(REGISTER_1_SECOND, seconds);
}


//-------------------------------------------------
//  advance_minutes -
//-------------------------------------------------

inline void rp5c15_device::advance_minutes()
{
	int minutes = read_counter(REGISTER_1_MINUTE);
	int hours = read_counter(REGISTER_1_HOUR);
	int days = read_counter(REGISTER_1_DAY);
	int month = read_counter(REGISTER_1_MONTH);
	int year = read_counter(REGISTER_1_YEAR);
	int day_of_week = m_reg[MODE00][REGISTER_DAY_OF_THE_WEEK];

	minutes++;

	if (minutes > 59)
	{
		minutes = 0;
		hours++;
	}

	if (hours > 23)
	{
		hours = 0;
		days++;
		day_of_week++;
	}

	if (day_of_week > 6)
	{
		day_of_week++;
	}

	if (days > DAYS_PER_MONTH[month - 1])
	{
		days = 1;
		month++;
	}

	if (month > 12)
	{
		month = 1;
		year++;
		m_reg[MODE01][REGISTER_LEAP_YEAR]++;
		m_reg[MODE01][REGISTER_LEAP_YEAR] &= 0x03;
	}

	if (year > 99)
	{
		year = 0;
	}

	write_counter(REGISTER_1_MINUTE, minutes);
	write_counter(REGISTER_1_HOUR, hours);
	write_counter(REGISTER_1_DAY, days);
	write_counter(REGISTER_1_MONTH, month);
	write_counter(REGISTER_1_YEAR, year);
	m_reg[MODE00][REGISTER_DAY_OF_THE_WEEK] = day_of_week;

	check_alarm();
	set_alarm_line();
}


//-------------------------------------------------
//  adjust_seconds -
//-------------------------------------------------

inline void rp5c15_device::adjust_seconds()
{
	int seconds = read_counter(REGISTER_1_SECOND);

	if (seconds < 30)
	{
		write_counter(REGISTER_1_SECOND, 0);
	}
	else
	{
		write_counter(REGISTER_1_SECOND, 0);
		advance_minutes();
	}
}


//-------------------------------------------------
//  check_alarm -
//-------------------------------------------------

inline void rp5c15_device::check_alarm()
{
	bool all_match = true;
	bool all_zeroes = true;

	for (int i = REGISTER_1_MINUTE; i < REGISTER_1_MONTH; i++)
	{
		if (m_reg[MODE01][i] != 0) all_zeroes = false;
		if (m_reg[MODE01][i] != m_reg[MODE00][i]) all_match = false;
	}

	m_alarm_on = (all_match || (!m_alarm_on && all_zeroes)) ? 0 : 1;
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  rp5c15_device - constructor
//-------------------------------------------------

rp5c15_device::rp5c15_device(running_machine &_machine, const rp5c15_device_config &config)
    : device_t(_machine, config),
	  device_rtc_interface(_machine, config, *this),
	  m_alarm(1),
	  m_alarm_on(1),
	  m_1hz(1),
	  m_16hz(1),
	  m_clkout(1),
      m_config(config)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void rp5c15_device::device_start()
{
	// resolve callbacks
	devcb_resolve_write_line(&m_out_alarm_func, &m_config.m_out_alarm_func, this);
	devcb_resolve_write_line(&m_out_clkout_func, &m_config.m_out_clkout_func, this);

	// allocate timers
	m_clock_timer = timer_alloc(TIMER_CLOCK);
	m_clock_timer->adjust(attotime::from_hz(clock() / 16384), 0, attotime::from_hz(clock() / 16384));

	m_16hz_timer = timer_alloc(TIMER_16HZ);
	m_16hz_timer->adjust(attotime::from_hz(clock() / 1024), 0, attotime::from_hz(clock() / 1024));

	m_clkout_timer = timer_alloc(TIMER_CLKOUT);

	// state saving
	save_item(NAME(m_reg[MODE00]));
	save_item(NAME(m_reg[MODE01]));
	save_item(NAME(m_mode));
	save_item(NAME(m_reset));
	save_item(NAME(m_alarm));
	save_item(NAME(m_alarm_on));
	save_item(NAME(m_1hz));
	save_item(NAME(m_16hz));
	save_item(NAME(m_clkout));
}


//-------------------------------------------------
//  device_timer - handler timer events
//-------------------------------------------------

void rp5c15_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_CLOCK:
		if (m_1hz && (m_mode & MODE_TIMER_EN))
		{
			advance_seconds();
		}

		m_1hz = !m_1hz;
		set_alarm_line();
		break;

	case TIMER_16HZ:
		m_16hz = !m_16hz;
		set_alarm_line();
		break;

	case TIMER_CLKOUT:
		m_clkout = !m_clkout;
		devcb_call_write_line(&m_out_clkout_func, m_clkout);
		break;
	}
}


//-------------------------------------------------
//  rtc_set_time - called to initialize the RTC to
//  a known state
//-------------------------------------------------

void rp5c15_device::rtc_set_time(int year, int month, int day, int day_of_week, int hour, int minute, int second)
{
	write_counter(REGISTER_1_YEAR, year);
	write_counter(REGISTER_1_MONTH, month);
	write_counter(REGISTER_1_DAY, day);
	m_reg[MODE00][REGISTER_DAY_OF_THE_WEEK] = day_of_week;
	write_counter(REGISTER_1_HOUR, hour);
	write_counter(REGISTER_1_MINUTE, minute);
	write_counter(REGISTER_1_SECOND, second);
}


//-------------------------------------------------
//  read -
//-------------------------------------------------

READ8_MEMBER( rp5c15_device::read )
{
	UINT8 data = 0;

	switch (offset & 0x0f)
	{
	case REGISTER_MODE:
		data = m_mode;
		break;

	case REGISTER_TEST:
	case REGISTER_RESET:
		// write only
		break;

	default:
		data = m_reg[m_mode & MODE_MASK][offset];
		break;
	}

	if (LOG) logerror("RP5C15 '%s' Register %u Read %02x\n", tag(), offset & 0x0f, data);

	return data & 0x0f;
}


//-------------------------------------------------
//  write -
//-------------------------------------------------

WRITE8_MEMBER( rp5c15_device::write )
{
	int mode = m_mode & MODE_MASK;

	switch (offset & 0x0f)
	{
	case REGISTER_MODE:
		m_mode = data & 0x0f;

		if (LOG)
		{
			logerror("RP5C15 '%s' Mode %u\n", tag(), data & MODE_MASK);
			logerror("RP5C15 '%s' Timer %s\n", tag(), (data & MODE_TIMER_EN) ? "enabled" : "disabled");
			logerror("RP5C15 '%s' Alarm %s\n", tag(), (data & MODE_ALARM_EN) ? "enabled" : "disabled");
		}
		break;

	case REGISTER_TEST:
		if (LOG) logerror("RP5C15 '%s' Test %u not supported!\n", tag(), data);
		break;

	case REGISTER_RESET:
		m_reset = data & 0x0f;

		if (data & RESET_ALARM)
		{
			int i;

			// reset alarm registers
			for (i = REGISTER_1_MINUTE; i < REGISTER_1_MONTH; i++)
			{
				m_reg[MODE01][i] = 0;
			}
		}

		if (LOG)
		{
			if (data & RESET_ALARM) logerror("RP5C15 '%s' Alarm Reset\n", tag());
			if (data & RESET_TIMER) logerror("RP5C15 '%s' Timer Reset not supported!\n", tag());
			logerror("RP5C15 '%s' 16Hz Signal %s\n", tag(), (data & RESET_16_HZ) ? "disabled" : "enabled");
			logerror("RP5C15 '%s' 1Hz Signal %s\n", tag(), (data & RESET_1_HZ) ? "disabled" : "enabled");
		}
		break;

	default:
		switch (mode)
		{
		case MODE00:
			m_reg[mode][offset & 0x0f] = data & REGISTER_WRITE_MASK[mode][offset & 0x0f];
			break;

		case MODE01:
			switch (offset & 0x0f)
			{
			case REGISTER_CLOCK_OUTPUT:
				switch (data & 0x07)
				{
				case CLKOUT_16384_HZ:
					m_clkout_timer->adjust(attotime::from_hz(clock()), 0, attotime::from_hz(clock()));
					break;

				case CLKOUT_1024_HZ:
					m_clkout_timer->adjust(attotime::from_hz(clock() / 16), 0, attotime::from_hz(clock() / 16));
					break;

				case CLKOUT_128_HZ:
					m_clkout_timer->adjust(attotime::from_hz(clock() / 128), 0, attotime::from_hz(clock() / 128));
					break;

				case CLKOUT_16_HZ:
					m_clkout_timer->adjust(attotime::from_hz(clock() / 1024), 0, attotime::from_hz(clock() / 1024));
					break;

				case CLKOUT_1_HZ:
					m_clkout_timer->adjust(attotime::from_hz(clock() / 16384), 0, attotime::from_hz(clock() / 16384));
					break;

				case CLKOUT_1_DIV_60_HZ:
					// TODO
					break;

				case CLKOUT_L:
				case CLKOUT_Z:
					m_clkout = 1;
					m_clkout_timer->adjust(attotime::zero, 0);
					break;
				}

				m_reg[mode][offset & 0x0f] = data & REGISTER_WRITE_MASK[mode][offset & 0x0f];
				break;

			case REGISTER_ADJUST:
				if (BIT(data, 0))
				{
					adjust_seconds();
				}
				m_reg[mode][offset & 0x0f] = data & REGISTER_WRITE_MASK[mode][offset & 0x0f];
				break;

			default:
				m_reg[mode][offset & 0x0f] = data & REGISTER_WRITE_MASK[mode][offset & 0x0f];
				break;
			}
			break;
		}

		if (LOG) logerror("RP5C15 '%s' Register %u Write %02x\n", tag(), offset & 0x0f, data);
		break;
	}
}
