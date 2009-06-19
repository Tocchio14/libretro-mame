typedef struct _hc11_opcode_list_struct hc11_opcode_list_struct;
struct _hc11_opcode_list_struct
{
	int page;
	int opcode;
	void (*handler)(hc11_state *cpustate);
};

static const hc11_opcode_list_struct hc11_opcode_list[] =
{
	/*  page    opcode          handler                     */
	{	0,		0x1b,			HC11OP(aba)					},
	{	0,		0x3a,			HC11OP(abx)					},
	{	0x18,	0x3a,			HC11OP(aby)					},
	{	0,		0x89,			HC11OP(adca_imm)			},
	{	0,		0x99,			HC11OP(adca_dir)			},
	{	0,		0xb9,			HC11OP(adca_ext)			},
	{	0,		0xa9,			HC11OP(adca_indx)			},
	{	0x18,	0xa9,			HC11OP(adca_indy)			},
	{	0,		0xc9,			HC11OP(adcb_imm)			},
	{	0,		0xd9,			HC11OP(adcb_dir)			},
	{	0,		0xf9,			HC11OP(adcb_ext)			},
	{	0,		0xe9,			HC11OP(adcb_indx)			},
	{	0x18,	0xe9,			HC11OP(adcb_indy)			},
	{	0,		0x8b,			HC11OP(adda_imm)			},
	{	0,		0x9b,			HC11OP(adda_dir)			},
	{	0,		0xbb,			HC11OP(adda_ext)			},
	{	0,		0xab,			HC11OP(adda_indx)			},
	{	0x18,	0xab,			HC11OP(adda_indy)			},
	{	0,		0xcb,			HC11OP(addb_imm)			},
	{	0,		0xdb,			HC11OP(addb_dir)			},
	{	0,		0xfb,			HC11OP(addb_ext)			},
	{	0,		0xeb,			HC11OP(addb_indx)			},
	{	0x18,	0xeb,			HC11OP(addb_indy)			},
	{	0,		0xc3,			HC11OP(addd_imm)			},
	{	0,		0xd3,			HC11OP(addd_dir)			},
	{	0,		0xf3,			HC11OP(addd_ext)			},
	{	0,		0xe3,			HC11OP(addd_indx)			},
	{	0x18,	0xe3,			HC11OP(addd_indy)			},
	{	0,		0x84,			HC11OP(anda_imm)			},
	{	0,		0x94,			HC11OP(anda_dir)			},
	{	0,		0xb4,			HC11OP(anda_ext)			},
	{	0,		0xa4,			HC11OP(anda_indx)			},
	{	0x18,	0xa4,			HC11OP(anda_indy)			},
	{	0,		0xc4,			HC11OP(andb_imm)			},
	{	0,		0xd4,			HC11OP(andb_dir)			},
	{	0,		0xf4,			HC11OP(andb_ext)			},
	{	0,		0xe4,			HC11OP(andb_indx)			},
	{	0x18,	0xe4,			HC11OP(andb_indy)			},
	{	0,		0x85,			HC11OP(bita_imm)			},
	{	0,		0x95,			HC11OP(bita_dir)			},
	{	0,		0xb5,			HC11OP(bita_ext)			},
	{	0,		0xa5,			HC11OP(bita_indx)			},
	{	0x18,	0xa5,			HC11OP(bita_indy)			},
	{	0,		0xc5,			HC11OP(bitb_imm)			},
	{	0,		0xd5,			HC11OP(bitb_dir)			},
	{	0,		0xf5,			HC11OP(bitb_ext)			},
	{	0,		0xe5,			HC11OP(bitb_indx)			},
	{	0x18,	0xe5,			HC11OP(bitb_indy)			},
	{	0,		0x24,			HC11OP(bcc)					},
	{	0,		0x25,			HC11OP(bcs)					},
	{	0,		0x27,			HC11OP(beq)					},
	{	0,		0x26,			HC11OP(bne)					},
	{	0,		0x2f,			HC11OP(ble)					},
	{	0,		0x2b,			HC11OP(bmi)					},
	{	0,		0x2a,			HC11OP(bpl)					},
	{	0,		0x20,			HC11OP(bra)					},
	{	0,		0x21,			HC11OP(brn)					},
	{	0,		0x8d,			HC11OP(bsr)					},
	{	0,		0x28,			HC11OP(bvc)					},
	{	0,		0x29,			HC11OP(bvs)					},
	{	0,		0x0e,			HC11OP(cli)					},
	{	0,		0x4f,			HC11OP(clra)				},
	{	0,		0x5f,			HC11OP(clrb)				},
	{	0,		0x7f,			HC11OP(clr_ext)				},
	{	0,		0x6f,			HC11OP(clr_indx)			},
	{	0x18,	0x6f,			HC11OP(clr_indy)			},
	{	0,		0x81,			HC11OP(cmpa_imm)			},
	{	0,		0x91,			HC11OP(cmpa_dir)			},
	{	0,		0xb1,			HC11OP(cmpa_ext)			},
	{	0,		0xa1,			HC11OP(cmpa_indx)			},
	{	0x18,	0x81,			HC11OP(cmpa_indy)			},
	{	0,		0xc1,			HC11OP(cmpb_imm)			},
	{	0,		0xd1,			HC11OP(cmpb_dir)			},
	{	0,		0xf1,			HC11OP(cmpb_ext)			},
	{	0,		0xe1,			HC11OP(cmpb_indx)			},
	{	0x18,	0xe1,			HC11OP(cmpb_indy)			},
	{	0x1a,	0x83,			HC11OP(cpd_imm)				},
	{	0x1a,	0x93,			HC11OP(cpd_dir)				},
	{	0x1a,	0xb3,			HC11OP(cpd_ext)				},
	{	0x1a,	0xa3,			HC11OP(cpd_indx)			},
	{	0xcd,	0xa3,			HC11OP(cpd_indy)			},
	{	0,		0x8c,			HC11OP(cpx_imm)				},
	{	0,		0x9c,			HC11OP(cpx_dir)				},
	{	0,		0xbc,			HC11OP(cpx_ext)				},
	{	0,		0xac,			HC11OP(cpx_indx)			},
	{	0xcd,	0xac,			HC11OP(cpx_indy)			},
	{	0x18,	0x8c,			HC11OP(cpy_imm)				},
	{	0,		0x09,			HC11OP(dex)					},
	{	0x18,	0x09,			HC11OP(dey)					},
	{	0,		0x88,			HC11OP(eora_imm)			},
	{	0,		0x98,			HC11OP(eora_dir)			},
	{	0,		0xb8,			HC11OP(eora_ext)			},
	{	0,		0xa8,			HC11OP(eora_indx)			},
	{	0x18,	0xa8,			HC11OP(eora_indy)			},
	{	0,		0xc8,			HC11OP(eorb_imm)			},
	{	0,		0xd8,			HC11OP(eorb_dir)			},
	{	0,		0xf8,			HC11OP(eorb_ext)			},
	{	0,		0xe8,			HC11OP(eorb_indx)			},
	{	0x18,	0xe8,			HC11OP(eorb_indy)			},
	{	0,		0x4c,			HC11OP(inca)				},
	{	0,		0x08,			HC11OP(inx)					},
	{	0x18,	0x08,			HC11OP(iny)					},
	{	0,		0x7e,			HC11OP(jmp_ext)				},
	{	0,		0x9d,			HC11OP(jsr_dir)				},
	{	0,		0xbd,			HC11OP(jsr_ext)				},
	{	0,		0xad,			HC11OP(jsr_indx)			},
	{	0x18,	0xad,			HC11OP(jsr_indy)			},
	{	0,		0x86,			HC11OP(ldaa_imm)			},
	{	0,		0x96,			HC11OP(ldaa_dir)			},
	{	0,		0xb6,			HC11OP(ldaa_ext)			},
	{	0,		0xa6,			HC11OP(ldaa_indx)			},
	{	0x18,	0xa6,			HC11OP(ldaa_indy)			},
	{	0,		0xc6,			HC11OP(ldab_imm)			},
	{	0,		0xd6,			HC11OP(ldab_dir)			},
	{	0,		0xf6,			HC11OP(ldab_ext)			},
	{	0,		0xe6,			HC11OP(ldab_indx)			},
	{	0x18,	0xe6,			HC11OP(ldab_indy)			},
	{	0,		0xcc,			HC11OP(ldd_imm)				},
	{	0,		0xdc,			HC11OP(ldd_dir)				},
	{	0,		0xfc,			HC11OP(ldd_ext)				},
	{	0,		0xec,			HC11OP(ldd_indx)			},
	{	0x18,	0xec,			HC11OP(ldd_indy)			},
	{	0,		0x8e,			HC11OP(lds_imm)				},
	{	0,		0x9e,			HC11OP(lds_dir)				},
	{	0,		0xbe,			HC11OP(lds_ext)				},
	{	0,		0xae,			HC11OP(lds_indx)			},
	{	0x18,	0xae,			HC11OP(lds_indy)			},
	{	0,		0xce,			HC11OP(ldx_imm)				},
	{	0,		0xde,			HC11OP(ldx_dir)				},
	{	0,		0xfe,			HC11OP(ldx_ext)				},
	{	0,		0xee,			HC11OP(ldx_indx)			},
	{	0xcd,	0xee,			HC11OP(ldx_indy)			},
	{	0x18,	0xce,			HC11OP(ldy_imm)				},
	{	0x18,	0xde,			HC11OP(ldy_dir)				},
	{	0x18,	0xfe,			HC11OP(ldy_ext)				},
	{	0x1a,	0xee,			HC11OP(ldy_indx)			},
	{	0x18,	0xee,			HC11OP(ldy_indy)			},
	{	0,		0x05,			HC11OP(lsld)				},
	{	0,		0x8a,			HC11OP(oraa_imm)			},
	{	0,		0x9a,			HC11OP(oraa_dir)			},
	{	0,		0xba,			HC11OP(oraa_ext)			},
	{	0,		0xaa,			HC11OP(oraa_indx)			},
	{	0x18,	0xaa,			HC11OP(oraa_indy)			},
	{	0,		0xca,			HC11OP(orab_imm)			},
	{	0,		0xda,			HC11OP(orab_dir)			},
	{	0,		0xfa,			HC11OP(orab_ext)			},
	{	0,		0xea,			HC11OP(orab_indx)			},
	{	0x18,	0xea,			HC11OP(orab_indy)			},
	{	0,		0x36,			HC11OP(psha)				},
	{	0,		0x37,			HC11OP(pshb)				},
	{	0,		0x3c,			HC11OP(pshx)				},
	{	0x18,	0x3c,			HC11OP(pshy)				},
	{	0,		0x32,			HC11OP(pula)				},
	{	0,		0x33,			HC11OP(pulb)				},
	{	0,		0x38,			HC11OP(pulx)				},
	{	0x18,	0x38,			HC11OP(puly)				},
	{	0,		0x39,			HC11OP(rts)					},
	{	0,		0x0f,			HC11OP(sei)					},
	{	0,		0x97,			HC11OP(staa_dir)			},
	{	0,		0xb7,			HC11OP(staa_ext)			},
	{	0,		0xa7,			HC11OP(staa_indx)			},
	{	0x18,	0xa7,			HC11OP(staa_indy)			},
	{	0,		0xd7,			HC11OP(stab_dir)			},
	{	0,		0xf7,			HC11OP(stab_ext)			},
	{	0,		0xe7,			HC11OP(stab_indx)			},
	{	0x18,	0xe7,			HC11OP(stab_indy)			},
	{	0,		0xdd,			HC11OP(std_dir)				},
	{	0,		0xfd,			HC11OP(std_ext)				},
	{	0,		0xed,			HC11OP(std_indx)			},
	{	0x18,	0xed,			HC11OP(std_indy)			},
	{	0,		0x16,			HC11OP(tab)					},
	{	0,		0x06,			HC11OP(tap)					},
	{	0,		0x07,			HC11OP(tpa)					},
	{	0,		0x4d,			HC11OP(tsta)				},
	{	0,		0x5d,			HC11OP(tstb)				},
	{	0,		0x7d,			HC11OP(tst_ext)				},
	{	0,		0x6d,			HC11OP(tst_indx)			},
	{	0x18,	0x6d,			HC11OP(tst_indy)			},
	{	0,		0x8f,			HC11OP(xgdx)				},
	{	0x18,	0x8f,			HC11OP(xgdy)				},

	{	0,		0x18,			HC11OP(page2)				},
	{	0,		0x1a,			HC11OP(page3)				},
	{	0,		0xcd,			HC11OP(page4)				},
};
