// license:BSD-3-Clause
// copyright-holders:etabeta
/***********************************************************************************************************

    APF Imagination / M-1000 cart emulation
    (through slot devices)

 ***********************************************************************************************************/


#include "emu.h"
#include "slot.h"

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type APF_CART_SLOT = &device_creator<apf_cart_slot_device>;

//**************************************************************************
//    APF Cartridges Interface
//**************************************************************************

//-------------------------------------------------
//  device_apf_cart_interface - constructor
//-------------------------------------------------

device_apf_cart_interface::device_apf_cart_interface(const machine_config &mconfig, device_t &device)
	: device_slot_card_interface(mconfig, device),
		m_rom(NULL),
		m_rom_size(0)
{
}


//-------------------------------------------------
//  ~device_apf_cart_interface - destructor
//-------------------------------------------------

device_apf_cart_interface::~device_apf_cart_interface()
{
}

//-------------------------------------------------
//  rom_alloc - alloc the space for the cart
//-------------------------------------------------

void device_apf_cart_interface::rom_alloc(UINT32 size, const char *tag)
{
	if (m_rom == NULL)
	{
		astring tempstring(tag);
		tempstring.cat(APFSLOT_ROM_REGION_TAG);
		m_rom = device().machine().memory().region_alloc(tempstring, size, 1, ENDIANNESS_LITTLE)->base();
		m_rom_size = size;
	}
}


//-------------------------------------------------
//  ram_alloc - alloc the space for the ram
//-------------------------------------------------

void device_apf_cart_interface::ram_alloc(UINT32 size)
{
	m_ram.resize(size);
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  apf_cart_slot_device - constructor
//-------------------------------------------------
apf_cart_slot_device::apf_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
						device_t(mconfig, APF_CART_SLOT, "APF Cartridge Slot", tag, owner, clock, "apf_cart_slot", __FILE__),
						device_image_interface(mconfig, *this),
						device_slot_interface(mconfig, *this),
						m_type(APF_STD)
{
}


//-------------------------------------------------
//  apf_cart_slot_device - destructor
//-------------------------------------------------

apf_cart_slot_device::~apf_cart_slot_device()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void apf_cart_slot_device::device_start()
{
	m_cart = dynamic_cast<device_apf_cart_interface *>(get_card_device());
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void apf_cart_slot_device::device_config_complete()
{
	// set brief and instance name
	update_names();
}


//-------------------------------------------------
//  APF PCB
//-------------------------------------------------

struct apf_slot
{
	int                     pcb_id;
	const char              *slot_option;
};

// Here, we take the feature attribute from .xml (i.e. the PCB name) and we assign a unique ID to it
static const apf_slot slot_list[] =
{
	{ APF_STD,      "std" },
	{ APF_BASIC,    "basic" },
	{ APF_SPACEDST, "spacedst" }
};

static int apf_get_pcb_id(const char *slot)
{
	for (int i = 0; i < ARRAY_LENGTH(slot_list); i++)
	{
		if (!core_stricmp(slot_list[i].slot_option, slot))
			return slot_list[i].pcb_id;
	}

	return 0;
}

static const char *apf_get_slot(int type)
{
	for (int i = 0; i < ARRAY_LENGTH(slot_list); i++)
	{
		if (slot_list[i].pcb_id == type)
			return slot_list[i].slot_option;
	}

	return "std";
}


/*-------------------------------------------------
 call load
 -------------------------------------------------*/

bool apf_cart_slot_device::call_load()
{
	if (m_cart)
	{
		UINT32 size = (software_entry() == NULL) ? length() : get_software_region_length("rom");

		if (size > 0x3800)
		{
			seterror(IMAGE_ERROR_UNSPECIFIED, "Image extends beyond the expected size for an APF cart");
			return IMAGE_INIT_FAIL;
		}

		m_cart->rom_alloc(size, tag());

		if (software_entry() == NULL)
			fread(m_cart->get_rom_base(), size);
		else
			memcpy(m_cart->get_rom_base(), get_software_region("rom"), size);

		if (software_entry() == NULL)
		{
			m_type = APF_STD;
			// attempt to identify Space Destroyer, which needs 1K of additional RAM
			if (size == 0x1800)
			{
				m_type = APF_SPACEDST;
				m_cart->ram_alloc(0x400);
			}
			if (size > 0x2000)
				m_type = APF_BASIC;
		}
		else
		{
			const char *pcb_name = get_feature("slot");
			if (pcb_name)
				m_type = apf_get_pcb_id(pcb_name);

			if (get_software_region("ram"))
				m_cart->ram_alloc(get_software_region_length("ram"));
		}

		//printf("Type: %s\n", apf_get_slot(m_type));

		return IMAGE_INIT_PASS;
	}

	return IMAGE_INIT_PASS;
}


/*-------------------------------------------------
 call softlist load
 -------------------------------------------------*/

bool apf_cart_slot_device::call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry)
{
	load_software_part_region(*this, swlist, swname, start_entry);
	return TRUE;
}


/*-------------------------------------------------
 get default card software
 -------------------------------------------------*/

void apf_cart_slot_device::get_default_card_software(astring &result)
{
	if (open_image_file(mconfig().options()))
	{
		const char *slot_string = "std";
		UINT32 size = core_fsize(m_file);
		int type = APF_STD;

		// attempt to identify Space Destroyer, which needs 1K of additional RAM
		if (size == 0x1800)
			type = APF_SPACEDST;
		if (size > 0x2000)
			type = APF_BASIC;

		slot_string = apf_get_slot(type);

		//printf("type: %s\n", slot_string);
		clear();

		result.cpy(slot_string);
		return;
	}

	software_get_default_slot(result, "std");
}

/*-------------------------------------------------
 read
 -------------------------------------------------*/

READ8_MEMBER(apf_cart_slot_device::read_rom)
{
	if (m_cart)
		return m_cart->read_rom(space, offset);
	else
		return 0xff;
}

/*-------------------------------------------------
 read
 -------------------------------------------------*/

READ8_MEMBER(apf_cart_slot_device::extra_rom)
{
	if (m_cart)
		return m_cart->extra_rom(space, offset);
	else
		return 0xff;
}

/*-------------------------------------------------
 read
 -------------------------------------------------*/

READ8_MEMBER(apf_cart_slot_device::read_ram)
{
	if (m_cart)
		return m_cart->read_ram(space, offset);
	else
		return 0xff;
}

/*-------------------------------------------------
 write
 -------------------------------------------------*/

WRITE8_MEMBER(apf_cart_slot_device::write_ram)
{
	if (m_cart)
		m_cart->write_ram(space, offset, data);
}
