#include "emu.h"
#include "includes/pgm.h"



/*** ASIC27a (Puzzle Star) -- ARM but with no external rom? behaves a bit like KOV ***/


/*** (pstarS) ***/

static const int pstar_ba[0x1E]={
	0x02,0x00,0x00,0x01,0x00,0x03,0x00,0x00, //0
	0x02,0x00,0x06,0x00,0x22,0x04,0x00,0x03, //8
	0x00,0x00,0x06,0x00,0x20,0x07,0x00,0x03, //10
	0x00,0x21,0x01,0x00,0x00,0x63
};

static const int pstar_b0[0x10]={
	0x09,0x0A,0x0B,0x00,0x01,0x02,0x03,0x04,
	0x05,0x06,0x07,0x08,0x00,0x00,0x00,0x00
};

static const int pstar_ae[0x10]={
	0x5D,0x86,0x8C ,0x8B,0xE0,0x8B,0x62,0xAF,
	0xB6,0xAF,0x10A,0xAF,0x00,0x00,0x00,0x00
};

static const int pstar_a0[0x10]={
	0x02,0x03,0x04,0x05,0x06,0x01,0x0A,0x0B,
	0x0C,0x0D,0x0E,0x09,0x00,0x00,0x00,0x00,
};

static const int pstar_9d[0x10]={
	0x05,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static const int pstar_90[0x10]={
	0x0C,0x10,0x0E,0x0C,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const int pstar_8c[0x23]={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,
	0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x04,
	0x03,0x03,0x03
};

static const int pstar_80[0x1a3]={
	0x03,0x03,0x04,0x04,0x04,0x04,0x05,0x05,
	0x05,0x05,0x06,0x06,0x03,0x03,0x04,0x04,
	0x05,0x05,0x05,0x05,0x06,0x06,0x07,0x07,
	0x03,0x03,0x04,0x04,0x05,0x05,0x05,0x05,
	0x06,0x06,0x07,0x07,0x06,0x06,0x06,0x06,
	0x06,0x06,0x06,0x07,0x07,0x07,0x07,0x07,
	0x06,0x06,0x06,0x06,0x06,0x06,0x07,0x07,
	0x07,0x07,0x08,0x08,0x05,0x05,0x05,0x05,
	0x05,0x05,0x05,0x06,0x06,0x06,0x07,0x07,
	0x06,0x06,0x06,0x07,0x07,0x07,0x08,0x08,
	0x09,0x09,0x09,0x09,0x07,0x07,0x07,0x07,
	0x07,0x08,0x08,0x08,0x08,0x09,0x09,0x09,
	0x06,0x06,0x07,0x07,0x07,0x08,0x08,0x08,
	0x08,0x08,0x09,0x09,0x05,0x05,0x06,0x06,
	0x06,0x07,0x07,0x08,0x08,0x08,0x08,0x09,
	0x07,0x07,0x07,0x07,0x07,0x08,0x08,0x08,
	0x08,0x09,0x09,0x09,0x06,0x06,0x07,0x03,
	0x07,0x06,0x07,0x07,0x08,0x07,0x05,0x04,
	0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,
	0x06,0x06,0x06,0x06,0x03,0x04,0x04,0x04,
	0x04,0x05,0x05,0x06,0x06,0x06,0x06,0x07,
	0x04,0x04,0x05,0x05,0x06,0x06,0x06,0x06,
	0x06,0x07,0x07,0x08,0x05,0x05,0x06,0x07,
	0x07,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
	0x05,0x05,0x05,0x07,0x07,0x07,0x07,0x07,
	0x07,0x08,0x08,0x08,0x08,0x08,0x09,0x09,
	0x09,0x09,0x03,0x04,0x04,0x05,0x05,0x05,
	0x06,0x06,0x07,0x07,0x07,0x07,0x08,0x08,
	0x08,0x09,0x09,0x09,0x03,0x04,0x05,0x05,
	0x04,0x03,0x04,0x04,0x04,0x05,0x05,0x04,
	0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
	0x03,0x03,0x03,0x04,0x04,0x04,0x04,0x04,
	0x04,0x04,0x04,0x04,0x04,0x03,0x03,0x03,
	0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,
	0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00
};




READ16_HANDLER( pstars_protram_r )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();

	if (offset == 4)		//region
		return input_port_read(space->machine(), "Region");
	else if (offset >= 0x10)  //timer
	{
		logerror("PSTARS ACCESS COUNTER %6X\n", state->m_pstar_ram[offset - 0x10]);
		return state->m_pstar_ram[offset - 0x10]--;
	}
	return 0x0000;
}

READ16_HANDLER( pstars_r )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();

	if (offset == 0)
	{
		UINT16 d = state->m_pstars_val & 0xffff;
		UINT16 realkey = state->m_pstars_key >> 8;
		realkey |= state->m_pstars_key;
		d ^= realkey;
//      logerror("PSTARS A27 R  %6X\n", state->m_pstars_val);
		return d;
	}
	else if (offset == 1)
	{
		UINT16 d = state->m_pstars_val >> 16;
		UINT16 realkey = state->m_pstars_key >> 8;
		realkey |= state->m_pstars_key;
		d ^= realkey;
		return d;

	}
	return 0xff;
}

WRITE16_HANDLER( pstars_w )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();

	if (offset == 0)
	{
		state->m_pstars_int[0] = data;
		return;
	}

	if (offset == 1)
	{
		UINT16 realkey;
		if ((data >> 8) == 0xff)
			state->m_pstars_key = 0xff00;
		realkey = state->m_pstars_key >> 8;
		realkey |= state->m_pstars_key;
		{
			state->m_pstars_key += 0x100;
			state->m_pstars_key &= 0xff00;
			if (state->m_pstars_key == 0xff00)
				state->m_pstars_key = 0x100;
		}
		data ^= realkey;
		state->m_pstars_int[1] = data;
		state->m_pstars_int[0] ^= realkey;

		switch (state->m_pstars_int[1] & 0xff)
		{
		case 0x99:
			state->m_pstars_key = 0x100;
			state->m_pstars_val = 0x880000;
			break;

		case 0xe0:
			state->m_pstars_val = 0xa00000 + (state->m_pstars_int[0] << 6);
			break;

		case 0xdc:
			state->m_pstars_val = 0xa00800 + (state->m_pstars_int[0] << 6);
			break;

		case 0xd0:
			state->m_pstars_val = 0xa01000 + (state->m_pstars_int[0] << 5);
			break;

		case 0xb1:
			state->m_pstar_b1 = state->m_pstars_int[0];
			state->m_pstars_val = 0x890000;
			break;

		case 0xbf:
			state->m_pstars_val = state->m_pstar_b1 * state->m_pstars_int[0];
			break;

		case 0xc1: //TODO:TIMER  0,1,2,FIX TO 0 should be OK?
			state->m_pstars_val = 0;
			break;

		case 0xce: //TODO:TIMER  0,1,2
			state->m_pstar_ce = state->m_pstars_int[0];
			state->m_pstars_val=0x890000;
			break;

		case 0xcf: //TODO:TIMER  0,1,2
			state->m_pstar_ram[state->m_pstar_ce] = state->m_pstars_int[0];
			state->m_pstars_val = 0x890000;
			break;

		case 0xe7:
			state->m_pstar_e7 = (state->m_pstars_int[0] >> 12) & 0xf;
			state->m_pstars_regs[state->m_pstar_e7] &= 0xffff;
			state->m_pstars_regs[state->m_pstar_e7] |= (state->m_pstars_int[0] & 0xff) << 16;
			state->m_pstars_val = 0x890000;
			break;

		case 0xe5:
			state->m_pstars_regs[state->m_pstar_e7] &= 0xff0000;
			state->m_pstars_regs[state->m_pstar_e7] |= state->m_pstars_int[0];
			state->m_pstars_val = 0x890000;
			break;

		case 0xf8: //@73C
			state->m_pstars_val = state->m_pstars_regs[state->m_pstars_int[0] & 0xf] & 0xffffff;
			break;

		case 0xba:
			state->m_pstars_val = pstar_ba[state->m_pstars_int[0]];
			break;

		case 0xb0:
			state->m_pstars_val = pstar_b0[state->m_pstars_int[0]];
			break;

		case 0xae:
			state->m_pstars_val = pstar_ae[state->m_pstars_int[0]];
			break;

		case 0xa0:
			state->m_pstars_val = pstar_a0[state->m_pstars_int[0]];
			break;

		case 0x9d:
			state->m_pstars_val = pstar_9d[state->m_pstars_int[0]];
			break;

		case 0x90:
			state->m_pstars_val = pstar_90[state->m_pstars_int[0]];
			break;

		case 0x8c:
			state->m_pstars_val = pstar_8c[state->m_pstars_int[0]];
			break;

		case 0x80:
			state->m_pstars_val = pstar_80[state->m_pstars_int[0]];
			break;

		default:
			state->m_pstars_val = 0x890000;
			logerror("PSTARS PC(%06x) UNKNOWN %4X %4X\n", cpu_get_pc(&space->device()), state->m_pstars_int[1], state->m_pstars_int[0]);

		}

	}
}

/*** ASIC 3 (oriental legends protection) ****************************************/

static void asic3_compute_hold(running_machine &machine)
{
	pgm_state *state = machine.driver_data<pgm_state>();

	// The mode is dependent on the region
	static const int modes[4] = { 1, 1, 3, 2 };
	int mode = modes[input_port_read(machine, "Region") & 3];

	switch (mode)
	{
	case 1:
		state->m_asic3_hold =
			(state->m_asic3_hold << 1)
			 ^ 0x2bad
			 ^ BIT(state->m_asic3_hold, 15) ^ BIT(state->m_asic3_hold, 10) ^ BIT(state->m_asic3_hold, 8) ^ BIT(state->m_asic3_hold, 5)
			 ^ BIT(state->m_asic3_z, state->m_asic3_y)
			 ^ (BIT(state->m_asic3_x, 0) << 1) ^ (BIT(state->m_asic3_x, 1) << 6) ^ (BIT(state->m_asic3_x, 2) << 10) ^ (BIT(state->m_asic3_x, 3) << 14);
		break;
	case 2:
		state->m_asic3_hold =
			(state->m_asic3_hold << 1)
			 ^ 0x2bad
			 ^ BIT(state->m_asic3_hold, 15) ^ BIT(state->m_asic3_hold, 7) ^ BIT(state->m_asic3_hold, 6) ^ BIT(state->m_asic3_hold, 5)
			 ^ BIT(state->m_asic3_z, state->m_asic3_y)
			 ^ (BIT(state->m_asic3_x, 0) << 4) ^ (BIT(state->m_asic3_x, 1) << 6) ^ (BIT(state->m_asic3_x, 2) << 10) ^ (BIT(state->m_asic3_x, 3) << 12);
		break;
	case 3:
		state->m_asic3_hold =
			(state->m_asic3_hold << 1)
			 ^ 0x2bad
			 ^ BIT(state->m_asic3_hold, 15) ^ BIT(state->m_asic3_hold, 10) ^ BIT(state->m_asic3_hold, 8) ^ BIT(state->m_asic3_hold, 5)
			 ^ BIT(state->m_asic3_z, state->m_asic3_y)
			 ^ (BIT(state->m_asic3_x, 0) << 4) ^ (BIT(state->m_asic3_x, 1) << 6) ^ (BIT(state->m_asic3_x, 2) << 10) ^ (BIT(state->m_asic3_x, 3) << 12);
		break;
	}
}

READ16_HANDLER( pgm_asic3_r )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();
	UINT8 res = 0;
	/* region is supplied by the protection device */

	switch (state->m_asic3_reg)
	{
	case 0x00: res = (state->m_asic3_latch[0] & 0xf7) | ((input_port_read(space->machine(), "Region") << 3) & 0x08); break;
	case 0x01: res = state->m_asic3_latch[1]; break;
	case 0x02: res = (state->m_asic3_latch[2] & 0x7f) | ((input_port_read(space->machine(), "Region") << 6) & 0x80); break;
	case 0x03:
		res = (BIT(state->m_asic3_hold, 15) << 0)
			| (BIT(state->m_asic3_hold, 12) << 1)
			| (BIT(state->m_asic3_hold, 13) << 2)
			| (BIT(state->m_asic3_hold, 10) << 3)
			| (BIT(state->m_asic3_hold, 7) << 4)
			| (BIT(state->m_asic3_hold, 9) << 5)
			| (BIT(state->m_asic3_hold, 2) << 6)
			| (BIT(state->m_asic3_hold, 5) << 7);
		break;
	case 0x20: res = 0x49; break;
	case 0x21: res = 0x47; break;
	case 0x22: res = 0x53; break;
	case 0x24: res = 0x41; break;
	case 0x25: res = 0x41; break;
	case 0x26: res = 0x7f; break;
	case 0x27: res = 0x41; break;
	case 0x28: res = 0x41; break;
	case 0x2a: res = 0x3e; break;
	case 0x2b: res = 0x41; break;
	case 0x2c: res = 0x49; break;
	case 0x2d: res = 0xf9; break;
	case 0x2e: res = 0x0a; break;
	case 0x30: res = 0x26; break;
	case 0x31: res = 0x49; break;
	case 0x32: res = 0x49; break;
	case 0x33: res = 0x49; break;
	case 0x34: res = 0x32; break;
	}

	return res;
}

WRITE16_HANDLER( pgm_asic3_w )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();

	if(ACCESSING_BITS_0_7)
	{
		if (state->m_asic3_reg < 3)
			state->m_asic3_latch[state->m_asic3_reg] = data << 1;
		else if (state->m_asic3_reg == 0xa0)
			state->m_asic3_hold = 0;
		else if (state->m_asic3_reg == 0x40)
		{
			state->m_asic3_h2 = state->m_asic3_h1;
			state->m_asic3_h1 = data;
		}
		else if (state->m_asic3_reg == 0x48)
		{
			state->m_asic3_x = 0;
			if (!(state->m_asic3_h2 & 0x0a))
				state->m_asic3_x |= 8;
			if (!(state->m_asic3_h2 & 0x90))
				state->m_asic3_x |= 4;
			if (!(state->m_asic3_h1 & 0x06))
				state->m_asic3_x |= 2;
			if (!(state->m_asic3_h1 & 0x90))
				state->m_asic3_x |= 1;
		}
		else if(state->m_asic3_reg >= 0x80 && state->m_asic3_reg <= 0x87)
		{
			state->m_asic3_y = state->m_asic3_reg & 7;
			state->m_asic3_z = data;
			asic3_compute_hold(space->machine());
		}
	}
}

WRITE16_HANDLER( pgm_asic3_reg_w )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();

	if(ACCESSING_BITS_0_7)
		state->m_asic3_reg = data & 0xff;
}

/*** Knights of Valour / Sango / PhotoY2k Protection (from ElSemi) (ASIC28) ***/

static const UINT32 B0TABLE[16] = {2, 0, 1, 4, 3}; //maps char portraits to tables

// photo2yk bonus stage
static const UINT32 AETABLE[16]={0x00,0x0a,0x14,
		0x01,0x0b,0x15,
		0x02,0x0c,0x16
};

//Not sure if BATABLE is complete
static const UINT32 BATABLE[0x40]= {
     0x00,0x29,0x2c,0x35,0x3a,0x41,0x4a,0x4e,  //0x00
     0x57,0x5e,0x77,0x79,0x7a,0x7b,0x7c,0x7d, //0x08
     0x7e,0x7f,0x80,0x81,0x82,0x85,0x86,0x87, //0x10
     0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x90,  //0x18
     0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,
     0x9e,0xa3,0xd4,0xa9,0xaf,0xb5,0xbb,0xc1
};



READ16_HANDLER( sango_protram_r )
{
	// at offset == 4 is region (supplied by device)
	// 0 = china
	// 1 = taiwan
	// 2 = japan
	// 3 = korea
	// 4 = hong kong
	// 5 = world

	if (offset == 4)
		return input_port_read(space->machine(), "Region");

	// otherwise it doesn't seem to use the ram for anything important, we return 0 to avoid test mode corruption
	// kovplus reads from offset 000e a lot ... why?
#ifdef MAME_DEBUG
	popmessage ("protection ram r %04x",offset);
#endif
	return 0x0000;
}

#define BITSWAP10(val,B9,B8,B7,B6,B5,B4,B3,B2,B1,B0) \
                ((BIT(val, B9) <<  9) | \
                 (BIT(val, B8) <<  8) | \
                 (BIT(val, B7) <<  7) | \
                 (BIT(val, B6) <<  6) | \
                 (BIT(val, B5) <<  5) | \
                 (BIT(val, B4) <<  4) | \
                 (BIT(val, B3) <<  3) | \
                 (BIT(val, B2) <<  2) | \
                 (BIT(val, B1) <<  1) | \
                 (BIT(val, B0) <<  0))


READ16_HANDLER( asic28_r )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();
	UINT32 val = (state->m_asic28_regs[1] << 16) | (state->m_asic28_regs[0]);

	//logerror("Asic28 Read PC = %06x Command = %02x ??\n", cpu_get_pc(&space->device()), state->m_asic28_regs[1]);

	switch (state->m_asic28_regs[1] & 0xff)
	{
		case 0x99:
			val = 0x880000;
			break;

		case 0x9d:	// spr palette
			val = 0xa00000 + ((state->m_asic28_regs[0] & 0x1f) << 6);
			break;

		case 0xb0:
			val = B0TABLE[state->m_asic28_regs[0] & 0xf];
			break;

		case 0xb4:
			{
				//UINT16 tmp = state->m_eoregs[v2];
				int v2 = state->m_asic28_regs[0] & 0x0f;
				int v1 = (state->m_asic28_regs[0] & 0x0f00) >> 8;
				//state->m_eoregs[v2] = state->m_eoregs[v1];
				//state->m_eoregs[v1] = tmp;
				if (state->m_asic28_regs[0] == 0x102)
					state->m_eoregs[1] = state->m_eoregs[0];
				else
					state->m_eoregs[v1] = state->m_eoregs[v2];

				val = 0x880000;
			}
			break;

		case 0xba:
			val = BATABLE[state->m_asic28_regs[0] & 0x3f];
			if (state->m_asic28_regs[0] > 0x2f)
				popmessage("Unmapped BA com %02x, contact ElSemi / MameDev", state->m_asic28_regs[0]);
			break;

		case 0xc0:
			val = 0x880000;
			break;

		case 0xc3:	//TXT tile position Uses C0 to select column
			val = 0x904000 + (state->m_asic_params[0xc0] + state->m_asic_params[0xc3] * 64) * 4;
			break;

		case 0xcb:
			val = 0x880000;
			break;

		case 0xcc: //BG
			{
			int y = state->m_asic_params[0xcc];
			if (y & 0x400)    //y is signed (probably x too and it also applies to TXT, but I've never seen it used)
				y -= (0x400 - (y & 0x3ff));
			val = 0x900000 + (((state->m_asic_params[0xcb] + (y) * 64) * 4) /*&0x1fff*/);
			}
			break;

		case 0xd0:	//txt palette
			val = 0xa01000 + (state->m_asic28_regs[0] << 5);
			break;

		case 0xd6:	//???? check it
			{
				int v2 = state->m_asic28_regs[0] & 0xf;
				//int v1 = (state->m_asic28_regs[0] & 0xf0) >> 4;
				state->m_eoregs[0] = state->m_eoregs[v2];
				//state->m_eoregs[v2] = 0;
				val = 0x880000;
			}
			break;

		case 0xdc:	//bg palette
			val = 0xa00800 + (state->m_asic28_regs[0] << 6);
			break;

		case 0xe0:	//spr palette
			val = 0xa00000 + ((state->m_asic28_regs[0] & 0x1f) << 6);
			break;

		case 0xe5:
			val = 0x880000;
			break;

		case 0xe7:
			val = 0x880000;
			break;

		case 0xf0:
			val = 0x00C000;
			break;

		case 0xf8:
			val = state->m_eoregs[state->m_asic28_regs[0] & 0xf] & 0xffffff;
			break;

		case 0xfc:	//Adjust damage level to char experience level
			{
			val = (state->m_asic_params[0xfc] * state->m_asic_params[0xfe]) >> 6;
			break;
			}

		case 0xfe:	//todo
			val = 0x880000;
			break;


		default:
			val = 0x880000;
	}

	if(offset == 0)
	{
		UINT16 d = val & 0xffff;
		UINT16 realkey = state->m_asic28_key >> 8;
		realkey |= state->m_asic28_key;
		d ^= realkey;
		return d;
	}
	else if (offset == 1)
	{
		UINT16 d = val >> 16;
		UINT16 realkey = state->m_asic28_key >> 8;
		realkey |= state->m_asic28_key;
		d ^= realkey;
		state->m_asic28_rcnt++;
		if (!(state->m_asic28_rcnt & 0xf))
		{
			state->m_asic28_key += 0x100;
			state->m_asic28_key &= 0xff00;
		}
		return d;
	}
	return 0xff;
}

WRITE16_HANDLER( asic28_w )
{
	pgm_state *state = space->machine().driver_data<pgm_state>();

	if (offset == 0)
	{
		UINT16 realkey =state->m_asic28_key >> 8;
		realkey |= state->m_asic28_key;
		data ^= realkey;
		state->m_asic28_regs[0] = data;
		return;
	}
	if (offset == 1)
	{
		UINT16 realkey;

		state->m_asic28_key = data & 0xff00;

		realkey = state->m_asic28_key >> 8;
		realkey |= state->m_asic28_key;
		data ^= realkey;
		state->m_asic28_regs[1] = data;
		logerror("ASIC28 CMD %04x  PARAM %04x\n", state->m_asic28_regs[1], state->m_asic28_regs[0]);

		state->m_asic_params[state->m_asic28_regs[1] & 0xff] = state->m_asic28_regs[0];
		if (state->m_asic28_regs[1] == 0xE7)
		{
			UINT32 E0R = (state->m_asic_params[0xe7] >> 12) & 0xf;
			state->m_eoregs[E0R] &= 0xffff;
			state->m_eoregs[E0R] |= state->m_asic28_regs[0] << 16;
		}
		if (state->m_asic28_regs[1]==0xE5)
		{
			UINT32 E0R = (state->m_asic_params[0xe7] >> 12) & 0xf;
			state->m_eoregs[E0R] &= 0xff0000;
			state->m_eoregs[E0R] |= state->m_asic28_regs[0];
		}
		state->m_asic28_rcnt = 0;
	}
}

/* Dragon World 2 */

#define DW2BITSWAP(s,d,bs,bd)  d=((d&(~(1<<bd)))|(((s>>bs)&1)<<bd))
//Use this handler for reading from 0xd80000-0xd80002
READ16_HANDLER( dw2_d80000_r )
{
// addr&=0xff;
// if(dw2reg<0x20) //NOT SURE!!
	{
		//The value at 0x80EECE is computed in the routine at 0x107c18
		UINT16 d = pgm_mainram[0xEECE/2];
		UINT16 d2 = 0;
		d = (d >> 8) | (d << 8);
		DW2BITSWAP(d, d2, 7,  0);
		DW2BITSWAP(d, d2, 4,  1);
		DW2BITSWAP(d, d2, 5,  2);
		DW2BITSWAP(d, d2, 2,  3);
		DW2BITSWAP(d, d2, 15, 4);
		DW2BITSWAP(d, d2, 1,  5);
		DW2BITSWAP(d, d2, 10, 6);
		DW2BITSWAP(d, d2, 13, 7);
		// ... missing bitswaps here (8-15) there is not enough data to know them
		// the code only checks the lowest 8 bytes
		return d2;
	}
}

/* Dragon World 3

Dragon World 3 has 2 protection chips
ASIC022 and ASIC025
one of them also has an external data rom (encrypted?)

code below is ElSemi's preliminary code, it doesn't work properly and isn't used, much of the protection isn't understood */

#if 0
AddWriteArea(0xda0000,0xdaffff,0,dw3_w8,dw3_w16,dw3_w32);
AddReadArea (0xda0000,0xdaffff,0,dw3_r8,dw3_r16,dw3_r32);

#define DW3BITSWAP(s,d,bs,bd)  d=((d&(~(1<<bd)))|(((s>>bs)&1)<<bd))

UINT16 dw3_Rw[8];
UINT8 *dw3_R=(UINT8 *) dw3_Rw;

UINT8 dw3_r8(UINT32 addr)
{
	if(addr>=0xDA5610 && addr<=0xDA5613)
		return *((UINT8 *) (dw3_R+((addr-0xDA5610)^1)));
	return 0;
}

UINT16 dw3_r16(UINT32 addr)
{
	if(addr>=0xDA5610 && addr<=0xDA5613)
		return *((UINT16 *) (dw3_R+(addr-0xDA5610)));
	return 0;
}

UINT32 dw3_r32(UINT32 addr)
{
	return 0;
}

void dw3_w8(UINT32 addr,UINT8 val)
{
	if(addr==0xDA5610)
		dw3_R[1]=val;
	if(addr==0xDA5611)
		dw3_R[0]=val;
	if(addr==0xDA5612)
		dw3_R[3]=val;
	if(addr==0xDA5613)
		dw3_R[2]=val;
}

void dw3_w16(UINT32 addr,UINT16 val)
{
	if(addr>=0xDA5610 && addr<=0xDA5613)
	{
		UINT16 *s=((UINT16 *) (dw3_R+(addr-0xDA5610)));
		*s=val;
		if(addr==0xDA5610)
		{
			if(val==1)
			{
				UINT16 v1=dw3_Rw[1];
				UINT16 v2=0;
				DW3BITSWAP(v1,v2,0,0);
				DW3BITSWAP(v1,v2,1,1);
				DW3BITSWAP(v1,v2,7,2);
				DW3BITSWAP(v1,v2,6,3);
				DW3BITSWAP(v1,v2,5,4);
				DW3BITSWAP(v1,v2,4,5);
				DW3BITSWAP(v1,v2,3,6);
				DW3BITSWAP(v1,v2,2,7);

				dw3_Rw[1]=v2;
			}
		}
	}

}


void dw3_w32(UINT32 addr,UINT32 val)
{

}
#endif

/* Oriental Legend Super Plus ARM simulation */

static const int oldsplus_80[0x5]={
	0xbb8,0x1770,0x2328,0x2ee0,0xf4240
};

static const int oldsplus_fc[0x20]={
	0x00,0x00,0x0a,0x3a,0x4e,0x2e,0x03,0x40,
	0x33,0x43,0x26,0x2c,0x00,0x00,0x00,0x00,
	0x00,0x00,0x44,0x4d,0xb,0x27,0x3d,0x0f,
	0x37,0x2b,0x02,0x2f,0x15,0x45,0x0e,0x30
};

static const int oldsplus_a0[0x20]={
	0x000,0x023,0x046,0x069,0x08c,0x0af,0x0d2,0x0f5,
	0x118,0x13b,0x15e,0x181,0x1a4,0x1c7,0x1ea,0x20d,
	0x20d,0x20d,0x20d,0x20d,0x20d,0x20d,0x20d,0x20d,
	0x20d,0x20d,0x20d,0x20d,0x20d,0x20d,0x20d,0x20d,
};

static const int oldsplus_90[0x7]={
	0x50,0xa0,0xc8,0xf0,0x190,0x1f4,0x258
};

static const int oldsplus_5e[0x20]={
	0x04,0x04,0x04,0x04,0x04,0x03,0x03,0x03,
	0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const int oldsplus_b0[0xe0]={
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,
	0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
	0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,

	0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,
	0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
	0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,
	0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,

	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
	0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
	0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,

	0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,
	0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,
	0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,
	0x1c,0x1d,0x1e,0x1f,0x1f,0x1f,0x1f,0x1f
};

static const int oldsplus_ae[0xe0]={
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,

	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,
	0x50,0x50,0x50,0x50,0x50,0x50,0x50,0x50,

	0x1E,0x1F,0x20,0x21,0x22,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,

	0x1F,0x20,0x21,0x22,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,

	0x20,0x21,0x22,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,

	0x21,0x22,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,

	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23,
	0x23,0x23,0x23,0x23,0x23,0x23,0x23,0x23
};

static const int oldsplus_ba[0x4]={
	0x3138,0x2328,0x1C20,0x1518
};

static const int oldsplus_9d[0x111]={
	0x0000,0x0064,0x00c8,0x012c,0x0190,0x01f4,0x0258,0x02bc,
	0x02f8,0x0334,0x0370,0x03ac,0x03e8,0x0424,0x0460,0x049c,
	0x04d8,0x0514,0x0550,0x058c,0x05c8,0x0604,0x0640,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x0000,
	0x0064,0x00c8,0x012c,0x0190,0x01f4,0x0258,0x02bc,0x0302,
	0x0348,0x038e,0x03d4,0x041a,0x0460,0x04a6,0x04ec,0x0532,
	0x0578,0x05be,0x0604,0x064a,0x0690,0x06d6,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x0000,0x0064,
	0x00c8,0x012c,0x0190,0x01f4,0x0258,0x02bc,0x0316,0x0370,
	0x03ca,0x0424,0x047e,0x04d8,0x0532,0x058c,0x05e6,0x0640,
	0x069a,0x06f4,0x074e,0x07a8,0x0802,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x0000,0x0064,0x00c8,
	0x012c,0x0190,0x01f4,0x0258,0x02bc,0x032a,0x0398,0x0406,
	0x0474,0x04e2,0x0550,0x05be,0x062c,0x069a,0x0708,0x0776,
	0x07e4,0x0852,0x08c0,0x092e,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x0000,0x0064,0x00c8,0x012c,
	0x0190,0x01f4,0x0258,0x02bc,0x0348,0x03d4,0x0460,0x04ec,
	0x0578,0x0604,0x0690,0x071c,0x07a8,0x0834,0x08c0,0x094c,
	0x09d8,0x0a64,0x0af0,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x0000,0x0064,0x00c8,0x012c,0x0190,
	0x01f4,0x0258,0x02bc,0x0384,0x044c,0x0514,0x05dc,0x06a4,
	0x076c,0x0834,0x08fc,0x09c4,0x0a8c,0x0b54,0x0c1c,0x0ce4,
	0x0dac,0x0e74,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x0000,0x0064,0x00c8,0x012c,0x0190,0x01f4,
	0x0258,0x02bc,0x030c,0x035c,0x03ac,0x03fc,0x044c,0x049c,
	0x04ec,0x053c,0x058c,0x05dc,0x062c,0x067c,0x06cc,0x071c,
	0x076c,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,0x06bc,
	0x06bc
};

static const int oldsplus_8c[0x20]={
	0x0032,0x0032,0x0064,0x0096,0x0096,0x00fa,0x012c,0x015e,
	0x0032,0x0064,0x0096,0x00c8,0x00c8,0x012c,0x015e,0x0190,
	0x0064,0x0096,0x00c8,0x00fa,0x00fa,0x015e,0x0190,0x01c2,
	0x0096,0x00c8,0x00fa,0x012c,0x012c,0x0190,0x01c2,0x01f4
};

READ16_HANDLER( oldsplus_protram_r )
{
	if (offset == 4)
		return input_port_read(space->machine(), "Region");

	return 0x0000;
}

READ16_HANDLER( oldsplus_r )
{
	oldsplus_state *state = space->machine().driver_data<oldsplus_state>();

	if (offset == 0)
	{
		UINT16 d = state->m_oldsplus_val & 0xffff;
		UINT16 realkey = state->m_oldsplus_key >> 8;
		realkey |= state->m_oldsplus_key;
		d ^= realkey;
		return d;
	}
	else if (offset == 1)
	{
		UINT16 d = state->m_oldsplus_val >> 16;
		UINT16 realkey = state->m_oldsplus_key >> 8;
		realkey |= state->m_oldsplus_key;
		d ^= realkey;
		return d;

	}
	return 0xff;
}

WRITE16_HANDLER( oldsplus_w )
{
	oldsplus_state *state = space->machine().driver_data<oldsplus_state>();

	if (offset == 0)
	{
		state->m_oldsplus_int[0] = data;
		return;
	}

	if (offset == 1)
	{
		UINT16 realkey;
		if ((data >> 8) == 0xff)
			state->m_oldsplus_key = 0xff00;
		realkey = state->m_oldsplus_key >> 8;
		realkey |= state->m_oldsplus_key;
		{
			state->m_oldsplus_key += 0x100;
			state->m_oldsplus_key &= 0xff00;
			if (state->m_oldsplus_key == 0xff00)
				state->m_oldsplus_key = 0x100;
		}
		data ^= realkey;
		state->m_oldsplus_int[1] = data;
		state->m_oldsplus_int[0] ^= realkey;

		switch (state->m_oldsplus_int[1] & 0xff)
		{
			case 0x88:
				state->m_oldsplus_key = 0x100;
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xd0:
				state->m_oldsplus_val = 0xa01000 + (state->m_oldsplus_int[0] << 5);
				break;

			case 0xc0:
				state->m_oldsplus_val = 0xa00000 + (state->m_oldsplus_int[0] << 6);
				break;

			case 0xc3:
				state->m_oldsplus_val = 0xa00800 + (state->m_oldsplus_int[0] << 6);
				break;

			case 0x36:
				state->m_oldsplus_ram[0x36] = state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0x33:
				state->m_oldsplus_ram[0x33] = state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0x35:
				state->m_oldsplus_ram[0x36] += state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0x37:
				state->m_oldsplus_ram[0x33] += state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0x34:
				state->m_oldsplus_val = state->m_oldsplus_ram[0x36];
				break;

			case 0x38:
				state->m_oldsplus_val = state->m_oldsplus_ram[0x33];
				break;

			case 0x80:
				state->m_oldsplus_val = oldsplus_80[state->m_oldsplus_int[0]];
				break;

			case 0xe7:
				state->m_oldsplus_ram[0xe7] = state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xe5:
				switch (state->m_oldsplus_ram[0xe7])
				{
					case 0xb000:
						state->m_oldsplus_regs[0xb] = state->m_oldsplus_int[0];
						state->m_oldsplus_regs[0xc] = 0;
						break;

					case 0xc000:
						state->m_oldsplus_regs[0xc] = state->m_oldsplus_int[0];
						break;

					case 0xd000:
						state->m_oldsplus_regs[0xd] = state->m_oldsplus_int[0];
						break;

					case 0xf000:
						state->m_oldsplus_regs[0xf] = state->m_oldsplus_int[0];
						break;
				}
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xf8:
				state->m_oldsplus_val = state->m_oldsplus_regs[state->m_oldsplus_int[0]];
				break;

			case 0xfc:
				state->m_oldsplus_val = oldsplus_fc[state->m_oldsplus_int[0]];
				break;

			case 0xc5:
				state->m_oldsplus_regs[0xd] --;
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xd6:
				state->m_oldsplus_regs[0xb] ++;
				state->m_oldsplus_val = 0x990000;
				break;

			case 0x3a:
				state->m_oldsplus_regs[0xf] = 0;
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xf0:
				state->m_oldsplus_ram[0xf0] = state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xed:
				state->m_oldsplus_val = state->m_oldsplus_int[0] << 0x6;
				state->m_oldsplus_val += state->m_oldsplus_ram[0xf0];
				state->m_oldsplus_val = state->m_oldsplus_val << 0x2;
				state->m_oldsplus_val += 0x900000;
				break;

			case 0xe0:
				state->m_oldsplus_ram[0xe0] = state->m_oldsplus_int[0];
				state->m_oldsplus_val = 0x990000;
				break;

			case 0xdc:
				state->m_oldsplus_val = state->m_oldsplus_int[0] << 0x6;
				state->m_oldsplus_val += state->m_oldsplus_ram[0xe0];
				state->m_oldsplus_val = state->m_oldsplus_val << 0x2;
				state->m_oldsplus_val += 0x904000;
				break;

			case 0xcb:
				state->m_oldsplus_val =  0xc000;
				break;

			case 0xa0:
				state->m_oldsplus_val = oldsplus_a0[state->m_oldsplus_int[0]];
				break;

			case 0xba:
				state->m_oldsplus_val = oldsplus_ba[state->m_oldsplus_int[0]];
				break;

			case 0x5e:
				state->m_oldsplus_val = oldsplus_5e[state->m_oldsplus_int[0]];
				break;

			case 0xb0:
				state->m_oldsplus_val = oldsplus_b0[state->m_oldsplus_int[0]];
				break;

			case 0xae:
				state->m_oldsplus_val = oldsplus_ae[state->m_oldsplus_int[0]];
				break;

			case 0x9d:
				state->m_oldsplus_val = oldsplus_9d[state->m_oldsplus_int[0]];
				break;

			case 0x90:
				state->m_oldsplus_val = oldsplus_90[state->m_oldsplus_int[0]];
				break;

			case 0x8c:
				state->m_oldsplus_val = oldsplus_8c[state->m_oldsplus_int[0]];
				break;

			default:
				state->m_oldsplus_val = 0x990000;
				printf("%06X: oldsplus_UNKNOWN W CMD %X  VAL %X\n",cpu_get_pc(&space->device()),state->m_oldsplus_int[1],state->m_oldsplus_int[0]);
				break;
		}
	}
}

/* Old KOV and bootlegs sim ... really these should be read out... */

static const UINT8 kov_BATABLE[0x40] = {
	0x00,0x29,0x2c,0x35,0x3a,0x41,0x4a,0x4e,0x57,0x5e,0x77,0x79,0x7a,0x7b,0x7c,0x7d,
	0x7e,0x7f,0x80,0x81,0x82,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x90,
	0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9e,0xa3,0xd4,0xa9,0xaf,0xb5,0xbb,0xc1
};

static const UINT8 kov_B0TABLE[16] = { 2, 0, 1, 4, 3 }; // Maps char portraits to tables

static UINT32 kov_slots[16];
static UINT16 kov_internal_slot;
static UINT16 kov_key;
static UINT32 kov_response;
static UINT16 kov_value;

static UINT16 kov_c0_value;
static UINT16 kov_cb_value;
static UINT16 kov_fe_value;

WRITE16_HANDLER( kov_asic_sim_w )
{
	switch ((offset*2) & 0x06)
	{
		case 0: kov_value = data; return;

		case 2:
		{
			if ((data >> 8) == 0xff) kov_key = 0xffff;

			kov_value ^= kov_key;

		//	bprintf (PRINT_NORMAL, _T("ASIC27 command: %2.2x data: %4.4x\n"), (data ^ kov_key) & 0xff, kov_value);

			switch ((data ^ kov_key) & 0xff)
			{
				case 0x67: // unknown or status check?
				case 0x8e:
				case 0xa3:
				case 0x33: // kovsgqyz (a3)
				case 0x3a: // kovplus
				case 0xc5: // kovplus
					kov_response = 0x880000;
				break;

				case 0x99: // Reset
					kov_response = 0x880000;
					kov_key = 0;
				break;

				case 0x9d: // Sprite palette offset
					kov_response = 0xa00000 + ((kov_value & 0x1f) * 0x40);
				break;

				case 0xb0: // Read from data table
					kov_response = kov_B0TABLE[kov_value & 0x0f];
				break;

				case 0xb4: // Copy slot 'a' to slot 'b'
				case 0xb7: // kovsgqyz (b4)
				{
					kov_response = 0x880000;

					if (kov_value == 0x0102) kov_value = 0x0100; // why?

					kov_slots[(kov_value >> 8) & 0x0f] = kov_slots[(kov_value >> 0) & 0x0f];
				}
				break;

				case 0xba: // Read from data table
					kov_response = kov_BATABLE[kov_value & 0x3f];
				break;

				case 0xc0: // Text layer 'x' select
					kov_response = 0x880000;
					kov_c0_value = kov_value;
				break;

				case 0xc3: // Text layer offset
					kov_response = 0x904000 + ((kov_c0_value + (kov_value * 0x40)) * 4);
				break;

				case 0xcb: // Background layer 'x' select
					kov_response = 0x880000;
					kov_cb_value = kov_value;
				break;

				case 0xcc: // Background layer offset
					if (kov_value & 0x400) kov_value = -(0x400 - (kov_value & 0x3ff));
					kov_response = 0x900000 + ((kov_cb_value + (kov_value * 0x40)) * 4);
				break;

				case 0xd0: // Text palette offset
				case 0xcd: // kovsgqyz (d0)
					kov_response = 0xa01000 + (kov_value * 0x20);
				break;

				case 0xd6: // Copy slot to slot 0
					kov_response = 0x880000;
					kov_slots[0] = kov_slots[kov_value & 0x0f];
				break;

				case 0xdc: // Background palette offset
				case 0x11: // kovsgqyz (dc)
					kov_response = 0xa00800 + (kov_value * 0x40);
				break;

				case 0xe0: // Sprite palette offset
				case 0x9e: // kovsgqyz (e0)
					kov_response = 0xa00000 + ((kov_value & 0x1f) * 0x40);
				break;

				case 0xe5: // Write slot (low)
				{
					kov_response = 0x880000;

					INT32 sel = (kov_internal_slot >> 12) & 0x0f;
					kov_slots[sel] = (kov_slots[sel] & 0x00ff0000) | ((kov_value & 0xffff) <<  0);
				}
				break;

				case 0xe7: // Write slot (and slot select) (high)
				{
					kov_response = 0x880000;
					kov_internal_slot = kov_value;

					INT32 sel = (kov_internal_slot >> 12) & 0x0f;
					kov_slots[sel] = (kov_slots[sel] & 0x0000ffff) | ((kov_value & 0x00ff) << 16);
				}
				break;

				case 0xf0: // Some sort of status read?
					kov_response = 0x00c000;
				break;

				case 0xf8: // Read slot
				case 0xab: // kovsgqyz (f8)
					kov_response = kov_slots[kov_value & 0x0f] & 0x00ffffff;
				break;

				case 0xfc: // Adjust damage level to char experience level
					kov_response = (kov_value * kov_fe_value) >> 6;
				break;

				case 0xfe: // Damage level adjust
					kov_response = 0x880000;
					kov_fe_value = kov_value;
				break;

				default:
					kov_response = 0x880000;
		//			bprintf (PRINT_NORMAL, _T("Unknown ASIC27 command: %2.2x data: %4.4x\n"), (data ^ kov_key) & 0xff, kov_value);
				break;
			}

			kov_key = (kov_key + 0x0100) & 0xff00;
			if (kov_key == 0xff00) kov_key = 0x0100;
			kov_key |= kov_key >> 8;
		}
		return;

		case 4: return;
	}
}

READ16_HANDLER( kov_asic_sim_r )
{
	switch ((offset*2) & 0x02)
	{
		case 0: return (kov_response >>  0) ^ kov_key;
		case 2: return (kov_response >> 16) ^ kov_key;
	}

	return 0;
}

MACHINE_RESET( kov )
{
	kov_internal_slot = 0;
	kov_key = 0;
	kov_response = 0;
	kov_value = 0;

	kov_c0_value = 0;
	kov_cb_value = 0;
	kov_fe_value = 0;

	cputag_set_input_line(machine, "soundcpu", INPUT_LINE_HALT, ASSERT_LINE);
}

void install_protection_asic_sim_kov(running_machine &machine)
{
	machine.device("maincpu")->memory().space(AS_PROGRAM)->install_legacy_readwrite_handler(0x500000, 0x500003, FUNC(kov_asic_sim_r), FUNC(kov_asic_sim_w));
	machine.device("maincpu")->memory().space(AS_PROGRAM)->install_legacy_read_handler(0x4f0000, 0x4fffff, FUNC(sango_protram_r));
}
