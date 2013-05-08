/***************************************************************************

Quiz Do Re Mi Fa Grand Prix (Japan)     (GQ460) (c)1994 Konami
Quiz Do Re Mi Fa Grand Prix 2 (Japan)   (GE557) (c)1995 Konami


CPU  :MC68HC000FN16
OSC  :18.43200MHz/32.00000MHz
Other(GQ460):Konami 053252,054156,056832,054539
Other(GE557):Konami 056832,058141,058143

TODO:
- enabling irq acks breaks both games, why?

--
driver by Eisuke Watanabe

Note:
GP1 HDD data contents:
    0x000-0x52D intro quiz musics
    0x52E-0x535 not used quiz (system music or invalid data)
***************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "machine/idectrl.h"
#include "sound/k054539.h"
#include "video/konicdev.h"
#include "machine/k053252.h"
#include "machine/nvram.h"
#include "includes/qdrmfgp.h"


/*************************************
 *
 *  68k CPU memory handlers
 *
 *************************************/

CUSTOM_INPUT_MEMBER(qdrmfgp_state::inputs_r)
{
	const char *tag1 = (const char *)param;
	const char *tag2 = tag1 + strlen(tag1) + 1;
	return ioport((m_control & 0x0080) ? tag1 : tag2)->read();
}

CUSTOM_INPUT_MEMBER(qdrmfgp_state::battery_sensor_r)
{
	/* bit 0-1  battery power sensor: 3=good, 2=low, other=bad */
	return 0x0003;
}


WRITE16_MEMBER(qdrmfgp_state::gp_control_w)
{
	/* bit 0        enable irq 1 (sound) */
	/* bit 1        enable irq 2 (not used) */
	/* bit 2        enable irq 3 (vblank) */
	/* bit 3        enable irq 4 (hdd) */
	/* bit 4-6      palette (tilemap) */
	/* bit 7        inputports bankswitch */
	/* bit 8        enable volume control */
	/* bit 9        volume: 1=up, 0=down (low5,mid90,high180) */
	/* bit 10       enable headphone volume control */
	/* bit 11       headphone volume: 1=up, 0=down */
	/* bit 15       gfxrom bankswitch */

	COMBINE_DATA(&m_control);
	m_pal = m_control & 0x70;

	if (m_control & 0x0100)
	{
		int vol = m_nvram[0x10] & 0xff;
		if (vol)
		{
			int i;
			double gain = vol / 90.0;

			for (i=0; i<8; i++)
				m_k054539->set_gain(i, gain);
		}
	}
}

WRITE16_MEMBER(qdrmfgp_state::gp2_control_w)
{
	/* bit 2        enable irq 3 (sound) */
	/* bit 3        enable irq 4 (vblank) */
	/* bit 4        enable irq 5 (hdd) */
	/* bit 7        inputports bankswitch */
	/* bit 8        enable volume control */
	/* bit 9        volume: 1=up, 0=down (low0,mid90,high255) */
	/* bit 10       enable headphone volume control */
	/* bit 11       headphone volume: 1=up, 0=down */
	/* bit 15       gfxrom bankswitch */

	COMBINE_DATA(&m_control);
	m_pal = 0;

	if (m_control & 0x0100)
	{
		int vol = m_nvram[0x8] & 0xff;
		if (vol)
		{
			int i;
			double gain = vol / 90.0;

			for (i=0; i<8; i++)
				m_k054539->set_gain(i, gain);
		}
	}
}


READ16_MEMBER(qdrmfgp_state::v_rom_r)
{
	UINT8 *mem8 = memregion("gfx1")->base();
	int bank = k056832_word_r(m_k056832, space, 0x34/2, 0xffff);

	offset += bank * 0x800 * 4;

	if (m_control & 0x8000)
		offset += 0x800 * 2;

	return (mem8[offset + 1] << 8) + mem8[offset];
}


READ16_MEMBER(qdrmfgp_state::gp2_vram_r)
{
	if (offset < 0x1000 / 2)
		return k056832_ram_word_r(m_k056832, space, offset * 2 + 1, mem_mask);
	else
		return k056832_ram_word_r(m_k056832, space, (offset - 0x1000 / 2) * 2, mem_mask);
}

READ16_MEMBER(qdrmfgp_state::gp2_vram_mirror_r)
{
	if (offset < 0x1000 / 2)
		return k056832_ram_word_r(m_k056832, space, offset * 2, mem_mask);
	else
		return k056832_ram_word_r(m_k056832, space, (offset - 0x1000 / 2) * 2 + 1, mem_mask);
}

WRITE16_MEMBER(qdrmfgp_state::gp2_vram_w)
{
	if (offset < 0x1000 / 2)
		k056832_ram_word_w(m_k056832, space, offset * 2 + 1, data, mem_mask);
	else
		k056832_ram_word_w(m_k056832, space, (offset - 0x1000 / 2) * 2, data, mem_mask);
}

WRITE16_MEMBER(qdrmfgp_state::gp2_vram_mirror_w)
{
	if (offset < 0x1000 / 2)
		k056832_ram_word_w(m_k056832, space, offset * 2, data, mem_mask);
	else
		k056832_ram_word_w(m_k056832, space, (offset - 0x1000 / 2) * 2 + 1, data, mem_mask);
}


/*************/

READ16_MEMBER(qdrmfgp_state::sndram_r)
{
	if (ACCESSING_BITS_0_7)
		return m_sndram[offset];

	return 0;
}

WRITE16_MEMBER(qdrmfgp_state::sndram_w)
{
	if (ACCESSING_BITS_0_7)
	{
		m_sndram[offset] = data & 0xff;
		if (offset >= 0x40000)
			m_sndram[offset+0xc00000-0x900000] = data & 0xff;
	}
}

/*************/

#define IDE_STD_OFFSET  (0x1f0/2)
#define IDE_ALT_OFFSET  (0x3f6/2)

READ16_MEMBER(qdrmfgp_state::ide_std_r)
{
	device_t *device = machine().device("ide");
	if (offset & 0x01)
		return ide_controller16_r(device, space, IDE_STD_OFFSET + offset/2, 0xff00) >> 8;
	else
		return ide_controller16_r(device, space, IDE_STD_OFFSET + offset/2, 0xffff);
}

WRITE16_MEMBER(qdrmfgp_state::ide_std_w)
{
	device_t *device = machine().device("ide");
	if (offset & 0x01)
		ide_controller16_w(device, space, IDE_STD_OFFSET + offset/2, data << 8, 0xff00);
	else
		ide_controller16_w(device, space, IDE_STD_OFFSET + offset/2, data, 0xffff);
}

READ16_MEMBER(qdrmfgp_state::ide_alt_r)
{
	device_t *device = machine().device("ide");
	if (offset == 0)
		return ide_controller16_r(device, space, IDE_ALT_OFFSET, 0x00ff);

	return 0;
}

WRITE16_MEMBER(qdrmfgp_state::ide_alt_w)
{
	device_t *device = machine().device("ide");
	if (offset == 0)
		ide_controller16_w(device, space, IDE_ALT_OFFSET, data, 0x00ff);
}


READ16_MEMBER(qdrmfgp_state::gp2_ide_std_r)
{
	device_t *device = machine().device("ide");
	if (offset & 0x01)
	{
		if (offset == 0x07)
		{
			switch (space.device().safe_pcbase())
			{
				case 0xdb4c:
					if ((m_workram[0x5fa4/2] - space.device().state().state_int(M68K_D0)) <= 0x10)
						m_gp2_irq_control = 1;
					break;
				case 0xdec2:
					m_gp2_irq_control = 1;
				default:
					break;
			}
		}
		return ide_controller16_r(device, space, IDE_STD_OFFSET + offset/2, 0xff00) >> 8;
	} else {
		return ide_controller16_r(device, space, IDE_STD_OFFSET + offset/2, 0xffff);
	}
}


/*************************************
 *
 *  Interrupt handlers
 *
 *************************************/

TIMER_DEVICE_CALLBACK_MEMBER(qdrmfgp_state::qdrmfgp_interrupt)
{
	int scanline = param;

	if(scanline == 0)
		if (m_control & 0x0001)
			m_maincpu->set_input_line(1, HOLD_LINE);

	/* trigger V-blank interrupt */
	if(scanline == 240)
		if (m_control & 0x0004)
			m_maincpu->set_input_line(3, HOLD_LINE);
}

WRITE_LINE_MEMBER(qdrmfgp_state::ide_interrupt)
{
	if (m_control & 0x0008)
	{
		if (state != CLEAR_LINE)
			m_maincpu->set_input_line(4, HOLD_LINE);
		else
			m_maincpu->set_input_line(4, CLEAR_LINE);
	}
}

/*************/

TIMER_CALLBACK_MEMBER(qdrmfgp_state::gp2_timer_callback)
{
	if (m_control & 0x0004)
		m_maincpu->set_input_line(3, HOLD_LINE);
}

INTERRUPT_GEN_MEMBER(qdrmfgp_state::qdrmfgp2_interrupt)
{
	/* trigger V-blank interrupt */
	if (m_control & 0x0008)
		device.execute().set_input_line(4, HOLD_LINE);
}

WRITE_LINE_MEMBER(qdrmfgp_state::gp2_ide_interrupt)
{
	if (m_control & 0x0010)
	{
		if (state != CLEAR_LINE)
		{
			if (m_gp2_irq_control)
				m_gp2_irq_control = 0;
			else
				m_maincpu->set_input_line(5, HOLD_LINE);
		}
		else
		{
			m_maincpu->set_input_line(5, CLEAR_LINE);
		}
	}
}


/*************************************
 *
 *  Memory definitions
 *
 *************************************/

static ADDRESS_MAP_START( qdrmfgp_map, AS_PROGRAM, 16, qdrmfgp_state )
	AM_RANGE(0x000000, 0x0fffff) AM_ROM
	AM_RANGE(0x100000, 0x10ffff) AM_RAM AM_SHARE("workram")                                     /* work ram */
	AM_RANGE(0x180000, 0x183fff) AM_RAM AM_SHARE("nvram")   /* backup ram */
	AM_RANGE(0x280000, 0x280fff) AM_RAM_WRITE(paletteram_xBBBBBGGGGGRRRRR_word_w) AM_SHARE("paletteram")
	AM_RANGE(0x300000, 0x30003f) AM_DEVWRITE_LEGACY("k056832", k056832_word_w)                                      /* video reg */
	AM_RANGE(0x320000, 0x32001f) AM_DEVREADWRITE8_LEGACY("k053252", k053252_r, k053252_w,0x00ff)                    /* ccu */
	AM_RANGE(0x330000, 0x330001) AM_READ_PORT("SENSOR")                                         /* battery power & service sw */
	AM_RANGE(0x340000, 0x340001) AM_READ_PORT("340000")                                         /* inputport */
	AM_RANGE(0x350000, 0x350001) AM_WRITENOP                                                    /* unknown */
	AM_RANGE(0x360000, 0x360001) AM_WRITENOP                                                    /* unknown */
	AM_RANGE(0x370000, 0x370001) AM_WRITE(gp_control_w)                                         /* control reg */
	AM_RANGE(0x380000, 0x380001) AM_WRITENOP                                                    /* Watchdog */
	AM_RANGE(0x800000, 0x80045f) AM_DEVREADWRITE8("k054539", k054539_device, read, write, 0x00ff)        /* sound regs */
	AM_RANGE(0x880000, 0x881fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)          /* vram */
	AM_RANGE(0x882000, 0x883fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)          /* vram (mirror) */
	AM_RANGE(0x900000, 0x901fff) AM_READ(v_rom_r)                                               /* gfxrom through */
	AM_RANGE(0xa00000, 0xa0000f) AM_READWRITE(ide_std_r,ide_std_w)                  /* IDE control regs */
	AM_RANGE(0xa4000c, 0xa4000f) AM_READWRITE(ide_alt_r,ide_alt_w)                  /* IDE status control reg */
	AM_RANGE(0xc00000, 0xcbffff) AM_READWRITE(sndram_r, sndram_w)                               /* sound ram */
ADDRESS_MAP_END


static ADDRESS_MAP_START( qdrmfgp2_map, AS_PROGRAM, 16, qdrmfgp_state )
	AM_RANGE(0x000000, 0x0fffff) AM_ROM
	AM_RANGE(0x100000, 0x110fff) AM_RAM AM_SHARE("workram")                                     /* work ram */
	AM_RANGE(0x180000, 0x183fff) AM_RAM AM_SHARE("nvram")   /* backup ram */
	AM_RANGE(0x280000, 0x280fff) AM_RAM_WRITE(paletteram_xBBBBBGGGGGRRRRR_word_w) AM_SHARE("paletteram")
	AM_RANGE(0x300000, 0x30003f) AM_DEVWRITE_LEGACY("k056832", k056832_word_w)                                      /* video reg */
	AM_RANGE(0x320000, 0x32001f) AM_DEVREADWRITE8_LEGACY("k053252", k053252_r, k053252_w,0xff00)                    /* ccu */
	AM_RANGE(0x330000, 0x330001) AM_READ_PORT("SENSOR")                                         /* battery power & service */
	AM_RANGE(0x340000, 0x340001) AM_READ_PORT("340000")                                         /* inputport */
	AM_RANGE(0x350000, 0x350001) AM_WRITENOP                                                    /* unknown */
	AM_RANGE(0x360000, 0x360001) AM_WRITENOP                                                    /* unknown */
	AM_RANGE(0x370000, 0x370001) AM_WRITE(gp2_control_w)                                        /* control reg */
	AM_RANGE(0x380000, 0x380001) AM_WRITENOP                                                    /* Watchdog */
	AM_RANGE(0x800000, 0x80045f) AM_DEVREADWRITE8("k054539", k054539_device, read, write, 0x00ff)        /* sound regs */
	AM_RANGE(0x880000, 0x881fff) AM_READWRITE(gp2_vram_r, gp2_vram_w)                           /* vram */
	AM_RANGE(0x89f000, 0x8a0fff) AM_READWRITE(gp2_vram_mirror_r, gp2_vram_mirror_w)             /* vram (mirror) */
	AM_RANGE(0x900000, 0x901fff) AM_READ(v_rom_r)                                               /* gfxrom through */
	AM_RANGE(0xa00000, 0xa0000f) AM_READ(gp2_ide_std_r) AM_WRITE(ide_std_w)         /* IDE control regs */
	AM_RANGE(0xa4000c, 0xa4000f) AM_READWRITE(ide_alt_r,ide_alt_w)                  /* IDE status control reg */
	AM_RANGE(0xc00000, 0xcbffff) AM_READWRITE(sndram_r,sndram_w)                                /* sound ram */
ADDRESS_MAP_END


/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( qdrmfgp )
	PORT_START("340000")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, qdrmfgp_state,inputs_r, "INPUTS\0DSW")

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1)   /* 1P STOP */
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2)   /* 2P STOP */

	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)

	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_SERVICE1 ) /* SERVICE */
	PORT_SERVICE_NO_TOGGLE( 0x2000, IP_ACTIVE_LOW )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("DSW")
	PORT_BIT( 0x0003, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0004, 0x0004, "Extended Service Menu" )     /* and skipped initial checks. */
	PORT_DIPSETTING(    0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, "Skip HDD Check" )
	PORT_DIPSETTING(    0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, "Initialize Backup RAM" )
	PORT_DIPSETTING(    0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0500, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0900, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )

	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x3000, "All The Time" )
	PORT_DIPSETTING(      0x1000, "Once Every 2Cycles" )
	PORT_DIPSETTING(      0x2000, "Once Every 4Cycles" )
	PORT_DIPSETTING(      0x0000, "Completely Off" )
	PORT_DIPNAME( 0xc000, 0x4000, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Very_Hard ) )

	PORT_START("SENSOR")
	PORT_BIT( 0x0003, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, qdrmfgp_state,battery_sensor_r, NULL)   /* battery power sensor */
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0xfff0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( qdrmfgp2 )
	PORT_START("340000")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, qdrmfgp_state,inputs_r, "INPUTS\0DSW")

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1)   /* 1P STOP */
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(2)   /* 2P STOP */

	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)

	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_SERVICE1 ) /* SERVICE */
	PORT_SERVICE_NO_TOGGLE( 0x2000, IP_ACTIVE_LOW )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("DSW")
	PORT_BIT( 0x0003, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0004, 0x0004, "Extended Service Menu & None Sounds Mode" )      /* and skipped initial checks. */
	PORT_DIPSETTING(    0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0000, "Initialize Backup RAM" )
	PORT_DIPSETTING(    0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )

	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0500, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0900, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )

	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x3000, "All The Time" )
	PORT_DIPSETTING(      0x1000, "Once Every 2Cycles" )
	PORT_DIPSETTING(      0x2000, "Once Every 4Cycles" )
	PORT_DIPSETTING(      0x0000, "Completely Off" )
	PORT_DIPNAME( 0xc000, 0x4000, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Very_Hard ) )

	PORT_START("SENSOR")
	PORT_BIT( 0x0003, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, qdrmfgp_state,battery_sensor_r, NULL)   /* battery power sensor */
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0xfff0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


/*************************************
 *
 *  Graphics layouts
 *
 *************************************/


/*************************************
 *
 *  Sound interfaces
 *
 *************************************/

static void sound_irq(device_t *device)
{
	qdrmfgp_state *state = device->machine().driver_data<qdrmfgp_state>();
	if (state->m_control & 0x0001)
		state->m_maincpu->set_input_line(1, HOLD_LINE);
}

static const k054539_interface k054539_config =
{
	NULL,
	NULL,
	sound_irq
};


/*************************************
 *
 *  Machine-specific init
 *
 *************************************/

static const k056832_interface qdrmfgp_k056832_intf =
{
	"gfx1", 0,
	K056832_BPP_4dj,
	1, 0,
	KONAMI_ROM_DEINTERLEAVE_NONE,
	qdrmfgp_tile_callback, "none"
};

static const k056832_interface qdrmfgp2_k056832_intf =
{
	"gfx1", 0,
	K056832_BPP_4dj,
	1, 0,
	KONAMI_ROM_DEINTERLEAVE_NONE,
	qdrmfgp2_tile_callback, "none"
};

WRITE_LINE_MEMBER(qdrmfgp_state::qdrmfgp_irq3_ack_w)
{
//  m_maincpu->set_input_line(M68K_IRQ_3, CLEAR_LINE);
}

WRITE_LINE_MEMBER(qdrmfgp_state::qdrmfgp_irq4_ack_w)
{
//  m_maincpu->set_input_line(M68K_IRQ_4, CLEAR_LINE);
}

static const k053252_interface qdrmfgp_k053252_intf =
{
	"screen",
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_LINE_MEMBER(qdrmfgp_state,qdrmfgp_irq3_ack_w),
	DEVCB_NULL,
	40, 16
};

static const k053252_interface qdrmfgp2_k053252_intf =
{
	"screen",
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_LINE_MEMBER(qdrmfgp_state,qdrmfgp_irq4_ack_w),
	DEVCB_DRIVER_LINE_MEMBER(qdrmfgp_state,qdrmfgp_irq3_ack_w),
	40, 16
};

MACHINE_START_MEMBER(qdrmfgp_state,qdrmfgp)
{
	save_item(NAME(m_control));
	save_item(NAME(m_pal));
	save_item(NAME(m_gp2_irq_control));
}

MACHINE_START_MEMBER(qdrmfgp_state,qdrmfgp2)
{
	/* sound irq (CCU? 240Hz) */
	machine().scheduler().timer_pulse(attotime::from_hz(18432000/76800), timer_expired_delegate(FUNC(qdrmfgp_state::gp2_timer_callback),this));

	MACHINE_START_CALL_MEMBER( qdrmfgp );
}

void qdrmfgp_state::machine_reset()
{
	m_sndram = memregion("k054539")->base() + 0x100000;

	/* reset the IDE controller */
	m_gp2_irq_control = 0;
	machine().device("ide")->reset();
}


/*************************************
 *
 *  Machine driver
 *
 *************************************/
static MACHINE_CONFIG_START( qdrmfgp, qdrmfgp_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 32000000/2) /*  16.000 MHz */
	MCFG_CPU_PROGRAM_MAP(qdrmfgp_map)
	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer", qdrmfgp_state, qdrmfgp_interrupt, "screen", 0, 1)

	MCFG_MACHINE_START_OVERRIDE(qdrmfgp_state,qdrmfgp)
	MCFG_NVRAM_ADD_1FILL("nvram")

	MCFG_IDE_CONTROLLER_ADD("ide", ide_devices, "hdd", NULL, true)
	MCFG_IDE_CONTROLLER_IRQ_HANDLER(DEVWRITELINE(DEVICE_SELF, qdrmfgp_state, ide_interrupt))

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(40, 40+384-1, 16, 16+224-1)
	MCFG_SCREEN_UPDATE_DRIVER(qdrmfgp_state, screen_update_qdrmfgp)

	MCFG_PALETTE_LENGTH(2048)

	MCFG_VIDEO_START_OVERRIDE(qdrmfgp_state,qdrmfgp)

	MCFG_K056832_ADD("k056832", qdrmfgp_k056832_intf)
	MCFG_K053252_ADD("k053252", 32000000/4, qdrmfgp_k053252_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_K054539_ADD("k054539", 48000, k054539_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( qdrmfgp2, qdrmfgp_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 32000000/2) /*  16.000 MHz */
	MCFG_CPU_PROGRAM_MAP(qdrmfgp2_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", qdrmfgp_state,  qdrmfgp2_interrupt)

	MCFG_MACHINE_START_OVERRIDE(qdrmfgp_state,qdrmfgp2)
	MCFG_NVRAM_ADD_1FILL("nvram")

	MCFG_IDE_CONTROLLER_ADD("ide", ide_devices, "hdd", NULL, true)
	MCFG_IDE_CONTROLLER_IRQ_HANDLER(DEVWRITELINE(DEVICE_SELF, qdrmfgp_state, gp2_ide_interrupt))

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(40, 40+384-1, 16, 16+224-1)
	MCFG_SCREEN_UPDATE_DRIVER(qdrmfgp_state, screen_update_qdrmfgp)

	MCFG_PALETTE_LENGTH(2048)

	MCFG_VIDEO_START_OVERRIDE(qdrmfgp_state,qdrmfgp2)

	MCFG_K056832_ADD("k056832", qdrmfgp2_k056832_intf)
	MCFG_K053252_ADD("k053252", 32000000/4, qdrmfgp2_k053252_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_K054539_ADD("k054539", 48000, k054539_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_CONFIG_END


/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( qdrmfgp )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "gq_460_b04.20e", 0x000000, 0x80000, CRC(293d8174) SHA1(cf507d0b29dab161190f0160c05c640f16306bae) )
	ROM_LOAD16_WORD_SWAP( "gq_460_a05.22e", 0x080000, 0x80000, CRC(4128cb3c) SHA1(4a16d85a66934a20afd074546de362c40a1ea785) )

	ROM_REGION( 0x100000, "gfx1", 0 )       /* TILEMAP */
	ROM_LOAD( "gq_460_a01.15e", 0x000000, 0x80000, CRC(6536b700) SHA1(47ffe0cfbf80810179560150b23d825fe1a5c5ca) )
	ROM_LOAD( "gq_460_a02.17e", 0x080000, 0x80000, CRC(ac01d675) SHA1(bf66433ace95f4ef14699d03add7cbc2e5d90eea) )

	ROM_REGION( 0x460000, "k054539", 0)      /* SE SAMPLES + space for additional RAM */
	ROM_LOAD( "gq_460_a07.14h", 0x000000, 0x80000, CRC(67d8ea6b) SHA1(11af1b5a33de2a6e24823964d210bef193ecefe4) )
	ROM_LOAD( "gq_460_a06.12h", 0x080000, 0x80000, CRC(97ed5a77) SHA1(68600fd8d914451284cf181fb4bd5872860fb9ad) )

	DISK_REGION( "drive_0" )            /* IDE HARD DRIVE */
	DISK_IMAGE( "gq460a08", 0, SHA1(2f142f986fa3c79d5c4102e800980d1706c35f75) )
ROM_END

ROM_START( qdrmfgp2 )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "ge_557_c05.20e", 0x000000, 0x80000, CRC(336df99f) SHA1(46fb36d40371761be0cfa17b34f28cc893a44a22) )
	ROM_LOAD16_WORD_SWAP( "ge_557_a06.22e", 0x080000, 0x80000, CRC(ad77e10f) SHA1(4a762a59fe3096d48e3cbf0da3bb0d75c5087e78) )

	ROM_REGION( 0x100000, "gfx1", 0 )       /* TILEMAP */
	ROM_LOAD( "ge_557_a01.13e", 0x000000, 0x80000, CRC(c301d406) SHA1(5fad8cc611edd83380972abf37ec80561b9317a6) )
	ROM_LOAD( "ge_557_a02.15e", 0x080000, 0x80000, CRC(3bfe1e56) SHA1(9e4df512a804a96fcb545d4e0eb58b5421d65ea4) )

	ROM_REGION( 0x460000, "k054539", 0)      /* SE SAMPLES + space for additional RAM */
	ROM_LOAD( "ge_557_a07.19h", 0x000000, 0x80000, CRC(7491e0c8) SHA1(6459ab5e7af052ef7a1c4ce01cd844c0f4319f2e) )
	ROM_LOAD( "ge_557_a08.19k", 0x080000, 0x80000, CRC(3da2b20c) SHA1(fdc2cdc27f3299f541944a78ce36ed33a7926056) )

	DISK_REGION( "drive_0" )            /* IDE HARD DRIVE */
	DISK_IMAGE( "ge557a09", 0, SHA1(1ef8093b542fe0bf8240a5fd64e5af3839b6a04c) )
ROM_END


/*************************************
 *
 *  Game drivers
 *
 *************************************/

/*     year  rom       clone     machine   inputs    init */
GAME(  1994, qdrmfgp,  0,        qdrmfgp,  qdrmfgp, driver_device,  0,        ROT0, "Konami", "Quiz Do Re Mi Fa Grand Prix (Japan)", 0 )
GAME(  1995, qdrmfgp2, 0,        qdrmfgp2, qdrmfgp2, driver_device, 0,        ROT0, "Konami", "Quiz Do Re Mi Fa Grand Prix 2 - Shin-Kyoku Nyuukadayo (Japan)", 0 )
