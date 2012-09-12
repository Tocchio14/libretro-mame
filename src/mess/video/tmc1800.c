#include "emu.h"
#include "includes/tmc1800.h"
#include "cpu/cosmac/cosmac.h"
#include "video/cdp1861.h"
#include "sound/cdp1864.h"
#include "machine/rescap.h"

/* Telmac 1800 */

static CDP1861_INTERFACE( tmc1800_cdp1861_intf )
{
	CDP1802_TAG,
	SCREEN_TAG,
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_INT),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_DMAOUT),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_EF1)
};

/* Telmac 2000 */

READ_LINE_MEMBER( tmc2000_state::rdata_r )
{
	return BIT(m_color, 2);
}

READ_LINE_MEMBER( tmc2000_state::bdata_r )
{
	return BIT(m_color, 1);
}

READ_LINE_MEMBER( tmc2000_state::gdata_r )
{
	return BIT(m_color, 0);
}

static CDP1864_INTERFACE( tmc2000_cdp1864_intf )
{
	CDP1802_TAG,
	SCREEN_TAG,
	CDP1864_INTERLACED,
	DEVCB_DRIVER_LINE_MEMBER(tmc2000_state, rdata_r),
	DEVCB_DRIVER_LINE_MEMBER(tmc2000_state, bdata_r),
	DEVCB_DRIVER_LINE_MEMBER(tmc2000_state, gdata_r),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_INT),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_DMAOUT),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_EF1),
	DEVCB_NULL,
	RES_K(1.21),	// RL64
	RES_K(2.05),	// RL63
	RES_K(2.26),	// RL61
	RES_K(3.92)		// RL65 (also RH62 (2K pot) in series, but ignored here)
};

/* OSCOM Nano */

static CDP1864_INTERFACE( nano_cdp1864_intf )
{
	CDP1802_TAG,
	SCREEN_TAG,
	CDP1864_INTERLACED,
	DEVCB_LINE_VCC,
	DEVCB_LINE_VCC,
	DEVCB_LINE_VCC,
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_INT),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_DMAOUT),
	DEVCB_CPU_INPUT_LINE(CDP1802_TAG, COSMAC_INPUT_LINE_EF1),
	DEVCB_NULL,
	RES_K(1.21), // R18 unconfirmed
	0, // not connected
	0, // not connected
	0  // not connected
};

/* OSM-200 */

UINT32 osc1000b_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return 0;
}

/* Machine Drivers */

MACHINE_CONFIG_FRAGMENT( tmc1800_video )
	MCFG_CDP1861_SCREEN_ADD(CDP1861_TAG, SCREEN_TAG, XTAL_1_75MHz)
	MCFG_CDP1861_ADD(CDP1861_TAG, XTAL_1_75MHz, tmc1800_cdp1861_intf)
MACHINE_CONFIG_END

MACHINE_CONFIG_FRAGMENT( osc1000b_video )
	MCFG_SCREEN_ADD(SCREEN_TAG, RASTER)
	MCFG_SCREEN_UPDATE_DRIVER(osc1000b_state, screen_update)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(320, 200)
	MCFG_SCREEN_VISIBLE_AREA(0, 319, 0, 199)
MACHINE_CONFIG_END

MACHINE_CONFIG_FRAGMENT( tmc2000_video )
	MCFG_CDP1864_SCREEN_ADD(SCREEN_TAG, XTAL_1_75MHz)
	MCFG_SCREEN_UPDATE_DEVICE(CDP1864_TAG, cdp1864_device, screen_update)

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_CDP1864_ADD(CDP1864_TAG, XTAL_1_75MHz, tmc2000_cdp1864_intf)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END

MACHINE_CONFIG_FRAGMENT( nano_video )
	MCFG_CDP1864_SCREEN_ADD(SCREEN_TAG, XTAL_1_75MHz)
	MCFG_SCREEN_UPDATE_DEVICE(CDP1864_TAG, cdp1864_device, screen_update)

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_CDP1864_ADD(CDP1864_TAG, XTAL_1_75MHz, nano_cdp1864_intf)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END
