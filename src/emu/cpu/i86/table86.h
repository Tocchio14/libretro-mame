static void (*const PREFIX86(_instruction)[256])(void) =
{
	 PREFIX86(_add_br8),          /* 0x00 */
	 PREFIX86(_add_wr16),         /* 0x01 */
	 PREFIX86(_add_r8b),          /* 0x02 */
	 PREFIX86(_add_r16w),         /* 0x03 */
	 PREFIX86(_add_ald8),         /* 0x04 */
	 PREFIX86(_add_axd16),        /* 0x05 */
	 PREFIX86(_push_es),          /* 0x06 */
	 PREFIX86(_pop_es),           /* 0x07 */
	 PREFIX86(_or_br8),           /* 0x08 */
	 PREFIX86(_or_wr16),          /* 0x09 */
	 PREFIX86(_or_r8b),           /* 0x0a */
	 PREFIX86(_or_r16w),          /* 0x0b */
	 PREFIX86(_or_ald8),          /* 0x0c */
	 PREFIX86(_or_axd16),         /* 0x0d */
	 PREFIX86(_push_cs),          /* 0x0e */
	 PREFIX86(_invalid),
	 PREFIX86(_adc_br8),          /* 0x10 */
	 PREFIX86(_adc_wr16),         /* 0x11 */
	 PREFIX86(_adc_r8b),          /* 0x12 */
	 PREFIX86(_adc_r16w),         /* 0x13 */
	 PREFIX86(_adc_ald8),         /* 0x14 */
	 PREFIX86(_adc_axd16),        /* 0x15 */
	 PREFIX86(_push_ss),          /* 0x16 */
	 PREFIX86(_pop_ss),           /* 0x17 */
	 PREFIX86(_sbb_br8),          /* 0x18 */
	 PREFIX86(_sbb_wr16),         /* 0x19 */
	 PREFIX86(_sbb_r8b),          /* 0x1a */
	 PREFIX86(_sbb_r16w),         /* 0x1b */
	 PREFIX86(_sbb_ald8),         /* 0x1c */
	 PREFIX86(_sbb_axd16),        /* 0x1d */
	 PREFIX86(_push_ds),          /* 0x1e */
	 PREFIX86(_pop_ds),           /* 0x1f */
	 PREFIX86(_and_br8),          /* 0x20 */
	 PREFIX86(_and_wr16),         /* 0x21 */
	 PREFIX86(_and_r8b),          /* 0x22 */
	 PREFIX86(_and_r16w),         /* 0x23 */
	 PREFIX86(_and_ald8),         /* 0x24 */
	 PREFIX86(_and_axd16),        /* 0x25 */
	 PREFIX86(_es),               /* 0x26 */
	 PREFIX86(_daa),              /* 0x27 */
	 PREFIX86(_sub_br8),          /* 0x28 */
	 PREFIX86(_sub_wr16),         /* 0x29 */
	 PREFIX86(_sub_r8b),          /* 0x2a */
	 PREFIX86(_sub_r16w),         /* 0x2b */
	 PREFIX86(_sub_ald8),         /* 0x2c */
	 PREFIX86(_sub_axd16),        /* 0x2d */
	 PREFIX86(_cs),               /* 0x2e */
	 PREFIX86(_das),		/* 0x2f */
	 PREFIX86(_xor_br8),          /* 0x30 */
	 PREFIX86(_xor_wr16),         /* 0x31 */
	 PREFIX86(_xor_r8b),          /* 0x32 */
	 PREFIX86(_xor_r16w),         /* 0x33 */
	 PREFIX86(_xor_ald8),         /* 0x34 */
	 PREFIX86(_xor_axd16),        /* 0x35 */
	 PREFIX86(_ss),               /* 0x36 */
	 PREFIX86(_aaa),		/* 0x37 */
	 PREFIX86(_cmp_br8),          /* 0x38 */
	 PREFIX86(_cmp_wr16),         /* 0x39 */
	 PREFIX86(_cmp_r8b),          /* 0x3a */
	 PREFIX86(_cmp_r16w),         /* 0x3b */
	 PREFIX86(_cmp_ald8),         /* 0x3c */
	 PREFIX86(_cmp_axd16),        /* 0x3d */
	 PREFIX86(_ds),               /* 0x3e */
	 PREFIX86(_aas),		/* 0x3f */
	 PREFIX86(_inc_ax),           /* 0x40 */
	 PREFIX86(_inc_cx),           /* 0x41 */
	 PREFIX86(_inc_dx),           /* 0x42 */
	 PREFIX86(_inc_bx),           /* 0x43 */
	 PREFIX86(_inc_sp),           /* 0x44 */
	 PREFIX86(_inc_bp),           /* 0x45 */
	 PREFIX86(_inc_si),           /* 0x46 */
	 PREFIX86(_inc_di),           /* 0x47 */
	 PREFIX86(_dec_ax),           /* 0x48 */
	 PREFIX86(_dec_cx),           /* 0x49 */
	 PREFIX86(_dec_dx),           /* 0x4a */
	 PREFIX86(_dec_bx),           /* 0x4b */
	 PREFIX86(_dec_sp),           /* 0x4c */
	 PREFIX86(_dec_bp),           /* 0x4d */
	 PREFIX86(_dec_si),           /* 0x4e */
	 PREFIX86(_dec_di),           /* 0x4f */
	 PREFIX86(_push_ax),          /* 0x50 */
	 PREFIX86(_push_cx),          /* 0x51 */
	 PREFIX86(_push_dx),          /* 0x52 */
	 PREFIX86(_push_bx),          /* 0x53 */
	 PREFIX86(_push_sp),          /* 0x54 */
	 PREFIX86(_push_bp),          /* 0x55 */
	 PREFIX86(_push_si),          /* 0x56 */
	 PREFIX86(_push_di),          /* 0x57 */
	 PREFIX86(_pop_ax),           /* 0x58 */
	 PREFIX86(_pop_cx),           /* 0x59 */
	 PREFIX86(_pop_dx),           /* 0x5a */
	 PREFIX86(_pop_bx),           /* 0x5b */
	 PREFIX86(_pop_sp),           /* 0x5c */
	 PREFIX86(_pop_bp),           /* 0x5d */
	 PREFIX86(_pop_si),           /* 0x5e */
	 PREFIX86(_pop_di),           /* 0x5f */
	 PREFIX86(_invalid), // PREFIX86(_pusha),            /* 0x60 */
	 PREFIX86(_invalid), // PREFIX86(_popa),             /* 0x61 */
	 PREFIX86(_invalid), // PREFIX86(_bound),            /* 0x62 */
	 PREFIX86(_invalid),
	 PREFIX86(_invalid),
	 PREFIX86(_invalid),
	 PREFIX86(_invalid),
	 PREFIX86(_invalid),
	 PREFIX86(_invalid), //i_push_d16),         /* 0x68 */
	 PREFIX86(_invalid), //i_imul_d16),         /* 0x69 */
	 PREFIX86(_invalid), //i_push_d8),          /* 0x6a */
	 PREFIX86(_invalid), //i_imul_d8),          /* 0x6b */
	 PREFIX86(_invalid), //i_insb),             /* 0x6c */
	 PREFIX86(_invalid), //i_insw),             /* 0x6d */
	 PREFIX86(_invalid), //i_outsb),            /* 0x6e */
	 PREFIX86(_invalid), //i_outsw),            /* 0x6f */
	 PREFIX86(_jo),               /* 0x70 */
	 PREFIX86(_jno),              /* 0x71 */
	 PREFIX86(_jb),               /* 0x72 */
	 PREFIX86(_jnb),              /* 0x73 */
	 PREFIX86(_jz),               /* 0x74 */
	 PREFIX86(_jnz),              /* 0x75 */
	 PREFIX86(_jbe),              /* 0x76 */
	 PREFIX86(_jnbe),             /* 0x77 */
	 PREFIX86(_js),               /* 0x78 */
	 PREFIX86(_jns),              /* 0x79 */
	 PREFIX86(_jp),               /* 0x7a */
	 PREFIX86(_jnp),              /* 0x7b */
	 PREFIX86(_jl),               /* 0x7c */
	 PREFIX86(_jnl),              /* 0x7d */
	 PREFIX86(_jle),              /* 0x7e */
	 PREFIX86(_jnle),             /* 0x7f */
	 PREFIX86(_80pre),            /* 0x80 */
	 PREFIX86(_81pre),            /* 0x81 */
	 PREFIX86(_82pre),			/* 0x82 */
	 PREFIX86(_83pre),            /* 0x83 */
	 PREFIX86(_test_br8),         /* 0x84 */
	 PREFIX86(_test_wr16),        /* 0x85 */
	 PREFIX86(_xchg_br8),         /* 0x86 */
	 PREFIX86(_xchg_wr16),        /* 0x87 */
	 PREFIX86(_mov_br8),          /* 0x88 */
	 PREFIX86(_mov_wr16),         /* 0x89 */
	 PREFIX86(_mov_r8b),          /* 0x8a */
	 PREFIX86(_mov_r16w),         /* 0x8b */
	 PREFIX86(_mov_wsreg),        /* 0x8c */
	 PREFIX86(_lea),              /* 0x8d */
	 PREFIX86(_mov_sregw),        /* 0x8e */
	 PREFIX86(_popw),             /* 0x8f */
	 PREFIX86(_nop),              /* 0x90 */
	 PREFIX86(_xchg_axcx),        /* 0x91 */
	 PREFIX86(_xchg_axdx),        /* 0x92 */
	 PREFIX86(_xchg_axbx),        /* 0x93 */
	 PREFIX86(_xchg_axsp),        /* 0x94 */
	 PREFIX86(_xchg_axbp),        /* 0x95 */
	 PREFIX86(_xchg_axsi),        /* 0x97 */
	 PREFIX86(_xchg_axdi),        /* 0x97 */
	 PREFIX86(_cbw),              /* 0x98 */
	 PREFIX86(_cwd),              /* 0x99 */
	 PREFIX86(_call_far),         /* 0x9a */
	 PREFIX86(_wait),             /* 0x9b */
	 PREFIX86(_pushf),            /* 0x9c */
	 PREFIX86(_popf),             /* 0x9d */
	 PREFIX86(_sahf),             /* 0x9e */
	 PREFIX86(_lahf),             /* 0x9f */
	 PREFIX86(_mov_aldisp),       /* 0xa0 */
	 PREFIX86(_mov_axdisp),       /* 0xa1 */
	 PREFIX86(_mov_dispal),       /* 0xa2 */
	 PREFIX86(_mov_dispax),       /* 0xa3 */
	 PREFIX86(_movsb),            /* 0xa4 */
	 PREFIX86(_movsw),            /* 0xa5 */
	 PREFIX86(_cmpsb),            /* 0xa6 */
	 PREFIX86(_cmpsw),            /* 0xa7 */
	 PREFIX86(_test_ald8),        /* 0xa8 */
	 PREFIX86(_test_axd16),       /* 0xa9 */
	 PREFIX86(_stosb),            /* 0xaa */
	 PREFIX86(_stosw),            /* 0xab */
	 PREFIX86(_lodsb),            /* 0xac */
	 PREFIX86(_lodsw),            /* 0xad */
	 PREFIX86(_scasb),            /* 0xae */
	 PREFIX86(_scasw),            /* 0xaf */
	 PREFIX86(_mov_ald8),         /* 0xb0 */
	 PREFIX86(_mov_cld8),         /* 0xb1 */
	 PREFIX86(_mov_dld8),         /* 0xb2 */
	 PREFIX86(_mov_bld8),         /* 0xb3 */
	 PREFIX86(_mov_ahd8),         /* 0xb4 */
	 PREFIX86(_mov_chd8),         /* 0xb5 */
	 PREFIX86(_mov_dhd8),         /* 0xb6 */
	 PREFIX86(_mov_bhd8),         /* 0xb7 */
	 PREFIX86(_mov_axd16),        /* 0xb8 */
	 PREFIX86(_mov_cxd16),        /* 0xb9 */
	 PREFIX86(_mov_dxd16),        /* 0xba */
	 PREFIX86(_mov_bxd16),        /* 0xbb */
	 PREFIX86(_mov_spd16),        /* 0xbc */
	 PREFIX86(_mov_bpd16),        /* 0xbd */
	 PREFIX86(_mov_sid16),        /* 0xbe */
	 PREFIX86(_mov_did16),        /* 0xbf */
	 PREFIX86(_invalid), // PREFIX86(_rotshft_bd8),      /* 0xc0 */
	 PREFIX86(_invalid), // PREFIX86(_rotshft_wd8),      /* 0xc1 */
	 PREFIX86(_ret_d16),          /* 0xc2 */
	 PREFIX86(_ret),              /* 0xc3 */
	 PREFIX86(_les_dw),           /* 0xc4 */
	 PREFIX86(_lds_dw),           /* 0xc5 */
	 PREFIX86(_mov_bd8),          /* 0xc6 */
	 PREFIX86(_mov_wd16),         /* 0xc7 */
	 PREFIX86(_invalid), //i_enter),            /* 0xc8 */
	 PREFIX86(_invalid), //leave),            /* 0xc9 */
	 PREFIX86(_retf_d16),         /* 0xca */
	 PREFIX86(_retf),             /* 0xcb */
	 PREFIX86(_int3),             /* 0xcc */
	 PREFIX86(_int),              /* 0xcd */
	 PREFIX86(_into),             /* 0xce */
	 PREFIX86(_iret),             /* 0xcf */
	 PREFIX86(_rotshft_b),        /* 0xd0 */
	 PREFIX86(_rotshft_w),        /* 0xd1 */
	 PREFIX86(_rotshft_bcl),      /* 0xd2 */
	 PREFIX86(_rotshft_wcl),      /* 0xd3 */
	 PREFIX86(_aam),              /* 0xd4 */
	 PREFIX86(_aad),              /* 0xd5 */
	 PREFIX86(_invalid),
	 PREFIX86(_xlat),             /* 0xd7 */
	 PREFIX86(_escape),           /* 0xd8 */
	 PREFIX86(_escape),           /* 0xd9 */
	 PREFIX86(_escape),           /* 0xda */
	 PREFIX86(_escape),           /* 0xdb */
	 PREFIX86(_escape),           /* 0xdc */
	 PREFIX86(_escape),           /* 0xdd */
	 PREFIX86(_escape),           /* 0xde */
	 PREFIX86(_escape),           /* 0xdf */
	 PREFIX86(_loopne),           /* 0xe0 */
	 PREFIX86(_loope),            /* 0xe1 */
	 PREFIX86(_loop),             /* 0xe2 */
	 PREFIX86(_jcxz),             /* 0xe3 */
	 PREFIX86(_inal),             /* 0xe4 */
	 PREFIX86(_inax),             /* 0xe5 */
	 PREFIX86(_outal),            /* 0xe6 */
	 PREFIX86(_outax),            /* 0xe7 */
	 PREFIX86(_call_d16),         /* 0xe8 */
	 PREFIX86(_jmp_d16),          /* 0xe9 */
	 PREFIX86(_jmp_far),          /* 0xea */
	 PREFIX86(_jmp_d8),           /* 0xeb */
	 PREFIX86(_inaldx),           /* 0xec */
	 PREFIX86(_inaxdx),           /* 0xed */
	 PREFIX86(_outdxal),          /* 0xee */
	 PREFIX86(_outdxax),          /* 0xef */
	 PREFIX86(_lock),             /* 0xf0 */
	 PREFIX86(_invalid),          /* 0xf1 */
	 PREFIX86(_repne),            /* 0xf2 */
	 PREFIX86(_repe),             /* 0xf3 */
	 PREFIX86(_hlt),		/* 0xf4 */
	 PREFIX86(_cmc),              /* 0xf5 */
	 PREFIX86(_f6pre),            /* 0xf6 */
	 PREFIX86(_f7pre),            /* 0xf7 */
	 PREFIX86(_clc),              /* 0xf8 */
	 PREFIX86(_stc),              /* 0xf9 */
	 PREFIX86(_cli),              /* 0xfa */
	 PREFIX86(_sti),              /* 0xfb */
	 PREFIX86(_cld),              /* 0xfc */
	 PREFIX86(_std),              /* 0xfd */
	 PREFIX86(_fepre),            /* 0xfe */
	 PREFIX86(_ffpre)             /* 0xff */
};

#if defined(BIGCASE) && !defined(RS6000)
  /* Some compilers cannot handle large case statements */
#define TABLE86 \
	switch(FETCHOP)\
	{\
	case 0x00:    PREFIX86(_add_br8)(); break;\
	case 0x01:    PREFIX86(_add_wr16)(); break;\
	case 0x02:    PREFIX86(_add_r8b)(); break;\
	case 0x03:    PREFIX86(_add_r16w)(); break;\
	case 0x04:    PREFIX86(_add_ald8)(); break;\
	case 0x05:    PREFIX86(_add_axd16)(); break;\
	case 0x06:    PREFIX86(_push_es)(); break;\
	case 0x07:    PREFIX86(_pop_es)(); break;\
	case 0x08:    PREFIX86(_or_br8)(); break;\
	case 0x09:    PREFIX86(_or_wr16)(); break;\
	case 0x0a:    PREFIX86(_or_r8b)(); break;\
	case 0x0b:    PREFIX86(_or_r16w)(); break;\
	case 0x0c:    PREFIX86(_or_ald8)(); break;\
	case 0x0d:    PREFIX86(_or_axd16)(); break;\
	case 0x0e:    PREFIX86(_push_cs)(); break;\
	case 0x0f:    PREFIX86(_invalid)(); break;\
	case 0x10:    PREFIX86(_adc_br8)(); break;\
	case 0x11:    PREFIX86(_adc_wr16)(); break;\
	case 0x12:    PREFIX86(_adc_r8b)(); break;\
	case 0x13:    PREFIX86(_adc_r16w)(); break;\
	case 0x14:    PREFIX86(_adc_ald8)(); break;\
	case 0x15:    PREFIX86(_adc_axd16)(); break;\
	case 0x16:    PREFIX86(_push_ss)(); break;\
	case 0x17:    PREFIX86(_pop_ss)(); break;\
	case 0x18:    PREFIX86(_sbb_br8)(); break;\
	case 0x19:    PREFIX86(_sbb_wr16)(); break;\
	case 0x1a:    PREFIX86(_sbb_r8b)(); break;\
	case 0x1b:    PREFIX86(_sbb_r16w)(); break;\
	case 0x1c:    PREFIX86(_sbb_ald8)(); break;\
	case 0x1d:    PREFIX86(_sbb_axd16)(); break;\
	case 0x1e:    PREFIX86(_push_ds)(); break;\
	case 0x1f:    PREFIX86(_pop_ds)(); break;\
	case 0x20:    PREFIX86(_and_br8)(); break;\
	case 0x21:    PREFIX86(_and_wr16)(); break;\
	case 0x22:    PREFIX86(_and_r8b)(); break;\
	case 0x23:    PREFIX86(_and_r16w)(); break;\
	case 0x24:    PREFIX86(_and_ald8)(); break;\
	case 0x25:    PREFIX86(_and_axd16)(); break;\
	case 0x26:    PREFIX86(_es)(); break;\
	case 0x27:    PREFIX86(_daa)(); break;\
	case 0x28:    PREFIX86(_sub_br8)(); break;\
	case 0x29:    PREFIX86(_sub_wr16)(); break;\
	case 0x2a:    PREFIX86(_sub_r8b)(); break;\
	case 0x2b:    PREFIX86(_sub_r16w)(); break;\
	case 0x2c:    PREFIX86(_sub_ald8)(); break;\
	case 0x2d:    PREFIX86(_sub_axd16)(); break;\
	case 0x2e:    PREFIX86(_cs)(); break;\
	case 0x2f:    PREFIX86(_das)(); break;\
	case 0x30:    PREFIX86(_xor_br8)(); break;\
	case 0x31:    PREFIX86(_xor_wr16)(); break;\
	case 0x32:    PREFIX86(_xor_r8b)(); break;\
	case 0x33:    PREFIX86(_xor_r16w)(); break;\
	case 0x34:    PREFIX86(_xor_ald8)(); break;\
	case 0x35:    PREFIX86(_xor_axd16)(); break;\
	case 0x36:    PREFIX86(_ss)(); break;\
	case 0x37:    PREFIX86(_aaa)(); break;\
	case 0x38:    PREFIX86(_cmp_br8)(); break;\
	case 0x39:    PREFIX86(_cmp_wr16)(); break;\
	case 0x3a:    PREFIX86(_cmp_r8b)(); break;\
	case 0x3b:    PREFIX86(_cmp_r16w)(); break;\
	case 0x3c:    PREFIX86(_cmp_ald8)(); break;\
	case 0x3d:    PREFIX86(_cmp_axd16)(); break;\
	case 0x3e:    PREFIX86(_ds)(); break;\
	case 0x3f:    PREFIX86(_aas)(); break;\
	case 0x40:    PREFIX86(_inc_ax)(); break;\
	case 0x41:    PREFIX86(_inc_cx)(); break;\
	case 0x42:    PREFIX86(_inc_dx)(); break;\
	case 0x43:    PREFIX86(_inc_bx)(); break;\
	case 0x44:    PREFIX86(_inc_sp)(); break;\
	case 0x45:    PREFIX86(_inc_bp)(); break;\
	case 0x46:    PREFIX86(_inc_si)(); break;\
	case 0x47:    PREFIX86(_inc_di)(); break;\
	case 0x48:    PREFIX86(_dec_ax)(); break;\
	case 0x49:    PREFIX86(_dec_cx)(); break;\
	case 0x4a:    PREFIX86(_dec_dx)(); break;\
	case 0x4b:    PREFIX86(_dec_bx)(); break;\
	case 0x4c:    PREFIX86(_dec_sp)(); break;\
	case 0x4d:    PREFIX86(_dec_bp)(); break;\
	case 0x4e:    PREFIX86(_dec_si)(); break;\
	case 0x4f:    PREFIX86(_dec_di)(); break;\
	case 0x50:    PREFIX86(_push_ax)(); break;\
	case 0x51:    PREFIX86(_push_cx)(); break;\
	case 0x52:    PREFIX86(_push_dx)(); break;\
	case 0x53:    PREFIX86(_push_bx)(); break;\
	case 0x54:    PREFIX86(_push_sp)(); break;\
	case 0x55:    PREFIX86(_push_bp)(); break;\
	case 0x56:    PREFIX86(_push_si)(); break;\
	case 0x57:    PREFIX86(_push_di)(); break;\
	case 0x58:    PREFIX86(_pop_ax)(); break;\
	case 0x59:    PREFIX86(_pop_cx)(); break;\
	case 0x5a:    PREFIX86(_pop_dx)(); break;\
	case 0x5b:    PREFIX86(_pop_bx)(); break;\
	case 0x5c:    PREFIX86(_pop_sp)(); break;\
	case 0x5d:    PREFIX86(_pop_bp)(); break;\
	case 0x5e:    PREFIX86(_pop_si)(); break;\
	case 0x5f:    PREFIX86(_pop_di)(); break;\
		  case 0x60:    PREFIX86(_invalid)(); break;\
		  case 0x61:    PREFIX86(_invalid)(); break;\
		  case 0x62:    PREFIX86(_invalid)(); break;\
	case 0x63:    PREFIX86(_invalid)(); break;\
	case 0x64:    PREFIX86(_invalid)(); break;\
	case 0x65:	  PREFIX86(_invalid)(); break;\
	case 0x66:    PREFIX86(_invalid)(); break;\
	case 0x67:    PREFIX86(_invalid)(); break;\
		  case 0x68:    PREFIX86(_invalid)(); break;\
		  case 0x69:    PREFIX86(_invalid)(); break;\
		  case 0x6a:    PREFIX86(_invalid)(); break;\
		  case 0x6b:    PREFIX86(_invalid)(); break;\
		  case 0x6c:    PREFIX86(_invalid)(); break;\
		  case 0x6d:    PREFIX86(_invalid)(); break;\
		  case 0x6e:    PREFIX86(_invalid)(); break;\
		  case 0x6f:    PREFIX86(_invalid)(); break;\
	case 0x70:    PREFIX86(_jo)(); break;\
	case 0x71:    PREFIX86(_jno)(); break;\
	case 0x72:    PREFIX86(_jb)(); break;\
	case 0x73:    PREFIX86(_jnb)(); break;\
	case 0x74:    PREFIX86(_jz)(); break;\
	case 0x75:    PREFIX86(_jnz)(); break;\
	case 0x76:    PREFIX86(_jbe)(); break;\
	case 0x77:    PREFIX86(_jnbe)(); break;\
	case 0x78:    PREFIX86(_js)(); break;\
	case 0x79:    PREFIX86(_jns)(); break;\
	case 0x7a:    PREFIX86(_jp)(); break;\
	case 0x7b:    PREFIX86(_jnp)(); break;\
	case 0x7c:    PREFIX86(_jl)(); break;\
	case 0x7d:    PREFIX86(_jnl)(); break;\
	case 0x7e:    PREFIX86(_jle)(); break;\
	case 0x7f:    PREFIX86(_jnle)(); break;\
	case 0x80:    PREFIX86(_80pre)(); break;\
	case 0x81:    PREFIX86(_81pre)(); break;\
	case 0x82:	  PREFIX86(_82pre)(); break;\
	case 0x83:    PREFIX86(_83pre)(); break;\
	case 0x84:    PREFIX86(_test_br8)(); break;\
	case 0x85:    PREFIX86(_test_wr16)(); break;\
	case 0x86:    PREFIX86(_xchg_br8)(); break;\
	case 0x87:    PREFIX86(_xchg_wr16)(); break;\
	case 0x88:    PREFIX86(_mov_br8)(); break;\
	case 0x89:    PREFIX86(_mov_wr16)(); break;\
	case 0x8a:    PREFIX86(_mov_r8b)(); break;\
	case 0x8b:    PREFIX86(_mov_r16w)(); break;\
	case 0x8c:    PREFIX86(_mov_wsreg)(); break;\
	case 0x8d:    PREFIX86(_lea)(); break;\
	case 0x8e:    PREFIX86(_mov_sregw)(); break;\
	case 0x8f:    PREFIX86(_popw)(); break;\
	case 0x90:    PREFIX86(_nop)(); break;\
	case 0x91:    PREFIX86(_xchg_axcx)(); break;\
	case 0x92:    PREFIX86(_xchg_axdx)(); break;\
	case 0x93:    PREFIX86(_xchg_axbx)(); break;\
	case 0x94:    PREFIX86(_xchg_axsp)(); break;\
	case 0x95:    PREFIX86(_xchg_axbp)(); break;\
	case 0x96:    PREFIX86(_xchg_axsi)(); break;\
	case 0x97:    PREFIX86(_xchg_axdi)(); break;\
	case 0x98:    PREFIX86(_cbw)(); break;\
	case 0x99:    PREFIX86(_cwd)(); break;\
	case 0x9a:    PREFIX86(_call_far)(); break;\
	case 0x9b:    PREFIX86(_wait)(); break;\
	case 0x9c:    PREFIX86(_pushf)(); break;\
	case 0x9d:    PREFIX86(_popf)(); break;\
	case 0x9e:    PREFIX86(_sahf)(); break;\
	case 0x9f:    PREFIX86(_lahf)(); break;\
	case 0xa0:    PREFIX86(_mov_aldisp)(); break;\
	case 0xa1:    PREFIX86(_mov_axdisp)(); break;\
	case 0xa2:    PREFIX86(_mov_dispal)(); break;\
	case 0xa3:    PREFIX86(_mov_dispax)(); break;\
	case 0xa4:    PREFIX86(_movsb)(); break;\
	case 0xa5:    PREFIX86(_movsw)(); break;\
	case 0xa6:    PREFIX86(_cmpsb)(); break;\
	case 0xa7:    PREFIX86(_cmpsw)(); break;\
	case 0xa8:    PREFIX86(_test_ald8)(); break;\
	case 0xa9:    PREFIX86(_test_axd16)(); break;\
	case 0xaa:    PREFIX86(_stosb)(); break;\
	case 0xab:    PREFIX86(_stosw)(); break;\
	case 0xac:    PREFIX86(_lodsb)(); break;\
	case 0xad:    PREFIX86(_lodsw)(); break;\
	case 0xae:    PREFIX86(_scasb)(); break;\
	case 0xaf:    PREFIX86(_scasw)(); break;\
	case 0xb0:    PREFIX86(_mov_ald8)(); break;\
	case 0xb1:    PREFIX86(_mov_cld8)(); break;\
	case 0xb2:    PREFIX86(_mov_dld8)(); break;\
	case 0xb3:    PREFIX86(_mov_bld8)(); break;\
	case 0xb4:    PREFIX86(_mov_ahd8)(); break;\
	case 0xb5:    PREFIX86(_mov_chd8)(); break;\
	case 0xb6:    PREFIX86(_mov_dhd8)(); break;\
	case 0xb7:    PREFIX86(_mov_bhd8)(); break;\
	case 0xb8:    PREFIX86(_mov_axd16)(); break;\
	case 0xb9:    PREFIX86(_mov_cxd16)(); break;\
	case 0xba:    PREFIX86(_mov_dxd16)(); break;\
	case 0xbb:    PREFIX86(_mov_bxd16)(); break;\
	case 0xbc:    PREFIX86(_mov_spd16)(); break;\
	case 0xbd:    PREFIX86(_mov_bpd16)(); break;\
	case 0xbe:    PREFIX86(_mov_sid16)(); break;\
	case 0xbf:    PREFIX86(_mov_did16)(); break;\
		  case 0xc0:    PREFIX86(_invalid)(); break;\
		  case 0xc1:    PREFIX86(_invalid)(); break;\
	case 0xc2:    PREFIX86(_ret_d16)(); break;\
	case 0xc3:    PREFIX86(_ret)(); break;\
	case 0xc4:    PREFIX86(_les_dw)(); break;\
	case 0xc5:    PREFIX86(_lds_dw)(); break;\
	case 0xc6:    PREFIX86(_mov_bd8)(); break;\
	case 0xc7:    PREFIX86(_mov_wd16)(); break;\
		  case 0xc8:    PREFIX86(_invalid)(); break;\
		  case 0xc9:    PREFIX86(_invalid)(); break;\
	case 0xca:    PREFIX86(_retf_d16)(); break;\
	case 0xcb:    PREFIX86(_retf)(); break;\
	case 0xcc:    PREFIX86(_int3)(); break;\
	case 0xcd:    PREFIX86(_int)(); break;\
	case 0xce:    PREFIX86(_into)(); break;\
	case 0xcf:    PREFIX86(_iret)(); break;\
		  case 0xd0:    PREFIX86(_rotshft_b)(); break;\
		  case 0xd1:    PREFIX86(_rotshft_w)(); break;\
		  case 0xd2:    PREFIX86(_rotshft_bcl)(); break;\
		  case 0xd3:    PREFIX86(_rotshft_wcl)(); break;\
	case 0xd4:    PREFIX86(_aam)(); break;\
	case 0xd5:    PREFIX86(_aad)(); break;\
	case 0xd6:    PREFIX86(_invalid)(); break;\
	case 0xd7:    PREFIX86(_xlat)(); break;\
	case 0xd8:    PREFIX86(_escape)(); break;\
	case 0xd9:    PREFIX86(_escape)(); break;\
	case 0xda:    PREFIX86(_escape)(); break;\
	case 0xdb:    PREFIX86(_escape)(); break;\
	case 0xdc:    PREFIX86(_escape)(); break;\
	case 0xdd:    PREFIX86(_escape)(); break;\
	case 0xde:    PREFIX86(_escape)(); break;\
	case 0xdf:    PREFIX86(_escape)(); break;\
	case 0xe0:    PREFIX86(_loopne)(); break;\
	case 0xe1:    PREFIX86(_loope)(); break;\
	case 0xe2:    PREFIX86(_loop)(); break;\
	case 0xe3:    PREFIX86(_jcxz)(); break;\
	case 0xe4:    PREFIX86(_inal)(); break;\
	case 0xe5:    PREFIX86(_inax)(); break;\
	case 0xe6:    PREFIX86(_outal)(); break;\
	case 0xe7:    PREFIX86(_outax)(); break;\
	case 0xe8:    PREFIX86(_call_d16)(); break;\
	case 0xe9:    PREFIX86(_jmp_d16)(); break;\
	case 0xea:    PREFIX86(_jmp_far)(); break;\
	case 0xeb:    PREFIX86(_jmp_d8)(); break;\
	case 0xec:    PREFIX86(_inaldx)(); break;\
	case 0xed:    PREFIX86(_inaxdx)(); break;\
	case 0xee:    PREFIX86(_outdxal)(); break;\
	case 0xef:    PREFIX86(_outdxax)(); break;\
	case 0xf0:    PREFIX86(_lock)(); break;\
	case 0xf1:    PREFIX86(_invalid)(); break;\
	case 0xf2:    PREFIX86(_repne)(); break;\
	case 0xf3:    PREFIX86(_repe)(); break;\
	case 0xf4:    PREFIX86(_hlt)(); break;\
	case 0xf5:    PREFIX86(_cmc)(); break;\
	case 0xf6:    PREFIX86(_f6pre)(); break;\
	case 0xf7:    PREFIX86(_f7pre)(); break;\
	case 0xf8:    PREFIX86(_clc)(); break;\
	case 0xf9:    PREFIX86(_stc)(); break;\
	case 0xfa:    PREFIX86(_cli)(); break;\
	case 0xfb:    PREFIX86(_sti)(); break;\
	case 0xfc:    PREFIX86(_cld)(); break;\
	case 0xfd:    PREFIX86(_std)(); break;\
	case 0xfe:    PREFIX86(_fepre)(); break;\
	case 0xff:    PREFIX86(_ffpre)(); break;\
	};
#else
#define TABLE86 PREFIX86(_instruction)[FETCHOP]();
#endif
