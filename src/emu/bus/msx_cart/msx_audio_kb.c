#include "emu.h"
#include "msx_audio_kb.h"


const device_type MSX_AUDIO_KBDC_PORT = &device_creator<msx_audio_kbdc_port_device>;


msx_audio_kbdc_port_device::msx_audio_kbdc_port_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, MSX_AUDIO_KBDC_PORT, "MSX Audio keyboard connector port", tag, owner, clock, "msx_audio_kbdc_port", __FILE__)
	, device_slot_interface(mconfig, *this)
{
}


void msx_audio_kbdc_port_device::device_start()
{
	m_keyboard = dynamic_cast<msx_audio_kb_port_interface *>(get_card_device());
}


WRITE8_MEMBER(msx_audio_kbdc_port_device::write)
{
	if (m_keyboard)
	{
		m_keyboard->write(space, offset, data);
	}
}


READ8_MEMBER(msx_audio_kbdc_port_device::read)
{
	if (m_keyboard)
	{
		return m_keyboard->read(space, offset);
	}
	return 0xff;
}


extern const device_type MSX_AUDIO_KB_HXMU901;
extern const device_type MSX_AUDIO_KB_NMS1160;


class msx_hxmu901 : public device_t
					, public msx_audio_kb_port_interface
{
public:
	msx_hxmu901(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
		: device_t(mconfig, MSX_AUDIO_KB_HXMU901, "Toshiba HXMU901", tag, owner, clock, "hxmu901", __FILE__)
		, msx_audio_kb_port_interface(mconfig, *this)
		, m_row(0)
		, m_keyboard(*this, "KEY")
	{ };

	virtual ioport_constructor device_input_ports() const;

	virtual DECLARE_READ8_MEMBER(read)
	{
		UINT8 result = 0xff;

		for (int i = 0; i < 8; i++)
		{
			if (BIT(m_row,i))
			{
				result &= m_keyboard[i]->read();
			}
		}
		return result;
	}

	virtual DECLARE_WRITE8_MEMBER(write)
	{
		m_row = data;
	}

protected:
	virtual void device_start() { }

private:
	UINT8 m_row;
	required_ioport_array<8> m_keyboard;
};


static INPUT_PORTS_START( hxmu901)
	PORT_START("KEY.0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C#1")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D1")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D#1")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E1")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F1")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F#1")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C1")

	PORT_START("KEY.1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G1")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G#1")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A1")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A#1")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B1")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C2")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY.2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C#2")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D2")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D#2")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E2")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F2")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F#2")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY.3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G2")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G#2")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A2")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A#2")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B2")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C3")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY.4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C#3")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D3")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D#3")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E3")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F3")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F#3")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY.5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G3")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G#3")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A3")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A#3")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B3")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C4")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY.6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C#4")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D4")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D#4")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E4")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F4")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F#4")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY.7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G4")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G#4")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A4")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)  // Multi sensor related?
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A#4")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B4")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C5")
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END


ioport_constructor msx_hxmu901::device_input_ports() const
{
	return INPUT_PORTS_NAME( hxmu901 );
}


class msx_nms1160 : public device_t
					, public msx_audio_kb_port_interface
{
public:
	msx_nms1160(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
		: device_t(mconfig, MSX_AUDIO_KB_NMS1160, "Philips NMS-1160", tag, owner, clock, "nms1160", __FILE__)
		, msx_audio_kb_port_interface(mconfig, *this)
		, m_row(0)
	{ };

//	virtual ioport_constructor device_input_ports() const;

	virtual DECLARE_READ8_MEMBER(read)
	{
		return 0xff;
	}

	virtual DECLARE_WRITE8_MEMBER(write)
	{
		printf("msx_nms1160::write %02x\n", data);
		m_row = data;
	}

protected:
	virtual void device_start() { }

private:
	UINT8 m_row;
};


const device_type MSX_AUDIO_KB_HXMU901 = &device_creator<msx_hxmu901>;
const device_type MSX_AUDIO_KB_NMS1160 = &device_creator<msx_nms1160>;


SLOT_INTERFACE_START( msx_audio_keyboards )
	SLOT_INTERFACE("hxmu901", MSX_AUDIO_KB_HXMU901)
	SLOT_INTERFACE("nms1160", MSX_AUDIO_KB_NMS1160)
SLOT_INTERFACE_END


