#ifndef __SNS_SDD1_H
#define __SNS_SDD1_H

#include "machine/sns_slot.h"

// misc classes for the S-DD1

class SDD1__IM //Input Manager
{
public:
	SDD1__IM() {}

	UINT32 m_byte_ptr;
	UINT8 m_bit_count;

	void IM_prepareDecomp(UINT32 in_buf);
	UINT8 IM_getCodeword(UINT8 *ROM, UINT32 *mmc, const UINT8 code_len);
};

class SDD1__GCD //Golomb-Code Decoder
{
public:
	SDD1__GCD(SDD1__IM* associatedIM)
	: m_IM(associatedIM) { }

	SDD1__IM* m_IM;

	void GCD_getRunCount(UINT8 *ROM, UINT32 *mmc, UINT8 code_num, UINT8* MPScount, UINT8* LPSind);
};

class SDD1__BG // Bits Generator
{
public:
	SDD1__BG(SDD1__GCD* associatedGCD, UINT8 code)
	: m_code_num(code),
	m_GCD(associatedGCD) { }

	UINT8 m_code_num;
	UINT8 m_MPScount;
	UINT8 m_LPSind;
	SDD1__GCD* m_GCD;

	void BG_prepareDecomp();
	UINT8 BG_getBit(UINT8 *ROM, UINT32 *mmc, UINT8* endOfRun);
} ;

struct SDD1__PEM_ContextInfo
{
	UINT8 status;
	UINT8 MPS;
};

class SDD1__PEM //Probability Estimation Module
{
public:
	SDD1__PEM(
				SDD1__BG* associatedBG0, SDD1__BG* associatedBG1,
				SDD1__BG* associatedBG2, SDD1__BG* associatedBG3,
				SDD1__BG* associatedBG4, SDD1__BG* associatedBG5,
				SDD1__BG* associatedBG6, SDD1__BG* associatedBG7)
	{
		m_BG[0] = associatedBG0;
		m_BG[1] = associatedBG1;
		m_BG[2] = associatedBG2;
		m_BG[3] = associatedBG3;
		m_BG[4] = associatedBG4;
		m_BG[5] = associatedBG5;
		m_BG[6] = associatedBG6;
		m_BG[7] = associatedBG7;
	}

	SDD1__PEM_ContextInfo m_contextInfo[32];
	SDD1__BG* m_BG[8];

	void PEM_prepareDecomp();
	UINT8 PEM_getBit(UINT8 *ROM, UINT32 *mmc, UINT8 context);
} ;


class SDD1__CM
{
public:
	SDD1__CM(SDD1__PEM* associatedPEM)
	: m_PEM(associatedPEM) { }

	UINT8 m_bitplanesInfo;
	UINT8 m_contextBitsInfo;
	UINT8 m_bit_number;
	UINT8 m_currBitplane;
	UINT16 m_prevBitplaneBits[8];
	SDD1__PEM* m_PEM;

	void CM_prepareDecomp(UINT8 *ROM, UINT32 *mmc, UINT32 first_byte);
	UINT8 CM_getBit(UINT8 *ROM, UINT32 *mmc);
} ;


class SDD1__OL
{
public:
	SDD1__OL(SDD1__CM* associatedCM)
	: m_CM(associatedCM) { }

	UINT8 m_bitplanesInfo;
	UINT16 m_length;
	UINT8* m_buffer;
	SDD1__CM* m_CM;

	void OL_prepareDecomp(UINT8 *ROM, UINT32 *mmc, UINT32 first_byte, UINT16 out_len, UINT8 *out_buf);
	void OL_launch(UINT8 *ROM, UINT32 *mmc);
} ;

class SDD1__emu
{
public:
	SDD1__emu(running_machine &machine);

	running_machine &machine() const { return m_machine; }

	SDD1__IM* m_IM;
	SDD1__GCD* m_GCD;
	SDD1__BG* m_BG0;   SDD1__BG* m_BG1;   SDD1__BG* m_BG2;   SDD1__BG* m_BG3;
	SDD1__BG* m_BG4;   SDD1__BG* m_BG5;   SDD1__BG* m_BG6;   SDD1__BG* m_BG7;
	SDD1__PEM* m_PEM;
	SDD1__CM* m_CM;
	SDD1__OL* m_OL;

	void SDD1emu_decompress(UINT8 *ROM, UINT32 *mmc, UINT32 in_buf, UINT16 out_len, UINT8 *out_buf);

private:
	running_machine& m_machine;
};



// ======================> sns_rom_sdd1_device

class sns_rom_sdd1_device : public device_t,
						public device_sns_cart_interface
{
public:
	// construction/destruction
	sns_rom_sdd1_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock);
	sns_rom_sdd1_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete() { m_shortname = "sns_rom_sdd1"; }

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_l);
	virtual DECLARE_READ8_MEMBER(read_h);
	virtual DECLARE_READ8_MEMBER(read_ram);
	virtual DECLARE_WRITE8_MEMBER(write_ram);
	virtual DECLARE_READ8_MEMBER(chip_read);
	virtual DECLARE_WRITE8_MEMBER(chip_write);

	UINT8 read_helper(UINT32 offset);

	UINT8 m_sdd1_enable;  // channel bit-mask
	UINT8 m_xfer_enable;  // channel bit-mask
	UINT32 m_mmc[4];      // memory map controller ROM indices

	struct
	{
		UINT32 addr;    // $43x2-$43x4 -- DMA transfer address
		UINT16 size;    // $43x5-$43x6 -- DMA transfer size
	} m_dma[8];

	SDD1__emu* m_sdd1emu;

	struct
	{
		UINT8 *data;    // pointer to decompressed S-DD1 data (65536 bytes)
		UINT16 offset;  // read index into S-DD1 decompression buffer
		UINT32 size;    // length of data buffer; reads decrement counter, set ready to false at 0
		UINT8 ready;    // 1 when data[] is valid; 0 to invoke sdd1emu.decompress()
	} m_buffer;
};


// device type definition
extern const device_type SNS_LOROM_SDD1;

#endif
