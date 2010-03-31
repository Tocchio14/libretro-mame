typedef struct {
	UINT8 opcode;
	UINT32 flags;
	void (*handler16)(i386_state *cpustate);
	void (*handler32)(i386_state *cpustate);
} X86_OPCODE;

#define OP_I386			0x1
#define OP_FPU			0x2
#define OP_I486			0x4
#define OP_PENTIUM		0x8
#define OP_MMX			0x10
#define OP_PPRO			0x20
#define OP_SSE			0x40
#define OP_SSE2			0x80
#define OP_SSE3			0x100
#define OP_CYRIX		0x8000
#define OP_2BYTE		0x80000000

static const X86_OPCODE x86_opcode_table[] =
{
//  Opcode      Flags                       16-bit handler                  32-bit handler
	{ 0x00,		OP_I386,					I386OP(add_rm8_r8),				I386OP(add_rm8_r8),			},
	{ 0x01,		OP_I386,					I386OP(add_rm16_r16),			I386OP(add_rm32_r32),		},
	{ 0x02,		OP_I386,					I386OP(add_r8_rm8),				I386OP(add_r8_rm8),			},
	{ 0x03,		OP_I386,					I386OP(add_r16_rm16),			I386OP(add_r32_rm32),		},
	{ 0x04,		OP_I386,					I386OP(add_al_i8),				I386OP(add_al_i8),			},
	{ 0x05,		OP_I386,					I386OP(add_ax_i16),				I386OP(add_eax_i32),		},
	{ 0x06,		OP_I386,					I386OP(push_es16),				I386OP(push_es32),			},
	{ 0x07,		OP_I386,					I386OP(pop_es16),				I386OP(pop_es32),			},
	{ 0x08,		OP_I386,					I386OP(or_rm8_r8),				I386OP(or_rm8_r8),			},
	{ 0x09,		OP_I386,					I386OP(or_rm16_r16),			I386OP(or_rm32_r32),		},
	{ 0x0A,		OP_I386,					I386OP(or_r8_rm8),				I386OP(or_r8_rm8),			},
	{ 0x0B,		OP_I386,					I386OP(or_r16_rm16),			I386OP(or_r32_rm32),		},
	{ 0x0C,		OP_I386,					I386OP(or_al_i8),				I386OP(or_al_i8),			},
	{ 0x0D,		OP_I386,					I386OP(or_ax_i16),				I386OP(or_eax_i32),			},
	{ 0x0E,		OP_I386,					I386OP(push_cs16),				I386OP(push_cs32),			},
	{ 0x0F,		OP_I386,					I386OP(decode_two_byte),		I386OP(decode_two_byte),	},
	{ 0x10,		OP_I386,					I386OP(adc_rm8_r8),				I386OP(adc_rm8_r8),			},
	{ 0x11,		OP_I386,					I386OP(adc_rm16_r16),			I386OP(adc_rm32_r32),		},
	{ 0x12,		OP_I386,					I386OP(adc_r8_rm8),				I386OP(adc_r8_rm8),			},
	{ 0x13,		OP_I386,					I386OP(adc_r16_rm16),			I386OP(adc_r32_rm32),		},
	{ 0x14,		OP_I386,					I386OP(adc_al_i8),				I386OP(adc_al_i8),			},
	{ 0x15,		OP_I386,					I386OP(adc_ax_i16),				I386OP(adc_eax_i32),		},
	{ 0x16,		OP_I386,					I386OP(push_ss16),				I386OP(push_ss32),			},
	{ 0x17,		OP_I386,					I386OP(pop_ss16),				I386OP(pop_ss32),			},
	{ 0x18,		OP_I386,					I386OP(sbb_rm8_r8),				I386OP(sbb_rm8_r8),			},
	{ 0x19,		OP_I386,					I386OP(sbb_rm16_r16),			I386OP(sbb_rm32_r32),		},
	{ 0x1A,		OP_I386,					I386OP(sbb_r8_rm8),				I386OP(sbb_r8_rm8),			},
	{ 0x1B,		OP_I386,					I386OP(sbb_r16_rm16),			I386OP(sbb_r32_rm32),		},
	{ 0x1C,		OP_I386,					I386OP(sbb_al_i8),				I386OP(sbb_al_i8),			},
	{ 0x1D,		OP_I386,					I386OP(sbb_ax_i16),				I386OP(sbb_eax_i32),		},
	{ 0x1E,		OP_I386,					I386OP(push_ds16),				I386OP(push_ds32),			},
	{ 0x1F,		OP_I386,					I386OP(pop_ds16),				I386OP(pop_ds32),			},
	{ 0x20,		OP_I386,					I386OP(and_rm8_r8),				I386OP(and_rm8_r8),			},
	{ 0x21,		OP_I386,					I386OP(and_rm16_r16),			I386OP(and_rm32_r32),		},
	{ 0x22,		OP_I386,					I386OP(and_r8_rm8),				I386OP(and_r8_rm8),			},
	{ 0x23,		OP_I386,					I386OP(and_r16_rm16),			I386OP(and_r32_rm32),		},
	{ 0x24,		OP_I386,					I386OP(and_al_i8),				I386OP(and_al_i8),			},
	{ 0x25,		OP_I386,					I386OP(and_ax_i16),				I386OP(and_eax_i32),		},
	{ 0x26,		OP_I386,					I386OP(segment_ES),				I386OP(segment_ES),			},
	{ 0x27,		OP_I386,					I386OP(daa),					I386OP(daa),				},
	{ 0x28,		OP_I386,					I386OP(sub_rm8_r8),				I386OP(sub_rm8_r8),			},
	{ 0x29,		OP_I386,					I386OP(sub_rm16_r16),			I386OP(sub_rm32_r32),		},
	{ 0x2A,		OP_I386,					I386OP(sub_r8_rm8),				I386OP(sub_r8_rm8),			},
	{ 0x2B,		OP_I386,					I386OP(sub_r16_rm16),			I386OP(sub_r32_rm32),		},
	{ 0x2C,		OP_I386,					I386OP(sub_al_i8),				I386OP(sub_al_i8),			},
	{ 0x2D,		OP_I386,					I386OP(sub_ax_i16),				I386OP(sub_eax_i32),		},
	{ 0x2E,		OP_I386,					I386OP(segment_CS),				I386OP(segment_CS),			},
	{ 0x2F,		OP_I386,					I386OP(das),					I386OP(das),				},
	{ 0x30,		OP_I386,					I386OP(xor_rm8_r8),				I386OP(xor_rm8_r8),			},
	{ 0x31,		OP_I386,					I386OP(xor_rm16_r16),			I386OP(xor_rm32_r32),		},
	{ 0x32,		OP_I386,					I386OP(xor_r8_rm8),				I386OP(xor_r8_rm8),			},
	{ 0x33,		OP_I386,					I386OP(xor_r16_rm16),			I386OP(xor_r32_rm32),		},
	{ 0x34,		OP_I386,					I386OP(xor_al_i8),				I386OP(xor_al_i8),			},
	{ 0x35,		OP_I386,					I386OP(xor_ax_i16),				I386OP(xor_eax_i32),		},
	{ 0x36,		OP_I386,					I386OP(segment_SS),				I386OP(segment_SS),			},
	{ 0x37,		OP_I386,					I386OP(aaa),					I386OP(aaa),				},
	{ 0x38,		OP_I386,					I386OP(cmp_rm8_r8),				I386OP(cmp_rm8_r8),			},
	{ 0x39,		OP_I386,					I386OP(cmp_rm16_r16),			I386OP(cmp_rm32_r32),		},
	{ 0x3A,		OP_I386,					I386OP(cmp_r8_rm8),				I386OP(cmp_r8_rm8),			},
	{ 0x3B,		OP_I386,					I386OP(cmp_r16_rm16),			I386OP(cmp_r32_rm32),		},
	{ 0x3C,		OP_I386,					I386OP(cmp_al_i8),				I386OP(cmp_al_i8),			},
	{ 0x3D,		OP_I386,					I386OP(cmp_ax_i16),				I386OP(cmp_eax_i32),		},
	{ 0x3E,		OP_I386,					I386OP(segment_DS),				I386OP(segment_DS),			},
	{ 0x3F,		OP_I386,					I386OP(aas),					I386OP(aas),				},
	{ 0x40,		OP_I386,					I386OP(inc_ax),					I386OP(inc_eax),			},
	{ 0x41,		OP_I386,					I386OP(inc_cx),					I386OP(inc_ecx),			},
	{ 0x42,		OP_I386,					I386OP(inc_dx),					I386OP(inc_edx),			},
	{ 0x43,		OP_I386,					I386OP(inc_bx),					I386OP(inc_ebx),			},
	{ 0x44,		OP_I386,					I386OP(inc_sp),					I386OP(inc_esp),			},
	{ 0x45,		OP_I386,					I386OP(inc_bp),					I386OP(inc_ebp),			},
	{ 0x46,		OP_I386,					I386OP(inc_si),					I386OP(inc_esi),			},
	{ 0x47,		OP_I386,					I386OP(inc_di),					I386OP(inc_edi),			},
	{ 0x48,		OP_I386,					I386OP(dec_ax),					I386OP(dec_eax),			},
	{ 0x49,		OP_I386,					I386OP(dec_cx),					I386OP(dec_ecx),			},
	{ 0x4A,		OP_I386,					I386OP(dec_dx),					I386OP(dec_edx),			},
	{ 0x4B,		OP_I386,					I386OP(dec_bx),					I386OP(dec_ebx),			},
	{ 0x4C,		OP_I386,					I386OP(dec_sp),					I386OP(dec_esp),			},
	{ 0x4D,		OP_I386,					I386OP(dec_bp),					I386OP(dec_ebp),			},
	{ 0x4E,		OP_I386,					I386OP(dec_si),					I386OP(dec_esi),			},
	{ 0x4F,		OP_I386,					I386OP(dec_di),					I386OP(dec_edi),			},
	{ 0x50,		OP_I386,					I386OP(push_ax),				I386OP(push_eax),			},
	{ 0x51,		OP_I386,					I386OP(push_cx),				I386OP(push_ecx),			},
	{ 0x52,		OP_I386,					I386OP(push_dx),				I386OP(push_edx),			},
	{ 0x53,		OP_I386,					I386OP(push_bx),				I386OP(push_ebx),			},
	{ 0x54,		OP_I386,					I386OP(push_sp),				I386OP(push_esp),			},
	{ 0x55,		OP_I386,					I386OP(push_bp),				I386OP(push_ebp),			},
	{ 0x56,		OP_I386,					I386OP(push_si),				I386OP(push_esi),			},
	{ 0x57,		OP_I386,					I386OP(push_di),				I386OP(push_edi),			},
	{ 0x58,		OP_I386,					I386OP(pop_ax),					I386OP(pop_eax),			},
	{ 0x59,		OP_I386,					I386OP(pop_cx),					I386OP(pop_ecx),			},
	{ 0x5A,		OP_I386,					I386OP(pop_dx),					I386OP(pop_edx),			},
	{ 0x5B,		OP_I386,					I386OP(pop_bx),					I386OP(pop_ebx),			},
	{ 0x5C,		OP_I386,					I386OP(pop_sp),					I386OP(pop_esp),			},
	{ 0x5D,		OP_I386,					I386OP(pop_bp),					I386OP(pop_ebp),			},
	{ 0x5E,		OP_I386,					I386OP(pop_si),					I386OP(pop_esi),			},
	{ 0x5F,		OP_I386,					I386OP(pop_di),					I386OP(pop_edi),			},
	{ 0x60,		OP_I386,					I386OP(pusha),					I386OP(pushad),				},
	{ 0x61,		OP_I386,					I386OP(popa),					I386OP(popad),				},
	{ 0x62,		OP_I386,					I386OP(bound_r16_m16_m16),		I386OP(bound_r32_m32_m32),	},
	{ 0x63,		OP_I386,					I386OP(arpl),					I386OP(arpl),				},
	{ 0x64,		OP_I386,					I386OP(segment_FS),				I386OP(segment_FS),			},
	{ 0x65,		OP_I386,					I386OP(segment_GS),				I386OP(segment_GS),			},
	{ 0x66,		OP_I386,					I386OP(operand_size),			I386OP(operand_size),		},
	{ 0x67,		OP_I386,					I386OP(address_size),			I386OP(address_size),		},
	{ 0x68,		OP_I386,					I386OP(push_i16),				I386OP(push_i32),			},
	{ 0x69,		OP_I386,					I386OP(imul_r16_rm16_i16),		I386OP(imul_r32_rm32_i32),	},
	{ 0x6A,		OP_I386,					I386OP(push_i8),				I386OP(push_i8),			},
	{ 0x6B,		OP_I386,					I386OP(imul_r16_rm16_i8),		I386OP(imul_r32_rm32_i8),	},
	{ 0x6C,		OP_I386,					I386OP(insb),					I386OP(insb),				},
	{ 0x6D,		OP_I386,					I386OP(insw),					I386OP(insd),				},
	{ 0x6E,		OP_I386,					I386OP(outsb),					I386OP(outsb),				},
	{ 0x6F,		OP_I386,					I386OP(outsw),					I386OP(outsd),				},
	{ 0x70,		OP_I386,					I386OP(jo_rel8),				I386OP(jo_rel8),			},
	{ 0x71,		OP_I386,					I386OP(jno_rel8),				I386OP(jno_rel8),			},
	{ 0x72,		OP_I386,					I386OP(jc_rel8),				I386OP(jc_rel8),			},
	{ 0x73,		OP_I386,					I386OP(jnc_rel8),				I386OP(jnc_rel8),			},
	{ 0x74,		OP_I386,					I386OP(jz_rel8),				I386OP(jz_rel8),			},
	{ 0x75,		OP_I386,					I386OP(jnz_rel8),				I386OP(jnz_rel8),			},
	{ 0x76,		OP_I386,					I386OP(jbe_rel8),				I386OP(jbe_rel8),			},
	{ 0x77,		OP_I386,					I386OP(ja_rel8),				I386OP(ja_rel8),			},
	{ 0x78,		OP_I386,					I386OP(js_rel8),				I386OP(js_rel8),			},
	{ 0x79,		OP_I386,					I386OP(jns_rel8),				I386OP(jns_rel8),			},
	{ 0x7A,		OP_I386,					I386OP(jp_rel8),				I386OP(jp_rel8),			},
	{ 0x7B,		OP_I386,					I386OP(jnp_rel8),				I386OP(jnp_rel8),			},
	{ 0x7C,		OP_I386,					I386OP(jl_rel8),				I386OP(jl_rel8),			},
	{ 0x7D,		OP_I386,					I386OP(jge_rel8),				I386OP(jge_rel8),			},
	{ 0x7E,		OP_I386,					I386OP(jle_rel8),				I386OP(jle_rel8),			},
	{ 0x7F,		OP_I386,					I386OP(jg_rel8),				I386OP(jg_rel8),			},
	{ 0x80,		OP_I386,					I386OP(group80_8),				I386OP(group80_8),			},
	{ 0x81,		OP_I386,					I386OP(group81_16),				I386OP(group81_32),			},
	{ 0x82,		OP_I386,					I386OP(group80_8),				I386OP(group80_8),			},
	{ 0x83,		OP_I386,					I386OP(group83_16),				I386OP(group83_32),			},
	{ 0x84,		OP_I386,					I386OP(test_rm8_r8),			I386OP(test_rm8_r8),		},
	{ 0x85,		OP_I386,					I386OP(test_rm16_r16),			I386OP(test_rm32_r32),		},
	{ 0x86,		OP_I386,					I386OP(xchg_r8_rm8),			I386OP(xchg_r8_rm8),		},
	{ 0x87,		OP_I386,					I386OP(xchg_r16_rm16),			I386OP(xchg_r32_rm32),		},
	{ 0x88,		OP_I386,					I386OP(mov_rm8_r8),				I386OP(mov_rm8_r8),			},
	{ 0x89,		OP_I386,					I386OP(mov_rm16_r16),			I386OP(mov_rm32_r32),		},
	{ 0x8A,		OP_I386,					I386OP(mov_r8_rm8),				I386OP(mov_r8_rm8),			},
	{ 0x8B,		OP_I386,					I386OP(mov_r16_rm16),			I386OP(mov_r32_rm32),		},
	{ 0x8C,		OP_I386,					I386OP(mov_rm16_sreg),			I386OP(mov_rm16_sreg),		},
	{ 0x8D,		OP_I386,					I386OP(lea16),					I386OP(lea32),				},
	{ 0x8E,		OP_I386,					I386OP(mov_sreg_rm16),			I386OP(mov_sreg_rm16),		},
	{ 0x8F,		OP_I386,					I386OP(pop_rm16),				I386OP(pop_rm32),			},
	{ 0x90,		OP_I386,					I386OP(nop),					I386OP(nop),				},
	{ 0x91,		OP_I386,					I386OP(xchg_ax_cx),				I386OP(xchg_eax_ecx),		},
	{ 0x92,		OP_I386,					I386OP(xchg_ax_dx),				I386OP(xchg_eax_edx),		},
	{ 0x93,		OP_I386,					I386OP(xchg_ax_bx),				I386OP(xchg_eax_ebx),		},
	{ 0x94,		OP_I386,					I386OP(xchg_ax_sp),				I386OP(xchg_eax_esp),		},
	{ 0x95,		OP_I386,					I386OP(xchg_ax_bp),				I386OP(xchg_eax_ebp),		},
	{ 0x96,		OP_I386,					I386OP(xchg_ax_si),				I386OP(xchg_eax_esi),		},
	{ 0x97,		OP_I386,					I386OP(xchg_ax_di),				I386OP(xchg_eax_edi),		},
	{ 0x98,		OP_I386,					I386OP(cbw),					I386OP(cwde),				},
	{ 0x99,		OP_I386,					I386OP(cwd),					I386OP(cdq),				},
	{ 0x9A,		OP_I386,					I386OP(call_abs16),				I386OP(call_abs32),			},
	{ 0x9B,		OP_I386,					I386OP(wait),					I386OP(wait),				},
	{ 0x9C,		OP_I386,					I386OP(pushf),					I386OP(pushfd),				},
	{ 0x9D,		OP_I386,					I386OP(popf),					I386OP(popfd),				},
	{ 0x9E,		OP_I386,					I386OP(sahf),					I386OP(sahf),				},
	{ 0x9F,		OP_I386,					I386OP(lahf),					I386OP(lahf),				},
	{ 0xA0,		OP_I386,					I386OP(mov_al_m8),				I386OP(mov_al_m8),			},
	{ 0xA1,		OP_I386,					I386OP(mov_ax_m16),				I386OP(mov_eax_m32),		},
	{ 0xA2,		OP_I386,					I386OP(mov_m8_al),				I386OP(mov_m8_al),			},
	{ 0xA3,		OP_I386,					I386OP(mov_m16_ax),				I386OP(mov_m32_eax),		},
	{ 0xA4,		OP_I386,					I386OP(movsb),					I386OP(movsb),				},
	{ 0xA5,		OP_I386,					I386OP(movsw),					I386OP(movsd),				},
	{ 0xA6,		OP_I386,					I386OP(cmpsb),					I386OP(cmpsb),				},
	{ 0xA7,		OP_I386,					I386OP(cmpsw),					I386OP(cmpsd),				},
	{ 0xA8,		OP_I386,					I386OP(test_al_i8),				I386OP(test_al_i8),			},
	{ 0xA9,		OP_I386,					I386OP(test_ax_i16),			I386OP(test_eax_i32),		},
	{ 0xAA,		OP_I386,					I386OP(stosb),					I386OP(stosb),				},
	{ 0xAB,		OP_I386,					I386OP(stosw),					I386OP(stosd),				},
	{ 0xAC,		OP_I386,					I386OP(lodsb),					I386OP(lodsb),				},
	{ 0xAD,		OP_I386,					I386OP(lodsw),					I386OP(lodsd),				},
	{ 0xAE,		OP_I386,					I386OP(scasb),					I386OP(scasb),				},
	{ 0xAF,		OP_I386,					I386OP(scasw),					I386OP(scasd),				},
	{ 0xB0,		OP_I386,					I386OP(mov_al_i8),				I386OP(mov_al_i8),			},
	{ 0xB1,		OP_I386,					I386OP(mov_cl_i8),				I386OP(mov_cl_i8),			},
	{ 0xB2,		OP_I386,					I386OP(mov_dl_i8),				I386OP(mov_dl_i8),			},
	{ 0xB3,		OP_I386,					I386OP(mov_bl_i8),				I386OP(mov_bl_i8),			},
	{ 0xB4,		OP_I386,					I386OP(mov_ah_i8),				I386OP(mov_ah_i8),			},
	{ 0xB5,		OP_I386,					I386OP(mov_ch_i8),				I386OP(mov_ch_i8),			},
	{ 0xB6,		OP_I386,					I386OP(mov_dh_i8),				I386OP(mov_dh_i8),			},
	{ 0xB7,		OP_I386,					I386OP(mov_bh_i8),				I386OP(mov_bh_i8),			},
	{ 0xB8,		OP_I386,					I386OP(mov_ax_i16),				I386OP(mov_eax_i32),		},
	{ 0xB9,		OP_I386,					I386OP(mov_cx_i16),				I386OP(mov_ecx_i32),		},
	{ 0xBA,		OP_I386,					I386OP(mov_dx_i16),				I386OP(mov_edx_i32),		},
	{ 0xBB,		OP_I386,					I386OP(mov_bx_i16),				I386OP(mov_ebx_i32),		},
	{ 0xBC,		OP_I386,					I386OP(mov_sp_i16),				I386OP(mov_esp_i32),		},
	{ 0xBD,		OP_I386,					I386OP(mov_bp_i16),				I386OP(mov_ebp_i32),		},
	{ 0xBE,		OP_I386,					I386OP(mov_si_i16),				I386OP(mov_esi_i32),		},
	{ 0xBF,		OP_I386,					I386OP(mov_di_i16),				I386OP(mov_edi_i32),		},
	{ 0xC0,		OP_I386,					I386OP(groupC0_8),				I386OP(groupC0_8),			},
	{ 0xC1,		OP_I386,					I386OP(groupC1_16),				I386OP(groupC1_32),			},
	{ 0xC2,		OP_I386,					I386OP(ret_near16_i16),			I386OP(ret_near32_i16),		},
	{ 0xC3,		OP_I386,					I386OP(ret_near16),				I386OP(ret_near32),			},
	{ 0xC4,		OP_I386,					I386OP(les16),					I386OP(les32),				},
	{ 0xC5,		OP_I386,					I386OP(lds16),					I386OP(lds32),				},
	{ 0xC6,		OP_I386,					I386OP(mov_rm8_i8),				I386OP(mov_rm8_i8),			},
	{ 0xC7,		OP_I386,					I386OP(mov_rm16_i16),			I386OP(mov_rm32_i32),		},
	{ 0xC8,		OP_I386,					I386OP(enter16),				I386OP(enter32),			},
	{ 0xC9,		OP_I386,					I386OP(leave16),				I386OP(leave32),			},
	{ 0xCA,		OP_I386,					I386OP(retf_i16),				I386OP(retf_i32),			},
	{ 0xCB,		OP_I386,					I386OP(retf16),					I386OP(retf32),				},
	{ 0xCC,		OP_I386,					I386OP(int3),					I386OP(int3),				},
	{ 0xCD,		OP_I386,					I386OP(int),					I386OP(int),				},
	{ 0xCE,		OP_I386,					I386OP(into),					I386OP(into),				},
	{ 0xCF,		OP_I386,					I386OP(iret16),					I386OP(iret32),				},
	{ 0xD0,		OP_I386,					I386OP(groupD0_8),				I386OP(groupD0_8),			},
	{ 0xD1,		OP_I386,					I386OP(groupD1_16),				I386OP(groupD1_32),			},
	{ 0xD2,		OP_I386,					I386OP(groupD2_8),				I386OP(groupD2_8),			},
	{ 0xD3,		OP_I386,					I386OP(groupD3_16),				I386OP(groupD3_32),			},
	{ 0xD4,		OP_I386,					I386OP(aam),					I386OP(aam),				},
	{ 0xD5,		OP_I386,					I386OP(aad),					I386OP(aad),				},
	{ 0xD6,		OP_I386,					I386OP(setalc),					I386OP(setalc),				},
	{ 0xD7,		OP_I386,					I386OP(xlat16),					I386OP(xlat32),				},
	{ 0xD8,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xD9,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xDA,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xDB,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xDC,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xDD,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xDE,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xDF,		OP_I386,					I386OP(escape),					I386OP(escape),				},
	{ 0xD8,		OP_FPU,						I386OP(fpu_group_d8),			I386OP(fpu_group_d8),		},
	{ 0xD9,		OP_FPU,						I386OP(fpu_group_d9),			I386OP(fpu_group_d9),		},
	{ 0xDA,		OP_FPU,						I386OP(fpu_group_da),			I386OP(fpu_group_da),		},
	{ 0xDB,		OP_FPU,						I386OP(fpu_group_db),			I386OP(fpu_group_db),		},
	{ 0xDC,		OP_FPU,						I386OP(fpu_group_dc),			I386OP(fpu_group_dc),		},
	{ 0xDD,		OP_FPU,						I386OP(fpu_group_dd),			I386OP(fpu_group_dd),		},
	{ 0xDE,		OP_FPU,						I386OP(fpu_group_de),			I386OP(fpu_group_de),		},
	{ 0xDF,		OP_FPU,						I386OP(fpu_group_df),			I386OP(fpu_group_df),		},
	{ 0xE0,		OP_I386,					I386OP(loopne16),				I386OP(loopne32),			},
	{ 0xE1,		OP_I386,					I386OP(loopz16),				I386OP(loopz32),			},
	{ 0xE2,		OP_I386,					I386OP(loop16),					I386OP(loop32),				},
	{ 0xE3,		OP_I386,					I386OP(jcxz16),					I386OP(jcxz32),				},
	{ 0xE4,		OP_I386,					I386OP(in_al_i8),				I386OP(in_al_i8),			},
	{ 0xE5,		OP_I386,					I386OP(in_ax_i8),				I386OP(in_eax_i8),			},
	{ 0xE6,		OP_I386,					I386OP(out_al_i8),				I386OP(out_al_i8),			},
	{ 0xE7,		OP_I386,					I386OP(out_ax_i8),				I386OP(out_eax_i8),			},
	{ 0xE8,		OP_I386,					I386OP(call_rel16),				I386OP(call_rel32),			},
	{ 0xE9,		OP_I386,					I386OP(jmp_rel16),				I386OP(jmp_rel32),			},
	{ 0xEA,		OP_I386,					I386OP(jmp_abs16),				I386OP(jmp_abs32),			},
	{ 0xEB,		OP_I386,					I386OP(jmp_rel8),				I386OP(jmp_rel8),			},
	{ 0xEC,		OP_I386,					I386OP(in_al_dx),				I386OP(in_al_dx),			},
	{ 0xED,		OP_I386,					I386OP(in_ax_dx),				I386OP(in_eax_dx),			},
	{ 0xEE,		OP_I386,					I386OP(out_al_dx),				I386OP(out_al_dx),			},
	{ 0xEF,		OP_I386,					I386OP(out_ax_dx),				I386OP(out_eax_dx),			},
	{ 0xF0,		OP_I386,					I386OP(lock),					I386OP(lock),				},
	{ 0xF1,		OP_I386,					I386OP(invalid),				I386OP(invalid),			},
	{ 0xF2,		OP_I386,					I386OP(repne),					I386OP(repne),				},
	{ 0xF3,		OP_I386,					I386OP(rep),					I386OP(rep),				},
	{ 0xF4,		OP_I386,					I386OP(hlt),					I386OP(hlt),				},
	{ 0xF5,		OP_I386,					I386OP(cmc),					I386OP(cmc),				},
	{ 0xF6,		OP_I386,					I386OP(groupF6_8),				I386OP(groupF6_8),			},
	{ 0xF7,		OP_I386,					I386OP(groupF7_16),				I386OP(groupF7_32),			},
	{ 0xF8,		OP_I386,					I386OP(clc),					I386OP(clc),				},
	{ 0xF9,		OP_I386,					I386OP(stc),					I386OP(stc),				},
	{ 0xFA,		OP_I386,					I386OP(cli),					I386OP(cli),				},
	{ 0xFB,		OP_I386,					I386OP(sti),					I386OP(sti),				},
	{ 0xFC,		OP_I386,					I386OP(cld),					I386OP(cld),				},
	{ 0xFD,		OP_I386,					I386OP(std),					I386OP(std),				},
	{ 0xFE,		OP_I386,					I386OP(groupFE_8),				I386OP(groupFE_8),			},
	{ 0xFF,		OP_I386,					I386OP(groupFF_16),				I386OP(groupFF_32),			},
	{ 0x00,		OP_2BYTE|OP_I386,			I386OP(group0F00_16),			I386OP(group0F00_32),		},
	{ 0x01,		OP_2BYTE|OP_I386,			I386OP(group0F01_16),			I386OP(group0F01_32),		},
	{ 0x01,		OP_2BYTE|OP_I486,			I486OP(group0F01_16),			I486OP(group0F01_32),		},
	{ 0x02,		OP_2BYTE|OP_I386,			I386OP(unimplemented),			I386OP(unimplemented),		},
	{ 0x03,		OP_2BYTE|OP_I386,			I386OP(lsl_r16_rm16),			I386OP(lsl_r32_rm32),		},
	{ 0x06,		OP_2BYTE|OP_I386,			I386OP(clts),					I386OP(clts),				},
	{ 0x08,		OP_2BYTE|OP_I486,			I486OP(invd),					I486OP(invd),				},
	{ 0x09,		OP_2BYTE|OP_I486,			I486OP(wbinvd),					I486OP(wbinvd),				},
	{ 0x0B,		OP_2BYTE|OP_I386,			I386OP(unimplemented),			I386OP(unimplemented),		},
	{ 0x20,		OP_2BYTE|OP_I386,			I386OP(mov_r32_cr),				I386OP(mov_r32_cr),			},
	{ 0x21,		OP_2BYTE|OP_I386,			I386OP(mov_r32_dr),				I386OP(mov_r32_dr),			},
	{ 0x22,		OP_2BYTE|OP_I386,			I386OP(mov_cr_r32),				I386OP(mov_cr_r32),			},
	{ 0x23,		OP_2BYTE|OP_I386,			I386OP(mov_dr_r32),				I386OP(mov_dr_r32),			},
	{ 0x24,		OP_2BYTE|OP_I386,			I386OP(mov_r32_tr),				I386OP(mov_r32_tr),			},
	{ 0x26,		OP_2BYTE|OP_I386,			I386OP(mov_tr_r32),				I386OP(mov_tr_r32),			},
	{ 0x30,		OP_2BYTE|OP_PENTIUM,		PENTIUMOP(wrmsr),				PENTIUMOP(wrmsr),			},
	{ 0x31,		OP_2BYTE|OP_PENTIUM,		PENTIUMOP(rdtsc),				PENTIUMOP(rdtsc),			},
	{ 0x32,		OP_2BYTE|OP_PENTIUM,		PENTIUMOP(rdmsr),				PENTIUMOP(rdmsr),			},
	{ 0x74,		OP_2BYTE|OP_CYRIX,			I386OP(cyrix_unknown),			I386OP(cyrix_unknown),		},
	{ 0x80,		OP_2BYTE|OP_I386,			I386OP(jo_rel16),				I386OP(jo_rel32),			},
	{ 0x81,		OP_2BYTE|OP_I386,			I386OP(jno_rel16),				I386OP(jno_rel32),			},
	{ 0x82,		OP_2BYTE|OP_I386,			I386OP(jc_rel16),				I386OP(jc_rel32),			},
	{ 0x83,		OP_2BYTE|OP_I386,			I386OP(jnc_rel16),				I386OP(jnc_rel32),			},
	{ 0x84,		OP_2BYTE|OP_I386,			I386OP(jz_rel16),				I386OP(jz_rel32),			},
	{ 0x85,		OP_2BYTE|OP_I386,			I386OP(jnz_rel16),				I386OP(jnz_rel32),			},
	{ 0x86,		OP_2BYTE|OP_I386,			I386OP(jbe_rel16),				I386OP(jbe_rel32),			},
	{ 0x87,		OP_2BYTE|OP_I386,			I386OP(ja_rel16),				I386OP(ja_rel32),			},
	{ 0x88,		OP_2BYTE|OP_I386,			I386OP(js_rel16),				I386OP(js_rel32),			},
	{ 0x89,		OP_2BYTE|OP_I386,			I386OP(jns_rel16),				I386OP(jns_rel32),			},
	{ 0x8A,		OP_2BYTE|OP_I386,			I386OP(jp_rel16),				I386OP(jp_rel32),			},
	{ 0x8B,		OP_2BYTE|OP_I386,			I386OP(jnp_rel16),				I386OP(jnp_rel32),			},
	{ 0x8C,		OP_2BYTE|OP_I386,			I386OP(jl_rel16),				I386OP(jl_rel32),			},
	{ 0x8D,		OP_2BYTE|OP_I386,			I386OP(jge_rel16),				I386OP(jge_rel32),			},
	{ 0x8E,		OP_2BYTE|OP_I386,			I386OP(jle_rel16),				I386OP(jle_rel32),			},
	{ 0x8F,		OP_2BYTE|OP_I386,			I386OP(jg_rel16),				I386OP(jg_rel32),			},
	{ 0x90,		OP_2BYTE|OP_I386,			I386OP(seto_rm8),				I386OP(seto_rm8),			},
	{ 0x91,		OP_2BYTE|OP_I386,			I386OP(setno_rm8),				I386OP(setno_rm8),			},
	{ 0x92,		OP_2BYTE|OP_I386,			I386OP(setc_rm8),				I386OP(setc_rm8),			},
	{ 0x93,		OP_2BYTE|OP_I386,			I386OP(setnc_rm8),				I386OP(setnc_rm8),			},
	{ 0x94,		OP_2BYTE|OP_I386,			I386OP(setz_rm8),				I386OP(setz_rm8),			},
	{ 0x95,		OP_2BYTE|OP_I386,			I386OP(setnz_rm8),				I386OP(setnz_rm8),			},
	{ 0x96,		OP_2BYTE|OP_I386,			I386OP(setbe_rm8),				I386OP(setbe_rm8),			},
	{ 0x97,		OP_2BYTE|OP_I386,			I386OP(seta_rm8),				I386OP(seta_rm8),			},
	{ 0x98,		OP_2BYTE|OP_I386,			I386OP(sets_rm8),				I386OP(sets_rm8),			},
	{ 0x99,		OP_2BYTE|OP_I386,			I386OP(setns_rm8),				I386OP(setns_rm8),			},
	{ 0x9A,		OP_2BYTE|OP_I386,			I386OP(setp_rm8),				I386OP(setp_rm8),			},
	{ 0x9B,		OP_2BYTE|OP_I386,			I386OP(setnp_rm8),				I386OP(setnp_rm8),			},
	{ 0x9C,		OP_2BYTE|OP_I386,			I386OP(setl_rm8),				I386OP(setl_rm8),			},
	{ 0x9D,		OP_2BYTE|OP_I386,			I386OP(setge_rm8),				I386OP(setge_rm8),			},
	{ 0x9E,		OP_2BYTE|OP_I386,			I386OP(setle_rm8),				I386OP(setle_rm8),			},
	{ 0x9F,		OP_2BYTE|OP_I386,			I386OP(setg_rm8),				I386OP(setg_rm8),			},
	{ 0xA0,		OP_2BYTE|OP_I386,			I386OP(push_fs16),				I386OP(push_fs32),			},
	{ 0xA1,		OP_2BYTE|OP_I386,			I386OP(pop_fs16),				I386OP(pop_fs32),			},
	{ 0xA2,		OP_2BYTE|OP_I486,			I486OP(cpuid),					I486OP(cpuid),				},
	{ 0xA3,		OP_2BYTE|OP_I386,			I386OP(bt_rm16_r16),			I386OP(bt_rm32_r32),		},
	{ 0xA4,		OP_2BYTE|OP_I386,			I386OP(shld16_i8),				I386OP(shld32_i8),			},
	{ 0xA5,		OP_2BYTE|OP_I386,			I386OP(shld16_cl),				I386OP(shld32_cl),			},
	{ 0xA8,		OP_2BYTE|OP_I386,			I386OP(push_gs16),				I386OP(push_gs32),			},
	{ 0xA9,		OP_2BYTE|OP_I386,			I386OP(pop_gs16),				I386OP(pop_gs32),			},
	{ 0xAA,		OP_2BYTE|OP_I386,			I386OP(unimplemented),			I386OP(unimplemented),		},
	{ 0xAB,		OP_2BYTE|OP_I386,			I386OP(bts_rm16_r16),			I386OP(bts_rm32_r32),		},
	{ 0xAC,		OP_2BYTE|OP_I386,			I386OP(shrd16_i8),				I386OP(shrd32_i8),			},
	{ 0xAD,		OP_2BYTE|OP_I386,			I386OP(shrd16_cl),				I386OP(shrd32_cl),			},
	{ 0xAE,		OP_2BYTE|OP_I386,			I386OP(invalid),				I386OP(invalid),			},
	{ 0xAF,		OP_2BYTE|OP_I386,			I386OP(imul_r16_rm16),			I386OP(imul_r32_rm32),		},
	{ 0xB0,		OP_2BYTE|OP_I486,			I486OP(cmpxchg_rm8_r8),			I486OP(cmpxchg_rm8_r8),		},
	{ 0xB1,		OP_2BYTE|OP_I486,			I486OP(cmpxchg_rm16_r16),		I486OP(cmpxchg_rm32_r32),	},
	{ 0xB2,		OP_2BYTE|OP_I386,			I386OP(lss16),					I386OP(lss32),				},
	{ 0xB3,		OP_2BYTE|OP_I386,			I386OP(btr_rm16_r16),			I386OP(btr_rm32_r32),		},
	{ 0xB4,		OP_2BYTE|OP_I386,			I386OP(lfs16),					I386OP(lfs32),				},
	{ 0xB5,		OP_2BYTE|OP_I386,			I386OP(lgs16),					I386OP(lgs32),				},
	{ 0xB6,		OP_2BYTE|OP_I386,			I386OP(movzx_r16_rm8),			I386OP(movzx_r32_rm8),		},
	{ 0xB7,		OP_2BYTE|OP_I386,			I386OP(invalid),				I386OP(movzx_r32_rm16),		},
	{ 0xBA,		OP_2BYTE|OP_I386,			I386OP(group0FBA_16),			I386OP(group0FBA_32),		},
	{ 0xBB,		OP_2BYTE|OP_I386,			I386OP(btc_rm16_r16),			I386OP(btc_rm32_r32),		},
	{ 0xBC,		OP_2BYTE|OP_I386,			I386OP(bsf_r16_rm16),			I386OP(bsf_r32_rm32),		},
	{ 0xBD,		OP_2BYTE|OP_I386,			I386OP(bsr_r16_rm16),			I386OP(bsr_r32_rm32),		},
	{ 0xBE,		OP_2BYTE|OP_I386,			I386OP(movsx_r16_rm8),			I386OP(movsx_r32_rm8),		},
	{ 0xBF,		OP_2BYTE|OP_I386,			I386OP(invalid),				I386OP(movsx_r32_rm16),		},
	{ 0xC0,		OP_2BYTE|OP_I486,			I486OP(xadd_rm8_r8),			I486OP(xadd_rm8_r8),		},
	{ 0xC1,		OP_2BYTE|OP_I486,			I486OP(xadd_rm16_r16),			I486OP(xadd_rm32_r32),		},
	{ 0xC7,		OP_2BYTE|OP_PENTIUM,		PENTIUMOP(cmpxchg8b_m64),		PENTIUMOP(cmpxchg8b_m64),	},
	{ 0xC8,		OP_2BYTE|OP_I486,			I486OP(bswap_eax),				I486OP(bswap_eax),			},
	{ 0xC9,		OP_2BYTE|OP_I486,			I486OP(bswap_ecx),				I486OP(bswap_ecx),			},
	{ 0xCA,		OP_2BYTE|OP_I486,			I486OP(bswap_edx),				I486OP(bswap_edx),			},
	{ 0xCB,		OP_2BYTE|OP_I486,			I486OP(bswap_ebx),				I486OP(bswap_ebx),			},
	{ 0xCC,		OP_2BYTE|OP_I486,			I486OP(bswap_esp),				I486OP(bswap_esp),			},
	{ 0xCD,		OP_2BYTE|OP_I486,			I486OP(bswap_ebp),				I486OP(bswap_ebp),			},
	{ 0xCE,		OP_2BYTE|OP_I486,			I486OP(bswap_esi),				I486OP(bswap_esi),			},
	{ 0xCF,		OP_2BYTE|OP_I486,			I486OP(bswap_edi),				I486OP(bswap_edi),			}
};
