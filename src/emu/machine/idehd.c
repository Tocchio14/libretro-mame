// license:MAME
// copyright-holders:smf
#include "idehd.h"

/***************************************************************************
    DEBUGGING
***************************************************************************/

#define VERBOSE                     0
#define PRINTF_IDE_COMMANDS         0
#define PRINTF_IDE_PASSWORD         0

#define LOGPRINT(x) do { if (VERBOSE) logerror x; if (PRINTF_IDE_COMMANDS) mame_printf_debug x; } while (0)

#define TIME_PER_SECTOR                     (attotime::from_usec(100))
#define TIME_PER_ROTATION                   (attotime::from_hz(5400/60))
#define TIME_BETWEEN_SECTORS                (attotime::from_nsec(400))

#define TIME_FULL_STROKE_SEEK               (attotime::from_usec(13000))
#define TIME_AVERAGE_ROTATIONAL_LATENCY     (attotime::from_usec(1300))

ata_mass_storage_device::ata_mass_storage_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock,const char *shortname, const char *source)
	: ata_hle_device(mconfig, type, name, tag, owner, clock, shortname, source),
	m_can_identify_device(0),
	m_master_password(NULL),
	m_user_password(NULL)
{
}

/*************************************
 *
 *  Compute the LBA address
 *
 *************************************/

UINT32 ata_mass_storage_device::lba_address()
{
	/* LBA direct? */
	if (m_device_head & IDE_DEVICE_HEAD_L)
		return ((m_device_head & IDE_DEVICE_HEAD_HS) << 24) | (m_cylinder_high << 16) | (m_cylinder_low << 8) | m_sector_number;

	/* standard CHS */
	else
		return (((((m_cylinder_high << 8) | m_cylinder_low) * m_num_heads) + (m_device_head & IDE_DEVICE_HEAD_HS)) * m_num_sectors) + m_sector_number - 1;
}


/*************************************
 *
 *  Build a features page
 *
 *************************************/

static void swap_strncpy(UINT16 *dst, const char *src, int field_size_in_words)
{
	for (int i = 0; i < field_size_in_words; i++)
	{
		UINT16 d = 0;

		if (*src)
		{
			d = *(src++) << 8;
		}
		else
		{
			d = ' ' << 8;
		}

		if (*src)
		{
			d |= *(src++);
		}
		else
		{
			d |= ' ';
		}

		dst[i] = d;
	}

	assert(*(src) == 0);
}


void ata_mass_storage_device::ide_build_identify_device()
{
	memset(m_identify_buffer, 0, sizeof(m_identify_buffer));
	int total_sectors = m_num_cylinders * m_num_heads * m_num_sectors;

	/* basic geometry */
	m_identify_buffer[0] = 0x045a;                    /*  0: configuration bits */
	m_identify_buffer[1] = m_num_cylinders;           /*  1: logical cylinders */
	m_identify_buffer[2] = 0;                         /*  2: reserved */
	m_identify_buffer[3] = m_num_heads;               /*  3: logical heads */
	m_identify_buffer[4] = 0;                         /*  4: vendor specific (obsolete) */
	m_identify_buffer[5] = 0;                         /*  5: vendor specific (obsolete) */
	m_identify_buffer[6] = m_num_sectors;             /*  6: logical sectors per logical track */
	m_identify_buffer[7] = 0;                         /*  7: vendor-specific */
	m_identify_buffer[8] = 0;                         /*  8: vendor-specific */
	m_identify_buffer[9] = 0;                         /*  9: vendor-specific */
	swap_strncpy(&m_identify_buffer[10],               /* 10-19: serial number */
			"00000000000000000000", 10);
	m_identify_buffer[20] = 0;                         /* 20: vendor-specific */
	m_identify_buffer[21] = 0;                         /* 21: vendor-specific */
	m_identify_buffer[22] = 4;                         /* 22: # of vendor-specific bytes on read/write long commands */
	swap_strncpy(&m_identify_buffer[23],               /* 23-26: firmware revision */
			"1.0", 4);
	swap_strncpy(&m_identify_buffer[27],               /* 27-46: model number */
			"MAME Compressed Hard Disk", 20);
	m_identify_buffer[47] = 0x8001;                    /* 47: read/write multiple support */
	m_identify_buffer[48] = 0;                         /* 48: reserved */
	m_identify_buffer[49] = 0x0f03;                    /* 49: capabilities */
	m_identify_buffer[50] = 0;                         /* 50: reserved */
	m_identify_buffer[51] = 2;                         /* 51: PIO data transfer cycle timing mode */
	m_identify_buffer[52] = 2;                         /* 52: single word DMA transfer cycle timing mode */
	m_identify_buffer[53] = 3;                         /* 53: field validity */
	m_identify_buffer[54] = m_num_cylinders;           /* 54: number of current logical cylinders */
	m_identify_buffer[55] = m_num_heads;               /* 55: number of current logical heads */
	m_identify_buffer[56] = m_num_sectors;             /* 56: number of current logical sectors per track */
	m_identify_buffer[57] = total_sectors & 0xffff;    /* 57-58: current capacity in sectors (ATA-1 through ATA-5; obsoleted in ATA-6) */
	m_identify_buffer[58] = total_sectors >> 16;
	m_identify_buffer[59] = 0;                         /* 59: multiple sector timing */
	m_identify_buffer[60] = total_sectors & 0xffff;    /* 60-61: total user addressable sectors for LBA mode (ATA-1 through ATA-7) */
	m_identify_buffer[61] = total_sectors >> 16;
	m_identify_buffer[62] = 0x0007;                    /* 62: single word dma transfer */
	m_identify_buffer[63] = 0x0407;                    /* 63: multiword DMA transfer */
	m_identify_buffer[64] = 0x0003;                    /* 64: flow control PIO transfer modes supported */
	m_identify_buffer[65] = 0x78;                      /* 65: minimum multiword DMA transfer cycle time per word */
	m_identify_buffer[66] = 0x78;                      /* 66: mfr's recommended multiword DMA transfer cycle time */
	m_identify_buffer[67] = 0x014d;                    /* 67: minimum PIO transfer cycle time without flow control */
	m_identify_buffer[68] = 0x78;                      /* 68: minimum PIO transfer cycle time with IORDY */
	m_identify_buffer[69] = 0x00;                      /* 69-70: reserved */
	m_identify_buffer[71] = 0x00;                      /* 71: reserved for IDENTIFY PACKET command */
	m_identify_buffer[72] = 0x00;                      /* 72: reserved for IDENTIFY PACKET command */
	m_identify_buffer[73] = 0x00;                      /* 73: reserved for IDENTIFY PACKET command */
	m_identify_buffer[74] = 0x00;                      /* 74: reserved for IDENTIFY PACKET command */
	m_identify_buffer[75] = 0x00;                      /* 75: queue depth */
	m_identify_buffer[76] = 0x00;                      /* 76-79: reserved */
	m_identify_buffer[80] = 0x00;                      /* 80: major version number */
	m_identify_buffer[81] = 0x00;                      /* 81: minor version number */
	m_identify_buffer[82] = 0x00;                      /* 82: command set supported */
	m_identify_buffer[83] = 0x00;                      /* 83: command sets supported */
	m_identify_buffer[84] = 0x00;                      /* 84: command set/feature supported extension */
	m_identify_buffer[85] = 0x00;                      /* 85: command set/feature enabled */
	m_identify_buffer[86] = 0x00;                      /* 86: command set/feature enabled */
	m_identify_buffer[87] = 0x00;                      /* 87: command set/feature default */
	m_identify_buffer[88] = 0x00;                      /* 88: additional DMA modes */
	m_identify_buffer[89] = 0x00;                      /* 89: time required for security erase unit completion */
	m_identify_buffer[90] = 0x00;                      /* 90: time required for enhanced security erase unit completion */
	m_identify_buffer[91] = 0x00;                      /* 91: current advanced power management value */
	m_identify_buffer[92] = 0x00;                      /* 92: master password revision code */
	m_identify_buffer[93] = 0x00;                      /* 93: hardware reset result */
	m_identify_buffer[94] = 0x00;                      /* 94: acoustic management values */
	m_identify_buffer[95] = 0x00;                      /* 95-99: reserved */
	m_identify_buffer[100] = total_sectors & 0xffff;   /* 100-103: maximum 48-bit LBA */
	m_identify_buffer[101] = total_sectors >> 16;
	m_identify_buffer[102] = 0x00;
	m_identify_buffer[103] = 0x00;
	m_identify_buffer[104] = 0x00;                     /* 104-126: reserved */
	m_identify_buffer[127] = 0x00;                     /* 127: removable media status notification */
	m_identify_buffer[128] = 0x00;                     /* 128: security status */
	m_identify_buffer[129] = 0x00;                     /* 129-159: vendor specific */
	m_identify_buffer[160] = 0x00;                     /* 160: CFA power mode 1 */
	m_identify_buffer[161] = 0x00;                     /* 161-175: reserved for CompactFlash */
	m_identify_buffer[176] = 0x00;                     /* 176-205: current media serial number */
	m_identify_buffer[206] = 0x00;                     /* 206-254: reserved */
	m_identify_buffer[255] = 0x00;                     /* 255: integrity word */
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void ata_mass_storage_device::device_start()
{
	ata_hle_device::device_start();

	save_item(NAME(m_can_identify_device));
	save_item(NAME(m_cur_lba));
	save_item(NAME(m_sectors_until_int));
	save_item(NAME(m_master_password_enable));
	save_item(NAME(m_user_password_enable));
	save_item(NAME(m_block_count));
}

void ata_mass_storage_device::soft_reset()
{
	ata_hle_device::soft_reset();

	m_cur_lba = 0;
	m_status |= IDE_STATUS_DSC;

	m_master_password_enable = (m_master_password != NULL);
	m_user_password_enable = (m_user_password != NULL);
}

void ata_mass_storage_device::perform_diagnostic()
{
	if (m_can_identify_device)
		m_error = IDE_ERROR_DIAGNOSTIC_PASSED;
}

void ata_mass_storage_device::signature()
{
	m_sector_count = 1;
	m_sector_number = 1;
	m_cylinder_low = 0;
	m_cylinder_high = 0;
	m_device_head = 0;
}

void ata_mass_storage_device::finished_command()
{
	switch (m_command)
	{
	case IDE_COMMAND_IDENTIFY_DEVICE:
		if (m_can_identify_device)
		{
			for( int w = 0; w < 256; w++ )
			{
				m_buffer[w * 2] = m_identify_buffer[ w ] & 0xff;
				m_buffer[(w * 2) + 1] = m_identify_buffer[ w ] >> 8;
			}

			m_status |= IDE_STATUS_DRQ;
		}
		else
		{
			m_status |= IDE_STATUS_ERR;
			m_error = IDE_ERROR_NONE;
		}

		set_irq(ASSERT_LINE);
		break;

	case IDE_COMMAND_SET_CONFIG:
		set_geometry(m_sector_count,(m_device_head & IDE_DEVICE_HEAD_HS) + 1);
		set_irq(ASSERT_LINE);
		break;

	case IDE_COMMAND_READ_SECTORS:
	case IDE_COMMAND_READ_SECTORS_NORETRY:
	case IDE_COMMAND_READ_MULTIPLE:
	case IDE_COMMAND_VERIFY_SECTORS:
	case IDE_COMMAND_VERIFY_SECTORS_NORETRY:
	case IDE_COMMAND_READ_DMA:
		finished_read();
		break;

	case IDE_COMMAND_WRITE_SECTORS:
	case IDE_COMMAND_WRITE_SECTORS_NORETRY:
	case IDE_COMMAND_WRITE_MULTIPLE:
	case IDE_COMMAND_WRITE_DMA:
		finished_write();
		break;

	case IDE_COMMAND_RECALIBRATE:
		set_irq(ASSERT_LINE);
		break;

	default:
		ata_hle_device::finished_command();
		break;
	}
}

/*************************************
 *
 *  Advance to the next sector
 *
 *************************************/

void ata_mass_storage_device::next_sector()
{
	UINT8 cur_head = m_device_head & IDE_DEVICE_HEAD_HS;

	/* LBA direct? */
	if (m_device_head & IDE_DEVICE_HEAD_L)
	{
		m_sector_number++;
		if (m_sector_number == 0)
		{
			m_cylinder_low++;
			if (m_cylinder_low == 0)
			{
				m_cylinder_high++;
				if (m_cylinder_high == 0)
					cur_head++;
			}
		}
	}

	/* standard CHS */
	else
	{
		/* sectors are 1-based */
		m_sector_number++;
		if (m_sector_number > m_num_sectors)
		{
			/* heads are 0 based */
			m_sector_number = 1;
			cur_head++;
			if (cur_head >= m_num_heads)
			{
				cur_head = 0;
				m_cylinder_low++;
				if (m_cylinder_low == 0)
					m_cylinder_high++;
			}
		}
	}

	m_device_head = (m_device_head & ~IDE_DEVICE_HEAD_HS) | cur_head;

	m_cur_lba = lba_address();
}



/*************************************
 *
 *  security error handling
 *
 *************************************/

void ata_mass_storage_device::security_error()
{
	/* set error state */
	m_status |= IDE_STATUS_ERR;
	m_error = IDE_ERROR_NONE;
	m_status &= ~IDE_STATUS_DRDY;
}



/*************************************
 *
 *  Sector reading
 *
 *************************************/

attotime ata_mass_storage_device::seek_time()
{
	/* just set a timer */
	int new_lba = lba_address();

	int old_cylinder = m_cur_lba / (m_num_heads * m_num_sectors);
	int new_cylinder = new_lba / (m_num_heads * m_num_sectors);
	int diff = abs(old_cylinder - new_cylinder);

	m_cur_lba = new_lba;

	if (diff == 0)
		return TIME_BETWEEN_SECTORS;

	attotime seek_time = (TIME_FULL_STROKE_SEEK * diff) / m_num_cylinders;

	return seek_time + TIME_AVERAGE_ROTATIONAL_LATENCY;
}

void ata_mass_storage_device::fill_buffer()
{
	switch (m_command)
	{
	case IDE_COMMAND_IDENTIFY_DEVICE:
		break;

	default:
		/* if there is more data to read, keep going */
		if (m_sector_count > 0)
			m_sector_count--;

		if (m_sector_count > 0)
		{
			set_dasp(ASSERT_LINE);

			start_busy(TIME_BETWEEN_SECTORS, PARAM_COMMAND);
		}
		break;
	}
}


void ata_mass_storage_device::finished_read()
{
	int lba = lba_address(), count = 0;

	set_dasp(CLEAR_LINE);

	/* now do the read */
	count = read_sector(lba, m_buffer);

	/* if we succeeded, advance to the next sector and set the nice bits */
	if (count == 1)
	{
		/* advance the pointers, unless this is the last sector */
		/* Gauntlet: Dark Legacy checks to make sure we stop on the last sector */
		if (m_sector_count != 1)
			next_sector();

		/* signal an interrupt */
		if (--m_sectors_until_int == 0 || m_sector_count == 1)
		{
			m_sectors_until_int = ((m_command == IDE_COMMAND_READ_MULTIPLE) ? m_block_count : 1);
			set_irq(ASSERT_LINE);
		}

		/* if we're just verifying we can read the next sector */
		if (m_command == IDE_COMMAND_VERIFY_SECTORS ||
			m_command == IDE_COMMAND_VERIFY_SECTORS_NORETRY )
		{
			read_buffer_empty();
		}
		else
		{
			m_status |= IDE_STATUS_DRQ;

			if (m_command == IDE_COMMAND_READ_DMA)
				set_dmarq(ASSERT_LINE);
		}
	}

	/* if we got an error, we need to report it */
	else
	{
		/* set the error flag and the error */
		m_status |= IDE_STATUS_ERR;
		m_error = IDE_ERROR_BAD_SECTOR;

		/* signal an interrupt */
		set_irq(ASSERT_LINE);
	}
}


void ata_mass_storage_device::read_first_sector()
{
	if (m_master_password_enable || m_user_password_enable)
	{
		security_error();
	}
	else
	{
		set_dasp(ASSERT_LINE);

		start_busy(seek_time(), PARAM_COMMAND);
	}
}

/*************************************
 *
 *  Sector writing
 *
 *************************************/

void ata_mass_storage_device::process_buffer()
{
	if (m_command == IDE_COMMAND_SECURITY_UNLOCK)
	{
		if (m_user_password_enable && memcmp(m_buffer, m_user_password, 2 + 32) == 0)
		{
			LOGPRINT(("IDE Unlocked user password\n"));
			m_user_password_enable = 0;
		}
		if (m_master_password_enable && memcmp(m_buffer, m_master_password, 2 + 32) == 0)
		{
			LOGPRINT(("IDE Unlocked master password\n"));
			m_master_password_enable = 0;
		}
		if (PRINTF_IDE_PASSWORD)
		{
			int i;

			for (i = 0; i < 34; i += 2)
			{
				if (i % 8 == 2)
					mame_printf_debug("\n");

				mame_printf_debug("0x%02x, 0x%02x, ", m_buffer[i], m_buffer[i + 1]);
				//mame_printf_debug("0x%02x%02x, ", m_buffer[i], m_buffer[i + 1]);
			}
			mame_printf_debug("\n");
		}

		if (m_master_password_enable || m_user_password_enable)
			security_error();
	}
	else
	{
		set_dasp(ASSERT_LINE);

		if (m_command == IDE_COMMAND_WRITE_MULTIPLE)
		{
			if (m_sectors_until_int != 1)
			{
				/* ready to write now */
				finished_write();
			}
			else
			{
				/* set a timer to do the write */
				start_busy(TIME_PER_SECTOR, PARAM_COMMAND);
			}
		}
		else
		{
			/* set a timer to do the write */
			start_busy(TIME_PER_SECTOR, PARAM_COMMAND);
		}
	}
}


void ata_mass_storage_device::finished_write()
{
	int lba = lba_address(), count = 0;

	set_dasp(CLEAR_LINE);

	/* now do the write */
	count = write_sector(lba, m_buffer);

	/* if we succeeded, advance to the next sector and set the nice bits */
	if (count == 1)
	{
		/* advance the pointers, unless this is the last sector */
		/* Gauntlet: Dark Legacy checks to make sure we stop on the last sector */
		if (m_sector_count != 1)
			next_sector();

		/* signal an interrupt */
		if (--m_sectors_until_int == 0 || m_sector_count == 1)
		{
			m_sectors_until_int = ((m_command == IDE_COMMAND_WRITE_MULTIPLE) ? m_block_count : 1);
			set_irq(ASSERT_LINE);
		}

		/* signal an interrupt if there's more data needed */
		if (m_sector_count > 0)
			m_sector_count--;

		if (m_sector_count > 0)
		{
			m_status |= IDE_STATUS_DRQ;

			if (m_command == IDE_COMMAND_WRITE_DMA)
				set_dmarq(ASSERT_LINE);
		}
	}

	/* if we got an error, we need to report it */
	else
	{
		/* set the error flag and the error */
		m_status |= IDE_STATUS_ERR;
		m_error = IDE_ERROR_BAD_SECTOR;

		/* signal an interrupt */
		set_irq(ASSERT_LINE);
	}
}


/*************************************
 *
 *  Handle IDE commands
 *
 *************************************/

void ata_mass_storage_device::process_command()
{
	m_sectors_until_int = 0;
	m_buffer_size = IDE_DISK_SECTOR_SIZE;

	switch (m_command)
	{
	case IDE_COMMAND_READ_SECTORS:
	case IDE_COMMAND_READ_SECTORS_NORETRY:
		LOGPRINT(("IDE Read multiple: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		m_sectors_until_int = 1;

		/* start the read going */
		read_first_sector();
		break;

	case IDE_COMMAND_READ_MULTIPLE:
		LOGPRINT(("IDE Read multiple block: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		m_sectors_until_int = 1;

		/* start the read going */
		read_first_sector();
		break;

	case IDE_COMMAND_VERIFY_SECTORS:
	case IDE_COMMAND_VERIFY_SECTORS_NORETRY:
		LOGPRINT(("IDE Read verify multiple with/without retries: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		/* reset the buffer */
		m_sectors_until_int = m_sector_count;

		/* start the read going */
		read_first_sector();
		break;

	case IDE_COMMAND_READ_DMA:
		LOGPRINT(("IDE Read multiple DMA: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		/* reset the buffer */
		m_sectors_until_int = m_sector_count;

		/* start the read going */
		read_first_sector();
		break;

	case IDE_COMMAND_WRITE_SECTORS:
	case IDE_COMMAND_WRITE_SECTORS_NORETRY:
		LOGPRINT(("IDE Write multiple: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		/* reset the buffer */
		m_sectors_until_int = 1;

		/* mark the buffer ready */
		m_status |= IDE_STATUS_DRQ;
		break;

	case IDE_COMMAND_WRITE_MULTIPLE:
		LOGPRINT(("IDE Write multiple block: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		/* reset the buffer */
		m_sectors_until_int = 1;

		/* mark the buffer ready */
		m_status |= IDE_STATUS_DRQ;
		break;

	case IDE_COMMAND_WRITE_DMA:
		LOGPRINT(("IDE Write multiple DMA: C=%d H=%d S=%d LBA=%d count=%d\n",
			(m_cylinder_high << 8) | m_cylinder_low, m_device_head & IDE_DEVICE_HEAD_HS, m_sector_number, lba_address(), m_sector_count));

		/* reset the buffer */
		m_sectors_until_int = m_sector_count;

		/* mark the buffer ready */
		m_status |= IDE_STATUS_DRQ;

		/* start the read going */
		set_dmarq(ASSERT_LINE);
		break;

	case IDE_COMMAND_SECURITY_UNLOCK:
		LOGPRINT(("IDE Security Unlock\n"));

		/* mark the buffer ready */
		m_status |= IDE_STATUS_DRQ;

		set_irq(ASSERT_LINE);
		break;

	case IDE_COMMAND_IDENTIFY_DEVICE:
		LOGPRINT(("IDE Identify device\n"));

		start_busy(MINIMUM_COMMAND_TIME, PARAM_COMMAND);
		break;

	case IDE_COMMAND_RECALIBRATE:
		start_busy(MINIMUM_COMMAND_TIME, PARAM_COMMAND);
		break;

	case IDE_COMMAND_IDLE:
		/* signal an interrupt */
		set_irq(ASSERT_LINE);
		break;

	case IDE_COMMAND_SET_CONFIG:
		LOGPRINT(("IDE Set configuration (%d heads, %d sectors)\n", (m_device_head & IDE_DEVICE_HEAD_HS) + 1, m_sector_count));

		start_busy(MINIMUM_COMMAND_TIME, PARAM_COMMAND);
		break;

	case IDE_COMMAND_SET_MAX:
		LOGPRINT(("IDE Set max (%02X %02X %02X %02X %02X)\n", m_feature, m_sector_count & 0xff, m_sector_number, m_cylinder_low, m_cylinder_high));

		/* signal an interrupt */
		set_irq(ASSERT_LINE);
		break;

	case IDE_COMMAND_SET_BLOCK_COUNT:
		LOGPRINT(("IDE Set block count (%02X)\n", m_sector_count));

		m_block_count = m_sector_count;

		/* signal an interrupt */
		set_irq(ASSERT_LINE);
		break;

	case IDE_COMMAND_SEEK:
		/* signal an interrupt */
		set_irq(ASSERT_LINE);
		break;

	default:
		ata_hle_device::process_command();
		break;
	}
}

//**************************************************************************
//  IDE HARD DISK DEVICE
//**************************************************************************

// device type definition
const device_type IDE_HARDDISK = &device_creator<ide_hdd_device>;

//-------------------------------------------------
//  ide_hdd_device - constructor
//-------------------------------------------------

ide_hdd_device::ide_hdd_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: ata_mass_storage_device(mconfig, IDE_HARDDISK, "IDE Hard Disk", tag, owner, clock, "hdd", __FILE__),
	m_image(*this, "image")
{
}

ide_hdd_device::ide_hdd_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
	: ata_mass_storage_device(mconfig, type, name, tag, owner, clock, shortname, source),
	m_image(*this, "image")
{
}

void ide_hdd_device::device_start()
{
	ata_mass_storage_device::device_start();

	/* create a timer for timing status */
	m_last_status_timer = timer_alloc(TID_NULL);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void ide_hdd_device::device_reset()
{
	m_handle = m_image->get_chd_file();
	m_disk = m_image->get_hard_disk_file();

	if (m_disk != NULL && !m_can_identify_device)
	{
		const hard_disk_info *hdinfo = hard_disk_get_info(m_disk);
		if (hdinfo->sectorbytes == IDE_DISK_SECTOR_SIZE)
		{
			m_num_cylinders = hdinfo->cylinders;
			m_num_sectors = hdinfo->sectors;
			m_num_heads = hdinfo->heads;
			if (PRINTF_IDE_COMMANDS) mame_printf_debug("CHS: %d %d %d\n", m_num_cylinders, m_num_heads, m_num_sectors);
			mame_printf_debug("CHS: %d %d %d\n", m_num_cylinders, m_num_heads, m_num_sectors);
		}

		// build the features page
		UINT32 metalength;
		if (m_handle->read_metadata (HARD_DISK_IDENT_METADATA_TAG, 0, m_buffer, 512, metalength) == CHDERR_NONE)
		{
			for( int w = 0; w < 256; w++ )
			{
				m_identify_buffer[w] = (m_buffer[(w * 2) + 1] << 8) | m_buffer[w * 2];
			}
		}
		else
		{
			ide_build_identify_device();
		}

		m_can_identify_device = 1;
	}

	ata_mass_storage_device::device_reset();
}

UINT8 ide_hdd_device::calculate_status()
{
	UINT8 result = ata_hle_device::calculate_status();

	if (m_last_status_timer->elapsed() > TIME_PER_ROTATION)
	{
		result |= IDE_STATUS_IDX;
		m_last_status_timer->adjust(attotime::never);
	}

	return result;
}

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------
static MACHINE_CONFIG_FRAGMENT( hdd_image )
	MCFG_HARDDISK_ADD( "image" )
MACHINE_CONFIG_END

machine_config_constructor ide_hdd_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( hdd_image );
}
