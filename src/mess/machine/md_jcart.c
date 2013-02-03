/***********************************************************************************************************
 
 
 MegaDrive / Genesis J-Cart (+SEPROM) emulation
 
 
 i2c games mapping table:
 
 game name                         |   SDA_IN   |  SDA_OUT   |     SCL    |  SIZE_MASK     | PAGE_MASK |
 ----------------------------------|------------|------------|------------|----------------|-----------|
 Micro Machines 2                  | 0x380001-7 | 0x300000-0*| 0x300000-1*| 0x03ff (24C08) |   0x0f    |
 Micro Machines Military           | 0x380001-7 | 0x300000-0*| 0x300000-1*| 0x03ff (24C08) |   0x0f    |
 Micro Machines 96                 | 0x380001-7 | 0x300000-0*| 0x300000-1*| 0x07ff (24C16) |   0x0f    |
 ----------------------------------|------------|------------|------------|----------------|-----------|
 
 * Notes: check these


 TODO: proper SEPROM emulation, still not worked on (just hooked up the I2C device)
 
***********************************************************************************************************/



#include "emu.h"
#include "machine/md_jcart.h"


//-------------------------------------------------
//  md_rom_device - constructor
//-------------------------------------------------

const device_type MD_JCART = &device_creator<md_jcart_device>;
const device_type MD_SEPROM_CODEMAST = &device_creator<md_seprom_codemast_device>;
const device_type MD_SEPROM_MM96 = &device_creator<md_seprom_mm96_device>;

// Sampras, Super Skidmarks?
md_jcart_device::md_jcart_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, type, name, tag, owner, clock),
					device_md_cart_interface( mconfig, *this ),
					m_jcart3(*this, "JCART3"),
					m_jcart4(*this, "JCART4")
{
}

md_jcart_device::md_jcart_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, MD_JCART, "MD J-Cart games", tag, owner, clock),
					device_md_cart_interface( mconfig, *this ),
					m_jcart3(*this, "JCART3"),
					m_jcart4(*this, "JCART4")
{
}

// Micro Machines 2, Micro Machines Military
md_seprom_codemast_device::md_seprom_codemast_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock)
					: md_jcart_device(mconfig, type, name, tag, owner, clock),
					m_i2cmem(*this, "i2cmem")
{
}

md_seprom_codemast_device::md_seprom_codemast_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: md_jcart_device(mconfig, MD_SEPROM_CODEMAST, "MD J-Cart games + SEPROM", tag, owner, clock),
					m_i2cmem(*this, "i2cmem")
{
}

// Micro Machines 96
md_seprom_mm96_device::md_seprom_mm96_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: md_seprom_codemast_device(mconfig, MD_SEPROM_MM96, "MD Micro Machine 96", tag, owner, clock)
{
}


//-------------------------------------------------
//  SERIAL I2C DEVICE
//-------------------------------------------------

static const i2cmem_interface md_24c08_i2cmem_interface =
{
	I2CMEM_SLAVE_ADDRESS, 0, 0x400
};

static const i2cmem_interface md_24c16a_i2cmem_interface =
{
	I2CMEM_SLAVE_ADDRESS, 0, 0x800
};

// MD_SEPROM_CODEMAST
MACHINE_CONFIG_FRAGMENT( md_i2c_24c08 )
	MCFG_I2CMEM_ADD("i2cmem", md_24c08_i2cmem_interface)
MACHINE_CONFIG_END

// MD_SEPROM_MM96
MACHINE_CONFIG_FRAGMENT( md_i2c_24c16a )
	MCFG_I2CMEM_ADD("i2cmem", md_24c16a_i2cmem_interface)
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor md_seprom_codemast_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( md_i2c_24c08 );
}

machine_config_constructor md_seprom_mm96_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( md_i2c_24c16a );
}


static INPUT_PORTS_START( jcart_ipt )

	PORT_START("JCART3")     /* Joypad 3 on J-Cart (3 button + start) */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3) PORT_NAME("P3 B")
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3) PORT_NAME("P3 C")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3) PORT_NAME("P3 A")
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(3)

	PORT_START("JCART4")     /* Joypad 4 on J-Cart (3 button + start) */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4) PORT_NAME("P4 B")
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(4) PORT_NAME("P4 C")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4) PORT_NAME("P4 A")
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(4)

INPUT_PORTS_END

ioport_constructor md_jcart_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( jcart_ipt );
}


void md_jcart_device::device_start()
{
	m_jcart_io_data[0] = 0;
	m_jcart_io_data[1] = 0;
	save_item(NAME(m_jcart_io_data));
}

void md_seprom_codemast_device::device_start()
{
	m_i2c_mem = 0;
	m_i2c_clk = 0;
	m_jcart_io_data[0] = 0;
	m_jcart_io_data[1] = 0;
	save_item(NAME(m_i2c_mem));
	save_item(NAME(m_i2c_clk));
	save_item(NAME(m_jcart_io_data));
}


/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------
 J-CART ONLY (Pete Sampras Tennis)
 -------------------------------------------------*/

READ16_MEMBER(md_jcart_device::read)
{
	if (offset == 0x38fffe/2)
	{
		UINT8 joy[2];
		
		if (m_jcart_io_data[0] & 0x40)
		{
			joy[0] = m_jcart3->read_safe(0);
			joy[1] = m_jcart4->read_safe(0);
			return (m_jcart_io_data[0] & 0x40) | joy[0] | (joy[1] << 8);
		}
		else
		{
			joy[0] = ((m_jcart3->read_safe(0) & 0xc0) >> 2) | (m_jcart3->read_safe(0) & 0x03);
			joy[1] = ((m_jcart4->read_safe(0) & 0xc0) >> 2) | (m_jcart4->read_safe(0) & 0x03);
			return (m_jcart_io_data[0] & 0x40) | joy[0] | (joy[1] << 8);
		}
	}
	return m_rom[offset];
}

WRITE16_MEMBER(md_jcart_device::write)
{
	if (offset == 0x38fffe/2)
	{
		m_jcart_io_data[0] = (data & 1) << 6;
		m_jcart_io_data[1] = (data & 1) << 6;
	}
}

/*-------------------------------------------------
 J-CART + SEPROM
 -------------------------------------------------*/

READ16_MEMBER(md_seprom_codemast_device::read)
{
	if (offset == 0x380000/2)
	{
		m_i2c_mem = i2cmem_sda_read(m_i2cmem);
		return (m_i2c_mem & 1) << 7;
	}
	if (offset == 0x38fffe/2)
	{
		UINT8 joy[2];
		
		if (m_jcart_io_data[0] & 0x40)
		{
			joy[0] = m_jcart3->read_safe(0);
			joy[1] = m_jcart4->read_safe(0);
			return (m_jcart_io_data[0] & 0x40) | joy[0] | (joy[1] << 8);
		}
		else
		{
			joy[0] = ((m_jcart3->read_safe(0) & 0xc0) >> 2) | (m_jcart3->read_safe(0) & 0x03);
			joy[1] = ((m_jcart4->read_safe(0) & 0xc0) >> 2) | (m_jcart4->read_safe(0) & 0x03);
			return (m_jcart_io_data[0] & 0x40) | joy[0] | (joy[1] << 8);
		}
	}
	return m_rom[offset];
}

WRITE16_MEMBER(md_seprom_codemast_device::write)
{
	if (offset == 0x380000/2)
	{
		m_i2c_clk = BIT(data, 9);
		m_i2c_mem = BIT(data, 8);
		i2cmem_scl_write(m_i2cmem, m_i2c_clk);
		i2cmem_sda_write(m_i2cmem, m_i2c_mem);
	}
	if (offset == 0x38fffe/2)
	{
		m_jcart_io_data[0] = (data & 1) << 6;
		m_jcart_io_data[1] = (data & 1) << 6;
	}
}

