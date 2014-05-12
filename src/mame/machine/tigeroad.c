#include "emu.h"
#include "includes/tigeroad.h"

/*
 F1 Dream protection code written by Eric Hustvedt (hustvedt@ma.ultranet.com).

 The genuine F1 Dream game uses an 8751 microcontroller as a protection measure.
 Since the microcontroller's ROM is unavailable all interactions with it are handled
 via blackbox algorithm.

 Some notes:
 - The 8751 is triggered via location 0xfe4002, in place of the soundlatch normally
    present. The main cpu writes 0 to the location when it wants the 8751 to perform some work.
 - The 8751 has memory which shadows locations 0xffffe0-0xffffff of the main cpu's address space.
 - The word at 0xffffe0 contains an 'opcode' which is written just before the write to 0xfe4002.
 - Some of the writes to the soundlatch may not be handled. 0x27fc is the main sound routine, the
    other locations are less frequently used.
*/

static const int f1dream_613ea_lookup[16] = {
0x0052, 0x0031, 0x00a7, 0x0043, 0x0007, 0x008a, 0x00b1, 0x0066, 0x009f, 0x00cc, 0x0009, 0x004d, 0x0033, 0x0028, 0x00d0, 0x0025};

static const int f1dream_613eb_lookup[256] = {
0x0001, 0x00b5, 0x00b6, 0x00b6, 0x00b6, 0x00b6, 0x00b6, 0x00b6, 0x00b7, 0x0001, 0x00b8, 0x002f, 0x002f, 0x002f, 0x002f, 0x00b9,
0x00aa, 0x0031, 0x00ab, 0x00ab, 0x00ab, 0x00ac, 0x00ad, 0x00ad, 0x00ae, 0x00af, 0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x0091,
0x009c, 0x009d, 0x009e, 0x009f, 0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, 0x00a8, 0x00a9, 0x009b, 0x0091,
0x00bc, 0x0092, 0x000b, 0x0009, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0073, 0x0001, 0x0098, 0x0099, 0x009a, 0x009b, 0x0091,
0x00bc, 0x007b, 0x000b, 0x0008, 0x0087, 0x0088, 0x0089, 0x008a, 0x007f, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, 0x0090, 0x0091,
0x00bd, 0x007b, 0x000b, 0x0007, 0x007c, 0x007d, 0x007e, 0x0001, 0x007f, 0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086,
0x00bc, 0x0070, 0x000b, 0x0006, 0x0071, 0x0072, 0x0073, 0x0001, 0x0074, 0x000d, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a,
0x00bc, 0x00ba, 0x000a, 0x0005, 0x0065, 0x0066, 0x0067, 0x0068, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
0x00bc, 0x0059, 0x0001, 0x0004, 0x005a, 0x005b, 0x0001, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060, 0x0061, 0x0062, 0x0063, 0x0064,
0x0014, 0x004d, 0x0001, 0x0003, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0001, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
0x0014, 0x0043, 0x0001, 0x0002, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x00bb, 0x004a, 0x004b, 0x004c, 0x0001, 0x0001,
0x0014, 0x002b, 0x0001, 0x0038, 0x0039, 0x003a, 0x003b, 0x0031, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0001,
0x0014, 0x002d, 0x0001, 0x002e, 0x002f, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0001, 0x0014, 0x0037, 0x0001,
0x0014, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x0001, 0x0001, 0x0001, 0x002a, 0x002b, 0x002c,
0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001e, 0x001e, 0x001e, 0x001f, 0x0020,
0x000c, 0x000d, 0x000e, 0x0001, 0x000f, 0x0010, 0x0011, 0x0012, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x000d, 0x0013 };

static const int f1dream_17b74_lookup[128] = {
0x0003, 0x0040, 0x0005, 0x0080, 0x0003, 0x0080, 0x0005, 0x00a0, 0x0003, 0x0040, 0x0005, 0x00c0, 0x0003, 0x0080, 0x0005, 0x00e0,
0x0003, 0x0040, 0x0006, 0x0000, 0x0003, 0x0080, 0x0006, 0x0020, 0x0003, 0x0040, 0x0006, 0x0040, 0x0003, 0x0080, 0x0006, 0x0060,
0x0000, 0x00a0, 0x0009, 0x00e0, 0x0000, 0x00e0, 0x000a, 0x0000, 0x0000, 0x00a0, 0x000a, 0x0020, 0x0000, 0x00e0, 0x000a, 0x0040,
0x0000, 0x00a0, 0x000a, 0x0060, 0x0000, 0x00e0, 0x000a, 0x0080, 0x0000, 0x00a0, 0x000a, 0x00a0, 0x0000, 0x00e0, 0x000a, 0x00c0,
0x0003, 0x0040, 0x0005, 0x0080, 0x0003, 0x0080, 0x0005, 0x00a0, 0x0003, 0x0040, 0x0005, 0x00c0, 0x0003, 0x0080, 0x0005, 0x00e0,
0x0003, 0x0040, 0x0006, 0x0000, 0x0003, 0x0080, 0x0006, 0x0020, 0x0003, 0x0040, 0x0006, 0x0040, 0x0003, 0x0080, 0x0006, 0x0060,
0x0000, 0x00a0, 0x0009, 0x00e0, 0x0000, 0x00e0, 0x000a, 0x0000, 0x0000, 0x00a0, 0x000a, 0x0020, 0x0000, 0x00e0, 0x000a, 0x0040,
0x0000, 0x00a0, 0x000a, 0x0060, 0x0000, 0x00e0, 0x000a, 0x0080, 0x0000, 0x00a0, 0x000a, 0x00a0, 0x0000, 0x00e0, 0x000a, 0x00c0 };

static const int f1dream_2450_lookup[32] = {
0x0003, 0x0080, 0x0006, 0x0060, 0x0000, 0x00e0, 0x000a, 0x00c0, 0x0003, 0x0080, 0x0006, 0x0060, 0x0000, 0x00e0, 0x000a, 0x00c0,
0x0003, 0x0080, 0x0006, 0x0060, 0x0000, 0x00e0, 0x000a, 0x00c0, 0x0003, 0x0080, 0x0006, 0x0060, 0x0000, 0x00e0, 0x000a, 0x00c0 };

void tigeroad_state::f1dream_protection_w(address_space &space)
{
	int indx;
	int value = 255;
	int prevpc = space.device().safe_pcbase();

	if (prevpc == 0x244c)
	{
		/* Called once, when a race is started.*/
		indx = m_ram16[0x3ff0/2];
		m_ram16[0x3fe6/2] = f1dream_2450_lookup[indx];
		m_ram16[0x3fe8/2] = f1dream_2450_lookup[++indx];
		m_ram16[0x3fea/2] = f1dream_2450_lookup[++indx];
		m_ram16[0x3fec/2] = f1dream_2450_lookup[++indx];
	}
	else if (prevpc == 0x613a)
	{
		/* Called for every sprite on-screen.*/
		if (m_ram16[0x3ff6/2] < 15)
		{
			indx = f1dream_613ea_lookup[m_ram16[0x3ff6/2]] - m_ram16[0x3ff4/2];
			if (indx > 255)
			{
				indx <<= 4;
				indx += m_ram16[0x3ff6/2] & 0x00ff;
				value = f1dream_613eb_lookup[indx];
			}
		}

		m_ram16[0x3ff2/2] = value;
	}
	else if (prevpc == 0x17b70)
	{
		/* Called only before a real race, not a time trial.*/
		if (m_ram16[0x3ff0/2] >= 0x04) indx = 128;
		else if (m_ram16[0x3ff0/2] > 0x02) indx = 96;
		else if (m_ram16[0x3ff0/2] == 0x02) indx = 64;
		else if (m_ram16[0x3ff0/2] == 0x01) indx = 32;
		else indx = 0;

		indx += m_ram16[0x3fee/2];
		if (indx < 128)
		{
			m_ram16[0x3fe6/2] = f1dream_17b74_lookup[indx];
			m_ram16[0x3fe8/2] = f1dream_17b74_lookup[++indx];
			m_ram16[0x3fea/2] = f1dream_17b74_lookup[++indx];
			m_ram16[0x3fec/2] = f1dream_17b74_lookup[++indx];
		}
		else
		{
			m_ram16[0x3fe6/2] = 0x00ff;
			m_ram16[0x3fe8/2] = 0x00ff;
			m_ram16[0x3fea/2] = 0x00ff;
			m_ram16[0x3fec/2] = 0x00ff;
		}
	}
	else if ((prevpc == 0x27f8) || (prevpc == 0x511a) || (prevpc == 0x5142) || (prevpc == 0x516a))
	{
		/* The main CPU stuffs the byte for the soundlatch into 0xfffffd.*/
		soundlatch_byte_w(space,2,m_ram16[0x3ffc/2]);
	}
}

WRITE16_MEMBER(tigeroad_state::f1dream_control_w)
{
	logerror("protection write, PC: %04x  FFE1 Value:%01x\n",space.device().safe_pc(), m_ram16[0x3fe0/2]);
	f1dream_protection_w(space);
}


READ16_MEMBER(tigeroad_state::pushman_68705_r)
{
	if (offset == 0)
		return m_latch;

	if (offset == 3 && m_new_latch)
	{
		m_new_latch = 0;
		return 0;
	}
	if (offset == 3 && !m_new_latch)
		return 0xff;

	return (m_shared_ram[2 * offset + 1] << 8) + m_shared_ram[2 * offset];
}

WRITE16_MEMBER(tigeroad_state::pushman_68705_w)
{
	if (ACCESSING_BITS_8_15)
		m_shared_ram[2 * offset] = data >> 8;
	if (ACCESSING_BITS_0_7)
		m_shared_ram[2 * offset + 1] = data & 0xff;

	if (offset == 1)
	{
		m_mcu->set_input_line(M68705_IRQ_LINE, HOLD_LINE);
		space.device().execute().spin();
		m_new_latch = 0;
	}
}

/* ElSemi - Bouncing balls protection. */
READ16_MEMBER(tigeroad_state::bballs_68705_r)
{
	if (offset == 0)
		return m_latch;
	if (offset == 3 && m_new_latch)
	{
		m_new_latch = 0;
		return 0;
	}
	if (offset == 3 && !m_new_latch)
		return 0xff;

	return (m_shared_ram[2 * offset + 1] << 8) + m_shared_ram[2 * offset];
}

WRITE16_MEMBER(tigeroad_state::bballs_68705_w)
{
	if (ACCESSING_BITS_8_15)
		m_shared_ram[2 * offset] = data >> 8;
	if (ACCESSING_BITS_0_7)
		m_shared_ram[2 * offset + 1] = data & 0xff;

	if (offset == 0)
	{
		m_latch = 0;
		if (m_shared_ram[0] <= 0xf)
		{
			m_latch = m_shared_ram[0] << 2;
			if (m_shared_ram[1])
				m_latch |= 2;
			m_new_latch = 1;
		}
		else if (m_shared_ram[0])
		{
			if (m_shared_ram[1])
				m_latch |= 2;
			m_new_latch = 1;
		}
	}
}


READ8_MEMBER(tigeroad_state::pushman_68000_r)
{
	return m_shared_ram[offset];
}

WRITE8_MEMBER(tigeroad_state::pushman_68000_w)
{
	if (offset == 2 && (m_shared_ram[2] & 2) == 0 && data & 2)
	{
		m_latch = (m_shared_ram[1] << 8) | m_shared_ram[0];
		m_new_latch = 1;
	}
	m_shared_ram[offset] = data;
}