/*
    psxcard.c - Sony PlayStation memory card device

    by pSXAuthor
    MESS conversion by R. Belmont
*/

#include "emu.h"
#include "psxcard.h"
#include "machine/psxcport.h"

//
//
//

//#define debug_card

//
//
//

const device_type PSXCARD = &device_creator<psxcard_device>;

enum transfer_states
{
	state_illegal=0,
	state_command,
	state_cmdack,
	state_wait,
	state_addr_hi,
	state_addr_lo,
	state_read,
	state_write,
	state_writeack_2,
	state_writechk,
	state_end
};

psxcard_device::psxcard_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, PSXCARD, "Sony PSX Memory Card", tag, owner, clock)
{
}

void psxcard_device::device_start()
{
	cache=new unsigned char [128*1024];

	memset(cache, 0, 128*1024);

	m_owner = dynamic_cast<psx_controller_port_device *>(owner());
	m_ack_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(psxcard_device::ack_timer), this));

	// save state registrations
/*  save_item(NAME(pkt));
    save_item(NAME(pkt_ptr));
    save_item(NAME(pkt_sz));
    save_item(NAME(pkt));
    save_item(NAME(cache));
    save_item(NAME(addr));
    save_item(NAME(state));*/
}

void psxcard_device::device_reset()
{
	state = state_illegal;
	addr = 0;

	m_bit = 0;
	m_count = 0;
	m_pad = false;
	m_idata = 0;

	m_clock = true;
	m_sel = true;
	m_rx = true;
	m_ack = true;
	m_owner->ack();
}

//
//
//

bool psxcard_device::transfer(UINT8 to, UINT8 *from)
{
	bool ret=true;

	switch (state)
	{
		case state_illegal:
			if (to == 0x81)
			{
//              printf("CARD: begin\n");
				state = state_command;
				*from = 0x00;
			}
			else
			{
				ret = false;
			}
			break;

		case state_command:
			cmd=to;
			*from=0x5a;
			state=state_cmdack;
			break;

		case state_cmdack:
			*from=0x5d;
			state=state_wait;
			break;

		case state_wait:
			*from=0x00;
			state=state_addr_hi;
			break;

		case state_addr_hi:
			addr=(to<<8);
//          printf("addr_hi: %02x, addr = %x\n", to, addr);
			*from=to;
			state=state_addr_lo;
			break;

		case state_addr_lo:
			addr|=(to&0xff);
//          printf("addr_lo: %02x, addr = %x, cmd = %x\n", to, addr, cmd);

			switch (cmd)
			{
				case 'R':   // 0x52
				{
					pkt[0]=*from=0x5c;
					pkt[1]=0x5d;
					pkt[2]=(addr>>8);
					pkt[3]=(addr&0xff);
					read_card(addr,&pkt[4]);
					pkt[4+128]=checksum_data(&pkt[2],128+2);
					pkt[5+128]=0x47;
					pkt_sz=6+128;
					pkt_ptr=1;
					state=state_read;
					break;
				}
				case 'W':   // 0x57
				{
					pkt[0]=addr>>8;
					pkt[1]=addr&0xff;
					pkt_sz=129+2;
					pkt_ptr=2;
					state=state_write;
					*from=to;
					break;
				}
				default:
					state=state_illegal;
					break;
			}
			break;

		case state_read:
			//assert(to==0);
//          printf("state_read: pkt_ptr = %d, pkt_sz = %d\n", pkt_ptr, pkt_sz);
			*from=pkt[pkt_ptr++];
			if (pkt_ptr==pkt_sz)
			{
				#ifdef debug_card
					printf("card: read finished\n");
				#endif

				state=state_end;
			}
			break;

		case state_write:
			*from=to;
			pkt[pkt_ptr++]=to;
			if (pkt_ptr==pkt_sz)
			{
				*from=0x5c;
				state=state_writeack_2;
			}
			break;

		case state_writeack_2:
			*from=0x5d;
			state=state_writechk;
			break;

		case state_writechk:
		{
			unsigned char chk=checksum_data(pkt,128+2);
			if (chk==pkt[128+2])
			{
				#ifdef debug_card
					printf("card: write ok\n");
				#endif

				write_card(addr,pkt+2);

				*from='G';
			} else
			{
				#ifdef debug_card
					printf("card: write fail\n");
				#endif

				*from='N';
			}
			state=state_end;
			break;
		}

		case state_end:
			ret = false;
			state = state_illegal;
			break;

		default: /*assert(0);*/ ret=false; break;
	}

	#ifdef debug_card
//      printf("card: transfer to=%02x from=%02x ret=%c\n",to,*from,ret ? 'T' : 'F');
	#endif

	return ret;
}

void psxcard_device::read_card(const unsigned short addr, unsigned char *buf)
{
	#ifdef debug_card
		printf("card: read block %d\n",addr);
	#endif

	if (addr<1024)
	{
		memcpy(buf,cache+(addr*128),128);
	} else
	{
		memset(buf,0,128);
	}
}

//
//
//

void psxcard_device::write_card(const unsigned short addr, unsigned char *buf)
{
	#ifdef debug_card
		printf("card: write block %d\n",addr);
	#endif

	if (addr<1024)
	{
		memcpy(cache+(addr*128),buf,128);
	}
}

//
//
//

unsigned char psxcard_device::checksum_data(const unsigned char *buf, const unsigned int sz)
{
	unsigned char chk=*buf++;
	int left=sz;
	while (--left) chk^=*buf++;
	return chk;
}

void psxcard_device::do_card()
{
	if(!m_bit)
	{
		m_idata = 0;
		if(!m_count)
			m_odata = 0xff;
	}

	m_rx = (m_odata & (1 << m_bit)) ? true : false;
	m_idata |= (m_owner->tx_r()?1:0) << m_bit;
	m_bit = (m_bit + 1) % 8;

	if(!m_bit)
	{
		if((!m_count) && !(m_idata & 0x80))
		{
			m_pad = true;
			return;
		}

		if(transfer(m_idata, &m_odata))
		{
			m_count++;
			m_ack_timer->adjust(attotime::from_usec(10), 0);
		}
		else
			m_count = 0;
	}
}

void psxcard_device::ack_timer(void *ptr, int param)
{
	m_ack = param;
	m_owner->ack();

	if(!param)
		m_ack_timer->adjust(attotime::from_usec(2), 1);
}

void psxcard_device::sel_w(bool state)
{
	if(state && !m_sel)
		reset();
	m_sel = state;
}

