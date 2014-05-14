#ifndef __MSX_SLOT_CARTRIDGE_H
#define __MSX_SLOT_CARTRIDGE_H

#include "slot.h"
#include "bus/msx_cart/cartridge.h"


extern const device_type MSX_SLOT_CARTRIDGE;


#define MCFG_MSX_SLOT_CARTRIDGE_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, MSX_SLOT_CARTRIDGE, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(msx_cart, NULL, false)


class msx_slot_cartridge_device : public device_t
								, public device_image_interface
								, public device_slot_interface
								, public msx_internal_slot_interface
{
public:
	// construction/destruction
	msx_slot_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete() { update_names(MSX_SLOT_CARTRIDGE, "cartridge", "cart"); }

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload();
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry);
	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return true; }
	virtual bool is_writeable() const { return false; }
	virtual bool is_creatable() const { return false; }
	virtual bool must_be_loaded() const { return false; }
	virtual bool is_reset_on_load() const { return true; }
	virtual const option_guide *create_option_guide() const { return NULL; }
	virtual const char *image_interface() const { return "msx_cart"; }
	virtual const char *file_extensions() const { return "mx1,bin,rom"; }

	// slot interface overrides
	virtual void get_default_card_software(astring &result);

	// msx_internal_slot-level overrides
	virtual DECLARE_READ8_MEMBER(read);
	virtual DECLARE_WRITE8_MEMBER(write);

private:
	msx_cart_interface *m_cartridge;

	int get_cart_type(UINT8 *rom, UINT32 length);
};

#endif

