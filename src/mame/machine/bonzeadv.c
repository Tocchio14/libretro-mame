/*************************************************************************

  Bonze Adventure C-Chip
  ======================

  Based on RAINE. Improvements with a lot of help from Ruben Panossian.
  Additional thanks to Robert Gallagher and stephh.

  Verification on real hardware by Bryan McPhail, Feb 2006:
    The restart positions on real hardware are affected by the level
    variable, as well as scroll & player position.  The old implementation
    did not account for this and so overlapping map positions between
    levels reported the wrong data.

    Restart data is confirmed correct & complete.  Previously many
    restart points were missed.

    Mapping of restart positions now accurate to the real hardware -
    previously these values were approximated by hand (and many
    missing).

*************************************************************************/

#include "driver.h"
#include "includes/cchip.h"

static int current_round = 0;
static int current_bank = 0;

static UINT8 cval[26];
static UINT8 cc_port;
static UINT8 restart_status;

struct cchip_mapping
{
	UINT16 xmin;
	UINT16 xmax;
	UINT16 ymin;
	UINT16 ymax;
	UINT16 sx;
	UINT16 sy;
	UINT16 px;
	UINT16 py;
};

static const UINT16 CLEV[][13] =
{
/*    map start       player start    player y-range  player x-range  map y-range     map x-range     time   */
	{ 0x0000, 0x0018, 0x0020, 0x0030, 0x0028, 0x00D0, 0x0050, 0x0090, 0x0000, 0x0118, 0x0000, 0x0C90, 0x3800 },
	{ 0x0000, 0x0100, 0x0048, 0x0028, 0x0028, 0x0090, 0x0070, 0x00B0, 0x0000, 0x02C0, 0x0000, 0x0CA0, 0x3000 },
	{ 0x0000, 0x0518, 0x0068, 0x00B8, 0x0028, 0x00D0, 0x0068, 0x00B0, 0x02F8, 0x0518, 0x0000, 0x0EF8, 0x3000 },
	{ 0x0978, 0x0608, 0x00C8, 0x00B0, 0x0028, 0x00D0, 0x0098, 0x00C8, 0x0608, 0x06E8, 0x0000, 0x0A48, 0x2000 },
	{ 0x0410, 0x0708, 0x0070, 0x0030, 0x0028, 0x00D0, 0x0060, 0x0090, 0x0708, 0x0708, 0x0410, 0x1070, 0x3800 },
	{ 0x1288, 0x0808, 0x0099, 0x00CE, 0x0028, 0x00D0, 0x0060, 0x00C0, 0x0000, 0x0808, 0x1288, 0x1770, 0x3000 },
	{ 0x11B0, 0x0908, 0x0118, 0x0040, 0x0028, 0x00D0, 0x00B0, 0x00C0, 0x0900, 0x0910, 0x0050, 0x11B0, 0x3800 },
	{ 0x0000, 0x0808, 0x0028, 0x00B8, 0x0028, 0x00D0, 0x0070, 0x00B0, 0x0808, 0x0808, 0x0000, 0x0398, 0x1000 },
	{ 0x06F8, 0x0808, 0x0028, 0x00B8, 0x0028, 0x00D0, 0x0070, 0x00B0, 0x0808, 0x0808, 0x06F8, 0x06F8, 0x8800 },
	{ 0x06F8, 0x0808, 0x0028, 0x00B8, 0x0028, 0x00D0, 0x0070, 0x00B0, 0x0808, 0x0808, 0x06F8, 0x06F8, 0xffff },
	{ 0x06F8, 0x0808, 0x0028, 0x00B8, 0x0028, 0x00D0, 0x0070, 0x00B0, 0x0808, 0x0808, 0x06F8, 0x06F8, 0xffff },
};

static const struct cchip_mapping level00[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0x0200, 0x0000, 0x0100,	0x0000, 0x0018, 0x0020, 0x00a8 },
	{ 0x0200, 0x0380, 0x0000, 0x0100,	0x01e0, 0x0018, 0x0070, 0x0098 },
	{ 0x0380, 0x0620, 0x0000, 0x0100,	0x04a0, 0x0018, 0x0070, 0x0090 },
	{ 0x0620, 0x08f0, 0x0000, 0x0100,	0x06b8, 0x0018, 0x0078, 0x0078 },
	{ 0x08f0, 0x0a20, 0x0000, 0x0100,	0x08c8, 0x0018, 0x0070, 0x0028 },
	{ 0x0a20, 0x0c80, 0x0000, 0x0100,	0x0a68, 0x0018, 0x0070, 0x0058 },
	{ 0x0c80, 0x0e00, 0x0000, 0x0100,	0x0c40, 0x0018, 0x0070, 0x0040 },

	{ 0x0380, 0x07c0, 0x0100, 0x0200,	0x0038, 0x0418, 0x0070, 0x00a8 },
	{ 0xff }
};

static const struct cchip_mapping level01[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0x0120, 0x0100, 0x0200,	0x0000, 0x0208, 0x0040, 0x0070 },

	{ 0x0000, 0x0120, 0x0200, 0x0300,	0x0000, 0x0208, 0x0040, 0x0070 },
	{ 0x0120, 0x0460, 0x0200, 0x0300,	0x0080, 0x0218, 0x00b0, 0x0080 },
	{ 0x0460, 0x0690, 0x0200, 0x0278,	0x0450, 0x01f8, 0x0090, 0x0030 },
	{ 0x0460, 0x0690, 0x0278, 0x0300,	0x0450, 0x0218, 0x00a0, 0x00a0 },
	{ 0x0690, 0x07d0, 0x0200, 0x0278,	0x0618, 0x01f8, 0x00a0, 0x0040 },
	{ 0x0690, 0x07d0, 0x0278, 0x0300,	0x0628, 0x0218, 0x00a8, 0x00b0 },
	{ 0x07d0, 0x08b0, 0x0200, 0x0300,	0x07a8, 0x0218, 0x0090, 0x0060 },
	{ 0x08b0, 0x09d0, 0x0200, 0x0300,	0x0840, 0x0218, 0x0088, 0x0060 },
	{ 0x09d0, 0x0b00, 0x0200, 0x0300,	0x0958, 0x0218, 0x00a0, 0x0070 },
	{ 0x0b00, 0x0c90, 0x0200, 0x0300,	0x0a98, 0x0218, 0x0088, 0x0050 },
	{ 0x0c90, 0x0e00, 0x0200, 0x0300,	0x0c20, 0x0200, 0x00a0, 0x0028 },
	{ 0xff }
};

static const struct cchip_mapping level02[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0x01c0, 0x04f4, 0x05f8,	0x0000, 0x0518, 0x0068, 0x00b0 },
	{ 0x01c0, 0x03c0, 0x04f4, 0x05f8,	0x00d0, 0x0518, 0x0078, 0x0060 },
	{ 0x03c0, 0x06e0, 0x04f4, 0x05f8,	0x02f0, 0x0518, 0x0078, 0x0048 },
	{ 0x06e0, 0x0840, 0x04f4, 0x05f8,	0x0670, 0x0518, 0x0078, 0x0048 },
	{ 0x0840, 0x0a10, 0x04f4, 0x05f8,	0x07d8, 0x0518, 0x0070, 0x0060 },
	{ 0x0a10, 0x0b80, 0x04f4, 0x05f8,	0x09e8, 0x0500, 0x0080, 0x0080 },
	{ 0x0b80, 0x1090, 0x04f4, 0x05f8,	0x0b20, 0x0418, 0x0070, 0x0080 },

	{ 0x0230, 0x03a0, 0x040c, 0x04f4,	0x02e8, 0x04b0, 0x0080, 0x0090 },
	{ 0x03a0, 0x03b0, 0x040c, 0x04f4,	0x0278, 0x0318, 0x0078, 0x00a8 },
	{ 0x0520, 0x08e0, 0x040c, 0x04f4,	0x0608, 0x0318, 0x0080, 0x0058 },
	{ 0x08e0, 0x0a00, 0x040c, 0x04f4,	0x0878, 0x0318, 0x0078, 0x0098 },

	{ 0x0230, 0x03b0, 0x02f8, 0x040c,	0x0278, 0x0318, 0x0078, 0x00a8 },
	{ 0x03b0, 0x0520, 0x02f8, 0x040c,	0x0390, 0x0318, 0x0070, 0x00b8 },
	{ 0x0520, 0x08e0, 0x02f8, 0x040c,	0x0608, 0x0318, 0x0080, 0x0058 },
	{ 0x08e0, 0x0a00, 0x02f8, 0x040c,	0x0878, 0x0318, 0x0078, 0x0098 }
};

static const struct cchip_mapping level03[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0x0280, 0x0618, 0x06f8,	0x00d8, 0x0608, 0x00b8, 0x00a0 },
	{ 0x0280, 0x0380, 0x0618, 0x06f8,	0x02b0, 0x0608, 0x00b8, 0x0090 },
	{ 0x0380, 0x0620, 0x0618, 0x06f8,	0x02c8, 0x0608, 0x00c0, 0x0090 },
	{ 0x0620, 0x0760, 0x0618, 0x06f8,	0x0630, 0x0608, 0x00c0, 0x0028 },
	{ 0x0760, 0x0910, 0x0618, 0x06f8,	0x07e8, 0x0608, 0x00b8, 0x0078 },
	{ 0x0910, 0x0a30, 0x0618, 0x06f8,	0x0930, 0x0608, 0x00b8, 0x0080 },
	{ 0x0a30, 0x0b60, 0x0618, 0x06f8,	0x0a48, 0x0608, 0x00c0, 0x0068 },

	{ 0x0090, 0x01d0, 0x06f8, 0x07f8,	0x0020, 0x0610, 0x00b8, 0x00a8 },
	{ 0xff }
};

static const struct cchip_mapping level04[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0390, 0x06a0, 0x0704, 0x0804,	0x0560, 0x0708, 0x0068, 0x0090 },
	{ 0x06a0, 0x0850, 0x0704, 0x0804,	0x0600, 0x0708, 0x0080, 0x0070 },
	{ 0x0850, 0x09e0, 0x0704, 0x0804,	0x0860, 0x0708, 0x0060, 0x0090 },
	{ 0x09e0, 0x0c00, 0x0704, 0x0804,	0x09c0, 0x0708, 0x0068, 0x0080 },
	{ 0x0c00, 0x0da0, 0x0704, 0x0804,	0x0c58, 0x0708, 0x0068, 0x0070 },
	{ 0x0da0, 0x0f80, 0x0704, 0x0804,	0x0d80, 0x0708, 0x0070, 0x00b0 },
	{ 0x0f80, 0x1080, 0x0704, 0x0804,	0x0ea8, 0x0708, 0x0070, 0x00b0 },
	{ 0x1080, 0x1230, 0x0704, 0x0804,	0x0fc0, 0x0708, 0x0080, 0x0030 },
	{ 0xff }
};

static const struct cchip_mapping level05[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x1280, 0x1370, 0x800, 0x900,		0x1288, 0x0808, 0x0099, 0x00ce },
	{ 0x1370, 0x14e0, 0x800, 0x900,		0x12f0, 0x0808, 0x0078, 0x0028 },
	{ 0x14e0, 0x1660, 0x800, 0x900,		0x1488, 0x0808, 0x0080, 0x0070 },
	{ 0x1660, 0x18f0, 0x800, 0x900,		0x1600, 0x0808, 0x0080, 0x0090 },

	{ 0x1480, 0x1640, 0x6f8, 0x800,		0x1508, 0x0708, 0x0080, 0x0090 },
	{ 0x1640, 0x1760, 0x6f8, 0x800,		0x16e8, 0x06f0, 0x0080, 0x0058 },
	{ 0x1760, 0x1900, 0x6f8, 0x800,		0x1770, 0x0728, 0x0080, 0x0080 },

	{ 0x14e0, 0x1570, 0x6a0, 0x6f8,		0x1508, 0x05e8, 0x0040, 0x0088 },
	{ 0x1570, 0x1680, 0x6a0, 0x6f8,		0x1568, 0x0608, 0x0080, 0x0090 },
	{ 0x1680, 0x1770, 0x6a0, 0x6f8,		0x1640, 0x05e8, 0x0088, 0x0088 },
	{ 0x1770, 0x18d0, 0x6a0, 0x6f8,		0x1770, 0x0650, 0x00d8, 0x0070 },

	{ 0x14e0, 0x1570, 0x664, 0x6a0,		0x1508, 0x05e8, 0x0040, 0x0088 },
	{ 0x1570, 0x1680, 0x664, 0x6a0,		0x1578, 0x05d0, 0x0098, 0x0060 },
	{ 0x1680, 0x1770, 0x664, 0x6a0,		0x1640, 0x05e8, 0x0088, 0x0088 },
	{ 0x1770, 0x18d0, 0x664, 0x6a0,		0x1770, 0x0650, 0x00d8, 0x0070 },

	{ 0x14e0, 0x1570, 0x624, 0x664,		0x1508, 0x05e8, 0x0040, 0x0088 },
	{ 0x1570, 0x1680, 0x624, 0x664,		0x1578, 0x05d0, 0x0098, 0x0060 },
	{ 0x1680, 0x1770, 0x624, 0x664,		0x1640, 0x05e8, 0x0088, 0x0088 },
	{ 0x1770, 0x18d0, 0x624, 0x664,		0x1770, 0x0588, 0x0090, 0x0088 },

	{ 0x14e0, 0x1570, 0x5d4, 0x624,		0x1508, 0x05e8, 0x0040, 0x0088 },
	{ 0x1570, 0x1680, 0x5d4, 0x624,		0x1578, 0x05d0, 0x0098, 0x0060 },
	{ 0x1680, 0x16a0, 0x5d4, 0x624,		0x1630, 0x0528, 0x0088, 0x0088 },
	{ 0x16a0, 0x1770, 0x5d4, 0x624,		0x1658, 0x0528, 0x0088, 0x0088 },
	{ 0x1770, 0x18d0, 0x5d4, 0x624,		0x1770, 0x0588, 0x0090, 0x0088 },

	{ 0x14e0, 0x15d0, 0x5a4, 0x5d4,		0x1508, 0x0580, 0x0040, 0x0080 },
	{ 0x15d0, 0x15f0, 0x5a4, 0x5d4,		0x1508, 0x04f8, 0x0080, 0x0060 },
	{ 0x15f0, 0x1630, 0x5a4, 0x5d4,		0x1580, 0x04f8, 0x0068, 0x0080 },
	{ 0x1630, 0x16a0, 0x5a4, 0x5d4,		0x1630, 0x0530, 0x0088, 0x0080 },
	{ 0x16a0, 0x17b0, 0x5a4, 0x5d4,		0x1658, 0x0528, 0x0088, 0x0088 },
	{ 0x17b0, 0x18c0, 0x5a4, 0x5d4,		0x1770, 0x0508, 0x0098, 0x0088 },

	{ 0x14e0, 0x15f0, 0x4f8, 0x5a4,		0x1508, 0x04f8, 0x0080, 0x0060 },
	{ 0x15f0, 0x1670, 0x4f8, 0x5a4,		0x15c8, 0x0548, 0x00a8, 0x0070 },
	{ 0x1670, 0x1750, 0x4f8, 0x5a4,		0x1680, 0x0528, 0x00a8, 0x0070 },
	{ 0x1750, 0x18d0, 0x4f8, 0x5a4,		0x1770, 0x04a0, 0x00a8, 0x0078 },

	{ 0x14e0, 0x1630, 0x478, 0x4f8,		0x1508, 0x04f8, 0x0080, 0x0060 },
	{ 0x1630, 0x16b0, 0x478, 0x4f8,		0x15d0, 0x0440, 0x00b8, 0x0078 },
	{ 0x16b0, 0x1760, 0x478, 0x4f8,		0x1660, 0x0460, 0x0080, 0x0060 },
	{ 0x1760, 0x17f0, 0x478, 0x4f8,		0x1738, 0x0420, 0x0090, 0x0070 },
	{ 0x17f0, 0x18d0, 0x478, 0x4f8,		0x1770, 0x0450, 0x00d8, 0x0060 },

	{ 0x14e0, 0x16d0, 0x3f8, 0x478,		0x1500, 0x03e8, 0x0090, 0x0058 },
	{ 0x16d0, 0x17f0, 0x3f8, 0x478,		0x16e0, 0x0398, 0x0068, 0x0068 },
	{ 0x17f0, 0x18d0, 0x3f8, 0x478,		0x1770, 0x0450, 0x00d8, 0x0060 },

	{ 0x14e0, 0x1610, 0x35c, 0x3f8,		0x1518, 0x0368, 0x0090, 0x0058 },
	{ 0x1610, 0x16b0, 0x35c, 0x3f8,		0x15b0, 0x0340, 0x0098, 0x0058 },
	{ 0x16b0, 0x17e0, 0x35c, 0x3f8,		0x1728, 0x0320, 0x0068, 0x0068 },
	{ 0x17e0, 0x18d0, 0x35c, 0x3f8,		0x1770, 0x0330, 0x00c0, 0x0080 },

	{ 0x14e0, 0x1650, 0x2f8, 0x35c,		0x1518, 0x0368, 0x0090, 0x0058 },
	{ 0x1650, 0x17d0, 0x2f8, 0x35c,		0x1630, 0x02b0, 0x0088, 0x0060 },
	{ 0x17d0, 0x18d0, 0x2f8, 0x35c,		0x1740, 0x02a8, 0x0090, 0x0058 },

	{ 0x14e0, 0x1650, 0x268, 0x2f8,		0x15a8, 0x0268, 0x0080, 0x0058 },
	{ 0x1650, 0x17b0, 0x268, 0x2f8,		0x1650, 0x0250, 0x0080, 0x0058 },
	{ 0x17b0, 0x18d0, 0x268, 0x2f8,		0x1740, 0x02a8, 0x0090, 0x0058 },

	{ 0x14e0, 0x15f0, 0x1d8, 0x268,		0x1508, 0x0208, 0x0078, 0x0060 },
	{ 0x15f0, 0x17b0, 0x1d8, 0x268,		0x16c8, 0x0178, 0x0080, 0x0060 },
	{ 0x17b0, 0x18d0, 0x1d8, 0x268,		0x1740, 0x02a8, 0x0090, 0x0058 },

	{ 0x14e0, 0x18d0, 0x158, 0x1d8,		0x1610, 0x0110, 0x0078, 0x0068 },
	{ 0x14e0, 0x18d0, 0x158, 0x1d8,		0x1610, 0x0110, 0x0078, 0x0068 },

	{ 0x14d0, 0x18d0, 0x15c, 0x19c,		0x1618, 0x0110, 0x0078, 0x0068 },
	{ 0x14d0, 0x15b0, 0x19c, 0x1bc,		0x1618, 0x0110, 0x0078, 0x0068 },
	{ 0x15b0, 0x1670, 0x19c, 0x1bc,		0x1580, 0x0128, 0x0070, 0x0080 },
	{ 0x1670, 0x18d0, 0x19c, 0x1bc,		0x1618, 0x0110, 0x0078, 0x0068 },
	{ 0x14d0, 0x14f0, 0x1bc, 0x1dc,		0x1618, 0x0110, 0x0078, 0x0068 },
	{ 0x14f0, 0x15f0, 0x1bc, 0x1dc,		0x1508, 0x0208, 0x0078, 0x0060 },
	{ 0x15f0, 0x1670, 0x1bc, 0x1dc,		0x1580, 0x0128, 0x0070, 0x0080 },
	{ 0x1670, 0x18d0, 0x1bc, 0x1dc,		0x1618, 0x0110, 0x0078, 0x0068 },

	{ 0x14d0, 0x14f0, 0x1bc, 0x1dc,		0x1618, 0x0110, 0x0078, 0x0068 },
	{ 0x14f0, 0x1670, 0x18c, 0x1bc,		0x1508, 0x0208, 0x0078, 0x0060 },

	{ 0x14d0, 0x15b0, 0x18c, 0x1bc,		0x1618, 0x0110, 0x0078, 0x0068 },
	{ 0x15b0, 0x1670, 0x18c, 0x1bc,		0x1580, 0x0128, 0x0070, 0x0080 },
	{ 0x1670, 0x18d0, 0x18c, 0x1bc,		0x1618, 0x0110, 0x0078, 0x0068 },

	{ 0x14d0, 0x18d0, 0x15c, 0x18c,		0x1618, 0x0110, 0x0078, 0x0068 },

	{ 0x14e0, 0x1840, 0x098, 0x15c,		0x16b0, 0x00b0, 0x0088, 0x0060 },
	{ 0x1840, 0x18d0, 0x088, 0x15c,		0x1770, 0x0118, 0x00c0, 0x0060 },

	{ 0x1250, 0x15a0, 0x000, 0x108,		0x1500, 0x0000, 0x0080, 0x0048 },
	{ 0x15a0, 0x16f0, 0x000, 0x098,		0x1698, 0x0000, 0x0080, 0x0050 },
	{ 0x16f0, 0x18d0, 0x000, 0x088,		0x1770, 0x0000, 0x00b8, 0x0060 },

	{ 0x1250, 0x14e0, 0x000, 0x10c,		0x1500, 0x0000, 0x0080, 0x0048 },
	{ 0x14e0, 0x15a0, 0x000, 0x09c,		0x1500, 0x0000, 0x0080, 0x0048 },
	{ 0x15a0, 0x16f0, 0x000, 0x09c,		0x1698, 0x0000, 0x0080, 0x0050 },
	{ 0x16f0, 0x1830, 0x000, 0x09c,		0x1770, 0x0000, 0x00b8, 0x0060 },
	{ 0x1830, 0x18d0, 0x000, 0x09c,		0x1770, 0x0118, 0x00c0, 0x0060 },

	{ 0x14e0, 0x1830, 0x09c, 0x158,		0x16b0, 0x00b0, 0x0088, 0x0060 },
	{ 0x1830, 0x18d0, 0x09c, 0x158,		0x1770, 0x0118, 0x00c0, 0x0060 },

	//--------------------------
	{ 0x14e0, 0x1650, 0x29c, 0x2f0,		0x15a8, 0x0268, 0x0080, 0x0058 },
	{ 0x14e0, 0x1610, 0x2f0, 0x3fc,		0x1518, 0x0368, 0x0090, 0x0058 },
	{ 0x14e0, 0x1630, 0x3fc, 0x46c,		0x1500, 0x03e8, 0x0090, 0x0058 },
	{ 0x14e0, 0x1630, 0x46c, 0x51c,		0x1508, 0x04f8, 0x0080, 0x0060 },
	{ 0x14e0, 0x15f0, 0x51c, 0x59c,		0x1508, 0x04f8, 0x0080, 0x0060 },
	{ 0x14e0, 0x15f0, 0x59c, 0x5d8,		0x1508, 0x0580, 0x0040, 0x0080 },
	{ 0x14e0, 0x1570, 0x5d8, 0x700,		0x1508, 0x05e8, 0x0040, 0x0088 },
	{ 0x1480, 0x1640, 0x700, 0x808,		0x1508, 0x0708, 0x0080, 0x0090 },

	{ 0x0000, 0x18d0, 0x000, 0x900,		0x1288, 0x0808, 0x0099, 0x00ce },
	{ 0xff }
};

static const struct cchip_mapping level06[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0x01a0, 0x0904, 0x0a04,	0x00d0, 0x0908, 0x00b0, 0x0090 },
	{ 0x01a0, 0x04e0, 0x0904, 0x0a04,	0x03d0, 0x0908, 0x00b0, 0x0038 },
	{ 0x04e0, 0x0740, 0x0904, 0x0a04,	0x0660, 0x0908, 0x00b0, 0x0098 },
	{ 0x0740, 0x0950, 0x0904, 0x0a04,	0x07e8, 0x0908, 0x00b8, 0x00b8 },
	{ 0x0950, 0x0a80, 0x0904, 0x0a04,	0x08f8, 0x0908, 0x00b0, 0x0080 },
	{ 0x0a80, 0x0da0, 0x0904, 0x0a04,	0x0cd0, 0x0908, 0x00b8, 0x00b0 },
	{ 0x0da0, 0x0f40, 0x0904, 0x0a04,	0x0e08, 0x0908, 0x00b8, 0x0070 },
	{ 0x0f40, 0x10c0, 0x0904, 0x0a04,	0x0fe8, 0x0908, 0x00b8, 0x0098 },
	{ 0x10c0, 0x1370, 0x0904, 0x0a04,	0x1140, 0x0908, 0x00b8, 0x0068 },
	{ 0xff }
};

static const struct cchip_mapping level07[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0x0570, 0x0804, 0x0904,	0x0000, 0x0808, 0x0028, 0x00b8 },
	{ 0xff }
};

static const struct cchip_mapping level08[] =
{
	/* X1     X2      Y1     Y2     =>  SX      SY      PX      PY */
	{ 0x0000, 0xf000, 0x0000, 0xf000,	0x06f8, 0x0808, 0x0028, 0x00b8 },
	{ 0xff }
};

static const struct cchip_mapping *const levelData[]=
{
	level00,
	level01,
	level02,
	level03,
	level04,
	level05,
	level06,
	level07,
	level08
};

static void WriteLevelData(void)
{
	int i;

	for (i = 0; i < 13; i++)
	{
		UINT16 v = CLEV[current_round][i];

		cval[2 * i + 0] = v & 0xff;
		cval[2 * i + 1] = v >> 8;
	}
}

static void WriteRestartPos(int level)
{
	/*
        Cval0/1 = scroll x position
        Cval4/5 = player x screen position
        Cval2/3 = scroll y position
        Cval6/7 = player y screen position

        These are combined to find the absolute player position
        on the map, which is then given to the C-Chip in order
        for the restart position to be returned.
    */

	int x = cval[0] + 256 * cval[1] + cval[4] + 256 * cval[5];
	int y = cval[2] + 256 * cval[3] + cval[6] + 256 * cval[7];

	const struct cchip_mapping* thisLevel=levelData[level];

	while (thisLevel->xmin!=0xff)
	{
		if (x >= thisLevel->xmin && x < thisLevel->xmax &&
		    y >= thisLevel->ymin && y < thisLevel->ymax)
		{
			cval[0] = thisLevel->sx & 0xff;
			cval[1] = thisLevel->sx >> 8;
			cval[2] = thisLevel->sy & 0xff;
			cval[3] = thisLevel->sy >> 8;
			cval[4] = thisLevel->px & 0xff;
			cval[5] = thisLevel->px >> 8;
			cval[6] = thisLevel->py & 0xff;
			cval[7] = thisLevel->py >> 8;

			// Restart position found ok
			restart_status=0;

			return;
		}

		thisLevel++;
	}

	// No restart position found for this position (cval0-7 confirmed unchanged in this case)
	restart_status=0xff;
}


/*************************************
 *
 * Writes to C-Chip - Important Bits
 *
 *************************************/

WRITE16_HANDLER( bonzeadv_cchip_ctrl_w )
{
	/* value 2 is written here */
}

WRITE16_HANDLER( bonzeadv_cchip_bank_w )
{
	current_bank = data & 7;
}

WRITE16_HANDLER( bonzeadv_cchip_ram_w )
{
//  if (cpu_get_pc(space->cpu)!=0xa028)
//  logerror("%08x:  write %04x %04x cchip\n", cpu_get_pc(space->cpu), offset, data);

	if (current_bank == 0)
	{
		if (offset == 0x08)
		{
			cc_port = data;

			coin_lockout_w(1, data & 0x80);
			coin_lockout_w(0, data & 0x40);
			coin_counter_w(1, data & 0x20);
			coin_counter_w(0, data & 0x10);
		}

		if (offset == 0x0e && data != 0x00)
		{
			WriteRestartPos(current_round);
		}

		if (offset == 0x0f && data != 0x00)
		{
			WriteLevelData();
		}

		if (offset == 0x10)
		{
			current_round = data;
		}

		if (offset >= 0x11 && offset <= 0x2a)
		{
			cval[offset - 0x11] = data;
		}
	}
}

/*************************************
 *
 * Reads from C-Chip
 *
 *************************************/

READ16_HANDLER( bonzeadv_cchip_ctrl_r )
{
	/*
        Bit 2 = Error signal
        Bit 0 = Ready signal
    */
	return 0x01; /* Return 0x05 for C-Chip error */
}

READ16_HANDLER( bonzeadv_cchip_ram_r )
{
//  logerror("%08x:  read %04x cchip\n", cpu_get_pc(space->cpu), offset);

	if (current_bank == 0)
	{
		switch (offset)
		{
		case 0x03: return input_port_read(space->machine, "800007");    /* STARTn + SERVICE1 */
		case 0x04: return input_port_read(space->machine, "800009");    /* COINn */
		case 0x05: return input_port_read(space->machine, "80000B");    /* Player controls + TILT */
		case 0x06: return input_port_read(space->machine, "80000D");    /* Player controls (cocktail) */
		case 0x08: return cc_port;
		}

		if (offset == 0x0e)
		{
			return restart_status; /* 0xff signals error, 0 signals ok */
		}

		if (offset >= 0x11 && offset <= 0x2a)
		{
			return cval[offset - 0x11];
		}
	}

	return 0;
}
