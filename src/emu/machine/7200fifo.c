/**********************************************************************

    IDT7200 series 9-bit Asynchronous FIFO Emulation

    TODO:
    - retransmit (RT pin)

**********************************************************************/

#include "emu.h"

#include "machine/7200fifo.h"


const device_type FIFO7200 = &device_creator<fifo7200_device>;

//-------------------------------------------------
//  fifo7200_device - constructor
//-------------------------------------------------

fifo7200_device::fifo7200_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, FIFO7200, "IDT7200 Asynchronous FIFO", tag, owner, clock),
		m_ram_size(0),
		m_ef_handler(*this),
		m_ff_handler(*this),
		m_hf_handler(*this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void fifo7200_device::device_start()
{
	assert(m_ram_size > 1 && ~m_ram_size & 1);
	m_buffer = auto_alloc_array(machine(), UINT16, m_ram_size);

	// resolve callbacks
	m_ef_handler.resolve();
	m_ff_handler.resolve();
	m_hf_handler.resolve();
	
	// state save
	save_item(NAME(m_read_ptr));
	save_item(NAME(m_write_ptr));
	save_item(NAME(m_ef));
	save_item(NAME(m_ff));
	save_item(NAME(m_hf));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void fifo7200_device::device_reset()
{
	// master reset
	memset(m_buffer, 0, m_ram_size * sizeof(UINT16));
	m_read_ptr = 0;
	m_write_ptr = 0;
	
	m_ef = 1;
	m_ff = 0;
	m_hf = 0;
	
	if (!m_ef_handler.isnull()) m_ef_handler(m_ef);
	if (!m_ff_handler.isnull()) m_ff_handler(m_ff);
	if (!m_hf_handler.isnull()) m_hf_handler(m_hf);
}



void fifo7200_device::fifo_write(UINT32 data)
{
	if (m_ff)
		return;

	m_buffer[m_write_ptr] = data;
	m_write_ptr = (m_write_ptr + 1) % m_ram_size;
	
	// update flags
	if (m_ef)
	{
		m_ef = 0;
		if (!m_ef_handler.isnull()) m_ef_handler(m_ef);
	}

	else if (m_read_ptr == m_write_ptr)
	{
		m_ff = 1;
		if (!m_ff_handler.isnull()) m_ff_handler(m_ff);
	}

	else if (((m_read_ptr + 1 + m_ram_size / 2) % m_ram_size) == m_write_ptr)
	{
		m_hf = 1;
		if (!m_hf_handler.isnull()) m_hf_handler(m_hf);
	}
}

UINT32 fifo7200_device::fifo_read()
{
	if (m_ef)
		return ~0;
	
	UINT16 ret = m_buffer[m_read_ptr];
	m_read_ptr = (m_read_ptr + 1) % m_ram_size;

	// update flags
	if (m_ff)
	{
		m_ff = 0;
		if (!m_ff_handler.isnull()) m_ff_handler(m_ff);
	}

	else if (m_read_ptr == m_write_ptr)
	{
		m_ef = 1;
		if (!m_ef_handler.isnull()) m_ef_handler(m_ef);
	}
	
	else if (((m_read_ptr + m_ram_size / 2) % m_ram_size) == m_write_ptr)
	{
		m_hf = 0;
		if (!m_hf_handler.isnull()) m_hf_handler(m_hf);
	}
	
	return ret;
}